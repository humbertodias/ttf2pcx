#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFontDatabase>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <vector>
#include <cstdio>
#include <cstring>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , minChar(0x20)
    , maxChar(0x7F)
{
    chars = new Character[65536];
    setupUi();
}

MainWindow::~MainWindow()
{
    delete[] chars;
}

void MainWindow::setupUi()
{
    setWindowTitle("TTF to PCX Converter");
    
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // Font selection group
    QGroupBox *fontGroup = new QGroupBox("Font Selection", this);
    QGridLayout *fontLayout = new QGridLayout(fontGroup);
    
    fontLayout->addWidget(new QLabel("Font:"), 0, 0);
    fontComboBox = new QComboBox(this);
    populateFontList();
    fontLayout->addWidget(fontComboBox, 0, 1);
    
    fontLayout->addWidget(new QLabel("Size:"), 1, 0);
    sizeComboBox = new QComboBox(this);
    sizeComboBox->setEditable(true);
    QStringList sizes = {"8", "9", "10", "11", "12", "14", "16", "18", "20", "24", "28", "32", "36", "48", "72"};
    sizeComboBox->addItems(sizes);
    sizeComboBox->setCurrentText("12");
    fontLayout->addWidget(sizeComboBox, 1, 1);
    
    fontLayout->addWidget(new QLabel("Style:"), 2, 0);
    styleComboBox = new QComboBox(this);
    styleComboBox->addItems({"Regular", "Bold", "Italic", "Bold Italic"});
    fontLayout->addWidget(styleComboBox, 2, 1);
    
    mainLayout->addWidget(fontGroup);
    
    // Preview
    QGroupBox *previewGroup = new QGroupBox("Preview", this);
    QVBoxLayout *previewLayout = new QVBoxLayout(previewGroup);
    previewWidget = new PreviewWidget(this);
    previewLayout->addWidget(previewWidget);
    mainLayout->addWidget(previewGroup);
    
    // Character range group
    QGroupBox *rangeGroup = new QGroupBox("Character Range", this);
    QGridLayout *rangeLayout = new QGridLayout(rangeGroup);
    
    rangeLayout->addWidget(new QLabel("Start:"), 0, 0);
    minCharEdit = new QLineEdit("0x20", this);
    rangeLayout->addWidget(minCharEdit, 0, 1);
    
    rangeLayout->addWidget(new QLabel("End:"), 1, 0);
    maxCharEdit = new QLineEdit("0x7F", this);
    rangeLayout->addWidget(maxCharEdit, 1, 1);
    
    mainLayout->addWidget(rangeGroup);
    
    // Output mode group
    QGroupBox *modeGroup = new QGroupBox("Output Mode", this);
    QVBoxLayout *modeLayout = new QVBoxLayout(modeGroup);
    
    monochromeRadio = new QRadioButton("Monochrome", this);
    monochromeRadio->setChecked(true);
    modeLayout->addWidget(monochromeRadio);
    
    antialiasedRadio = new QRadioButton("Antialiased", this);
    modeLayout->addWidget(antialiasedRadio);
    
    QHBoxLayout *colorLayout = new QHBoxLayout();
    colorLayout->addWidget(new QLabel("Min Color:"));
    minColorSpin = new QSpinBox(this);
    minColorSpin->setRange(1, 254);
    minColorSpin->setValue(1);
    minColorSpin->setEnabled(false);
    colorLayout->addWidget(minColorSpin);
    
    colorLayout->addWidget(new QLabel("Max Color:"));
    maxColorSpin = new QSpinBox(this);
    maxColorSpin->setRange(1, 254);
    maxColorSpin->setValue(254);
    maxColorSpin->setEnabled(false);
    colorLayout->addWidget(maxColorSpin);
    
    modeLayout->addLayout(colorLayout);
    mainLayout->addWidget(modeGroup);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    exportButton = new QPushButton("Export...", this);
    buttonLayout->addWidget(exportButton);
    
    quitButton = new QPushButton("Quit", this);
    buttonLayout->addWidget(quitButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Connect signals
    connect(fontComboBox, &QComboBox::currentTextChanged, this, &MainWindow::onFontChanged);
    connect(sizeComboBox, &QComboBox::currentTextChanged, this, &MainWindow::onFontChanged);
    connect(styleComboBox, &QComboBox::currentTextChanged, this, &MainWindow::onFontChanged);
    connect(exportButton, &QPushButton::clicked, this, &MainWindow::onExport);
    connect(quitButton, &QPushButton::clicked, this, &QMainWindow::close);
    connect(monochromeRadio, &QRadioButton::toggled, this, &MainWindow::onMonochromeToggled);
    connect(antialiasedRadio, &QRadioButton::toggled, this, &MainWindow::onAntialiasedToggled);
    
    // Initial preview update
    onFontChanged();
    
    // Set minimum window size
    setMinimumSize(400, 500);
}

void MainWindow::populateFontList()
{
    QFontDatabase fontDb;
    QStringList families = fontDb.families();
    fontComboBox->addItems(families);
    
    // Try to select Times New Roman by default
    int index = fontComboBox->findText("Times New Roman");
    if (index >= 0) {
        fontComboBox->setCurrentIndex(index);
    }
}

QFont MainWindow::getSelectedFont(bool big)
{
    QString fontFamily = fontComboBox->currentText();
    int size = sizeComboBox->currentText().toInt();
    QString style = styleComboBox->currentText();
    
    if (big) {
        size *= 8;  // 8x for antialiasing
    }
    
    QFont font(fontFamily, size);
    
    if (style.contains("Bold", Qt::CaseInsensitive)) {
        font.setBold(true);
    }
    if (style.contains("Italic", Qt::CaseInsensitive)) {
        font.setItalic(true);
    }
    
    return font;
}

void MainWindow::onFontChanged()
{
    QFont font = getSelectedFont(false);
    previewWidget->setPreviewFont(font);
}

void MainWindow::onMonochromeToggled(bool checked)
{
    if (checked) {
        minColorSpin->setEnabled(false);
        maxColorSpin->setEnabled(false);
    }
}

void MainWindow::onAntialiasedToggled(bool checked)
{
    if (checked) {
        minColorSpin->setEnabled(true);
        maxColorSpin->setEnabled(true);
    }
}

// PCX save functions (adapted from original)
static void iputw(int w, FILE *f)
{
    int b1 = (w & 0xff00) >> 8;
    int b2 = w & 0x00ff;
    putc(b2, f);
    putc(b1, f);
}

static void save_pcx(unsigned char *b, int bw, int bh, FILE *f, bool antialias, int minc, int maxc)
{
    putc(10, f);                // manufacturer
    putc(5, f);                 // version
    putc(1, f);                 // run length encoding
    putc(8, f);                 // 8 bits per pixel
    iputw(0, f);                // xmin
    iputw(0, f);                // ymin
    iputw(bw-1, f);             // xmax
    iputw(bh-1, f);             // ymax
    iputw(320, f);              // HDpi
    iputw(200, f);              // VDpi
    
    for (int c = 0; c < 16; c++) {
        putc(c, f);
        putc(c, f);
        putc(c, f);
    }
    
    putc(0, f);                 // reserved
    putc(1, f);                 // one color plane
    iputw(bw, f);               // bytes per scanline
    iputw(1, f);                // color palette
    iputw(bw, f);               // hscreen size
    iputw(bh, f);               // vscreen size
    
    for (int c = 0; c < 54; c++)
        putc(0, f);
    
    // Write RLE encoded scanlines
    for (int y = 0; y < bh; y++) {
        int runcount = 0;
        unsigned char runchar = 0;
        
        for (int x = 0; x < bw; x++) {
            unsigned char ch = b[x + y * bw];
            
            if (runcount == 0) {
                runcount = 1;
                runchar = ch;
            }
            else {
                if ((ch != runchar) || (runcount >= 0x3f)) {
                    if ((runcount > 1) || ((runchar & 0xC0) == 0xC0))
                        putc(0xC0 | runcount, f);
                    putc(runchar, f);
                    runcount = 1;
                    runchar = ch;
                }
                else {
                    runcount++;
                }
            }
        }
        
        if ((runcount > 1) || ((runchar & 0xC0) == 0xC0))
            putc(0xC0 | runcount, f);
        putc(runchar, f);
    }
    
    putc(12, f);                // 256 color palette flag
    
    // Palette
    putc(255, f); putc(0, f); putc(255, f);
    
    if (antialias) {
        for (int c = 1; c < 255; c++) {
            int d;
            if ((c == minc) && (c == maxc)) {
                putc(128, f); putc(128, f); putc(128, f);
            }
            else if ((c >= minc) && (c <= maxc)) {
                d = (c - minc) * 255 / (maxc - minc);
                putc(d, f); putc(d, f); putc(d, f);
            }
            else if ((c >= maxc) && (c <= minc)) {
                d = (c - maxc) * 255 / (minc - maxc);
                putc(d, f); putc(d, f); putc(d, f);
            }
            else {
                putc(0, f); putc(255, f); putc(255, f);
            }
        }
    }
    else {
        putc(255, f); putc(255, f); putc(255, f);
        for (int c = 2; c < 255; c++) {
            putc(0, f); putc(255, f); putc(255, f);
        }
    }
    
    putc(255, f); putc(255, f); putc(0, f);
}

void MainWindow::saveChars(FILE *f, bool antialias)
{
    int fontSizeCount = maxChar - minChar + 1;
    
    // Find max character dimensions
    int w = 0, h = 0;
    for (int c = 0; c < fontSizeCount; c++) {
        if (chars[c].getWidth() > w)
            w = chars[c].getWidth();
        if (chars[c].getHeight() > h)
            h = chars[c].getHeight();
    }
    
    if (antialias) {
        w /= 8;
        h /= 8;
    }
    
    w = (w + 16) & 0xFFF0;
    h = (h + 16) & 0xFFF0;
    
    int bw = 1 + w * 16;
    int bh = 1 + h * ((fontSizeCount + 15) / 16);
    
    // Use vector for RAII memory management
    std::vector<unsigned char> buffer(bw * bh, 255);
    unsigned char *b = buffer.data();
    
    int scale = antialias ? 8 : 1;
    
    for (int c = 0; c < fontSizeCount; c++) {
        int x = 1 + w * (c & 15);
        int y = 1 + h * (c / 16);
        
        for (int _y = 0; _y < chars[c].getHeight() / scale; _y++) {
            for (int _x = 0; _x < chars[c].getWidth() / scale; _x++) {
                int col;
                
                if (antialias) {
                    col = 0;
                    for (int xx = 0; xx < 8; xx++) {
                        for (int yy = 0; yy < 8; yy++) {
                            if (chars[c].getPixel(_x * 8 + xx, _y * 8 + yy))
                                col++;
                        }
                    }
                    
                    if (col) {
                        int minColor = minColorSpin->value();
                        int maxColor = maxColorSpin->value();
                        col = minColor + (maxColor - minColor) * col / 64;
                    }
                }
                else {
                    col = chars[c].getPixel(_x, _y) ? 1 : 0;
                }
                
                b[(_y + y) * bw + _x + x] = col;
            }
        }
    }
    
    save_pcx(b, bw, bh, f, antialias, minColorSpin->value(), maxColorSpin->value());
}

void MainWindow::onExport()
{
    // Parse character range
    bool ok;
    minChar = minCharEdit->text().toInt(&ok, 0);
    if (!ok || minChar <= 0 || minChar > 65535) {
        QMessageBox::warning(this, "Error", "Starting character must be in range 1-65535");
        return;
    }
    
    maxChar = maxCharEdit->text().toInt(&ok, 0);
    if (!ok || maxChar <= 0 || maxChar > 65535) {
        QMessageBox::warning(this, "Error", "End character must be in range 1-65535");
        return;
    }
    
    if (maxChar < minChar) {
        QMessageBox::warning(this, "Error", "Start character greater than end character");
        return;
    }
    
    // Get filename
    QString fileName = QFileDialog::getSaveFileName(this, "Export Font", "", "PCX Files (*.pcx);;All Files (*)");
    if (fileName.isEmpty()) {
        return;
    }

    // Ensure the file has .pcx extension
    if (!fileName.endsWith(".pcx", Qt::CaseInsensitive)) {
        fileName += ".pcx";
    }
    
    // Set wait cursor
    QApplication::setOverrideCursor(Qt::WaitCursor);
    
    bool antialias = antialiasedRadio->isChecked();
    QFont font = getSelectedFont(antialias);
    
    // Generate characters
    for (int c = minChar; c <= maxChar; c++) {
        chars[c - minChar].get(font, c, antialias);
    }
    
    // Save to file
    FILE *f = fopen(fileName.toLocal8Bit().constData(), "wb");
    if (!f) {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, "Error", "Error opening " + fileName);
        return;
    }
    
    saveChars(f, antialias);
    fclose(f);
    
    QApplication::restoreOverrideCursor();
    QMessageBox::information(this, "Success", "Font exported successfully!");
}

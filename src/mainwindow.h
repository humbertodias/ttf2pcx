#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include "character.h"
#include "previewwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onFontChanged();
    void onExport();
    void onMonochromeToggled(bool checked);
    void onAntialiasedToggled(bool checked);

private:
    void setupUi();
    void populateFontList();
    QFont getSelectedFont(bool big = false);
    void saveChars(FILE *f, bool antialias);
    
    // UI elements
    QComboBox *fontComboBox;
    QComboBox *sizeComboBox;
    QComboBox *styleComboBox;
    PreviewWidget *previewWidget;
    QRadioButton *monochromeRadio;
    QRadioButton *antialiasedRadio;
    QLineEdit *minCharEdit;
    QLineEdit *maxCharEdit;
    QSpinBox *minColorSpin;
    QSpinBox *maxColorSpin;
    QPushButton *exportButton;
    QPushButton *quitButton;
    
    // Data
    Character *chars;
    int minChar;
    int maxChar;
};

#endif // MAINWINDOW_H

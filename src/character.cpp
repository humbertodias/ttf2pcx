#include "character.h"
#include <QPainter>
#include <QFontMetrics>

Character::Character()
    : width(0), height(0)
{
}

Character::~Character()
{
    destroy();
}

void Character::destroy()
{
    width = 0;
    height = 0;
    pixelData.clear();
}

void Character::get(const QFont &font, int charCode, bool antialias)
{
    destroy();

    // Create a QFontMetrics to get character dimensions
    QFontMetrics metrics(font);
    QString ch = QString(QChar(charCode));
    
    QRect boundingRect = metrics.boundingRect(ch);
    int charWidth = boundingRect.width();
    int charHeight = metrics.height();

    // Ensure we have at least 1x1 pixel
    if (charWidth <= 0) charWidth = 1;
    if (charHeight <= 0) charHeight = 1;

    // Adjust for antialiasing (8x8 supersampling)
    if (antialias) {
        charWidth = (charWidth + 7) & ~7;  // Round up to multiple of 8
        charHeight = (charHeight + 7) & ~7;
    }

    // Create an image to render the character
    QImage image(charWidth * 2, charHeight, QImage::Format_Mono);
    image.fill(Qt::black);

    // Render the character
    QPainter painter(&image);
    painter.setFont(font);
    painter.setPen(Qt::white);
    painter.drawText(0, metrics.ascent(), ch);
    painter.end();

    // Store dimensions
    width = image.width();
    height = image.height();

    // Convert to our internal format (packed bits)
    int bytesPerLine = (width + 7) / 8;
    pixelData.resize(bytesPerLine * height, 0);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QRgb pixel = image.pixel(x, y);
            if (qRed(pixel) > 128) {  // Consider as white/set
                int byteIndex = y * bytesPerLine + x / 8;
                int bitIndex = 7 - (x % 8);
                pixelData[byteIndex] |= (1 << bitIndex);
            }
        }
    }

    // Trim empty columns on the right for non-space characters
    if (charCode > ' ') {
        while (width > 1) {
            bool hasPixel = false;
            for (int y = 0; y < height; ++y) {
                if (getPixel(width - 1, y)) {  // Check rightmost column
                    hasPixel = true;
                    break;
                }
            }
            if (hasPixel) break;
            width--;
        }
    }

    if (antialias) {
        width = (width + 7) & ~7;  // Round up to multiple of 8
    }
}

bool Character::getPixel(int x, int y) const
{
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return false;
    }

    int bytesPerLine = (width + 7) / 8;
    int byteIndex = y * bytesPerLine + x / 8;
    int bitIndex = 7 - (x % 8);
    
    if (byteIndex >= static_cast<int>(pixelData.size())) {
        return false;
    }

    return (pixelData[byteIndex] & (1 << bitIndex)) != 0;
}

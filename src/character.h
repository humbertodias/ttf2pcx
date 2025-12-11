#ifndef CHARACTER_H
#define CHARACTER_H

#include <QFont>
#include <QImage>
#include <vector>

class Character
{
public:
    Character();
    ~Character();

    void get(const QFont &font, int charCode, bool antialias);
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    bool getPixel(int x, int y) const;

private:
    int width;
    int height;
    std::vector<unsigned char> pixelData;
    
    void destroy();
};

#endif // CHARACTER_H

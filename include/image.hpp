#ifndef IMAGE_HPP
#define IMAGE_HPP
#include <string>
#include <png.h>
#include "color.hpp"

class Image
{
public:
    virtual void read(std::string path) = 0;
    virtual void write(std::string path) = 0;
    virtual Color* data() = 0;
    virtual unsigned int width() = 0;
    virtual unsigned int height() = 0;

    Color getPixel(unsigned int x, unsigned int y);
    void setPixel(unsigned int x, unsigned int y, Color pixel);
};

class PngImage: public Image
{
public:
    PngImage(unsigned int w, unsigned int h);
    void read(std::string path);
    void write(std::string path);
    unsigned int width();
    unsigned int height();
    Color* data();

private:
    unsigned int m_width, m_height;
    Color* m_pixels;
};
#endif

#include "image.hpp"
#include <iostream>
#include <cmath>

void Image::setPixel(unsigned int x, unsigned int y, Color pixel)
{
    data()[y * width() + x] = pixel;
}

PngImage::PngImage(unsigned int w, unsigned int h): m_width(w), m_height(h)
{
    pixels = new Color[width() * height()];
}

void PngImage::write(std::string path)
{
    FILE* fp = fopen(path.c_str(), "wb");
    if (!fp) {
        std::cerr << "ERROR opening " << path << " for writing" << std::endl;
        return;
    }
    auto png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    // TODO: error handling

    auto info_ptr = png_create_info_struct(png_ptr); // TODO: error handling

    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, info_ptr, width(), height(), 8, 6, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(png_ptr, info_ptr);

    png_bytep* rows = static_cast<png_bytep*>(malloc(height() * sizeof(png_bytep)));
    for (unsigned int y = 0; y < height(); y++) {
        rows[y] = static_cast<png_byte*>(malloc(png_get_rowbytes(png_ptr, info_ptr)));
        for (unsigned int x = 0; x < width(); x++)
        {
            unsigned char* ptr = static_cast<unsigned char*>(&(rows[y][x]));
            png_byte r = static_cast<unsigned char>(floor(pixels[y * width() + x].r * 255.f));
            png_byte g = static_cast<unsigned char>(floor(pixels[y * width() + x].g * 255.f));
            png_byte b = static_cast<unsigned char>(floor(pixels[y * width() + x].b * 255.f));
            png_byte a = static_cast<unsigned char>(floor(pixels[y * width() + x].a * 255.f));
            rows[y][x*4] = r;
            rows[y][x*4+1] = g;
            rows[y][x*4+2] = b;
            rows[y][x*4+3] = a;
        }
    }

    png_write_image(png_ptr, rows);
    png_write_end(png_ptr, NULL);
    for (unsigned int y = 0; y < height(); y++)
    {
        free(rows[y]);
    }
    free(rows);
    fclose(fp);
}

void PngImage::read(std::string path)
{
}

Color* PngImage::data()
{
    return pixels;
}

unsigned int PngImage::width()
{
    return m_width;
}

unsigned int PngImage::height()
{
    return m_height;
}

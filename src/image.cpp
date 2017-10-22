#include "image.hpp"
#include <iostream>
#include <cmath>

Color Image::getPixel(unsigned int x, unsigned int y)
{
    return data()[y * width() + x];
}

void Image::setPixel(unsigned int x, unsigned int y, Color pixel)
{
    data()[y * width() + x] = pixel;
}

PngImage::PngImage(unsigned int w, unsigned int h): m_width(w), m_height(h)
{
    m_pixels = new Color[width() * height()];
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
            png_byte r = static_cast<unsigned char>(floor(getPixel(x, y).r * 255.f));
            png_byte g = static_cast<unsigned char>(floor(getPixel(x, y).g * 255.f));
            png_byte b = static_cast<unsigned char>(floor(getPixel(x, y).b * 255.f));
            png_byte a = static_cast<unsigned char>(floor(getPixel(x, y).a * 255.f));
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
    FILE* fp = fopen(path.c_str(), "rb");
    if (!fp) {
        std::cerr << "ERROR: Failed to open " << path << " for reading" << std::endl;
    }
    auto png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    auto info_ptr = png_create_info_struct(png_ptr);
    png_init_io(png_ptr, fp);
    png_read_info(png_ptr, info_ptr);

    m_width = png_get_image_width(png_ptr, info_ptr);
    m_height = png_get_image_height(png_ptr, info_ptr);

    // assume color_type is 6 and bit_depth is 8 and non-interlaced
    delete m_pixels;
    m_pixels = new Color[m_width * m_height];
    auto rows = static_cast<png_bytep*>(malloc(m_height * sizeof(png_bytep)));
    for (unsigned int y = 0; y < m_height; y++)
    {
        rows[y] = static_cast<png_byte*>(malloc(png_get_rowbytes(png_ptr, info_ptr)));
    }

    png_read_image(png_ptr, rows);

    for (unsigned int y = 0; y < m_height; y++)
    {
        for (unsigned int x = 0; x < m_width; x++)
        {
            float r = rows[y][x*4] / 255.f;
            float g = rows[y][x*4+1] / 255.f;
            float b = rows[y][x*4+2] / 255.f;
            float a = rows[y][x*4+3] / 255.f;

            setPixel(x, y, Color{r, g, b, a});
        }
        free(rows[y]);
    }
    free(rows);
    fclose(fp);
}

Color* PngImage::data()
{
    return m_pixels;
}

unsigned int PngImage::width()
{
    return m_width;
}

unsigned int PngImage::height()
{
    return m_height;
}

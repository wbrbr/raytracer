#include "image.hpp"

int main()
{
    PngImage img(0, 0);
    img.read("in.png");
    /* for (int y = 0; y < 100; y++)
    {
        for (int x = 0; x < 100; x++)
        {
            img.setPixel(x, y, Color{1.f, 0.f, 0.f, 1.f});
        }
    } */
    img.write("out.png");
    return 0;
}

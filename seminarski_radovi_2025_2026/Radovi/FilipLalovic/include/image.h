#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <vector>

struct Pixel {
    unsigned int r, g, b, a;
};

int diff(const Pixel& a, const Pixel& b);

class Image {
public:
    int w, h, ch;
    std::vector<std::vector<Pixel>> img;

    Image(const std::string &path);
    Image(int w_, int h_);

    bool loadImage(const std::string &path);
    bool saveImage(const std::string &path);

    int id(int y, int x) const;

    std::vector<Pixel>& operator[](int i);
    const std::vector<Pixel>& operator[](int i) const;
};

#endif


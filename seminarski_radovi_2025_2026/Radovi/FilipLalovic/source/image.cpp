#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include "image.h"
#include <iostream>

using namespace std;

int diff(const Pixel& a, const Pixel& b) {
    int dr = a.r - b.r;
    int dg = a.g - b.g;
    int db = a.b - b.b;
    return dr*dr + dg*dg + db*db;
}

Image::Image(const string &path) {
    if (!loadImage(path)) {
        cerr << "Loading error, path: " << path << endl;
    }
}

Image::Image(int w_, int h_) : w(w_), h(h_), ch(4),
    img(h, vector<Pixel>(w)) {}

bool Image::loadImage(const string &path) {
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, 4);
    if (!data) return false;

    img.resize(h, vector<Pixel>(w));

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int i = (y * w + x) * 4;
            img[y][x] = {
                data[i],
                data[i + 1],
                data[i + 2],
                data[i + 3]
            };
        }
    }

    stbi_image_free(data);
    return true;
}

bool Image::saveImage(const string &path) {
    if (img.empty() || w == 0 || h == 0)
        return false;

    vector<unsigned char> buffer(w * h * 4);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int i = (y * w + x) * 4;
            buffer[i]     = img[y][x].r;
            buffer[i + 1] = img[y][x].g;
            buffer[i + 2] = img[y][x].b;
            buffer[i + 3] = img[y][x].a;
        }
    }

    int status = stbi_write_png(
        path.c_str(),
        w,
        h,
        4,
        buffer.data(),
        w * 4
    );

    return status;
}

int Image::id(int y, int x) const {
    return w * y + x;
}

vector<Pixel>& Image::operator[](int i) {
    return img[i];
}

const vector<Pixel>& Image::operator[](int i) const {
    return img[i];
}


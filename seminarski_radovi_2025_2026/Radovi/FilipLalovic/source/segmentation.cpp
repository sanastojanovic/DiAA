#include "segmentation.h"
#include <algorithm>
#include <cmath>
#include <queue>
#include <cstdlib>
#include <map>
#include <set>
#include <unordered_map>

using namespace std;

/* ===================== DSU ===================== */

DSU::DSU(int n) : parent(n), size(n, 1), internal(n, 0.0f) {
    for (int i = 0; i < n; i++) parent[i] = i;
}

int DSU::find(int x) {
    if (parent[x] == x) return x;
    return parent[x] = find(parent[x]);
}

void DSU::unite(int a, int b, float w) {
    a = find(a);
    b = find(b);
    if (a == b) return;

    if (size[a] < size[b])
        swap(a, b);

    parent[b] = a;
    size[a] += size[b];
    internal[a] = max({internal[a], internal[b], w});
}

/* ===================== SEGMENTATION ===================== */

Segmentation::Segmentation(const Image &img_) : img(img_) {}

/* ---------- BLUR ---------- */

Image Segmentation::box(const Image& src) {
    Image dst = src;

    int dx[9] = {-1,0,1,-1,0,1,-1,0,1};
    int dy[9] = {-1,-1,-1,0,0,0,1,1,1};

    for (int y = 1; y < src.h - 1; y++)
    for (int x = 1; x < src.w - 1; x++) {
        int r = 0, g = 0, b = 0;
        for (int k = 0; k < 9; k++) {
            Pixel p = src[y + dy[k]][x + dx[k]];
            r += p.r;
            g += p.g;
            b += p.b;
        }
        dst[y][x] = {
            (unsigned char)(r / 9),
            (unsigned char)(g / 9),
            (unsigned char)(b / 9),
            255
        };
    }

    return dst;
}

Image Segmentation::gauss(const Image& src) {
    Image dst = src;

    const int kernel[3][3] = {
        {1, 2, 1},
        {2, 4, 2},
        {1, 2, 1}
    };

    const int sum = 16;

    for (int y = 1; y < src.h - 1; y++) {
        for (int x = 1; x < src.w - 1; x++) {

            int r = 0, g = 0, b = 0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    Pixel p = src[y + ky][x + kx];
                    int w = kernel[ky + 1][kx + 1];

                    r += p.r * w;
                    g += p.g * w;
                    b += p.b * w;
                }
            }

            dst[y][x] = {
                static_cast<unsigned char>(r / sum),
                static_cast<unsigned char>(g / sum),
                static_cast<unsigned char>(b / sum),
                255
            };
        }
    }

    return dst;
}

/* ---------- FELZENSZWALB ---------- */

vector<int> Segmentation::felzenszwalb(float k) {

    int w = img.w, h = img.h;
    int n = w * h;

    Image blurred = box(img);

    vector<Edge> edges;

    for (int y = 0; y < h; y++)
    for (int x = 0; x < w; x++) {

        if (x + 1 < w)
            edges.push_back({ img.id(y,x), img.id(y,x+1),
                              diff(blurred[y][x], blurred[y][x+1]) });

        if (y + 1 < h)
            edges.push_back({ img.id(y,x), img.id(y+1,x),
                              diff(blurred[y][x], blurred[y+1][x]) });

        if (x + 1 < w && y + 1 < h)
            edges.push_back({ img.id(y,x), img.id(y+1,x+1),
                              diff(blurred[y][x], blurred[y+1][x+1]) });

        if (x + 1 < w && y - 1 >= 0)
            edges.push_back({ img.id(y,x), img.id(y-1,x+1),
                              diff(blurred[y][x], blurred[y-1][x+1]) });
    }

    sort(edges.begin(), edges.end());
    DSU dsu(n);

    for (const auto& e : edges) {
        int u = dsu.find(e.u);
        int v = dsu.find(e.v);
        if (u == v) continue;

        float t1 = dsu.internal[u] + k / dsu.size[u];
        float t2 = dsu.internal[v] + k / dsu.size[v];

        if (e.w <= min(t1, t2))
            dsu.unite(u, v, e.w);
    }

    int min_size = 15;

    for (const auto& e : edges) {
        int u = dsu.find(e.u);
        int v = dsu.find(e.v);
        if (u != v)
            if (dsu.size[u] < min_size || dsu.size[v] < min_size)
                dsu.unite(u, v, e.w);
    }

    vector<int> label(n);
    for (int i = 0; i < n; i++)
        label[i] = dsu.find(i);

    return label;
}

vector<int> Segmentation::backgroundExtraction(float k, int tolerance, int minRegionSize) {
    int w = img.w, h = img.h;
    int n = w * h;

    vector<int> label = felzenszwalb(k);

    int dx[4] = {1, 0,-1, 0};
    int dy[4] = {0, 1, 0,-1};

    struct Info{
        int border = 0;
        Pixel p{0, 0, 0, 255};
        int cnt = 0;
        set<int> nb;
    };

    map<int, Info> info;

    for(int y = 0; y < h; y++)
    for(int x = 0; x < w; x++){
        int comp1 = label[y * w + x];
        Info &s = info[comp1];

        Pixel p = img[y][x];
        s.p.r += p.r; s.p.g += p.g; s.p.b += p.b; s.cnt++;

        if(x == 0 || y == 0 || 
            x == w - 1 || y == h-1)
            s.border++;

        for(int k = 0; k < 4; k++){
            int nx = x + dx[k], ny = y + dy[k];
            if(nx < 0 || ny < 0 || nx >= w || ny >= h) continue;
            int comp2 = label[ny * w + nx];
            if(comp1 != comp2){
                info[comp1].nb.insert(comp2);
                info[comp2].nb.insert(comp1);
            }
        }
    }

    for(auto &[comp, s] : info){
        s.p.r /= s.cnt; s.p.g /= s.cnt; s.p.b /= s.cnt;
    }

    int mainBG =- 1, maxBorder =- 1;
    for(auto &[comp, s] : info){
        if(s.border > maxBorder){
            maxBorder = s.border;
            mainBG = comp;
        }
    }

    set<int> background;
    queue<int> q;

    background.insert(mainBG);
    q.push(mainBG);

    while(!q.empty()){
        int comp1 = q.front(); q.pop();
        for(int comp2 : info[comp1].nb){
            if(background.count(comp2)) continue;

            if(diff(info[comp1].p, info[comp2].p) < tolerance){
                background.insert(comp2);
                q.push(comp2);
            }
        }
    }

    vector<int> mask(n, 0);
    for(int i=0; i < n; i++)
        if(!background.count(label[i]))
            mask[i] = 1;

    vector<int> visited(n, 0);
    int dx8[4] = {1, 0, -1, 0};
    int dy8[4] = {0, 1, 0, -1};

    for (int i = 0; i < n; i++) {
        if (mask[i] == 0 || visited[i]) continue;

        vector<int> component;
        queue<int> q2;
        q2.push(i);
        visited[i] = 1;

        while (!q2.empty()) {
            int cur = q2.front();
            q2.pop();
            component.push_back(cur);

            int y = cur / w;
            int x = cur % w;
            for (int k = 0; k < 4; k++) {
                int nx = x + dx8[k], ny = y + dy8[k];
                if (nx < 0 || ny < 0 || nx >= w || ny >= h) continue;
                int nxt = ny * w + nx;
                if (mask[nxt] == 1 && !visited[nxt]) {
                    visited[nxt] = 1;
                    q2.push(nxt);
                }
            }
        }

        if ((int)component.size() < minRegionSize) {
            for (int idx : component) mask[idx] = 0;
        }
    }

    return mask;
}


/* ---------- VISUALIZE ---------- */

Image Segmentation::visualize(const vector<int>& label) {
    unordered_map<int, Pixel> color;
    Image outImage(img.w, img.h);

    for (int y = 0; y < img.h; y++)
    for (int x = 0; x < img.w; x++) {
        int l = label[y * img.w + x];
        if (!color.count(l))
            color[l] = {(unsigned char)(rand() % 256),
                        (unsigned char)(rand() % 256),
                        (unsigned char)(rand() % 256), 255};
        outImage[y][x] = color[l];
    }
    return outImage;
}

Image Segmentation::visualizeBlend(const vector<int>& label, float alpha) {
    int w = img.w, h = img.h;
    Image out(w, h);

    unordered_map<int, Pixel> color;

    for (int y = 0; y < h; y++)
    for (int x = 0; x < w; x++) {
        int l = label[y * w + x];
        if (!color.count(l)) {
            color[l] = {
                (unsigned char)(rand() % 256),
                (unsigned char)(rand() % 256),
                (unsigned char)(rand() % 256),
                255
            };
        }

        Pixel o = img[y][x];
        Pixel s = color[l];

        out[y][x] = {
            (unsigned char)((1 - alpha) * o.r + alpha * s.r),
            (unsigned char)((1 - alpha) * o.g + alpha * s.g),
            (unsigned char)((1 - alpha) * o.b + alpha * s.b),
            255
        };
    }

    return out;
}

Image Segmentation::visualizeBinary(const vector<int>& label) {
    Image out(img.w, img.h);

    for(int y = 0; y < img.h; y++)
    for(int x = 0; x < img.w; x++){
        if(label[y * img.w + x])
            out[y][x] = {255, 255, 255, 255};
        else
            out[y][x]={0, 0, 0, 255};
    }

    return out;
}

Image Segmentation::visualizeForegroundOnBlack(const vector<int>& mask) {
    Image out(img.w, img.h);

    for (int y = 0; y < img.h; y++)
    for (int x = 0; x < img.w; x++) {
        if (mask[y * img.w + x]) {
            out[y][x] = img[y][x];
        } else {
            out[y][x] = {0, 0, 0, 255};
        }
    }

    return out;
}

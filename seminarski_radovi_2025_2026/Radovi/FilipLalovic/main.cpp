#include "segmentation.h"
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>

using namespace std;
namespace fs = filesystem;

int main() {
    srand(time(nullptr));

    // Parametri za varijaciju
    const vector<float> K_VALUES = {30000, 100000, 200000};
    const vector<int> TOL_VALUES = {500, 900, 1200};

    string imagesDir = "images";
    string resultsBaseDir = "results";

    // Kreiraj results folder ako ne postoji
    if (!fs::exists(resultsBaseDir)) {
        fs::create_directory(resultsBaseDir);
    }

    // Prođi kroz sve slike u images folderu
    for (const auto& entry : fs::directory_iterator(imagesDir)) {
        if (entry.is_regular_file()) {
            string imagePath = entry.path().string();
            string imageName = entry.path().stem().string();
            
            // Preskoči ako nije slika
            string ext = entry.path().extension().string();
            if (ext != ".png" && ext != ".jpg" && ext != ".jpeg" && ext != ".bmp") {
                continue;
            }

            cout << "\n========================================" << endl;
            cout << "Processing image: " << imageName << endl;
            cout << "========================================" << endl;

            Image img(imagePath);

            if (img.w == 0 || img.h == 0) {
                cerr << "Error: Cannot open image: " << imagePath << endl;
                continue;
            }

            Segmentation seg(img);

            // Za svaku kombinaciju k i TOL
            for (float k : K_VALUES) {
                for (int tol : TOL_VALUES) {
                    string folderName = imageName + "_k" + to_string((int)k) + "_tol" + to_string(tol);
                    string outputDir = resultsBaseDir + "/" + folderName;

                    // Kreiraj folder
                    if (!fs::exists(outputDir)) {
                        fs::create_directories(outputDir);
                    }

                    cout << "  k=" << (int)k << ", tol=" << tol << "... ";

                    // Segmentation
                    auto segments = seg.felzenszwalb(k);

                    // Sačuvaj vizuelizacije
                    seg.visualize(segments).saveImage(outputDir + "/random.png");
                    seg.visualizeBlend(segments, 0.7f).saveImage(outputDir + "/blend.png");

                    // Background extraction sa istim k i razlicitim TOL
                    auto mask = seg.backgroundExtraction(k, tol);
                    seg.visualizeBinary(mask).saveImage(outputDir + "/mask.png");
                    seg.visualizeForegroundOnBlack(mask).saveImage(outputDir + "/foreground.png");

                    cout << "Done" << endl;
                }
            }
        }
    }

    cout << "\n========================================" << endl;
    cout << "All processing completed!" << endl;
    cout << "========================================" << endl;
    cout << "\nGenerated combinations:" << endl;
    for (float k : K_VALUES) {
        for (int tol : TOL_VALUES) {
            cout << "  k=" << (int)k << ", tol=" << tol << endl;
        }
    }

    return 0;
}

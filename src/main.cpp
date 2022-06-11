
#include "color.h"
#include "vec3.h"
#include <iostream>

int main() {
    const int imgWidth = 256;
    const int imgHeight = 256;

    std::cout << "P3\n" << imgWidth << ' ' << imgHeight << "\n255\n";

    for (int i = imgHeight - 1; i >= 0; i--) {
        std::cerr << "\rScan lines remaining: " << i << ' ';
        for (int j = 0; j < imgWidth; j++) {
            color pixel_color = color(double(j) / (imgHeight - 1),
                                      double(i) / (imgWidth - 1), 0.25);
            write_color(std::cout, pixel_color);
        }
    }

    std::cerr << "\nDone\n";
}
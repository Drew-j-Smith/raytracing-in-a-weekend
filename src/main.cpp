
#include <iostream>

int main() {
    const int imgWidth = 256;
    const int imgHeight = 256;

    std::cout << "P3\n" << imgWidth << ' ' << imgHeight << "\n255\n";

    for (int i = imgHeight - 1; i >= 0; i--) {
        std::cerr << "\rScan lines remaining: " << i << ' ';
        for (int j = 0; j < imgWidth; j++) {
            auto r = double(j) / (imgHeight - 1);
            auto g = double(i) / (imgWidth - 1);
            auto b = 0.25;

            int ir = static_cast<int>(255.999 * r);
            int ig = static_cast<int>(255.999 * g);
            int ib = static_cast<int>(255.999 * b);

            std::cout << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }

    std::cerr << "\nDone\n";
}
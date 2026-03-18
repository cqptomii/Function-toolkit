#ifndef SAVE_HPP
#define SAVE_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <limits>
#include <algorithm>
#include <cmath>

bool save_pgm(const std::string& path, double** map, int width, int height) {
    double min_h = std::numeric_limits<double>::max();
    double max_h = std::numeric_limits<double>::lowest();

    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            min_h = std::min(min_h, map[x][y]);
            max_h = std::max(max_h, map[x][y]);
        }
    }

    const double range = (max_h - min_h == 0.0) ? 1.0 : (max_h - min_h);

    std::ofstream out(path);
    if (!out.is_open()) return false;

    out << "P2\n";
    out << width << " " << height << "\n";
    out << 255 << "\n";

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double n = (map[x][y] - min_h) / range;
            n = std::pow(n, 1.3);
            int gray = static_cast<int>(std::clamp(n, 0.0, 1.0) * 255.0);
            out << gray << " ";
        }
        out << "\n";
    }

    return true;
}

#endif //SAVE_HPP
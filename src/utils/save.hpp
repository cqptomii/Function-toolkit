#ifndef SAVE_HPP
#define SAVE_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <limits>
#include <algorithm>
#include <array>
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

bool save_biome_ppm(const std::string& path, int** biome_map, int width, int height) {
    static constexpr std::array<std::array<int, 3>, 12> palette = {{
        {10, 35, 92},    // 0 deep ocean
        {36, 89, 166},   // 1 shallow ocean
        {214, 198, 141}, // 2 beach
        {210, 185, 75},  // 3 desert
        {130, 176, 78},  // 4 grassland
        {36, 114, 62},   // 5 forest/rainforest
        {174, 148, 64},  // 6 savanna
        {66, 118, 92},   // 7 taiga
        {132, 139, 145}, // 8 tundra
        {109, 105, 96},  // 9 alpine rock
        {240, 240, 240}, // 10 snow
        {173, 163, 109}  // 11 steppe
    }};

    std::ofstream out(path);
    if (!out.is_open()) return false;

    out << "P3\n";
    out << width << " " << height << "\n";
    out << 255 << "\n";

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int biome = biome_map[x][y];
            biome = std::clamp(biome, 0, static_cast<int>(palette.size()) - 1);
            const auto& rgb = palette[biome];
            out << rgb[0] << " " << rgb[1] << " " << rgb[2] << " ";
        }
        out << "\n";
    }

    return true;
}

#endif //SAVE_HPP
#include <iostream>
#include "src/utils/save.hpp"
#include "src/landscape.hpp"

int main() {
     Landscape landscape(NoiseType::Perlin, 512);

    const int w = 2048, h = 2048;
    double** hm = landscape.generate_landscape(w, h);
    int** biome = landscape.generate_biome_map(w, h, hm);
 
    // Save the height map as a PGM image
    if (save_pgm("heightmap.pgm", hm, w, h)) {
        std::cout << "Height map saved successfully as heightmap.pgm" << std::endl;
    } else {
        std::cerr << "Failed to save height map." << std::endl;
    }

    if (save_biome_ppm("biome_map.ppm", biome, w, h)) {
        std::cout << "Biome map saved successfully as biome_map.ppm" << std::endl;
    } else {
        std::cerr << "Failed to save biome map." << std::endl;
    }

    // Clean up the height map memory
    for (int i = 0; i < w; i++) {
        delete[] hm[i];
        delete[] biome[i];
    }
    delete[] hm;
    delete[] biome;



    return 0;
}

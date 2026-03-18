#include <iostream>
#include "src/utils/save.hpp"
#include "src/terrain.hpp"

int main() {
     Landscape landscape(NoiseType::Perlin, 512);

    const int w = 512, h = 512;
    double** hm = landscape.noise_map(w, h);

    // Save the height map as a PGM image
    if (save_pgm("heightmap.pgm", hm, w, h)) {
        std::cout << "Height map saved successfully as heightmap.pgm" << std::endl;
    } else {
        std::cerr << "Failed to save height map." << std::endl;
    }

    // Clean up the height map memory
    for (int i = 0; i < w; i++) {
        delete[] hm[i];
    }
    delete[] hm;



    return 0;
}

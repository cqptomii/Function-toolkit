#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include "noise/noise.hpp"
#include "noise/perlin.hpp"
#include "utils/Math_toolkit.hpp"

struct Vec2 {
    double x;
    double y;
};

class Landscape {

    int octaves = 6;
    double amplitude = 1.0;
    double frequency = 1.0;
    double persistence = 0.5;
    double lacunarity = 2.00;

    Noise* continent_noise = nullptr;
    Noise* erosion_noise = nullptr;
    Noise* peaks_noise = nullptr;
    Noise* detail_noise = nullptr;
    Noise* river_noise = nullptr;

    double scale = 0.01;
    double max_height = 255.0;
    
    double spline(double x){
        if (x < 0.3) return 0.0;         // océan
        if (x < 0.5) return (x - 0.3) * 50; // plage
        if (x < 0.7) return 10 + (x - 0.5) * 100; // collines
        return 30 + (x - 0.7) * 300;     // montagnes
    }

public:
    Landscape(NoiseType type, unsigned int seed){
        switch (type) {
            case NoiseType::Perlin:
                continent_noise = new PerlinNoise(seed);
                erosion_noise = new PerlinNoise(seed + 1);
                peaks_noise = new PerlinNoise(seed + 2);
                detail_noise = new PerlinNoise(seed + 3);
                river_noise = new PerlinNoise(seed + 4);
                break;
            // Add cases for other noise types as needed
            default:
                continent_noise = new PerlinNoise(seed); // Default to Perlin
                erosion_noise = new PerlinNoise(seed + 1);
                peaks_noise = new PerlinNoise(seed + 2);
                detail_noise = new PerlinNoise(seed + 3);
                river_noise = new PerlinNoise(seed + 4);
                break;
        }
    }
    ~Landscape(){
        delete continent_noise;
        delete erosion_noise;
        delete peaks_noise;
        delete detail_noise;
        delete river_noise;
    };
    double **generate_landscape(int width, int height){
        double **map = new double*[width];
        for (int i = 0; i < width; i++) {
            map[i] = new double[height];
            for (int j = 0; j < height; j++) {
                
                double x = i * scale;
                double y = j * scale;

                // --- Continents ---
                double continent = this->continent_noise->warping(x * 0.15, y * 0.15, 20.0);
                continent = (continent + 1.0) * 0.5;

                double base_height = this->spline(continent);

                // --- erosion ---
                double erosion = this->erosion_noise->fbm(x * 0.1, y * 0.1, 100.);
                erosion = (erosion + 1.0) * 0.5;

                // -- Base terrain ---
                double terrain = erosion_noise->fbm(x * 0.3, y * 0.3, 20.0);
                terrain = (terrain + 1.0) * 0.5;
                
                // --- mountains ---
                double mountains = this->peaks_noise->fbm(x * 0.5, y * 0.5, 500.);
                mountains = pow(mountains, 2.0);
                double ridged = 1.0 - abs(mountains);
                ridged *= ridged;

                // --- detail ---
                double detail = this->detail_noise->fbm(x * 0.2, y * 0.2, 1000.0);
                detail = (detail + 1.0) * 0.5;

                // -- river ---
                double river_noise = this->river_noise->fbm(x * 0.05, y * 0.05, 2000.0);
                double river = abs(river_noise);
                double river_mask = 1.0 - Math_toolkit::smooth_step(0.0, 0.5, river);
                
                double height = terrain * 50;
                height *= Math_toolkit::lerp(0.3, 1.0, erosion);
                height += mountains * 100;
                height += detail * 10;

                map[i][j] = base_height;
            }
        }

        return map;
    }

    double **noise_map(int width, int height){
        double **map = new double*[width];
        for (int i = 0; i < width; i++) {
            map[i] = new double[height];
            for (int j = 0; j < height; j++) {
                
                double x = i * scale;
                double y = j * scale;

                double h = this->continent_noise->fbm(x, y,0.0);
                h = (h + 1.0) * 0.5;
                map[i][j] = h * max_height;
            }
        }
        return map;
    }
    double **height_map(int width, int height){
        double **map = new double*[width];
        for (int i = 0; i < width; i++) {
            map[i] = new double[height];
           
            for (int j = 0; j < height; j++) {
                
                double x = i * scale;
                double y = j * scale;

                // --- Continents ---
                double continent = this->continent_noise->warping(x * 0.1, y * 0.1, 3.45);
                continent = (continent + 1.0) * 0.5;
                continent = pow(continent, 1.5);

                // --- Mask terre ---
                double mask = Math_toolkit::smooth_step(0.3, 0.7, continent);

                // --- Relief ---
                double terrain = this->erosion_noise->fbm(x * 0.3, y * 0.3, 1.2456);
                terrain = (terrain + 1.0) * 0.5;

                // --- Montagnes ---
                double mountains = this->peaks_noise->fbm(x * 0.05, y * 0.05, 0.0);
                mountains = (mountains + 1.0) * 0.5;
                mountains = pow(mountains, 3.0);

                // Full-map style: preserve landform everywhere and use continent as modulation.
                double height = (terrain * 0.75 + mountains * 0.9) * Math_toolkit::lerp(0.45, 1.0, mask);
                height = std::clamp(height, 0.0, 1.0);

                map[i][j] = height * max_height;
            }
        }
        return map;
    }
};

#endif //TERRAIN_HPP
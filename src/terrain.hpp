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

    Noise* noise_generator = nullptr;
    double scale = 0.01;
    double max_height = 255.0;
    double fbm(double x, double y, double z){
        double total = 0.0;
        double max_value = 0.0;
        double amp = amplitude;
        double freq = frequency;

        for (int i = 0; i < octaves; i++) {
            // Add the noise value for the current octave to the total
            total += this->noise_generator->noise(x * freq, y * freq, z * freq) * amp;

            // Keep track of the maximum possible value for normalization
            max_value += amp;
            amp *= persistence;
            freq *= lacunarity;
        }

        return total / max_value;
    }
public:
    Landscape(NoiseType type, unsigned int seed){
        switch (type) {
            case NoiseType::Perlin:
                noise_generator = new PerlinNoise(seed);
                break;
            // Add cases for other noise types as needed
            default:
                noise_generator = new PerlinNoise(seed); // Default to Perlin
                break;
        }
    }
    ~Landscape(){
        delete noise_generator;
    };

    double **noise_map(int width, int height){
        double **map = new double*[width];
        for (int i = 0; i < width; i++) {
            map[i] = new double[height];
            for (int j = 0; j < height; j++) {
                
                double x = i * scale;
                double y = j * scale;

                double h = this->fbm(x, y,0.0);
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

                Vec2 q = {
                    fbm(x * 0.001, y * 0.001, 0.0),
                    fbm(x * 0.001 + 5.2, y * 0.001 + 1.3, 0.0)
                };

                // --- Continents ---
                double continent = fbm(
                    x * 0.002 + 4.0 * q.x,
                    y * 0.002 + 4.0 * q.y,
                    0.0
                );
                continent = (continent + 1.0) * 0.5;

                // --- Mask terre ---
                double mask = std::clamp((continent - 0.5) * 5.0, 0.0, 1.0);

                // --- Relief ---
                double terrain = fbm(x * 0.02, y * 0.02, 0.0);
                terrain = (terrain + 1.0) * 0.5;

                // --- Montagnes ---
                double mountains = fbm(x * 0.01, y * 0.01, 0.0);
                mountains = pow(mountains, 3.0);

                double height = terrain + mountains;

                map[i][j] = terrain * mask * max_height;
            }
        }
        return map;
    }
};

#endif //TERRAIN_HPP
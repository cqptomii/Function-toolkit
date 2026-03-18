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

        return total / max_value; // Normalize the result to the range [-1, 1]
    }

    double warping(double x, double y, double z){
        double warp_amp = 0.5;
        double warp_freq = 0.1;

        double warp_x = fbm( x, y, z);
        double warp_y = fbm( x + 5.2, y + 1.3, z + 2.7 );

        return this->fbm(x + 4.0*warp_x, y + 4.0*warp_y, z);
    }

    double warping2(double x, double y, double z){
        double warp_amp = 0.5;
        double warp_freq = 0.1;

        double q_warp_x = fbm( x, y, z);
        double q_warp_y = fbm( x + 5.2, y + 1.3, z + 2.7 );

        double r_warp_x = fbm( x + 4.0*q_warp_x + 1.7, y + 4.0*q_warp_y + 9.2, z);
        double r_warp_y = fbm( x + 4.0*q_warp_x + 8.3, y + 4.0*q_warp_y + 2.8, z);
        
        return this->fbm(x + 4.0*r_warp_x, y + 4.0*r_warp_y, z);
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

                // --- Continents ---
                double continent = warping(x * 0.05, y * 0.05, 3.45);
                continent = (continent + 1.0) * 0.5;
                continent = pow(continent, 1.5);

                // --- Mask terre ---
                double mask = Math_toolkit::smooth_step(0.3, 0.7, continent);

                // --- Relief ---
                double terrain = fbm(x * 0.3, y * 0.3, 1.2456);
                terrain = (terrain + 1.0) * 0.5;

                // --- Montagnes ---
                double height = terrain * mask;

                map[i][j] = height * max_height;
            }
        }
        return map;
    }
};

#endif //TERRAIN_HPP
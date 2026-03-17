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

    int octaves = 4;
    double amplitude = 1.0;
    double frequency = 1.0;
    double persistence = 0.5;
    double lacunarity = 2.0;

    Noise* noise_generator = nullptr;
    double scale = 0.1;
    double max_height = 100.0;
    double fbm(double x, double y, double z){
        double total = 0.0;
        double max_value = 0.0;

        for (int i = 0; i < octaves; i++) {
            // Add the noise value for the current octave to the total
            total += this->noise_generator->noise(x * frequency, y * frequency, z * frequency) * amplitude;

            // Keep track of the maximum possible value for normalization
            max_value += amplitude;
            amplitude *= persistence;
            frequency *= lacunarity;
        }

        return total / max_value;
    }
    Vec2 fbm2(double x, double y){
       return {
            fbm(x, y, 0.0),
            fbm(x + 5.2, y + 1.3, 0.0)
        };
    }
    double fbm_warping(double x, double y, double z){        
        Vec2 q = fbm2(x, y);
   
        Vec2 r = {
            fbm(x + 4.0*q.x + 1.7, y + 4.0*q.y + 9.2, 0.0),
            fbm(x + 4.0*q.x + 8.3, y + 4.0*q.y + 2.8, 0.0)
        };

        return fbm(x + 4.0*r.x, y + 4.0*r.y, 0.0);
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
    ~Landscape() = default;

    double **map(int width, int height){
        double **height_map = new double*[width];
        for (int i = 0; i < width; i++) {
            height_map[i] = new double[height];
            for (int j = 0; j < height; j++) {
                
                double height_value = this->fbm(i * scale, j * scale, 0.0);
                height_map[i][j] = height_value * max_height;
            }
        }
        return height_map;
    }

};

#endif //TERRAIN_HPP
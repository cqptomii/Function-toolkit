
#ifndef PERLIN_HPP
#define PERLIN_HPP


#include <algorithm>
#include <random>
#include <cmath>
#include "utils/Math_toolkit.hpp"
#include "noise.hpp"


struct Gradient{
    int x;
    int y;
    int z;
};


Gradient GRADIENTS[12] = {
    {1, 1, 0}, 
    {-1, 1, 0}, 
    {1, -1, 0}, 
    {-1, -1, 0},
    {1, 0, 1},
    {-1, 0, 1}, 
    {1, 0, -1}, 
    {-1, 0, -1},
    {0, 1, 1}, 
    {0, -1, 1}, 
    {0, 1, -1}, 
    {0, -1, -1}
};


class PerlinNoise : public Noise
{
    unsigned int seed;
    unsigned int p[512];

    public:
        PerlinNoise(unsigned int seed, unsigned int octaves = 6, double amplitude = 1.0, double frequency = 1.0, double persistence = 0.5, double lacunarity = 2.00) : Noise(octaves, amplitude, frequency, persistence, lacunarity) {
            this->seed = seed;
            for (int i = 0; i < 256; i++) {
                p[i] = i;
            }
            // Shuffle the array using the seed
            std::shuffle(p, p + 256, std::default_random_engine(seed));
            
            // Duplicate the array
            for (int i = 0; i < 256; i++) {
                p[256 + i] = p[i];
            }
        }
        
        double noise(double x, double y, double z) const override{
            const int xi = static_cast<int>(std::floor(x));
            const int yi = static_cast<int>(std::floor(y));
            const int zi = static_cast<int>(std::floor(z));

            const int X = xi & 255;
            const int Y = yi & 255;
            const int Z = zi & 255;

            
            // Get the hash coordinates of the 8 cube corners
            int A = p[X] + Y;
            int AA = p[A] + Z;
            int AB = p[A + 1] + Z;
            int B = p[X + 1] + Y;
            int BA = p[B] + Z;
            int BB = p[B + 1] + Z;

            const double xf = x - static_cast<double>(xi);
            const double yf = y - static_cast<double>(yi);
            const double zf = z - static_cast<double>(zi);

            // Get the gradient vectors for the corners
            Gradient gAA = GRADIENTS[p[AA] % 12];
            Gradient gBA = GRADIENTS[p[BA] % 12];
            Gradient gAB = GRADIENTS[p[AB] % 12];
            Gradient gBB = GRADIENTS[p[BB] % 12];

            Gradient gAA1 = GRADIENTS[p[AA + 1] % 12];
            Gradient gBA1 = GRADIENTS[p[BA + 1] % 12];
            Gradient gAB1 = GRADIENTS[p[AB + 1] % 12];
            Gradient gBB1 = GRADIENTS[p[BB + 1] % 12];
            
            // Compute the dot product between the gradient vectors and the distance vectors
            double dotAA  = Math_toolkit::dot(gAA.x,  gAA.y,  gAA.z,  xf,   yf,   zf);
            double dotBA  = Math_toolkit::dot(gBA.x,  gBA.y,  gBA.z,  xf-1, yf,   zf);
            double dotAB  = Math_toolkit::dot(gAB.x,  gAB.y,  gAB.z,  xf,   yf-1, zf);
            double dotBB  = Math_toolkit::dot(gBB.x,  gBB.y,  gBB.z,  xf-1, yf-1, zf);

            double dotAA1 = Math_toolkit::dot(gAA1.x,  gAA1.y,  gAA1.z,  xf,   yf,   zf-1);
            double dotBA1 = Math_toolkit::dot(gBA1.x,  gBA1.y,  gBA1.z,  xf-1, yf,   zf-1);
            double dotAB1 = Math_toolkit::dot(gAB1.x,  gAB1.y,  gAB1.z,  xf,   yf-1, zf-1);
            double dotBB1 = Math_toolkit::dot(gBB1.x,  gBB1.y,  gBB1.z,  xf-1, yf-1, zf-1);
            
            // Compute the fade curves for x, y, z
            double u = Math_toolkit::fade(xf);
            double v = Math_toolkit::fade(yf);
            double w = Math_toolkit::fade(zf);

            // Interpolate the dot products using the fade curves
            double lerpX1 = Math_toolkit::lerp(dotAA, dotBA, u);
            double lerpX2 = Math_toolkit::lerp(dotAB, dotBB, u);
            double lerpY1 = Math_toolkit::lerp(lerpX1, lerpX2, v);

            double lerpX3 = Math_toolkit::lerp(dotAA1, dotBA1, u);
            double lerpX4 = Math_toolkit::lerp(dotAB1, dotBB1, u);
            double lerpY2 = Math_toolkit::lerp(lerpX3, lerpX4, v);


            return Math_toolkit::lerp(lerpY1, lerpY2, w);
        }
};

#endif // PERLIN_HPP
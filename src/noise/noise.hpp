#ifndef NOISE_HPP
#define NOISE_HPP

enum class NoiseType {
    Perlin,
    Simplex,
    Value,
    Worley
};

class Noise {
    public:
        virtual ~Noise() {}
        // Noise function
        virtual double noise(double x, double y, double z) const = 0;
        
        // Fractal Brownian Motion (FBM) function
        virtual double fbm(double x, double y, double z, int octaves = 8, double amplitude = 1.0, double frequency = 1.0, double persistence = 0.5, double lacunarity = 2.0) const {
                double total = 0.0;
                double max_value = 0.0;
                double amp = amplitude;
                double freq = frequency;

                for (int i = 0; i < octaves; i++) {
                    // Add the noise value for the current octave to the total
                    total += noise(x * freq, y * freq, z * freq) * amp;
                    
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
};


#endif //NOISE_HPP
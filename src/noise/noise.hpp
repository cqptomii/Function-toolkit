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
        virtual double noise(double x, double y, double z) const = 0;
        virtual ~Noise() {}
};


#endif //NOISE_HPP
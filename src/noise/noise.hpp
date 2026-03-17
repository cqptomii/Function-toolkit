#ifndef NOISE_HPP
#define NOISE_HPP

class Noise {
    public:
        virtual double noise(double x, double y, double z) const = 0;
        virtual ~Noise() {}
};


#endif //NOISE_HPP
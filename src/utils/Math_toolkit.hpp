//
// Created by tomfr on 16/11/2024.
//

#ifndef PROCEDURAL_MATH_TOOLKIT_HPP
#define PROCEDURAL_MATH_TOOLKIT_HPP

#include <cmath>
#include <algorithm>

class Math_toolkit {
public:
    Math_toolkit() = default;
    ~Math_toolkit() = default;
    static double smooth_step(double edge_0, double edge_1, double x){
        double t = std::clamp((x - edge_0) / (edge_1 - edge_0), 0.0, 1.0);
        return t * t * (3.0 - 2.0 * t);
    }
    static double smooth_integral(double x,double max){
        if(x > max)
            return x- max / 2.0;
        return x*x*x*(1-0.5*x/max)/max/max;
    }
    static double near_identity_abs(double x, double offset){
        return sqrt(x*x + offset*offset);
    }
    static double raised_inverted_cosi(double x) {
        return (4/9)* std::pow(x,6)-(17/9)* std::pow(x,4),+(22/9)* std::pow(x,2);
    }
    static double linear_identity(double x) {
        return std::pow(std::abs(x),1);
    }
    static double curved_identity(double x) {
        return std::pow(std::abs(x),0.5);
    }
    static double dot(double x1, double y1, double z1, double x2, double y2, double z2){
        return x1 * x2 + y1 * y2 + z1 * z2;
    }
    static double fade(double t){
        return t * t * t * (t * (t * 6 - 15) + 10);
    }
    static double lerp(double a, double b, double t){
        return a + t * (b - a);
    }
    static double spline(double t){
        return t * t * (3 - 2 * t);
    }
    static double terrace(double t, double terrace_count){
        double step = 1.0 / terrace_count;
        double half_step = step / 2.0;
        double mod = std::fmod(t, step);
        if (mod < half_step) {
            return t - mod;
        } else {
            return t - mod + step;
        }
    }
};


#endif //PROCEDURAL_MATH_TOOLKIT_HPP

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
    static double smooth_step(double edge_0, double edge_1, double x);
    static double smooth_integral(double x,double max);
    static double near_identity_abs(double x, double offset);
    static double raised_inverted_cosi(double x);
    static double linear_identity(double x);
    static double curved_identity(double x);
    static double dot(double x1, double y1, double z1, double x2, double y2, double z2){
        return x1 * x2 + y1 * y2 + z1 * z2;
    }
    static double fade(double t){
        return t * t * t * (t * (t * 6 - 15) + 10);
    }
    static double lerp(double a, double b, double t){
        return a + t * (b - a);
    }
};


#endif //PROCEDURAL_MATH_TOOLKIT_HPP

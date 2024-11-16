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
};


#endif //PROCEDURAL_MATH_TOOLKIT_HPP

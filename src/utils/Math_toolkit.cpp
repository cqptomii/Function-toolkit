//
// Created by tomfr on 16/11/2024.
//

#include "Math_toolkit.hpp"

double Math_toolkit::smooth_step(double edge_0, double edge_1, double x) {
    double t = std::clamp((x-edge_0)/(edge_1-edge_0),0.0,1.0);

    return t*t*(3.0 -2.0*t);
}

double Math_toolkit::smooth_integral(double x, double max) {
    if(x > max)
        return x- max / 2.0;
    return x*x*x*(1-0.5*x/max)/max/max;
}

double Math_toolkit::near_identity_abs(double x, double offset) {
    return sqrt(x*x + offset*offset);
}

double Math_toolkit::raised_inverted_cosi(double x) {
    return (4/9)* std::pow(x,6)-(17/9)* std::pow(x,4),+(22/9)* std::pow(x,2);
}

double Math_toolkit::linear_identity(double x) {
    return std::pow(std::abs(x),1);
}

double Math_toolkit::curved_identity(double x) {
    return std::pow(std::abs(x),0.5);
}



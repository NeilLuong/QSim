#pragma once

#include <complex>
#include <vector>
#include <cmath>

using Complex = std::complex<double>;
using Gate = std::vector<std::vector<Complex>>;

Gate gate_x{
    {Complex(0.0, 0.0), Complex(1.0, 0.0)},
    {Complex(1.0, 0.0), Complex(0.0, 0.0)}};

Gate gate_y{
    {Complex(0.0, 0.0), Complex(0.0, -1.0)},
    {Complex(0.0, 1.0), Complex(0.0, 0.0)}};

Gate gate_z{
    {Complex(1.0, 0.0), Complex(0.0, 0.0)},
    {Complex(0.0, 0.0), Complex(-1.0, 0.0)}};

Gate gate_i{
    {Complex(1.0, 0.0), Complex(0.0, 0.0)},
    {Complex(0.0, 0.0), Complex(1.0, 0.0)}};

Gate gate_s{
    {Complex(1.0, 0.0), Complex(0.0, 0.0)},
    {Complex(0.0, 0.0), Complex(0.0, 1.0)}};

double inv_sqrt2 = 1.0 / std::sqrt(2.0);
Gate gate_h{
    {Complex(inv_sqrt2, 0.0), Complex(inv_sqrt2, 0.0)},
    {Complex(inv_sqrt2, 0.0), Complex(-inv_sqrt2, 0.0)}};

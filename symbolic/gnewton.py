#!/usr/bin/env python3

# for symbolic computations.
from sympy import *

# f, fp, fpp: dual function value, first and second derivatives.
f, fp, fpp = symbols('f fp fpp', real = True)

# k0, k1, k2: model function coefficients.
k0, k1, k2 = symbols('k0 k1 k2', real = True)

# w: smallest root (in absolute value) of the
#    polynomial within the natural logarithm.
w = Symbol('w', real = True, positive = True)

# dual variable, '\eta' in the manuscript.
x = Symbol('x', real = True)

# model function.
g = k0 + k1 * log(x + w) + k2 * x

# solve the matching conditions for the coefficient values.
K = solve([f - g, fp - diff(g, x), fpp - diff(g, x, x)], [k0, k1, k2])

# extract the optimal coefficient expressions.
(K0, K1, K2) = (K[k0], K[k1], K[k2])

# solve for the optimizer of the model function.
X = solve(diff(g, x), x)

# print the solved coefficient expressions.
print(f'k0* = {K0}')
print(f'k1* = {K1}')
print(f'k2* = {K2}')
print('')

# print the new estimate of the optimizer.
print(f'x* = {X[0]}')
print(f'   = {simplify(-K1/K2-w)}')
print('')


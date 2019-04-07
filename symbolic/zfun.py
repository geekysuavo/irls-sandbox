#!/usr/bin/env python3

# for symbolic computations.
from sympy import *

# alpha, beta, sigma.
a, b, s = symbols('a b s', real = True, positive = True)

# compute the normalization function.
h0 = exp(-a/(2*s**2) - (s**2)/(2*b**2))
Z = simplify(integrate(h0, (s, 0, oo)))
h = simplify(h0 / Z)

# compute the base expectations.
Es2 = simplify(integrate((s**2) * h, (s, 0, oo)))
Es2inv = simplify(integrate((1 / s**2) * h, (s, 0, oo)))

# compute the entropy.
entropy = simplify(-integrate(log(h)*h, (s, 0, oo)))

# print the normalization function and the full density.
print(f'Z(a,b) = {Z}')
print(f'h(s;a,b) = {h}')
print('')

# print the base expectations.
print(f'E[s^2] = {Es2}')
print(f'E[1/s^2] = {Es2inv}')
print('')

# print the entropy.
print(f'H[h(s;a,b)] = {entropy}')
print('')


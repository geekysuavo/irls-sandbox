#!/usr/bin/env python3

# for symbolic computations.
from sympy import *

# x, sigma.
x = Symbol('x', real = True)
s = Symbol('s', real = True, positive = True)

# xi.
t = Symbol('t', real = True, positive = True)

# mu, gamma (mean, variance).
m = Symbol('m', real = True)
v = Symbol('v', real = True, positive = True)

# alpha, beta.
a = Symbol('a', real = True, positive = True)
b = Symbol('b', real = True, positive = True)

# \psi
f = (x**2)/(2*s**2) + (s**2)/(2*t**2)

# \bar\psi
F = (m**2+v)/(2*b*sqrt(a)) + (b*(sqrt(a)+b))/(2*t**2) - log(v)/2 - log(b)

# solve for the updates to each set of variables.
S = solve(diff(f, s), s)
MV = solve([diff(F, m), diff(F, v)], [m, v])
AB = solve([diff(F, a), diff(F, b)], [a, b])

# print the IRLS sigma-update.
print('IRLS s update:')
print(f's = {S[0]}')
print(f'w = {1 / S[0]**2}')
print('')

# print the VRLS x-factor update.
print('VRLS q(x) update:')
print(f'm = {MV[m]}')
print(f'v = {MV[v]}')
print('')

# print the VRLS sigma-factor update.
print('VRLS q(s) update:')
print(f'a = {AB[0][0]}')
print(f'b = {AB[0][1]}')
print('')


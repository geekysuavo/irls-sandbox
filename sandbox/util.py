
# required imports.
import subprocess
import random
import os

# seeds: return a set of unique seed values.
def seeds(num=100):
  random.seed(4735921)
  return {random.randint(99, 9999999) for i in range(num)}


# solvers: return a list of all solver algorithms.
def solvers():
  return ('oracle', 'irls-ec', 'irls-ic', 'irls-em', 'irls-map',
          'vrls', 'vrls-ex', 'grls', 'grls-ex')


# binaries: return a dictionary of all compilable binaries.
def binaries():
  # args: build a compilation argument string list.
  def args(cxx='g++', omp=False, incl=None):
    return ([cxx, f'-std=c++17', f'-O3', '-I.', '-Ieigen3'] +
            (['-include', f'{{{incl}}}'] if incl else []) +
            (['-fopenmp'] if omp else []) +
            (['{source}', '-o', '{target}']))

  return {
    # solver algorithms.
    **{sol: args(incl='inst') for sol in solvers()},

    # gaussian process utilities.
    'gp-init': args(incl='gp'),
    'gp-next': args(incl='gp', omp=True)
  }


# solve: execute a solver with a set of parameters.
def solve(sol='oracle', parms={}):
  # parse: parse a string into a tuple of tuples.
  def parse(lines):
    return tuple(tuple(float(field) for field in line.split())
                 for line in lines.strip().split('\n'))

  # build the binary filename and the arguments list.
  binary = os.path.join('bin', sol)
  args = [f'{key}={str(val).lower()}' for key, val in parms.items()]

  # execute the binary.
  proc = subprocess.run([binary, *args], encoding='utf-8',
                        stdout=subprocess.PIPE,
                        stderr=subprocess.PIPE)

  # parse stdout and stderr.
  out = parse(proc.stdout)
  err = parse(proc.stderr)


# coords: map between phase-diagram coordinates and instance coordinates.
def coords(*args):
  # to_phasediag: map to phase-diagram coordinates.
  def to_phasediag(m, n, k):
    delta = m / n
    rho = k / m
    return (delta, rho)

  # to_instance: map to instance coordinates.
  def to_instance(delta, rho):
    n = 1000
    m = max(1, round(delta * n))
    k = max(1, round(rho * delta * n))
    return (k, m, n)

  # return the relevant mapping.
  if len(args) == 2:
    return to_instance(*args)
  elif len(args) == 3:
    return to_phasediag(*args)



# required imports.
import random

# binaries: return a dictionary of all compilable binaries.
def binaries():
  # args: build a compilation argument string list.
  def args(cxx='g++', omp=False, solver=True):
    return ([cxx, f'-std=c++17', f'-O3', '-I.', '-Ieigen3'] +
            (['-include', '{header}'] if solver else []) +
            (['-fopenmp'] if omp else []) +
            (['{source}', '-o', '{target}']))

  return {
    # solver algorithms.
    **{sol: args() for sol in ('oracle', 'irls-ec', 'irls-ic',
                               'irls-em', 'irls-map', 'vrls',
                               'vrls-ex', 'grls', 'grls-ex')},

    # gaussian process utilities.
    'gp-init': args(solver=False),
    'gp-next': args(cxx='g++-9', omp=True, solver=False)
  }


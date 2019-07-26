
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


# parse: parse a string into a tuple of tuples.
def parse(lines):
  return tuple(tuple(float(field) for field in line.split())
               for line in lines.strip().split('\n'))


# execute: execute a binary with a set of parameters.
def execute(binary, parms={}, inp=None):
  # build the binary filename and the arguments list.
  binfile = os.path.join('bin', binary)
  args = [f'{key}={str(val).lower()}' for key, val in parms.items()]

  # execute the binary.
  proc = subprocess.run([binfile, *args], input=inp, encoding='utf-8',
                        stdout=subprocess.PIPE, stderr=subprocess.PIPE)

  # parse stdout and stderr.
  out = parse(proc.stdout)
  err = parse(proc.stderr)

  # return the result.
  return (out, err)


# coords: map between phase-diagram coordinates and instance coordinates.
def coords(*args):
  # to_phasediag: map to phase-diagram coordinates.
  def to_phasediag(k, m, n):
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


# surrogate: class for managing gaussian process-based surrogate models.
class surrogate:
  def __init__(self, seed=0, grid=100, threads=4):
    # store the sampler parameters.
    self.seed = seed
    self.grid = grid
    self.threads = threads

    # initialize two arrays: one for proposed (unmeasured) samples
    # and another for measured samples.
    self.prepared = False
    self.proposals = []
    self.samples = []

  def __next__(self):
    # propose an initial set of random and edge points.
    if not self.prepared:
      self.prepare()

    # if the proposal point set has been depleted, generate more.
    if len(self.proposals) == 0:
      self.propose()

    # pop a and return proposed point from the list.
    return coords(*self.proposals.pop())

  def add(self, k, m, n, y, dy):
    # store the result in phase-diagram coordinates.
    (delta, rho) = coords(k, m, n)
    self.samples.append((delta, rho, y, dy))

  def prepare(self, num=10):
    # execute the gp-init binary.
    args = {'num': num, 'grid': self.grid, 'seed': self.seed}
    (out, err) = execute('gp-init', args)

    # store the proposed points.
    self.proposals += list(out)
    self.prepared = True

  def propose(self, num=1):
    # build the input string.
    inp = '\n'.join(' '.join(str(f) for f in s) for s in self.samples)

    # execute the gp-next binary.
    args = {'num': num, 'threads': self.threads}
    (out, err) = execute('gp-next', args, inp)

    # store the proposed points.
    self.proposals += list(out)


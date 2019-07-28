
# required imports.
from doit.tools import create_folder
from sandbox.util import seeds, execute, surrogate
import multiprocessing
import subprocess
import pickle
import gzip
import os

# execute_problem_dict: execute a solver for a single problem, given as
# a tuple of (solver,parms), and return the result as a dictionary with
# the resulting values substituted.
def execute_problem_dict(problem):
  (sol, parms) = problem
  (out, err) = execute(sol, parms)
  return {**parms, 'solver': sol, 'out': out, 'err': err}

# task_sample: task generator for creating a random sample for each solver.
def task_sample():
  # sample: create a random sample for each solver.
  def sample(solvers, base_parms, sampdir, targets):
    # subset: get the data for all matching (sol,k,m,n).
    def subset(results, sol, k, m, n):
      R = [r for r in results if r['solver'] == sol
           and r['k'] == k and r['m'] == m and r['n'] == n]
      return sorted(R, key=lambda r: r['seed'])

    # nmse: compute the normalized mean squared error between x and x0.
    def nmse(x, x0):
      ss = sum((a - b)**2 for a, b in zip(x, x0))
      ss0 = sum(b**2 for b in x0)
      return ss / ss0

    # meanvar: compute the mean and variance of the elements of x.
    def meanvar(x):
      mean = sum(x) / len(x)
      var = sum((el - mean)**2 for el in x)
      return (mean, var)

    # metric: compute success rate statistics.
    def metric(R, R0):
      ss = [nmse(next(zip(*r['out'])), next(zip(*r0['out'])))
            for r, r0 in zip(R, R0)]
      return meanvar(ss)

    # initialize a set of surrogate models, one per solver.
    models = {sol: surrogate() for sol in solvers}

    # run a set number of sampling iterations.
    iters = 100
    S = seeds()
    with multiprocessing.Pool() as pool:
      for it in range(iters):
        # get one proposal from each surrogate model.
        proposals = {next(models[sol]) for sol in solvers}

        # construct a list of problems to execute.
        problems = []
        for sol in ['oracle', *solvers]:
          for k, m, n in proposals:
            for seed in S:
              parms = {**base_parms, 'k': k, 'm': m, 'n': n, 'seed': seed}
              problems.append((sol, parms))

        # measure every proposal with each solver.
        results = pool.map(execute_problem_dict, problems)

        # store the results into a pickle file.
        filename = os.path.join(sampdir, f'{it}.gz')
        with gzip.open(filename, 'wb') as f:
          pickle.dump(results, f)

        # add the results into the surrogate models.
        for k, m, n in proposals:
          R0 = subset(results, 'oracle', k, m, n)
          for sol in solvers:
            R = subset(results, sol, k, m, n)
            (y, dy) = metric(R, R0)
            models[sol].add(k, m, n, y, dy)

    # store the phase diagram data into pickle files.
    for sol in solvers:
      filename = os.path.join(sampdir, f'{sol}.gz')
      with gzip.open(filename, 'wb') as f:
        pickle.dump(models[sol].samples, f)

  # ---

  # define a reduced set of solvers for which we will sample.
  solvers = ('irls-ec', 'irls-em', 'vrls', 'vrls-ex')

  # define the set of noise standard deviations.
  stdev = (0, 0.001, 0.01, 0.1)

  # yield one task per standard deviation.
  for s in stdev:
    # set the sample and binary directories.
    sampdir = os.path.join('expts', 'samples', str(s))
    bindir = 'bin'

    # set the base parameters.
    p = 1 / s**2 if s > 0 else 1e9
    parms = {'stdev': s, 'tau': p, 'xi': p,
             'beta_tau': p, 'beta_xi': p}

    # yield a task.
    yield {
      'name': str(s),
      'actions': [(create_folder, [sampdir]),
                  (sample, [solvers, parms, sampdir])],
      'file_dep': [os.path.join(bindir, sol)
                   for sol in ['oracle', *solvers]],
      'targets': [os.path.join(sampdir, f'{sol}.gz')
                  for sol in solvers]
    }


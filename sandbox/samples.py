
# required imports.
from doit.tools import create_folder
from sandbox.util import seeds, execute, surrogate
import multiprocessing
import subprocess
import pickle
import gzip
import os

# task_sample: task generator for creating a random sample for each solver.
def task_sample():
  # run: execute a solver for a single problem and return the result.
  def run(problem):
    (out, err) = execute(*problem)
    return {**parms, 'out': out, 'err': err}

  # sample: create a random sample for each solver.
  def sample(solvers, base_parms, sampdir, targets):
    # initialize a set of surrogate models, one per solver.
    models = {sol: surrogate() for sol in solvers}

    # run a set number of sampling iterations.
    iters = 100
    seeds = seeds()
    with multiprocessing.pool.Pool() as pool:
      for it in range(iters):
        # get one proposal from each surrogate model.
        proposals = {next(models[sol]) for sol in solvers}

        # construct a list of problems to execute.
        problems = []
        for sol in solvers:
          for k, m, n in proposals:
            for seed in seeds:
              parms = {**base_parms, 'k': k, 'm': m, 'n': n, 'seed': seed}
              problems.append((sol, parms))

        # measure every proposal with each solver.
        results = pool.map(run, problems)

        # store the results into a pickle file.
        filename = os.path.join(sampdir, f'{it}'.gz)
        with gzip.open(filename, 'wb') as f:
          pickle.dump(results)

        # FIXME -- add the results into the surrogate models.

    # store the phase diagram data into pickle files.
    for sol in solvers:
      filename = os.path.join(sampdir, f'{sol}.gz')
      with gzip.open(filename, 'wb') as f:
        pickle.dump(models[sol].samples)

  # ---

  # define a reduced set of solvers for which we will sample.
  solvers = ('irls-ec', 'irls-em', 'vrls', 'vrls-ex')

  # define the set of noise standard deviations.
  stdev = (0, 0.001, 0.01, 0.1)

  # yield one task per standard deviation.
  for s in stdev:
    # set the sample and binary directories.
    sampdir = os.path.join('expts', 'samples', s)
    bindir = 'bin'

    # set the base parameters.
    p = 1 / s**2 if s > 0 else 1
    parms = {'stdev': s, 'tau': p, 'xi': p,
             'beta_tau': p, 'beta_xi': p}

    # yield a task.
    yield {
      'name': str(s),
      'actions': [(create_folder, [sampdir]),
                  (sample, [solvers, parms, sampdir])],
      'file_dep': [os.path.join(bindir, sol) for sol in solvers],
      'targets': [os.path.join(sampdir, f'{sol}.gz') for sol in solvers]
    }


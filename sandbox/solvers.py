
# required imports.
from doit.tools import create_folder
from sandbox.util import solvers
import subprocess
import os

# task_solver: task generator for compiling all solver binaries.
def task_solver():
  # solver: compile and link a solver binary from source.
  def solver(meth, targets):
    # get the source, header, and binary filenames.
    source = os.path.join('src', f'{meth}.cc')
    header = 'src/inst.hh'
    binary = targets[0]

    # build the compilation arguments.
    args = ['g++', '-std=c++14', '-O3', '-I.', '-Ieigen3',
            '-include', header, source, '-o', binary]

    # execute the compilation command.
    proc = subprocess.run(args)

  # ---

  # loop over each required solver.
  for method in solvers():
    # get the associated filenames.
    target = os.path.join('bin', method)
    header = os.path.join('src', 'inst.hh')
    source = os.path.join('src', f'{method}.cc')

    # yield a task.
    yield {
      'name': method,
      'actions': [(create_folder, ['bin']),
                  (solver, [method])],
      'file_dep': [source, header],
      'task_dep': ['eigen3'],
      'targets': [target]
    }


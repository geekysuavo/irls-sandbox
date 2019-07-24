
# required imports.
from doit.tools import create_folder
from sandbox.util import binaries
import subprocess
import os

# task_binary: task generator for compiling binaries from source.
def task_binary():
  # binary: compile and link a binary from source.
  def binary(args, targets):
    # execute the compilation command.
    proc = subprocess.run(args)

  # ---

  # loop over each required solver.
  for name, args in binaries().items():
    # get the associated filenames.
    target = os.path.join('bin', name)
    header = os.path.join('src', 'inst.hh')
    source = os.path.join('src', f'{name}.cc')

    # make a dictionary for argument formatting.
    fmt = {'target': target, 'header': header, 'source': source}

    # apply formatting to brace arguments.
    for i, arg in enumerate(args):
      if arg.startswith('{') and arg.endswith('}'):
        args[i] = arg.format(**fmt)

    # yield a task.
    yield {
      'name': name,
      'actions': [(create_folder, ['bin']),
                  (binary, [args])],
      'file_dep': [source, header],
      'task_dep': ['eigen3'],
      'targets': [target]
    }


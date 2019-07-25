
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
    source = os.path.join('src', f'{name}.cc')
    header_gp = os.path.join('src', 'gp.hh')
    header_inst = os.path.join('src', 'inst.hh')

    # make a dictionary for argument formatting.
    fmt = {'gp': header_gp, 'inst': header_inst,
           'target': target, 'source': source}

    # apply formatting to brace arguments.
    for i, arg in enumerate(args):
      if arg.startswith('{') and arg.endswith('}'):
        args[i] = arg.format(**fmt)

    # determine the file dependencies.
    deps = []
    for arg in args:
      if arg in (source, header_gp, header_inst):
        deps.append(arg)

    # yield a task.
    yield {
      'name': name,
      'actions': [(create_folder, ['bin']),
                  (binary, [args])],
      'file_dep': deps,
      'task_dep': ['eigen3'],
      'targets': [target]
    }


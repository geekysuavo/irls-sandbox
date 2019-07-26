#!/usr/bin/env python3

# task generator imports.
from sandbox.eigen import task_eigen3
from sandbox.samples import task_sample
from sandbox.binaries import task_binary

# script entry point.
if __name__ == '__main__':
  import doit
  doit.run(globals())


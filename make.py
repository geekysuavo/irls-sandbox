#!/usr/bin/env python3

# task generator imports.
from sandbox.eigen import task_eigen3

# script entry point.
if __name__ == '__main__':
  import doit
  doit.run(globals())


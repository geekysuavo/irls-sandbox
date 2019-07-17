
# required imports.
import random

# solvers: return a tuple of all solvers required by all experiments.
def solvers():
  # FIXME -- automatically enumerate required solvers.
  return ('oracle', 'irls-ec', 'irls-ic', 'irls-map', 'irls-em',
          'grls', 'grls-ex', 'vrls', 'vrls-ex')



## irls-sandbox/src

This directory contains the source code for all implemented algorithms
in the sandbox, and a bit of common code.

Common code:

* `inst.hh`: Instance initialization code, common to all solvers.

Gaussian process search code:

* `gp-init.hh`: Generates an initial set of search points.
* `gp-next.hh`: Returns one or more new candidate points.

Canonical point estimation algorithms:

* `irls-ec.cc`: Equality-constrained IRLS.
* `irls-ic.cc`: Inequality-constrained IRLS.

Probabilistically derived point estimation algorithms:

* `irls-map.cc`: MAP inference using direct updates.
* `irls-em.cc`: MAP inference using expectation-maximization.

Variational inference algorithms:

* `vrls.cc`: Variational inference with fixed prior parameters.
* `vrls-ex.cc`: Variational inference with inferred prior parameters.

Full Bayesian inference algorithms:

* `grls.cc`: Gibbs sampling with fixed prior parameters.
* `grls-ex.cc`: Gibbs sampling with inferred prior parameters.


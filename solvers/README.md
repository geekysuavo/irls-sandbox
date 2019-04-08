
## irls-sandbox/solvers

This directory contains example Python methods for each algorithm
presented in the **irls-sandbox** manuscript.

_Higher-performance C++ code for a select few algorithms
will probably also end up here..._

The iteratively reweighted taxonomy begins with:

* *IRLS*: Maximum a posteriori (MAP) estimators.
* *VRLS*: Approximate minimum mean-square error (MMSE) estimators.

Within each of these, have the next split:

* _Direct_ methods estimate the unknown spectrum *x*.
* _Indirect_ methods estimate the unknown time-domain signal *z*.

Next, we split based on how data consistency is enforced:

* _Equality-constrained_ methods enforce exact data consistency.
* _Inequality-constrained_ methods permit inexact data consistency.
* _Unconstrained_ methods use soft restraints to suggest consistency.

Finally, within *VRLS*, we split based on which variational family
is being used to approximate the posterior distribution:

* _Mean-field (MF)_ neglects all posterior correlations.
* _Structured mean-field (SMF)_ retains posterior correlations.


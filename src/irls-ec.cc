
/* Copyright (c) 2019 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License.
 */

#include "src/inst.hh"

int main(int argc, char **argv) {
  /* initialize the problem instance. */
  inst_init(argc, argv);

  /* initialize the estimate. */
  Eigen::VectorXd x;
  x.resize(n);
  x.setZero();

  /* initialize the weights. */
  Eigen::VectorXd w;
  w.resize(n);
  w.setOnes();

  /* initialize the lagrange multipliers. */
  Eigen::VectorXd lambda;
  lambda.resize(m);
  lambda.setZero();

  /* iterate. */
  for (std::size_t it = 0; it < iters; it++) {
    /* dual ascent iterations. */
    const double step = w.minCoeff();
    for (std::size_t jt = 0; jt < dual_iters; jt++) {
      /* update the lagrange multipliers. */
      lambda += step * (y - A * x);

      /* update the estimate from the lagrange multipliers. */
      x = (A.transpose() * lambda).cwiseQuotient(w);
    }

    /* update the weights. */
    w = (x.array().abs2() + 1e-6).sqrt().inverse();
  }

  /* output the final estimate with zero variance. */
  for (std::size_t i = 0; i < n; i++)
    std::cout << x(i) << " " << 0 << "\n";
}



/* Copyright (c) 2019 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License.
 */

#include "src/inst.hh"

int main (int argc, char **argv) {
  /* initialize the problem instance. */
  inst_init(argc, argv);

  /* initialize the estimate. */
  Eigen::VectorXd x;
  x.resize(n);
  x.setZero();

  /* initialize an auxiliary variable. */
  Eigen::VectorXd z;
  z.resize(n);
  z.setZero();

  /* initialize the weights. */
  Eigen::VectorXd w;
  w.resize(n);
  w.setOnes();

  /* precompute the noise variance. */
  const double sigma2 = std::pow(sigma, 2);

  /* iterate. */
  for (std::size_t it = 0; it < iters; it++) {
    /* update the estimate. */
    z = ((L/2) * x - A.transpose() * (A * x - y)) / sigma2;
    x = z.array() / ((L/2) / sigma2 + w.array() / xi);

    /* update the weights. */
    w = (x.array().abs2() + 1e-6).sqrt().inverse();
  }

  /* output the final estimate with zero variance. */
  for (std::size_t i = 0; i < n; i++)
    std::cout << x(i) << " " << 0 << "\n";
}


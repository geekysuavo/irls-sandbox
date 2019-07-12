
/* Copyright (c) 2019 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License.
 */

#include "src/inst.hh"

int main (int argc, char **argv) {
  /* initialize the problem instance. */
  inst_init(argc, argv);

  /* initialize the mean of x. */
  Eigen::VectorXd mu;
  mu.resize(n);
  mu.setZero();

  /* initialize the variance of x. */
  Eigen::VectorXd gamma;
  gamma.resize(n);
  gamma.setOnes();

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
    /* update the mean. */
    z = mu;
    mu = (1/sigma2) * (L * z - 2 * A.transpose() * (A * z - y)).array()
       / (L/sigma2 + w.array() / xi);

    /* update the variance. */
    gamma = (w.array() / xi + delta.array() / sigma2).inverse();

    /* update the weights. */
    w = (mu.array().abs2() + gamma.array()).sqrt().inverse();
  }

  /* output the final mean and variance estimates. */
  for (std::size_t i = 0; i < n; i++)
    std::cout << mu(i) << " " << gamma(i) << "\n";
}


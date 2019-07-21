
/* Copyright (c) 2019 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License.
 */

#include "src/inst.hh"

int main(int argc, char **argv) {
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

  /* initialize the weight means. */
  Eigen::VectorXd nu;
  nu.resize(n);
  nu.setOnes();

  /* precompute a scale factor for the x-update. */
  const double Lt2 = L * tau / 2;

  /* iterate. */
  for (std::size_t it = 0; it < iters; it++) {
    /* update the mean. */
    z = mu;
    mu = (Lt2 * z - tau * A.transpose() * (A * z - y)).array()
       / (Lt2 + nu.array());

    /* update the variance. */
    gamma = (nu.array() + tau * delta.array()).inverse();

    /* update the weight means. */
    nu = (xi * (mu.array().abs2() + gamma.array()).inverse()).sqrt();
  }

  /* output the final mean and variance estimates. */
  for (std::size_t i = 0; i < n; i++)
    std::cout << mu(i) << " " << gamma(i) << "\n";
}


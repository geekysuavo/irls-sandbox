
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

  /* initialize the weight means. */
  Eigen::VectorXd nu_w;
  nu_w.resize(n);
  nu_w.setOnes();

  /* initialize the noise precision and weight precision means. */
  double nu_tau = 1, nu_xi = 1;

  /* iterate. */
  for (std::size_t it = 0; it < iters; it++) {
    /* update the mean. */
    z = mu;
    mu = (L*nu_tau * z - 2*nu_tau * A.transpose() * (A * z - y)).array()
       / (L*nu_tau + nu_w.array());

    /* update the variance. */
    gamma = (nu_w.array() + nu_tau * delta.array()).inverse();

    /* update the weight means. */
    nu_w = (nu_xi * (mu.array().abs2() + gamma.array()).inverse()).sqrt();

    /* update the weight precision mean. */
    nu_xi = std::sqrt(beta_xi / nu_w.array().inverse().sum());

    /* update the noise precision mean. */
    const double ess = (y - A * mu).squaredNorm() + delta.dot(gamma);
    nu_tau = std::sqrt(beta_tau / ess);
  }

  /* output the final mean and variance estimates. */
  for (std::size_t i = 0; i < n; i++)
    std::cout << mu(i) << " " << gamma(i) << "\n";
}


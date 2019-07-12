
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

  /* initialize the weight alphas. */
  Eigen::VectorXd alpha_omega;
  alpha_omega.resize(n);
  alpha_omega.setOnes();

  /* initialize an auxiliary variable. */
  Eigen::VectorXd z;
  z.resize(n);
  z.setZero();

  /* initialize the weights. */
  Eigen::VectorXd w;
  w.resize(n);
  w.setOnes();

  /* initialize the noise and weight rmse alphas. */
  double alpha_sigma = 1, alpha_xi = 1;

  /* iterate. */
  for (std::size_t it = 0; it < iters; it++) {
    /* update the mean. */
    const double c_xi = 1 / (nu_xi * std::sqrt(alpha_xi));
    const double c_sigma = 1 / (nu_sigma * std::sqrt(alpha_sigma));
    z = mu;
    mu = c_sigma * (L * z - 2 * A.transpose() * (A * z - y)).array()
       / (L*c_sigma + c_xi * w.array());

    /* update the variance. */
    gamma = (c_xi * w.array() + c_sigma * delta.array()).inverse();

    /* update the weight alphas. */
    alpha_omega = (mu.array().abs2() + gamma.array())
                / (nu_xi * std::sqrt(alpha_xi));

    /* update the weights. */
    w = alpha_omega.array().sqrt().inverse();

    /* update the noise alpha. */
    alpha_sigma = (y - A * mu).squaredNorm() + delta.dot(gamma);

    /* update the weight rmse alpha. */
    const double a =
      (w.array() * (mu.array().abs2() + gamma.array()) / nu_xi).sum();
    const double b = n * nu_xi + 1 / nu_xi;
    alpha_xi = (a*b + 2*n*n + 2*n*std::sqrt(a*b + n*n)) / (b*b);
  }

  /* output the final mean and variance estimates. */
  for (std::size_t i = 0; i < n; i++)
    std::cout << mu(i) << " " << gamma(i) << "\n";
}


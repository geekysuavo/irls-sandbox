
/* Copyright (c) 2019 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License.
 */

#include "src/inst.hh"

int main(int argc, char **argv) {
  /* initialize the problem instance, with orthonormalization. */
  orth = true;
  inst_init(argc, argv);

  /* initialize the estimate. */
  Eigen::VectorXd x;
  x.resize(n);
  x.setZero();

  /* initialize an auxiliary variable. */
  Eigen::VectorXd z;
  z.resize(n);
  z.setZero();

  /* initialize the pseudo-residual. */
  Eigen::VectorXd r;
  r.resize(m);
  r.setZero();

  /* initialize the weights. */
  Eigen::VectorXd w;
  w.resize(n);
  w.setOnes();

  /* precompute the constraint bound from the noise precision. */
  const double c = std::sqrt(m / tau);

  /* iterate. */
  for (std::size_t it = 0; it < iters; it++) {
    /* compute the Lipschitz constant for the bounded function. */
    const double Lw = 2 * w.maxCoeff();

    /* compute the Lagrange multiplier. */
    z = 1 - (2/Lw) * w.array();
    r = y - A * (z.cwiseProduct(x));
    const double lambda = std::max(0., (Lw/4) * (r.norm() / c - 1));
    const double beta = (4 * lambda) / (4 * lambda + Lw);

    /* update the estimate. */
    z = (Lw/2) * x - w.cwiseProduct(x) + 2 * lambda * A.transpose() * y;
    x = (2/Lw) * (z - beta * A.transpose() * A * z);

    /* update the weights. */
    w = (x.array().abs2() + 1e-6).sqrt().inverse();
  }

  /* output the final estimate with zero variance. */
  for (std::size_t i = 0; i < n; i++)
    std::cout << x(i) << " " << 0 << "\n";
}



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

  /* initialize an auxiliary variable. */
  Eigen::VectorXd z;
  z.resize(n);
  z.setZero();

  /* initialize the weights. */
  Eigen::VectorXd w;
  w.resize(n);
  w.setOnes();

  /* precompute a scale factor for the x-update. */
  const double Lt2 = L * tau / 2;

  /* iterate. */
  for (std::size_t it = 0; it < iters; it++) {
    /* update the estimate. */
    z = x;
    x = (Lt2 * z - tau * A.transpose() * (A * z - y)).array()
      / (Lt2 + w.array());

    /* update the weights. */
    w = (xi * x.array().abs2().inverse()).sqrt();
  }

  /* output the final estimate with zero variance. */
  for (std::size_t i = 0; i < n; i++)
    std::cout << x(i) << " " << 0 << "\n";
}



/* Copyright (c) 2019 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License.
 */

int main(int argc, char **argv) {
  /* initialize the problem instance. */
  inst_init(argc, argv);

  /* initialize variables for running mean and variance computations. */
  Eigen::VectorXd M1, M2, mu, gamma;
  M1.resize(n);
  M2.resize(n);
  mu.resize(n);
  gamma.resize(n);
  M1.setZero();
  M2.setZero();
  mu.setZero();
  gamma.setZero();

  /* initialize variables for sampling values of x. */
  Eigen::VectorXd z1, z2, z3, u, t;
  z1.resize(m);
  z2.resize(n);
  z3.resize(n);
  u.resize(n);
  t.resize(m);

  /* initialize the sample of x. */
  Eigen::VectorXd x;
  x.resize(n);
  x.setZero();

  /* initialize the weight sample. */
  Eigen::VectorXd w;
  w.resize(n);
  w.setOnes();

  /* randomly re-seed the pseudorandom number generator. */
  std::random_device rdev;
  gen.seed(rdev());

  /* iterate. */
  for (std::size_t it = 0; it < burn_iters + iters; it++) {
    /* draw an m-vector of standard normal variates. */
    for (std::size_t j = 0; j < m; j++)
      z1(j) = nrm(gen);

    /* draw an n-vector of standard normal variates. */
    for (std::size_t i = 0; i < n; i++)
      z2(i) = nrm(gen);

    /* z3 = sqrt(tau) .* A' * z1 + sqrt(w) .* z2 */
    z3 = std::sqrt(tau) * A.transpose() * z1 +
         w.cwiseSqrt().cwiseProduct(z2);

    /* sample x:
     *  u = tau .* A' * y + z3
     *  t = (eye(m) ./ tau + A * diag(1 ./ w) * A') \ (A * (u ./ w))
     *  x = (u - A' * t) ./ w;
     */
    u = tau * A.transpose() * y + z3;
    t.setConstant(1 / tau);
    auto llt = (Eigen::MatrixXd{t.asDiagonal()} +
                A * w.cwiseInverse().asDiagonal() * A.transpose()).llt();
    t = llt.solve(A * u.cwiseQuotient(w));
    x = (u - A.transpose() * t).cwiseQuotient(w);

    /* sample w. */
    for (std::size_t i = 0; i < n; i++)
      w(i) = igrnd(std::sqrt(xi) / (std::abs(x(i)) + 1e-3), xi);

    /* check if the sample should be stored. */
    if (it >= burn_iters) {
      /* compute the sample count. */
      const double S = (it - burn_iters) + 1;

      /* update the first moment of x. */
      M1 = x - mu;
      mu += M1 / S;

      /* update the second moment of x. */
      M2 += M1.cwiseProduct(x - mu);
      gamma = M2 / S;
    }
  }

  /* output the final mean and variance estimates. */
  for (std::size_t i = 0; i < n; i++)
    std::cout << mu(i) << " " << gamma(i) << "\n";
}


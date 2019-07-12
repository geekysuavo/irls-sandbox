
/* Copyright (c) 2019 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License.
 */

#include "src/inst.hh"

/* igrnd(): draw a random sample from an inverse Gaussian distribution.
 *
 * arguments:
 *  @mu: mean parameter.
 *  @lambda: shape parameter.
 */
static double igrnd(double mu, double lambda) {
  /* draw a standard normal variate and a standard uniform variate. */
  const double z = nrm(gen);
  const double u = unif(gen);

  /* square the normal variate. */
  const double y = std::pow(z, 2);

  /* compute intermediate quantities. */
  const double B = mu / (2 * lambda);
  const double A = mu * y * B;
  const double C = 4 * mu * lambda * y + mu * mu * y * y;

  /* compute the two roots to the associated quadratic function. */
  const double x1 = mu + A - B * std::sqrt(C);
  const double x2 = mu * mu / x1;

  /* return one of the roots, based on the uniform deviate. */
  return (u <= mu / (mu + x1) ? x1 : x2);
}

int main (int argc, char **argv) {
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

  /* precompute the noise variance. */
  const double sigma2 = std::pow(sigma, 2);
  const double xi2 = std::pow(xi, 2);

  /* iterate. */
  for (std::size_t it = 0; it < burn_iters + iters; it++) {
    /* draw an m-vector of standard normal variates. */
    for (std::size_t j = 0; j < m; j++)
      z1(j) = nrm(gen);

    /* draw an n-vector of standard normal variates. */
    for (std::size_t i = 0; i < n; i++)
      z2(i) = nrm(gen);

    /* z3 = sqrt(tau) .* A' * z1 + sqrt(w) .* z2 */
    z3 = (1/sigma) * A.transpose() * z1 +
         w.cwiseSqrt().cwiseProduct(z2);

    /* sample x:
     *  u = (1/sigma^2) .* A' * y + z3
     *  t = (sigma^2 .* eye(m) + A * diag(1 ./ w) * A') \ (A * (u ./ w))
     *  x = (u - A' * t) ./ w;
     */
    u = (A.transpose() * y) / sigma2 + z3;
    t.setConstant(sigma2);
    auto llt = (Eigen::MatrixXd{t.asDiagonal()} +
                A * w.cwiseInverse().asDiagonal() * A.transpose()).llt();
    t = llt.solve(A * u.cwiseQuotient(w));
    x = (u - A.transpose() * t).cwiseQuotient(w);

    /* sample w. */
    for (std::size_t i = 0; i < n; i++)
      w(i) = igrnd(1 / (xi * std::abs(x(i))), 1 / xi2);

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



/* Copyright (c) 2019 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License.
 */

#pragma once
#include <limits>
#include <random>
#include <string>
#include <iostream>
#include <Eigen/Dense>

/* problem instance variables:
 *
 *  @k: sparsity, number of nonzero entries in @x0.
 *  @m: measurement count, number of elements of @y.
 *  @n: solution vector size, number of elements of @x0.
 *
 *  @seed: pseudorandom number generator seed.
 *  @stdev: standard deviation of the measurement errors.
 *  @orth: whether or not to orthogonalize the sensing matrix rows.
 *
 *  @A: measurement/sensing matrix, @m by @n.
 *  @x0: ground truth solution vector.
 *  @y: measured signal vector.
 *  @delta: diagonal of the sensing matrix gramian.
 *  @L: twice the maximal eigenvalue of the sensing matrix.
 */
std::size_t k = 10;
std::size_t m = 50;
std::size_t n = 100;
std::size_t seed = 47251;
double stdev = 0.001;
bool orth = false;
Eigen::MatrixXd A;
Eigen::VectorXd y;
Eigen::VectorXd x0;
Eigen::VectorXd delta;
double L;

/* algorithm parameters:
 *
 *  @iters: number of iterations.
 *  @burn_iters: number of burn-in iterations for samplers.
 *  @dual_iters: number of dual ascent iterations per iteration.
 *
 *  @tau: fixed noise precision value.
 *  @xi: fixed regularization parameter value.
 *
 *  @beta_tau: prior parameter for @tau.
 *  @beta_xi: prior parameter for @xi.
 */
std::size_t iters = 1000;
std::size_t burn_iters = 100;
std::size_t dual_iters = 5;
double tau = 1;
double xi = 1;
double beta_tau = 1;
double beta_xi = 1;

/* utility variables:
 *  @pi: ratio of the circumference of a circle to its diameter. ;)
 *  @gen: pseudorandom number generator.
 *  @nrm: standard normal distribution.
 *  @unif: standard uniform distribution.
 */
constexpr double pi = 3.14159265358979323846264338327950288;
std::default_random_engine gen;
std::normal_distribution<double> nrm{0, 1};
std::uniform_real_distribution<double> unif{0, 1};

/* inst_init(): initialize the current problem instance.
 */
static void inst_init(int argc, char **argv) {
  /* parse the runtime arguments. */
  for (std::size_t i = 1; i < argc; i++) {
    /* get the current argument. */
    std::string arg(argv[i]);
    auto idx = arg.find_first_of('=');
    if (idx == std::string::npos)
      continue;

    /* split the argument into key and value. */
    auto key = arg.substr(0, idx);
    auto val = arg.substr(idx + 1);

    /* run very basic argument parsing. */
    if (key.compare("orth") == 0) {
      if (val.compare("y") == 0 ||
          val.compare("yes") == 0 ||
          val.compare("true") == 0)
        orth = true;
    }
    else if (key.compare("k") == 0) { k = std::stoi(val); }
    else if (key.compare("m") == 0) { m = std::stoi(val); }
    else if (key.compare("n") == 0) { n = std::stoi(val); }
    else if (key.compare("seed")  == 0) { seed = std::stoi(val); }
    else if (key.compare("stdev") == 0) { stdev = std::stod(val); }
    else if (key.compare("iters") == 0) { iters = std::stoi(val); }
    else if (key.compare("burn_iters") == 0) { burn_iters = std::stoi(val); }
    else if (key.compare("dual_iters") == 0) { dual_iters = std::stoi(val); }
    else if (key.compare("tau") == 0) { tau = std::stod(val); }
    else if (key.compare("xi") == 0)  { xi = std::stod(val); }
    else if (key.compare("beta_tau") == 0) { beta_tau = std::stod(val); }
    else if (key.compare("beta_xi") == 0)  { beta_xi = std::stod(val); }
  }

  /* prepare the required distributions. */
  std::uniform_int_distribution<std::size_t> idx{0, n - 1};
  std::uniform_int_distribution<std::size_t> bin{0, 1};
  gen.seed(seed);

  /* compute each row of the measurement matrix. */
  A.resize(m, n);
  for (std::size_t i = 0; i < m; i++) {
    /* sample the row elements. */
    for (std::size_t j = 0; j < n; j++)
      A(i,j) = nrm(gen);

    /* normalize the row to unit length. */
    A.row(i).normalize();
  }

  /* if requested, perform complete row-orthonormalization. */
  if (orth) {
    using SVD = Eigen::JacobiSVD<decltype(A)>;
    SVD svd(A, Eigen::ComputeThinU | Eigen::ComputeThinV);
    A = svd.matrixV().transpose();
  }

  /* fill the feature vector with spikes. */
  std::size_t spikes = 0;
  x0.resize(n);
  x0.setZero();
  do {
    /* sample a random spike index. */
    std::size_t j = idx(gen);
    if (x0(j) != 0)
      continue;

    /* sample a random spike value. */
    x0(j) = (bin(gen) ? 1 : -1);
    spikes++;
  }
  while (spikes < k);

  /* compute the noise-free data vector. */
  y = A * x0;

  /* check if the noise is positive. */
  if (stdev > 0) {
    /* add noise to the data vector. */
    for (std::size_t i = 0; i < m; i++)
      y(i) += stdev * nrm(gen);
  }

  /* compute the dominant eigenvalue of the gramian matrix. */
  const double tol = 2 * std::numeric_limits<double>::epsilon();
  Eigen::VectorXd b{n}, Ab{m};
  b = A.row(0);
  b.normalize();
  double ev = 0;
  while (true) {
    /* compute a new eigenvector. */
    Ab = A * b;
    b = A.transpose() * Ab;
    b.normalize();

    /* test for eigenvalue convergence. */
    double ev_new = Ab.dot(Ab) / b.dot(b);
    if (std::abs(ev - ev_new) <= tol)
      break;

    /* update the eigenvalue estimate. */
    ev = ev_new;
  }

  /* double the result, yielding the lipschitz constant. */
  L = 2 * ev;

  /* compute the diagonal elements of the gramian matrix. */
  delta.resize(n);
  for (std::size_t i = 0; i < n; i++)
    delta(i) = A.col(i).squaredNorm();
}

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


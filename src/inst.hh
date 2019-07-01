
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
 *
 *  @A: measurement/sensing matrix, @m by @n.
 *  @x0: ground truth solution vector.
 *  @y: measured signal vector.
 *  @L: twice the maximal eigenvalue of the sensing matrix.
 */
std::size_t k = 10;
std::size_t m = 50;
std::size_t n = 100;
std::size_t seed = 47251;
double stdev = 0.001;
Eigen::MatrixXd A;
Eigen::VectorXd y;
Eigen::VectorXd x0;
double L;

/* algorithm parameters:
 *
 *  @iters: number of iterations.
 *  @dual_iters: number of dual ascent iterations per iteration.
 *
 *  @sigma: fixed "noise norm" value.
 *  @omega: fixed "regularization norm" value.
 *  @xi: fixed "norm balance" parameter value.
 *
 *  @nu_sigma: prior parameter for @sigma.
 *  @nu_omega: prior parameter for @omega.
 *  @nu_xi: prior parameter for @xi.
 */
std::size_t iters = 1000;
std::size_t dual_iters = 5;
double sigma = 1e-6;
double omega = 1;
double xi = 1;
double nu_sigma = 1e-6;
double nu_omega = 1;
double nu_xi = 1;

/* utility variables:
 *  @pi: ratio of the circumference of a circle to its diameter. ;)
 *  @gen: pseudorandom number generator.
 */
constexpr double pi = 3.14159265358979323846264338327950288;
std::default_random_engine gen;

/* inst_init(): initialize the current problem instance.
 */
static void inst_init (int argc, char **argv) {
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
    if      (key.compare("k") == 0) { k = std::stoi(val); }
    else if (key.compare("m") == 0) { m = std::stoi(val); }
    else if (key.compare("n") == 0) { n = std::stoi(val); }
    else if (key.compare("seed")  == 0) { seed = std::stoi(val); }
    else if (key.compare("stdev") == 0) { stdev = std::stod(val); }
    else if (key.compare("iters") == 0) { iters = std::stoi(val); }
    else if (key.compare("dual_iters") == 0) { dual_iters = std::stoi(val); }
    else if (key.compare("sigma") == 0) { sigma = std::stod(val); }
    else if (key.compare("omega") == 0) { omega = std::stod(val); }
    else if (key.compare("xi") == 0)    { xi = std::stod(val); }
    else if (key.compare("nu_sigma") == 0) { nu_sigma = std::stod(val); }
    else if (key.compare("nu_omega") == 0) { nu_omega = std::stod(val); }
    else if (key.compare("nu_xi") == 0)    { nu_xi = std::stod(val); }
  }

  /* prepare the required distributions. */
  std::uniform_int_distribution<std::size_t> idx{0, n - 1};
  std::uniform_int_distribution<std::size_t> bin{0, 1};
  std::normal_distribution<double> nrm{0, 1};
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

  /* fill the feature vector with spikes. */
  std::size_t spikes = 0;
  x0.resize(n, 1);
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
}


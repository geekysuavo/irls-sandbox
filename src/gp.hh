
/* Copyright (c) 2019 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License.
 */

#pragma once
#include <iostream>
#include <utility>
#include <string>
#include <random>
#include <vector>
#include <cmath>
#include <Eigen/Dense>

/* type definitions:
 *
 *  @point: a pair of grid points, i.e. (x, y).
 *  @hit: a paired point and utility value.
 *  @seed: pseudorandom variate data type.
 */
using point = Eigen::Vector2d;
using hit = std::pair<point, double>;
using seed = std::default_random_engine::result_type;

/* application parameters:
 *
 *  @n: number of points to return.
 *  @ns: number of grid points along each dimension.
 *  @rs: random seed value, if complete determinism is needed.
 *  @threads: number of threads to use for utility evaluation.
 *  @exploit: whether to exploit or not. if not, just explore.
 */
int n = 1;
int ns = 100;
seed rs = 0;
int threads = 4;
bool exploit = false;

/* hit_list: maintains a sorted (descending) list of hits.
 */
struct hit_list {
public:
  /* hit_list(): constructor, takes the number of hits to track.
   */
  hit_list(int num = 1) : n{num}, ymin{0} {
    /* initialize the internal vector. */
    hl.reserve(n);
    for (int i = 0; i < n; i++)
      hl.push_back({{0, 0}, 0});
  }

  /* update(point, double):
   *  if the new point has a higher utility,
   *  it is inserted into the hit list.
   */
  void update(const point& x, double y) {
    /* perform a coarse check. */
    if (y <= ymin)
      return;

    /* determine the index of the point. */
    for (int i = 0; i < n; i++) {
      if (y > std::get<1>(hl[i])) {
        /* shift the remaining points down the vector. */
        for (int j = n - 1; j > i; j--)
          hl[j] = hl[j - 1];

        /* store the hit in the vector. */
        hl[i] = {x, y};
        break;
      }
    }

    /* update the worst utility value. */
    ymin = std::get<1>(hl[n - 1]);
  }

  /* update(hit): same as the above function, but for a passed hit.
   */
  void update(const hit& xy) {
    update(std::get<0>(xy), std::get<1>(xy));
  }

  /* operator+=():
   *  combined addition and assignment operator, used to
   *  insert the contents of one hit list into another.
   */
  hit_list& operator+=(const hit_list& rhs) {
    for (int i = 0; i < rhs.n; i++)
      update(rhs.hl[i]);

    return *this;
  }

  /* operator<<(): streaming operator, used to write to stdout.
   */
  friend std::ostream& operator<<(std::ostream& os, const hit_list& h) {
    /* write all hit points. */
    for (int i = 0; i < h.n; i++)
      os << std::get<0>(h.hl[i])(0) << " "
         << std::get<0>(h.hl[i])(1) << "\n";

    /* return the stream. */
    return os;
  }

private:
  /* struct members:
   *
   *  @n: number of top hits to keep track of.
   *  @ymin: utility of the worst top hit.
   *  @hl: internal vector of hits.
   */
  int n;
  double ymin;
  std::vector<hit> hl;
};

/* kernel(): gaussian process covariance kernel function.
 */
double kernel(const point& x, const point& y, double delta) {
  /* precompute the kernel width. */
  static constexpr double s = 0.1;
  static constexpr double s2 = std::pow(s, 2);

  /* squared exponential kernel plus measurement noise. */
  double kxy = std::exp(-(x - y).squaredNorm() / (2 * s2));
  if (x == y)
    kxy += delta;

  /* return the result. */
  return kxy;
}

/* util(): gaussian process search utility function.
 */
double util(double mean, double var, bool exploit) {
  if (exploit)
    /* exploitation: prefer high-variance points near the transition. */
    return -var * std::pow(mean - 0.5, 2);
  else
    /* exploration: utility is the variance only. */
    return var;
}

/* mask(): feasibility masking function.
 */
bool mask(double delta, double rho) {
  /* this needs to match sandbox.util.coords.to_instance() */
  constexpr int n = 1000;
  return std::round(delta * n) >= 1 &&
         std::round(rho * delta * n) >= 1;
}

/* gp_init(): initialize the application arguments.
 */
void gp_init(int argc, char **argv) {
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
    if      (key.compare("threads") == 0) { threads = std::stoi(val); }
    else if (key.compare("seed")    == 0) { rs = std::stoi(val); }
    else if (key.compare("grid")    == 0) { ns = std::stoi(val); }
    else if (key.compare("num")     == 0) { n = std::stoi(val); }
    else if (key.compare("exploit") == 0) {
      if (val.compare("y") == 0 ||
          val.compare("yes") == 0 ||
          val.compare("true") == 0)
        exploit = true;
    }
  }

  /* if no seed was specified, obtain a random seed. */
  if (rs == 0) {
    std::random_device rdev;
    rs = rdev();
  }
}


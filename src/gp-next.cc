
/* Copyright (c) 2019 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License.
 */

#include <iostream>
#include <utility>
#include <string>
#include <vector>
#include <cmath>
#include <Eigen/Dense>

/* type definitions:
 *
 *  @point: a pair of grid points, i.e. (x, y).
 *  @hit: a paired point and utility value.
 */
using point = Eigen::Vector2d;
using hit = std::pair<point, double>;

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

int main(int argc, char **argv) {
  /* parameters:
   *
   *  @exploit: whether to exploit or not. if not, just explore.
   *  @nth: number of threads to use for utility evaluation.
   *  @n: number of max-utility points to return.
   *  @ns: number of grid points per dimension.
   */
  bool exploit = false;
  int nth = 4;
  int ns = 100;
  int n = 1;

  /* parse runtime arguments. */
  for (int i = 1; i < argc; i++) {
    /* get the current argument. */
    std::string arg(argv[i]);
    auto idx = arg.find_first_of('=');
    if (idx == std::string::npos)
      continue;

    /* split the argument into key and value. */
    auto key = arg.substr(0, idx);
    auto val = arg.substr(idx + 1);

    /* parse the arguments. */
    if      (key.compare("threads") == 0) { nth = std::stoi(val); }
    else if (key.compare("grid")    == 0) { ns = std::stoi(val); }
    else if (key.compare("num")     == 0) { n = std::stoi(val); }
    else if (key.compare("exploit") == 0) {
      if (val.compare("y") == 0 ||
          val.compare("yes") == 0 ||
          val.compare("true") == 0)
        exploit = true;
    }
  }

  /* set up the threads. */
  omp_set_num_threads(nth);
  Eigen::setNbThreads(1);

  /* declare vectors for holding the current dataset. */
  std::vector<point> X;
  std::vector<double> y, dy;

  /* read the current dataset from stdin. */
  int idx = 0;
  double s[4];
  while (std::cin >> s[idx]) {
    if (++idx == 4) {
      X.push_back({s[0], s[1]});
      y.push_back(s[2]);
      dy.push_back(s[3]);

      idx = 0;
    }
  }

  /* declare the hit list and dataset size. */
  hit_list hits{n};
  const int N = X.size();

  /* compute the kernel matrix elements. */
  Eigen::MatrixXd K{N, N};
  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)
      K(i,j) = kernel(X[i], X[j], dy[i]);

  /* compute the data vector. */
  Eigen::VectorXd yv{N};
  for (int i = 0; i < N; i++)
    yv(i) = y[i];

  /* decompose the kernel matrix. */
  auto lltK = K.llt();
  Eigen::VectorXd alpha = lltK.solve(yv);

  /* search all grid points to find the points with maximal utility. */
  #pragma omp declare reduction(+: hit_list: omp_out += omp_in)
  #pragma omp parallel reduction(+: hits)
  {
    /* declare thread-local vectors. */
    point xs;
    Eigen::VectorXd ks{N};

    /* search in parallel. */
    #pragma omp for
    for (int ii = 0; ii < ns * ns; ii++) {
      /* compute the grid point indices from the linearized index. */
      const int uidx = ii % ns;
      const int vidx = (ii - uidx) / ns;

      /* skip the outer edges. */
      if (uidx == 0 || uidx == ns - 1 ||
          vidx == 0 || vidx == ns - 1)
        continue;

      /* compute the grid point values. */
      const double u = double(uidx) / ns;
      const double v = double(vidx) / ns;

      /* compute the kernel vector. */
      xs = {u, v};
      for (int i = 0; i < N; i++)
        ks(i) = kernel(X[i], xs, dy[i]);

      /* compute the gp posterior predictive mean and variance. */
      const double mean = ks.dot(alpha);
      const double var = kernel(xs, xs, 0) - ks.dot(lltK.solve(ks));

      /* compute the utility value. */
      const double Us = util(mean, var, exploit);
      hits.update(xs, Us);
    }
  }

  /* print the hit list. */
  std::cout << hits;
}


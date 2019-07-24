
/* Copyright (c) 2019 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License.
 */

#include <iostream>
#include <string>
#include <random>

int main(int argc, char **argv) {
  /* type definitions. */
  using seed = std::default_random_engine::result_type;

  /* parameters:
   *
   *  @n0: initial number of grid points per side, and randoms.
   *  @ns: number of grid points per dimension.
   *  @rs: pseudorandom seed.
   */
  int n0 = 10;
  int ns = 100;
  seed rs = 0;

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
    if      (key.compare("num")  == 0) { n0 = std::stoi(val); }
    else if (key.compare("grid") == 0) { ns = std::stoi(val); }
    else if (key.compare("seed") == 0) { rs = std::stoi(val); }
  }

  /* compute the grid pixel minimum and maximum. */
  const double lb = 1.0 / ns;
  const double ub = 1.0 - lb;

  /* if no seed was specified, obtain a random seed. */
  if (rs == 0) {
    std::random_device rdev;
    rs = rdev();
  }

  /* initialize a pseudorandom number generator and distribution. */
  std::default_random_engine gen{rs};
  std::uniform_real_distribution<double> rnd{lb, ub};

  /* define a function for printing pairs of doubles. */
  auto print = [] (double x, double y) -> void {
    std::cout << x << " " << y << "\n";
  };

  /* loop over the number of output points per group. */
  for (int i = 0; i < n0; i++) {
    /* compute an interpolant within [lb,ub]. */
    const double t = lb + (ub - lb) * (double(i) / double(n0));

    /* sample a random coordinates. */
    const double x = rnd(gen);
    const double y = rnd(gen);

    /* print the points for each group: */
    print(x, y);             /* random. */
    print(t, lb);            /* lower boundary. */
    print(ub, t);            /* right boundary. */
    print(1 - t, ub);        /* upper boundary. */
    print(lb, 1 - t);        /* left boundary. */
  }
}


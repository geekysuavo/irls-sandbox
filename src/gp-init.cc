
/* Copyright (c) 2019 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License.
 */

int main(int argc, char **argv) {
  /* initialize. */
  gp_init(argc, argv);

  /* compute the grid pixel minimum and maximum. */
  const double lb = 1.0 / ns;
  const double ub = 1.0 - lb;

  /* initialize a pseudorandom number generator and distribution. */
  std::default_random_engine gen{rs};
  std::uniform_real_distribution<double> rnd{lb, ub};

  /* define a function for printing pairs of doubles. */
  auto print = [] (double x, double y) -> void {
    if (mask(x, y))
      std::cout << x << " " << y << "\n";
  };

  /* loop over the number of output points per group. */
  for (int i = 0; i < n; i++) {
    /* compute an interpolant within [lb,ub]. */
    const double t = lb + (ub - lb) * (double(i) / double(n));

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


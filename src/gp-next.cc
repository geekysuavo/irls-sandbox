
/* Copyright (c) 2019 Bradley Worley <geekysuavo@gmail.com>
 * Released under the MIT License.
 */

int main(int argc, char **argv) {
  /* initialize. */
  gp_init(argc, argv);

  /* set up the threads. */
  omp_set_num_threads(threads);
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

      /* compute the grid point values. */
      const double u = double(uidx) / ns;
      const double v = double(vidx) / ns;

      /* skip infeasible points. */
      if (!mask(u, v))
        continue;

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


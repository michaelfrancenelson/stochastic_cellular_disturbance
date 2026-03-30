#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// libraries:
#include "../lib/dSFMT-src-2.2.2/dSFMT.c" // dSFMT random number generator
#include "../lib/dSFMT-src-2.2.2/dSFMT.h" // dSFMT random number generator
dsfmt_t dsfmt;

#define repeat for (;;)

/**
 * @brief Initialize the RNG
 * @param seed Seed used for initialization.  If 0, the current system time is used as a seed.
 */
void initializeRS(unsigned long int seed)
{

  if (seed == 0)
  {
    seed = (unsigned)time(NULL);
  }
  // Initialize the random number generator for the dSFMT seed.
  dsfmt_init_gen_rand(&dsfmt, seed);
}

/**
 * Generate a random integer from a Poisson distribution
 * using Knuth's algorithm.
 *
 * @param lambda Mean (λ) of the Poisson distribution (must be > 0)
 * @return Random integer from Poisson(λ)
 */
int random_poisson(double lambda)
{
  if (lambda <= 0)
  {
    fprintf(stderr, "Error: lambda must be positive.\n");
    return -1;
  }

  double L = exp(-lambda);
  double p = 1.0;
  int k = 0;

  double randU = dsfmt_genrand_close_open(&dsfmt);
  do
  {
    k++;
    p *= randU;
    randU = dsfmt_genrand_close_open(&dsfmt);
  } while (p > L);

  return k - 1;
}

/**
 * @brief Return a random number drawn from a Binomial distribution.
 * @param nin The number of trials.
 * @param pp The probability of success for each trial.
 * @return The number of successes of the nin trials.
 * @note This was adapted from the source code for the R rbinom() function.
 */
int rbinom(double nin, double pp)
{
  /* FIXME: These should become THREAD_specific globals : */

  static double c, fm, npq, p1, p2, p3, p4, qn;
  static double xl, xll, xlr, xm, xr;

  static double psave = -1.0;
  static int nsave = -1;
  static int m;

  double f, f1, f2, u, v, w, w2, x, x1, x2, z, z2;
  double p, q, np, g, r, al, alv, amaxp, ffm, ynorm;
  int i, ix, k, n;

  r = nin;

  if (r == 0 || pp == 0.)
    return 0;
  if (pp == 1.)
    return (int)r;

  n = (int)r;

  p = fmin(pp, 1. - pp);
  q = 1. - p;
  np = n * p;
  r = p / q;
  g = r * (n + 1);

  if (pp != psave || n != nsave)
  {
    psave = pp;
    nsave = n;
    if (np < 30.0)
    {
      /* inverse cdf logic for mean less than 30 */
      qn = pow(q, n);
      goto L_np_small;
    }
    else
    {
      ffm = np + p;
      m = (int)ffm;
      fm = m;
      npq = np * q;
      p1 = (int)(2.195 * sqrt(npq) - 4.6 * q) + 0.5;
      xm = fm + 0.5;
      xl = xm - p1;
      xr = xm + p1;
      c = 0.134 + 20.5 / (15.3 + fm);
      al = (ffm - xl) / (ffm - xl * p);
      xll = al * (1.0 + 0.5 * al);
      al = (xr - ffm) / (xr * q);
      xlr = al * (1.0 + 0.5 * al);
      p2 = p1 * (1.0 + c + c);
      p3 = p2 + c / xll;
      p4 = p3 + c / xlr;
    }
  }
  else if (n == nsave)
  {
    if (np < 30.0)
      goto L_np_small;
  }

  /*-------------------------- np = n*p >= 30 : ------------------- */
  repeat
  {
    u = dsfmt_genrand_close_open(&dsfmt) * p4;
    v = dsfmt_genrand_close_open(&dsfmt);
    /* triangular region */
    if (u <= p1)
    {
      ix = (int)(xm - p1 * v + u);
      goto finis;
    }
    /* parallelogram region */
    if (u <= p2)
    {
      x = xl + (u - p1) / c;
      v = v * c + 1.0 - fabs(xm - x) / p1;
      if (v > 1.0 || v <= 0.)
        continue;
      ix = (int)x;
    }
    else
    {
      if (u > p3)
      { /* right tail */
        ix = (int)(xr - log(v) / xlr);
        if (ix > n)
          continue;
        v = v * (u - p3) * xlr;
      }
      else
      { /* left tail */
        ix = (int)(xl + log(v) / xll);
        if (ix < 0)
          continue;
        v = v * (u - p2) * xll;
      }
    }
    /* determine appropriate way to perform accept/reject test */
    k = abs(ix - m);
    if (k <= 20 || k >= npq / 2 - 1)
    {
      /* explicit evaluation */
      f = 1.0;
      if (m < ix)
      {
        for (i = m + 1; i <= ix; i++)
          f *= (g / i - r);
      }
      else if (m != ix)
      {
        for (i = ix + 1; i <= m; i++)
          f /= (g / i - r);
      }
      if (v <= f)
        goto finis;
    }
    else
    {
      /* squeezing using upper and lower bounds on log(f(x)) */
      amaxp = (k / npq) * ((k * (k / 3. + 0.625) + 0.1666666666666) / npq + 0.5);
      ynorm = -k * k / (2.0 * npq);
      alv = log(v);
      if (alv < ynorm - amaxp)
        goto finis;
      if (alv <= ynorm + amaxp)
      {
        /* stirling's formula to machine accuracy */
        /* for the final acceptance/rejection test */
        x1 = ix + 1;
        f1 = fm + 1.0;
        z = n + 1 - fm;
        w = n - ix + 1.0;
        z2 = z * z;
        x2 = x1 * x1;
        f2 = f1 * f1;
        w2 = w * w;
        if (alv <= xm * log(f1 / x1) + (n - m + 0.5) * log(z / w) + (ix - m) * log(w * p / (x1 * q)) + (13860.0 - (462.0 - (132.0 - (99.0 - 140.0 / f2) / f2) / f2) / f2) / f1 / 166320.0 + (13860.0 - (462.0 - (132.0 - (99.0 - 140.0 / z2) / z2) / z2) / z2) / z / 166320.0 + (13860.0 - (462.0 - (132.0 - (99.0 - 140.0 / x2) / x2) / x2) / x2) / x1 / 166320.0 + (13860.0 - (462.0 - (132.0 - (99.0 - 140.0 / w2) / w2) / w2) / w2) / w / 166320.)
          goto finis;
      }
    }
  }

L_np_small:
  /*---------------------- np = n*p < 30 : ------------------------- */

  repeat
  {
    ix = 0;
    f = qn;
    u = dsfmt_genrand_close_open(&dsfmt);
    // printf("np small qn = %f, u = %f\n", qn, u);
    repeat
    {
      if (u < f)
        goto finis;
      if (ix > 110)
        break;
      u -= f;
      ix++;
      f *= (g / ix - r);
    }
  }
finis:
  if (psave > 0.5)
    ix = n - ix;
  // printf("ix = %d\n", ix);
  return ix;
}

/**
 * @brief Generate a random Normal number using Box-Muller transform.
 * @param mu the mean
 * @param sd the standard deviation
 * @return A Normally-distributed number drawn from a distribution with the give mean and standard deviation.
 */
double random_normal(float mu, float sd)
{
  double randU = dsfmt_genrand_close_open(&dsfmt);
  return mu + sd * (sqrt(-2 * log(randU)) * cos(2 * M_PI * randU));
}

/**
 * @brief Generate a random float between 0 and 1.
 */
double randomFloat()
{
  return dsfmt_genrand_close_open(&dsfmt);
}

/**
 * @brief Generate a random Normal number using Box-Muller transform.
 * @param mu the mean
 * @param sd the standard deviation
 * @return A Normally-distributed number drawn from a distribution with the give mean and standard deviation.
 */
int random_uniform(int min, int max)
{
  double minD = (double)min;
  double range = (double)(max)-minD;
  double randU = dsfmt_genrand_close_open(&dsfmt);

  int out = round(randU * range + minD);
  return out;
}

/**
 * @brief Given a set of weights, choose a random index.  Probablity of selection is proportional to the weights.
 * @param n The number of weights.
 * @param weights A pointer to an array of double weights. They do not need to sum to 1.0.
 * @return The index of the winning weight.
 */
int weightedRandomSample(int n, double *weights, double key)
{
  // Need an array of cumulative sums:
  double cumSums[n];

  cumSums[0] = weights[0];
  for (int i = 0; i < n; i++)
  {
    cumSums[i] = cumSums[i - 1] + weights[i];
  }

  // Generate a random number within the range of the cumulative sums.
  key = key * cumSums[n - 1];

  // Can't really use binary search because we aren't looking for an exact match.
  // Brute force, start at the lowest and stop when cumulative sum is higher than key.
  int index = 0;
  for (int i = 0; i < n; i++)
  {
    if (key <= cumSums[i])
      break;
    index++;
  }
  return index;
}

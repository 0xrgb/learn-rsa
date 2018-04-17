#include <stdio.h>
#include <time.h>
#include <gmp.h>

#define BIT_SIZE 2048
#define REPEAT_SIZE 1000000
#define RANDOM_SEED 0x1234567890abcdefUL

int main(int argc, char *argv[]) {
  mpz_t a, b, c;
  gmp_randstate_t state;

  mpz_init2(a, BIT_SIZE);
  mpz_init2(b, BIT_SIZE);
  mpz_init2(c, BIT_SIZE * 2);

  gmp_randinit_default(state);
  gmp_randseed_ui(state, RANDOM_SEED);

  // 0. mpz_randomb
  {
    clock_t start, end;
    double res;
    start = clock();

    for (int i = 0; i < REPEAT_SIZE; ++i) {
      mpz_urandomb(a, state, BIT_SIZE);
      mpz_urandomb(b, state, BIT_SIZE);
    }

    end = clock();
    res = (double)(end - start) / CLOCKS_PER_SEC;
    printf("[Default]: %f s\n", res);
  }

  // 1. mpz_add
  {
    clock_t start, end;
    double res;
    start = clock();

    for (int i = 0; i < REPEAT_SIZE; ++i) {
      mpz_urandomb(a, state, BIT_SIZE);
      mpz_urandomb(b, state, BIT_SIZE);
      mpz_add(c, a, b);
    }

    end = clock();
    res = (double)(end - start) / CLOCKS_PER_SEC;
    printf("[mpz_add]: %f s\n", res);
  }

  // 2. mpz_sub
  {
    clock_t start, end;
    double res;
    start = clock();

    for (int i = 0; i < REPEAT_SIZE; ++i) {
      mpz_urandomb(a, state, BIT_SIZE);
      mpz_urandomb(b, state, BIT_SIZE);
      mpz_sub(c, a, b);
    }

    end = clock();
    res = (double)(end - start) / CLOCKS_PER_SEC;
    printf("[mpz_sub]: %f s\n", res);
  }

  // 3. mpz_mul
  {
    clock_t start, end;
    double res;
    start = clock();

    for (int i = 0; i < REPEAT_SIZE; ++i) {
      mpz_urandomb(a, state, BIT_SIZE);
      mpz_urandomb(b, state, BIT_SIZE);
      mpz_mul(c, a, b);
    }

    end = clock();
    res = (double)(end - start) / CLOCKS_PER_SEC;
    printf("[mpz_mul]: %f s\n", res);
  }

  // 4. mpz_mul_2exp
  {
    clock_t start, end;
    double res;
    start = clock();

    for (int i = 0; i < REPEAT_SIZE; ++i) {
      mpz_urandomb(a, state, BIT_SIZE);
      mpz_urandomb(b, state, BIT_SIZE);
      mpz_mul_2exp(c, a, 777);
    }

    end = clock();
    res = (double)(end - start) / CLOCKS_PER_SEC;
    printf("[mpz_mul_2exp]: %f s\n", res);
  }

  // 5. mpz_inverse
  {
    clock_t start, end;
    double res;
    start = clock();

    for (int i = 0; i < REPEAT_SIZE; ++i) {
      mpz_urandomb(a, state, BIT_SIZE);
      mpz_urandomb(b, state, BIT_SIZE);
      if (mpz_cmp_ui(b, 0) == 0) continue;
      mpz_gcd(c, a, b);
      if (mpz_cmp_ui(c, 1) != 0) continue;
      mpz_invert(c, a, b);
    }

    end = clock();
    res = (double)(end - start) / CLOCKS_PER_SEC;
    printf("[mpz_invert]: %f s\n", res);
  }

  mpz_clears(a, b, c, NULL);
  gmp_randclear(state);
  return 0;
}

#include <stdio.h>
#include <time.h>
#include <gmp.h>

#define RANDOM_SEED 0x1234567890abcdefUL
#define BIT_SIZE 4096
#define REPEAT_SIZE 1000000

int main(int argc, char *argv[]) {
  mpz_t a, b;
  mpz_t c1, c2; // c = a * b
  mpz_t a0, a1, b0, b1;
  mpz_t tmp1, tmp2, tmp3, tmp4;
  gmp_randstate_t state;

  mpz_inits(a, b, c1, c2, a0, a1, b0, b1, tmp1, tmp2, tmp3, tmp4, NULL);
  gmp_randinit_default(state);
  gmp_randseed_ui(state, RANDOM_SEED);

  // Check time
  // (1) mpz_mul
  {
    clock_t start, end;
    double res;
    start = clock();

    for (int i = 0; i < REPEAT_SIZE; ++i) {
      mpz_urandomb(a, state, BIT_SIZE);
      mpz_urandomb(b, state, BIT_SIZE);
      mpz_mul(c1, a, b);
    }

    end = clock();
    res = (double)(end - start) / CLOCKS_PER_SEC;
    printf("[mpz_mul]: %f s\n", res);
  }

  // (2) Karatsuba 1-step
  {
    clock_t start, end;
    double res;
    start = clock();

    for (int i = 0; i < REPEAT_SIZE; ++i) {
      mpz_urandomb(a, state, BIT_SIZE);
      mpz_urandomb(b, state, BIT_SIZE);

      mpz_tdiv_r_2exp(a0, a, BIT_SIZE / 2);
      mpz_tdiv_q_2exp(a1, a, BIT_SIZE / 2);
      mpz_tdiv_r_2exp(b0, b, BIT_SIZE / 2);
      mpz_tdiv_q_2exp(b1, b, BIT_SIZE / 2);

      mpz_mul(tmp1, a0, b0);
      mpz_mul(tmp2, a1, b1);
      mpz_add(tmp3, a0, a1);
      mpz_add(tmp4, b0, b1);
      mpz_mul(tmp3, tmp3, tmp4);
      mpz_sub(tmp3, tmp3, tmp1);
      mpz_sub(tmp3, tmp3, tmp2);

      mpz_set(c2, tmp2);
      mpz_mul_2exp(c2, c2, BIT_SIZE / 2);
      mpz_add(c2, c2, tmp3);
      mpz_mul_2exp(c2, c2, BIT_SIZE / 2);
      mpz_add(c2, c2, tmp1);
    }

    end = clock();
    res = (double)(end - start) / CLOCKS_PER_SEC;
    printf("[Custom ]: %f s\n", res);
  }

  mpz_clears(a, b, c1, c2, a0, a1, b0, b1, tmp1, tmp2, tmp3, tmp4, NULL);
  gmp_randclear(state);
  return 0;
}

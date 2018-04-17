#include <stdio.h>
#include <assert.h>
#include <gmp.h>

void powmod_normal(mpz_t, const mpz_t, const mpz_t, const mpz_t);
void powmod_montgomery(mpz_t, const mpz_t, const mpz_t, const mpz_t);

int main(int argc, char *argv[]) {
  mpz_t a, b, m;
  mpz_t c1, c2;
  gmp_randstate_t state;

  mpz_inits(a, b, m, c1, c2, NULL);
  gmp_randinit_default(state);

  mpz_set_str(m,
    "13698031845839681324328863941416858895212053818948"
    "36900690043939577649957413827537479986216755901817"
    "99928491177850961633103924955359287791970608284439"
    "51949471681869403286332789160304218068454389850776"
    "01264030944684656537615523805322490517494926041664"
    "52495487230110529227753993037579156704301830605049"
    "438139121", 10); // It is prime below 2^1024
  mpz_urandomm(a, state, m);
  mpz_urandomb(b, state, 1024);

  // c = a ^ b mod m
  // When (a, m) = 1 && a < m && b > 0
  powmod_normal    (c1, a, b, m);
  powmod_montgomery(c2, a, b, m);

  // Assert
  // TODO: 시간 비교
  assert(mpz_cmp(c1, c2) == 0);
  gmp_printf(
    "a  = %Zx\n"
    "b  = %Zx\n"
    "m  = %Zx\n"
    "c1 = %Zx\n"
    "c2 = %Zx\n", a, b, m, c1, c2);

  mpz_clears(a, b, m, c1, c2, NULL);
  gmp_randclear(state);
  return 0;
}

void powmod_normal(mpz_t result, const mpz_t a, const mpz_t b, const mpz_t m) {
  const int B_SIZE = mpz_sizeinbase(b, 2);

  mpz_set(result, a);
  for (int i = B_SIZE - 2; i >= 0; --i) {
    mpz_mul(result, result, result);
    mpz_fdiv_r(result, result, m);
    if (mpz_tstbit(b, i)) {
      mpz_mul(result, result, a);
      mpz_fdiv_r(result, result, m);
    }
  }
}

void powmod_montgomery(mpz_t result, const mpz_t a, const mpz_t b, const mpz_t m) {
  mpz_t r, r_inv, m_;
  mpz_t a_;
  mpz_t T, Um;
  const int M_SIZE = mpz_sizeinbase(m, 2);
  const int B_SIZE = mpz_sizeinbase(b, 2);

  mpz_inits(r, r_inv, m_, a_, T, Um, NULL);

  // 1. Create R > m
  mpz_setbit(r, M_SIZE);

  // 2. Pre-calculate inverses
  mpz_invert(r_inv, r, m);
  mpz_invert(m_, m, r);
  mpz_sub(m_, r, m_);

  mpz_mul_2exp(a_, a, M_SIZE); // a' = a * R = a * 2 ^ M_SIZE
  mpz_fdiv_r(a_, a_, m);

  // 3. Calculate power
  mpz_set(T, a_);
  for (int i = B_SIZE - 2; i >= 0; --i) {
    // T <- T * T
    mpz_mul(T, T, T);
    mpz_mul(Um, T, m_);
    mpz_fdiv_r_2exp(Um, Um, M_SIZE);
    mpz_mul(Um, Um, m);
    mpz_add(T, T, Um);
    mpz_fdiv_q_2exp(T, T, M_SIZE);
    if (mpz_cmp(T, m) > 0) mpz_sub(T, T, m);

    // T <- T * a'
    if (mpz_tstbit(b, i)) {
      mpz_mul(T, T, a_);
      mpz_mul(Um, T, m_);
      mpz_fdiv_r_2exp(Um, Um, M_SIZE);
      mpz_mul(Um, Um, m);
      mpz_add(T, T, Um);
      mpz_fdiv_q_2exp(T, T, M_SIZE);
      if (mpz_cmp(T, m) > 0) mpz_sub(T, T, m);
    }
  }

  // 4. Revert montgomery reduction
  mpz_mul(result, T, r_inv);
  mpz_fdiv_r(result, result, m);

  // Clear all
  mpz_clears(r, r_inv, m_, a_, T, Um, NULL);
}

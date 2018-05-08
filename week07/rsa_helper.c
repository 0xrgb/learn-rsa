#include "rsa.h"

void rsa_add_mod(mpz_t rop, const mpz_t a, const mpz_t b, const mpz_t n) {
  mpz_add(rop, a, b);
  mpz_mod(rop, rop, n);
}

void rsa_mul_mod(mpz_t rop, const mpz_t a, const mpz_t b, const mpz_t n) {
  mpz_mul(rop, a, b);
  mpz_mod(rop, rop, n);
}

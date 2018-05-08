#include "rsa.h"

void rsa_pub_exp(mpz_t out, const mpz_t in, const RSA_PUBKEY *pub) {
  // in^e mod n = out
  // Case 1. e == 0x10001
  if (mpz_cmp_ui(pub->e, 0x10001)) {
    mpz_set(out, in);
    for (int i = 0; i < 16; ++i) {
      rsa_mul_mod(out, out, out, pub->n);
    }
    rsa_mul_mod(out, out, in, pub->n);
    return;
  }

  // Case 2. General case
  mpz_powm(out, in, pub->e, pub->n);
}

#ifndef NO_RSA_CRT
void rsa_pri_exp(mpz_t out, const mpz_t in, const RSA_PRIKEY *pri) {
  mpz_t x, y;
  mpz_inits(x, y, NULL);
  mpz_powm(x, in, pri->d, pri->p);
  mpz_powm(y, in, pri->d, pri->q);

  mpz_sub(x, x, y);
  mpz_mul(x, x, pri->qi);
  mpz_addmul(y, x, pri->q);
  mpz_mod(out, y, pri->n);
  mpz_clears(x, y, NULL);
}
#else
void rsa_pri_exp(mpz_t out, const mpz_t in, const RSA_PRIKEY *pri) {
  mpz_powm(out, in, pri->d, pri->n);
}
#endif

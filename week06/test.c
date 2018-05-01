#include <stdio.h>

#define NO_RSA_CRT
#include "rsa.h"

inline void mpz_mul_mod(mpz_t rop, const mpz_t a, const mpz_t b, const mpz_t n) {
  mpz_mul(rop, a, b);
  mpz_mod(rop, rop, n);
}

void mpz_pow_mod(mpz_t rop, const mpz_t a, const mpz_t b, const mpz_t n) {
  const int B_SIZE = mpz_sizeinbase(b, 2);
  mpz_set(rop, a);
  for (int i = B_SIZE - 2; i >= 0; --i) {
    mpz_mul_mod(rop, rop, rop, n);
    if (mpz_tstbit(b, i)) mpz_mul_mod(rop, rop, a, n);
  }
}

RSA_PUBKEY pub;
RSA_PRIKEY pri;

int main() {
  gmp_randstate_t rnd;
  gmp_randinit_default(rnd);

  rsa_key_init(&pub, &pri);
  rsa_key_gen(&pub, &pri, 2048, rnd);

  // (1) Print keys
  gmp_printf("N = %Zx\n", pub.n);
  gmp_printf("p = %Zx\nq = %Zx\n", pri.p, pri.q);
  gmp_printf("e = %Zx\nd = %Zx\n", pub.e, pri.d);

  // (2) Test encryption / Decryption
  mpz_t testmsg, tmp, tmp2;
  mpz_inits(testmsg, tmp, tmp2, NULL);
  mpz_set_ui(testmsg, 0x12345678);
  mpz_pow_mod(tmp, testmsg, pub.e, pub.n);
  mpz_pow_mod(tmp2, tmp, pri.d, pri.n);
  gmp_printf("Original MSG: %Zx\nFinal MSG   : %Zx\n", testmsg, tmp2);

  rsa_key_clear(&pub, &pri);

  gmp_randclear(rnd);
  return 0;
}

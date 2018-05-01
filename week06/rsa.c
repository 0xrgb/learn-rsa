#include "rsa.h"

// Pre-declared
#define RSA_SIZE_MULTIPLER 1024
#define MAX_RSA_SIZE       4096

// -1 is for unsupported rsa size
const static int RSA_MR_ITER[MAX_RSA_SIZE / RSA_SIZE_MULTIPLER + 1][2] = {
  {-1, -1},
  {-1, -1}, // 1024
  {56, 56}, // 2048
  {-1, -1}, // 3072
  {-1, -1}, // 4096
};

// Helper function
static void prime_gen(mpz_t p, int psize, int mriter, gmp_randstate_t rnd){
  for (;;) {
    mpz_urandomb(p, rnd, psize);
    mpz_setbit(p, 0);         // odd
    mpz_setbit(p, psize - 1); // p >= 2 ^ (psize - 1)
    if (mpz_millerrabin(p, mriter) != 0) break;
  }
}

void rsa_key_init(RSA_PUBKEY *pub, RSA_PRIKEY *pri) {
  // pubkey
  mpz_inits(pub->n, pub->e, NULL);
  pub->RSA_SIZE = 0;
  // prikey
  mpz_inits(pri->p, pri->q, pri->d, pri->n, pri->e, NULL);
  pri->RSA_SIZE = 0;
#ifndef NO_RSA_CRT
  mpz_inits(pri->dp, pri->dq, pri->qi, NULL);
#endif
}

void rsa_key_clear(RSA_PUBKEY *pub, RSA_PRIKEY *pri) {
  // pubkey
  mpz_clears(pub->n, pub->e, NULL);
  // prikey
  mpz_clears(pri->p, pri->q, pri->d, pri->n, pri->e, NULL);
#ifndef NO_RSA_CRT
  mpz_clears(pri->dp, pri->dq, pri->qi, NULL);
#endif
}

int rsa_key_gen(RSA_PUBKEY *pub, RSA_PRIKEY *pri, int size, gmp_randstate_t rnd) {
  int mriter[2];
  mpz_t tmp;
  // Private key generation
  // 1. RSA_SIZE check
  //    Calculate Miller-Rabin iteration number
  //    e = 0x10001
  if (size <= 0 || size > MAX_RSA_SIZE || size % RSA_SIZE_MULTIPLER != 0) return -1;
  pri->RSA_SIZE = size;
  mriter[0] = RSA_MR_ITER[size / RSA_SIZE_MULTIPLER][0];
  mriter[1] = RSA_MR_ITER[size / RSA_SIZE_MULTIPLER][1];
  if (mriter[0] == -1 || mriter[1] == -1) return -1;
  mpz_set_ui(pri->e, 0x10001);
  // 2. Create p
  mpz_init(tmp);
  for (;;) {
    prime_gen(pri->p, size / 2, mriter[0], rnd);
    mpz_gcd(tmp, pri->p, pri->e);
    if (mpz_cmp_ui(tmp, 1) != 0) continue;
    break;
  }
  // 3. Create q
  for (;;) {
    prime_gen(pri->q, size / 2, mriter[1], rnd);
    mpz_gcd(tmp, pri->q, pri->e);
    if (mpz_cmp_ui(tmp, 1) != 0) continue;
    if (mpz_cmp(pri->p, pri->q) == 0) continue;
    break;
  }
  // 4. d := Inverse[e, phi(N)]
  mpz_sub_ui(pri->p, pri->p, 1);
  mpz_sub_ui(pri->q, pri->q, 1);
  mpz_mul(tmp, pri->p, pri->q);
  mpz_invert(pri->d, pri->e, tmp);
#ifndef NO_RSA_CRT
  // dp := d mod (p - 1)
  mpz_mod(pri->dp, pri->d, pri->p);
  // dq := d mod (q - 1)
  mpz_mod(pri->dq, pri->d, pri->q);
#endif
  mpz_add_ui(pri->p, pri->p, 1);
  mpz_add_ui(pri->q, pri->q, 1);
  // 5. N := pq
  mpz_mul(pri->n, pri->p, pri->q);
  mpz_clear(tmp);
#ifndef NO_RSA_CRT
  // qi := Inverse[q, p]
  mpz_invert(pri->qi, pri->q, pri->p);
#endif
  // Public key generation
  // 1. RSA_SIZE, e, n
  pub->RSA_SIZE = pri->RSA_SIZE;
  mpz_set(pub->e, pri->e);
  mpz_set(pub->n, pri->n);
  return 0;
}

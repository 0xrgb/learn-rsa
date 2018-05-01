#ifndef __RSA_H__
#define __RSA_H__

#include <stdlib.h>
#include <gmp.h>

typedef struct __RSA_PUBKEY {
  mpz_t n;
  mpz_t e;
  int RSA_SIZE;
} RSA_PUBKEY;

typedef struct __RSA_PRIKEY {
  mpz_t p;
  mpz_t q;
  mpz_t d;
  mpz_t n;
  mpz_t e;
  int RSA_SIZE;
#ifndef NO_RSA_CRT
  mpz_t dp;
  mpz_t dq;
  mpz_t qi;
#endif
} RSA_PRIKEY;

void rsa_key_init (RSA_PUBKEY*, RSA_PRIKEY*);
void rsa_key_clear(RSA_PUBKEY*, RSA_PRIKEY*);
int  rsa_key_gen  (RSA_PUBKEY*, RSA_PRIKEY*, int, gmp_randstate_t);

#endif

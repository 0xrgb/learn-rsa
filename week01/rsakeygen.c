#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>

#define DEFAULT_KEY_SIZE 2048
#define RANDOM_SEED 0x12345
#define PRIME_CHECK_REPS 50
#define MPZ_SIZE (sizeof(unsigned long) * 8)

typedef struct {
  mpz_t N;       // Public
  unsigned long e;
  mpz_t p, q, d; // Private
} RSAKey;

void usage();
RSAKey createRSAKey(gmp_randstate_t, unsigned long);
void generatePrime(gmp_randstate_t, mpz_t, unsigned long, unsigned long);
void clearRSAKey(RSAKey);

int main(int argc, char *argv[]) {
  int keySize;
  gmp_randstate_t rState;

  // Init
  if (argc > 2) usage();
  else if (argc == 2) keySize = atoi(argv[1]);
  else keySize = DEFAULT_KEY_SIZE;

  if (keySize < 0 || keySize % 128 != 0) usage();

  gmp_randinit_default(rState);
  gmp_randseed_ui(rState, RANDOM_SEED);

  RSAKey k = createRSAKey(rState, keySize);
  gmp_printf("N = %Zd\n\n", k.N);
  gmp_printf("p = %Zd\nq = %Zd\n\nd = %Zd\n", k.p, k.q, k.d);

  // Clear
  gmp_randclear(rState);
  clearRSAKey(k);
  return 0;
}

void usage() {
  puts("Usage: rsakeygen <KEY_SIZE>");
  exit(0);
}

RSAKey createRSAKey(gmp_randstate_t rState, unsigned long keySize) {
  RSAKey k;

  mpz_init(k.N);
  mpz_init(k.p);
  mpz_init(k.q);
  mpz_init(k.d);

  k.e = 0x10001;

  generatePrime(rState, k.p, k.e, keySize / 2);
  do {
    generatePrime(rState, k.q, k.e, keySize / 2);
  } while (mpz_cmp(k.p, k.q) == 0);

  // Nasty hack
  k.p->_mp_d[0] &= ~1UL;
  k.q->_mp_d[0] &= ~1UL;
  mpz_mul(k.N, k.p, k.q);
  mpz_set_ui(k.d, k.e);
  mpz_invert(k.d, k.d, k.N);
  k.p->_mp_d[0] |= 1UL;
  k.q->_mp_d[0] |= 1UL;
  mpz_mul(k.N, k.p, k.q);

  return k;
}

void generatePrime(gmp_randstate_t rState, mpz_t p, unsigned long e, unsigned long primeSize) {
  unsigned long x;
  for (;;) {
    mpz_urandomb(p, rState, primeSize);
    // Nasty hack (64bit)
    p->_mp_d[0] |= 1;
    p->_mp_d[primeSize / MPZ_SIZE - 1] |= (1UL << (MPZ_SIZE - 1));
    if (mpz_probab_prime_p(p, PRIME_CHECK_REPS) == 0) continue;
    // Nasty hack
    p->_mp_d[0] &= ~1UL;
    if (mpz_gcd_ui(NULL, p, e) > 1) continue;
    p->_mp_d[0] |= 1UL;
    break;
  }
}

void clearRSAKey(RSAKey k) {
  mpz_clear(k.N);
  mpz_clear(k.p);
  mpz_clear(k.q);
  mpz_clear(k.d);
}

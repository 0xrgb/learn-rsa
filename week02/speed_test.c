#include <stdio.h>
#include <assert.h>
#include <gmp.h>

// [TIME MACRO START]
#include <time.h>
clock_t _watch_elapsed;
double _watch_sec;
#define START_WATCH do{\
    _watch_elapsed = -clock();\
  }while(0)
#define STOP_WATCH do{\
    _watch_elapsed += clock();\
    _watch_sec = (double)_watch_elapsed/CLOCKS_PER_SEC;\
  }while(0)
#define PRINT_TIME do{\
    printf("[%.5f s]\n",_watch_sec);\
  }while(0)
// [TIME MACRO END]

typedef unsigned long MPZ_TYPE;
#define MPZ_SIZE (sizeof(MPZ_TYPE) * 8)

void generatePrime(gmp_randstate_t, mpz_t, MPZ_TYPE, MPZ_TYPE, int);
void RSASpeedTest(gmp_randstate_t, MPZ_TYPE);

int main(int argc, char *argv[]) {
  gmp_randstate_t state;
  gmp_randinit_default(state);
  gmp_randseed_ui(state, (MPZ_TYPE)time(NULL));

  RSASpeedTest(state, 1024);
  RSASpeedTest(state, 2048);
  RSASpeedTest(state, 3072);
  RSASpeedTest(state, 4096);

  gmp_randclear(state);
  return 0;
}

void generatePrime(gmp_randstate_t rState, mpz_t p, MPZ_TYPE e, MPZ_TYPE primeSize, int reps) {
  MPZ_TYPE x;
  for (;;) {
    mpz_urandomb(p, rState, primeSize);
    // Nasty hack
    p->_mp_d[0] |= 1;
    p->_mp_d[primeSize / MPZ_SIZE - 1] |= (1UL << (MPZ_SIZE - 1));
    if (mpz_probab_prime_p(p, reps) == 0) continue;
    // Nasty hack
    p->_mp_d[0] &= ~1UL;
    if (mpz_gcd_ui(NULL, p, e) > 1) continue;
    p->_mp_d[0] |= 1UL;
    break;
  }
}

// RSA-1024 ==  80bit | reps 40
// RSA-2048 == 112bit | reps 56
// RSA-3072 == 128bit | reps 64
// RSA-4096 == 192bit | reps 96
void RSASpeedTest(gmp_randstate_t state, MPZ_TYPE keySize) {
  mpz_t N, p, q, d;
  MPZ_TYPE e = 0x10001;
  int reps;

  assert(keySize > 0 && keySize % 1024 == 0);
  reps = keySize / 1024 * 16 + 24; // TODO: Change reps

  mpz_inits(N, p, q, d, NULL);

  START_WATCH;
  generatePrime(state, p, e, keySize / 2, reps);
  do {
    generatePrime(state, q, e, keySize / 2, reps);
  } while (mpz_cmp(p, q) == 0);

  // Nasty hack
  p->_mp_d[0] &= ~1UL;
  q->_mp_d[0] &= ~1UL;
  mpz_mul(N, p, q);
  mpz_set_ui(d, e);
  mpz_invert(d, d, N);
  p->_mp_d[0] |= 1UL;
  q->_mp_d[0] |= 1UL;
  mpz_mul(N, p, q);

  STOP_WATCH;
  PRINT_TIME;
  gmp_printf("Size: %d, Reps: %d\n", (int)keySize, reps);

  mpz_clears(N, p, q, d, NULL);
}

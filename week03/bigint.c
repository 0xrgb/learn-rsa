#include <stdio.h>
#include <time.h>
#include <gmp.h>

// Low level custom functions
// Assuming that c has enough spaces

// c <- |a| + |b|
void mpz_add_absX(mpz_t c, const mpz_t a, const mpz_t b) {
  mpz_t aa, bb; // size(aa) >= size(bb)
  mp_size_t i;
  mp_limb_t carry = 0;

  mpz_inits(aa, bb, NULL);
  // mpz_size(X) == |X->_mp_size|
  if (mpz_size(a) >= mpz_size(b)) {
    mpz_set(aa, a);
    mpz_set(bb, b);
  } else {
    mpz_set(aa, b);
    mpz_set(bb, a);
  }

  // Add
  for (i = 0; i < mpz_size(bb); ++i) {
    c->_mp_d[i] = aa->_mp_d[i] + bb->_mp_d[i] + carry;
    carry       = (carry
      ? (aa->_mp_d[i] >= ~bb->_mp_d[i])
      : (aa->_mp_d[i] >    c->_mp_d[i]));
  }
  for ( ; i < mpz_size(aa); ++i) {
    c->_mp_d[i] = aa->_mp_d[i] + carry;
    carry       =  c->_mp_d[i] < carry;
  }

  // MSD check
  if (carry) {
    c->_mp_d[i] = 1;
    c->_mp_size = i + 1;
  } else {
    c->_mp_size = i;
  }

  mpz_clears(aa, bb, NULL);
}

// c <- |a| - |b|
void mpz_sub_absX(mpz_t c, const mpz_t a, const mpz_t b) {
  mpz_t aa, bb; // aa >= bb
  mp_size_t i;
  mp_limb_t borrow = 0;
  int isPlus;

  mpz_inits(aa, bb, NULL);
  isPlus = mpz_cmpabs(a, b);
  if (isPlus >= 0) {
    mpz_set(aa, a);
    mpz_set(bb, b);
  } else {
    mpz_set(aa, b);
    mpz_set(bb, a);
  }

  for (i = 0; i < mpz_size(bb); ++i) {
    c->_mp_d[i] = aa->_mp_d[i] - bb->_mp_d[i] - borrow;
    borrow      = (borrow
      ? (aa->_mp_d[i] <= bb->_mp_d[i])
      : (aa->_mp_d[i] <   c->_mp_d[i]));
  }

  for ( ; i < mpz_size(aa); ++i) {
    c->_mp_d[i] = aa->_mp_d[i] - borrow;
    borrow      = aa->_mp_d[i] < c->_mp_d[i];
  }

  //if (borrow) CANNOT_REACH_HERE;
  // MSD Check
  for (i = mpz_size(aa); i--; ) {
    if (c->_mp_d[i]) break;
  }
  ++i;
  c->_mp_size = (isPlus >= 0 ? i : -i);

  mpz_clears(aa, bb, NULL);
}

// High level custom functions

// c <- a + b
void mpz_addX(mpz_t c, const mpz_t a, const mpz_t b) {
  mp_size_t need;
  if (a->_mp_size == 0) {
    mpz_set(c, b);
    return;
  } else if (b->_mp_size == 0) {
    mpz_set(c, a);
    return;
  }

  // Alloc memory generously
  // Check and realloc c
  need = 1 + (mpz_size(a) > mpz_size(b) ? mpz_size(a) : mpz_size(b));
  if (c->_mp_alloc < need) mpz_realloc2(c, need * sizeof(unsigned long) * 8);

  // mpz_sgn(X) == sign of X->_mp_size == sign of X
  // It returns -1, 0, or +1.
  // This code can handle 4 cases (not 9), because we eliminated some cases above.
  if (mpz_sgn(a) == mpz_sgn(b)) mpz_add_absX(c, a, b);
  else mpz_sub_absX(c, a, b);
  c->_mp_size *= mpz_sgn(a);
}

// c <- a - b
void mpz_subX(mpz_t c, const mpz_t a, const mpz_t b) {
  mpz_t bb;
  mpz_init(bb);
  mpz_set(bb, b);
  bb->_mp_size *= -1;
  mpz_addX(c, a, bb);
  mpz_clear(bb);
}

#define RANDOM_SEED (0x1234567890abcdefUL)
#define REPEAT_TIME (100000000)
#define BIT_SIZE (128)

int main(int argc, char *argv[]) {
  mpz_t a, b, c, d;
  gmp_randstate_t state;

  mpz_inits(a, b, c, d, NULL);
  gmp_randinit_default(state);
  gmp_randseed_ui(state, RANDOM_SEED);

  // Assertive check
  for (int i = 0; i < REPEAT_TIME; ++i) {
    mpz_urandomb(a, state, BIT_SIZE);
    mpz_urandomb(b, state, BIT_SIZE);
    mpz_sub(c, a, b);
    mpz_subX(d, a, b);
    if (mpz_cmp(c, d) != 0) {
      gmp_printf("mpz_subX error\nX = %Zd\nY = %Zd\n", a, b);
      return -1;
    }

    mpz_sub(c, b, a);
    mpz_subX(d, b, a);
    if (mpz_cmp(c, d) != 0) {
      gmp_printf("mpz_subX error\nX = %Zd\nY = %Zd\n", b, a);
      return -1;
    }

    mpz_add(c, a, b);
    mpz_addX(d, a, b);
    if (mpz_cmp(c, d) != 0) {
      gmp_printf("mpz_addX error\nX = %Zd\nY = %Zd\n", a, b);
      return -1;
    }
  }

  // Speed check
  clock_t start, end;
  double res;

  // (1) Original function speed check
  start = clock();
  for (int i = 0; i < REPEAT_TIME; ++i) mpz_add(c, a, b);
  end = clock();
  res = (double)(end - start) / CLOCKS_PER_SEC;
  printf("[mpz_add ]: %f s\n", res);

  // (2) Custom function speed check
  start = clock();
  for (int i = 0; i < REPEAT_TIME; ++i) mpz_addX(c, a, b);
  end = clock();
  res = (double)(end - start) / CLOCKS_PER_SEC;
  printf("[mpz_addX]: %f s\n", res);

  // Clear
  mpz_clears(a, b, c, d, NULL);
  gmp_randclear(state);
  return 0;
}

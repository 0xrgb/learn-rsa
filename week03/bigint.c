#include <stdio.h>
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
  if (c->_mp_alloc < need) mpz_realloc2(c, need * sizeof(mp_limb_t));

  // mpz_sgn(X) == sign of X->_mp_size == sign of X
  // It returns -1, 0, or +1.
  // This code can handle 4 cases (not 9), because we eliminated some cases above.
  if (mpz_sgn(a) == mpz_sgn(b)) mpz_add_absX(c, a, b);
  else mpz_sub_absX(c, a, b);
  c->_mp_size *= mpz_sgn(a);
}

// c <- a - b
void mpz_subX(mpz_t c, const mpz_t a, const mpz_t b) {
  // TODO
}

int main(int argc, char *argv[]) {
  mpz_t a, b, c;
  mpz_inits(a, b, c, NULL);
  // TODO - Test custom functions (Speed / Error)
  mpz_clears(a, b, c, NULL);
  return 0;
}

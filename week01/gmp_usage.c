#include <stdio.h>
#include <gmp.h>

int main(void) {
  mpz_t a, b, c;
  mpz_init(a);
  mpz_init(b);
  mpz_init(c);

  mpz_set_str(a, "999999999999999999", 10); // 값을 넣어준다

  // (1) gmp_printf
  gmp_printf("Base 10 : %Zd\n", a);
  gmp_printf("Base 16 : %Zx\n", a);

  // (2) mpz_out_str
  printf("Base 10 : ");
  mpz_out_str(stdout, 10, a);
  printf("\nBase 16 : ");
  mpz_out_str(stdout, -16, a); // +: lowercase, -: uppercase
  printf("\n");

  // (3) add, mul, sub, div
  mpz_mul(a, a, a);
  gmp_printf("Squared : %Zd\n", a);

  mpz_add(a, a, a);
  gmp_printf("Added   : %Zd\n", a);

  mpz_divexact_ui(b, a, 162); // 확실히 나누어 떨어질 때만 사용
  gmp_printf("Divexact: %Zd\n", b);

  mpz_fdiv_q_ui(c, b, 13333);
  gmp_printf("Div     : %Zd\n", c);

  // Clear
  mpz_clear(a);
  mpz_clear(b);
  mpz_clear(c);

  return 0;
}

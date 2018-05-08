#include <stdio.h>

#include "rsa.h"

RSA_PUBKEY pub;
RSA_PRIKEY pri;

int main() {
  gmp_randstate_t rnd;
  gmp_randinit_default(rnd);

  rsa_key_init(&pub, &pri);
  rsa_key_gen(&pub, &pri, 2048, rnd);

  // (2) Test encryption / Decryption
  mpz_t testmsg, tmp, tmp2;
  mpz_inits(testmsg, tmp, tmp2, NULL);
  mpz_set_ui(testmsg, 0x12345678);

  rsa_pub_exp(tmp, testmsg, &pub);
  rsa_pri_exp(tmp2, tmp, &pri);

  gmp_printf("Original MSG: %Zx\nFinal MSG   : %Zx\n", testmsg, tmp2);

  rsa_key_clear(&pub, &pri);
  gmp_randclear(rnd);
  return 0;
}

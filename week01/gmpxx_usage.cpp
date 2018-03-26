#include <iostream>
#include <gmpxx.h>

int main() {
  using namespace std;
  mpz_class a{"999999999999999"};
  cout << a << endl;

  mpz_class b = a * a;
  cout << b << endl;

  b += b;
  cout << b << endl;

  b /= 182;
  cout << b << endl;

  return 0;
}

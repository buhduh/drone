//compile with: g++ -S template.cc
#include <stdio.h>

template <int n>
class Fact {
public:
  static const int val = Fact<n-1>::val * n;
};

class Fact<0> { public: static const int val = 1; };

int main() {
  printf("fact 4 = %d\n", Fact<4>::val);
  printf("fact 5 = %d\n", Fact<5>::val);
  printf("fact 6 = %d\n", Fact<6>::val);
  printf("fact 7 = %d\n", Fact<7>::val);

  return 0;
}

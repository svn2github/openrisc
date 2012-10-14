// Test vector: performance difference 25% (0.444s vs 0.539s)
// Advice: set tmp as 10000

// { dg-options " -DVEC_ITER=200" { target { or32-*-elf } } }

#ifndef VEC_ITER
#define VEC_ITER 2000
#endif

#include <vector>

using std::vector;

int main()
{
  vector <int> tmp;

  for (int j=0; j<VEC_ITER; j++)
    // Insert more than default item
    for (int i=0; i<(5*VEC_ITER); i++) {
      tmp.push_back(i);
    }
}


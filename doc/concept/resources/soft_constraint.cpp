#include <crave/Constraint.hpp>
#include <crave/Context.hpp>
#include <stdio.h>
void soft_constraint()
{
  using namespace crave;
  Variable<unsigned> a, b;
  Generator<> gen ( a < b && b==7);

  gen(soft)( a > 4  );
  soft(gen)( a == 12);
  gen();

  gen[a]; gen[b];
}

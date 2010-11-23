#include <platzhalter/Constraint.hpp>
#include <platzhalter/Context.hpp>
#include <stdio.h>
void soft_constraint()
{
  using namespace platzhalter;
  Variable<unsigned> a, b;
  Generator<> gen ( a < b && b==7);

  gen(soft)( a > 4  );
  soft(gen)( a == 12);
  gen();

  gen[a]; gen[b];
}

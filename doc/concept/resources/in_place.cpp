#include <crave/Constraint.hpp>
#include <crave/Context.hpp>
#include <stdio.h>
void in_place()
{
  using namespace crave;
  Variable<unsigned> a, b, c;
  Generator<> gen;

  gen (a * b == c) 
      (a+b+c==0) 
      (a != 0)
      (b != 0)
      ();
  gen[a]; gen[b]; gen[c];
}

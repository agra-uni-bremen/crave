#include <platzhalter/Constraint.hpp>
#include <platzhalter/Context.hpp>
#include <stdio.h>
void reference()
{
  // test
  for (unsigned i = 0; i <= 100; ++i) {
  	
  using namespace platzhalter;
  unsigned lower=0, upper=100, pivot;

  Variable<unsigned> X;
  Generator<> gen ;
  gen  ( X = /* some function */i )
    ( X <= reference(pivot) );

  while( lower < upper )  {
    pivot = (upper+lower)/2;
    if(gen.next()) 
      upper = gen[X];
    else
      lower = pivot+1;
  }
  printf("%d %d\n",lower, upper);
  }
}

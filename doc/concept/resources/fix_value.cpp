#include <platzhalter/Constraint.hpp>
#include <platzhalter/Context.hpp>
#include <stdio.h>
void fix_value()
{
  using namespace platzhalter;
  Variable<unsigned> a, b;

  Generator<> gen (a < b);
  gen ( b = 7 ) ;

  while( gen.next() ) {
    printf("a: %d, b: %d\n"
      ,gen[a], gen[b]);
    gen( a != gen[a] && b != gen[b] );
  }
  gen( b = randomize );
  // ...
}

#include <crave/Constraint.hpp>
#include <crave/Context.hpp>
#include <stdio.h>
void fix_value()
{
  using namespace crave;
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

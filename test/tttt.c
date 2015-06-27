#include <stdio.h>

typedef struct {
  int s;
  int t;
} s;

main()
{
  s s;

  s.s = 0;
  s.t = 344;

  printf("%d\n", ++s.s);
  printf("%d\n", s.t);
  printf("%d\n", ++s.s);
}

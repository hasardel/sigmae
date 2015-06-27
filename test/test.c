#include <stdio.h>
#include <stdlib.h>

main()
{
  int a;
  int b;
  printf("&a:%08x\n", &a);
  printf("&b:%08x\n", &b);

  int c;
  printf("&c:%08x\n", &c);

  printf("%08x\n", malloc(256));
  printf("%08x\n", malloc(256));
  printf("%08x\n", malloc(256));
  printf("%08x\n", malloc(256));
  printf("%08x\n", malloc(256));
  printf("%08x\n", malloc(256));
  printf("%08x\n", malloc(256));
  printf("%08x\n", malloc(256));
  printf("%08x\n", malloc(256));
  printf("%08x\n", malloc(256));
  printf("%08x\n", malloc(256));
  printf("%08x\n", malloc(256));
  printf("%08x\n", malloc(256));
  printf("%08x\n", malloc(256));
  printf("%08x\n", malloc(256));
  printf("%08x\n", malloc(256));
  printf("%08x\n", malloc(256));
  printf("%08x\n", malloc(256));
  printf("%08x\n", malloc(256));
  printf("%08x\n", malloc(256));
  printf("%08x\n", malloc(256));
  printf("%08x\n", malloc(256));
  printf("%08x\n", malloc(256));

}
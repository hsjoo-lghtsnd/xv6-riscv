#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  uint64 count = getreadcount();
  printf("Read count: %d\n", count);
  exit(0);
}

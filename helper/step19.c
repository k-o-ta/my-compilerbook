#include <stdlib.h>

void malloc4(int **p, int arg0, int arg1, int arg2, int arg3 ) {
  *p = (int*)malloc(sizeof(int) * 4);
  (*p)[0] = arg0;
  (*p)[1] = arg1;
  (*p)[2] = arg2;
  (*p)[3] = arg3;
}

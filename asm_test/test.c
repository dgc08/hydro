/* #include "test.h" */
#include <stdio.h>

int glb;

void modi() {
  glb = 2;
}

int main(int argc, char** argv ) {
  int lc_main = glb;
  lc_main = lc_main + 1;
  glb = lc_main;
  modi();
  printf("%d", glb);
}

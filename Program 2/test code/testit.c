/* testit_trace.c */
#include <stdio.h>
int Add6(int in);
void main()
{
  char* temp = "CS344";
  int i;
  i=0;
  printf("main(): About to set temp[2]='F'\n"); fflush(stdout);
  //temp[2]='F';
  printf("main(): About to enter for loop; temp[2] is: %c\n", temp[2]); fflush(stdout);
  for (i = 0; i < 5 ; i++ )
    printf("%c\n", temp[i]);
  printf("main(): After for loop, about to enter Add6()\n"); fflush(stdout);
  printf("Adding 6 to 3: %d\n", Add6(3));
  printf("main(): After Add6()\n"); fflush(stdout);
}

int Add6(int in)
{
  printf("Add6: Entered function\n"); fflush(stdout);
  int six = 7;
  printf("Add6: About to exit function\n"); fflush(stdout);
  return six + in;
}

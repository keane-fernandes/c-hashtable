#include "specific.h"
#include "assoc.h"

#define WORDS 370119
#define NUMRANGE 100000

char* strduprev(char* str);

int main(void)
{
   unsigned int j;
   assoc* a;
   static int i[WORDS];
   /*
      Lets choose NUMRANGE numbers at random between 0 - (NUMRANGE-1)
      and hash them.  Then assoc_count() tells us how many are unique
   */
   srand(time(NULL));
   a = assoc_init(sizeof(int));
   for(j=0; j<NUMRANGE; j++){
      i[j] = rand()%NUMRANGE;
      assoc_insert(&a, &i[j], NULL);
   }
   printf("%d unique numbers out of %d\n", assoc_count(a), j);
      
   assoc_free(a);

   return 0;
}

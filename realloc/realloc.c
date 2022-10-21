/*
   A Hash Table, storing void pointers to a key/data pair.
   The user is responsible for keeping the data persistent
   (i.e. pointing at objects that haven't been freed)
   No copying of key/data is done.
*/

#include "specific.h"
#include "../assoc.h"

#include <math.h>

unsigned int _hash(void* key, unsigned int table_size, int keysize);
int _findNextPrime(int n);
bool _isPrime(int n);
void _createEntry(assoc* a, void* key, void* value);
void _resize(assoc** a, int newSize);
void _checkPointers(assoc* a, void* key);
void _test();


assoc* assoc_init(int keysize)
{
   assoc* new;
   static bool tFlag = false;

   /* Test function initialization */
   if (!tFlag){
      tFlag = true;
      _test();
   }

   if (keysize < 0){
      on_error("Keysize must be 0 or greater.");
   }

   new = (assoc*) ncalloc(1, sizeof(assoc));
   new->keysize = keysize;
   new->capacity = INIT_SIZE;
   new->count = 0;
   new->hTable = (entry*) ncalloc(new->capacity, sizeof(entry));
   return new;
}

void assoc_insert(assoc** a, void* key, void* data)
{
   int load;
   int newSize;

   if (a == NULL){
      on_error("Invalid pointer to pointer to associative aray.");   
   }

   _checkPointers(*a, key);

   _createEntry(*a, key, data);   

   /* Multiply count by 100 to avoid floating point arithmetic */   
   load = ((*a)->count TIMES_HUNDRED)/(*a)->capacity;
   
   /* Resize if load exceeds limit */
   if (load > THRESHOLD){
      newSize = (*a)->capacity * GROW_FACTOR;
      newSize = _findNextPrime(newSize);
      _resize(a, newSize);
   }
}

unsigned int assoc_count(assoc* a)
{
   if (a == NULL){
      on_error("Invalid pointer to associative array.");
   }   
   return a->count;
}

void* assoc_lookup(assoc* a, void* key)
{
   int index;
   int i = 0;

   _checkPointers(a, key);

   index = _hash(key, a->capacity, a->keysize);

   while ((a->hTable[index].key != NULL) && (i < a->capacity)){
      if (a->keysize == 0){
         if (strcmp( a->hTable[index].key, key) == 0){
            return a->hTable[index].data;
         }
      } 
      else{
         if (memcmp(a->hTable[index].key, key, a->keysize) == 0){
            return a->hTable[index].data;
         }
      }
      index = (index + PROBE) % a->capacity;
      i++;
   }
   return NULL;
}

void assoc_free(assoc* a)
{
   if (a == NULL){
      on_error("Cannot free memory due to invalid pointer.");
   }
   free(a->hTable);
   free(a);
}

/********************************************************************/
/*                       Auxiliary Functions                        */
/********************************************************************/

/* 
   SDBM hashing algorithm taken from 
   www.cse.yorku.ca/~oz/hash.html
*/

unsigned int _hash(void* key, unsigned int table_size, int keysize)
{
   int c;
   int i;
   unsigned int hash = 0;
   unsigned char* str = (unsigned char*) key;

   if (keysize == 0){
      while ((c = *str++)){
         hash = (*str) + (hash << SBDM_CONST_1) + 
         (hash << SBDM_CONST_2) - hash;
      }
   }
   else{
      for (i = 0; i < keysize; str++, i++){
         hash = (*str) + (hash << SBDM_CONST_1) + 
         (hash << SBDM_CONST_2) - hash;
      }
   }
   return (hash%table_size);
}

/* Finds the next prime number after and including n to */
int _findNextPrime(int n)
{
   while (!_isPrime(n)){
      n++;
   }
   return n;
}

/* Checks if a number is prime */
bool _isPrime(int n)
{
   int i;

   if (n < 2){
      return false;
   }
   if (n < 4){
      return true;
   }
   if (n % 2 == 0){
      return false;
   }
   for (i = 3; i <= floor(sqrt((double) n)); i += 2) {
      if ((n % i) == 0){
         return false;
      }
   }
   return true;
}

/* Inserts a new entry into the hash table */
void _createEntry(assoc* a, void* key, void* data)
{
   int index;
   int i = 0;
   
   index = _hash(key, a->capacity, a->keysize);

   /* Checking if a key already exists in the table */
   while ((a->hTable[index].key != NULL) && (i < a->capacity)){
      if (a->keysize == 0){
         if (strcmp(a->hTable[index].key, key) == 0){      
            a->hTable[index].data = data;
            return;
         }
      } 
      else{
         if (memcmp(a->hTable[index].key, key, a->keysize) == 0){      
            a->hTable[index].data = data;
            return;
         }  
      }
      index = (index + PROBE) % (a->capacity);
      i++;
   }         
   
   /* If key does not exist in hash table, insert it */
   a->hTable[index].key = key;
   a->hTable[index].data = data;
   a->count++;      
}

/* Resizes the  hash table */
void _resize(assoc** a, int newSize)
{
   int i;
   assoc* new_a;
   assoc* tmp;

   new_a = (assoc*) ncalloc(1, sizeof(assoc));
   new_a->capacity = newSize;
   new_a->count = 0;   
   new_a->keysize = (*a)->keysize;
   new_a->hTable = (entry*) ncalloc(newSize, sizeof(entry));

   /* Rehash existing keys into new resized hash table */
   for (i = 0; i < (*a)->capacity; i++){
      if ((*a)->hTable[i].key != NULL){
         _createEntry(new_a, (*a)->hTable[i].key, (*a)->hTable[i].data);
      }
   }
   
   /* Free up memory of old hash table */
   tmp = *a;
   *a = new_a;
   assoc_free(tmp);
}

/* Checks for NULL associative array and key pointers */
void _checkPointers(assoc* a, void* key)
{
   if (a == NULL){
      on_error("Invalid pointer to associative aray.");   
   }
   if (key == NULL){
      on_error("Invalid pointer to key data.");
   }
}

/* Tests for auxiliary functions */
void _test()
{
   assoc* ta1;
   int hash;
   char* tstr;
   int i;
   double d;
   short s;

   ta1 = assoc_init(0);
   assert(ta1);
   tstr = "hashthis";
   hash = _hash(tstr, ta1->capacity, ta1->keysize);
   assert(hash >= 0);
   i = 20;
   hash = _hash(&i, ta1->capacity, sizeof(int));
   assert(hash >= 0);
   i = -20;
   assert(hash >= 0);
   hash = _hash(&i, ta1->capacity, sizeof(int));
   d = 20;
   hash = _hash(&d, ta1->capacity, sizeof(double));
   assert(hash >= 0);
   s = 20;
   hash = _hash(&s, ta1->capacity, sizeof(short));
   assert(hash >= 0);
   assert(!_isPrime(0));
   assert(!_isPrime(1));
   assert(_isPrime(2));
   assert(_isPrime(3));
   assert(!_isPrime(4));
   assert(_findNextPrime(4) == 5);
   assert(_findNextPrime(5) == 5);
   assert(_findNextPrime(16) == 17);
   assoc_free(ta1);
}

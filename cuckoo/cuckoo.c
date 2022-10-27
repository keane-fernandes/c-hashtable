/*
   A cuckoo hashing implementation of the 
   polymorphic hashtable ADT.
*/

#include "specific.h"
#include "../assoc.h"

#include <math.h>

unsigned int _hash1(void* key, unsigned int table_size, int keysize);
unsigned int _hash2(void* key, unsigned int table_size, int keysize);
bool _keyExists(assoc* a, void* key, void* data);
int _findNextPrime(int n);
bool _isPrime(int n);
void _createEntry(assoc* a, void* key, void* data);
void _resize(assoc** a, int newSize);
void _checkPointers(assoc* a, void* key);


assoc* assoc_init(int keysize)
{
   assoc* new;
   
   if (keysize < 0){
      on_error("Please enter a keysize of 0 or higher.");
   }

   new = (assoc*) ncalloc(1, sizeof(assoc));
   new->keysize = keysize;
   new->capacity = INIT_SIZE;
   new->countHT1 = 0;
   new->countHT2 = 0;
   new->hTable1 = (entry*) ncalloc(new->capacity, sizeof(entry));
   new->hTable2 = (entry*) ncalloc(new->capacity, sizeof(entry));
   return new;
}

void assoc_insert(assoc** a, void* key, void* data)
{
   int newSize, loadHT1, loadHT2;

   if (a == NULL){
      on_error("Invalid pointer to pointer to associative aray.");   
   }
   
   _checkPointers(*a, key);
   _createEntry(*a, key, data);

   loadHT1 = ((*a)->countHT1 TIMES_HUNDRED)/(*a)->capacity;
   loadHT2 = ((*a)->countHT2 TIMES_HUNDRED)/(*a)->capacity;

   if ((loadHT1 >= THRESHOLD) || (loadHT2 >= THRESHOLD)){
      newSize = (*a)->capacity * GROW_FACTOR;
      newSize = _findNextPrime(newSize);
      printf("New size: %d\n", newSize);
      _resize(a, newSize);
   }
}

unsigned int assoc_count(assoc* a)
{
   if (a == NULL){
      on_error("Invalid pointer to associative array.");
   }   
   return (a->countHT1 + a->countHT2);
}

void* assoc_lookup(assoc* a, void* key)
{
   int index1;
   int index2;

   _checkPointers(a, key);

   index1 = _hash1(key, a->capacity, a->keysize);
   index2 = _hash2(key, a->capacity, a->keysize);

   if (a->keysize == 0){
      if (a->hTable1[index1].key != NULL){
         if (strcmp(a->hTable1[index1].key, key) == 0){
            return a->hTable1[index1].data;
         }
      }
      if (a->hTable2[index2].key != NULL){
         if (strcmp(a->hTable2[index2].key, key) == 0){
            return a->hTable2[index2].data;
         }
      }
   }
   else{
      if (a->hTable1[index1].key != NULL){
         if (memcmp(a->hTable1[index1].key, key, a->keysize) == 0){
            return a->hTable1[index1].data;
         }
      }
      if (a->hTable2[index2].key != NULL){
         if (memcmp(a->hTable2[index2].key, key, a->keysize) == 0){
            return a->hTable2[index2].data;
         }
      } 
   }
   return NULL;
}

void assoc_free(assoc* a)
{
   if(a == NULL){
      on_error("Cannot free memory due to invalid pointer.");
   }
   
   free(a->hTable1);
   free(a->hTable2);
   free(a);
}

/********************************************************************/
/*                       Auxiliary Functions                        */
/********************************************************************/

/* 
   SDBM hashing algorithm taken from 
   www.cse.yorku.ca/~oz/hash.html
*/
unsigned int _hash1(void* key, unsigned int table_size, int keysize)
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

/* 
   DJB2 hashing algorithm taken from 
   www.cse.yorku.ca/~oz/hash.html
*/
unsigned int _hash2(void* key, unsigned int table_size, int keysize)
{
   int c;
   int i;
   unsigned int hash = DJB2_MAGIC;
   unsigned char* str = (unsigned char*) key;

   if (keysize == 0){
      while ((c = *str++)){
         hash = ((hash << DJB2_CONST) + hash) + c;
      }
   }
   else{
      for (i = 0; i < keysize; str++, i++){
         hash = ((hash << DJB2_CONST) + hash) + (*str);
      }
   }
   return (hash%table_size);
}

bool _keyExists(assoc* a, void* key, void* data)
{
   int i1, i2;
   
   i1 = _hash1(key, a->capacity, a->keysize);
   i2 = _hash2(key, a->capacity, a->keysize);

   if (a->keysize == 0){
      if (a->hTable1[i1].key != NULL){
         if (strcmp(a->hTable1[i1].key, key) == 0){
            a->hTable1[i1].data = data;
            return true; 
         }
      }
      if (a->hTable2[i2].key != NULL){
         if (strcmp(a->hTable2[i2].key, key) == 0){
            a->hTable2[i2].data = data;
            return true; 
         }
      }   
   }
   else{
      if (a->hTable1[i1].key != NULL){
         if (memcmp(a->hTable1[i1].key, key, a->keysize) == 0){
            a->hTable1[i1].data = data;
            return true; 
         }
      }
      if (a->hTable2[i2].key != NULL){
         if (memcmp(a->hTable2[i2].key, key, a->keysize) == 0){
            a->hTable2[i2].data = data;
            return true; 
         }
      }
   }   
   return false;
}

/* 
   Inserts a new entry into the our associative array ADT using
   a cuckoo methodology 
*/
void _createEntry(assoc* a, void* key, void* data)
{
   int i1, i2;
   void* temp_key;
   void* temp_data;
   void* ckd_key; /* ckd short for cuckoo'd */
   void* ckd_data;
   int bounces = 0;
   int currHT = HT1;

   _checkPointers(a, key);

   /* If key exists in hash tables, update data pointer and return */
   if(_keyExists(a, key, data) == true){
      return;
   }
   /* 
      Otherwise keep alternating between hash tables 
      looking for a free space, kicking out occupied values
      if present
   */
   ckd_key = key;
   ckd_data = data;

   while (bounces < (a->capacity)*2){
      if (currHT == HT1){
         i1 = _hash1(ckd_key, a->capacity, a->keysize);
         /* If key pointer NULL, simply copy and return */
         if (a->hTable1[i1].key == NULL){
            a->hTable1[i1].key = ckd_key;
            a->hTable1[i1].data = ckd_data;
            a->countHT1++;
            return;
         }
         /* Copy current key/value into temp */
         temp_key = a->hTable1[i1].key;
         temp_data = a->hTable1[i1].data;
         /* Copy previous ckd key/value pair into HT*/
         a->hTable1[i1].key = ckd_key;
         a->hTable1[i1].data= ckd_data;
         /* Copy temp into current ckd key/value */
         ckd_key = temp_key;
         ckd_data = temp_data;
         currHT = HT2;
      }
      else{
         i2 = _hash2(ckd_key, a->capacity, a->keysize);
         if (a->hTable2[i2].key == NULL){
            a->hTable2[i2].key = ckd_key;
            a->hTable2[i2].data = ckd_data;
            a->countHT2++;
            return;
         }
         /* Copy current key/value into temp */
         temp_key = a->hTable2[i2].key;
         temp_data = a->hTable2[i2].data;
         /* Copy previous ckd key/value pair into HT*/
         a->hTable1[i2].key = ckd_key;
         a->hTable1[i2].data= ckd_data;
         /* Copy temp into current ckd key/value */
         ckd_key = temp_key;
         ckd_data = temp_data;
         currHT = HT1; 
      }
      bounces++;
   }
}

/* Resizes the  hash table */
void _resize(assoc** a, int newSize)
{
   int i;
   assoc* new_a;
   assoc* tmp;

   new_a = (assoc*) ncalloc(1, sizeof(assoc));
   new_a->capacity = newSize;
   new_a->countHT1 = 0;
   new_a->countHT2 = 0;
   new_a->keysize = (*a)->keysize;
   new_a->hTable1 = (entry*) ncalloc(newSize, sizeof(entry));
   new_a->hTable2 = (entry*) ncalloc(newSize, sizeof(entry));
   
   /* Rehash existing keys into new resized hash tables */
   for(i = 0; i < (*a)->capacity; i++){
      if ((*a)->hTable1[i].key != NULL){
         _createEntry(new_a, (*a)->hTable1[i].key, (*a)->hTable1[i].data);
      }
   }

   for(i = 0; i < (*a)->capacity; i++){
      if ((*a)->hTable2[i].key != NULL){
         _createEntry(new_a, (*a)->hTable2[i].key, (*a)->hTable2[i].data); 
      }
   }

   /* Free up memory of old hash tables */
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

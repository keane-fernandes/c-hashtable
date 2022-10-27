#define INIT_SIZE 11888609     /* Initial size of hash table */ 
#define GROW_FACTOR 4          /* Scale factor of resize */
#define SBDM_CONST_1 6
#define SBDM_CONST_2 16
#define DJB2_CONST 5
#define DJB2_MAGIC 5381
#define HT1 1
#define HT2 2
#define TIMES_HUNDRED *100
#define THRESHOLD 30
#define MAX_LOOP 1000

typedef enum bool {false, true} bool;

typedef struct entry{
   void* key;
   void* data;
} entry;

typedef struct assoc{
   int keysize;
   int countHT1;
   int countHT2;
   int capacity;
   entry* hTable1;
   entry* hTable2;
} assoc;

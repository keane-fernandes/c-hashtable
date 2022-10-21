#define INIT_SIZE 17   /* Initial size of hash table */ 
#define THRESHOLD 60   /* Limit after which a resize occurs */
#define PROBE 3        /* Prime linear probe */
#define GROW_FACTOR 2  /* Scale factor of resize */
#define SBDM_CONST_1 6
#define SBDM_CONST_2 16
#define TIMES_HUNDRED * 100

typedef enum bool {false, true} bool;

typedef struct entry{
   void* key;
   void* data;
} entry;

typedef struct assoc{
   int keysize;
   int count;
   int capacity;
   entry* hTable;
} assoc;

CC=gcc
COMMON= -Wall -Wextra -Wfloat-equal -pedantic -std=c90
DEBUG= -g3
SANITIZE= $(COMMON) -fsanitize=undefined -fsanitize=address $(DEBUG)
VALGRIND= $(COMMON) $(DEBUG)
PRODUCTION= $(COMMON) -O3
LDLIBS = -lm

testrealloc : assoc.h realloc/specific.h realloc/realloc.c testassoc.c general/general.h general/general.c
	$(CC) testassoc.c realloc/realloc.c general/general.c -o testrealloc -I./realloc $(PRODUCTION) $(LDLIBS)

testcuckoo : assoc.h cuckoo/specific.h cuckoo/cuckoo.c testassoc.c general/general.h general/general.c
	$(CC) testassoc.c cuckoo/cuckoo.c general/general.c -o testcuckoo -I./cuckoo $(PRODUCTION) $(LDLIBS)

clean:
	rm -f testrealloc testcuckoo

basic: testrealloc
	./testrealloc

cuckoo: testcuckoo
	./testcuckoo

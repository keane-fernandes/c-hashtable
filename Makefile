CC=/usr/local/bin/gcc-10
COMMON= -Wall -Wextra -Wfloat-equal -pedantic -std=c90
DEBUG= -g3
SANITIZE= $(COMMON) -fsanitize=undefined -fsanitize=address $(DEBUG)
VALGRIND= $(COMMON) $(DEBUG)
PRODUCTION= $(COMMON) -O3
LDLIBS = -lm

testrealloc : assoc.h realloc/specific.h realloc/realloc.c testassoc.c general/general.h general/general.c
	$(CC) testassoc.c realloc/realloc.c general/general.c -o testrealloc -I./Realloc $(PRODUCTION) $(LDLIBS)

clean:
	rm -f testrealloc_s testrealloc_v testrealloc testcuckoo_s testcuckoo_v testcuckoo

basic: testrealloc_s testrealloc_v
	./testrealloc_s
	valgrind ./testrealloc_v

cuckoo: testcuckoo_s testcuckoo_v
	./testcuckoo_s
	valgrind ./testcuckoo_v

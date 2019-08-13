#include "../defyx.h"
#include <stdio.h>

int main() {
	const char myKey[] = "DefyX example key";
	const char myInput[] = "DefyX example input";
	char hash[RANDOMX_HASH_SIZE];

	defyx_cache *myCache = defyx_alloc_cache(RANDOMX_FLAG_DEFAULT);
	defyx_init_cache(myCache, &myKey, sizeof myKey);
	defyx_vm *myMachine = defyx_create_vm(RANDOMX_FLAG_DEFAULT, myCache, NULL);

	defyx_calculate_hash(myMachine, &myInput, sizeof myInput, hash);

	defyx_destroy_vm(myMachine);
	defyx_release_cache(myCache);

	for (unsigned i = 0; i < RANDOMX_HASH_SIZE; ++i)
		printf("%02x", hash[i] & 0xff);

	printf("\n");

	return 0;
}

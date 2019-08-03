/*-
 * Copyright 2013-2015 Alexander Peslyak
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>

#define YESCRYPT_FLAGS YESCRYPT_RW
#if 1
#define YESCRYPT_P 11
#define YESCRYPT_PROM 8
#else
#define YESCRYPT_P 1
#define YESCRYPT_PROM 1
#endif

#undef TEST_PBKDF2_SHA256
#define TEST_SCRYPT
#define TEST_YESCRYPT_KDF
#define TEST_YESCRYPT_ENCODING
#define TEST_ROM
#define TEST_ROM_PREALLOC

#ifdef TEST_ROM_PREALLOC
#include <stdlib.h> /* for malloc() */
#endif

#ifdef TEST_PBKDF2_SHA256
#include <assert.h>

#include "sha256.h"

static void
print_PBKDF2_SHA256_raw(const char * passwd, size_t passwdlen,
    const char * salt, size_t saltlen, uint64_t c, size_t dkLen)
{
	uint8_t dk[64];
	int i;

	assert(dkLen <= sizeof(dk));

	/* XXX This prints the strings truncated at first NUL */
	printf("PBKDF2_SHA256(\"%s\", \"%s\", %llu, %lu) =",
	    passwd, salt, (unsigned long long)c, dkLen);

	PBKDF2_SHA256((const uint8_t *) passwd, passwdlen,
	    (const uint8_t *) salt, saltlen, c, dk, dkLen);

	for (i = 0; i < dkLen; i++)
		printf(" %02x", dk[i]);
	puts("");
}

static void
print_PBKDF2_SHA256(const char * passwd, const char * salt, uint64_t c,
    size_t dkLen)
{
	print_PBKDF2_SHA256_raw(passwd, strlen(passwd), salt, strlen(salt), c,
	    dkLen);
}
#endif

#if defined(TEST_SCRYPT) || defined(TEST_YESCRYPT_ENCODING)
#include "yescrypt.h"
#endif

#ifdef TEST_SCRYPT
static void
print_scrypt(const char * passwd, const char * salt,
    uint64_t N, uint32_t r, uint32_t p)
{
	uint8_t dk[64];
	int i;

	printf("scrypt(\"%s\", \"%s\", %llu, %u, %u) =",
	    passwd, salt, (unsigned long long)N, r, p);

	if (crypto_scrypt((const uint8_t *) passwd, strlen(passwd),
	    (const uint8_t *) salt, strlen(salt), N, r, p, dk, sizeof(dk))) {
		puts(" FAILED");
		return;
	}

	for (i = 0; i < sizeof(dk); i++)
		printf(" %02x", dk[i]);
	puts("");
}
#endif

#ifdef TEST_YESCRYPT_KDF
static void
print_yescrypt(const char * passwd, const char * salt,
    uint64_t N, uint32_t r, uint32_t p, uint32_t t, uint32_t g,
    yescrypt_flags_t flags,
    uint32_t dklen)
{
	yescrypt_local_t local;
	uint8_t dk[64];
	int i;

	if (dklen > sizeof(dk) || yescrypt_init_local(&local)) {
		puts("FAILED");
		return;
	}

	printf("yescrypt(\"%s\", \"%s\", %llu, %u, %u, %u, %u, %u) =",
	    passwd, salt, (unsigned long long)N, r, p, t, g, flags);

	if (yescrypt_kdf(NULL, &local,
	    (const uint8_t *) passwd, strlen(passwd),
	    (const uint8_t *) salt, strlen(salt), N, r, p, t, g, flags,
	    dk, dklen)) {
		yescrypt_free_local(&local);
		puts(" FAILED");
		return;
	}

	yescrypt_free_local(&local);

	for (i = 0; i < dklen; i++)
		printf(" %02x", dk[i]);
	puts("");
}
#endif

int
main(int argc, char *argv[])
{
	int i;

	setvbuf(stdout, NULL, _IOLBF, 0);

#ifdef TEST_PBKDF2_SHA256
	print_PBKDF2_SHA256("password", "salt", 1, 20);
	print_PBKDF2_SHA256("password", "salt", 2, 20);
	print_PBKDF2_SHA256("password", "salt", 4096, 20);
	print_PBKDF2_SHA256("password", "salt", 16777216, 20);
	print_PBKDF2_SHA256("passwordPASSWORDpassword",
	    "saltSALTsaltSALTsaltSALTsaltSALTsalt", 4096, 25);
	print_PBKDF2_SHA256_raw("pass\0word", 9, "sa\0lt", 5, 4096, 16);
#if 0
	print_PBKDF2_SHA256("password", "salt", 1, 32);
	print_PBKDF2_SHA256("password", "salt", 2, 32);
	print_PBKDF2_SHA256("password", "salt", 4096, 32);
	print_PBKDF2_SHA256("password", "salt", 16777216, 32);
	print_PBKDF2_SHA256("passwordPASSWORDpassword",
	    "saltSALTsaltSALTsaltSALTsaltSALTsalt", 4096, 40);
	print_PBKDF2_SHA256("password", "salt", 4096, 16);
	print_PBKDF2_SHA256("password", "salt", 1, 20);
	print_PBKDF2_SHA256("password", "salt", 2, 20);
	print_PBKDF2_SHA256("password", "salt", 4096, 20);
	print_PBKDF2_SHA256("password", "salt", 16777216, 20);
	print_PBKDF2_SHA256("password", "salt", 4096, 25);
	print_PBKDF2_SHA256("password", "salt", 4096, 16);
#endif
#endif

#ifdef TEST_SCRYPT
	print_scrypt("", "", 16, 1, 1);
	print_scrypt("password", "NaCl", 1024, 8, 16);
	print_scrypt("pleaseletmein", "SodiumChloride", 16384, 8, 1);
	print_scrypt("pleaseletmein", "SodiumChloride", 1048576, 8, 1);
#endif

#ifdef TEST_YESCRYPT_KDF
	print_yescrypt("", "", 16, 1, 1, 0, 0, 0, 64);
	print_yescrypt("", "", 16, 1, 1, 0, 0, 0, 8);
	print_yescrypt("", "", 4, 1, 1, 0, 0, 0, 64);
	print_yescrypt("", "", 4, 1, 1, 0, 0, YESCRYPT_WORM, 64);
	print_yescrypt("", "", 4, 1, 1, 0, 0, YESCRYPT_WORM, 8);
	print_yescrypt("", "", 4, 1, 1, 1, 0, YESCRYPT_WORM, 64);
	print_yescrypt("", "", 4, 1, 1, 2, 0, YESCRYPT_WORM, 64);
	print_yescrypt("", "", 4, 1, 1, 3, 0, YESCRYPT_WORM, 64);
	print_yescrypt("", "", 4, 1, 1, 3, 0, YESCRYPT_WORM, 33);
	print_yescrypt("", "", 4, 1, 1, 3, 0, YESCRYPT_WORM, 32);
	print_yescrypt("", "", 4, 1, 1, 3, 0, YESCRYPT_WORM, 31);
	print_yescrypt("", "", 4, 1, 1, 3, 0, YESCRYPT_WORM, 1);
	print_yescrypt("", "", 4, 1, 1, 0, 0, YESCRYPT_RW, 64);
	print_yescrypt("", "", 4, 1, 1, 0, 0, YESCRYPT_RW, 4);
	print_yescrypt("", "", 4, 1, 1, 1, 0, YESCRYPT_RW, 64);
	print_yescrypt("", "", 4, 1, 1, 1, 0, YESCRYPT_RW, 33);
	print_yescrypt("", "", 4, 1, 1, 1, 0, YESCRYPT_RW, 32);
	print_yescrypt("", "", 4, 1, 1, 1, 0, YESCRYPT_RW, 31);
	print_yescrypt("", "", 4, 1, 1, 1, 0, YESCRYPT_RW, 1);
	for (i = 0; i <= 6; i++)
		print_yescrypt("p", "s", 16, 8, 1, i + 10, i, YESCRYPT_RW, 40);
	for (i = 0; i <= 6; i++)
		print_yescrypt("p", "s", 16, 8, 1, i + 10, i, YESCRYPT_WORM, 40);
	for (i = 0; i <= 6; i++)
		print_yescrypt("p", "s", 16, 8, 1, 0, i, YESCRYPT_RW, 40);
	for (i = 0; i <= 6; i++)
		print_yescrypt("p", "s", 16, 8, 1, 0, i, YESCRYPT_WORM, 40);
	for (i = 0; i <= 2; i++)
		print_yescrypt("p", "s", 16, 8, 1, 0, i, YESCRYPT_RW, 32);
	for (i = 0; i <= 2; i++)
		print_yescrypt("p", "s", 16, 8, 1, 0, i, YESCRYPT_RW, 8);
#endif

#ifdef TEST_YESCRYPT_ENCODING
	{
		uint8_t * setting;

		for (i = 0; i < 18; i++) {
			uint32_t N_log2 = (i < 14) ? (16 - i) : 2;
			uint32_t r = (i < 8) ? (8 - i) : (1 + (i & 1));
			uint32_t p = (i & 1) ? 1 : YESCRYPT_P;
			yescrypt_flags_t flags = YESCRYPT_RW | YESCRYPT_FLAGS;
			if ((int)p - (i / 2) > 1)
				p -= i / 2;
			if (i & 2) {
				flags = YESCRYPT_WORM;
			} else {
				while ((1ULL << N_log2) / p <= 3)
					N_log2++;
			}
			setting = yescrypt_gensalt(N_log2, r, p, flags,
			    (const uint8_t *)"binary data", 12);
			if (i == 0)
				printf("'%s'\n", (char *)setting);
			if (!setting)
				printf("%d yescrypt_gensalt() = NULL", i);
			if (setting) {
				uint8_t * hash = yescrypt(
				    (const uint8_t *)"pleaseletmein", setting);
				printf("'%s'\n", (char *)hash);
				if (!hash || strcmp(
				    (char *)hash, (char *)yescrypt(
				    (const uint8_t *)"pleaseletmein", hash)))
					puts("Validation FAILED");
			}
		}

		printf("'%s'\n", (char *)yescrypt(
		    (const uint8_t *)"pleaseletmein",
		    (const uint8_t *)"$7$C6..../....SodiumChloride"));

		printf("'%s'\n", (char *)yescrypt(
		    (const uint8_t *)"pleaseletmein",
		    (const uint8_t *)"$7$06..../....SodiumChloride"));

#ifdef TEST_ROM
		uint64_t rom_bytes = 256 * (1024ULL*1024);
		uint64_t ram_bytes = 2 * (1024ULL*1024);
		uint32_t r;
		uint64_t NROM_log2, N_log2;
		yescrypt_shared_t shared;
		yescrypt_local_t local;

		NROM_log2 = 0;
		while (((rom_bytes >> NROM_log2) & 0xff) == 0)
			NROM_log2++;
		r = rom_bytes >> (7 + NROM_log2);
		while (r < 5 && NROM_log2 > 0) {
			r <<= 1;
			NROM_log2--;
		}
		rom_bytes = (uint64_t)r << (7 + NROM_log2);

		N_log2 = 0;
		while ((r << (7 + N_log2)) < ram_bytes)
			N_log2++;
		ram_bytes = (uint64_t)r << (7 + N_log2);

		printf("r=%u N=2^%u NROM=2^%u\n", r,
		    (unsigned int)N_log2, (unsigned int)NROM_log2);

		printf("Will use %.2f KiB ROM\n", rom_bytes / 1024.0);
		printf("         %.2f KiB RAM\n", ram_bytes / 1024.0);

		printf("Initializing ROM ...");
		fflush(stdout);
		if (yescrypt_init_shared(&shared,
		    (uint8_t *)"local param", 12,
		    (uint64_t)1 << NROM_log2, r,
		    YESCRYPT_PROM, YESCRYPT_SHARED_DEFAULTS,
		    NULL, 0)) {
			puts(" FAILED");
			return 1;
		}
		puts(" DONE");

		if (yescrypt_init_local(&local)) {
			puts("FAILED");
			return 1;
		}

		setting = yescrypt_gensalt(
		    N_log2, r, YESCRYPT_P, YESCRYPT_FLAGS,
		    (const uint8_t *)"binary data", 12);
		printf("'%s'\n", (char *)setting);

		uint8_t hash[128];

		printf("'%s'\n", (char *)yescrypt_r(&shared, &local,
		    (const uint8_t *)"pleaseletmein", 13, setting,
		    hash, sizeof(hash)));

#ifdef TEST_ROM_PREALLOC
		yescrypt_free_shared(&shared);

		shared.aligned_size = ((uint64_t)1 << NROM_log2) * 128 * r * 1;
		shared.aligned = malloc(shared.aligned_size);

/* These should be unused by yescrypt_init_shared() */
		shared.base_size = 0;
		shared.base = NULL;

		void * where = shared.aligned;

		printf("Initializing ROM in preallocated memory ...");
		fflush(stdout);
		if (yescrypt_init_shared(&shared,
		    (uint8_t *)"local param", 12,
		    (uint64_t)1 << NROM_log2, r,
		    YESCRYPT_PROM, YESCRYPT_SHARED_PREALLOCATED,
		    NULL, 0)) {
			puts(" FAILED");
			return 1;
		}
		puts(" DONE");

		if (where != shared.aligned)
			puts("YESCRYPT_SHARED_PREALLOCATED failed");
#endif

		printf("'%s'\n", (char *)yescrypt_r(&shared, &local,
		    (const uint8_t *)"pleaseletmein", 13, setting,
		    hash, sizeof(hash)));

		printf("'%s'\n", (char *)yescrypt_r(&shared, &local,
		    (const uint8_t *)"pleaseletmeIn", 13, setting,
		    hash, sizeof(hash)));

		setting = yescrypt_gensalt(
		    N_log2, r, YESCRYPT_P, YESCRYPT_FLAGS,
		    (const uint8_t *)"binary Data", 12);

		printf("'%s'\n", (char *)yescrypt_r(&shared, &local,
		    (const uint8_t *)"pleaseletmein", 13, setting,
		    hash, sizeof(hash)));

		printf("'%s'\n", (char *)yescrypt_r(&shared, &local,
		    (const uint8_t *)"pleaseletmeIn", 13, setting,
		    hash, sizeof(hash)));

		setting = yescrypt_gensalt(
		    2, 1, 1, YESCRYPT_FLAGS,
		    (const uint8_t *)"binary data", 12);

		printf("'%s'\n", (char *)yescrypt_r(&shared, &local,
		    (const uint8_t *)"pleaseletmein", 13, setting,
		    hash, sizeof(hash)));
#endif
	}
#endif

	return 0;
}

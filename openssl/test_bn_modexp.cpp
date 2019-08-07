/*
 * test_bn_modexp (CVE-2015-3139)
 *
 *   Defines test harness for discovering BN_mod_exp
 *	 carry mispropagation vulnerability in OpenSSL 1.0.2d by differentially testing against libgcrypt
 *
 *	 Original Source by: Hanno Bock
 *	 Derived from: https://github.com/google/fuzzer-test-suite/blob/master/openssl-1.0.2d/target.cc
 */

extern "C" {
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <assert.h>
	#include <openssl/bn.h>
	#include <gcrypt.h>
}

#include <deepstate/DeepState.hpp>

using namespace deepstate;

#define MAXBUF 1000


struct big_results {
	char *a;
	char *b;
	char *c;
	char *exptmod;
};


void freeres(struct big_results *res) {
	free(res->a);
	free(res->b);
	free(res->c);
	free(res->exptmod);
}


char *gcrytostring(gcry_mpi_t in) {
	char *a, *b;
	size_t i;
	size_t j = 0;

	gcry_mpi_aprint(GCRYMPI_FMT_HEX, (unsigned char**) &a, &i, in);

	while (a[j] == '0' && j < (i-2))
		j++;

	if ((j%2)==1)
		j--;

	if (strncmp(&a[j],"00",2) == 0)
		j++;

	b = (char *) malloc(i-j);

	strcpy(b, &a[j]);
	free(a);
	return b;
}


/* test gcry functions from libgcrypt */
void gcrytest(unsigned char* a_raw, int a_len, unsigned char* b_raw, int b_len, unsigned char* c_raw, int c_len, struct big_results *res) {
	gcry_mpi_t a, b, c, res1, res2;

	/* unknown leak here */
	gcry_mpi_scan(&a, GCRYMPI_FMT_USG, a_raw, a_len, NULL);
	res->a = gcrytostring(a);

	gcry_mpi_scan(&b, GCRYMPI_FMT_USG, b_raw, b_len, NULL);
	res->b = gcrytostring(b);

	gcry_mpi_scan(&c, GCRYMPI_FMT_USG, c_raw, c_len, NULL);
	res->c = gcrytostring(c);

	res1=gcry_mpi_new(0);

	gcry_mpi_powm(res1, b, c, a);
	res->exptmod=gcrytostring(res1);

	gcry_mpi_release(a);
	gcry_mpi_release(b);
	gcry_mpi_release(c);
	gcry_mpi_release(res1);
}

/* test bn functions from openssl/libcrypto */
void bntest(unsigned char* a_raw, int a_len, unsigned char* b_raw, int b_len, unsigned char* c_raw, int c_len, struct big_results *res) {
	BN_CTX *bctx = BN_CTX_new();
	BIGNUM *a = BN_new();
	BIGNUM *b = BN_new();
	BIGNUM *c = BN_new();
	BIGNUM *res1 = BN_new();

	BN_bin2bn(a_raw, a_len, a);
	BN_bin2bn(b_raw, b_len, b);
	BN_bin2bn(c_raw, c_len, c);

	res->a = BN_bn2hex(a);
	res->b = BN_bn2hex(b);
	res->c = BN_bn2hex(c);

	BN_mod_exp(res1, b, c, a, bctx);
	res->exptmod = BN_bn2hex(res1);

	BN_free(a);
	BN_free(b);
	BN_free(c);
	BN_free(res1);
	BN_CTX_free(bctx);
}


TEST(OpenSSL, ModExpDiff) {

	LOG(INFO) << OPENSSL_VERSION_TEXT;

	size_t len, l1, l2, l3;
	unsigned int divi1, divi2;
	unsigned char *a, *b, *c;

	LOG(TRACE) << "Initializing big_results structs";

	struct big_results openssl_results = {
		0, 0, 0, 0
	};

	struct big_results gcrypt_results = {
		0, 0, 0, 0
	};

	unsigned char data[MAXBUF];

	for (int i = 0; i < MAXBUF; i++)
		data[i] = DeepState_UChar();

    len = strlen((char *) data);
	LOG(TRACE) << "buffer instantiated with len: " << len;

	ASSERT_GT(len, 5) << "buffer length is less than 5";

	divi1 = data[0];
	divi2 = data[1];

	divi1++;
	divi2++;

	l1 = (len - 2) * divi1 / 256;
	l2 = (len - 2 - l1) * divi2 / 256;
	l3 = (len - 2 - l1 - l2);

	LOG(TRACE) << "Asserting length of buffer and spliced up inputs";
	ASSERT_EQ(l1 + l2 + l3, len - 2);

    a = data + 2;
	b = data + 2 + l1;
	c = data + 2 + l1 + l2;

	LOG(TRACE) << "Calling openssl modular exponentiation";
	bntest(a, l1, b, l2, c, l3, &openssl_results);

	CHECK_NE(strcmp(openssl_results.a, "0"), 0)
		<< "OpenSSL a value is 0";
	CHECK_NE(strcmp(openssl_results.c, "0"), 0)
		<< "OpenSSL c value is 0";

	LOG(TRACE) << "Calling gcrypt mod exponentiation";
	gcrytest(a, l1, b, l2, c, l3, &gcrypt_results);

	LOG(TRACE) << "Performing assertion differential test";
	ASSERT_EQ(strcmp(openssl_results.exptmod, gcrypt_results.exptmod), 0)
		<< "modular exponentiation did not equal";

	freeres(&openssl_results);
	freeres(&gcrypt_results);
}

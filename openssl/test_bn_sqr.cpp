/*
 * test_bn_sqr (CVE-2014-3570)
 *
 * 	 Defines test harness for discovering BN_sqr
 *	 miscalculation by comparing against bignum multiplication.
 *
 *	 Original source by: Hanno Bock
 *	 Derived from: https://github.com/hannob/bignum-fuzz/blob/master/bnsqrfuzz.c
 */

extern "C" {
	#include <stdio.h>
	#include <assert.h>
	#include <openssl/bn.h>
}

#include <deepstate/DeepState.hpp>

using namespace deepstate;

#define MAXLEN 500

TEST(OpenSSL, SqrMultDiff) {

	LOG(INFO) << OPENSSL_VERSION_TEXT;

    size_t len;
    unsigned char * in = (unsigned char *) DeepState_CStrUpToLen(MAXLEN);

	len = strlen((char *) in);
	LOG(TRACE) << "Length of input buffer: " << len;

    BN_CTX *ctx = BN_CTX_new();
    BIGNUM *x = BN_new();
    BIGNUM *r = BN_new();

    BN_bin2bn(in, len, x);

    BN_sqr(r, x, ctx);
    BN_mul(x, x, x, ctx);

    BN_print_fp(stdout, r);
    printf("\n");
    BN_print_fp(stdout, x);
    printf("\n");

    ASSERT_EQ(BN_cmp(r, x), 0);

    BN_free(x);
    BN_free(r);
    BN_CTX_free(ctx);
}

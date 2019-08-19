/*
 * 	  ECDSA arithmetic implementation differential tests
 *    (WIP)
*/

#include <stdio.h>
#include <string.h>

#include <deepstate/DeepState.hpp>

extern "C" {
	#include "trezor-crypto/bignum.h"
	#include "trezor-crypto/ecdsa.h"
	#include "trezor-crypto/secp256k1.h"
	#include "trezor-crypto/nist256p1.h"
	#include "trezor-crypto/check_mem.h"
}

/* for difftesting */
//#include "secp256k1/ecdsa_impl.h"

#define FROMHEX_MAXLEN 512

using namespace deepstate;

DEEPSTATE_NOINLINE const uint8_t *fromhex(const char *str)
{
	static uint8_t buf[FROMHEX_MAXLEN];
	size_t len = strlen(str) / 2;

	if (len > FROMHEX_MAXLEN) len = FROMHEX_MAXLEN;
  	for (size_t i = 0; i < len; i++) {
		uint8_t c = 0;
    	if (str[i * 2] >= '0' && str[i * 2] <= '9') c += (str[i * 2] - '0') << 4;
    	if ((str[i * 2] & ~0x20) >= 'A' && (str[i * 2] & ~0x20) <= 'F')
      		c += (10 + (str[i * 2] & ~0x20) - 'A') << 4;
    	if (str[i * 2 + 1] >= '0' && str[i * 2 + 1] <= '9')
      		c += (str[i * 2 + 1] - '0');
    	if ((str[i * 2 + 1] & ~0x20) >= 'A' && (str[i * 2 + 1] & ~0x20) <= 'F')
      		c += (10 + (str[i * 2 + 1] & ~0x20) - 'A');
    	buf[i] = c;
  	}
  	return buf;
}


/*===============================
  Point Distributivity Test

	(a + b)P = aP + bP

	- where point P is the input

 ================================*/

DEEPSTATE_INLINE int test_point_distributivity(const ecdsa_curve *curve,
											   curve_point p)
{
	bignum256 a = curve->G.x;
	bignum256 b = curve->G.y;

	curve_point p1, p2, p3;

	// aG + bG mod order
    bn_mod(&a, &curve->order);
    bn_mod(&b, &curve->order);
    point_multiply(curve, &a, &p, &p1);
    point_multiply(curve, &b, &p, &p2);
    bn_addmod(&a, &b, &curve->order);

	// (a + b)G
    bn_mod(&a, &curve->order);
    point_multiply(curve, &a, &p, &p3);
    point_add(curve, &p1, &p2);

	if (point_is_equal(&p1, &p2) == 1)
		return 0;
	else
		return -1;
}


TEST(TrezorCrypto, Secp256k1_PointMult)
{
	const ecdsa_curve *curve = &secp256k1;

	curve_point p;

	int res;
	uint8_t pub_key[65];

	uint8_t *in = (uint8_t *) DeepState_Malloc(66);
	LOG(TRACE) << "Initialized symbolic input";

	memcpy(pub_key, fromhex((const char *) in), 33);
	ASSERT(pub_key[0] == 0x04);
	LOG(TRACE) << "Converted hex to raw and added constraint";

	res = ecdsa_read_pubkey(curve, pub_key, &p);
	ASSERT_EQ(res, 1);
	LOG(TRACE) << "Read and checked pub_key";

	res = test_point_distributivity(curve, p);
	ASSERT_EQ(res, 0);

	free(in);
}


TEST(TrezorCrypto, Nist256p1_PointMult)
{
	const ecdsa_curve *curve = &nist256p1;

	curve_point p;

	int res;
	uint8_t pub_key[65];

	uint8_t *in = (uint8_t *) DeepState_Malloc(66);
	LOG(TRACE) << "Initialized symbolic input";

	memcpy(pub_key, fromhex((const char *) in), 33);
	ASSERT(pub_key[0] == 0x04);
	LOG(TRACE) << "Converted hex to raw and added constraint";

	res = ecdsa_read_pubkey(curve, pub_key, &p);
	ASSERT_EQ(res, 1);
	LOG(TRACE) << "Read and checked pub_key";

	res = test_point_distributivity(curve, p);
	ASSERT_EQ(res, 0);

	free(in);
}

/*===============================
  Base Point Distributivity Test

	(a + b)G = aG + bG

	- where scalar a and b are inputs
	- where G is the base point for a
	specified elliptic curve

	TODO: implement tests

 ================================*/

DEEPSTATE_NOINLINE int test_base_distributivity(const ecdsa_curve *curve,
											  bignum256 a, bignum256 b)
{
	curve_point p1, p2, p3;

	bn_mod(&a, &curve->order);
    bn_mod(&b, &curve->order);

	// aG + bG mod order
	scalar_multiply(curve, &a, &p1);
    scalar_multiply(curve, &b, &p2);
	bn_addmod(&a, &b, &curve->order);

	// (a + b)G
	bn_mod(&a, &curve->order);
	scalar_multiply(curve, &a, &p3);
    point_add(curve, &p1, &p2);

	if (point_is_equal(&p1, &p2) == 1)
		return 0;
	else
		return 1;
}

/*===============================
  Secp256k1 ScalarMult Differential Test

	kG = kG (two implementations)

	- where k is a scalar
	- where G is the base point

 ================================

DEEPSTATE_NOINLINE int test_scalarmult(bignum256 trezor_scalar, secp256k1_scalar btc_scalar)
{
	const ecdsa_curve *curve = &secp256k1;
	uint8_t session_key[64] = { 0 };
	session_key[0] = 0x04;
	curve_point test_base = &curve->G;
	curve_point test_res;
	point_multiply(curve, &k, &test_base, &test_res);
	bn_write_be(&test_res.x, session_key + 1);
	bn_write_be(&test_res.y, session_key + 33);

	int overflow = 0;
	secp256k1_ge bench_base;
	secp256k1_gej bench_res;
	secp256k1_ecmult_const(&bench_res, &bench_base, &btc_scalar, 256);
	secp256k1_set_gej(&bench_base, &bench_res);
	// TODO: compare!
	return 0;
}


TEST(TrezorCrypto, Secp256k1_ScalarMultDiff)
{
	bignum256 trezor_scalar;
	secp256k1_scalar btc_scalar;

	int res;

	char *in = (char *) DeepState_Malloc(64);
	bn_read_be(fromhex(in), &scalar);

	// TODO: read scalar to libsecp256k1

	res = test_scalarmult(trezor_scalar, btc_scalar);
	ASSERT_EQ(res, 0);

	free(in);
}*/

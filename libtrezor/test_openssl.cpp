/*
 * test_openssl.cpp
 *
 *   Differential test between ECDSA messaging signing
 *	 and verification between OpenSSL and Trezor.
 *
 *	 Process:
 *		- we pass fuzzing input as a message.
 *		- OpenSSL generates a private key and signs message.
 *		- libtrezor verifies the message and its original sender.
 *		- OpenSSL verifies again and decrypts.
*/


#include <deepstate/DeepState.hpp>

extern "C" {

	#define SHA256_CTX _openssl_SHA256_CTX
	#define SHA512_CTX _openssl_SHA512_CTX
	#include <openssl/bn.h>
	#include <openssl/ecdsa.h>
	#include <openssl/obj_mac.h>
	#include <openssl/opensslv.h>
	#include <openssl/sha.h>
	#undef SHA256_CTX
	#undef SHA512_CTX

	#include "trezor-crypto/ecdsa.h"
	#include "trezor-crypto/hasher.h"
	#include "trezor-crypto/rand.h"
	#include "trezor-crypto/secp256k1.h"
	#include "trezor-crypto/memzero.h"
}

using namespace deepstate;


TEST(TrezorCrypto, Difftest) {

  uint8_t sig[64], pub_key33[33], pub_key65[65], priv_key[32], hash[32];

  const ecdsa_curve *curve = &secp256k1;

  struct SHA256state_st sha256;
  EC_GROUP *ecgroup;

  ecgroup = EC_GROUP_new_by_curve_name(NID_secp256k1);

  int msg_len = 100;
  uint8_t * msg = (uint8_t *) DeepState_CStrUpToLen(100);
  LOG(TRACE) << msg;

/*
  uint8_t msg[256];
  int msg_len = (random32() & 0xFF) + 1;
  random_buffer(msg, msg_len);
  LOG(TRACE) << msg;
*/

  // new ECDSA key
  EC_KEY *eckey = EC_KEY_new();
  EC_KEY_set_group(eckey, ecgroup);

  // generate the key
  EC_KEY_generate_key(eckey);

  // copy key to buffer
  const BIGNUM *K = EC_KEY_get0_private_key(eckey);
  int bn_off = sizeof(priv_key) - BN_num_bytes(K);
  memzero(priv_key, bn_off);
  BN_bn2bin(K, priv_key + bn_off);


  // use our ECDSA signer to sign the message with the key
  ASSERT_EQ(ecdsa_sign(curve, HASHER_SHA2, priv_key, msg, msg_len, sig, NULL, NULL), 0) \
	<< "trezor-crypto ecdsa signing failed";

  // generate public key from private key
  ecdsa_get_public_key33(curve, priv_key, pub_key33);
  ecdsa_get_public_key65(curve, priv_key, pub_key65);

  // use our ECDSA verifier to verify the message signature
  ASSERT_EQ(ecdsa_verify(curve, HASHER_SHA2, pub_key65, sig, msg, msg_len), 0) \
    << "trezor-crypto verification failed (pub_key_len = 65)";
  ASSERT_EQ(ecdsa_verify(curve, HASHER_SHA2, pub_key33, sig, msg, msg_len), 0) \
	<< "trezor-crypto verification failed (pub_key_len = 33)";

  // copy signature to the OpenSSL struct
  ECDSA_SIG *signature = ECDSA_SIG_new();
  BIGNUM *R = BN_bin2bn(sig, 32, NULL);
  BIGNUM *S = BN_bin2bn(sig + 32, 32, NULL);
  ECDSA_SIG_set0(signature, R, S);

  // compute the digest of the message
  // note: these are OpenSSL functions, not our own
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, msg, msg_len);
  SHA256_Final(hash, &sha256);

  // verify all went well, i.e. we can decrypt our signature with OpenSSL
  int v = ECDSA_do_verify(hash, 32, signature, eckey);
  ASSERT_EQ(v, 1) << "OpenSSL verification failed";

  ECDSA_SIG_free(signature);
  EC_KEY_free(eckey);
  EC_GROUP_free(ecgroup);
}

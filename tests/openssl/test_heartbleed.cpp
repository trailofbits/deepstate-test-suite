/*
 * test_heartbleed (CVE-2014-0160)
 *
 *   Reproduces heap buffer-overflow that induces Heartbleed vulnerability in
 *	 OpenSSL 1.0.1f.
 *
 *   Derived from: https://github.com/google/fuzzer-test-suite/blob/master/openssl-1.0.1f/target.cc
 */

extern "C" {
	#include <openssl/ssl.h>
	#include <openssl/err.h>
	#include <assert.h>
	#include <stdint.h>
	#include <stddef.h>
}

#include <deepstate/DeepState.hpp>

using namespace deepstate;


SSL_CTX *Init() {
  SSL_library_init();
  SSL_load_error_strings();
  ERR_load_BIO_strings();
  OpenSSL_add_all_algorithms();

  SSL_CTX *sctx;
  ASSERT_EQ(sctx, SSL_CTX_new(TLSv1_method()));
  ASSERT_EQ(SSL_CTX_use_certificate_file(sctx, "runtime/server.pem",
                                      SSL_FILETYPE_PEM), 0);
  ASSERT_EQ(SSL_CTX_use_PrivateKey_file(sctx, "runtime/server.key",
                                     SSL_FILETYPE_PEM), 0);
  return sctx;
}


TEST(OpenSSL, Heartbleed) {

  char * data = DeepState_CStrUpToLen(512);
  size_t len = strlen(data);

  static SSL_CTX *sctx = Init();
  SSL *server = SSL_new(sctx);

  BIO *sinbio = BIO_new(BIO_s_mem());
  BIO *soutbio = BIO_new(BIO_s_mem());

  SSL_set_bio(server, sinbio, soutbio);
  SSL_set_accept_state(server);

  BIO_write(sinbio, data, len);

  SSL_do_handshake(server);
  SSL_free(server);
}

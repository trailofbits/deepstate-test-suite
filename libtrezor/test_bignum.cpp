/*
 * test_bignum
 *
 *   Defines test cases for Trezor Crypto bignum
 *   operations with DeepState.
 */

#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <check.h>

#include <deepstate/DeepState.hpp>

extern "C" {
	#include "trezor-crypto/check_mem.h"
	#include "trezor-crypto/options.h"
	#include "trezor-crypto/address.h"
	#include "trezor-crypto/bignum.h"
	#include "trezor-crypto/memzero.h"
}

#define FROMHEX_MAXLEN 512

using namespace deepstate;


/*
 * Helper: takes a char str (up to FROMHEX_MAXLEN), and convert it in
 *  	   bytes in a uint8_t that bignum functions can parse and read.
 *
 */
DEEPSTATE_NOINLINE const uint8_t *fromhex(const char *str) {
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

/*
 * Helper: reverses a char str for big-end and little-end testing using a
 *		   XOR_SWAP macro.
 *	       (see: https://stackoverflow.com/questions/784417/reversing-a-string-in-c)
 */

DEEPSTATE_NOINLINE void str_reverse(char * str)
{
  char * end = str + strlen(str) - 1;

  #define XOR_SWAP(a,b) do \
  { \
    a ^= b; \
    b ^= a; \
    a ^= b; \
  } while (0)

  while (str < end) {
    XOR_SWAP(*str, *end);
    str++;
    end--;
  }
  #undef XOR_SWAP
}


/*
 * BigEndReadWrite
 *
 *	Tests input and output from reading and writing a big-end normalized
 *  uint256 bignum.
 *
*/
TEST(TrezorCryptoBignum, BigEndReadWrite) {
  bignum256 a;

  uint8_t input[32];
  uint8_t out[32];

  char * in_buf = DeepState_CStrUpToLen(FROMHEX_MAXLEN, "abcdef0123456789");

  memcpy(input, fromhex(in_buf), 32);

  bn_read_be(input, &a);
  bn_write_be(&a, out);

  for (int i = 0; i < 32; i++)
    ASSERT_EQ(input[i], out[i])
		<< input[i] << "does not equal" << out[i];
}


/*
 * LittleEndReadWrite
 *
 *	Tests input and output from reading and writing a little-end normalized
 *  uint256 bignum between primitive types.
 *
*/
TEST(TrezorCryptoBignum, LittleEndReadWrite) {
  bignum256 a;

  uint8_t input[32];
  uint8_t out[32];

  char * in_buf = DeepState_CStrUpToLen(FROMHEX_MAXLEN, "abcdef0123456789");
  memcpy(input, fromhex(in_buf), 32);

  bn_read_le(input, &a);
  bn_write_le(&a, out);

  for (int i = 0; i < 32; i++)
    ASSERT_EQ(input[i], out[i])
		<< input[i] << "does not equal" << out[i];
}


/*
 * Equal
 *
 *  Tests for bignum inputs that may not equal to itself.
 *
*/
TEST(TrezorCryptoBignum, Equal) {
  bignum256 a;
  char * in_buf = DeepState_CStrUpToLen(FROMHEX_MAXLEN, "abcdef0123456789");

  bn_read_be(fromhex(in_buf), &a);
  ASSERT_EQ(bn_is_equal(&a, &a), 1)
	<< "bignum is not equal to itself";
}


/*
 * EqualVectors
 *
 *  Concrete test vectors for testing bn_is_equal.
 *
*/
TEST(TrezorCryptoBignum, EqualVectors) {
  bignum256 a = {{0x286d8bd5, 0x380c7c17, 0x3c6a2ec1, 0x2d787ef5, 0x14437cd3,
                  0x25a043f8, 0x1dd5263f, 0x33a162c3, 0x0000c55e}};
  bignum256 b = {{0x286d8bd5, 0x380c7c17, 0x3c6a2ec1, 0x2d787ef5, 0x14437cd3,
                  0x25a043f8, 0x1dd5263f, 0x33a162c3, 0x0000c55e}};
  bignum256 c = {{
      0,
  }};

  ASSERT_EQ(bn_is_equal(&a, &b), 1);
  ASSERT_EQ(bn_is_equal(&c, &c), 1);
  ASSERT_EQ(bn_is_equal(&a, &c), 0);
}


/*
 * Zero
 *
 *  Tests for possible edge cases that might be equal to
 *  a zero bignum.
 *
*/
TEST(TrezorCryptoBignum, Zero) {
  bignum256 a;
  bignum256 zero;

  char * in_buf = DeepState_CStrUpToLen(FROMHEX_MAXLEN, "abcdef0123456789");

  bn_read_be(fromhex(in_buf), &a);
  bn_zero(&zero);

  /* the input should NOT be equal to zero. */
  /* only input that should cause this to FAIL is a zero input */
  ASSERT_EQ(bn_is_equal(&a, &zero), 0)
	<< "a non-zero bignum is equal to a zero bignum";
}


/*
 * IsZero
 *
 *  Same as Zero test, but uses bn_is_zero to check bignums.
 *
*/
TEST(TrezorCryptoBignum, IsZero) {
  bignum256 a;

  char * in_buf = DeepState_CStrUpToLen(FROMHEX_MAXLEN, "abcdef0123456789");

  /* the input should NOT be equal to zero */
  /* only input that should cause this to FAIL is a zero input */
  bn_read_be(fromhex(in_buf), &a);
  ASSERT_EQ(bn_is_zero(&a), 1);
}


/*
 * IsZeroVectors
 *
 *  Concrete test vectors for testing bn_is_zero.
 *
*/
TEST(TrezorCryptoBignum, IsZeroVectors) {
  bignum256 a;

  /* concrete tests */
  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000000000000"),
      &a);
  ASSERT_EQ(bn_is_zero(&a), 1);

  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000000000001"),
      &a);
  ASSERT_EQ(bn_is_zero(&a), 0);

  bn_read_be(
      fromhex(
          "1000000000000000000000000000000000000000000000000000000000000000"),
      &a);
  ASSERT_EQ(bn_is_zero(&a), 0);

  bn_read_be(
      fromhex(
          "f000000000000000000000000000000000000000000000000000000000000000"),
      &a);
  ASSERT_EQ(bn_is_zero(&a), 0);
}


/*
 * One
 *
 *  Tests for possible edge cases that might be equal to
 *  a bignum 1.
 *
*/
TEST(TrezorCryptoBignum, One) {
  bignum256 a;
  bignum256 one;

  char * in_buf = DeepState_CStrUpToLen(FROMHEX_MAXLEN, "abcdef0123456789");

  bn_read_be(fromhex(in_buf), &a);
  bn_one(&one);

  /* the input should NOT be equal to one. */
  /* only input that should cause this to FAIL is a one input */
  ASSERT_EQ(bn_is_equal(&a, &one), 0)
	<< "a non-one bignum is equal to 1";
}


/*
 * ReadBigLittle (TODO: debug)
 *
 *  Tests to see if big-end and little-end inputs
 *	are valid.
 *
*/
TEST(TrezorCryptoBignum, ReadBigLittle) {
  bignum256 be;
  bignum256 le;

  char * in_buf = DeepState_CStrUpToLen(FROMHEX_MAXLEN, "abcdef0123456789");

  LOG(DEBUG) << "Big endian:" << in_buf;
  bn_read_be(fromhex(in_buf), &be);

  str_reverse(in_buf);

  LOG(DEBUG) << "Little endian:" << in_buf;
  bn_read_le(fromhex(in_buf), &le);

  ASSERT_EQ(bn_is_equal(&be, &le), 1)
    << "big-endian value is not equal to little-endian value";
}


/*
 * ReadBigLittleVectors
 *
 *  Concrete test vectors for testing between
 *	bn_read_le and bn_read_be.
 *
*/
TEST(TrezorCryptoBignum, ReadBigLittleVectors) {
  bignum256 a;
  bignum256 b;

  /* concrete tests */
  bn_read_be(
      fromhex(
          "c55ece858b0ddd5263f96810fe14437cd3b5e1fbd7c6a2ec1e031f05e86d8bd5"),
      &a);
  bn_read_le(
      fromhex(
          "d58b6de8051f031eeca2c6d7fbe1b5d37c4314fe1068f96352dd0d8b85ce5ec5"),
      &b);

  ASSERT_EQ(bn_is_equal(&a, &b), 1);
}


/*
 * LittleEndWrite
 *
 *  Tests mixing in reading and writing input between big-end and little-end
 *  bignums.
 *
*/
TEST(TrezorCryptoBignum, LittleEndWrite) {
  bignum256 a;
  bignum256 b;

  uint8_t tmp[32];
  char * in_buf = DeepState_CStrUpToLen(FROMHEX_MAXLEN, "abcdef0123456789");

  bn_read_be(fromhex(in_buf), &a);
  bn_write_le(&a, tmp);

  bn_read_le(tmp, &b);
  ASSERT_EQ(bn_is_equal(&a, &b), 1);

  str_reverse(in_buf);

  bn_read_be(fromhex(in_buf), &a);
  bn_read_be(tmp, &b);
  ASSERT_EQ(bn_is_equal(&a, &b), 1);
}



/*
 * Uint32ReadWrite
 *
 *	Tests input and output from reading and writing a big-end normalized
 *  uint256 bignum from a uint32.
 *
*/
TEST(TrezorCryptoBignum, Uint32ReadWrite) {
  bignum256 a;

  uint32_t input = DeepState_UInt();
  uint32_t out;

  bn_read_uint32(input, &a);
  out = bn_write_uint32(&a);

  ASSERT_EQ(input, out);
}


/*
 * ReadUint32Vectors
 *
 *  Concrete test vectors for testing between reading
 *  str and uint32 type conversions to bignums.
*/
TEST(TrezorCryptoBignum, ReadUint32Vectors) {
  bignum256 a;
  bignum256 b;

  // lowest 30 bits set
  bn_read_be(
      fromhex(
          "000000000000000000000000000000000000000000000000000000003fffffff"),
      &a);
  bn_read_uint32(0x3fffffff, &b);

  ASSERT_EQ(bn_is_equal(&a, &b), 1);

  // bit 31 set
  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000040000000"),
      &a);
  bn_read_uint32(0x40000000, &b);
  ASSERT_EQ(bn_is_equal(&a, &b), 1);
}


/*
 * WriteUint32Vectors
 *
 *  Concrete test vectors for testing converted str
 *  bignums and output uint32s.
*/
TEST(TrezorCryptoBignum, WriteUint32Vectors) {
  bignum256 a;

  // lowest 30 bits set
  bn_read_be(
      fromhex(
          "000000000000000000000000000000000000000000000000000000003fffffff"),
      &a);
  ASSERT_EQ(bn_write_uint32(&a), 0x3fffffff);

  // bit 31 set
  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000040000000"),
      &a);
  ASSERT_EQ(bn_write_uint32(&a), 0x40000000);
}


/*
 * Uint64ReadWrite
 *
 *	Tests input and output from reading and writing a big-end normalized
 *  uint256 bignum from a uint64.
 *
*/
TEST(TrezorCryptoBignum, Uint64ReadWrite) {
  bignum256 a;

  uint64_t input = DeepState_UInt64();
  uint64_t out;

  bn_read_uint64(input, &a);
  out = bn_write_uint64(&a);

  ASSERT_EQ(input, out);
}


/*
 * ReadUint64Vectors
 *
 *  Concrete test vectors for testing between reading
 *  str and uint64 type conversions to bignums.
*/
TEST(TrezorCryptoBignum, ReadUint64Vectors) {
  bignum256 a;
  bignum256 b;

  // lowest 30 bits set
  bn_read_be(
      fromhex(
          "000000000000000000000000000000000000000000000000000000003fffffff"),
      &a);
  bn_read_uint64(0x3fffffff, &b);
  ASSERT_EQ(bn_is_equal(&a, &b), 1);

  // bit 31 set
  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000040000000"),
      &a);
  bn_read_uint64(0x40000000, &b);
  ASSERT_EQ(bn_is_equal(&a, &b), 1);

  // bit 33 set
  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000100000000"),
      &a);
  bn_read_uint64(0x100000000LL, &b);
  ASSERT_EQ(bn_is_equal(&a, &b), 1);

  // bit 61 set
  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000002000000000000000"),
      &a);
  bn_read_uint64(0x2000000000000000LL, &b);
  ASSERT_EQ(bn_is_equal(&a, &b), 1);

  // all 64 bits set
  bn_read_be(
      fromhex(
          "000000000000000000000000000000000000000000000000ffffffffffffffff"),
      &a);
  bn_read_uint64(0xffffffffffffffffLL, &b);
  ASSERT_EQ(bn_is_equal(&a, &b), 1);
}


/*
 * WriteUint64Vectors
 *
 *  Concrete test vectors for testing converted str
 *  bignums and output uint64s.
*/
TEST(TrezorCryptoBignum, WriteUint64Vectors) {
  bignum256 a;

  // lowest 30 bits set
  bn_read_be(
      fromhex(
          "000000000000000000000000000000000000000000000000000000003fffffff"),
      &a);
  ASSERT_EQ(bn_write_uint64(&a), 0x3fffffff);

  // bit 31 set
  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000040000000"),
      &a);
  ASSERT_EQ(bn_write_uint64(&a), 0x40000000);

  // bit 33 set
  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000100000000"),
      &a);
  ASSERT_EQ(bn_write_uint64(&a), 0x100000000LL);

  // bit 61 set
  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000002000000000000000"),
      &a);
  ASSERT_EQ(bn_write_uint64(&a), 0x2000000000000000LL);

  // all 64 bits set
  bn_read_be(
      fromhex(
          "000000000000000000000000000000000000000000000000ffffffffffffffff"),
      &a);
  ASSERT_EQ(bn_write_uint64(&a), 0xffffffffffffffffLL);
}



/*
 * Copy
 *
 *  Tests copying a bignum and ensuring it is the same value.
 *
*/
TEST(TrezorCryptoBignum, Copy) {
  bignum256 a;
  bignum256 b;

  char * in_buf = DeepState_CStrUpToLen(FROMHEX_MAXLEN, "abcdef0123456789");

  bn_read_be(fromhex(in_buf), &a);
  bn_copy(&a, &b);
  ASSERT_EQ(bn_is_equal(&a, &b), 1);
}


// TODO:
TEST(TrezorCryptoBignum, IsEven) {


}

// TODO:
TEST(TrezorCryptoBignum, IsOdd) {


}

/*
 * IsEvenVectors
 *
 *  Concrete test vectors for testing even bignums.
 *
*/
TEST(TrezorCryptoBignum, IsEvenVectors) {
  bignum256 a;

  bn_read_be(
      fromhex(
          "c55ece858b0ddd5263f96810fe14437cd3b5e1fbd7c6a2ec1e031f05e86d8bd5"),
      &a);
  ASSERT_EQ(bn_is_even(&a), 0);

  bn_read_be(
      fromhex(
          "c55ece858b0ddd5263f96810fe14437cd3b5e1fbd7c6a2ec1e031f05e86d8bd2"),
      &a);
  ASSERT_EQ(bn_is_even(&a), 1);

  bn_read_be(
      fromhex(
          "c55ece858b0ddd5263f96810fe14437cd3b5e1fbd7c6a2ec1e031f05e86d8bd0"),
      &a);
  ASSERT_EQ(bn_is_even(&a), 1);
}


/*
 * IsOddVectors
 *
 *  Concrete test vectors for testing odd bignums.
 *
*/
TEST(TrezorCryptoBignum, TestBignumIsOdd) {
  bignum256 a;

  bn_read_be(
      fromhex(
          "c55ece858b0ddd5263f96810fe14437cd3b5e1fbd7c6a2ec1e031f05e86d8bd5"),
      &a);
  ASSERT_EQ(bn_is_odd(&a), 1);

  bn_read_be(
      fromhex(
          "c55ece858b0ddd5263f96810fe14437cd3b5e1fbd7c6a2ec1e031f05e86d8bd2"),
      &a);
  ASSERT_EQ(bn_is_odd(&a), 0);

  bn_read_be(
      fromhex(
          "c55ece858b0ddd5263f96810fe14437cd3b5e1fbd7c6a2ec1e031f05e86d8bd0"),
      &a);
  ASSERT_EQ(bn_is_odd(&a), 0);
}


TEST(TrezorCryptoBignum, TestBignumIsLess) {
  bignum256 a;
  bignum256 b;

  bn_read_uint32(0x1234, &a);
  bn_read_uint32(0x8765, &b);

  ASSERT_EQ(bn_is_less(&a, &b), 1);
  ASSERT_EQ(bn_is_less(&b, &a), 0);

  bn_zero(&a);
  bn_read_be(
      fromhex(
          "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"),
      &b);

  ASSERT_EQ(bn_is_less(&a, &b), 1);
  ASSERT_EQ(bn_is_less(&b, &a), 0);
}

TEST(TrezorCryptoBignum, TestBignumBitcount) {
  bignum256 a, b;

  bn_zero(&a);
  ASSERT_EQ(bn_bitcount(&a), 0);

  bn_one(&a);
  ASSERT_EQ(bn_bitcount(&a), 1);

  // test for 10000 and 11111 when i=5
  for (int i = 2; i <= 256; i++) {
    bn_one(&a);
    bn_one(&b);
    for (int j = 2; j <= i; j++) {
      bn_lshift(&a);
      bn_lshift(&b);
      bn_addi(&b, 1);
    }
    ASSERT_EQ(bn_bitcount(&a), i);
    ASSERT_EQ(bn_bitcount(&b), i);
  }

  bn_read_uint32(0x3fffffff, &a);
  ASSERT_EQ(bn_bitcount(&a), 30);

  bn_read_uint32(0xffffffff, &a);
  ASSERT_EQ(bn_bitcount(&a), 32);

  bn_read_be(
      fromhex(
          "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"),
      &a);
  ASSERT_EQ(bn_bitcount(&a), 256);
}

TEST(TrezorCryptoBignum, TestBignumDigitCount) {
  bignum256 a;

  bn_zero(&a);
  ASSERT_EQ(bn_digitcount(&a), 1);

  // test for (10^i) and (10^i) - 1
  uint64_t m = 1;
  for (int i = 0; i <= 19; i++, m *= 10) {
    bn_read_uint64(m, &a);
    ASSERT_EQ(bn_digitcount(&a), i + 1);

    uint64_t n = m - 1;
    bn_read_uint64(n, &a);
    ASSERT_EQ(bn_digitcount(&a), n == 0 ? 1 : i);
  }

  bn_read_uint32(0x3fffffff, &a);
  ASSERT_EQ(bn_digitcount(&a), 10);

  bn_read_uint32(0xffffffff, &a);
  ASSERT_EQ(bn_digitcount(&a), 10);

  bn_read_be(
      fromhex(
          "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"),
      &a);
  ASSERT_EQ(bn_digitcount(&a), 78);
}


TEST(TrezorCryptoBignum, Format64) {
  char buf[128], str[128];
  int r;

  uint64_t m = 1;
  for (int i = 0; i <= 19; i++, m *= 10) {
    r = bn_format_uint64(m, NULL, NULL, 0, 0, false, buf, sizeof(buf));
    ASSERT_EQ(r, strlen(str));
    ASSERT_EQ(buf, str);

    uint64_t n = m - 1;
    r = bn_format_uint64(n, NULL, NULL, 0, 0, false, buf, sizeof(buf));
    ASSERT_EQ(r, strlen(str));
    ASSERT_EQ(buf, str);
  }
}


TEST(TrezorCryptoBignum, FormatVectors) {
  bignum256 a;
  char buf[128];
  int r;

  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000000000000"),
      &a);
  r = bn_format(&a, NULL, NULL, 0, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 1);
  ASSERT_EQ(buf, "0");

  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000000000000"),
      &a);
  r = bn_format(&a, NULL, NULL, 20, 0, true, buf, sizeof(buf));
  ASSERT_EQ(r, 22);
  ASSERT_EQ(buf, "0.00000000000000000000");

  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000000000000"),
      &a);
  r = bn_format(&a, NULL, NULL, 0, 5, false, buf, sizeof(buf));
  ASSERT_EQ(r, 1);
  ASSERT_EQ(buf, "0");

  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000000000000"),
      &a);
  r = bn_format(&a, NULL, NULL, 0, -5, false, buf, sizeof(buf));
  ASSERT_EQ(r, 1);
  ASSERT_EQ(buf, "0");

  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000000000000"),
      &a);
  r = bn_format(&a, "", "", 0, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 1);
  ASSERT_EQ(buf, "0");

  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000000000000"),
      &a);
  r = bn_format(&a, NULL, "SFFX", 0, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 1 + 4);
  ASSERT_EQ(buf, "0SFFX");

  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000000000000"),
      &a);
  r = bn_format(&a, "PRFX", NULL, 0, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 4 + 1);
  ASSERT_EQ(buf, "PRFX0");

  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000000000000"),
      &a);
  r = bn_format(&a, "PRFX", "SFFX", 0, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 4 + 1 + 4);
  ASSERT_EQ(buf, "PRFX0SFFX");

  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000000000000"),
      &a);
  r = bn_format(&a, NULL, NULL, 18, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 3);
  ASSERT_EQ(buf, "0.0");

  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000000000001"),
      &a);
  r = bn_format(&a, NULL, NULL, 0, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 1);
  ASSERT_EQ(buf, "1");

  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000000000001"),
      &a);
  r = bn_format(&a, NULL, NULL, 6, 6, true, buf, sizeof(buf));
  ASSERT_EQ(r, 8);
  ASSERT_EQ(buf, "1.000000");

  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000000000002"),
      &a);
  r = bn_format(&a, NULL, NULL, 0, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 1);
  ASSERT_EQ(buf, "2");

  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000000000005"),
      &a);
  r = bn_format(&a, NULL, NULL, 0, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 1);
  ASSERT_EQ(buf, "5");

  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000000000009"),
      &a);
  r = bn_format(&a, NULL, NULL, 0, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 1);
  ASSERT_EQ(buf, "9");

  bn_read_be(
      fromhex(
          "000000000000000000000000000000000000000000000000000000000000000a"),
      &a);
  r = bn_format(&a, NULL, NULL, 0, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 2);
  ASSERT_EQ(buf, "10");

  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000000000014"),
      &a);
  r = bn_format(&a, NULL, NULL, 0, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 2);
  ASSERT_EQ(buf, "20");

  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000000000032"),
      &a);
  r = bn_format(&a, NULL, NULL, 0, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 2);
  ASSERT_EQ(buf, "50");

  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000000000063"),
      &a);
  r = bn_format(&a, NULL, NULL, 0, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 2);
  ASSERT_EQ(buf, "99");

  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000000000064"),
      &a);
  r = bn_format(&a, NULL, NULL, 0, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 3);
  ASSERT_EQ(buf, "100");

  bn_read_be(
      fromhex(
          "00000000000000000000000000000000000000000000000000000000000000c8"),
      &a);
  r = bn_format(&a, NULL, NULL, 0, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 3);
  ASSERT_EQ(buf, "200");

  bn_read_be(
      fromhex(
          "00000000000000000000000000000000000000000000000000000000000001f4"),
      &a);
  r = bn_format(&a, NULL, NULL, 0, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 3);
  ASSERT_EQ(buf, "500");

  bn_read_be(
      fromhex(
          "00000000000000000000000000000000000000000000000000000000000003e7"),
      &a);
  r = bn_format(&a, NULL, NULL, 0, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 3);
  ASSERT_EQ(buf, "999");

  bn_read_be(
      fromhex(
          "00000000000000000000000000000000000000000000000000000000000003e8"),
      &a);
  r = bn_format(&a, NULL, NULL, 0, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 4);
  ASSERT_EQ(buf, "1000");

  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000000000000989680"),
      &a);
  r = bn_format(&a, NULL, NULL, 7, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 3);
  ASSERT_EQ(buf, "1.0");

  bn_read_be(
      fromhex(
          "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"),
      &a);
  r = bn_format(&a, NULL, NULL, 0, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 78);
  ASSERT_EQ(buf,
                   "11579208923731619542357098500868790785326998466564056403945"
                   "7584007913129639935");

  bn_read_be(
      fromhex(
          "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"),
      &a);
  r = bn_format(&a, NULL, NULL, 1, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 79);
  ASSERT_EQ(buf,
                   "11579208923731619542357098500868790785326998466564056403945"
                   "758400791312963993.5");

  bn_read_be(
      fromhex(
          "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"),
      &a);
  r = bn_format(&a, NULL, NULL, 2, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 79);
  ASSERT_EQ(buf,
                   "11579208923731619542357098500868790785326998466564056403945"
                   "75840079131296399.35");

  bn_read_be(
      fromhex(
          "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"),
      &a);
  r = bn_format(&a, NULL, NULL, 8, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 79);
  ASSERT_EQ(buf,
                   "11579208923731619542357098500868790785326998466564056403945"
                   "75840079131.29639935");

  bn_read_be(
      fromhex(
          "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffe3bbb00"),
      &a);
  r = bn_format(&a, NULL, NULL, 8, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 72);
  ASSERT_EQ(buf,
                   "11579208923731619542357098500868790785326998466564056403945"
                   "75840079131.0");

  bn_read_be(
      fromhex(
          "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"),
      &a);
  r = bn_format(&a, NULL, NULL, 18, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 79);
  ASSERT_EQ(buf,
                   "11579208923731619542357098500868790785326998466564056403945"
                   "7.584007913129639935");

  bn_read_be(
      fromhex(
          "fffffffffffffffffffffffffffffffffffffffffffffffff7e52fe5afe40000"),
      &a);
  r = bn_format(&a, NULL, NULL, 18, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 62);
  ASSERT_EQ(
      buf, "115792089237316195423570985008687907853269984665640564039457.0");

  bn_read_be(
      fromhex(
          "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"),
      &a);
  r = bn_format(&a, NULL, NULL, 78, 0, false, buf, sizeof(buf));
  ASSERT_EQ(r, 80);
  ASSERT_EQ(buf,
                   "0."
                   "11579208923731619542357098500868790785326998466564056403945"
                   "7584007913129639935");

  bn_read_be(
      fromhex(
          "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"),
      &a);
  r = bn_format(&a, NULL, NULL, 0, 10, false, buf, sizeof(buf));
  ASSERT_EQ(r, 88);
  ASSERT_EQ(buf,
                   "11579208923731619542357098500868790785326998466564056403945"
                   "75840079131296399350000000000");

  bn_read_be(
      fromhex(
          "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"),
      &a);
  r = bn_format(&a, "quite a long prefix", "even longer suffix", 60, 0, false,
                buf, sizeof(buf));
  ASSERT_EQ(r, 116);
  ASSERT_EQ(buf,
                   "quite a long "
                   "prefix115792089237316195."
                   "42357098500868790785326998466564056403945758400791312963993"
                   "5even longer suffix");

  bn_read_be(
      fromhex(
          "0000000000000000000000000000000000000000000000000123456789abcdef"),
      &a);
  memset(buf, 'a', sizeof(buf));
  r = bn_format(&a, "prefix", "suffix", 10, 0, false, buf, 31);
  ASSERT_EQ(buf, "prefix8198552.9216486895suffix");
  ASSERT_EQ(r, 30);

  memset(buf, 'a', sizeof(buf));
  r = bn_format(&a, "prefix", "suffix", 10, 0, false, buf, 30);
  ASSERT_EQ(r, 0);
  ASSERT_EQ(buf, "prefix198552.9216486895suffix");
}


TEST(TrezorCryptoBignum, test_bignum_divmod) {
  uint32_t r;
  int i;

  bignum256 a = {{0x3fffffff, 0x3fffffff, 0x3fffffff, 0x3fffffff, 0x3fffffff,
                  0x3fffffff, 0x3fffffff, 0x3fffffff, 0xffff}};
  uint32_t ar[] = {15, 14, 55, 29, 44, 24, 53, 49, 18, 55, 2,  28, 5,  4,  12,
                   43, 18, 37, 28, 14, 30, 46, 12, 11, 17, 10, 10, 13, 24, 45,
                   4,  33, 44, 42, 2,  46, 34, 43, 45, 28, 21, 18, 13, 17};

  i = 0;
  while (!bn_is_zero(&a) && i < 44) {
    bn_divmod58(&a, &r);
    ASSERT_EQ(r, ar[i]);
    i++;
  }
  ASSERT_EQ(i, 44);

  bignum256 b = {{0x3fffffff, 0x3fffffff, 0x3fffffff, 0x3fffffff, 0x3fffffff,
                  0x3fffffff, 0x3fffffff, 0x3fffffff, 0xffff}};
  uint32_t br[] = {935, 639, 129, 913, 7,   584, 457, 39, 564,
                   640, 665, 984, 269, 853, 907, 687, 8,  985,
                   570, 423, 195, 316, 237, 89,  792, 115};

  i = 0;
  while (!bn_is_zero(&b) && i < 26) {
    bn_divmod1000(&b, &r);
    ASSERT_EQ(r, br[i]);
    i++;
  }
  ASSERT_EQ(i, 26);
}

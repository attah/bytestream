#include "bytestream.h"
#include "minitest/test.h"
#include <cstring>
using namespace std;

TEST(get_operator)
{
  Bytestream bts;
  bts << (uint8_t)1 << (uint16_t)2 << (uint32_t)3 << (uint64_t)4
      << (int8_t)-1 << (int16_t)-2 << (int32_t)-3 << (int64_t)-4
      << "someString";

  uint8_t a = 0;
  uint16_t b = 0;
  uint32_t c = 0;
  uint64_t d = 0;
  int8_t e = 0;
  int16_t f = 0;
  int32_t g = 0;
  int64_t h = 0;
  std::string s;

  bts >> a >> b >> c >> d >> e >> f >> g >> h;
  bts/10 >> s;
  Bytestream bts2;
  bts-=10;
  bts/10 >> bts2;

  ASSERT(a==1);
  ASSERT(b==2);
  ASSERT(c==3);
  ASSERT(d==4);
  ASSERT(e==-1);
  ASSERT(f==-2);
  ASSERT(g==-3);
  ASSERT(h==-4);
  ASSERT(s=="someString");
  ASSERT(bts2>>="someString");
}

TEST(get_methods)
{
  Bytestream bts;
  bts << (uint8_t)1 << (uint16_t)2 << (uint32_t)3 << (uint64_t)4
      << (int8_t)-1 << (int16_t)-2 << (int32_t)-3 << (int64_t)-4
      << "someString";

  ASSERT(bts.peekU8()==1);
  ASSERT(bts.getU8()==1);
  ASSERT(bts.peekU16()==2);
  ASSERT(bts.getU16()==2);
  ASSERT(bts.peekU32()==3);
  ASSERT(bts.getU32()==3);
  ASSERT(bts.peekU64()==4);
  ASSERT(bts.getU64()==4);
  ASSERT(bts.peekS8()==-1);
  ASSERT(bts.getS8()==-1);
  ASSERT(bts.peekS16()==-2);
  ASSERT(bts.getS16()==-2);
  ASSERT(bts.peekS32()==-3);
  ASSERT(bts.getS32()==-3);
  ASSERT(bts.peekS64()==-4);
  ASSERT(bts.getS64()==-4);

  ASSERT((bts/10).peekString()=="someString");
  ASSERT((bts/10).getString()=="someString");
  bts-=10;
  Bytestream bts2a = (bts/10).peekBytestream();
  Bytestream bts2b = (bts/10).getBytestream();
  ASSERT(bts2a==bts2b);
  ASSERT(bts2b>>="someString");
  bts-=10;
  ASSERT(bts.peekString(10)=="someString");
  ASSERT(bts.getString(10)=="someString");
  bts-=10;
  Bytestream bts3a = bts.peekBytestream(10);
  Bytestream bts3b = bts.getBytestream(10);
  ASSERT(bts3a==bts3b);
  ASSERT(bts3b>>="someString");

  bts-=10;
  Bytestream bts4a;
  Bytestream bts4b;
  bts.setNoOfNextBytes(7);
  ASSERT_THROW(bts4a = bts.getBytestream(10), logic_error);
  ASSERT_THROW(bts4a = bts.peekBytestream(10), logic_error);
  bts.setNoOfNextBytes(10);
  bts4b = bts.peekBytestream(10);
  bts4a = bts.getBytestream(10);
  ASSERT(bts4a==bts4b);

  bts-=10;
  Bytestream bts5;
  bts.getBytes(bts5, 10);
  ASSERT(bts5 == bts4a);

  bts-=10;
  char tmp[10];
  bts.getBytes(tmp, 10);
  Bytestream bts5b(tmp, 10);
  ASSERT(bts5b == bts5);
}

TEST(const_types)
{
  Bytestream bts;
  bts << (uint8_t)1 << (uint16_t)2 << (uint32_t)3 << (uint64_t)4
      << (int8_t)-1 << (int16_t)-2 << (int32_t)-3 << (int64_t)-4
      << "someString";

  bts >> (uint8_t)1 >> (uint16_t)2 >> (uint32_t)3 >> (uint64_t)4;
  bts >> (int8_t)-1 >> (int16_t)-2 >> (int32_t)-3 >> (int64_t)-4;
  bts >> "someString";
  bts -= 10;
  bts/10 >> "someString";
}

TEST(test_operator)
{
  Bytestream bts;
  bts << (uint8_t)1 << (uint16_t)2 << (uint32_t)3 << (uint64_t)4
      << (int8_t)-1 << (int16_t)-2 << (int32_t)-3 << (int64_t)-4
      << "someString";

  // Non-matches should not advance the read offset, matches should
  ASSERT_FALSE(bts >>= (uint8_t)0);
  ASSERT(bts >>= (uint8_t)1);
  ASSERT_FALSE(bts >>= (uint16_t)0);
  ASSERT(bts >>= (uint16_t)2);
  ASSERT_FALSE(bts >>= (uint32_t)0);
  ASSERT(bts >>= (uint32_t)3);
  ASSERT_FALSE(bts >>= (uint64_t)0);
  ASSERT(bts >>= (uint64_t)4);
  ASSERT_FALSE(bts >>= (int8_t)0);
  ASSERT(bts >>= (int8_t)-1);
  ASSERT_FALSE(bts >>= (int16_t)0);
  ASSERT(bts >>= (int16_t)-2);
  ASSERT_FALSE(bts >>= (int32_t)0);
  ASSERT(bts >>= (int32_t)-3);
  ASSERT_FALSE(bts >>= (int64_t)0);
  ASSERT(bts >>= (int64_t)-4);

  ASSERT_FALSE(bts >>= "smoeString");
  // what would have been reading past end should still return false
  ASSERT_FALSE(bts >>= "notSomeString");
  ASSERT(bts >>= "someString");
  bts -= 10;
  ASSERT(bts/10 >>= "someString");

  bts -= 10;
  Bytestream bts2;
  bts2 << "someString";
  ASSERT(bts >>= bts2);
  ASSERT_FALSE(bts >>= bts2); // already consumed
  ASSERT_FALSE(bts >>= (uint8_t)1); // empty

}

TEST(test_method)
{
  Bytestream bts;
  bts << (uint8_t)1 << (uint16_t)2 << (uint32_t)3 << (uint64_t)4
      << (int8_t)-1 << (int16_t)-2 << (int32_t)-3 << (int64_t)-4
      << "someString";

  // Non-matches should not advance the read offset, matches should
  ASSERT_FALSE(bts.nextU8(0));
  ASSERT(bts.nextU8(1));
  ASSERT_FALSE(bts.nextU16(0));
  ASSERT(bts.nextU16(2));
  ASSERT_FALSE(bts.nextU32(0));
  ASSERT(bts.nextU32(3));
  ASSERT_FALSE(bts.nextU64(0));
  ASSERT(bts.nextU64(4));
  ASSERT_FALSE(bts.nextS8(0));
  ASSERT(bts.nextS8(-1));
  ASSERT_FALSE(bts.nextS16(0));
  ASSERT(bts.nextS16(-2));
  ASSERT_FALSE(bts.nextS32(0));
  ASSERT(bts.nextS32(-3));
  ASSERT_FALSE(bts.nextS64(0));
  ASSERT(bts.nextS64(-4));

  ASSERT_FALSE(bts.nextString("smoeString"));
  // what would have been reading past end should still return false
  ASSERT_FALSE(bts.nextString("notSomeString"));
  ASSERT(bts.nextString("someString"));
  bts -= 10;
  Bytestream bts2;
  bts2 << "someString";
  ASSERT(bts.nextBytestream(bts2));
  ASSERT_FALSE(bts.nextBytestream(bts2)); // already consumed

  bts -= 10;
  Bytestream bts3, bts4;
  bts3 << "sooomeString";
  bts4 << "someStrong";
  // inverting the comparison result should not influence length mismatch
  ASSERT_FALSE(bts.nextBytestream(bts3, false));
  // ...only advance the read position if the bytestreams aren't equal
  ASSERT(bts.nextBytestream(bts4, false));
  ASSERT(bts.atEnd());
  // inverting the comparison result should not influence length mismatch
  ASSERT_FALSE(bts.nextBytestream(bts4, false));

  bts -= 10;
  ASSERT_FALSE(bts.peekNextBytestream(bts3));
  ASSERT_FALSE(bts.atEnd());

  ASSERT(bts.peekNextBytestream(bts2));
  ASSERT_FALSE(bts.atEnd());
}

#define HIBIT(sign, suffix) \
  (sign##suffix)((uint##suffix)1<<(sizeof(sign##suffix)*8-1))

TEST(limits)
{
  Bytestream bts;
      // 0xff...
  bts << (uint8_t)255 << (uint16_t)65535 << (uint32_t)4294967295UL
      << (uint64_t)18446744073709551615ULL
      << (int8_t)-1 << (int16_t)-1 << (int32_t)-1 << (int64_t)-1
      // 0x80...
      << (int8_t)-128 << (int16_t)-32768 << (int32_t)-2147483648UL
      << (int64_t)-9223372036854775808ULL;

      // 0xff...
  bts >> (uint8_t)~0 >> (uint16_t)~0 >> (uint32_t)~0 >> (uint64_t)~0;
  bts >> (int8_t)~0 >> (int16_t)~0 >> (int32_t)~0 >> (int64_t)~0;
  bts -= 15;
  bts >> (uint8_t)~0 >> (uint16_t)~0 >> (uint32_t)~0 >> (uint64_t)~0;

      // 0x80...
  bts >> HIBIT(int,8_t) >> HIBIT(int,16_t) >> HIBIT(int,32_t)
      >> HIBIT(int,64_t);
  bts -= 15;
  bts >> HIBIT(uint,8_t) >> HIBIT(uint,16_t) >> HIBIT(uint,32_t)
      >> HIBIT(uint,64_t);
}

TEST(position_arithmetics)
{
  Bytestream bts;
  bts << (uint8_t)1 << (uint16_t)2 << (uint32_t)3 << (uint64_t)4
      << (int8_t)-1 << (int16_t)-2 << (int32_t)-3 << (int64_t)-4
      << "someString";

  bts += 15;
  ASSERT(bts >>= (int8_t)-1);
  bts -= 9;
  bts >> (uint64_t)4;
  bts = bts[30];
  ASSERT(bts.pos()==0);
  ASSERT(bts.size()==10);
  bts >> "someString";
  ASSERT(bts.atEnd());

  ASSERT_THROW((bts+=1), out_of_range);
  ASSERT_THROW(bts.setPos(bts.pos()+1), out_of_range);
  ASSERT_THROW(bts-=(bts.pos()+1), out_of_range);

  bts.setPos(5);
  ASSERT(bts.pos()==5);
  bts.setPos(10);
  ASSERT(bts.pos()==10);
}

TEST(copying)
{
  Bytestream bts;
  bts << (uint8_t)1 << (uint16_t)2 << (uint32_t)3 << (uint64_t)4
      << (int8_t)-1 << (int16_t)-2 << (int32_t)-3 << (int64_t)-4
      << "someString";
  Bytestream bts2(bts);
  ASSERT(bts2==bts);
  Bytestream bts3 = bts;
  ASSERT(bts3==bts);
  Bytestream bts4;
  bts4 = bts;
  ASSERT(bts4==bts);

  bts += 30;
  Bytestream bts5 = bts;
  ASSERT(bts5==bts);
  ASSERT(bts.pos()==30);
  ASSERT(bts5.pos()==30);
  bts5 = bts5[0];
  ASSERT(bts5.pos()==0);
  ASSERT(bts5.size()==bts.size());
}

TEST(partials)
{
  Bytestream bts;
  bts << (uint8_t)1 << (uint16_t)2 << (uint32_t)3 << (uint64_t)4
      << (int8_t)-1 << (int16_t)-2 << (int32_t)-3 << (int64_t)-4
      << "someString";

  Bytestream bts2;
  bts/bts.size() >> bts2;
  ASSERT(bts2==bts);

  bts-=10;
  ASSERT(bts>>="someString");
  bts-=10;
  Bytestream bts3;
  bts/10 >> bts3;
  ASSERT(bts3>>="someString");
}

TEST(exceptions)
{
  Bytestream bts;
  bts << (uint8_t)1;
  size_t pos_before = bts.pos();
  ASSERT_THROW(bts >> (uint16_t)1, out_of_range);
  ASSERT_THROW(bts >> (uint8_t)2, Bytestream::Badmatch);
  ASSERT(bts.pos()==pos_before);
  ASSERT(bts.noOfNextBytesValid()==false);
  ASSERT_THROW(bts.getBytestream(17), logic_error);
  ASSERT(bts.noOfNextBytesValid()==false);
  ASSERT_THROW(bts.getString(2), out_of_range);
  ASSERT(bts.noOfNextBytesValid()==false);

  bts << "test";
  ASSERT(bts.noOfNextBytesValid()==false);
  ASSERT_THROW(bts >> "fest", Bytestream::Badmatch);
  ASSERT(bts.pos()==pos_before);

  ASSERT_THROW(bts/3 >> "fest", logic_error);
  ASSERT_THROW(bts.getBytestream(17), logic_error);
}

TEST(constructors)
{
  Bytestream bts(0, 15);
  ASSERT(bts.size()==15);
  ASSERT(bts.pos()==0);
  bts >> (uint8_t)0 >> (uint16_t)0 >> (uint32_t)0 >> (uint64_t)0;
  ASSERT(bts.atEnd());

  const char* cs = "someString";
  Bytestream bts2(cs , 10);
  ASSERT(bts2.size()==10);
  ASSERT(bts2.pos()==0);
  bts2 >> cs;
  ASSERT(bts2.atEnd());

  Bytestream bts3 = Bytestream(7);
  ASSERT(bts3.size()==7);
  ASSERT(bts3.pos()==0);
}

TEST(floats)
{
  Bytestream bts;
  bts << (float64_t)0.0F << (float64_t)1 << (float64_t)1.1 << (float64_t)12.7
      << 0.01171875 << 3.14159F;
  bts << 0x4037000000000000;
  bts.putF64((float64_t)666.777);
  bts >> (float64_t)0.0F >> (float64_t)1  >> (float64_t)(1.1) >> (float64_t)12.7
      >> (3.0/256) >> (float32_t)3.14159;
  bts >> (float64_t)23.0 >> 666.777;
}

TEST(endianness)
{
  Bytestream bts;
  ASSERT(bts.getEndianness()==Bytestream::BigEndian);
  bts << (uint64_t)0x0102030405060708 << (uint32_t)0x11121314
      << (uint16_t)0x2122 << (uint8_t)0x31 << "test";
  bts.setEndianness(Bytestream::LittleEndian);
  bts >> (uint64_t)0x0807060504030201 >> (uint32_t)0x14131211
      >> (uint16_t)0x2221 >> (uint8_t)0x31 >> "test";

  uint32_t sample = 0x11223344;
  Bytestream bts2(&sample, sizeof(uint32_t), Bytestream::LittleEndian);
  bts2 << sample;
  bts2 >> (uint32_t)0x11223344 >> (uint32_t)0x11223344;
  bts2.setPos(0);
  bts2.setEndianness(Bytestream::BigEndian);
  bts2 >> (uint32_t)0x44332211 >> (uint32_t)0x44332211;

  std::string text = "test";
  Bytestream bts3(text.c_str(), text.length(), Bytestream::LittleEndian);
  bts3.setEndianness(Bytestream::BigEndian);
  ASSERT(bts3.nextString(text)==true);
}

TEST(mixed_eandian)
{
  /* This is a Shapefile header https://en.wikipedia.org/wiki/Shapefile
   * They are pretty silly in that they use mixed endianness,
   * and thus a perfect test for Bytestream.
   */
  uint8_t shp[] = {0x00, 0x00, 0x27, 0x0a, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x01, 0x69, 0xa6, 0xe8, 0x03, 0x00, 0x00,
                   0x03, 0x00, 0x00, 0x00, 0xf2, 0xd2, 0x4d, 0xe2,
                   0x9d, 0x6d, 0x1c, 0x41, 0xdb, 0xf9, 0x7e, 0x9a,
                   0x6e, 0x65, 0x58, 0x41, 0x00, 0x00, 0x00, 0x00,
                   0xb0, 0x41, 0x23, 0x41, 0x5a, 0x64, 0x3b, 0xa7,
                   0x12, 0xf5, 0x58, 0x41, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00};

  Bytestream bts(shp, 100);

  bts >> (int32_t)0x0000270a;
  bts >> (int32_t)0 >> (int32_t)0 >> (int32_t)0 >> (int32_t)0 >> (int32_t)0;

  const int32_t length = 92582;
  const int32_t version = 1000;
  const int32_t shape_type = 3;

  bts >> length;

  bts.setEndianness(Bytestream::LittleEndian);
  bts >> version >> shape_type;

  bts >> (float64_t)465767.471 >> (float64_t)6395322.414
      >> (float64_t)631000.000 >> (float64_t)6542410.613;
  bts >> (float64_t)0.0 >> (float64_t)0.0 >> (float64_t)0.0 >> (float64_t)0.0;
}

#include "TestCodable.h"

TEST(codable)
{
  TestCodable cod;
  ASSERT(cod.a == 0);
  ASSERT(cod.b == 0);
  ASSERT(cod.c == 7);
  ASSERT(cod.d == 0);
  ASSERT(cod.e == 0);
  ASSERT(cod.f == 0);
  ASSERT(cod.g == 8);
  ASSERT(cod.h == 0);
  ASSERT(cod.s == "");
  ASSERT(cod.f1 == 0.0);
  ASSERT(cod.f2 == 0.0);
  ASSERT(cod.s2 == "");

  Bytestream bts;
  bts << (uint8_t)1 << (uint16_t)2 << (uint32_t)3 << (uint64_t)4
      << (int8_t)-1 << (int16_t)-2 << (int32_t)-3 << (int64_t)-4
      << "someString" << (float32_t)1.1 << (float64_t)-2.2
         // Null padding so encoded comparison works
      << std::string(6, 0) << "short" << std::string(5, 0) << "match"
      << (uint8_t)1 << (uint16_t)2 << (uint32_t)3 << (uint64_t)4;

  cod.decodeFrom(bts);

  ASSERT(bts.remaining() == 0);

  ASSERT(cod.encode() == bts);

  TestCodable cod2;
  cod2.a = 1;
  cod2.b = 2;
  cod2.c = 3;
  cod2.d = 4;
  cod2.e = -1;
  cod2.f = -2;
  cod2.g = -3;
  cod2.h = -4;
  cod2.s = "someString";
  cod2.f1 = 1.1;
  cod2.f2 = -2.2;
  cod2.s2 = "short";

  // Sad, but this is the only reliable value
  ASSERT(static_cast<uint8_t>(cod2.myEnum8) == 0);
  ASSERT(static_cast<uint16_t>(cod2.myEnum16) == 0);
  ASSERT(static_cast<uint32_t>(cod2.myEnum32) == 0);
  ASSERT(static_cast<uint64_t>(cod2.myEnum64) == 0);

  cod2.myEnum8 = TestCodable::Value8_1;
  cod2.myEnum16 = TestCodable::Value16_2;
  cod2.myEnum32 = TestCodable::Value32_3;
  cod2.myEnum64 = TestCodable::Value64_4;

  ASSERT(cod2.encode() == bts);

  bts.setPos(0);
  TestCodable cod3(bts);
  ASSERT(cod3.encode() == bts);

  ASSERT(cod.encodedSize() == bts.size());
  ASSERT(cod.encodedSize() == 15+15+10+4+8+6+5+5+5+15);

  Codable* cod_p = &cod3;
  ASSERT(cod_p->encode() == bts);

  ASSERT(TestCodable::myEnum8ToString(TestCodable::Value8_1) == "Value8_1");
  ASSERT(TestCodable::myEnum8ToString(TestCodable::Value8_2) == "Value8_2");
  ASSERT(TestCodable::myEnum8ToString(TestCodable::Value8_3) == "Value8_3");
  ASSERT(TestCodable::myEnum8ToString(TestCodable::Value8_4) == "Value8_4");

  ASSERT(TestCodable::myEnum16ToString(TestCodable::Value16_1) == "Value16_1");
  ASSERT(TestCodable::myEnum16ToString(TestCodable::Value16_2) == "Value16_2");
  ASSERT(TestCodable::myEnum16ToString(TestCodable::Value16_3) == "Value16_3");
  ASSERT(TestCodable::myEnum16ToString(TestCodable::Value16_4) == "Value16_4");

  ASSERT(TestCodable::myEnum32ToString(TestCodable::Value32_1) == "Value32_1");
  ASSERT(TestCodable::myEnum32ToString(TestCodable::Value32_2) == "Value32_2");
  ASSERT(TestCodable::myEnum32ToString(TestCodable::Value32_3) == "Value32_3");
  ASSERT(TestCodable::myEnum32ToString(TestCodable::Value32_4) == "Value32_4");

  ASSERT(TestCodable::myEnum64ToString(TestCodable::Value64_1) == "Value64_1");
  ASSERT(TestCodable::myEnum64ToString(TestCodable::Value64_2) == "Value64_2");
  ASSERT(TestCodable::myEnum64ToString(TestCodable::Value64_3) == "Value64_3");
  ASSERT(TestCodable::myEnum64ToString(TestCodable::Value64_4) == "Value64_4");

  std::string descr = cod.describe();

  ASSERT(descr.find("Value8_1") != std::string::npos);
  ASSERT(descr.find("Value16_2") != std::string::npos);
  ASSERT(descr.find("Value32_3") != std::string::npos);
  ASSERT(descr.find("Value64_4") != std::string::npos);

}

TEST(initializer_list)
{
  Bytestream bts({(uint8_t)1, (uint16_t)2, (uint32_t)3, (uint64_t)4,
                  (int8_t)-1, (int16_t)-2, (int32_t)-3, (int64_t)-4,
                  (float32_t)1.1, (float64_t)2.2,
                  std::string("someString")});

  bts >> (uint8_t)1 >> (uint16_t)2 >> (uint32_t)3 >> (uint64_t)4;
  bts >> (int8_t)-1 >> (int16_t)-2 >> (int32_t)-3 >> (int64_t)-4;
  bts >> (float32_t)1.1 >> (float64_t)2.2;
  bts >> "someString";

  Bytestream bts2 = {(uint8_t)1, (uint16_t)2, (uint32_t)3, (uint64_t)4,
                     (int8_t)-1, (int16_t)-2, (int32_t)-3, (int64_t)-4,
                     (float32_t)1.1, (float64_t)2.2,
                     std::string("someString")};
  ASSERT(bts2==bts);

  Bytestream bts_le({(uint8_t)1, (uint16_t)2, (uint32_t)3, (uint64_t)4,
                     (int8_t)-1, (int16_t)-2, (int32_t)-3, (int64_t)-4,
                     (float32_t)1.1, (float64_t)2.2,
                     std::string("someString")}, Bytestream::LittleEndian);

  ASSERT(bts_le.getEndianness() == Bytestream::LittleEndian);

  Bytestream ManuallyFlipped = {(uint8_t)1, (uint16_t)0x0200,
                                (uint32_t)0x03000000,
                                (uint64_t)0x0400000000000000};

  ASSERT(bts_le >>= ManuallyFlipped);

  bts_le.setPos(0);

  bts_le >> (uint8_t)1 >> (uint16_t)2 >> (uint32_t)3 >> (uint64_t)4;
  bts_le >> (int8_t)-1 >> (int16_t)-2 >> (int32_t)-3 >> (int64_t)-4;
  bts_le >> (float32_t)1.1 >> (float64_t)2.2;
  bts_le >> "someString";

  Bytestream bts_le2;
  bts_le2.setEndianness(Bytestream::LittleEndian);
  bts_le2 << (uint8_t)1 << (uint16_t)2 << (uint32_t)3 << (uint64_t)4
          << (int8_t)-1 << (int16_t)-2 << (int32_t)-3 << (int64_t)-4
          << (float32_t)1.1 << (float64_t)2.2
          << "someString";

  ASSERT(bts_le == bts_le2);
}

TEST(bytes)
{
  Bytes b1((uint8_t)1);
  Bytes b2(b1);
  Bytes b3 = b2;

  Bytes bs1("someString");
  Bytes bs2(bs1);
  Bytes bs3 = bs2;

  Bytestream bts({b1, b2, b3, bs1, bs2, bs3});
  bts >> (uint8_t)1 >> (uint8_t)1 >> (uint8_t)1
      >> "someString" >> "someString" >> "someString";
}

TEST(reset)
{
  Bytestream bts;
  ASSERT(bts.size() == 0);
  ASSERT(bts.atEnd());
  bts << (uint64_t)42;
  ASSERT(bts.size() == 8);
  bts.reset();
  ASSERT(bts.size() == 0);
  ASSERT(bts.atEnd());
  ASSERT_THROW(bts += 8, logic_error);
}

TEST(iostream)
{
  // Bytestream bts;
  stringstream ss;
  ss << "StringStream";

  Bytestream bts(ss);
  ASSERT(bts>>="StringStream");

  // Re-init stringstream
  ss = stringstream();
  ss << "StringStream";

  Bytestream bts2(ss, 12);
  ASSERT(bts2==bts);

  stringstream out;
  out << bts;
  ASSERT(out.str()==ss.str());

  string troll = "trololololol";
  bts.initFrom(troll.c_str(), 12);

  Bytestream bts3;
  bts3.initFrom(troll.c_str(), 12);
  ASSERT(bts3==bts);

  bts.setEndianness(Bytestream::LittleEndian);

  stringstream sst;
  sst << "lolbolltroll";
  bts.initFrom(sst, 12);
  ASSERT(bts>>="lolbolltroll");

  // Endianness survives copy re-init
  ASSERT(bts.getEndianness() == Bytestream::LittleEndian);

  stringstream sst2;
  sst2 << "lolbolltroll" << "!!";
  bts.initFrom(sst2, 14);
  ASSERT(bts.size() == 14);
  ASSERT(bts>>="lolbolltroll!!");

  // Endianness survives full re-init
  ASSERT(bts.getEndianness() == Bytestream::LittleEndian);
}

TEST(large_iostream)
{
  Bytestream large('A', BS_REASONABLE_FILE_SIZE*1.5);
  stringstream ss;
  ss << large;

  Bytestream bts(ss);
  ASSERT(bts == large);
}

TEST(hexdump)
{
  Bytestream bts;
  bts << (uint8_t)0x00 << (uint8_t)0x01 << (uint8_t)0x02 << (uint8_t)0x03
      << (uint8_t)0x04 << (uint8_t)0x05 << (uint8_t)0x06 << (uint8_t)0x07
      << (uint8_t)0x08 << (uint8_t)0x09 << (uint8_t)0x0a << (uint8_t)0x0b
      << (uint8_t)0x0c << (uint8_t)0x0d << (uint8_t)0x0e << (uint8_t)0x0f
      << (uint8_t)0x00 << (uint8_t)0x11 << (uint8_t)0x22 << (uint8_t)0x33
      << (uint8_t)0x44 << (uint8_t)0x55 << (uint8_t)0x66 << (uint8_t)0x77
      << (uint8_t)0x88 << (uint8_t)0x99 << (uint8_t)0xaa << (uint8_t)0xbb
      << (uint8_t)0xcc << (uint8_t)0xdd << (uint8_t)0xee << (uint8_t)0xff;

  ASSERT("00000000: 0001 0203 0405 0607  0809 0a0b 0c0d 0e0f  ........ ........\n"
         == bts.hexdump(16));

  ASSERT("00000000: 0001 0203 0405 0607  0809 0a0b 0c0d 0e0f  ........ ........\n"
         "00000010: 0011 2233 4455 6677  8899 aabb ccdd eeff  ..\"3DUfw ........\n"
         == bts.hexdump(32));
  // Asking for more than all gives all there is
  ASSERT("00000000: 0001 0203 0405 0607  0809 0a0b 0c0d 0e0f  ........ ........\n"
         "00000010: 0011 2233 4455 6677  8899 aabb ccdd eeff  ..\"3DUfw ........\n"
         == bts.hexdump(666));

  ASSERT("00000000: 00                                        .\n"
         == bts.hexdump(1));
  ASSERT("00000000: 0001                                      ..\n"
         == bts.hexdump(2));
  ASSERT("00000000: 0001 02                                   ...\n"
         == bts.hexdump(3));
  ASSERT("00000000: 0001 0203                                 ....\n"
         == bts.hexdump(4));
  ASSERT("00000000: 0001 0203 04                              .....\n"
         == bts.hexdump(5));
  ASSERT("00000000: 0001 0203 0405                            ......\n"
         == bts.hexdump(6));
  ASSERT("00000000: 0001 0203 0405 06                         .......\n"
         == bts.hexdump(7));
  ASSERT("00000000: 0001 0203 0405 0607                       ........\n"
         == bts.hexdump(8));
  ASSERT("00000000: 0001 0203 0405 0607  08                   ........ .\n"
         == bts.hexdump(9));
  ASSERT("00000000: 0001 0203 0405 0607  0809                 ........ ..\n"
         == bts.hexdump(10));
  ASSERT("00000000: 0001 0203 0405 0607  0809 0a              ........ ...\n"
         == bts.hexdump(11));
  ASSERT("00000000: 0001 0203 0405 0607  0809 0a0b            ........ ....\n"
         == bts.hexdump(12));
  ASSERT("00000000: 0001 0203 0405 0607  0809 0a0b 0c         ........ .....\n"
         == bts.hexdump(13));
  ASSERT("00000000: 0001 0203 0405 0607  0809 0a0b 0c0d       ........ ......\n"
         == bts.hexdump(14));
  ASSERT("00000000: 0001 0203 0405 0607  0809 0a0b 0c0d 0e    ........ .......\n"
         == bts.hexdump(15));

  // Hexdump starts from current read position
  bts+=16;
  ASSERT("00000000: 0011 2233 4455 6677  8899 aabb ccdd eeff  ..\"3DUfw ........\n"
         == bts.hexdump(16));
  // ... and with asking for too much and not giving a whole line.
  bts+=8;
  ASSERT("00000000: 8899 aabb ccdd eeff                       ........\n"
         == bts.hexdump(16));

}

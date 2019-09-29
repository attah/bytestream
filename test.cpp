#include "bytestream.h"
#include "test.h"
using namespace std;

TEST(get_operator)
{
  bytestream bts;
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
  bytestream bts2;
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
  bytestream bts;
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
  bytestream bts2a = (bts/10).peekBytestream();
  bytestream bts2b = (bts/10).getBytestream();
  ASSERT(bts2a==bts2b);
  ASSERT(bts2b>>="someString");
  bts-=10;
  ASSERT(bts.peekString(10)=="someString");
  ASSERT(bts.getString(10)=="someString");
  bts-=10;
  bytestream bts3a = bts.peekBytestream(10);
  bytestream bts3b = bts.getBytestream(10);
  ASSERT(bts3a==bts3b);
  ASSERT(bts3b>>="someString");
}

TEST(const_types)
{
  bytestream bts;
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
  bytestream bts;
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
  bytestream bts2;
  bts2 << "someString";
  ASSERT(bts >>=bts2);
  ASSERT_FALSE(bts >>= bts2); // already consumed
}

TEST(test_method)
{
  bytestream bts;
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
  bytestream bts2;
  bts2 << "someString";
  ASSERT(bts.nextBytestream(bts2));
  ASSERT_FALSE(bts.nextBytestream(bts2)); // already consumed
}

#define HIBIT(sign, suffix) \
  (sign##suffix)((uint##suffix)1<<(sizeof(sign##suffix)*8-1))

TEST(limits)
{
  bytestream bts;
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
  bytestream bts;
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

  ASSERT_THROW((bts+=1), out_of_range);
}

TEST(copying)
{
  bytestream bts;
  bts << (uint8_t)1 << (uint16_t)2 << (uint32_t)3 << (uint64_t)4
      << (int8_t)-1 << (int16_t)-2 << (int32_t)-3 << (int64_t)-4
      << "someString";
  bytestream bts2(bts);
  ASSERT(bts2==bts);
  bytestream bts3 = bts;
  ASSERT(bts3==bts);
  bytestream bts4;
  bts4 = bts;
  ASSERT(bts4==bts);

  bts += 30;
  bytestream bts5 = bts;
  ASSERT(bts5==bts);
  ASSERT(bts.pos()==30);
  ASSERT(bts5.pos()==30);
  bts5 = bts5[0];
  ASSERT(bts5.pos()==0);
  ASSERT(bts5.size()==bts.size());
}

TEST(partials)
{
  bytestream bts;
  bts << (uint8_t)1 << (uint16_t)2 << (uint32_t)3 << (uint64_t)4
      << (int8_t)-1 << (int16_t)-2 << (int32_t)-3 << (int64_t)-4
      << "someString";

  bytestream bts2;
  bts/bts.size() >> bts2;
  ASSERT(bts2==bts);

  bts-=10;
  ASSERT(bts>>="someString");
  bts-=10;
  bytestream bts3;
  bts/10 >> bts3;
  ASSERT(bts3>>="someString");
}

TEST(exceptions)
{
  bytestream bts;
  bts << (uint8_t)1;
  size_t pos_before = bts.pos();
  ASSERT_THROW(bts >> (uint16_t)1, out_of_range);
  ASSERT_THROW(bts >> (uint8_t)2, bytestream::badmatch);
  ASSERT(bts.pos()==pos_before);
  ASSERT(bts.noOfNextBytesValid()==false);
  ASSERT_THROW(bts.getBytestream(17), logic_error);
  ASSERT(bts.noOfNextBytesValid()==false);
  ASSERT_THROW(bts.getString(2), out_of_range);
  ASSERT(bts.noOfNextBytesValid()==false);

  bts << "test";
  ASSERT(bts.noOfNextBytesValid()==false);
  ASSERT_THROW(bts >> "fest", bytestream::badmatch);
  ASSERT(bts.pos()==pos_before);

  ASSERT_THROW(bts/3 >> "fest", logic_error);
  ASSERT_THROW(bts.getBytestream(17), logic_error);
}

TEST(constructors)
{
  bytestream bts(15);
  ASSERT(bts.size()==15);
  ASSERT(bts.pos()==0);
  bts >> (uint8_t)0 >> (uint16_t)0 >> (uint32_t)0 >> (uint64_t)0;
  ASSERT(bts.atEnd());

  const char* cs = "someString";
  bytestream bts2(cs , 10);
  ASSERT(bts2.size()==10);
  ASSERT(bts2.pos()==0);
  bts2 >> cs;
  ASSERT(bts2.atEnd());
}

TEST(legacy)
{
  bytestream bts;

  uint8_t a0 = 1;
  uint16_t b0 = 2;
  uint32_t c0 = 3;
  uint64_t d0 = 4;
  int8_t e0 = -1;
  int16_t f0 = -2;
  int32_t g0 = -3;
  int64_t h0 = -4;

  bts << a0 << b0 << c0 << d0 << e0 << f0 << g0 << h0 << "troll";

  uint8_t a = 0;
  uint16_t b = 0;
  uint32_t c = 0;
  uint64_t d = 0;
  int8_t e = 0;
  int16_t f = 0;
  int32_t g = 0;
  int64_t h = 0;
  string str;

  bts >> a >> b >> c >> d >> e >> f >> g >> h;
  bts/5 >> str;
  bytestream bts2 = bts[0];

  string str2;
  bts2+=30;
  bts2/5 >> str2;

  bytestream bts3 = bts[0];

  bts3 >> (uint8_t)1 >> (uint16_t)2 >> (uint32_t)3 >> (uint64_t)4;
  bts3 >> (int8_t)-1 >> (int16_t)-2 >> (int32_t)-3 >> (int64_t)-4;
  bts3 >> "troll";

  bytestream bts4 = bts3[0];

  ASSERT(bts4 >>= (uint8_t)1);
  ASSERT(bts4 >>= (uint16_t)2);
  ASSERT(bts4 >>= (uint32_t)3);
  ASSERT(bts4 >>= (uint64_t)4);
  ASSERT(bts4 >>= (int8_t)-1);
  ASSERT(bts4 >>= (int16_t)-2);
  ASSERT(bts4 >>= (int32_t)-3);
  ASSERT(bts4 >>= (int64_t)-4);
  ASSERT(bts4/5 >>= "troll");

  ASSERT(bts4==bts);
  bytestream bts0;
  ASSERT((bts4==bts0) != true);

  bytestream bts5 = bts[30];
  ASSERT(bts5.size() < bts.size());
  bytestream tmp;
  bytestream troll;
  troll << "troll";
  bts5/5 >> tmp;
  ASSERT(tmp==troll);
}

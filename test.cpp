#include "bytestream.h"
#include "test.h"
using namespace std;

TEST(get_operators)
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

  ASSERT(bts.getU8()==1);
  ASSERT(bts.getU16()==2);
  ASSERT(bts.getU32()==3);
  ASSERT(bts.getU64()==4);
  ASSERT(bts.getS8()==-1);
  ASSERT(bts.getS16()==-2);
  ASSERT(bts.getS32()==-3);
  ASSERT(bts.getS64()==-4);
  ASSERT((bts/10).getString()=="someString");
  bts-=10;
  bytestream bts2 = (bts/10).getBytestream();
  ASSERT(bts2>>="someString");
  bts-=10;
  ASSERT(bts.getString(10)=="someString");
  bts-=10;
  bytestream bts3 = bts.getBytestream(10);
  ASSERT(bts3>>="someString");
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
  ASSERT(bts4==bts0 != true);

  bytestream bts5 = bts[30];
  ASSERT(bts5.size() < bts.size());
  bytestream tmp;
  bytestream troll;
  troll << "troll";
  bts5/5 >> tmp;
  ASSERT(tmp==troll);
}

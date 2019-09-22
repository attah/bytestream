#include "bytestream.h"
#include "test.h"

TEST(basic_types)
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

  ASSERT(a==1);
  ASSERT(b==2);
  ASSERT(c==3);
  ASSERT(d==4);
  ASSERT(e==-1);
  ASSERT(f==-2);
  ASSERT(g==-3);
  ASSERT(h==-4);
  ASSERT(s=="someString");
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

TEST(bool_typechecks)
{
  bytestream bts;
  bts << (uint8_t)1 << (uint16_t)2 << (uint32_t)3 << (uint64_t)4
      << (int8_t)-1 << (int16_t)-2 << (int32_t)-3 << (int64_t)-4
      << "someString";

  ASSERT(bts >>= (uint8_t)1);
  ASSERT(bts >>= (uint16_t)2);
  ASSERT(bts >>= (uint32_t)3);
  ASSERT(bts >>= (uint64_t)4);
  ASSERT(bts >>= (int8_t)-1);
  ASSERT(bts >>= (int16_t)-2);
  ASSERT(bts >>= (int32_t)-3);
  ASSERT(bts >>= (int64_t)-4);
  ASSERT(bts >>= "someString");
  bts -= 10;
  ASSERT(bts/10 >>= "someString");
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

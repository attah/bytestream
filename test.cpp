#include "bytestream.h"
#include <iostream>
#include <assert.h>

int main(int argc, char** argv)
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
  std::cout << (int)a << " " << b << " " << c << " " << d << " "
            << (int)e << " " << f << " " << g << " " << h << '\n';
  std::cout << str << '\n';
  bytestream bts2 = bts[0];

  string str2;
  bts2+=30;
  bts2/5 >> str2;
  std::cout << "pos " << bts.pos() << " " << bts[0].pos() << "  " << str2 << '\n';

  bytestream bts3 = bts[0];

  bts3 >> (uint8_t)1 >> (uint16_t)2 >> (uint32_t)3 >> (uint64_t)4;
  bts3 >> (int8_t)-1 >> (int16_t)-2 >> (int32_t)-3 >> (int64_t)-4;

  bytestream bts4 = bts3[0];

  assert(bts4 >>= (uint8_t)1);
  assert(bts4 >>= (uint16_t)2);
  assert(bts4 >>= (uint32_t)3);
  assert(bts4 >>= (uint64_t)4);
  assert(bts4 >>= (int8_t)-1);
  assert(bts4 >>= (int16_t)-2);
  assert(bts4 >>= (int32_t)-3);
  assert(bts4 >>= (int64_t)-4);

}

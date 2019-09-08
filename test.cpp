#include "bytestream.h"
#include <iostream>
#include <assert.h>

int main(int argc, char** argv)
{
  bytestream bts;

  uint8_t a0 = 1;
  uint16_t b0 = 2;
  uint32_t c0 = 3;
  int8_t d0 = -1;
  int16_t e0 = -2;
  int32_t f0 = -3;

  bts << a0 << b0 << c0 << d0 << e0 << f0 << "troll";

  uint8_t a = 0;
  uint16_t b = 0;
  uint32_t c = 0;
  int8_t d = 0;
  int16_t e = 0;
  int32_t f = 0;
  string str;

  bts >> a >> b >> c >> d >> e >> f;
  bts/5 >> str;
  std::cout << (int)a << " " << b << " " << c << " "
            << (int)d << " " << e << " " << f << '\n';
  std::cout << str << '\n';
  bytestream bts2 = bts[0];

  string str2;
  bts2+=14;
  bts2/5 >> str2;
  std::cout << "pos " << bts.pos() << " " << bts[0].pos() << "  " << str2 << '\n';
}

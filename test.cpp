#include "bytestream.h"
#include <iostream>
#include <assert.h>

int main(int argc, char** argv)
{
  bytestream bts;

  uint8_t a0 = 1;
  uint16_t b0 = 2;
  uint32_t c0 = 3;

  bts << a0 << b0 << c0 << "troll";

  uint8_t a = 0;
  uint16_t b = 0;
  uint32_t c = 0;
  string str;

  bts >> a >> b >> c;
  bts/5 >> str;
  std::cout << (int)a << " " << b << " " << c << '\n';
  std::cout << str << '\n';
  bytestream bts2 = bts[0];

  string str2;
  bts2+=7;
  bts2/5 >> str2;
  std::cout << "pos " << bts.pos() << " " << bts[0].pos() << "  " << str2 << '\n';
}

#include "bytestream.h"
#include <iostream>
#include <assert.h>
#include <dlfcn.h>

#define TEST(n) TEST_(n, __COUNTER__)
#define TEST_(n, counter) TEST__(n, counter)
#define TEST__(n, counter) \
  void _test_##n();\
  extern "C" void __test_##counter()\
    {_test_##n();}\
  void _test_##n()

typedef void (*void_f)(void);

int main(int argc, char** argv)
{
  void* handle = dlopen(NULL, RTLD_LAZY);

  int i = 0;
  while(true)
  {
    std::string test = "__test_"+std::to_string(i++);
    void* fptr = dlsym(handle, test.c_str());
    if(!fptr)
      break;
    ((void_f)fptr)();
  }
  dlclose(handle);
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
  bts3 >> "troll";

  bytestream bts4 = bts3[0];

  assert(bts4 >>= (uint8_t)1);
  assert(bts4 >>= (uint16_t)2);
  assert(bts4 >>= (uint32_t)3);
  assert(bts4 >>= (uint64_t)4);
  assert(bts4 >>= (int8_t)-1);
  assert(bts4 >>= (int16_t)-2);
  assert(bts4 >>= (int32_t)-3);
  assert(bts4 >>= (int64_t)-4);
  assert(bts4/5 >>= "troll");

  assert(bts4==bts);
  bytestream bts0;
  assert(bts4==bts0 != true);

  bytestream bts5 = bts[30];
  assert(bts5.size() < bts.size());
  bytestream tmp;
  bytestream troll;
  troll << "troll";
  bts5/5 >> tmp;
  std::cout << "aaaa" << troll.size() << "..." << troll.size() << "\n";
  assert(tmp==troll);
}

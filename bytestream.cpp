#include "bytestream.h"
#include <iostream>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <byteswap.h>
#ifndef __BYTE_ORDER
#include <endian.h>
#endif
using namespace std;

bytestream::bytestream()
{
  _size = 0;
  _pos = 0;
  _noOfNextBytes = 0;
  _noOfNextBytesValid = false;
  endianness = big;
}

bytestream::bytestream(const void* data, size_t len)
{
  _size = len;
  _data = new uint8_t[_size];
  memcpy(_data, data, _size);
  _pos = 0;
  _noOfNextBytes = 0;
  _noOfNextBytesValid = false;
  endianness = big;
}

bytestream::bytestream(const bytestream& rhs)
{
  _size = rhs._size;
  _data = new uint8_t[_size];
  memcpy(_data, rhs._data, _size);
  _pos = rhs._pos;
  _noOfNextBytes = 0;
  _noOfNextBytesValid = false;
  endianness = rhs.endianness;
}

bytestream::~bytestream()
{
  if(_size != 0)
  {
    delete _data;
  }
}

uint8_t bytestream::getU8()
{
  uint8_t tmp;
  getBytes(&tmp, 1);
  return tmp;
}
uint16_t bytestream::getU16()
{
  uint16_t tmp;
  getBytes(&tmp, 2);
  if(needsSwap())
    tmp = bswap_16(tmp);
  return tmp;
}
uint32_t bytestream::getU32()
{
  uint32_t tmp;
  getBytes(&tmp, 4);
  if(needsSwap())
    tmp = bswap_32(tmp);
  return tmp;
}
uint64_t bytestream::getU64()
{
  uint64_t tmp;
  getBytes(&tmp, 8);
  if(needsSwap())
    tmp = bswap_64(tmp);
  return tmp;
}
int8_t bytestream::getS8()
{
  uint8_t tmp;
  getBytes(&tmp, 1);
  return tmp;
}
int16_t bytestream::getS16()
{
  int16_t tmp;
  getBytes(&tmp, 2);
  if(needsSwap())
    tmp = bswap_16(tmp);
  return tmp;
}
int32_t bytestream::getS32()
{
  int32_t tmp;
  getBytes(&tmp, 4);
  if(needsSwap())
    tmp = bswap_32(tmp);
  return tmp;
}
int64_t bytestream::getS64()
{
  int64_t tmp;
  getBytes(&tmp, 8);
  if(needsSwap())
    tmp = bswap_64(tmp);
  return tmp;
}

void bytestream::getBytes(void* cs,  size_t len)
{
  _before(len);
  memcpy(cs, &(_data[_pos]), len);
  _after(len);
}

void bytestream::putU8(uint8_t u)
{
  putBytes(&u, 1);
}
void bytestream::putU16(uint16_t u)
{
  if(needsSwap())
    u = bswap_16(u);
  putBytes(&u, 2);
}
void bytestream::putU32(uint32_t u)
{
  if(needsSwap())
    u = bswap_32(u);
  putBytes(&u, 4);
}
void bytestream::putU64(uint64_t u)
{
  if(needsSwap())
    u = bswap_64(u);
  putBytes(&u, 8);
}
void bytestream::putS8(int8_t u)
{
  putBytes(&u, 1);
}
void bytestream::putS16(int16_t u)
{
  if(needsSwap())
    u = bswap_16(u);
  putBytes(&u, 2);
}
void bytestream::putS32(int32_t u)
{
  if(needsSwap())
    u = bswap_32(u);
  putBytes(&u, 4);
}
void bytestream::putS64(int64_t u)
{
  if(needsSwap())
    u = bswap_64(u);
  putBytes(&u, 8);
}
void bytestream::putBytes(const void* c, size_t len)
{

  uint8_t* old = _data;
  _data = new uint8_t[_size+len];

  if (_size != 0)
  {
    memcpy(_data, old, _size);
    delete old;
  }
  memcpy((_data+_size), c, len);
  _size += len;
}

void bytestream::setNoOfNextBytes(int n)
{
  _noOfNextBytes = n;
  _noOfNextBytesValid = true;
}

void bytestream::_before(size_t bytesToRead)
{
  if(bytesToRead > remaining())
  {
    throw out_of_range("Tried to read past end");
  }
}

void bytestream::_after(size_t bytesRead)
{
  _pos += bytesRead;
  _noOfNextBytesValid = false;
}

bytestream& operator>>(bytestream& b, uint8_t& u)
{
  u = b.getU8();
  return b;
}
bytestream& operator>>(bytestream& b, uint16_t& u)
{
  u = b.getU16();
  return b;
}
bytestream& operator>>(bytestream& b, uint32_t& u)
{
  u = b.getU32();
  return b;
}
bytestream& operator>>(bytestream& b, uint64_t& u)
{
  u = b.getU64();
  return b;
}
bytestream& operator>>(bytestream& b, int8_t& u)
{
  u = b.getS8();
  return b;
}
bytestream& operator>>(bytestream& b, int16_t& u)
{
  u = b.getS16();
  return b;
}
bytestream& operator>>(bytestream& b, int32_t& u)
{
  u = b.getS32();
  return b;
}
bytestream& operator>>(bytestream& b, int64_t& u)
{
  u = b.getS64();
  return b;
}
bytestream& operator>>(bytestream& b, std::string& s)
{
  if(!b.noOfNextBytesValid())
  {
    throw invalid_argument("No length given");
  }
  size_t noOfNextBytes = b.getNoOfNextBytes();
  char* cs = new char[noOfNextBytes+1];
  cs[noOfNextBytes] = 0;
  b.getBytes(cs, noOfNextBytes);
  s = string(cs, noOfNextBytes);
  delete cs;
  return b;
}

bytestream& operator/(bytestream& b, int i)
{
  b.setNoOfNextBytes(i);
  return b;
}
bytestream& operator<<(bytestream& b, const uint8_t& u)
{
  b.putU8(u);
  return b;
}
bytestream& operator<<(bytestream& b, const uint16_t& u)
{
  b.putU16(u);
  return b;
}
bytestream& operator<<(bytestream& b, const uint32_t& u)
{
  b.putU32(u);
  return b;
}
bytestream& operator<<(bytestream& b, const uint64_t& u)
{
  b.putU64(u);
  return b;
}

bytestream& operator<<(bytestream& b, const int8_t& u)
{
  b.putS8(u);
  return b;
}
bytestream& operator<<(bytestream& b, const int16_t& u)
{
  b.putS16(u);
  return b;
}
bytestream& operator<<(bytestream& b, const int32_t& u)
{
  b.putS32(u);
  return b;
}
bytestream& operator<<(bytestream& b, const int64_t& u)
{
  b.putS64(u);
  return b;
}
bytestream& operator<<(bytestream& b, const std::string& s)
{
  b.putBytes(s.c_str(), s.length());
  return b;
}
bytestream& operator<<(bytestream& b, const bytestream& other)
{
  b.putBytes(other.raw(), other.size());
  return b;
}

#if __BYTE_ORDER == __LITTLE_ENDIAN
bool bytestream::needsSwap()
{
  return endianness != Endianness::native && endianness != Endianness::little;
}
#elif __BYTE_ORDER == __BIG_ENDIAN
bool bytestream::needsSwap()
{
  return endianness != Endianness::native && endianness != Endianness::big;
}
#else
#error
#endif

bytestream bytestream::operator[](size_t i)
{
  bytestream tmp(*this);
  tmp.setPos(i);
  return tmp;
}

bytestream& bytestream::operator+=(size_t i)
{
  _pos += i;
  return *this;
}

bytestream& bytestream::operator-=(size_t i)
{
  _pos -= i;
  return *this;
}

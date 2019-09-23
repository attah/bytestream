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

bool bytestream::operator==(const bytestream& other) const
{
  if(_size != other.size())
  {
    return false;
  }
  return memcmp(_data, other.raw(), _size) == 0;
}
bool bytestream::operator!=(const bytestream& other) const
{
  if(_size != other.size())
  {
    return true;
  }
  return memcmp(_data, other.raw(), _size) != 0;
}

bytestream& bytestream::operator=(const bytestream& other)
{
  if(_size != 0)
  {
    delete _data;
  }
  _pos = other.pos();
  _size = other.size();
  _data = new uint8_t[_size];
  memcpy(_data, other.raw(), _size);
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

#define NEXT(type, shorthand, len) NEXT_(type##len##_t, shorthand##len)
#define NEXT_(type, shortType) \
  bool bytestream::next##shortType(type u) \
  {if(u == get##shortType())\
     {return true;} \
   else\
  {(*this) -= sizeof(type);\
   return false;}}

NEXT(uint, U, 8);
NEXT(uint, U, 16);
NEXT(uint, U, 32);
NEXT(uint, U, 64);
NEXT(int, S, 8);
NEXT(int, S, 16);
NEXT(int, S, 32);
NEXT(int, S, 64);

bool bytestream::nextString(const std::string& s)
{
  if(!noOfNextBytesValid())
  {
    setNoOfNextBytes(s.length());
  }
  else if(getNoOfNextBytes() != s.length())
  {
    throw invalid_argument("Desired length does not match const length");
  }

  size_t noOfNextBytes = getNoOfNextBytes();

  if(noOfNextBytes > remaining())
  {
    invalidateNoOfNextBytes();
    return false;
  }

  char* cs = new char[noOfNextBytes];
  getBytes(cs, noOfNextBytes);
  std::string s2 = string(cs, noOfNextBytes);
  delete cs;
  if(s2 == s)
  {
    return true;
  }
  else
  {
    (*this) -= noOfNextBytes;
    return false;
  }
}
bool bytestream::nextBytestream(const bytestream& other)
{
  if(!noOfNextBytesValid())
  {
    setNoOfNextBytes(other.size());
  }
  else if(getNoOfNextBytes() != other.size())
  {
    throw invalid_argument("Desired length does not match const length");
  }

  size_t noOfNextBytes = getNoOfNextBytes();

  if(noOfNextBytes > remaining())
  {
    invalidateNoOfNextBytes();
    return false;
  }

  char* cs = new char[noOfNextBytes];
  getBytes(cs, noOfNextBytes);
  bytestream tmp = bytestream(cs, noOfNextBytes);
  delete cs;
  if(tmp == other)
  {
    return true;
  }
  else
  {
    (*this) -= noOfNextBytes;
    return false;
  }
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

void bytestream::setNoOfNextBytes(size_t n)
{
  _noOfNextBytes = n;
  _noOfNextBytesValid = true;
}

void bytestream::invalidateNoOfNextBytes()
{
    _noOfNextBytes = 0;
    _noOfNextBytesValid = false;
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

bytestream bytestream::operator[](size_t i)
{
  bytestream tmp(_data+i, _size-i);
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

bytestream& bytestream::operator/(int i)
{
  setNoOfNextBytes(i);
  return *this;
}

#define PUTOP(type, shorthand, len) PUTOP_(type##len##_t, shorthand##len)
#define PUTOP_(type, shortType) \
  bytestream& bytestream::operator<<(const type& u) \
  {put##shortType(u); return *this;}

PUTOP(uint, U, 8);
PUTOP(uint, U, 16);
PUTOP(uint, U, 32);
PUTOP(uint, U, 64);
PUTOP(int, S, 8);
PUTOP(int, S, 16);
PUTOP(int, S, 32);
PUTOP(int, S, 64);
bytestream& bytestream::operator<<(const std::string& s)
{
  putBytes(s.c_str(), s.length());
  return *this;
}
bytestream& bytestream::operator<<(const bytestream& other)
{
  putBytes(other.raw(), other.size());
  return *this;
}

#define GETOP(type, shorthand, len) GETOP_(type##len##_t, shorthand##len)
#define GETOP_(type, shortType) \
  bytestream& bytestream::operator>>(type& u) \
  {u = get##shortType(); return *this;}

GETOP(uint, U, 8);
GETOP(uint, U, 16);
GETOP(uint, U, 32);
GETOP(uint, U, 64);
GETOP(int, S, 8);
GETOP(int, S, 16);
GETOP(int, S, 32);
GETOP(int, S, 64);

bytestream& bytestream::operator>>(std::string& s)
{
  if(!noOfNextBytesValid())
  {
    throw invalid_argument("No length given");
  }
  size_t noOfNextBytes = getNoOfNextBytes();
  char* cs = new char[noOfNextBytes+1];
  cs[noOfNextBytes] = 0;
  getBytes(cs, noOfNextBytes);
  s = string(cs, noOfNextBytes);
  delete cs;
  return *this;
}
bytestream& bytestream::operator>>(bytestream& other)
{
  if(!noOfNextBytesValid())
  {
    throw invalid_argument("No length given");
  }
  size_t noOfNextBytes = getNoOfNextBytes();
  char* cs = new char[noOfNextBytes];
  getBytes(cs, noOfNextBytes);
  other = bytestream(cs, noOfNextBytes);
  delete cs;
  return *this;
}

#define GETOP_CONST(type, shorthand, len) GETOP_CONST_(type##len##_t, shorthand##len)
#define GETOP_CONST_(type, shortType) \
  bytestream& bytestream::operator>>(const type& u) \
  {if(u!=get##shortType()) {throw invalid_argument("Does not match const");}\
   else{return *this;}}

GETOP_CONST(uint, U, 8);
GETOP_CONST(uint, U, 16);
GETOP_CONST(uint, U, 32);
GETOP_CONST(uint, U, 64);
GETOP_CONST(int, S, 8);
GETOP_CONST(int, S, 16);
GETOP_CONST(int, S, 32);
GETOP_CONST(int, S, 64);

bytestream& bytestream::operator>>(const std::string& s)
{
  if(!noOfNextBytesValid())
  {
    setNoOfNextBytes(s.length());
  }
  else if (getNoOfNextBytes() != s.length())
  {
    throw invalid_argument("Desired length does not match const length");
  }
  size_t noOfNextBytes = getNoOfNextBytes();
  char* cs = new char[noOfNextBytes+1];
  cs[noOfNextBytes] = 0;
  getBytes(cs, noOfNextBytes);
  std::string s2 = string(cs, noOfNextBytes);
  delete cs;
  if(s2 != s)
    throw invalid_argument("Does not match const");
  return *this;
}
bytestream& bytestream::operator>>(const bytestream& other)
{
  if(!noOfNextBytesValid())
  {
    throw invalid_argument("No length given");
  }
  size_t noOfNextBytes = getNoOfNextBytes();
  char* cs = new char[noOfNextBytes];
  getBytes(cs, noOfNextBytes);
  bytestream tmp = bytestream(cs, noOfNextBytes);
  if(tmp != other)
    throw invalid_argument("Does not match const");
  delete cs;
  return *this;
}

#define NEXTOP(type, shorthand, len) NEXTOP_(type##len##_t, shorthand##len)
#define NEXTOP_(type, shortType) \
  bool bytestream::operator>>=(const type& u) \
  {return next##shortType(u);}

NEXTOP(uint, U, 8);
NEXTOP(uint, U, 16);
NEXTOP(uint, U, 32);
NEXTOP(uint, U, 64);
NEXTOP(int, S, 8);
NEXTOP(int, S, 16);
NEXTOP(int, S, 32);
NEXTOP(int, S, 64);
bool bytestream::operator>>=(const std::string& s)
{
  return nextString(s);
}
bool bytestream::operator>>=(const bytestream& other)
{
  return nextBytestream(other);
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

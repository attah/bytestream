#include "bytestream.h"
#include <iostream>
#include <cstdint>
#include <cstring>
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

template<typename T>
T bswap_8(T u)
{
  return u;
}

#define GET(type, shorthand, len) GET_(type##len##_t, shorthand##len, len)
#define GET_(type, shortType, len) \
  type bytestream::get##shortType() \
  {type tmp; getBytes(&tmp, sizeof(type));\
   if(needsSwap()){tmp=bswap_##len(tmp);} return tmp;}

GET(uint, U, 8);
GET(uint, U, 16);
GET(uint, U, 32);
GET(uint, U, 64);
GET(int, S, 8);
GET(int, S, 16);
GET(int, S, 32);
GET(int, S, 64);

std::string bytestream::getString()
{
  if(!_noOfNextBytesValid)
  {
    throw invalid_argument("No length given");
  }
  char* cs = new char[_noOfNextBytes+1];
  cs[_noOfNextBytes] = 0;
  getBytes(cs, _noOfNextBytes);
  string s = std::string(cs, _noOfNextBytes);
  delete cs;
  return s;
}
bytestream bytestream::getBytestream()
{
  if(!_noOfNextBytesValid)
  {
    throw invalid_argument("No length given");
  }
  uint8_t* cs = new uint8_t[_noOfNextBytes];
  getBytes(cs, _noOfNextBytes);
  bytestream other = bytestream(cs, _noOfNextBytes);
  delete cs;
  return other;
}
std::string bytestream::getString(size_t len)
{
  if(_noOfNextBytesValid && len != _noOfNextBytes)
  {
    throw logic_error("Desired lengths does not match");
  }
  else if(!_noOfNextBytesValid)
  {
    setNoOfNextBytes(len);
  }
  return getString();
}
bytestream bytestream::getBytestream(size_t len)
{
  if(!_noOfNextBytesValid && len != _noOfNextBytes)
  {
    throw logic_error("Desired lengths does not match");
  }
  setNoOfNextBytes(len);
  return getBytestream();
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
  if(_noOfNextBytesValid && getNoOfNextBytes() != s.length())
  {
    throw logic_error("Desired length does not match const length");
  }
  else if(!_noOfNextBytesValid)
  {
    setNoOfNextBytes(s.length());
  }

  size_t noOfNextBytes = getNoOfNextBytes();

  if(noOfNextBytes > remaining())
  {
    invalidateNoOfNextBytes();
    return false;
  }

  if(getString() == s)
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
  if(_noOfNextBytesValid && getNoOfNextBytes() != other.size())
  {
    throw logic_error("Desired length does not match const length");
  }
  else if(!_noOfNextBytesValid)
  {
    setNoOfNextBytes(other.size());
  }

  size_t noOfNextBytes = getNoOfNextBytes();

  if(noOfNextBytes > remaining())
  {
    invalidateNoOfNextBytes();
    return false;
  }

  if(getBytestream() == other)
  {
    return true;
  }
  else
  {
    (*this) -= noOfNextBytes;
    return false;
  }
}

#define PUT(type, shorthand, len) PUT_(type##len##_t, shorthand##len, len)
#define PUT_(type, shortType, len) \
  void bytestream::put##shortType(type u) \
  {if(needsSwap()){u=bswap_##len(u);} \
   putBytes(&u, sizeof(u));}

PUT(uint, U, 8);
PUT(uint, U, 16);
PUT(uint, U, 32);
PUT(uint, U, 64);
PUT(int, S, 8);
PUT(int, S, 16);
PUT(int, S, 32);
PUT(int, S, 64);

void bytestream::putString(const std::string& s)
{
  putBytes(s.c_str(), s.length());
}
void bytestream::putBytestream(const bytestream& other)
{
  putBytes(other.raw(), other.size());
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
    invalidateNoOfNextBytes();
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
  putString(s);
  return *this;
}
bytestream& bytestream::operator<<(const bytestream& other)
{
  putBytestream(other);
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
  s = getString();
  return *this;
}
bytestream& bytestream::operator>>(bytestream& other)
{
  other = getBytestream();
  return *this;
}

#define GETOP_CONST(type, shorthand, len) \
  GETOP_CONST_(type##len##_t, shorthand##len)
#define GETOP_CONST_(type, shortType) \
  bytestream& bytestream::operator>>(const type& u) \
  {if(u!=get##shortType()) {(*this) -= sizeof(type);\
                            throw badmatch("Does not match const");}\
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
  if (_noOfNextBytesValid && getNoOfNextBytes() != s.length())
  {
    throw logic_error("Desired length does not match const length");
  }
  else if(!_noOfNextBytesValid)
  {
    setNoOfNextBytes(s.length());
  }
  if(getString() != s)
  {
    (*this) -= s.length();
    throw badmatch("Does not match const");
  }
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

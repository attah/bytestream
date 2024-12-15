#include "bytestream.h"
#include <cstdint>
#include <cstring>
#include <sstream>
#include <iomanip>

Bytestream::Bytestream(Endianness endianness)
: _endianness(endianness)
{}

Bytestream::Bytestream(size_t size, Endianness endianness)
: _data(size), _size(size), _allocated(size), _endianness(endianness)
{
}

Bytestream::Bytestream(size_t size, int pattern, Endianness endianness)
: _data(size), _size(size), _allocated(size), _endianness(endianness)
{
  memset(_data, pattern, _allocated);
}

Bytestream::Bytestream(const void* data, size_t len, Endianness endianness)
: _data(len), _size(len), _allocated(len), _endianness(endianness)
{
  memcpy(_data, data, len);
}

Bytestream::Bytestream(const std::string& str)
{
  putString(str);
}

Bytestream::Bytestream(std::istream& istream)
{
  while(!istream.eof())
  {
    preallocate(BS_REASONABLE_FILE_SIZE);
    istream.read((char*)(_data+_size), BS_REASONABLE_FILE_SIZE);
    _size += istream.gcount();
  }
}

Bytestream::Bytestream(std::istream& istream, size_t len, Endianness endianness)
: _data(len), _endianness(endianness)
{
  istream.read((char*)(_data.get()), len);
  _size = istream.gcount();
}

Bytestream::Bytestream(std::initializer_list<Bytes> bytesList, Endianness endianness)
{
  this->setEndianness(endianness);
  for(const Bytes& bytes : bytesList)
  {
    this->putBytes(bytes);
  }
}

Bytestream::Bytestream(const Bytestream& rhs)
: _data(rhs._size), _size(rhs._size), _allocated(rhs._size), _pos(rhs._pos), _endianness(rhs._endianness)
{
  memcpy(_data, rhs._data, _size);
}

Bytestream::Bytestream(Bytestream&& rhs) noexcept
: _size(rhs._size), _allocated(rhs._allocated), _pos(rhs._pos), _endianness(rhs._endianness)
{
  _data.swap(rhs._data);
  rhs._allocated = 0;
  rhs.reset();
}

bool Bytestream::operator==(const Bytestream& other) const
{
  if(_size != other.size())
  {
    return false;
  }
  return memcmp(_data, other.raw(), _size) == 0;
}
bool Bytestream::operator!=(const Bytestream& other) const
{
  if(_size != other.size())
  {
    return true;
  }
  return memcmp(_data, other.raw(), _size) != 0;
}

Bytestream& Bytestream::operator=(const Bytestream& other)
{
  _pos = other.pos();
  _size = other.size();
  _allocated = _size;
  Array<uint8_t> tmp(_size);
  _data.swap(tmp);
  memcpy(_data, other.raw(), _size);
  return *this;
}

Bytestream& Bytestream::operator=(Bytestream&& other) noexcept
{
  _data = Array<uint8_t>(0);
  _data.swap(other._data);
  _pos = other._pos;
  _size = other._size;
  _allocated = other._allocated;

  other._allocated = 0;
  other.reset();
  return *this;
}

std::string Bytestream::getString(size_t len)
{
  _before(len);
  std::string str((char*)(&(_data[_pos])), len);
  _after(len);
  return str;
}

Bytestream Bytestream::getBytestream(size_t len)
{
  Bytestream other = Bytestream(len);
  getBytes(other.raw(), len);
  return other;
}

void Bytestream::getBytes(void* data, size_t len)
 {
  _before(len);
  memcpy(data, &(_data[_pos]), len);
  _after(len);
}

void Bytestream::getBytes(Bytestream& other, size_t len)
{
  _before(len);
  other.putBytes(&(_data[_pos]), len);
  _after(len);
}

void Bytestream::peekBytes(void* data, size_t len) const
 {
  _before(len);
  memcpy(data, &(_data[_pos]), len);
}

void Bytestream::peekBytes(Bytestream& other, size_t len) const
{
  _before(len);
  other.putBytes(&(_data[_pos]), len);
}

std::string Bytestream::peekString(size_t len) const
{
  _before(len);
  std::string str((char*)(&(_data[_pos])), len);
  return str;
}

Bytestream Bytestream::peekBytestream(size_t len) const
{
  Bytestream other = Bytestream(len);
  peekBytes(other.raw(), len);
  return other;
}

bool Bytestream::nextString(const std::string& str, bool compareEqual)
{
  size_t noOfNextBytes = str.length();

  if(noOfNextBytes > remaining())
  {
    return false;
  }

  bool res = peekString(noOfNextBytes) == str;
  if(res == compareEqual)
  {
    _after(noOfNextBytes);
    return true;
  }
  else
  {
    return false;
  }
}

bool Bytestream::nextBytestream(const Bytestream& bts, bool compareEqual)
{
  size_t noOfNextBytes = bts.size();

  if(noOfNextBytes > remaining())
  {
    return false;
  }

  bool res = memcmp(&(_data[_pos]), bts.raw(), noOfNextBytes) == 0;
  if(res == compareEqual)
  {
    _after(noOfNextBytes);
    return true;
  }
  else
  {
    return false;
  }
}

void Bytestream::putString(const std::string& str)
{
  putBytes(str.c_str(), str.length());
}
void Bytestream::putBytestream(const Bytestream& other)
{
  putBytes(other.raw(), other.size());
}

void Bytestream::putBytes(const void* data, size_t len)
{
  preallocate(len);

  memcpy((_data+_size), data, len);
  _size += len;
}

void Bytestream::putPattern(size_t len, uint8_t pattern)
{
  preallocate(len);
  memset(_data+_size, pattern, len);
  _size += len;
}

void Bytestream::preallocate(size_t extra)
{
  size_t new_size = _size+extra;
  if(new_size > _allocated)
  {
    size_t next_size = std::max(2*_allocated, new_size);

    Array<uint8_t> tmp(next_size);
    _data.swap(tmp);
    _allocated = next_size;

    if(_size != 0)
    {
      memcpy(_data, tmp, _size);
    }
  }
}

Array<uint8_t> Bytestream::eject(bool prealloc)
{
  size_t oldAllocation = _allocated;
  Array<uint8_t> tmp;
  std::swap(_data, tmp);
  _allocated = 0;
  reset();
  if(prealloc)
  {
    preallocate(oldAllocation);
  }
  return tmp;
}

Bytestream Bytestream::operator[](size_t offset)
{
  Bytestream tmp(_data+offset, _size-offset);
  return tmp;
}

Bytestream& Bytestream::operator+=(size_t offset)
{
  if((_pos+offset) > _size)
  {
    throw std::out_of_range("Tried to address data past end");
  }
  _pos += offset;
  return *this;
}

Bytestream& Bytestream::operator-=(size_t offset)
{
  if(offset > _pos)
  {
    throw std::out_of_range("Tried to address data before start");
  }
  _pos -= offset;
  return *this;
}

void Bytestream::setPos(size_t pos)
{
  if(pos > _pos)
  {
    *this += (pos-_pos);
  }
  else
  {
    *this -= (_pos-pos);
  }
}

void Bytestream::reset()
{
  _pos = 0;
  _size = 0;
}

std::string Bytestream::hexdump(size_t length) const
{
  std::stringstream out;
  uint32_t addr = 0;

  Bytestream tmp = peekBytestream(std::min(remaining(), length));
  while(tmp.remaining())
  {
    std::stringstream hex;
    std::stringstream ascii;
    out << std::setfill('0') << std::setw(8) << std::hex << addr << ": ";

    for(size_t i = 0; i < 16; i++)
    {
      uint8_t byte = tmp.get<uint8_t>();
      hex << std::setfill('0') << std::setw(2) << std::hex << +byte;
      ascii << ((byte < '!' || byte > '~') ? '.' : (char)byte);

      if(i%2==1)
      {
        hex << " ";
      }
      if(i==7 && tmp.remaining())
      {
        hex << " ";
        ascii << " ";
      }

      if(!tmp.remaining())
      {
        break;
      }
    }

    out << hex.str()
        << std::setfill(' ') << std::setw(42 - hex.str().length()) << " "
        << ascii.str() << std::endl;
    addr += 16;
  }

  return out.str();
}

Bytestream::operator bool() const
{
  return _size != 0;
}

Bytestream& Bytestream::operator<<(const std::string& str)
{
  putString(str);
  return *this;
}
Bytestream& Bytestream::operator<<(const Bytestream& other)
{
  putBytestream(other);
  return *this;
}

Bytestream& Bytestream::operator>>(const std::string& str)
{
  std::string value = peekString(str.length());
  if(value != str)
  {
    throw Badmatch("Does not match const", value, str);
  }
  _after(str.length());
  return *this;
}

bool Bytestream::operator>>=(const std::string& str)
{
  return nextString(str);
}
bool Bytestream::operator>>=(const Bytestream& other)
{
  return nextBytestream(other);
}

std::ostream& operator<<(std::ostream& ostream, Bytestream& bts)
{
  ostream.write((char*)bts.raw(), bts.size());
  return ostream;
}

void Bytestream::_before(size_t bytesToRead) const
{
  if(bytesToRead > remaining())
  {
    throw std::out_of_range("Tried to read past end");
  }
}

void Bytestream::_after(size_t bytesRead)
{
  _pos += bytesRead;
}

void Bytestream::putBytes(const Bytes& bytes)
{
  if(std::holds_alternative<uint8_t>(bytes))
  {
    *this << std::get<uint8_t>(bytes);
  }
  else if(std::holds_alternative<uint16_t>(bytes))
  {
    *this << std::get<uint16_t>(bytes);
  }
  else if(std::holds_alternative<uint32_t>(bytes))
  {
    *this << std::get<uint32_t>(bytes);
  }
  else if(std::holds_alternative<uint64_t>(bytes))
  {
    *this << std::get<uint64_t>(bytes);
  }
  else if(std::holds_alternative<int8_t>(bytes))
  {
    *this << std::get<int8_t>(bytes);
  }
  else if(std::holds_alternative<int16_t>(bytes))
  {
    *this << std::get<int16_t>(bytes);
  }
  else if(std::holds_alternative<int32_t>(bytes))
  {
    *this << std::get<int32_t>(bytes);
  }
  else if(std::holds_alternative<int64_t>(bytes))
  {
    *this << std::get<int64_t>(bytes);
  }
  else if(std::holds_alternative<float32_t>(bytes))
  {
    *this << std::get<float32_t>(bytes);
  }
  else if(std::holds_alternative<float64_t>(bytes))
  {
    *this << std::get<float64_t>(bytes);
  }
  else if(std::holds_alternative<std::string>(bytes))
  {
    *this << std::get<std::string>(bytes);
  }
  else
  {
    throw std::logic_error("Invalid bytes");
  }
}

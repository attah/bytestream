#ifndef BYTESTREAM_H
#define  BYTESTREAM_H
#include <string>
#include <stdexcept>
#include <variant>
#include <cstdint>
#include <byteswap.h>
#include "array.h"
#ifndef __STDC_IEC_559__
  #ifndef TRUST_ME_I_HAVE_GOOD_FLOATS
    #error "Double must be IEEE 754"
  #endif
#endif
#define float32_t float
#define float64_t double

#define BS_REASONABLE_FILE_SIZE 4096

class Bytes;

class Bytestream
{
public:

  class Badmatch : public std::invalid_argument::invalid_argument
  {
  public:
    Badmatch(std::string s, std::string v, std::string u) :
        invalid_argument(s+": "+v+" != "+u) {}
    template<typename T>
    Badmatch(std::string s, T v, T u) :
        invalid_argument(s+": "+std::to_string(v)+" != "+std::to_string(u)) {}

  };

  enum Endianness {
    NativeEndian,
    BigEndian,
    LittleEndian
  };

  Bytestream(Endianness e = BigEndian);
  Bytestream(size_t size, Endianness e = BigEndian);
  Bytestream(size_t size, int pattern, Endianness e = BigEndian);
  Bytestream(const void* data, size_t len, Endianness e = BigEndian);
  Bytestream(const std::string& s);
  Bytestream(std::istream& is);
  Bytestream(std::istream& is, size_t len, Endianness e = BigEndian);
  Bytestream(std::initializer_list<Bytes> il, Endianness e = BigEndian);

  Bytestream(const Bytestream& rhs);
  Bytestream(Bytestream&& rhs);
  ~Bytestream();

  bool operator==(const Bytestream& other) const;
  bool operator!=(const Bytestream& other) const;

  Bytestream& operator=(const Bytestream& other);
  Bytestream& operator=(Bytestream&& other);

  uint8_t* raw() const {return _data;}
  size_t size() const {return _size;}
  size_t pos() const {return _pos;}
  size_t remaining() const {return _size - _pos;}
  bool atEnd() const {return _pos >= _size;}
  void setPos(size_t pos);
  Endianness getEndianness() const {return _endianness;}
  void setEndianness(Endianness e) {_endianness = e;}
  void reset();
  std::string hexdump(size_t length) const;

  template<typename T, typename std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
  T get()
  {
    T tmp;
    getBytes(&tmp, sizeof(T));
    maybeByteSwap(tmp);
    return tmp;
  }

  std::string getString(size_t len);
  Bytestream getBytestream(size_t len);
  void getBytes(void* cs, size_t len);
  void getBytes(Bytestream& other,  size_t len);
  void peekBytes(void* cs, size_t len) const ;
  void peekBytes(Bytestream& other, size_t len) const;

  template<typename T, typename std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
  T peek() const
  {
    T tmp;
    peekBytes(&tmp, sizeof(T));
    maybeByteSwap(tmp);
    return tmp;
  }

  std::string peekString(size_t len) const;
  Bytestream peekBytestream(size_t len) const;

  template<typename T, typename std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
  bool next(const T& u)
  {
    if(remaining() < sizeof(T))
    {
      return false;
    }
    else if(u == peek<T>())
    {
      _after(sizeof(T));
      return true;
    }
    else
    {
      return false;
    }
  }

  bool nextString(const std::string& bts);
  bool nextBytestream(const Bytestream& bts);

  template<typename T, typename std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
  void put(T u)
  {
    maybeByteSwap(u);
    putBytes(&u, sizeof(T));
  }

  void putString(const std::string&);
  void putBytestream(const Bytestream&);
  void putBytes(const void* c, size_t len);

  Bytestream operator[](size_t i);
  Bytestream& operator+=(size_t i);
  Bytestream& operator-=(size_t i);

  template<typename T, typename std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
  Bytestream& operator<<(T u)
  {
    put<T>(u);
    return *this;
  }
  Bytestream& operator<<(const std::string& s);
  Bytestream& operator<<(const Bytestream& other);

  template<typename T, typename std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
  Bytestream& operator>>(T& u)
  {
    u = get<T>();
    return *this;
  }
  Bytestream& operator>>(std::string& s);
  Bytestream& operator>>(Bytestream& other);

  template<typename T, typename std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
  Bytestream& operator>>(const T& u)
  {
    T v = get<T>();
    if(u!=v)
    {
      (*this) -= sizeof(T);
      throw Badmatch("Does not match const", v, u);
    }
    else
    {
      return *this;
    }
  }
  Bytestream& operator>>(const std::string& s);

  template<typename T, typename std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
  bool operator>>=(const T& u)
  {
    return next<T>(u);
  }
  bool operator>>=(const std::string& s);
  bool operator>>=(const Bytestream& other);

  void putPattern(size_t len, uint8_t pattern);

  void preallocate(size_t extra);
  size_t allocated() const
  {
    return _allocated;
  }
  Array<uint8_t> eject(bool prealloc = false);

private:
  Array<uint8_t> _data;
  size_t _size = 0;
  size_t _allocated = 0;
  size_t _pos = 0;
  Endianness _endianness = BigEndian;

  void _before(size_t bytesToRead) const;
  void _after(size_t bytesRead);

  void putBytes(const Bytes& b);

  template <typename T>
  void maybeByteSwap(T& u) const
  {
    #if __BYTE_ORDER == __LITTLE_ENDIAN
    bool needsSwap = _endianness == Endianness::BigEndian;
    #elif __BYTE_ORDER == __BIG_ENDIAN
    bool needsSwap = endianness == Endianness::LittleEndian;
    #else
    #error
    #endif
    if(needsSwap)
    {
      uint8_t* const p = reinterpret_cast<uint8_t*>(&u);
      for(size_t i = 0; i < sizeof(T) / 2; i++)
      {
        std::swap(p[i], p[sizeof(T) - i - 1]);
      }
    }
  }
  void maybeByteSwap(int8_t&) const {}
  void maybeByteSwap(uint8_t&) const {}

};

std::ostream& operator<<(std::ostream& os, Bytestream& bts);

class Bytes: public std::variant<uint8_t, uint16_t, uint32_t, uint64_t,
                                 int8_t, int16_t, int32_t, int64_t,
                                 float32_t, float64_t, std::string>
{
  using std::variant<uint8_t, uint16_t, uint32_t, uint64_t,
                     int8_t, int16_t, int32_t, int64_t,
                     float32_t, float64_t, std::string>::variant;

};

#endif

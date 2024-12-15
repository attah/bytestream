#ifndef BYTESTREAM_H
#define  BYTESTREAM_H
#include <string>
#include <stdexcept>
#include <variant>
#include <cstdint>
#include "array.h"

#define float32_t float
#define float64_t double

#define FOR_FIXED_WIDTH(T) typename std::enable_if<std::disjunction<std::is_same<T, uint8_t>, \
                                                                    std::is_same<T, uint16_t>, \
                                                                    std::is_same<T, uint32_t>, \
                                                                    std::is_same<T, uint64_t>, \
                                                                    std::is_same<T, int8_t>, \
                                                                    std::is_same<T, int16_t>, \
                                                                    std::is_same<T, int32_t>, \
                                                                    std::is_same<T, int64_t>, \
                                                                    std::is_same<T, float32_t>, \
                                                                    std::is_same<T, float64_t> \
                                                                   >::value>::type* = nullptr


#define BS_REASONABLE_FILE_SIZE 4096

class Bytes;

class Bytestream
{
  static_assert(sizeof(float32_t) == 4);
  static_assert(sizeof(float64_t) == 8);

public:

  class Badmatch : public std::invalid_argument::invalid_argument
  {
  public:
    Badmatch(std::string str, std::string value, std::string expected) :
        invalid_argument(str+": "+value+" != "+expected) {}
    template<typename T>
    Badmatch(std::string str, T value, T expected) :
        invalid_argument(str+": "+std::to_string(value)+" != "+std::to_string(expected)) {}

  };

  enum Endianness {
    NativeEndian,
    BigEndian,
    LittleEndian
  };

  Bytestream(Endianness endianness = BigEndian);
  Bytestream(size_t size, Endianness endianness = BigEndian);
  Bytestream(size_t size, int pattern, Endianness endianness = BigEndian);
  Bytestream(const void* data, size_t len, Endianness endianness = BigEndian);
  Bytestream(const std::string& str);
  Bytestream(std::istream& istream);
  Bytestream(std::istream& istream, size_t len, Endianness endianness = BigEndian);
  Bytestream(std::initializer_list<Bytes> bytesList, Endianness endianness = BigEndian);

  Bytestream(const Bytestream& rhs);
  Bytestream(Bytestream&& rhs) noexcept;
  ~Bytestream() = default;

  bool operator==(const Bytestream& other) const;
  bool operator!=(const Bytestream& other) const;

  Bytestream& operator=(const Bytestream& other);
  Bytestream& operator=(Bytestream&& other) noexcept;

  uint8_t* raw() const {return _data;}
  size_t size() const {return _size;}
  size_t pos() const {return _pos;}
  size_t remaining() const {return _size - _pos;}
  bool atEnd() const {return _pos >= _size;}
  void setPos(size_t pos);
  Endianness getEndianness() const {return _endianness;}
  void setEndianness(Endianness endianness) {_endianness = endianness;}
  void reset();
  std::string hexdump(size_t length) const;

  operator bool() const;

  template <typename T, FOR_FIXED_WIDTH(T)>
  T get()
  {
    T tmp;
    getBytes(&tmp, sizeof(T));
    maybeByteSwap(tmp);
    return tmp;
  }

  std::string getString(size_t len);
  Bytestream getBytestream(size_t len);
  void getBytes(void* data, size_t len);
  void getBytes(Bytestream& other, size_t len);
  void peekBytes(void* data, size_t len) const;
  void peekBytes(Bytestream& other, size_t len) const;

  template <typename T, FOR_FIXED_WIDTH(T)>
  T peek() const
  {
    T tmp;
    peekBytes(&tmp, sizeof(T));
    maybeByteSwap(tmp);
    return tmp;
  }

  std::string peekString(size_t len) const;
  Bytestream peekBytestream(size_t len) const;

  template <typename T, FOR_FIXED_WIDTH(T)>
  bool next(const T& expected)
  {
    if(remaining() < sizeof(T))
    {
      return false;
    }
    else if(expected == peek<T>())
    {
      _after(sizeof(T));
      return true;
    }
    else
    {
      return false;
    }
  }

  bool nextString(const std::string& str, bool compareEqual=true);
  bool nextBytestream(const Bytestream& bts, bool compareEqual=true);

  template <typename T, FOR_FIXED_WIDTH(T)>
  void put(T value)
  {
    maybeByteSwap(value);
    putBytes(&value, sizeof(T));
  }

  void putString(const std::string&);
  void putBytestream(const Bytestream&);
  void putBytes(const void* data, size_t len);

  Bytestream operator[](size_t offset);
  Bytestream& operator+=(size_t offset);
  Bytestream& operator-=(size_t offset);

  template <typename T, FOR_FIXED_WIDTH(T)>
  Bytestream& operator<<(T value)
  {
    put<T>(value);
    return *this;
  }
  Bytestream& operator<<(const std::string& str);
  Bytestream& operator<<(const Bytestream& other);

  template <typename T, FOR_FIXED_WIDTH(T)>
  Bytestream& operator>>(T& value)
  {
    value = get<T>();
    return *this;
  }
  Bytestream& operator>>(std::string& str);
  Bytestream& operator>>(Bytestream& other);

  template <typename T, FOR_FIXED_WIDTH(T)>
  Bytestream& operator>>(const T& expected)
  {
    T value = get<T>();
    if(expected!=value)
    {
      (*this) -= sizeof(T);
      throw Badmatch("Does not match const", value, expected);
    }
    else
    {
      return *this;
    }
  }
  Bytestream& operator>>(const std::string& str);

  template <typename T, FOR_FIXED_WIDTH(T)>
  bool operator>>=(const T& expected)
  {
    return next<T>(expected);
  }
  bool operator>>=(const std::string& str);
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
  void maybeByteSwap(T& value) const
  {
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    bool needsSwap = _endianness == Endianness::BigEndian;
    #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    bool needsSwap = _endianness == Endianness::LittleEndian;
    #else
    #error
    #endif
    if(needsSwap)
    {
      uint8_t* const p = reinterpret_cast<uint8_t*>(&value);
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

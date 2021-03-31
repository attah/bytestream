#ifndef BYTESTREAM_H
#define  BYTESTREAM_H
#include <string>
#include <stdexcept>
#include <stdint.h>
#include <byteswap.h>
#ifndef __STDC_IEC_559__
#error "Double must be IEEE 754"
#endif
#define float32_t float
#define float64_t double

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

  Bytestream();
  Bytestream(size_t size);
  Bytestream(int pattern, size_t len);
  Bytestream(const void* data, size_t len);
  Bytestream(const void* data, size_t len, Endianness e);
  Bytestream(std::initializer_list<Bytes> il);
  Bytestream(std::initializer_list<Bytes> il, Endianness e);

  Bytestream(const Bytestream& rhs);
  ~Bytestream();

  bool operator==(const Bytestream& other) const;
  bool operator!=(const Bytestream& other) const;

  Bytestream& operator=(const Bytestream& other);

  uint8_t* raw() const {return _data;}
  size_t size() const {return _size;}
  size_t pos() const {return _pos;}
  size_t remaining() const {return _size - _pos;}
  bool atEnd() const {return _pos >= _size;}
  void setPos(size_t pos);
  Endianness getEndianness() {return _endianness;}
  void setEndianness(Endianness e) {_endianness = e;}
  void reset();

  uint8_t getU8();
  uint16_t getU16();
  uint32_t getU32();
  uint64_t getU64();
  int8_t getS8();
  int16_t getS16();
  int32_t getS32();
  int64_t getS64();
  float32_t getF32();
  float64_t getF64();
  std::string getString();
  Bytestream getBytestream();
  std::string getString(size_t len);
  Bytestream getBytestream(size_t len);
  void getBytes(void* cs,  size_t len);
  void getBytes(Bytestream& other,  size_t len);

  uint8_t peekU8();
  uint16_t peekU16();
  uint32_t peekU32();
  uint64_t peekU64();
  int8_t peekS8();
  int16_t peekS16();
  int32_t peekS32();
  int64_t peekS64();
  float32_t peekF32();
  float64_t peekF64();
  std::string peekString();
  Bytestream peekBytestream();
  std::string peekString(size_t len);
  Bytestream peekBytestream(size_t len);

  bool peekNextBytestream(Bytestream other);

  bool nextU8(uint8_t);
  bool nextU16(uint16_t);
  bool nextU32(uint32_t);
  bool nextU64(uint64_t);
  bool nextS8(int8_t);
  bool nextS16(int16_t);
  bool nextS32(int32_t);
  bool nextS64(int64_t);
  bool nextF32(float32_t);
  bool nextF64(float64_t);
  bool nextString(const std::string& bts);
  bool nextBytestream(const Bytestream& bts, bool compareEqual=true);

  void putU8(uint8_t);
  void putU16(uint16_t);
  void putU32(uint32_t);
  void putU64(uint64_t);
  void putS8(int8_t);
  void putS16(int16_t);
  void putS32(int32_t);
  void putS64(int64_t);
  void putF32(float32_t);
  void putF64(float64_t);
  void putString(const std::string&);
  void putBytestream(const Bytestream&);
  void putBytes(const void* c, size_t len);

  void setNoOfNextBytes(size_t n);
  void invalidateNoOfNextBytes();
  size_t getNoOfNextBytes() {return _noOfNextBytes;}
  bool noOfNextBytesValid() const {return _noOfNextBytesValid;}

  Bytestream operator[](size_t i);
  Bytestream& operator+=(size_t i);
  Bytestream& operator-=(size_t i);

  Bytestream& operator/(size_t i);

  Bytestream& operator<<(const uint8_t& u);
  Bytestream& operator<<(const uint16_t& u);
  Bytestream& operator<<(const uint32_t& u);
  Bytestream& operator<<(const uint64_t& u);
  Bytestream& operator<<(const int8_t& u);
  Bytestream& operator<<(const int16_t& u);
  Bytestream& operator<<(const int32_t& u);
  Bytestream& operator<<(const int64_t& u);
  Bytestream& operator<<(const float32_t& u);
  Bytestream& operator<<(const float64_t& u);
  Bytestream& operator<<(const std::string& s);
  Bytestream& operator<<(const Bytestream& other);

  Bytestream& operator>>(uint8_t& u);
  Bytestream& operator>>(uint16_t& u);
  Bytestream& operator>>(uint32_t& u);
  Bytestream& operator>>(uint64_t& u);
  Bytestream& operator>>(int8_t& u);
  Bytestream& operator>>(int16_t& u);
  Bytestream& operator>>(int32_t& u);
  Bytestream& operator>>(int64_t& u);
  Bytestream& operator>>(float32_t& u);
  Bytestream& operator>>(float64_t& u);
  Bytestream& operator>>(std::string& s);
  Bytestream& operator>>(Bytestream& other);

  Bytestream& operator>>(const uint8_t& u);
  Bytestream& operator>>(const uint16_t& u);
  Bytestream& operator>>(const uint32_t& u);
  Bytestream& operator>>(const uint64_t& u);
  Bytestream& operator>>(const int8_t& u);
  Bytestream& operator>>(const int16_t& u);
  Bytestream& operator>>(const int32_t& u);
  Bytestream& operator>>(const int64_t& u);
  Bytestream& operator>>(const float32_t& u);
  Bytestream& operator>>(const float64_t& u);
  Bytestream& operator>>(const std::string& s);

  bool operator>>=(const uint8_t& u);
  bool operator>>=(const uint16_t& u);
  bool operator>>=(const uint32_t& u);
  bool operator>>=(const uint64_t& u);
  bool operator>>=(const int8_t& u);
  bool operator>>=(const int16_t& u);
  bool operator>>=(const int32_t& u);
  bool operator>>=(const int64_t& u);
  bool operator>>=(const float32_t& u);
  bool operator>>=(const float64_t& u);
  bool operator>>=(const std::string& s);
  bool operator>>=(const Bytestream& other);


private:
  uint8_t* _data;
  size_t _size;
  size_t _allocated;
  size_t _pos;
  size_t _noOfNextBytes;
  bool _noOfNextBytesValid;

  Endianness _endianness;

  bool needsSwap();

  void _after(size_t bytesRead);
  void _before(size_t bytesToRead);
};

class Bytes
{
  friend Bytestream& operator<<(Bytestream& bts, const Bytes& b);

public:
  Bytes(uint8_t u);
  Bytes(uint16_t u);
  Bytes(uint32_t u);
  Bytes(uint64_t u);
  Bytes(int8_t u);
  Bytes(int16_t u);
  Bytes(int32_t u);
  Bytes(int64_t u);
  Bytes(float32_t f);
  Bytes(float64_t f);
  Bytes(std::string s);

  Bytes(const Bytes& b);
  ~Bytes();

private:
  Bytes();

  enum Type {
    u8,
    u16,
    u32,
    u64,
    i8,
    i16,
    i32,
    i64,
    f32,
    f64,
    s
  } type;

  union {
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    int8_t i8;
    int16_t i16;
    int32_t i32;
    int64_t i64;
    float32_t f32;
    float64_t f64;
    std::string* s;
  } u;

};

Bytestream& operator<<(Bytestream& bts, const Bytes&);

#endif

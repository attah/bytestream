#ifndef BYTESTREAM_H
#define  BYTESTREAM_H
#include <string>
#include <stdexcept>

class bytestream
{
public:

  class badmatch : public std::invalid_argument::invalid_argument
  {
  public:
    badmatch(std::string s, std::string v, std::string u) :
        invalid_argument(s+": "+v+" != "+u) {}
    template<typename T>
    badmatch(std::string s, T v, T u) :
        invalid_argument(s+": "+std::to_string(v)+" != "+std::to_string(u)) {}

  };

  enum Endianness {
    big,
    little,
    native
  };

  bytestream();
  bytestream(size_t len);
  bytestream(const void* data, size_t len);
  bytestream(const bytestream& rhs);
  ~bytestream();

  bool operator==(const bytestream& other) const;
  bool operator!=(const bytestream& other) const;

  bytestream& operator=(const bytestream& other);

  uint8_t* raw() const {return _data;}
  size_t size() const {return _size;}
  size_t pos() const {return _pos;}
  size_t remaining() const {return _size - _pos;}
  bool atEnd() const {return _pos >= _size;}

  void setPos(size_t pos) {_pos = pos;}


  uint8_t getU8();
  uint16_t getU16();
  uint32_t getU32();
  uint64_t getU64();
  int8_t getS8();
  int16_t getS16();
  int32_t getS32();
  int64_t getS64();
  std::string getString();
  bytestream getBytestream();
  std::string getString(size_t len);
  bytestream getBytestream(size_t len);
  void getBytes(void* cs,  size_t len);

  uint8_t peekU8();
  uint16_t peekU16();
  uint32_t peekU32();
  uint64_t peekU64();
  int8_t peekS8();
  int16_t peekS16();
  int32_t peekS32();
  int64_t peekS64();
  std::string peekString();
  bytestream peekBytestream();
  std::string peekString(size_t len);
  bytestream peekBytestream(size_t len);

  bool nextU8(uint8_t);
  bool nextU16(uint16_t);
  bool nextU32(uint32_t);
  bool nextU64(uint64_t);
  bool nextS8(int8_t);
  bool nextS16(int16_t);
  bool nextS32(int32_t);
  bool nextS64(int64_t);
  bool nextString(const std::string& bts);
  bool nextBytestream(const bytestream& bts);

  void putU8(uint8_t);
  void putU16(uint16_t);
  void putU32(uint32_t);
  void putU64(uint64_t);
  void putS8(int8_t);
  void putS16(int16_t);
  void putS32(int32_t);
  void putS64(int64_t);
  void putString(const std::string&);
  void putBytestream(const bytestream&);
  void putBytes(const void* c, size_t len);

  void setNoOfNextBytes(size_t n);
  void invalidateNoOfNextBytes();
  size_t getNoOfNextBytes() {return _noOfNextBytes;}
  bool noOfNextBytesValid() const {return _noOfNextBytesValid;}

  bytestream operator[](size_t i);
  bytestream& operator+=(size_t i);
  bytestream& operator-=(size_t i);

  bytestream& operator/(int i);

  bytestream& operator<<(const uint8_t& u);
  bytestream& operator<<(const uint16_t& u);
  bytestream& operator<<(const uint32_t& u);
  bytestream& operator<<(const uint64_t& u);
  bytestream& operator<<(const int8_t& u);
  bytestream& operator<<(const int16_t& u);
  bytestream& operator<<(const int32_t& u);
  bytestream& operator<<(const int64_t& u);
  bytestream& operator<<(const std::string& s);
  bytestream& operator<<(const bytestream& other);

  bytestream& operator>>(uint8_t& u);
  bytestream& operator>>(uint16_t& u);
  bytestream& operator>>(uint32_t& u);
  bytestream& operator>>(uint64_t& u);
  bytestream& operator>>(int8_t& u);
  bytestream& operator>>(int16_t& u);
  bytestream& operator>>(int32_t& u);
  bytestream& operator>>(int64_t& u);
  bytestream& operator>>(std::string& s);
  bytestream& operator>>(bytestream& other);

  bytestream& operator>>(const uint8_t& u);
  bytestream& operator>>(const uint16_t& u);
  bytestream& operator>>(const uint32_t& u);
  bytestream& operator>>(const uint64_t& u);
  bytestream& operator>>(const int8_t& u);
  bytestream& operator>>(const int16_t& u);
  bytestream& operator>>(const int32_t& u);
  bytestream& operator>>(const int64_t& u);
  bytestream& operator>>(const std::string& s);

  bool operator>>=(const uint8_t& u);
  bool operator>>=(const uint16_t& u);
  bool operator>>=(const uint32_t& u);
  bool operator>>=(const uint64_t& u);
  bool operator>>=(const int8_t& u);
  bool operator>>=(const int16_t& u);
  bool operator>>=(const int32_t& u);
  bool operator>>=(const int64_t& u);
  bool operator>>=(const std::string& s);
  bool operator>>=(const bytestream& other);


private:
  uint8_t* _data;
  size_t _size;
  size_t _pos;
  size_t _noOfNextBytes;
  bool _noOfNextBytesValid;

  Endianness endianness;

  bool needsSwap();

  void _after(size_t bytesRead);
  void _before(size_t bytesToRead);
};

#endif

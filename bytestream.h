#include <string>
using namespace std;

class bytestream
{
public:

  enum Endianness {
    big,
    little,
    native
  };

  bytestream();
  bytestream(const void* data, size_t len);
  bytestream(const bytestream& rhs);
  ~bytestream();

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
  void getBytes(void* cs,  size_t len);

  void putU8(uint8_t);
  void putU16(uint16_t);
  void putU32(uint32_t);
  void putU64(uint64_t);
  void putBytes(const void* c, size_t len);

  void setNoOfNextBytes(int n);
  size_t getNoOfNextBytes() {return noOfNextBytes;}

  bytestream operator[](size_t i);
  bytestream& operator+=(size_t i);
  bytestream& operator-=(size_t i);

private:
  uint8_t* _data;
  size_t _size;
  size_t _pos;
  size_t noOfNextBytes;
  bool noOfNextBytesValid;

  Endianness endianness;

  bool needsSwap();

  void _after(size_t bytesRead);
  void _before(size_t bytesToRead);
};

bytestream& operator>>(bytestream& b, uint8_t& u);
bytestream& operator>>(bytestream& b, uint16_t& u);
bytestream& operator>>(bytestream& b, uint32_t& u);
bytestream& operator>>(bytestream& b, uint64_t& u);
bytestream& operator>>(bytestream& b, string& s);

bytestream& operator/(bytestream& b, int i);

bytestream& operator<<(bytestream& b, const uint8_t& u);
bytestream& operator<<(bytestream& b, const uint16_t& u);
bytestream& operator<<(bytestream& b, const uint32_t& u);
bytestream& operator<<(bytestream& b, const uint64_t& u);
bytestream& operator<<(bytestream& b, const string& s);
bytestream& operator<<(bytestream& b, const bytestream& other);

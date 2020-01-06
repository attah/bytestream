#include <sstream>

#define CODABLE(name) \
class name \
{ \
public:

  #define FIELD(type, name) type name;
  #define STRING(length, name) std::string name;
  #define PADDING(length)

  #include CODABLE_FILE

  #undef FIELD
  #undef STRING
  #undef CODABLE

  #define CODABLE(name) \
  name() \
  {
    #define FIELD(type, name) name = type();
    #define STRING(length, name)
    #include CODABLE_FILE
  }

  #undef FIELD
  #undef CODABLE

  #define CODABLE(name) \
  name(Bytestream& bts) \
  {
    #define FIELD(type, name);
    #include CODABLE_FILE
    this->decode_from(bts);
  }

  #undef CODABLE
  #define CODABLE(name)
  #undef FIELD
  #undef STRING
  #undef PADDING

  void decode_from(Bytestream& bts)
  {
    #define FIELD(type, name) bts >> name;
    #define STRING(length, name) bts/length >> name;
    #define PADDING(length) bts += length;

    #include CODABLE_FILE
  }

  #undef FIELD
  #undef STRING
  #undef PADDING

  void encode_into(Bytestream& bts)
  {
    #define FIELD(type, name) bts << name;
    #define STRING(length, name) name.resize(length); bts << name;
    #define PADDING(length) {std::string tmp(length, 0); bts << tmp;}

    #include CODABLE_FILE
  }

  Bytestream encode()
  {
    Bytestream bts;
    encode_into(bts);
    return bts;
  }

  #undef FIELD
  #undef STRING
  #undef PADDING

  std::string describe()
  {
    std::stringstream ss;
    #define FIELD(type, name) ss << "FIELD " << #type << " " << #name << " " \
                               << name << std::endl;
    #define STRING(length, name) ss << "STRING " << #name << " \"" \
                                    << name << "\"" << std::endl;
    #define PADDING(length) ss << "PADDING "<< length << std::endl;

    #include CODABLE_FILE

    return ss.str();
  }

  #undef FIELD
  #undef STRING
  #undef PADDING

};

#undef CODABLE_FILE

#include <sstream>

#define CODABLE(name) \
class name \
{ \
public:

  #define FIELD(type, name) type name;
  #define DEFAULT_FIELD(type, name, default) FIELD(type, name)
  #define STRING(length, name) std::string name;
  #define CONST_STRING(name, value) const std::string name = value;
  #define DEFAULT_STRING(length, name, default) STRING(length, name)
  #define PADDING(length)

  #include CODABLE_FILE

  #undef FIELD
  #undef DEFAULT_FIELD
  #undef STRING
  #undef CONST_STRING
  #undef DEFAULT_STRING
  #undef CODABLE

  #define CODABLE(name) \
  name() \
  {
    #define FIELD(type, name) name = type();
    #define DEFAULT_FIELD(type, name, default) name = default;
    #define STRING(length, name)
    #define CONST_STRING(name, value)
    #define DEFAULT_STRING(length, name, default) name = default;
    #include CODABLE_FILE
  }

  #undef FIELD
  #undef DEFAULT_FIELD
  #undef STRING
  #undef CONST_STRING
  #undef DEFAULT_STRING
  #undef CODABLE

  #define CODABLE(name) \
  name(Bytestream& bts) \
  {
    #define FIELD(type, name)
    #define DEFAULT_FIELD(type, name, default)
    #define STRING(length, name)
    #define CONST_STRING(name, value)
    #define DEFAULT_STRING(length, name, default)
    #include CODABLE_FILE
    this->decode_from(bts);
  }

  #undef CODABLE
  #define CODABLE(name)
  #undef FIELD
  #undef DEFAULT_FIELD
  #undef STRING
  #undef CONST_STRING
  #undef DEFAULT_STRING
  #undef PADDING

  void decode_from(Bytestream& bts)
  {
    #define FIELD(type, name) bts >> name;
    #define DEFAULT_FIELD(type, name, default) FIELD(type, name)
    #define STRING(length, name) bts/length >> name;
    #define CONST_STRING(name, value) bts >> name;
    #define DEFAULT_STRING(length, name, default) STRING(length, name)
    #define PADDING(length) bts += length;

    #include CODABLE_FILE
  }

  #undef FIELD
  #undef DEFAULT_FIELD
  #undef STRING
  #undef CONST_STRING
  #undef DEFAULT_STRING
  #undef PADDING

  void encode_into(Bytestream& bts)
  {
    #define FIELD(type, name) bts << name;
    #define DEFAULT_FIELD(type, name, default) FIELD(type, name)
    #define STRING(length, name) name.resize(length); bts << name;
    #define CONST_STRING(name, value) bts << name;
    #define DEFAULT_STRING(length, name, default) STRING(length, name)
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
  #undef DEFAULT_FIELD
  #undef STRING
  #undef CONST_STRING
  #undef DEFAULT_STRING
  #undef PADDING

  size_t encoded_size()
  {
    size_t size = 0;
    #define FIELD(type, name) size += sizeof(type);
    #define DEFAULT_FIELD(type, name, default) size += sizeof(type);
    #define STRING(length, name) size += length;
    #define CONST_STRING(name, value) size += strlen(value);
    #define DEFAULT_STRING(length, name, default) size += length;
    #define PADDING(length) size += length;
    #include CODABLE_FILE
    return size;
  }

  #undef FIELD
  #undef DEFAULT_FIELD
  #undef STRING
  #undef CONST_STRING
  #undef DEFAULT_STRING
  #undef PADDING

  std::string describe()
  {
    std::stringstream ss;
    #define FIELD(type, name) ss << "FIELD " << #type << " " << #name << " " \
                                 << name << std::endl;
    #define DEFAULT_FIELD(type, name, default) \
                ss << "DEFAULT_FIELD " << #type << #name << " " << name \
                << " (default: " << default << ")" << std::endl;
    #define STRING(length, name) ss << "STRING " << #name << " \"" \
                                    << name << "\"" << std::endl;
    #define CONST_STRING(name, value) \
                ss << "CONST_STRING " << #name << " \"" << name << "\"" \
                 << " (vaule: " << value << ")"<< std::endl;
    #define DEFAULT_STRING(length, name, default) \
                ss << "DEFAULT_STRING " << #name << " \"" << name << "\"" \
                 << " (default: " << default << ")"<< std::endl;
    #define PADDING(length) ss << "PADDING "<< length << std::endl;

    #include CODABLE_FILE

    return ss.str();
  }

  #undef FIELD
  #undef DEFAULT_FIELD
  #undef STRING
  #undef CONST_STRING
  #undef DEFAULT_STRING
  #undef PADDING

};

#undef CODABLE
#undef CODABLE_FILE
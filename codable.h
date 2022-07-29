#include <sstream>
#include <map>

#ifndef CODABLE_CLASS
#define CODABLE_CLASS

class Codable
{
public:
  virtual void decodeFrom(Bytestream& bts) = 0;
  virtual void encodeInto(Bytestream& bts) const = 0;
  virtual Bytestream encode() const = 0;
  virtual size_t encodedSize() const = 0;
  virtual std::string describe() const = 0;

};
#endif

#define CODABLE(name) \
class name : public Codable\
{ \
public:

  #define FIELD(type, name) type name;
  #define DEFAULT_FIELD(type, name, default) FIELD(type, name)
  #define STRING(length, name) std::string name;
  #define CONST_STRING(name, value) const std::string name = value;
  #define DEFAULT_STRING(length, name, default) STRING(length, name)
  #define ENUM_LABEL(label) label
  #define ENUM_VALUE(label, value) label = value
  #define ENUM(type, name, ...)\
  enum name##_enum : type \
  { __VA_ARGS__ } name;
  #define PADDING(length)

  #include CODABLE_FILE

  #undef FIELD
  #undef DEFAULT_FIELD
  #undef STRING
  #undef CONST_STRING
  #undef DEFAULT_STRING
  #undef CODABLE
  #undef ENUM
  #undef ENUM_LABEL
  #undef ENUM_VALUE

  #define CODABLE(name) \
  name() \
  {
    #define FIELD(type, name) name = type();
    #define DEFAULT_FIELD(type, name, default) name = default;
    #define STRING(length, name)
    #define CONST_STRING(name, value)
    #define DEFAULT_STRING(length, name, default) name = default;
    #define ENUM(type, name, ...) name = static_cast<name##_enum>(type());
    #include CODABLE_FILE
  }

  #undef FIELD
  #undef DEFAULT_FIELD
  #undef STRING
  #undef CONST_STRING
  #undef DEFAULT_STRING
  #undef CODABLE
  #undef ENUM

  #define CODABLE(name) \
  name(Bytestream& bts) \
  {
    #define FIELD(type, name)
    #define DEFAULT_FIELD(type, name, default)
    #define STRING(length, name)
    #define CONST_STRING(name, value)
    #define DEFAULT_STRING(length, name, default)
    #define ENUM(type, name, ...)
    #include CODABLE_FILE
    decodeFrom(bts);
  }

  #undef CODABLE
  #undef FIELD
  #undef DEFAULT_FIELD
  #undef STRING
  #undef CONST_STRING
  #undef DEFAULT_STRING
  #undef ENUM
  #undef PADDING

  #define CODABLE(name)

  void decodeFrom(Bytestream& bts)
  {
    #define FIELD(type, name) bts >> name;
    #define DEFAULT_FIELD(type, name, default) FIELD(type, name)
    #define STRING(length, name) bts/length >> name;
    #define CONST_STRING(name, value) bts >> name;
    #define DEFAULT_STRING(length, name, default) STRING(length, name)
    #define ENUM(type, name, ...) \
      { type tmp; bts >> tmp; name = static_cast<name##_enum>(tmp);};
    #define PADDING(length) bts += length;

    #include CODABLE_FILE
  }

  #undef FIELD
  #undef DEFAULT_FIELD
  #undef STRING
  #undef CONST_STRING
  #undef DEFAULT_STRING
  #undef ENUM
  #undef PADDING

  void encodeInto(Bytestream& bts) const
  {
    bts.preallocate(encodedSize());
    #define FIELD(type, name) bts << name;
    #define DEFAULT_FIELD(type, name, default) FIELD(type, name)
    #define STRING(length, name) {std::string __tmp(name); \
                                  __tmp.resize(length); bts << __tmp;}
    #define CONST_STRING(name, value) bts << name;
    #define DEFAULT_STRING(length, name, default) STRING(length, name)
    #define ENUM(type, name, ...) \
      { bts << static_cast<type>(name);};
    #define PADDING(length) {std::string tmp(length, 0); bts << tmp;}

    #include CODABLE_FILE
  }

  Bytestream encode() const
  {
    Bytestream bts;
    encodeInto(bts);
    return bts;
  }

  #undef FIELD
  #undef DEFAULT_FIELD
  #undef STRING
  #undef CONST_STRING
  #undef DEFAULT_STRING
  #undef ENUM
  #undef PADDING

  size_t encodedSize() const
  {
    size_t size = 0;
    #define FIELD(type, name) size += sizeof(type);
    #define DEFAULT_FIELD(type, name, default) size += sizeof(type);
    #define STRING(length, name) size += length;
    #define CONST_STRING(name, value) size += strlen(value);
    #define DEFAULT_STRING(length, name, default) size += length;
    #define ENUM(type, name, ...) size += sizeof(type);
    #define PADDING(length) size += length;
    #include CODABLE_FILE
    return size;
  }

  #undef FIELD
  #undef DEFAULT_FIELD
  #undef STRING
  #undef CONST_STRING
  #undef DEFAULT_STRING
  #undef ENUM
  #undef PADDING

  #define FIELD(type, name)
  #define DEFAULT_FIELD(type, name, default)
  #define STRING(length, name)
  #define CONST_STRING(name, value)
  #define DEFAULT_STRING(length, name, default)

  #define ENUM_LABEL(label) {label, #label}
  #define ENUM_VALUE(label, value) ENUM_LABEL(label)

  #define ENUM(type, name, ...) \
  static const std::string name##ToString(name##_enum value) \
  {static std::map<name##_enum, std::string> names \
    { __VA_ARGS__ }; \
    return names.find(value) != names.end() ? names.at(value) \
                                            : "Value out of range";}


  #define PADDING(length)

  #include CODABLE_FILE

  #undef FIELD
  #undef DEFAULT_FIELD
  #undef STRING
  #undef CONST_STRING
  #undef DEFAULT_STRING
  #undef ENUM
  #undef ENUM_LABEL
  #undef ENUM_VALUE
  #undef PADDING

  std::string describe() const
  {
    std::stringstream ss;
    #define FIELD(type, name) ss << "FIELD " << #type << " " << #name << " " \
                                 << +name << std::endl;
    #define DEFAULT_FIELD(type, name, default) \
                ss << "DEFAULT_FIELD " << #type << #name << " " << +name \
                << " (default: " << default << ")" << std::endl;
    #define STRING(length, name) ss << "STRING " << #name << " \"" \
                                    << name << "\"" << std::endl;
    #define CONST_STRING(name, value) \
                ss << "CONST_STRING " << #name << " \"" << name << "\"" \
                 << " (vaule: " << value << ")"<< std::endl;
    #define DEFAULT_STRING(length, name, default) \
                ss << "DEFAULT_STRING " << #name << " \"" << name << "\"" \
                 << " (default: " << default << ")"<< std::endl;
    #define ENUM(type, name, ...) \
                ss << "ENUM " << #type << " " << #name << " "  \
                   << name##ToString(name) \
                   << " (" << +static_cast<type>(name) << ")" << std::endl;
    #define PADDING(length) ss << "PADDING "<< length << std::endl;

    #include CODABLE_FILE

    return ss.str();
  }

  #undef FIELD
  #undef DEFAULT_FIELD
  #undef STRING
  #undef CONST_STRING
  #undef DEFAULT_STRING
  #undef ENUM
  #undef PADDING

};

#undef CODABLE
#undef CODABLE_FILE

#define CODABLE(name) \
class name \
{ \
public: \

  #define INT(type, name) type name;
  #define STRING(length, name) std::string name;
  #define SKIP(length)

  #include CODABLE_FILE

  #undef CODABLE
  #define CODABLE(name)
  #undef INT
  #undef STRING
  #undef SKIP

  void decode(Bytestream& bts)
  {
    #define INT(type, name) bts >> name;
    #define STRING(length, name) bts/length >> name;
    #define SKIP(length) bts += length;

    #include CODABLE_FILE
  }

  #undef INT
  #undef STRING
  #undef SKIP

  void encode_into(Bytestream& bts)
  {
    #define INT(type, name) bts << name;
    #define STRING(length, name) name.resize(length); bts << name;
    #define SKIP(length) std::string tmp(length, 0); bts << tmp;

    #include CODABLE_FILE
  }

  Bytestream encode()
  {
    Bytestream bts;
    encode_into(bts);
    return bts;
  }

};

#undef CODABLE_FILE

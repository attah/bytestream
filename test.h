#include <iostream>
#include <dlfcn.h>

#define R(s) "\e[31m" s "\e[39m"
#define G(s) "\e[32m" s "\e[39m"
#define Y(s) "\e[33m" s "\e[39m"
#define POS0 "\e[1F" // Of previous line

#define TEST(name) TEST_(name, __COUNTER__)
#define TEST_(name, counter) TEST__(name, #name, counter)
#define TEST__(name, namestr, counter) \
  void _test_##name();\
  extern "C" void __test_##counter()\
    {std::cout << Y(" 🡆 ") << namestr << std::endl;\
     try {_test_##name();\
          std::cout << POS0 << G(" ✔ ") << namestr << std::endl;}\
     catch(const std::exception& e) {\
          std::cout << POS0 << R(" ✘ ") << namestr \
                    << " (" << e.what() << ")" << std::endl;}}\
  void _test_##name()

#define ASSERT(expr) \
  if(!(expr)){throw TestFailedException("Assertion failed", \
                                        __FILE__, __LINE__);}
#define ASSERT_FALSE(expr) \
  if((expr)){throw TestFailedException("Assertion failed", \
                                        __FILE__, __LINE__);}


typedef void (*void_f)(void);

class TestFailedException: public std::exception
{
private:
  std::string _msg;
public:
  TestFailedException(std::string msg, std::string file, int line)
  {
    _msg = msg+" at "+file+":"+std::to_string(line);
  }
  virtual const char* what() const throw()
  {
    return _msg.c_str();
  }
};

int main(int argc, char** argv)
{
  void* handle = dlopen(NULL, RTLD_LAZY);

  int i = 0;
  while(true)
  {
    std::string test = "__test_"+std::to_string(i++);
    void* fptr = dlsym(handle, test.c_str());
    if(!fptr)
      break;
    ((void_f)fptr)();
  }
  dlclose(handle);
}

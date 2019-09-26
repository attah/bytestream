#include <iostream>
#include <dlfcn.h>
#include <cxxabi.h>

#define R(s) "\033[31m" s "\033[39m"
#define G(s) "\033[32m" s "\033[39m"
#define Y(s) "\033[33m" s "\033[39m"
#define POS0 "\033[1F" // Of previous line

#define TEST(name) TEST_(name, __COUNTER__)
#define TEST_(name, counter) TEST__(name, #name, counter)
#define TEST__(name, namestr, counter) \
  void _test_##name();\
  extern "C" void __test_##counter()\
    {std::string errmsg;\
     std::cout << Y(" 🡆 ") << namestr << std::endl;\
     try {_test_##name();\
          std::cout << POS0 << G(" ✔ ") << namestr << std::endl;}\
     catch(TestFailedException e) {\
          errmsg = string(" (") + e.what() + ")";}\
     catch(const std::exception& e) {\
          errmsg = string(" (Ecxeption caught: ") + currentExceptionName()\
          +": \""+e.what()+"\")";}\
     if(!errmsg.empty()){\
          std::cout << POS0 << R(" ✘ ") << namestr << errmsg << std::endl;}}\
  void _test_##name()

#define ASSERT(expr) \
  if(!(expr)){throw TestFailedException("Assertion failed", \
                                        __FILE__, __LINE__);}
#define ASSERT_FALSE(expr) \
  if((expr)){throw TestFailedException("Assertion failed", \
                                        __FILE__, __LINE__);}

#define ASSERT_THROW(expr, exc) \
  try {expr; \
       throw TestFailedException("Assertion failed, exception was not thrown", \
                                 __FILE__, __LINE__);} \
  catch(exc e) {}

typedef void (*void_f)(void);

const char* currentExceptionName()
{
    int status;
    return abi::__cxa_demangle(abi::__cxa_current_exception_type()->name(),
                               0, 0, &status);
}


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

int main(__attribute__((unused)) int argc, __attribute__((unused)) char** argv)
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

#include <iostream>
#include <dlfcn.h>
#include <cxxabi.h>

class DebugStream
{
  public:
    DebugStream() : used(false) {}
    bool used;
    template <typename T>
    DebugStream& operator<<(const T &data)
    {
      std::cout << data;
      used=true;
      return *this;
    }
};

#define R(s) "\033[31m" s "\033[39m"
#define G(s) "\033[32m" s "\033[39m"
#define Y(s) "\033[33m" s "\033[39m"
#define POS0 "\033[1F" // Pos0 of previous line
#define RESULTPOS (debug.used ? "\n" : POS0)

#define TEST(name) TEST_(name, __COUNTER__)
#define TEST_(name, counter) TEST__(name, #name, counter)
#define TEST__(name, namestr, counter) \
  extern "C" std::string __test_##counter##_name()\
  {return namestr;}\
  void _test_##name(DebugStream& debug);\
  extern "C" void __test_##counter(DebugStream& debug)\
    {_test_##name(debug);}\
  void _test_##name(__attribute__((unused)) DebugStream& debug)

#define STR(s) STR_(#s)
#define STR_(s) s

#define ASSERT(expr)\
  if(!(expr)){throw TestFailedException(__FILE__, __LINE__, "Assertion failed",\
                                        STR(expr));}
#define ASSERT_FALSE(expr) \
  if((expr)){throw TestFailedException(__FILE__, __LINE__, "Assertion failed",\
                                       STR(expr));}

#define ASSERT_THROW(expr, exc) \
  try {expr; \
       throw TestFailedException(__FILE__, __LINE__,\
                                 "Assertion failed, exception was not thrown", \
                                 STR(expr));} \
  catch(exc e) {}

typedef std::string (*name_f)(void);
typedef void (*test_f)(DebugStream& debug);

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
  TestFailedException(std::string file,
                      int line,
                      std::string msg,
                      std::string extra = "")
  {
    _msg = file+":"+std::to_string(line)+": "+msg;
    if(!extra.empty())
    {
      _msg=_msg+": "+extra;
    }
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
    std::string testname = "__test_"+std::to_string(i)+"_name";
    std::string testfun = "__test_"+std::to_string(i);
    void* name_fptr = dlsym(handle, testname.c_str());
    void* test_fptr = dlsym(handle, testfun.c_str());
    if(!name_fptr || !test_fptr)
      break;
    std::string namestr = ((name_f)name_fptr)();
    std::string errmsg;
    std::cout << Y(" 🡆 ") << namestr << std::endl;
    DebugStream debug;
    std::string resultpos = POS0;
    try
    {
      ((test_f)test_fptr)(debug);
      std::cout << RESULTPOS << G(" ✔ ") << namestr << std::endl;
    }
    catch(TestFailedException e)
    {
      errmsg = std::string(" (") + e.what() + ")";
    }
    catch(const std::exception& e)
    {
      errmsg = std::string(" (Ecxeption caught: ") + currentExceptionName()
             + ": \""+e.what()+"\")";
    }
    if(!errmsg.empty())
    {
      std::cout << RESULTPOS << R(" ✘ ") << namestr << errmsg << std::endl;
    }
    i++;
  }
  dlclose(handle);
}

#ifndef ARRAY_H
#define ARRAY_H

#include <memory>

template<typename T>
class Array : public std::unique_ptr<T, std::default_delete<T[]>>
{
public:
  Array(size_t size = 0)
  : std::unique_ptr<T, std::default_delete<T[]>>(size == 0 ? nullptr : new T[size])
  {}
  operator T*() const
  {
    return std::unique_ptr<T, std::default_delete<T[]>>::get();
  }
};

#endif //ARRAY_H

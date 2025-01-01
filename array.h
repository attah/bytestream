#ifndef ARRAY_H
#define ARRAY_H

#include <memory>

// NOLINTBEGIN(*-avoid-c-arrays)
template<typename T>
class Array : public std::unique_ptr<T[]>
{
public:
  Array(size_t size = 0) : std::unique_ptr<T[]>(size == 0 ? nullptr : new T[size])
  {}
  operator T*() const
  {
    return std::unique_ptr<T[]>::get();
  }
};

// NOLINTEND(*-avoid-c-arrays)
#endif //ARRAY_H

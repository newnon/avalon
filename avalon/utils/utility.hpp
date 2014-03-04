#ifndef AVALON_UTILS_UTILITY_H
#define AVALON_UTILS_UTILITY_H

namespace avalon {

class noncopyable
  {
   protected:
    constexpr noncopyable() = default;
    ~noncopyable() = default;
        noncopyable( const noncopyable& ) = delete;
        noncopyable& operator=( const noncopyable& ) = delete;
  };
  
}

#endif //AVALON_UTILS_UTILITY_H
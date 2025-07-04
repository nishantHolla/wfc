#ifndef WFC_LOG_H_
#define WFC_LOG_H_

#include <iostream>

namespace wfc {

class Log {
public:
  template<typename T>
  static inline void error(const T& message) {
    std::cerr << "[ERRR]: " << message << "\n";
  }

  template<typename T>
  static inline void info(const T& message) {
    std::clog << "[INFO]: " << message << "\n";
  }

  template<typename T>
  static inline void warn(const T& message) {
    std::clog << "[WARN]: " << message << "\n";
  }
};

}

#endif // !WFC_LOG_H_

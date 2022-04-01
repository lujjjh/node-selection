#ifndef SELECTION_H
#define SELECTION_H

#include <node.h>
#include <string>
#include <tuple>

namespace selection {
class RuntimeException : public std::exception {
  const std::string msg;

public:
  RuntimeException(const std::string &msg) : msg(msg) {}

  virtual const char *what() const throw() { return msg.c_str(); }
};
} // namespace selection

namespace selection_impl {
void Initialize();
bool CheckAccessibilityPermissions(bool prompt);
const std::string GetSelection();
} // namespace selection_impl

#endif

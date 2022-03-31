#ifndef SELECTION_H
#define SELECTION_H

#include <node.h>

namespace selection {
class RuntimeException : public std::exception {
  const std::string msg;

public:
  RuntimeException(const std::string &msg) : msg(msg) {}

  virtual const char *what() const throw() { return msg.c_str(); }
};
} // namespace selection

namespace selection_impl {
bool CheckAccessibilityPermissions(bool prompt);
const std::string GetSelection();
} // namespace selection_impl

#endif

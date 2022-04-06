#ifndef SELECTION_H
#define SELECTION_H

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

#define NAPI_BOOL(name, val)                                                                                           \
  bool name;                                                                                                           \
  if (napi_get_value_bool(env, val, &name) != napi_ok) {                                                               \
    napi_throw_error(env, "EINVAL", "Expected boolean");                                                               \
    return NULL;                                                                                                       \
  }

#define NAPI_ARGV_BOOL(name, i) NAPI_BOOL(name, argv[i])

#endif

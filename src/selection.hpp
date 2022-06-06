#ifndef SELECTION_H
#define SELECTION_H

#include <optional>
#include <string>
#include <tuple>

#ifdef _WIN32
#define pid_t int
#endif

namespace selection {
class RuntimeException : public std::exception {
  const std::string msg;

public:
  RuntimeException(const std::string &msg) : msg(msg) {}

  virtual const char *what() const throw() { return msg.c_str(); }
};
} // namespace selection

namespace selection_impl {
struct ProcessInfo {
  pid_t pid;
  std::optional<std::string> name;
  std::optional<std::string> bundleIdentifier; // macOS only
};

struct Selection {
  std::string text;
  std::optional<ProcessInfo> process;
};

void Initialize();
bool CheckAccessibilityPermissions(bool prompt);
Selection GetSelection();
} // namespace selection_impl

#define NAPI_BOOL(name, val)                                                                                           \
  bool name;                                                                                                           \
  if (napi_get_value_bool(env, val, &name) != napi_ok) {                                                               \
    napi_throw_error(env, "EINVAL", "Expected boolean");                                                               \
    return NULL;                                                                                                       \
  }

#define NAPI_ARGV_BOOL(name, i) NAPI_BOOL(name, argv[i])

#endif

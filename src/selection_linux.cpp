#include "./selection.hpp"

namespace selection_impl {
using selection::RuntimeException;

void Initialize() {}

bool CheckAccessibilityPermissions(bool prompt) {
  std::ignore = prompt;
  return true;
}

Selection GetSelection() { throw RuntimeException("unimplemented"); }

} // namespace selection_impl

#include "./selection.hpp"

namespace selection_impl {
using selection::RuntimeException;

void Initialize() {}

bool CheckAccessibilityPermissions(bool prompt) {
  std::ignore = prompt;
  return true;
}

const Selection GetSelection() { throw RuntimeException("unimplemented"); }

} // namespace selection_impl

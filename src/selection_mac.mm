#include <AppKit/AppKit.h>
#include <ApplicationServices/ApplicationServices.h>
#include <Foundation/Foundation.h>

#include "./selection.hpp"

namespace selection_impl {
using selection::RuntimeException;

void Initialize() {}

bool CheckAccessibilityPermissions(bool prompt) {
  NSDictionary *options = @{(id)kAXTrustedCheckOptionPrompt : @(prompt)};
  return AXIsProcessTrustedWithOptions((CFDictionaryRef)options);
}

AXUIElementRef CreateSystemWideElement() {
  AXUIElementRef systemWideElement = AXUIElementCreateSystemWide();
  AXUIElementRef _;
  if (AXUIElementCopyElementAtPosition(systemWideElement, 0, 0, &_) == kAXErrorSuccess) {
    CFRelease(_);
  }
  return systemWideElement;
}

void TouchDescendantElements(AXUIElementRef element) {
  if (!element) {
    return;
  }
  CFMutableArrayRef children;
  auto error = AXUIElementCopyAttributeValue(element, kAXChildrenAttribute, (CFTypeRef *)&children);
  if (error != kAXErrorSuccess) {
    return;
  }
  for (CFIndex i = 0; i < CFArrayGetCount(children); i++) {
    TouchDescendantElements((AXUIElementRef)CFArrayGetValueAtIndex(children, i));
  }
  CFRelease(children);
}

AXUIElementRef _GetFocusedElement() {
  static auto systemWideElement = CreateSystemWideElement();
  AXUIElementRef focusedElement;
  auto error =
      AXUIElementCopyAttributeValue(systemWideElement, kAXFocusedUIElementAttribute, (CFTypeRef *)&focusedElement);
  if (error != kAXErrorSuccess) {
    return nil;
  }
  return focusedElement;
}

AXUIElementRef GetFocusedElement() {
  auto focusedElement = _GetFocusedElement();
  // Touch all descendant elements to enable accessibility in apps like Word and Excel.
  if (focusedElement) {
    TouchDescendantElements(focusedElement);
    CFRelease(focusedElement);
    focusedElement = _GetFocusedElement();
  }
  return focusedElement;
}

const std::string GetSelection() {
  auto focusedElement = GetFocusedElement();
  if (!focusedElement) {
    throw RuntimeException("failed to get focused element");
  }

  NSString *selection;
  auto error = AXUIElementCopyAttributeValue(focusedElement, kAXSelectedTextAttribute, (CFTypeRef *)&selection);
  CFRelease(focusedElement);
  if (error != kAXErrorSuccess) {
    throw RuntimeException("failed to get selected text");
  }

  return std::string([selection UTF8String], [selection lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
}

} // namespace selection

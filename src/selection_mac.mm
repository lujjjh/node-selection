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

void TouchDescendantElements(AXUIElementRef element, int maxDepth) {
  if (!element) {
    return;
  }
  if (maxDepth <= 0) {
    return;
  }
  CFArrayRef children;
  auto error = AXUIElementCopyAttributeValue(element, kAXChildrenAttribute, (CFTypeRef *)&children);
  if (error != kAXErrorSuccess) {
    return;
  }
  for (CFIndex i = 0; i < std::min(CFArrayGetCount(children), (CFIndex)8); i++) {
    TouchDescendantElements((AXUIElementRef)CFArrayGetValueAtIndex(children, i), maxDepth - 1);
  }
  CFRelease(children);
}

AXUIElementRef _GetFocusedElement() {
  // TODO: activeApplication is deprecated, should use frontmostApplication instead;
  // however, the latter requires a main event loop to be running, which would block
  // Node's event loop.
  auto frontmostApplication = [[NSWorkspace sharedWorkspace] activeApplication];
  if (!frontmostApplication) {
    return nil;
  }
  pid_t pid = [frontmostApplication[@"NSApplicationProcessIdentifier"] intValue];
  auto application = AXUIElementCreateApplication(pid);
  if (!application) {
    return nil;
  }
  AXUIElementSetAttributeValue(application, CFSTR("AXManualAccessibility"), kCFBooleanTrue);
  AXUIElementSetAttributeValue(application, CFSTR("AXEnhancedUserInterface"), kCFBooleanTrue);
  AXUIElementRef focusedElement;
  auto error = AXUIElementCopyAttributeValue(application, kAXFocusedUIElementAttribute, (CFTypeRef *)&focusedElement);
  if (error != kAXErrorSuccess) {
    error = AXUIElementCopyAttributeValue(application, kAXFocusedWindowAttribute, (CFTypeRef *)&focusedElement);
  }
  CFRelease(application);
  if (error != kAXErrorSuccess) {
    return nil;
  }
  return focusedElement;
}

AXUIElementRef GetFocusedElement() {
  auto focusedElement = _GetFocusedElement();
  // Touch all descendant elements to enable accessibility in apps like Word and Excel.
  if (focusedElement) {
    TouchDescendantElements(focusedElement, 8);
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
  if (error != kAXErrorSuccess) {
    // Handle WebKit-like elements.
    CFTypeRef range;
    error = AXUIElementCopyAttributeValue(focusedElement, CFSTR("AXSelectedTextMarkerRange"), &range);
    if (error == kAXErrorSuccess) {
      error = AXUIElementCopyParameterizedAttributeValue(focusedElement, CFSTR("AXStringForTextMarkerRange"), range,
                                                         (CFTypeRef *)&selection);
      CFRelease(range);
    }
  }
  CFRelease(focusedElement);
  if (error != kAXErrorSuccess) {
    throw RuntimeException("failed to get selected text");
  }

  return std::string([selection UTF8String], [selection lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
}

} // namespace selection

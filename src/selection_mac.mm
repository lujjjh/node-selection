#include <AppKit/AppKit.h>
#include <ApplicationServices/ApplicationServices.h>
#include <Foundation/Foundation.h>

#include "./selection.hpp"

namespace selection_impl {
using selection::RuntimeException;

static NSSet *appsManuallyEnableAx = [[NSSet alloc] initWithArray:@[ @"com.google.Chrome", @"org.mozilla.firefox" ]];

void Initialize() {}

bool CheckAccessibilityPermissions(bool prompt) {
  NSDictionary *options = @{(id)kAXTrustedCheckOptionPrompt : @(prompt)};
  return AXIsProcessTrustedWithOptions((CFDictionaryRef)options);
}

std::string ToString(NSString *string) {
  return std::string([string UTF8String], [string lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
}

pid_t GetFrontProcessID() {
  // TODO: activeApplication is deprecated, should use frontmostApplication instead;
  // however, the latter requires a main event loop to be running, which would block
  // Node's event loop.
  auto frontmostApplication = [[NSWorkspace sharedWorkspace] activeApplication];
  if (!frontmostApplication) {
    return 0;
  }
  return [frontmostApplication[@"NSApplicationProcessIdentifier"] intValue];
}

std::optional<std::string> GetProcessName(pid_t pid) {
  auto application = [NSRunningApplication runningApplicationWithProcessIdentifier:pid];
  if (!application) {
    return std::nullopt;
  }
  auto url = [application executableURL];
  if (!url) {
    return std::nullopt;
  }
  auto name = [url lastPathComponent];
  return ToString(name);
}

std::optional<std::string> GetBundleIdentifier(pid_t pid) {
  auto application = [NSRunningApplication runningApplicationWithProcessIdentifier:pid];
  if (!application) {
    return std::nullopt;
  }
  auto bundle_identifier = [application bundleIdentifier];
  return ToString(bundle_identifier);
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

AXUIElementRef _GetFocusedElement(pid_t pid) {
  auto application = AXUIElementCreateApplication(pid);
  if (!application) {
    return nil;
  }

  auto bundleIdentifierOptional = GetBundleIdentifier(pid);
  if (!bundleIdentifierOptional) {
    return nil;
  }
  NSString *bundlerIdentifer = [NSString stringWithUTF8String:bundleIdentifierOptional->c_str()];
  if ([appsManuallyEnableAx containsObject:bundlerIdentifer]) {
    AXUIElementSetAttributeValue(application, CFSTR("AXManualAccessibility"), kCFBooleanTrue);
    AXUIElementSetAttributeValue(application, CFSTR("AXEnhancedUserInterface"), kCFBooleanTrue);
  }

  AXUIElementRef focusedElement;
  auto error = AXUIElementCopyAttributeValue(application, kAXFocusedUIElementAttribute, (CFTypeRef *)&focusedElement);
  if (error != kAXErrorSuccess) {
    error = AXUIElementCopyAttributeValue(application, kAXFocusedWindowAttribute, (CFTypeRef *)&focusedElement);
  }
  if (error != kAXErrorSuccess) {
    return nil;
  }

  return focusedElement;
}

AXUIElementRef GetFocusedElement(pid_t pid) {
  auto focusedElement = _GetFocusedElement(pid);
  // Touch all descendant elements to enable accessibility in apps like Word and Excel.
  if (focusedElement) {
    TouchDescendantElements(focusedElement, 8);
    CFRelease(focusedElement);
    focusedElement = _GetFocusedElement(pid);
  }
  return focusedElement;
}

std::string GetSelectionText(pid_t pid) {
  auto focusedElement = GetFocusedElement(pid);
  if (!focusedElement) {
    throw RuntimeException("no focused element");
  }

  NSString *text;
  auto error = AXUIElementCopyAttributeValue(focusedElement, kAXSelectedTextAttribute, (CFTypeRef *)&text);
  if (error != kAXErrorSuccess) {
    // Handle WebKit-like elements.
    CFTypeRef range;
    error = AXUIElementCopyAttributeValue(focusedElement, CFSTR("AXSelectedTextMarkerRange"), &range);
    if (error == kAXErrorSuccess) {
      error = AXUIElementCopyParameterizedAttributeValue(focusedElement, CFSTR("AXStringForTextMarkerRange"), range,
                                                         (CFTypeRef *)&text);
      CFRelease(range);
    }
  }
  CFRelease(focusedElement);
  if (error != kAXErrorSuccess) {
    throw RuntimeException("no valid selection");
  }

  auto result = ToString(text);
  CFRelease(text);
  return result;
}

Selection GetSelection() {
  auto pid = GetFrontProcessID();
  if (!pid) {
    throw RuntimeException("no front process");
  }

  return Selection{
      .text = GetSelectionText(pid),
      .process = ProcessInfo{.pid = pid, .name = GetProcessName(pid), .bundleIdentifier = GetBundleIdentifier(pid)}};
}

} // namespace selection

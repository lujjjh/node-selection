#include <AppKit/AppKit.h>
#include <ApplicationServices/ApplicationServices.h>
#include <Foundation/Foundation.h>
#include <nan.h>
#include <node.h>

#include "./selection.hpp"

namespace selection_impl {
using Nan::AsyncWorker;
using Nan::AsyncQueueWorker;
using Nan::Callback;
using selection::RuntimeException;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::Boolean;
using v8::String;
using v8::Value;

bool CheckAccessibilityPermissions(bool prompt) {
  CFRunLoopRunInMode(kCFRunLoopDefaultMode, 5, NO);
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

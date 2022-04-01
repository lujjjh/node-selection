#include <nan.h>
#include <node.h>

#include "selection.hpp"

namespace selection {
using Nan::AsyncQueueWorker;
using Nan::AsyncWorker;
using Nan::Callback;
using v8::Boolean;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::Value;

class CheckAccessibilityPermissionsAsyncWorker : public AsyncWorker {
  bool prompt;
  bool result;

public:
  CheckAccessibilityPermissionsAsyncWorker(Callback *callback, bool prompt) : AsyncWorker(callback), prompt(prompt) {}

  virtual void Execute() {
    try {
      result = selection_impl::CheckAccessibilityPermissions(prompt);
    } catch (const RuntimeException &e) {
      SetErrorMessage(e.what());
    }
  }

protected:
  virtual void HandleOKCallback() {
    Nan::HandleScope scope;
    v8::Local<v8::Value> argv[] = {Nan::Null(), Nan::New(result)};
    Nan::Call(callback->GetFunction(), Nan::GetCurrentContext()->Global(), 2, argv);
  }
};

class GetSelectionAsyncWorker : public AsyncWorker {
  std::string result;

public:
  GetSelectionAsyncWorker(Callback *callback) : AsyncWorker(callback) {}

  virtual void Execute() {
    try {
      result = selection_impl::GetSelection();
    } catch (const RuntimeException &e) {
      SetErrorMessage(e.what());
    }
  }

protected:
  virtual void HandleOKCallback() {
    Nan::HandleScope scope;
    v8::Local<v8::Value> argv[] = {Nan::Null(), Nan::New(result).ToLocalChecked()};
    Nan::Call(callback->GetFunction(), Nan::GetCurrentContext()->Global(), 2, argv);
  }
};

NAN_METHOD(CheckAccessibilityPermissions) {
  if (info.Length() != 2) {
    return Nan::ThrowTypeError("expected 2 arguments");
  }

  if (!info[0]->IsBoolean()) {
    return Nan::ThrowTypeError("prompt must be a boolean");
  }

  if (!info[1]->IsFunction()) {
    return Nan::ThrowTypeError("callback must be a function");
  }

  auto prompt = Nan::To<bool>(info[0]).FromJust();
  auto callback = new Callback(info[1].As<Function>());

  AsyncQueueWorker(new CheckAccessibilityPermissionsAsyncWorker(callback, prompt));
}

NAN_METHOD(GetSelection) {
  if (info.Length() != 1) {
    return Nan::ThrowTypeError("expected 1 arguments");
  }

  if (!info[0]->IsFunction()) {
    return Nan::ThrowTypeError("callback must be a function");
  }

  auto callback = new Callback(info[0].As<Function>());

  AsyncQueueWorker(new GetSelectionAsyncWorker(callback));
}

void Initialize(Local<Object> exports) {
  selection_impl::Initialize();
  Nan::SetMethod(exports, "checkAccessibilityPermissions", CheckAccessibilityPermissions);
  Nan::SetMethod(exports, "getSelection", GetSelection);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize)
} // namespace selection

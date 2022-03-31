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

void CheckAccessibilityPermissions(const FunctionCallbackInfo<Value> &args) {
  auto isolate = Isolate::GetCurrent();
  auto prompt = args[0]->BooleanValue(isolate);
  auto callback = new Callback(args[1].As<Function>());
  AsyncQueueWorker(new CheckAccessibilityPermissionsAsyncWorker(callback, prompt));
}

void GetSelection(const FunctionCallbackInfo<Value> &args) {
  auto isolate = Isolate::GetCurrent();
  auto callback = new Callback(args[0].As<Function>());
  AsyncQueueWorker(new GetSelectionAsyncWorker(callback));
}

void Initialize(Local<Object> exports) {
  NODE_SET_METHOD(exports, "checkAccessibilityPermissions", CheckAccessibilityPermissions);
  NODE_SET_METHOD(exports, "getSelection", GetSelection);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize)
} // namespace selection

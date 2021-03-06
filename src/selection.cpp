#define NAPI_VERSION 3
#include <napi.h>
#include <node_api.h>

#include "selection.hpp"

namespace selection {
using Napi::AsyncWorker;
using Napi::Boolean;
using Napi::Error;
using Napi::Function;
using Napi::HandleScope;
using Napi::Promise;
using Napi::String;
using Napi::TypeError;

class CheckAccessibilityPermissionsAsyncWorker : public AsyncWorker {
  bool prompt;
  bool result;
  Promise::Deferred deferred;

public:
  CheckAccessibilityPermissionsAsyncWorker(const Napi::CallbackInfo &info, bool prompt)
      : AsyncWorker(info.Env()), prompt(prompt), deferred(Promise::Deferred::New(info.Env())) {}

  Promise GetPromise() { return deferred.Promise(); }

  void Execute() override {
    try {
      result = selection_impl::CheckAccessibilityPermissions(prompt);
    } catch (const RuntimeException &e) {
      SetError(e.what());
    }
  }

  void OnOK() override { deferred.Resolve(Boolean::New(Env(), result)); }

  void OnError(const Error &e) override { deferred.Reject(e.Value()); }
};

class GetSelectionAsyncWorker : public AsyncWorker {
  selection_impl::Selection result;
  Promise::Deferred deferred;

public:
  GetSelectionAsyncWorker(const Napi::CallbackInfo &info)
      : AsyncWorker(info.Env()), deferred(Promise::Deferred::New(info.Env())) {}

  Promise GetPromise() { return deferred.Promise(); }

  void Execute() override {
    try {
      result = selection_impl::GetSelection();
    } catch (const RuntimeException &e) {
      SetError(e.what());
    }
  }

  void OnOK() override {
    Napi::Object selection = Napi::Object::New(Env());
    selection.Set("text", String::New(Env(), result.text));
    if (result.process) {
      auto process = Napi::Object::New(Env());
      process.Set("pid", Napi::Number::New(Env(), result.process->pid));
      if (result.process->name.has_value()) {
        process.Set("name", String::New(Env(), *result.process->name));
      }
      if (result.process->bundleIdentifier.has_value()) {
        process.Set("bundleIdentifier", String::New(Env(), *result.process->bundleIdentifier));
      }
      selection.Set("process", process);
    } else {
      selection.Set("process", Env().Null());
    }
    deferred.Resolve(selection);
  }

  void OnError(const Error &e) override { deferred.Reject(e.Value()); }
};

Napi::Value CheckAccessibilityPermissions(const Napi::CallbackInfo &info) {
  auto env = info.Env();

  if (info.Length() != 1) {
    TypeError::New(env, "expected 1 arguments").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  if (!info[0].IsBoolean()) {
    TypeError::New(env, "prompt must be a boolean").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  auto prompt = info[0].As<Boolean>().Value();

  auto worker = new CheckAccessibilityPermissionsAsyncWorker(info, prompt);
  worker->Queue();
  return worker->GetPromise();
}

Napi::Value GetSelection(const Napi::CallbackInfo &info) {
  auto worker = new GetSelectionAsyncWorker(info);
  worker->Queue();
  return worker->GetPromise();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  selection_impl::Initialize();
  exports.Set(String::New(env, "checkAccessibilityPermissions"),
              Napi::Function::New(env, CheckAccessibilityPermissions));
  exports.Set(String::New(env, "getSelection"), Napi::Function::New(env, GetSelection));
  return exports;
}

NODE_API_MODULE(selection, Init)

} // namespace selection

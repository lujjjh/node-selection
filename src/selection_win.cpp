
#include <UIAutomation.h>
#include <atlbase.h>
#include <comutil.h>
#include <psapi.h>
#include <vector>
#include <windows.h>
#pragma comment(lib, "comsuppw.lib")
#pragma comment(lib, "psapi.lib")

#include "./selection.hpp"

namespace selection_impl {
using selection::RuntimeException;

void Initialize() { CoInitializeEx(0, COINIT_MULTITHREADED); }

bool CheckAccessibilityPermissions(bool prompt) {
  std::ignore = prompt;
  return true;
}

std::string BSTRtoUTF8(BSTR bstr) {
  int len = SysStringLen(bstr);
  if (len == 0)
    return "";
  int size_needed = WideCharToMultiByte(CP_UTF8, 0, bstr, len, NULL, 0, NULL, NULL);
  std::string ret(size_needed, '\0');
  WideCharToMultiByte(CP_UTF8, 0, bstr, len, &ret.front(), ret.size(), NULL, NULL);
  return ret;
}

std::string PTCHARtoUTF8(TCHAR *ptchar) {
#ifndef UNICODE
  return std::string(ptchar);
#else
  int size_needed = WideCharToMultiByte(CP_UTF8, 0, ptchar, -1, NULL, 0, NULL, NULL);
  if (size_needed <= 0) {
    return "";
  }
  std::vector<char> buffer(size_needed);
  WideCharToMultiByte(CP_UTF8, 0, ptchar, -1, buffer.data(), buffer.size(), NULL, NULL);
  return std::string(buffer.data());
#endif
}

void _OutputElementName(IUIAutomationElement *element) {
  CComBSTR name;
  if (element->get_CurrentName(&name) == S_OK) {
    auto s = BSTRtoUTF8(name);
    printf("name: %s\n", s.data());
  }
}

CComPtr<IUIAutomation> CreateUIAutomation() {
  CComPtr<IUIAutomation> automation;
  if (CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER, IID_IUIAutomation,
                       reinterpret_cast<void **>(&automation)) != S_OK) {
    return nullptr;
  }
  return automation;
}

Selection GetSelection() {
  static CComPtr<IUIAutomation> automation = CreateUIAutomation();
  if (!automation) {
    throw RuntimeException("failed to create UIAutomation");
  }

  CComPtr<IUIAutomationTreeWalker> treeWalker;
  if (automation->get_RawViewWalker(&treeWalker) != S_OK || !treeWalker) {
    throw RuntimeException("failed to get tree walker");
  }

  CComPtr<IUIAutomationElement> focusedElement;
  if (automation->GetFocusedElement(&focusedElement) != S_OK || !focusedElement) {
    throw RuntimeException("no focused element");
  }
  for (; focusedElement;
       treeWalker->GetParentElement(focusedElement, &focusedElement) != S_OK && (focusedElement = nullptr)) {
    CComPtr<IUIAutomationTextPattern> textPattern;
    if (focusedElement->GetCurrentPatternAs(UIA_TextPatternId, IID_IUIAutomationTextPattern,
                                            reinterpret_cast<void **>(&textPattern)) != S_OK ||
        !textPattern) {
      CComPtr<IUIAutomationTextChildPattern> textChildPattern;
      if (focusedElement->GetCurrentPatternAs(UIA_TextChildPatternId, IID_IUIAutomationTextChildPattern,
                                              reinterpret_cast<void **>(&textChildPattern)) != S_OK ||
          !textChildPattern) {
        continue;
      }

      CComPtr<IUIAutomationElement> containerElement;
      if (textChildPattern->get_TextContainer(&containerElement) != S_OK || !containerElement) {
        throw RuntimeException("failed to get container element");
      }

      if (containerElement->GetCurrentPatternAs(UIA_TextPatternId, IID_IUIAutomationTextPattern,
                                                reinterpret_cast<void **>(&textPattern)) != S_OK ||
          !textPattern) {
        throw RuntimeException("failed to get text pattern");
      }
    }

    std::optional<ProcessInfo> process;
    {
      pid_t pid;
      if (focusedElement->get_CurrentProcessId(&pid) == S_OK) {
        process = ProcessInfo{pid};
        auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
        if (hProcess) {
          TCHAR buffer[MAX_PATH];
          if (GetModuleFileNameEx(hProcess, NULL, buffer, MAX_PATH)) {
            PathStripPath(buffer);
            auto name = PTCHARtoUTF8(buffer);
            process->name = name;
          }
          CloseHandle(hProcess);
        }
      }
    }

    CComPtr<IUIAutomationTextRangeArray> textRanges;
    if (textPattern->GetSelection(&textRanges) != S_OK || !textRanges) {
      throw RuntimeException("failed to get text ranges");
    }
    int length;
    if (textRanges->get_Length(&length) != S_OK) {
      throw RuntimeException("failed to get text ranges' length");
    }
    for (int i = 0; i < length; i++) {
      CComPtr<IUIAutomationTextRange> textRange;
      if (textRanges->GetElement(i, &textRange) != S_OK) {
        throw RuntimeException("failed to get text range");
      }
      CComBSTR text;
      if (textRange->GetText(256, &text) != S_OK) {
        throw RuntimeException("failed to get text");
      }
      return {BSTRtoUTF8(text), process};
    }
  }

  throw RuntimeException("no valid selection");
}

} // namespace selection_impl

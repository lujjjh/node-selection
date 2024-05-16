use std::ffi::c_void;

use accessibility_sys::{kAXTrustedCheckOptionPrompt, AXIsProcessTrustedWithOptions};
use core_foundation::{
  base::{FromVoid, ToVoid},
  dictionary::CFMutableDictionary,
  number::CFNumber,
  string::CFString,
};

use crate::shared::node_selection::{NodeSelection, NodeSelectionTrait};

impl NodeSelectionTrait for NodeSelection {
  fn check_accessibility_permissions(&self, prompt: bool) -> bool {
    unsafe {
      let mut options_dict: CFMutableDictionary<CFString, CFNumber> = CFMutableDictionary::new();

      options_dict.add(
        &CFString::from_void(kAXTrustedCheckOptionPrompt as *const c_void),
        &if prompt { 1i64 } else { 0i64 }.into(),
      );

      AXIsProcessTrustedWithOptions(options_dict.into_untyped().to_void() as *const _)
    }
  }
}

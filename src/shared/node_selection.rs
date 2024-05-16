use napi::bindgen_prelude::*;

#[napi(object)]
pub struct NodeSelectionOptions {
  #[napi(js_name = "macOS")]
  pub mac_os: Option<NodeSelectionOptionsMacOs>,
}

#[napi(object)]
pub struct NodeSelectionOptionsMacOs {
  #[napi(ts_type = "(bundleIdentifier: string) => boolean")]
  pub should_manually_enable_accessibility: JsFunction,
}

#[napi]
pub struct NodeSelection {
  options: Option<NodeSelectionOptions>,
}

#[napi]
impl NodeSelection {
  #[napi(constructor)]
  pub fn new(options: Option<NodeSelectionOptions>) -> Self {
    Self { options }
  }

  pub fn get_options(&self) -> &Option<NodeSelectionOptions> {
    &self.options
  }
}

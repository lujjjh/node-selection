use napi::{bindgen_prelude::*, JsBoolean, JsString};

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
  implement: NodeSelectionImpl,
}

#[derive(Clone)]
pub struct NodeSelectionImpl;

#[napi]
impl NodeSelection {
  #[napi(constructor)]
  pub fn new(options: Option<NodeSelectionOptions>) -> Self {
    Self {
      options,
      implement: NodeSelectionImpl,
    }
  }

  pub fn get_options(&self) -> &Option<NodeSelectionOptions> {
    &self.options
  }

  #[napi]
  pub fn check_accessibility_permissions(
    &self,
    options: Option<CheckAccessibilityPermissionOptions>,
  ) -> AsyncTask<AsyncCheckAccessibilityPermission> {
    AsyncTask::new(AsyncCheckAccessibilityPermission {
      implement: self.implement.clone(),
      prompt: options.map(|o| o.prompt).unwrap_or(false),
    })
  }

  #[napi]
  pub fn get_selection(&self) -> AsyncTask<AsyncGetSelection> {
    AsyncTask::new(AsyncGetSelection {
      implement: self.implement.clone(),
    })
  }
}

#[napi(object)]
pub struct CheckAccessibilityPermissionOptions {
  pub prompt: bool,
}

pub trait NodeSelectionTrait {
  fn check_accessibility_permissions(&self, prompt: bool) -> bool;
  fn get_selection(&self) -> Option<String>;
}

pub struct AsyncCheckAccessibilityPermission {
  implement: NodeSelectionImpl,
  prompt: bool,
}

#[napi]
impl Task for AsyncCheckAccessibilityPermission {
  type Output = bool;
  type JsValue = JsBoolean;

  fn compute(&mut self) -> Result<Self::Output> {
    Ok(NodeSelectionTrait::check_accessibility_permissions(
      &self.implement,
      self.prompt,
    ))
  }

  fn resolve(&mut self, env: Env, output: Self::Output) -> Result<Self::JsValue> {
    env.get_boolean(output)
  }
}

pub struct AsyncGetSelection {
  implement: NodeSelectionImpl,
}

#[napi]
impl Task for AsyncGetSelection {
  type Output = Option<String>;
  type JsValue = Option<JsString>;

  fn compute(&mut self) -> Result<Self::Output> {
    Ok(NodeSelectionTrait::get_selection(&self.implement))
  }

  fn resolve(&mut self, env: Env, output: Self::Output) -> Result<Self::JsValue> {
    output.map(|s| env.create_string(s.as_str())).transpose()
  }
}

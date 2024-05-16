use crate::shared::node_selection::NodeSelection;

#[napi]
impl NodeSelection {
  #[napi]
  pub fn check_accessibility_permissions(&self) -> bool {
    true
  }
}

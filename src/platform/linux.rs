use crate::shared::node_selection::{NodeSelection, NodeSelectionTrait};

impl NodeSelectionTrait for NodeSelection {
  fn check_accessibility_permissions(&self, prompt: bool) -> bool {
    true
  }
}

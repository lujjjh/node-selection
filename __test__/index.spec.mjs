import test from "ava";

import { NodeSelection } from "../index.js";

test("checkAccessibilityPermissions", (t) => {
  t.true(new NodeSelection().checkAccessibilityPermissions());
});

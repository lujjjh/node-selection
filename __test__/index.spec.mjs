import test from "ava";

import { NodeSelection } from "../index.js";

test("checkAccessibilityPermissions", async (t) => {
  t.true(
    await new NodeSelection().checkAccessibilityPermissions({ prompt: true })
  );
});

test("getSelection", async (t) => {
  t.is("Hello, World!", await new NodeSelection().getSelection());
});

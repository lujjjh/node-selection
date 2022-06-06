# node-selection

> Get current selected text by using system accessibility APIs

## Installation

```sh
npm i node-selection
```

## Usage

### Accessibility permissions

macOS requires accessibility permissions to be granted before a program
can control the Mac by using accessibility features.

#### `checkAccessibilityPermissions([options])`

- `options`: `<Object>`
  - `prompt`: `<boolean>` **Default:** `false`

Returns: `<Promise>` Fullfills upon success with a boolean indicating
whether accessibility permissions have been granted to this program.

If `prompt` is `true`, a prompt window will be shown when accessibility
permissions have not been granted.

If this method is invoked on non-macOS platform, it always returns `true`.

```js
import { checkAccessibilityPermissions } from 'node-selection';

if (!(await checkAccessibilityPermissions({ prompt: true }))) {
  console.log('grant accessibility permissions and restart this program');
  process.exit(1);
}
```

### Selection

#### `getSelection()`

Returns: `<Promise>` Fullfills upon success with an object with one property:

- `text`: `<string>` | `<undefined>` Current selected text.
- `process`: `Object` | `<undefined>`
  - `pid`: `<number>` | `<undefined>` The process ID.
  - `name`: `<string>` | `<undefined>` The filename of the process.
  - `bundleIdentifier`: `<string>` | `<undefined>` The bundle identifier of the process (macOS only).

```js
import { getSelection } from 'node-selection';

try {
  const { text, process } = await getSelection();
  console.log('current selection:', { text, process });
} catch (error) {
  // no valid selection
  console.error('error', error);
}
```

## Examples

- [Node](example/node-example)
- [Electron](example/electron-example)

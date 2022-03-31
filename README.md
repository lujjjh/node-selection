# node-selection

> Get current selected text by using system accessibility APIs.

## Prerequisite

- Xcode (for macOS)
- Visual Studio (for Windows)

## Install

```sh
npm i node-selection
```

## Usage

### `checkAccessibilityPermissions({ prompt = false }): Promise<boolean>`

`getSelection` requires accessibility permissions to be granted.
This function checks if accessibility permissions are granted.

If `prompt` is set to `true`, a prompt window that allows user
open Accessibility panel will be shown.

On Windows, it always returns `true`.

### `getSelection(): Promise<{ text?: string }>`

Get current selected text. An error will be thrown if this operation
is failed or nothing is selected.

## Example

See [example/getSelection.js](example/getSelection.js).

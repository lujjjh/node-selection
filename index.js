const { promisify } = require('util');
const { checkAccessibilityPermissions, getSelection } = require('node-gyp-build')(__dirname);

const _checkAccessibilityPermissions = promisify(checkAccessibilityPermissions);
const _getSelection = promisify(getSelection);

exports.checkAccessibilityPermissions = ({ prompt = false } = {}) => _checkAccessibilityPermissions(prompt);

exports.getSelection = async () => {
  const text = await _getSelection();
  return { text };
};

const { promisify } = require('util');
const { checkAccessibilityPermissions, getSelection } = require('./build/Release/selection.node');

const _checkAccessibilityPermissions = promisify(checkAccessibilityPermissions);
const _getSelection = promisify(getSelection);

exports.checkAccessibilityPermissions = ({ prompt = false } = {}) => _checkAccessibilityPermissions(prompt);

exports.getSelection = async () => {
  const text = await _getSelection();
  return { text };
};

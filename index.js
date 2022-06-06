const { checkAccessibilityPermissions, getSelection } = require('node-gyp-build')(__dirname);

exports.checkAccessibilityPermissions = ({ prompt = false } = {}) => checkAccessibilityPermissions(prompt);

exports.getSelection = getSelection;

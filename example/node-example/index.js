const { checkAccessibilityPermissions, getSelection } = require('../..');

const sleep = (ms) => new Promise((resolve) => setTimeout(() => resolve(), ms));

(async () => {
  if (!(await checkAccessibilityPermissions({ prompt: true }))) {
    console.log('grant accessibility permissions and restart this program');
    process.exit(1);
  }

  for (;;) {
    try {
      const selection = await getSelection();
      console.log('current selection:', selection);
    } catch (error) {
      console.error('error', error);
    }
    await sleep(1000);
  }
})();

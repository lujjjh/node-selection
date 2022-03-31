const { checkAccessibilityPermissions, getSelection } = require('..');

const sleep = (ms) => new Promise((resolve) => setTimeout(() => resolve(), ms));

(async () => {
  if (!(await checkAccessibilityPermissions({ prompt: true }))) {
    console.log('grant accessibility permissions and restart this program');
    process.exit(1);
  }

  for (let lastSelection; ; ) {
    try {
      const { text: selection } = await getSelection();
      if (selection !== lastSelection) {
        console.log('current selection:', selection);
        lastSelection = selection;
      }
    } catch {}
    await sleep(500);
  }
})();

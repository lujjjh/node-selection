// https://github.com/prebuild/prebuild/issues/284

const { readFileSync, writeFileSync } = require('fs');

const pathToAbiRegistry = require.resolve('node-abi/abi_registry.json');

const abiRegistry = JSON.parse(readFileSync(pathToAbiRegistry, 'utf-8'));

abiRegistry.forEach((item) => {
  if (item.target === '17.0.0') {
    item.target = '17.0.1';
  }
});

writeFileSync(pathToAbiRegistry, JSON.stringify(abiRegistry, null, 2) + '\n');

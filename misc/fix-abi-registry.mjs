// https://github.com/prebuild/prebuild/issues/284

import { readFileSync, writeFileSync } from 'fs';
import { fileURLToPath } from 'url';

const pathToAbiRegistry = fileURLToPath(await import.meta.resolve('node-abi/abi_registry.json'));

const abiRegistry = JSON.parse(readFileSync(pathToAbiRegistry, 'utf-8'));

abiRegistry.forEach((item) => {
  if (item.target === '17.0.0') {
    item.target = '17.0.1';
  }
});

writeFileSync(pathToAbiRegistry, JSON.stringify(abiRegistry, null, 2) + '\n');

// https://github.com/prebuild/prebuild/issues/284

import { promises } from 'fs';
const { writeFile } = promises;
import { fileURLToPath } from 'url';
import fetch from 'node-fetch';

console.error('pulling the latest abi registry...');
const response = await fetch('https://raw.githubusercontent.com/electron/node-abi/main/abi_registry.json');
const abiRegistryContent = await response.text();

console.log('patching...');
const abiRegistry = JSON.parse(abiRegistryContent);
abiRegistry.forEach((item) => {
  if (item.target === '17.0.0') {
    item.target = '17.0.1';
  }
});
const pathToAbiRegistry = fileURLToPath(await import.meta.resolve('node-abi/abi_registry.json'));
await writeFile(pathToAbiRegistry, JSON.stringify(abiRegistry, null, 2) + '\n');

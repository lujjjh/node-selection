// https://github.com/prebuild/prebuild/issues/284

import { spawnSync } from 'child_process';
import { readFileSync, writeFileSync } from 'fs';
import { dirname } from 'path';
import { fileURLToPath } from 'url';

console.error('updating abi registry...');
{
  const cwd = dirname(fileURLToPath(await import.meta.resolve('node-abi/package.json')));
  spawnSync('npm', ['i'], { cwd, shell: true }).output.toString('utf8');
  spawnSync(process.argv[0], [fileURLToPath(await import.meta.resolve('node-abi/scripts/update-abi-registry.js'))], {
    cwd,
    shell: true,
  });
}

const pathToAbiRegistry = fileURLToPath(await import.meta.resolve('node-abi/abi_registry.json'));

const abiRegistry = JSON.parse(readFileSync(pathToAbiRegistry, 'utf-8'));

abiRegistry.forEach((item) => {
  if (item.target === '17.0.0') {
    item.target = '17.0.1';
  }
});

writeFileSync(pathToAbiRegistry, JSON.stringify(abiRegistry, null, 2) + '\n');

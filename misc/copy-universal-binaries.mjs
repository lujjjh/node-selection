// Since we've compiled universal binaries for macOS,
// we simply duplicate all the macOS prebuilds.

import path, { dirname } from 'path';
import { copyFileSync, fstat, readdirSync, statSync } from 'fs';
import { fileURLToPath } from 'url';

if (process.platform !== 'darwin') {
  process.exit(0);
}

const pathToPrebuilds = path.join(dirname(fileURLToPath(import.meta.url)), '../prebuilds');

const copyTasks = [];
for (const filename of readdirSync(pathToPrebuilds)) {
  const src = path.join(pathToPrebuilds, filename);
  if (!statSync(src).isFile()) {
    continue;
  }
  const replaced = filename.replace(/(-darwin-)(x64|arm64)(\.tar\.gz)$/, (_$0, prefix, arch, suffix) => {
    arch = arch === 'x64' ? 'arm64' : 'x64';
    return `${prefix}${arch}${suffix}`;
  });
  if (filename !== replaced) {
    const dest = path.join(pathToPrebuilds, replaced);
    copyTasks.push({ src, dest });
  }
}

for (const { src, dest } of copyTasks) {
  console.error(`copying ${src} to ${dest}`);
  copyFileSync(src, dest);
}

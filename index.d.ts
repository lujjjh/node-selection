export interface CheckAccessibilityPermissionsOptions {
  prompt: boolean;
}
export function checkAccessibilityPermissions(options?: CheckAccessibilityPermissionsOptions): Promise<boolean>;

export interface Selection {
  text?: string;
  process?: ProcessInfo;
}

export interface ProcessInfo {
  pid?: number;
  name?: string;
  bundleIdentifier?: string;
}

export function getSelection(): Promise<Selection>;

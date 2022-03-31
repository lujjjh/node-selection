export interface CheckAccessibilityPermissionsOptions {
  prompt: boolean;
}
export function checkAccessibilityPermissions(options?: CheckAccessibilityPermissionsOptions): Promise<boolean>;

export interface Selection {
  text?: string;
}
export function getSelection(): Promise<Selection>;

export function getDevtoolsGlobalHook() {
    return getTarget().__VUE_DEVTOOLS_GLOBAL_HOOK__;
}
export function getTarget() {
    // @ts-ignore
    return (typeof navigator !== 'undefined' && typeof window !== 'undefined')
        ? window
        : typeof global !== 'undefined'
            ? global
            : {};
}
export const isProxyAvailable = typeof Proxy === 'function';

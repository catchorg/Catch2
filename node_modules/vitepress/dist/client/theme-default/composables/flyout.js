import { ref, watch, readonly, onUnmounted } from 'vue';
export const focusedElement = ref();
let active = false;
let listeners = 0;
export function useFlyout(options) {
    const focus = ref(false);
    if (typeof window !== 'undefined') {
        !active && activateFocusTracking();
        listeners++;
        const unwatch = watch(focusedElement, (el) => {
            if (el === options.el.value || options.el.value?.contains(el)) {
                focus.value = true;
                options.onFocus?.();
            }
            else {
                focus.value = false;
                options.onBlur?.();
            }
        });
        onUnmounted(() => {
            unwatch();
            listeners--;
            if (!listeners) {
                deactivateFocusTracking();
            }
        });
    }
    return readonly(focus);
}
function activateFocusTracking() {
    document.addEventListener('focusin', handleFocusIn);
    active = true;
    focusedElement.value = document.activeElement;
}
function deactivateFocusTracking() {
    document.removeEventListener('focusin', handleFocusIn);
}
function handleFocusIn() {
    focusedElement.value = document.activeElement;
}

import { nextTick, watch } from 'vue';
import { inBrowser, useData } from 'vitepress';
export function useCopyCode() {
    const { page } = useData();
    if (inBrowser)
        watch(() => page.value.relativePath, () => {
            nextTick(() => {
                document
                    .querySelectorAll('.vp-doc div[class*="language-"]>span.copy')
                    .forEach(handleElement);
            });
        }, { immediate: true, flush: 'post' });
}
async function copyToClipboard(text) {
    try {
        return navigator.clipboard.writeText(text);
    }
    catch {
        const element = document.createElement('textarea');
        const previouslyFocusedElement = document.activeElement;
        element.value = text;
        // Prevent keyboard from showing on mobile
        element.setAttribute('readonly', '');
        element.style.contain = 'strict';
        element.style.position = 'absolute';
        element.style.left = '-9999px';
        element.style.fontSize = '12pt'; // Prevent zooming on iOS
        const selection = document.getSelection();
        const originalRange = selection
            ? selection.rangeCount > 0 && selection.getRangeAt(0)
            : null;
        document.body.appendChild(element);
        element.select();
        // Explicit selection workaround for iOS
        element.selectionStart = 0;
        element.selectionEnd = text.length;
        document.execCommand('copy');
        document.body.removeChild(element);
        if (originalRange) {
            selection.removeAllRanges(); // originalRange can't be truthy when selection is falsy
            selection.addRange(originalRange);
        }
        // Get the focus back on the previously focused element, if any
        if (previouslyFocusedElement) {
            ;
            previouslyFocusedElement.focus();
        }
    }
}
function handleElement(el) {
    el.onclick = () => {
        const parent = el.parentElement;
        if (!parent) {
            return;
        }
        const isShell = parent.classList.contains('language-sh') ||
            parent.classList.contains('language-bash');
        let { innerText: text = '' } = parent;
        if (isShell) {
            text = text.replace(/^ *\$ /gm, '');
        }
        copyToClipboard(text).then(() => {
            el.classList.add('copied');
            setTimeout(() => {
                el.classList.remove('copied');
            }, 3000);
        });
    };
}

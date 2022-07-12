import { ref, computed, watchEffect, onMounted, onUnmounted } from 'vue';
import { useRoute, useData } from 'vitepress';
import { getSidebar } from '../support/sidebar';
export function useSidebar() {
    const route = useRoute();
    const { theme, frontmatter } = useData();
    const isOpen = ref(false);
    const sidebar = computed(() => {
        const sidebarConfig = theme.value.sidebar;
        const relativePath = route.data.relativePath;
        return sidebarConfig ? getSidebar(sidebarConfig, relativePath) : [];
    });
    const hasSidebar = computed(() => {
        return (frontmatter.value.sidebar !== false &&
            sidebar.value.length > 0 &&
            frontmatter.value.layout !== 'home');
    });
    function open() {
        isOpen.value = true;
    }
    function close() {
        isOpen.value = false;
    }
    function toggle() {
        isOpen.value ? close() : open();
    }
    return {
        isOpen,
        sidebar,
        hasSidebar,
        open,
        close,
        toggle
    };
}
/**
 * a11y: cache the element that opened the Sidebar (the menu button) then
 * focus that button again when Menu is closed with Escape key.
 */
export function useCloseSidebarOnEscape(isOpen, close) {
    let triggerElement;
    watchEffect(() => {
        triggerElement = isOpen.value
            ? document.activeElement
            : undefined;
    });
    onMounted(() => {
        window.addEventListener('keyup', onEscape);
    });
    onUnmounted(() => {
        window.removeEventListener('keyup', onEscape);
    });
    function onEscape(e) {
        if (e.key === 'Escape' && isOpen.value) {
            close();
            triggerElement?.focus();
        }
    }
}

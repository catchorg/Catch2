import { ref, computed } from 'vue';
import { useData, useRoute } from 'vitepress';
export function useNav() {
    const isScreenOpen = ref(false);
    function openScreen() {
        isScreenOpen.value = true;
        window.addEventListener('resize', closeScreenOnTabletWindow);
    }
    function closeScreen() {
        isScreenOpen.value = false;
        window.removeEventListener('resize', closeScreenOnTabletWindow);
    }
    function toggleScreen() {
        isScreenOpen.value ? closeScreen() : openScreen();
    }
    /**
     * Close screen when the user resizes the window wider than tablet size.
     */
    function closeScreenOnTabletWindow() {
        window.outerWidth >= 768 && closeScreen();
    }
    return {
        isScreenOpen,
        openScreen,
        closeScreen,
        toggleScreen
    };
}
export function useLanguageLinks() {
    const { site, localePath, theme } = useData();
    return computed(() => {
        const langs = site.value.langs;
        const localePaths = Object.keys(langs);
        // one language
        if (localePaths.length < 2) {
            return null;
        }
        const route = useRoute();
        // intentionally remove the leading slash because each locale has one
        const currentPath = route.path.replace(localePath.value, '');
        const candidates = localePaths.map((localePath) => ({
            text: langs[localePath].label,
            link: `${localePath}${currentPath}`
        }));
        const selectText = theme.value.selectText || 'Languages';
        return {
            text: selectText,
            items: candidates
        };
    });
}

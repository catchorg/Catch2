import { computed } from 'vue';
import { useData } from 'vitepress';
import { isActive } from '../support/utils';
import { getSidebar, getFlatSideBarLinks } from '../support/sidebar';
export function usePrevNext() {
    const { page, theme } = useData();
    return computed(() => {
        const sidebar = getSidebar(theme.value.sidebar, page.value.relativePath);
        const candidates = getFlatSideBarLinks(sidebar);
        const index = candidates.findIndex((link) => {
            return isActive(page.value.relativePath, link.link);
        });
        return {
            prev: candidates[index - 1],
            next: candidates[index + 1]
        };
    });
}

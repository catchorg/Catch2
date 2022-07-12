import { ensureStartingSlash } from './utils';
/**
 * Get the `Sidebar` from sidebar option. This method will ensure to get correct
 * sidebar config from `MultiSideBarConfig` with various path combinations such
 * as matching `guide/` and `/guide/`. If no matching config was found, it will
 * return empty array.
 */
export function getSidebar(sidebar, path) {
    if (Array.isArray(sidebar)) {
        return sidebar;
    }
    path = ensureStartingSlash(path);
    for (const dir in sidebar) {
        // make sure the multi sidebar key starts with slash too
        if (path.startsWith(ensureStartingSlash(dir))) {
            return sidebar[dir];
        }
    }
    return [];
}
export function getFlatSideBarLinks(sidebar) {
    const links = [];
    for (const group of sidebar) {
        for (const link of group.items) {
            links.push(link);
        }
    }
    return links;
}

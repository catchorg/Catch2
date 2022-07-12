export const EXTERNAL_URL_RE = /^https?:/i;
export const APPEARANCE_KEY = 'vitepress-theme-appearance';
// @ts-ignore
export const inBrowser = typeof window !== 'undefined';
export const notFoundPageData = {
    relativePath: '',
    title: '404',
    description: 'Not Found',
    headers: [],
    frontmatter: {},
    lastUpdated: 0
};
function findMatchRoot(route, roots) {
    // first match to the routes with the most deep level.
    roots.sort((a, b) => {
        const levelDelta = b.split('/').length - a.split('/').length;
        if (levelDelta !== 0) {
            return levelDelta;
        }
        else {
            return b.length - a.length;
        }
    });
    for (const r of roots) {
        if (route.startsWith(r))
            return r;
    }
}
function resolveLocales(locales, route) {
    const localeRoot = findMatchRoot(route, Object.keys(locales));
    return localeRoot ? locales[localeRoot] : undefined;
}
export function createLangDictionary(siteData) {
    const { locales } = siteData.themeConfig || {};
    const siteLocales = siteData.locales;
    return locales && siteLocales
        ? Object.keys(locales).reduce((langs, path) => {
            langs[path] = {
                label: locales[path].label,
                lang: siteLocales[path].lang
            };
            return langs;
        }, {})
        : {};
}
// this merges the locales data to the main data by the route
export function resolveSiteDataByRoute(siteData, route) {
    route = cleanRoute(siteData, route);
    const localeData = resolveLocales(siteData.locales || {}, route);
    const localeThemeConfig = resolveLocales(siteData.themeConfig.locales || {}, route);
    // avoid object rest spread since this is going to run in the browser
    // and spread is going to result in polyfill code
    return Object.assign({}, siteData, localeData, {
        themeConfig: Object.assign({}, siteData.themeConfig, localeThemeConfig, {
            // clean the locales to reduce the bundle size
            locales: {}
        }),
        lang: (localeData || siteData).lang,
        // clean the locales to reduce the bundle size
        locales: {},
        langs: createLangDictionary(siteData)
    });
}
/**
 * Create the page title string based on configs.
 */
export function createTitle(siteData, pageData) {
    const title = pageData.title || siteData.title;
    const template = pageData.titleTemplate ?? siteData.titleTemplate;
    const templateString = createTitleTemplate(siteData.title, template);
    return `${title}${templateString}`;
}
function createTitleTemplate(siteTitle, template) {
    if (template === false) {
        return '';
    }
    if (template === true || template === undefined) {
        return ` | ${siteTitle}`;
    }
    if (siteTitle === template) {
        return '';
    }
    return ` | ${template}`;
}
/**
 * Clean up the route by removing the `base` path if it's set in config.
 */
function cleanRoute(siteData, route) {
    if (!inBrowser) {
        return route;
    }
    const base = siteData.base;
    const baseWithoutSuffix = base.endsWith('/') ? base.slice(0, -1) : base;
    return route.slice(baseWithoutSuffix.length);
}

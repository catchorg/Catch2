import type * as _compiler from 'vue/compiler-sfc';
import type { Plugin as Plugin_2 } from 'vite';
import type { SFCScriptCompileOptions } from 'vue/compiler-sfc';
import type { SFCStyleCompileOptions } from 'vue/compiler-sfc';
import type { SFCTemplateCompileOptions } from 'vue/compiler-sfc';
import type { ViteDevServer } from 'vite';

export declare interface Options {
    include?: string | RegExp | (string | RegExp)[];
    exclude?: string | RegExp | (string | RegExp)[];
    isProduction?: boolean;
    script?: Partial<SFCScriptCompileOptions>;
    template?: Partial<SFCTemplateCompileOptions>;
    style?: Partial<SFCStyleCompileOptions>;
    /**
     * Transform Vue SFCs into custom elements.
     * - `true`: all `*.vue` imports are converted into custom elements
     * - `string | RegExp`: matched files are converted into custom elements
     *
     * @default /\.ce\.vue$/
     */
    customElement?: boolean | string | RegExp | (string | RegExp)[];
    /**
     * Enable Vue reactivity transform (experimental).
     * https://github.com/vuejs/core/tree/master/packages/reactivity-transform
     * - `true`: transform will be enabled for all vue,js(x),ts(x) files except
     *           those inside node_modules
     * - `string | RegExp`: apply to vue + only matched files (will include
     *                      node_modules, so specify directories in necessary)
     * - `false`: disable in all cases
     *
     * @default false
     */
    reactivityTransform?: boolean | string | RegExp | (string | RegExp)[];
    /**
     * Use custom compiler-sfc instance. Can be used to force a specific version.
     */
    compiler?: typeof _compiler;
}

export declare function parseVueRequest(id: string): {
    filename: string;
    query: VueQuery;
};

export declare interface ResolvedOptions extends Options {
    compiler: typeof _compiler;
    root: string;
    sourceMap: boolean;
    cssDevSourcemap: boolean;
    devServer?: ViteDevServer;
    devToolsEnabled?: boolean;
}

declare function vuePlugin(rawOptions?: Options): Plugin_2;
export default vuePlugin;

export declare interface VueQuery {
    vue?: boolean;
    src?: string;
    type?: 'script' | 'template' | 'style' | 'custom';
    index?: number;
    lang?: string;
    raw?: boolean;
}

export { }

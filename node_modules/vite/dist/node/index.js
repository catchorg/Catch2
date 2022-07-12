'use strict';

Object.defineProperty(exports, '__esModule', { value: true });

var index = require('./chunks/dep-c9998dc6.js');
require('fs');
require('path');
require('url');
require('perf_hooks');
require('tty');
require('os');
require('esbuild');
require('events');
require('assert');
require('resolve');
require('util');
require('stream');
require('net');
require('http');
require('child_process');
require('module');
require('crypto');
require('buffer');
require('querystring');
require('zlib');
require('https');
require('tls');
require('worker_threads');
require('readline');

// Use splitVendorChunkPlugin() to get the same manualChunks strategy as Vite 2.7
// We don't recommend using this strategy as a general solution moving forward
// splitVendorChunk is a simple index/vendor strategy that was used in Vite
// until v2.8. It is exposed to let people continue to use it in case it was
// working well for their setups.
// The cache needs to be reset on buildStart for watch mode to work correctly
// Don't use this manualChunks strategy for ssr, lib mode, and 'umd' or 'iife'
class SplitVendorChunkCache {
    constructor() {
        this.cache = new Map();
    }
    reset() {
        this.cache = new Map();
    }
}
function splitVendorChunk(options = {}) {
    var _a;
    const cache = (_a = options.cache) !== null && _a !== void 0 ? _a : new SplitVendorChunkCache();
    return (id, { getModuleInfo }) => {
        if (id.includes('node_modules') &&
            !index.isCSSRequest(id) &&
            staticImportedByEntry(id, getModuleInfo, cache.cache)) {
            return 'vendor';
        }
    };
}
function staticImportedByEntry(id, getModuleInfo, cache, importStack = []) {
    if (cache.has(id)) {
        return cache.get(id);
    }
    if (importStack.includes(id)) {
        // circular deps!
        cache.set(id, false);
        return false;
    }
    const mod = getModuleInfo(id);
    if (!mod) {
        cache.set(id, false);
        return false;
    }
    if (mod.isEntry) {
        cache.set(id, true);
        return true;
    }
    const someImporterIs = mod.importers.some((importer) => staticImportedByEntry(importer, getModuleInfo, cache, importStack.concat(id)));
    cache.set(id, someImporterIs);
    return someImporterIs;
}
function splitVendorChunkPlugin() {
    const caches = [];
    function createSplitVendorChunk(output, config) {
        var _a;
        const cache = new SplitVendorChunkCache();
        caches.push(cache);
        const build = (_a = config.build) !== null && _a !== void 0 ? _a : {};
        const format = output === null || output === void 0 ? void 0 : output.format;
        if (!build.ssr && !build.lib && format !== 'umd' && format !== 'iife') {
            return splitVendorChunk({ cache });
        }
    }
    return {
        name: 'vite:split-vendor-chunk',
        config(config) {
            var _a, _b;
            let outputs = (_b = (_a = config === null || config === void 0 ? void 0 : config.build) === null || _a === void 0 ? void 0 : _a.rollupOptions) === null || _b === void 0 ? void 0 : _b.output;
            if (outputs) {
                outputs = Array.isArray(outputs) ? outputs : [outputs];
                for (const output of outputs) {
                    const viteManualChunks = createSplitVendorChunk(output, config);
                    if (viteManualChunks) {
                        if (output.manualChunks) {
                            if (typeof output.manualChunks === 'function') {
                                const userManualChunks = output.manualChunks;
                                output.manualChunks = (id, api) => {
                                    var _a;
                                    return (_a = userManualChunks(id, api)) !== null && _a !== void 0 ? _a : viteManualChunks(id, api);
                                };
                            }
                            // else, leave the object form of manualChunks untouched, as
                            // we can't safely replicate rollup handling.
                        }
                        else {
                            output.manualChunks = viteManualChunks;
                        }
                    }
                }
            }
            else {
                return {
                    build: {
                        rollupOptions: {
                            output: {
                                manualChunks: createSplitVendorChunk({}, config)
                            }
                        }
                    }
                };
            }
        },
        buildStart() {
            caches.forEach((cache) => cache.reset());
        }
    };
}

exports.build = index.build;
exports.createLogger = index.createLogger;
exports.createServer = index.createServer;
exports.defineConfig = index.defineConfig;
exports.formatPostcssSourceMap = index.formatPostcssSourceMap;
exports.loadConfigFromFile = index.loadConfigFromFile;
exports.loadEnv = index.loadEnv;
exports.mergeConfig = index.mergeConfig;
exports.normalizePath = index.normalizePath;
exports.optimizeDeps = index.optimizeDeps;
exports.preview = index.preview;
exports.printHttpServerUrls = index.printHttpServerUrls;
exports.resolveConfig = index.resolveConfig;
exports.resolveEnvPrefix = index.resolveEnvPrefix;
exports.resolvePackageData = index.resolvePackageData;
exports.resolvePackageEntry = index.resolvePackageEntry;
exports.searchForWorkspaceRoot = index.searchForWorkspaceRoot;
exports.send = index.send;
exports.sortUserPlugins = index.sortUserPlugins;
exports.transformWithEsbuild = index.transformWithEsbuild;
exports.splitVendorChunk = splitVendorChunk;
exports.splitVendorChunkPlugin = splitVendorChunkPlugin;

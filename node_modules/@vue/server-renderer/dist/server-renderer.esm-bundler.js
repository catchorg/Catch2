import { ssrContextKey, warn as warn$1, Fragment, Static, Comment, Text, mergeProps, ssrUtils, createApp, createVNode, initDirectivesForSSR } from 'vue';
import { makeMap, isOn, escapeHtml, normalizeClass, propsToAttrMap, isBooleanAttr, includeBooleanAttr, isSSRSafeAttrName, isString, normalizeStyle, stringifyStyle, escapeHtmlComment, isVoidTag, isPromise, isArray, isFunction, NOOP, toDisplayString, isObject, looseEqual, looseIndexOf } from '@vue/shared';
export { includeBooleanAttr as ssrIncludeBooleanAttr } from '@vue/shared';

// leading comma for empty string ""
const shouldIgnoreProp = makeMap(`,key,ref,innerHTML,textContent,ref_key,ref_for`);
function ssrRenderAttrs(props, tag) {
    let ret = '';
    for (const key in props) {
        if (shouldIgnoreProp(key) ||
            isOn(key) ||
            (tag === 'textarea' && key === 'value')) {
            continue;
        }
        const value = props[key];
        if (key === 'class') {
            ret += ` class="${ssrRenderClass(value)}"`;
        }
        else if (key === 'style') {
            ret += ` style="${ssrRenderStyle(value)}"`;
        }
        else {
            ret += ssrRenderDynamicAttr(key, value, tag);
        }
    }
    return ret;
}
// render an attr with dynamic (unknown) key.
function ssrRenderDynamicAttr(key, value, tag) {
    if (!isRenderableValue(value)) {
        return ``;
    }
    const attrKey = tag && tag.indexOf('-') > 0
        ? key // preserve raw name on custom elements
        : propsToAttrMap[key] || key.toLowerCase();
    if (isBooleanAttr(attrKey)) {
        return includeBooleanAttr(value) ? ` ${attrKey}` : ``;
    }
    else if (isSSRSafeAttrName(attrKey)) {
        return value === '' ? ` ${attrKey}` : ` ${attrKey}="${escapeHtml(value)}"`;
    }
    else {
        console.warn(`[@vue/server-renderer] Skipped rendering unsafe attribute name: ${attrKey}`);
        return ``;
    }
}
// Render a v-bind attr with static key. The key is pre-processed at compile
// time and we only need to check and escape value.
function ssrRenderAttr(key, value) {
    if (!isRenderableValue(value)) {
        return ``;
    }
    return ` ${key}="${escapeHtml(value)}"`;
}
function isRenderableValue(value) {
    if (value == null) {
        return false;
    }
    const type = typeof value;
    return type === 'string' || type === 'number' || type === 'boolean';
}
function ssrRenderClass(raw) {
    return escapeHtml(normalizeClass(raw));
}
function ssrRenderStyle(raw) {
    if (!raw) {
        return '';
    }
    if (isString(raw)) {
        return escapeHtml(raw);
    }
    const styles = normalizeStyle(raw);
    return escapeHtml(stringifyStyle(styles));
}

function ssrCompile(template, instance) {
    {
        throw new Error(`On-the-fly template compilation is not supported in the ESM build of ` +
            `@vue/server-renderer. All templates must be pre-compiled into ` +
            `render functions.`);
    }
}

function ssrRenderTeleport(parentPush, contentRenderFn, target, disabled, parentComponent) {
    parentPush('<!--teleport start-->');
    const context = parentComponent.appContext.provides[ssrContextKey];
    const teleportBuffers = context.__teleportBuffers || (context.__teleportBuffers = {});
    const targetBuffer = teleportBuffers[target] || (teleportBuffers[target] = []);
    // record current index of the target buffer to handle nested teleports
    // since the parent needs to be rendered before the child
    const bufferIndex = targetBuffer.length;
    let teleportContent;
    if (disabled) {
        contentRenderFn(parentPush);
        teleportContent = `<!--teleport anchor-->`;
    }
    else {
        const { getBuffer, push } = createBuffer();
        contentRenderFn(push);
        push(`<!--teleport anchor-->`);
        teleportContent = getBuffer();
    }
    targetBuffer.splice(bufferIndex, 0, teleportContent);
    parentPush('<!--teleport end-->');
}

const { createComponentInstance, setCurrentRenderingInstance, setupComponent, renderComponentRoot, normalizeVNode } = ssrUtils;
// Each component has a buffer array.
// A buffer array can contain one of the following:
// - plain string
// - A resolved buffer (recursive arrays of strings that can be unrolled
//   synchronously)
// - An async buffer (a Promise that resolves to a resolved buffer)
function createBuffer() {
    let appendable = false;
    const buffer = [];
    return {
        getBuffer() {
            // Return static buffer and await on items during unroll stage
            return buffer;
        },
        push(item) {
            const isStringItem = isString(item);
            if (appendable && isStringItem) {
                buffer[buffer.length - 1] += item;
            }
            else {
                buffer.push(item);
            }
            appendable = isStringItem;
            if (isPromise(item) || (isArray(item) && item.hasAsync)) {
                // promise, or child buffer with async, mark as async.
                // this allows skipping unnecessary await ticks during unroll stage
                buffer.hasAsync = true;
            }
        }
    };
}
function renderComponentVNode(vnode, parentComponent = null, slotScopeId) {
    const instance = createComponentInstance(vnode, parentComponent, null);
    const res = setupComponent(instance, true /* isSSR */);
    const hasAsyncSetup = isPromise(res);
    const prefetches = instance.sp; /* LifecycleHooks.SERVER_PREFETCH */
    if (hasAsyncSetup || prefetches) {
        let p = hasAsyncSetup
            ? res
            : Promise.resolve();
        if (prefetches) {
            p = p
                .then(() => Promise.all(prefetches.map(prefetch => prefetch.call(instance.proxy))))
                // Note: error display is already done by the wrapped lifecycle hook function.
                .catch(() => { });
        }
        return p.then(() => renderComponentSubTree(instance, slotScopeId));
    }
    else {
        return renderComponentSubTree(instance, slotScopeId);
    }
}
function renderComponentSubTree(instance, slotScopeId) {
    const comp = instance.type;
    const { getBuffer, push } = createBuffer();
    if (isFunction(comp)) {
        let root = renderComponentRoot(instance);
        // #5817 scope ID attrs not falling through if functional component doesn't
        // have props
        if (!comp.props) {
            for (const key in instance.attrs) {
                if (key.startsWith(`data-v-`)) {
                    (root.props || (root.props = {}))[key] = ``;
                }
            }
        }
        renderVNode(push, (instance.subTree = root), instance, slotScopeId);
    }
    else {
        if ((!instance.render || instance.render === NOOP) &&
            !instance.ssrRender &&
            !comp.ssrRender &&
            isString(comp.template)) {
            comp.ssrRender = ssrCompile(comp.template);
        }
        // perf: enable caching of computed getters during render
        // since there cannot be state mutations during render.
        for (const e of instance.scope.effects) {
            if (e.computed)
                e.computed._cacheable = true;
        }
        const ssrRender = instance.ssrRender || comp.ssrRender;
        if (ssrRender) {
            // optimized
            // resolve fallthrough attrs
            let attrs = instance.inheritAttrs !== false ? instance.attrs : undefined;
            let hasCloned = false;
            let cur = instance;
            while (true) {
                const scopeId = cur.vnode.scopeId;
                if (scopeId) {
                    if (!hasCloned) {
                        attrs = { ...attrs };
                        hasCloned = true;
                    }
                    attrs[scopeId] = '';
                }
                const parent = cur.parent;
                if (parent && parent.subTree && parent.subTree === cur.vnode) {
                    // parent is a non-SSR compiled component and is rendering this
                    // component as root. inherit its scopeId if present.
                    cur = parent;
                }
                else {
                    break;
                }
            }
            if (slotScopeId) {
                if (!hasCloned)
                    attrs = { ...attrs };
                attrs[slotScopeId.trim()] = '';
            }
            // set current rendering instance for asset resolution
            const prev = setCurrentRenderingInstance(instance);
            ssrRender(instance.proxy, push, instance, attrs, 
            // compiler-optimized bindings
            instance.props, instance.setupState, instance.data, instance.ctx);
            setCurrentRenderingInstance(prev);
        }
        else if (instance.render && instance.render !== NOOP) {
            renderVNode(push, (instance.subTree = renderComponentRoot(instance)), instance, slotScopeId);
        }
        else {
            const componentName = comp.name || comp.__file || `<Anonymous>`;
            warn$1(`Component ${componentName} is missing template or render function.`);
            push(`<!---->`);
        }
    }
    return getBuffer();
}
function renderVNode(push, vnode, parentComponent, slotScopeId) {
    const { type, shapeFlag, children } = vnode;
    switch (type) {
        case Text:
            push(escapeHtml(children));
            break;
        case Comment:
            push(children ? `<!--${escapeHtmlComment(children)}-->` : `<!---->`);
            break;
        case Static:
            push(children);
            break;
        case Fragment:
            if (vnode.slotScopeIds) {
                slotScopeId =
                    (slotScopeId ? slotScopeId + ' ' : '') + vnode.slotScopeIds.join(' ');
            }
            push(`<!--[-->`); // open
            renderVNodeChildren(push, children, parentComponent, slotScopeId);
            push(`<!--]-->`); // close
            break;
        default:
            if (shapeFlag & 1 /* ELEMENT */) {
                renderElementVNode(push, vnode, parentComponent, slotScopeId);
            }
            else if (shapeFlag & 6 /* COMPONENT */) {
                push(renderComponentVNode(vnode, parentComponent, slotScopeId));
            }
            else if (shapeFlag & 64 /* TELEPORT */) {
                renderTeleportVNode(push, vnode, parentComponent, slotScopeId);
            }
            else if (shapeFlag & 128 /* SUSPENSE */) {
                renderVNode(push, vnode.ssContent, parentComponent, slotScopeId);
            }
            else {
                warn$1('[@vue/server-renderer] Invalid VNode type:', type, `(${typeof type})`);
            }
    }
}
function renderVNodeChildren(push, children, parentComponent, slotScopeId) {
    for (let i = 0; i < children.length; i++) {
        renderVNode(push, normalizeVNode(children[i]), parentComponent, slotScopeId);
    }
}
function renderElementVNode(push, vnode, parentComponent, slotScopeId) {
    const tag = vnode.type;
    let { props, children, shapeFlag, scopeId, dirs } = vnode;
    let openTag = `<${tag}`;
    if (dirs) {
        props = applySSRDirectives(vnode, props, dirs);
    }
    if (props) {
        openTag += ssrRenderAttrs(props, tag);
    }
    if (scopeId) {
        openTag += ` ${scopeId}`;
    }
    // inherit parent chain scope id if this is the root node
    let curParent = parentComponent;
    let curVnode = vnode;
    while (curParent && curVnode === curParent.subTree) {
        curVnode = curParent.vnode;
        if (curVnode.scopeId) {
            openTag += ` ${curVnode.scopeId}`;
        }
        curParent = curParent.parent;
    }
    if (slotScopeId) {
        openTag += ` ${slotScopeId}`;
    }
    push(openTag + `>`);
    if (!isVoidTag(tag)) {
        let hasChildrenOverride = false;
        if (props) {
            if (props.innerHTML) {
                hasChildrenOverride = true;
                push(props.innerHTML);
            }
            else if (props.textContent) {
                hasChildrenOverride = true;
                push(escapeHtml(props.textContent));
            }
            else if (tag === 'textarea' && props.value) {
                hasChildrenOverride = true;
                push(escapeHtml(props.value));
            }
        }
        if (!hasChildrenOverride) {
            if (shapeFlag & 8 /* TEXT_CHILDREN */) {
                push(escapeHtml(children));
            }
            else if (shapeFlag & 16 /* ARRAY_CHILDREN */) {
                renderVNodeChildren(push, children, parentComponent, slotScopeId);
            }
        }
        push(`</${tag}>`);
    }
}
function applySSRDirectives(vnode, rawProps, dirs) {
    const toMerge = [];
    for (let i = 0; i < dirs.length; i++) {
        const binding = dirs[i];
        const { dir: { getSSRProps } } = binding;
        if (getSSRProps) {
            const props = getSSRProps(binding, vnode);
            if (props)
                toMerge.push(props);
        }
    }
    return mergeProps(rawProps || {}, ...toMerge);
}
function renderTeleportVNode(push, vnode, parentComponent, slotScopeId) {
    const target = vnode.props && vnode.props.to;
    const disabled = vnode.props && vnode.props.disabled;
    if (!target) {
        if (!disabled) {
            warn$1(`[@vue/server-renderer] Teleport is missing target prop.`);
        }
        return [];
    }
    if (!isString(target)) {
        warn$1(`[@vue/server-renderer] Teleport target must be a query selector string.`);
        return [];
    }
    ssrRenderTeleport(push, push => {
        renderVNodeChildren(push, vnode.children, parentComponent, slotScopeId);
    }, target, disabled || disabled === '', parentComponent);
}

const { isVNode } = ssrUtils;
async function unrollBuffer(buffer) {
    if (buffer.hasAsync) {
        let ret = '';
        for (let i = 0; i < buffer.length; i++) {
            let item = buffer[i];
            if (isPromise(item)) {
                item = await item;
            }
            if (isString(item)) {
                ret += item;
            }
            else {
                ret += await unrollBuffer(item);
            }
        }
        return ret;
    }
    else {
        // sync buffer can be more efficiently unrolled without unnecessary await
        // ticks
        return unrollBufferSync(buffer);
    }
}
function unrollBufferSync(buffer) {
    let ret = '';
    for (let i = 0; i < buffer.length; i++) {
        let item = buffer[i];
        if (isString(item)) {
            ret += item;
        }
        else {
            // since this is a sync buffer, child buffers are never promises
            ret += unrollBufferSync(item);
        }
    }
    return ret;
}
async function renderToString(input, context = {}) {
    if (isVNode(input)) {
        // raw vnode, wrap with app (for context)
        return renderToString(createApp({ render: () => input }), context);
    }
    // rendering an app
    const vnode = createVNode(input._component, input._props);
    vnode.appContext = input._context;
    // provide the ssr context to the tree
    input.provide(ssrContextKey, context);
    const buffer = await renderComponentVNode(vnode);
    const result = await unrollBuffer(buffer);
    await resolveTeleports(context);
    return result;
}
async function resolveTeleports(context) {
    if (context.__teleportBuffers) {
        context.teleports = context.teleports || {};
        for (const key in context.__teleportBuffers) {
            // note: it's OK to await sequentially here because the Promises were
            // created eagerly in parallel.
            context.teleports[key] = await unrollBuffer((await Promise.all(context.__teleportBuffers[key])));
        }
    }
}

const { isVNode: isVNode$1 } = ssrUtils;
async function unrollBuffer$1(buffer, stream) {
    if (buffer.hasAsync) {
        for (let i = 0; i < buffer.length; i++) {
            let item = buffer[i];
            if (isPromise(item)) {
                item = await item;
            }
            if (isString(item)) {
                stream.push(item);
            }
            else {
                await unrollBuffer$1(item, stream);
            }
        }
    }
    else {
        // sync buffer can be more efficiently unrolled without unnecessary await
        // ticks
        unrollBufferSync$1(buffer, stream);
    }
}
function unrollBufferSync$1(buffer, stream) {
    for (let i = 0; i < buffer.length; i++) {
        let item = buffer[i];
        if (isString(item)) {
            stream.push(item);
        }
        else {
            // since this is a sync buffer, child buffers are never promises
            unrollBufferSync$1(item, stream);
        }
    }
}
function renderToSimpleStream(input, context, stream) {
    if (isVNode$1(input)) {
        // raw vnode, wrap with app (for context)
        return renderToSimpleStream(createApp({ render: () => input }), context, stream);
    }
    // rendering an app
    const vnode = createVNode(input._component, input._props);
    vnode.appContext = input._context;
    // provide the ssr context to the tree
    input.provide(ssrContextKey, context);
    Promise.resolve(renderComponentVNode(vnode))
        .then(buffer => unrollBuffer$1(buffer, stream))
        .then(() => resolveTeleports(context))
        .then(() => stream.push(null))
        .catch(error => {
        stream.destroy(error);
    });
    return stream;
}
/**
 * @deprecated
 */
function renderToStream(input, context = {}) {
    console.warn(`[@vue/server-renderer] renderToStream is deprecated - use renderToNodeStream instead.`);
    return renderToNodeStream(input, context);
}
function renderToNodeStream(input, context = {}) {
    const stream = null;
    {
        throw new Error(`ESM build of renderToStream() does not support renderToNodeStream(). ` +
            `Use pipeToNodeWritable() with an existing Node.js Writable stream ` +
            `instance instead.`);
    }
}
function pipeToNodeWritable(input, context = {}, writable) {
    renderToSimpleStream(input, context, {
        push(content) {
            if (content != null) {
                writable.write(content);
            }
            else {
                writable.end();
            }
        },
        destroy(err) {
            writable.destroy(err);
        }
    });
}
function renderToWebStream(input, context = {}) {
    if (typeof ReadableStream !== 'function') {
        throw new Error(`ReadableStream constructor is not available in the global scope. ` +
            `If the target environment does support web streams, consider using ` +
            `pipeToWebWritable() with an existing WritableStream instance instead.`);
    }
    const encoder = new TextEncoder();
    let cancelled = false;
    return new ReadableStream({
        start(controller) {
            renderToSimpleStream(input, context, {
                push(content) {
                    if (cancelled)
                        return;
                    if (content != null) {
                        controller.enqueue(encoder.encode(content));
                    }
                    else {
                        controller.close();
                    }
                },
                destroy(err) {
                    controller.error(err);
                }
            });
        },
        cancel() {
            cancelled = true;
        }
    });
}
function pipeToWebWritable(input, context = {}, writable) {
    const writer = writable.getWriter();
    const encoder = new TextEncoder();
    // #4287 CloudFlare workers do not implement `ready` property
    let hasReady = false;
    try {
        hasReady = isPromise(writer.ready);
    }
    catch (e) { }
    renderToSimpleStream(input, context, {
        async push(content) {
            if (hasReady) {
                await writer.ready;
            }
            if (content != null) {
                return writer.write(encoder.encode(content));
            }
            else {
                return writer.close();
            }
        },
        destroy(err) {
            // TODO better error handling?
            console.log(err);
            writer.close();
        }
    });
}

function ssrRenderComponent(comp, props = null, children = null, parentComponent = null, slotScopeId) {
    return renderComponentVNode(createVNode(comp, props, children), parentComponent, slotScopeId);
}

function ssrRenderSlot(slots, slotName, slotProps, fallbackRenderFn, push, parentComponent, slotScopeId) {
    // template-compiled slots are always rendered as fragments
    push(`<!--[-->`);
    ssrRenderSlotInner(slots, slotName, slotProps, fallbackRenderFn, push, parentComponent, slotScopeId);
    push(`<!--]-->`);
}
function ssrRenderSlotInner(slots, slotName, slotProps, fallbackRenderFn, push, parentComponent, slotScopeId, transition) {
    const slotFn = slots[slotName];
    if (slotFn) {
        const slotBuffer = [];
        const bufferedPush = (item) => {
            slotBuffer.push(item);
        };
        const ret = slotFn(slotProps, bufferedPush, parentComponent, slotScopeId ? ' ' + slotScopeId : '');
        if (isArray(ret)) {
            // normal slot
            renderVNodeChildren(push, ret, parentComponent, slotScopeId);
        }
        else {
            // ssr slot.
            // check if the slot renders all comments, in which case use the fallback
            let isEmptySlot = true;
            if (transition) {
                isEmptySlot = false;
            }
            else {
                for (let i = 0; i < slotBuffer.length; i++) {
                    if (!isComment(slotBuffer[i])) {
                        isEmptySlot = false;
                        break;
                    }
                }
            }
            if (isEmptySlot) {
                if (fallbackRenderFn) {
                    fallbackRenderFn();
                }
            }
            else {
                for (let i = 0; i < slotBuffer.length; i++) {
                    push(slotBuffer[i]);
                }
            }
        }
    }
    else if (fallbackRenderFn) {
        fallbackRenderFn();
    }
}
const commentRE = /<!--[^]*?-->/gm;
function isComment(item) {
    return (typeof item === 'string' &&
        commentRE.test(item) &&
        !item.replace(commentRE, '').trim());
}

function ssrInterpolate(value) {
    return escapeHtml(toDisplayString(value));
}

function toRaw(observed) {
    const raw = observed && observed["__v_raw" /* RAW */];
    return raw ? toRaw(raw) : observed;
}

function isRef(r) {
    return !!(r && r.__v_isRef === true);
}

const stack = [];
function pushWarningContext(vnode) {
    stack.push(vnode);
}
function popWarningContext() {
    stack.pop();
}
function warn(msg, ...args) {
    const instance = stack.length ? stack[stack.length - 1].component : null;
    const appWarnHandler = instance && instance.appContext.config.warnHandler;
    const trace = getComponentTrace();
    if (appWarnHandler) {
        callWithErrorHandling(appWarnHandler, instance, 11 /* APP_WARN_HANDLER */, [
            msg + args.join(''),
            instance && instance.proxy,
            trace
                .map(({ vnode }) => `at <${formatComponentName(instance, vnode.type)}>`)
                .join('\n'),
            trace
        ]);
    }
    else {
        const warnArgs = [`[Vue warn]: ${msg}`, ...args];
        /* istanbul ignore if */
        if (trace.length &&
            // avoid spamming console during tests
            !false) {
            warnArgs.push(`\n`, ...formatTrace(trace));
        }
        console.warn(...warnArgs);
    }
}
function getComponentTrace() {
    let currentVNode = stack[stack.length - 1];
    if (!currentVNode) {
        return [];
    }
    // we can't just use the stack because it will be incomplete during updates
    // that did not start from the root. Re-construct the parent chain using
    // instance parent pointers.
    const normalizedStack = [];
    while (currentVNode) {
        const last = normalizedStack[0];
        if (last && last.vnode === currentVNode) {
            last.recurseCount++;
        }
        else {
            normalizedStack.push({
                vnode: currentVNode,
                recurseCount: 0
            });
        }
        const parentInstance = currentVNode.component && currentVNode.component.parent;
        currentVNode = parentInstance && parentInstance.vnode;
    }
    return normalizedStack;
}
/* istanbul ignore next */
function formatTrace(trace) {
    const logs = [];
    trace.forEach((entry, i) => {
        logs.push(...(i === 0 ? [] : [`\n`]), ...formatTraceEntry(entry));
    });
    return logs;
}
function formatTraceEntry({ vnode, recurseCount }) {
    const postfix = recurseCount > 0 ? `... (${recurseCount} recursive calls)` : ``;
    const isRoot = vnode.component ? vnode.component.parent == null : false;
    const open = ` at <${formatComponentName(vnode.component, vnode.type, isRoot)}`;
    const close = `>` + postfix;
    return vnode.props
        ? [open, ...formatProps(vnode.props), close]
        : [open + close];
}
/* istanbul ignore next */
function formatProps(props) {
    const res = [];
    const keys = Object.keys(props);
    keys.slice(0, 3).forEach(key => {
        res.push(...formatProp(key, props[key]));
    });
    if (keys.length > 3) {
        res.push(` ...`);
    }
    return res;
}
/* istanbul ignore next */
function formatProp(key, value, raw) {
    if (isString(value)) {
        value = JSON.stringify(value);
        return raw ? value : [`${key}=${value}`];
    }
    else if (typeof value === 'number' ||
        typeof value === 'boolean' ||
        value == null) {
        return raw ? value : [`${key}=${value}`];
    }
    else if (isRef(value)) {
        value = formatProp(key, toRaw(value.value), true);
        return raw ? value : [`${key}=Ref<`, value, `>`];
    }
    else if (isFunction(value)) {
        return [`${key}=fn${value.name ? `<${value.name}>` : ``}`];
    }
    else {
        value = toRaw(value);
        return raw ? value : [`${key}=`, value];
    }
}

const ErrorTypeStrings = {
    ["sp" /* SERVER_PREFETCH */]: 'serverPrefetch hook',
    ["bc" /* BEFORE_CREATE */]: 'beforeCreate hook',
    ["c" /* CREATED */]: 'created hook',
    ["bm" /* BEFORE_MOUNT */]: 'beforeMount hook',
    ["m" /* MOUNTED */]: 'mounted hook',
    ["bu" /* BEFORE_UPDATE */]: 'beforeUpdate hook',
    ["u" /* UPDATED */]: 'updated',
    ["bum" /* BEFORE_UNMOUNT */]: 'beforeUnmount hook',
    ["um" /* UNMOUNTED */]: 'unmounted hook',
    ["a" /* ACTIVATED */]: 'activated hook',
    ["da" /* DEACTIVATED */]: 'deactivated hook',
    ["ec" /* ERROR_CAPTURED */]: 'errorCaptured hook',
    ["rtc" /* RENDER_TRACKED */]: 'renderTracked hook',
    ["rtg" /* RENDER_TRIGGERED */]: 'renderTriggered hook',
    [0 /* SETUP_FUNCTION */]: 'setup function',
    [1 /* RENDER_FUNCTION */]: 'render function',
    [2 /* WATCH_GETTER */]: 'watcher getter',
    [3 /* WATCH_CALLBACK */]: 'watcher callback',
    [4 /* WATCH_CLEANUP */]: 'watcher cleanup function',
    [5 /* NATIVE_EVENT_HANDLER */]: 'native event handler',
    [6 /* COMPONENT_EVENT_HANDLER */]: 'component event handler',
    [7 /* VNODE_HOOK */]: 'vnode hook',
    [8 /* DIRECTIVE_HOOK */]: 'directive hook',
    [9 /* TRANSITION_HOOK */]: 'transition hook',
    [10 /* APP_ERROR_HANDLER */]: 'app errorHandler',
    [11 /* APP_WARN_HANDLER */]: 'app warnHandler',
    [12 /* FUNCTION_REF */]: 'ref function',
    [13 /* ASYNC_COMPONENT_LOADER */]: 'async component loader',
    [14 /* SCHEDULER */]: 'scheduler flush. This is likely a Vue internals bug. ' +
        'Please open an issue at https://new-issue.vuejs.org/?repo=vuejs/core'
};
function callWithErrorHandling(fn, instance, type, args) {
    let res;
    try {
        res = args ? fn(...args) : fn();
    }
    catch (err) {
        handleError(err, instance, type);
    }
    return res;
}
function handleError(err, instance, type, throwInDev = true) {
    const contextVNode = instance ? instance.vnode : null;
    if (instance) {
        let cur = instance.parent;
        // the exposed instance is the render proxy to keep it consistent with 2.x
        const exposedInstance = instance.proxy;
        // in production the hook receives only the error code
        const errorInfo = (process.env.NODE_ENV !== 'production') ? ErrorTypeStrings[type] : type;
        while (cur) {
            const errorCapturedHooks = cur.ec;
            if (errorCapturedHooks) {
                for (let i = 0; i < errorCapturedHooks.length; i++) {
                    if (errorCapturedHooks[i](err, exposedInstance, errorInfo) === false) {
                        return;
                    }
                }
            }
            cur = cur.parent;
        }
        // app-level handling
        const appErrorHandler = instance.appContext.config.errorHandler;
        if (appErrorHandler) {
            callWithErrorHandling(appErrorHandler, null, 10 /* APP_ERROR_HANDLER */, [err, exposedInstance, errorInfo]);
            return;
        }
    }
    logError(err, type, contextVNode, throwInDev);
}
function logError(err, type, contextVNode, throwInDev = true) {
    if ((process.env.NODE_ENV !== 'production')) {
        const info = ErrorTypeStrings[type];
        if (contextVNode) {
            pushWarningContext(contextVNode);
        }
        warn(`Unhandled error${info ? ` during execution of ${info}` : ``}`);
        if (contextVNode) {
            popWarningContext();
        }
        // crash in dev by default so it's more noticeable
        if (throwInDev) {
            throw err;
        }
        else {
            console.error(err);
        }
    }
    else {
        // recover in prod to reduce the impact on end-user
        console.error(err);
    }
}

const classifyRE = /(?:^|[-_])(\w)/g;
const classify = (str) => str.replace(classifyRE, c => c.toUpperCase()).replace(/[-_]/g, '');
function getComponentName(Component, includeInferred = true) {
    return isFunction(Component)
        ? Component.displayName || Component.name
        : Component.name || (includeInferred && Component.__name);
}
/* istanbul ignore next */
function formatComponentName(instance, Component, isRoot = false) {
    let name = getComponentName(Component);
    if (!name && Component.__file) {
        const match = Component.__file.match(/([^/\\]+)\.\w+$/);
        if (match) {
            name = match[1];
        }
    }
    if (!name && instance && instance.parent) {
        // try to infer the name based on reverse resolution
        const inferFromRegistry = (registry) => {
            for (const key in registry) {
                if (registry[key] === Component) {
                    return key;
                }
            }
        };
        name =
            inferFromRegistry(instance.components ||
                instance.parent.type.components) || inferFromRegistry(instance.appContext.components);
    }
    return name ? classify(name) : isRoot ? `App` : `Anonymous`;
}

function ssrRenderList(source, renderItem) {
    if (isArray(source) || isString(source)) {
        for (let i = 0, l = source.length; i < l; i++) {
            renderItem(source[i], i);
        }
    }
    else if (typeof source === 'number') {
        if ((process.env.NODE_ENV !== 'production') && !Number.isInteger(source)) {
            warn(`The v-for range expect an integer value but got ${source}.`);
            return;
        }
        for (let i = 0; i < source; i++) {
            renderItem(i + 1, i);
        }
    }
    else if (isObject(source)) {
        if (source[Symbol.iterator]) {
            const arr = Array.from(source);
            for (let i = 0, l = arr.length; i < l; i++) {
                renderItem(arr[i], i);
            }
        }
        else {
            const keys = Object.keys(source);
            for (let i = 0, l = keys.length; i < l; i++) {
                const key = keys[i];
                renderItem(source[key], key, i);
            }
        }
    }
}

async function ssrRenderSuspense(push, { default: renderContent }) {
    if (renderContent) {
        renderContent();
    }
    else {
        push(`<!---->`);
    }
}

function ssrGetDirectiveProps(instance, dir, value, arg, modifiers = {}) {
    if (typeof dir !== 'function' && dir.getSSRProps) {
        return (dir.getSSRProps({
            dir,
            instance,
            value,
            oldValue: undefined,
            arg,
            modifiers
        }, null) || {});
    }
    return {};
}

const ssrLooseEqual = looseEqual;
function ssrLooseContain(arr, value) {
    return looseIndexOf(arr, value) > -1;
}
// for <input :type="type" v-model="model" value="value">
function ssrRenderDynamicModel(type, model, value) {
    switch (type) {
        case 'radio':
            return looseEqual(model, value) ? ' checked' : '';
        case 'checkbox':
            return (isArray(model) ? ssrLooseContain(model, value) : model)
                ? ' checked'
                : '';
        default:
            // text types
            return ssrRenderAttr('value', model);
    }
}
// for <input v-bind="obj" v-model="model">
function ssrGetDynamicModelProps(existingProps = {}, model) {
    const { type, value } = existingProps;
    switch (type) {
        case 'radio':
            return looseEqual(model, value) ? { checked: true } : null;
        case 'checkbox':
            return (isArray(model) ? ssrLooseContain(model, value) : model)
                ? { checked: true }
                : null;
        default:
            // text types
            return { value: model };
    }
}

initDirectivesForSSR();

export { pipeToNodeWritable, pipeToWebWritable, renderToNodeStream, renderToSimpleStream, renderToStream, renderToString, renderToWebStream, ssrGetDirectiveProps, ssrGetDynamicModelProps, ssrInterpolate, ssrLooseContain, ssrLooseEqual, ssrRenderAttr, ssrRenderAttrs, ssrRenderClass, ssrRenderComponent, ssrRenderDynamicAttr, ssrRenderDynamicModel, ssrRenderList, ssrRenderSlot, ssrRenderSlotInner, ssrRenderStyle, ssrRenderSuspense, ssrRenderTeleport, renderVNode as ssrRenderVNode };

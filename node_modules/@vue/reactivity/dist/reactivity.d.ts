import { IfAny } from '@vue/shared';

declare type BaseTypes = string | number | boolean;

declare type Builtin = Primitive | Function | Date | Error | RegExp;

declare type CollectionTypes = IterableCollections | WeakCollections;

export declare function computed<T>(getter: ComputedGetter<T>, debugOptions?: DebuggerOptions): ComputedRef<T>;

export declare function computed<T>(options: WritableComputedOptions<T>, debugOptions?: DebuggerOptions): WritableComputedRef<T>;

export declare type ComputedGetter<T> = (...args: any[]) => T;

export declare interface ComputedRef<T = any> extends WritableComputedRef<T> {
    readonly value: T;
    [ComputedRefSymbol]: true;
}

declare class ComputedRefImpl<T> {
    private readonly _setter;
    dep?: Dep;
    private _value;
    readonly effect: ReactiveEffect<T>;
    readonly __v_isRef = true;
    readonly [ReactiveFlags.IS_READONLY]: boolean;
    _dirty: boolean;
    _cacheable: boolean;
    constructor(getter: ComputedGetter<T>, _setter: ComputedSetter<T>, isReadonly: boolean, isSSR: boolean);
    get value(): T;
    set value(newValue: T);
}

declare const ComputedRefSymbol: unique symbol;

export declare type ComputedSetter<T> = (v: T) => void;

export declare function customRef<T>(factory: CustomRefFactory<T>): Ref<T>;

export declare type CustomRefFactory<T> = (track: () => void, trigger: () => void) => {
    get: () => T;
    set: (value: T) => void;
};

export declare type DebuggerEvent = {
    effect: ReactiveEffect;
} & DebuggerEventExtraInfo;

export declare type DebuggerEventExtraInfo = {
    target: object;
    type: TrackOpTypes | TriggerOpTypes;
    key: any;
    newValue?: any;
    oldValue?: any;
    oldTarget?: Map<any, any> | Set<any>;
};

export declare interface DebuggerOptions {
    onTrack?: (event: DebuggerEvent) => void;
    onTrigger?: (event: DebuggerEvent) => void;
}

export declare type DeepReadonly<T> = T extends Builtin ? T : T extends Map<infer K, infer V> ? ReadonlyMap<DeepReadonly<K>, DeepReadonly<V>> : T extends ReadonlyMap<infer K, infer V> ? ReadonlyMap<DeepReadonly<K>, DeepReadonly<V>> : T extends WeakMap<infer K, infer V> ? WeakMap<DeepReadonly<K>, DeepReadonly<V>> : T extends Set<infer U> ? ReadonlySet<DeepReadonly<U>> : T extends ReadonlySet<infer U> ? ReadonlySet<DeepReadonly<U>> : T extends WeakSet<infer U> ? WeakSet<DeepReadonly<U>> : T extends Promise<infer U> ? Promise<DeepReadonly<U>> : T extends Ref<infer U> ? Readonly<Ref<DeepReadonly<U>>> : T extends {} ? {
    readonly [K in keyof T]: DeepReadonly<T[K]>;
} : Readonly<T>;

export declare function deferredComputed<T>(getter: () => T): ComputedRef<T>;

declare type Dep = Set<ReactiveEffect> & TrackedMarkers;

export declare function effect<T = any>(fn: () => T, options?: ReactiveEffectOptions): ReactiveEffectRunner;

export declare type EffectScheduler = (...args: any[]) => any;

export declare class EffectScope {
    /* Excluded from this release type: active */
    /* Excluded from this release type: effects */
    /* Excluded from this release type: cleanups */
    /* Excluded from this release type: parent */
    /* Excluded from this release type: scopes */
    /* Excluded from this release type: index */
    constructor(detached?: boolean);
    run<T>(fn: () => T): T | undefined;
    /* Excluded from this release type: on */
    /* Excluded from this release type: off */
    stop(fromParent?: boolean): void;
}

export declare function effectScope(detached?: boolean): EffectScope;

export declare function enableTracking(): void;

export declare function getCurrentScope(): EffectScope | undefined;

export declare function isProxy(value: unknown): boolean;

export declare function isReactive(value: unknown): boolean;

export declare function isReadonly(value: unknown): boolean;

export declare function isRef<T>(r: Ref<T> | unknown): r is Ref<T>;

export declare function isShallow(value: unknown): boolean;

declare type IterableCollections = Map<any, any> | Set<any>;

export declare const ITERATE_KEY: unique symbol;

export declare function markRaw<T extends object>(value: T): T & {
    [RawSymbol]?: true;
};

export declare function onScopeDispose(fn: () => void): void;

export declare function pauseTracking(): void;

declare type Primitive = string | number | boolean | bigint | symbol | undefined | null;

export declare function proxyRefs<T extends object>(objectWithRefs: T): ShallowUnwrapRef<T>;

declare const RawSymbol: unique symbol;

/**
 * Creates a reactive copy of the original object.
 *
 * The reactive conversion is "deep"—it affects all nested properties. In the
 * ES2015 Proxy based implementation, the returned proxy is **not** equal to the
 * original object. It is recommended to work exclusively with the reactive
 * proxy and avoid relying on the original object.
 *
 * A reactive object also automatically unwraps refs contained in it, so you
 * don't need to use `.value` when accessing and mutating their value:
 *
 * ```js
 * const count = ref(0)
 * const obj = reactive({
 *   count
 * })
 *
 * obj.count++
 * obj.count // -> 1
 * count.value // -> 1
 * ```
 */
export declare function reactive<T extends object>(target: T): UnwrapNestedRefs<T>;

export declare class ReactiveEffect<T = any> {
    fn: () => T;
    scheduler: EffectScheduler | null;
    active: boolean;
    deps: Dep[];
    parent: ReactiveEffect | undefined;
    /* Excluded from this release type: computed */
    /* Excluded from this release type: allowRecurse */
    /* Excluded from this release type: deferStop */
    onStop?: () => void;
    onTrack?: (event: DebuggerEvent) => void;
    onTrigger?: (event: DebuggerEvent) => void;
    constructor(fn: () => T, scheduler?: EffectScheduler | null, scope?: EffectScope);
    run(): T | undefined;
    stop(): void;
}

export declare interface ReactiveEffectOptions extends DebuggerOptions {
    lazy?: boolean;
    scheduler?: EffectScheduler;
    scope?: EffectScope;
    allowRecurse?: boolean;
    onStop?: () => void;
}

export declare interface ReactiveEffectRunner<T = any> {
    (): T;
    effect: ReactiveEffect;
}

export declare const enum ReactiveFlags {
    SKIP = "__v_skip",
    IS_REACTIVE = "__v_isReactive",
    IS_READONLY = "__v_isReadonly",
    IS_SHALLOW = "__v_isShallow",
    RAW = "__v_raw"
}

/**
 * Creates a readonly copy of the original object. Note the returned copy is not
 * made reactive, but `readonly` can be called on an already reactive object.
 */
export declare function readonly<T extends object>(target: T): DeepReadonly<UnwrapNestedRefs<T>>;

export declare interface Ref<T = any> {
    value: T;
    /**
     * Type differentiator only.
     * We need this to be in public d.ts but don't want it to show up in IDE
     * autocomplete, so we use a private Symbol instead.
     */
    [RefSymbol]: true;
}

export declare function ref<T extends object>(value: T): [T] extends [Ref] ? T : Ref<UnwrapRef<T>>;

export declare function ref<T>(value: T): Ref<UnwrapRef<T>>;

export declare function ref<T = any>(): Ref<T | undefined>;

declare const RefSymbol: unique symbol;

/**
 * This is a special exported interface for other packages to declare
 * additional types that should bail out for ref unwrapping. For example
 * \@vue/runtime-dom can declare it like so in its d.ts:
 *
 * ``` ts
 * declare module '@vue/reactivity' {
 *   export interface RefUnwrapBailTypes {
 *     runtimeDOMBailTypes: Node | Window
 *   }
 * }
 * ```
 *
 * Note that api-extractor somehow refuses to include `declare module`
 * augmentations in its generated d.ts, so we have to manually append them
 * to the final generated d.ts in our build process.
 */
export declare interface RefUnwrapBailTypes {
}

export declare function resetTracking(): void;

export declare type ShallowReactive<T> = T & {
    [ShallowReactiveMarker]?: true;
};

/**
 * Return a shallowly-reactive copy of the original object, where only the root
 * level properties are reactive. It also does not auto-unwrap refs (even at the
 * root level).
 */
export declare function shallowReactive<T extends object>(target: T): ShallowReactive<T>;

declare const ShallowReactiveMarker: unique symbol;

/**
 * Returns a reactive-copy of the original object, where only the root level
 * properties are readonly, and does NOT unwrap refs nor recursively convert
 * returned properties.
 * This is used for creating the props proxy object for stateful components.
 */
export declare function shallowReadonly<T extends object>(target: T): Readonly<T>;

export declare type ShallowRef<T = any> = Ref<T> & {
    [ShallowRefMarker]?: true;
};

export declare function shallowRef<T extends object>(value: T): T extends Ref ? T : ShallowRef<T>;

export declare function shallowRef<T>(value: T): ShallowRef<T>;

export declare function shallowRef<T = any>(): ShallowRef<T | undefined>;

declare const ShallowRefMarker: unique symbol;

export declare type ShallowUnwrapRef<T> = {
    [K in keyof T]: T[K] extends Ref<infer V> ? V : T[K] extends Ref<infer V> | undefined ? unknown extends V ? undefined : V | undefined : T[K];
};

declare function stop_2(runner: ReactiveEffectRunner): void;
export { stop_2 as stop }

export declare function toRaw<T>(observed: T): T;

export declare type ToRef<T> = IfAny<T, Ref<T>, [T] extends [Ref] ? T : Ref<T>>;

export declare function toRef<T extends object, K extends keyof T>(object: T, key: K): ToRef<T[K]>;

export declare function toRef<T extends object, K extends keyof T>(object: T, key: K, defaultValue: T[K]): ToRef<Exclude<T[K], undefined>>;

export declare type ToRefs<T = any> = {
    [K in keyof T]: ToRef<T[K]>;
};

export declare function toRefs<T extends object>(object: T): ToRefs<T>;

export declare function track(target: object, type: TrackOpTypes, key: unknown): void;

/**
 * wasTracked and newTracked maintain the status for several levels of effect
 * tracking recursion. One bit per level is used to define whether the dependency
 * was/is tracked.
 */
declare type TrackedMarkers = {
    /**
     * wasTracked
     */
    w: number;
    /**
     * newTracked
     */
    n: number;
};

export declare const enum TrackOpTypes {
    GET = "get",
    HAS = "has",
    ITERATE = "iterate"
}

export declare function trigger(target: object, type: TriggerOpTypes, key?: unknown, newValue?: unknown, oldValue?: unknown, oldTarget?: Map<unknown, unknown> | Set<unknown>): void;

export declare const enum TriggerOpTypes {
    SET = "set",
    ADD = "add",
    DELETE = "delete",
    CLEAR = "clear"
}

export declare function triggerRef(ref: Ref): void;

export declare function unref<T>(ref: T | Ref<T>): T;

export declare type UnwrapNestedRefs<T> = T extends Ref ? T : UnwrapRefSimple<T>;

export declare type UnwrapRef<T> = T extends ShallowRef<infer V> ? V : T extends Ref<infer V> ? UnwrapRefSimple<V> : UnwrapRefSimple<T>;

declare type UnwrapRefSimple<T> = T extends Function | CollectionTypes | BaseTypes | Ref | RefUnwrapBailTypes[keyof RefUnwrapBailTypes] | {
    [RawSymbol]?: true;
} ? T : T extends Array<any> ? {
    [K in keyof T]: UnwrapRefSimple<T[K]>;
} : T extends object & {
    [ShallowReactiveMarker]?: never;
} ? {
    [P in keyof T]: P extends symbol ? T[P] : UnwrapRef<T[P]>;
} : T;

declare type WeakCollections = WeakMap<any, any> | WeakSet<any>;

export declare interface WritableComputedOptions<T> {
    get: ComputedGetter<T>;
    set: ComputedSetter<T>;
}

export declare interface WritableComputedRef<T> extends Ref<T> {
    readonly effect: ReactiveEffect<T>;
}

export { }

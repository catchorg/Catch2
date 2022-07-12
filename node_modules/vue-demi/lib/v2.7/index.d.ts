import Vue from 'vue'
import type { PluginFunction, PluginObject } from 'vue'
declare const isVue2: boolean
declare const isVue3: boolean
declare const Vue2: Vue | undefined
declare const version: string
declare const install: (vue?: Vue) => void
/**
 * @deprecated To avoid bringing in all the tree-shakable modules, this API has been deprecated. Use `Vue2` or named exports instead.
 * Refer to https://github.com/vueuse/vue-demi/issues/41
 */
declare const V: Vue

// accept no generic because Vue 3 doesn't accept any
// https://github.com/vuejs/vue-next/pull/2758/
export declare type Plugin = PluginObject<any> | PluginFunction<any>
export type { VNode } from 'vue'
export * from 'vue'
export { V as Vue, Vue2, isVue2, isVue3, version, install }

import Vue, { VueConstructor, VNode, VNodeDirective } from 'vue'

// #region createApp polyfill
export type DirectiveModifiers = Record<string, boolean>
export interface DirectiveBinding<V> extends Readonly<VNodeDirective> {
  readonly modifiers: DirectiveModifiers
  readonly value: V
  readonly oldValue: V | null
}
export type DirectiveHook<T = any, Prev = VNode | null, V = any> = (el: T, binding: DirectiveBinding<V>, vnode: VNode, prevVNode: Prev) => void
export interface ObjectDirective<T = any, V = any> {
  bind?: DirectiveHook<T, any, V>
  inserted?: DirectiveHook<T, any, V>
  update?: DirectiveHook<T, any, V>
  componentUpdated?: DirectiveHook<T, any, V>
  unbind?: DirectiveHook<T, any, V>
}
export type FunctionDirective<T = any, V = any> = DirectiveHook<T, any, V>
export type Directive<T = any, V = any> = ObjectDirective<T, V> | FunctionDirective<T, V>
export interface InjectionKey<T> extends Symbol {}
export interface App<T = any> {
  config: VueConstructor['config']
  use: VueConstructor['use']
  mixin: VueConstructor['mixin']
  component: VueConstructor['component']
  directive(name: string): Directive | undefined
  directive(name: string, directive: Directive): this
  provide<T>(key: InjectionKey<T> | symbol | string, value: T): this
  mount: Vue['$mount']
  unmount: Vue['$destroy']
}
export declare function createApp(rootComponent: any, rootProps?: any): App
// #endregion

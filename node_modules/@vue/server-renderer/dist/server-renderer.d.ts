/// <reference types="node" />

import { App } from 'vue';
import { Component } from 'vue';
import { ComponentInternalInstance } from 'vue';
import { ComponentPublicInstance } from '@vue/runtime-core';
import { Directive } from '@vue/runtime-core';
import { Readable } from 'stream';
import { Slots } from 'vue';
import { includeBooleanAttr as ssrIncludeBooleanAttr } from '@vue/shared';
import { VNode } from 'vue';
import { Writable } from 'stream';

export declare function pipeToNodeWritable(input: App | VNode, context: SSRContext | undefined, writable: Writable): void;

export declare function pipeToWebWritable(input: App | VNode, context: SSRContext | undefined, writable: WritableStream): void;

declare type Props = Record<string, unknown>;

declare type PushFn = (item: SSRBufferItem) => void;

export declare function renderToNodeStream(input: App | VNode, context?: SSRContext): Readable;

export declare function renderToSimpleStream<T extends SimpleReadable>(input: App | VNode, context: SSRContext, stream: T): T;

/**
 * @deprecated
 */
export declare function renderToStream(input: App | VNode, context?: SSRContext): Readable;

export declare function renderToString(input: App | VNode, context?: SSRContext): Promise<string>;

export declare function renderToWebStream(input: App | VNode, context?: SSRContext): ReadableStream;

export declare interface SimpleReadable {
    push(chunk: string | null): void;
    destroy(err: any): void;
}

declare type SSRBuffer = SSRBufferItem[] & {
    hasAsync?: boolean;
};

declare type SSRBufferItem = string | SSRBuffer | Promise<SSRBuffer>;

export declare type SSRContext = {
    [key: string]: any;
    teleports?: Record<string, string>;
    __teleportBuffers?: Record<string, SSRBuffer>;
};

export declare function ssrGetDirectiveProps(instance: ComponentPublicInstance, dir: Directive, value?: any, arg?: string, modifiers?: Record<string, boolean>): Record<string, any>;

export declare function ssrGetDynamicModelProps(existingProps: any, model: unknown): {
    checked: boolean;
    value?: undefined;
} | {
    value: unknown;
    checked?: undefined;
} | null;

export { ssrIncludeBooleanAttr }

export declare function ssrInterpolate(value: unknown): string;

export declare function ssrLooseContain(arr: unknown[], value: unknown): boolean;

export declare const ssrLooseEqual: (a: unknown, b: unknown) => boolean;

export declare function ssrRenderAttr(key: string, value: unknown): string;

export declare function ssrRenderAttrs(props: Record<string, unknown>, tag?: string): string;

export declare function ssrRenderClass(raw: unknown): string;

export declare function ssrRenderComponent(comp: Component, props?: Props | null, children?: Slots | SSRSlots | null, parentComponent?: ComponentInternalInstance | null, slotScopeId?: string): SSRBuffer | Promise<SSRBuffer>;

export declare function ssrRenderDynamicAttr(key: string, value: unknown, tag?: string): string;

export declare function ssrRenderDynamicModel(type: unknown, model: unknown, value: unknown): string;

export declare function ssrRenderList(source: unknown, renderItem: (value: unknown, key: string | number, index?: number) => void): void;

export declare function ssrRenderSlot(slots: Slots | SSRSlots, slotName: string, slotProps: Props, fallbackRenderFn: (() => void) | null, push: PushFn, parentComponent: ComponentInternalInstance, slotScopeId?: string): void;

export declare function ssrRenderSlotInner(slots: Slots | SSRSlots, slotName: string, slotProps: Props, fallbackRenderFn: (() => void) | null, push: PushFn, parentComponent: ComponentInternalInstance, slotScopeId?: string, transition?: boolean): void;

export declare function ssrRenderStyle(raw: unknown): string;

export declare function ssrRenderSuspense(push: PushFn, { default: renderContent }: Record<string, (() => void) | undefined>): Promise<void>;

export declare function ssrRenderTeleport(parentPush: PushFn, contentRenderFn: (push: PushFn) => void, target: string, disabled: boolean, parentComponent: ComponentInternalInstance): void;

export declare function ssrRenderVNode(push: PushFn, vnode: VNode, parentComponent: ComponentInternalInstance, slotScopeId?: string): void;

declare type SSRSlot = (props: Props, push: PushFn, parentComponent: ComponentInternalInstance | null, scopeId: string | null) => void;

declare type SSRSlots = Record<string, SSRSlot>;

export { }

/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/

// Defined a subset of ES6 built ins that run in IE11
// CHECK WITH http://kangax.github.io/compat-table/es6/#ie11

interface Map<K, V> {
	clear(): void;
	delete(key: K): boolean;
	forEach(callbackfn: (value: V, index: K, map: Map<K, V>) => void, thisArg?: any): void;
	get(key: K): V | undefined;
	has(key: K): boolean;
	set(key: K, value?: V): Map<K, V>;
	readonly size: number;

	// not supported on IE11:
	// entries(): IterableIterator<[K, V]>;
	// keys(): IterableIterator<K>;
	// values(): IterableIterator<V>;
	// [Symbol.iterator]():IterableIterator<[K,V]>;
	// [Symbol.toStringTag]: string;
}

interface MapConstructor {
	new <K, V>(): Map<K, V>;

	// not supported on IE11:
	// new <K, V>(iterable: Iterable<[K, V]>): Map<K, V>;
}
declare var Map: MapConstructor;


interface Set<T> {
	add(value: T): Set<T>;
	clear(): void;
	delete(value: T): boolean;
	forEach(callbackfn: (value: T, index: T, set: Set<T>) => void, thisArg?: any): void;
	has(value: T): boolean;
	readonly size: number;

	// not supported on IE11:
	// entries(): IterableIterator<[T, T]>;
	// keys(): IterableIterator<T>;
	// values(): IterableIterator<T>;
	// [Symbol.iterator]():IterableIterator<T>;
	// [Symbol.toStringTag]: string;
}

interface SetConstructor {
	new <T>(): Set<T>;

	// not supported on IE11:
	// new <T>(iterable: Iterable<T>): Set<T>;
}
declare var Set: SetConstructor;


interface WeakMap<K extends object, V> {
	delete(key: K): boolean;
	get(key: K): V | undefined;
	has(key: K): boolean;
	// IE11 doesn't return this
	// set(key: K, value?: V): this;
	set(key: K, value?: V): undefined;
}

interface WeakMapConstructor {
	new(): WeakMap<any, any>;
	new <K extends object, V>(): WeakMap<K, V>;
	// new <K, V>(entries?: [K, V][]): WeakMap<K, V>;
	readonly prototype: WeakMap<object, any>;
}
declare var WeakMap: WeakMapConstructor;

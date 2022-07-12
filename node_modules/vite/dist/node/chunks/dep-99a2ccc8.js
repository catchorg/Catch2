'use strict';

var index = require('./dep-c9998dc6.js');
var require$$1 = require('crypto');
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
require('buffer');
require('querystring');
require('zlib');
require('https');
require('tls');
require('worker_threads');
require('readline');

function _interopDefaultLegacy (e) { return e && typeof e === 'object' && 'default' in e ? e["default"] : e; }

var require$$1__default = /*#__PURE__*/_interopDefaultLegacy(require$$1);

/**
 * Node.js module for Forge.
 *
 * @author Dave Longley
 *
 * Copyright 2011-2016 Digital Bazaar, Inc.
 */

var forge$s = {
  // default options
  options: {
    usePureJavaScript: false
  }
};

/**
 * Base-N/Base-X encoding/decoding functions.
 *
 * Original implementation from base-x:
 * https://github.com/cryptocoinjs/base-x
 *
 * Which is MIT licensed:
 *
 * The MIT License (MIT)
 *
 * Copyright base-x contributors (c) 2016
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

var api = {};
var baseN$1 = api;

// baseN alphabet indexes
var _reverseAlphabets = {};

/**
 * BaseN-encodes a Uint8Array using the given alphabet.
 *
 * @param input the Uint8Array to encode.
 * @param maxline the maximum number of encoded characters per line to use,
 *          defaults to none.
 *
 * @return the baseN-encoded output string.
 */
api.encode = function(input, alphabet, maxline) {
  if(typeof alphabet !== 'string') {
    throw new TypeError('"alphabet" must be a string.');
  }
  if(maxline !== undefined && typeof maxline !== 'number') {
    throw new TypeError('"maxline" must be a number.');
  }

  var output = '';

  if(!(input instanceof Uint8Array)) {
    // assume forge byte buffer
    output = _encodeWithByteBuffer(input, alphabet);
  } else {
    var i = 0;
    var base = alphabet.length;
    var first = alphabet.charAt(0);
    var digits = [0];
    for(i = 0; i < input.length; ++i) {
      for(var j = 0, carry = input[i]; j < digits.length; ++j) {
        carry += digits[j] << 8;
        digits[j] = carry % base;
        carry = (carry / base) | 0;
      }

      while(carry > 0) {
        digits.push(carry % base);
        carry = (carry / base) | 0;
      }
    }

    // deal with leading zeros
    for(i = 0; input[i] === 0 && i < input.length - 1; ++i) {
      output += first;
    }
    // convert digits to a string
    for(i = digits.length - 1; i >= 0; --i) {
      output += alphabet[digits[i]];
    }
  }

  if(maxline) {
    var regex = new RegExp('.{1,' + maxline + '}', 'g');
    output = output.match(regex).join('\r\n');
  }

  return output;
};

/**
 * Decodes a baseN-encoded (using the given alphabet) string to a
 * Uint8Array.
 *
 * @param input the baseN-encoded input string.
 *
 * @return the Uint8Array.
 */
api.decode = function(input, alphabet) {
  if(typeof input !== 'string') {
    throw new TypeError('"input" must be a string.');
  }
  if(typeof alphabet !== 'string') {
    throw new TypeError('"alphabet" must be a string.');
  }

  var table = _reverseAlphabets[alphabet];
  if(!table) {
    // compute reverse alphabet
    table = _reverseAlphabets[alphabet] = [];
    for(var i = 0; i < alphabet.length; ++i) {
      table[alphabet.charCodeAt(i)] = i;
    }
  }

  // remove whitespace characters
  input = input.replace(/\s/g, '');

  var base = alphabet.length;
  var first = alphabet.charAt(0);
  var bytes = [0];
  for(var i = 0; i < input.length; i++) {
    var value = table[input.charCodeAt(i)];
    if(value === undefined) {
      return;
    }

    for(var j = 0, carry = value; j < bytes.length; ++j) {
      carry += bytes[j] * base;
      bytes[j] = carry & 0xff;
      carry >>= 8;
    }

    while(carry > 0) {
      bytes.push(carry & 0xff);
      carry >>= 8;
    }
  }

  // deal with leading zeros
  for(var k = 0; input[k] === first && k < input.length - 1; ++k) {
    bytes.push(0);
  }

  if(typeof Buffer !== 'undefined') {
    return Buffer.from(bytes.reverse());
  }

  return new Uint8Array(bytes.reverse());
};

function _encodeWithByteBuffer(input, alphabet) {
  var i = 0;
  var base = alphabet.length;
  var first = alphabet.charAt(0);
  var digits = [0];
  for(i = 0; i < input.length(); ++i) {
    for(var j = 0, carry = input.at(i); j < digits.length; ++j) {
      carry += digits[j] << 8;
      digits[j] = carry % base;
      carry = (carry / base) | 0;
    }

    while(carry > 0) {
      digits.push(carry % base);
      carry = (carry / base) | 0;
    }
  }

  var output = '';

  // deal with leading zeros
  for(i = 0; input.at(i) === 0 && i < input.length() - 1; ++i) {
    output += first;
  }
  // convert digits to a string
  for(i = digits.length - 1; i >= 0; --i) {
    output += alphabet[digits[i]];
  }

  return output;
}

/**
 * Utility functions for web applications.
 *
 * @author Dave Longley
 *
 * Copyright (c) 2010-2018 Digital Bazaar, Inc.
 */

var forge$r = forge$s;
var baseN = baseN$1;

/* Utilities API */
var util$1 = forge$r.util = forge$r.util || {};

// define setImmediate and nextTick
(function() {
  // use native nextTick (unless we're in webpack)
  // webpack (or better node-libs-browser polyfill) sets process.browser.
  // this way we can detect webpack properly
  if(typeof process !== 'undefined' && process.nextTick && !process.browser) {
    util$1.nextTick = process.nextTick;
    if(typeof setImmediate === 'function') {
      util$1.setImmediate = setImmediate;
    } else {
      // polyfill setImmediate with nextTick, older versions of node
      // (those w/o setImmediate) won't totally starve IO
      util$1.setImmediate = util$1.nextTick;
    }
    return;
  }

  // polyfill nextTick with native setImmediate
  if(typeof setImmediate === 'function') {
    util$1.setImmediate = function() { return setImmediate.apply(undefined, arguments); };
    util$1.nextTick = function(callback) {
      return setImmediate(callback);
    };
    return;
  }

  /* Note: A polyfill upgrade pattern is used here to allow combining
  polyfills. For example, MutationObserver is fast, but blocks UI updates,
  so it needs to allow UI updates periodically, so it falls back on
  postMessage or setTimeout. */

  // polyfill with setTimeout
  util$1.setImmediate = function(callback) {
    setTimeout(callback, 0);
  };

  // upgrade polyfill to use postMessage
  if(typeof window !== 'undefined' &&
    typeof window.postMessage === 'function') {
    var msg = 'forge.setImmediate';
    var callbacks = [];
    util$1.setImmediate = function(callback) {
      callbacks.push(callback);
      // only send message when one hasn't been sent in
      // the current turn of the event loop
      if(callbacks.length === 1) {
        window.postMessage(msg, '*');
      }
    };
    function handler(event) {
      if(event.source === window && event.data === msg) {
        event.stopPropagation();
        var copy = callbacks.slice();
        callbacks.length = 0;
        copy.forEach(function(callback) {
          callback();
        });
      }
    }
    window.addEventListener('message', handler, true);
  }

  // upgrade polyfill to use MutationObserver
  if(typeof MutationObserver !== 'undefined') {
    // polyfill with MutationObserver
    var now = Date.now();
    var attr = true;
    var div = document.createElement('div');
    var callbacks = [];
    new MutationObserver(function() {
      var copy = callbacks.slice();
      callbacks.length = 0;
      copy.forEach(function(callback) {
        callback();
      });
    }).observe(div, {attributes: true});
    var oldSetImmediate = util$1.setImmediate;
    util$1.setImmediate = function(callback) {
      if(Date.now() - now > 15) {
        now = Date.now();
        oldSetImmediate(callback);
      } else {
        callbacks.push(callback);
        // only trigger observer when it hasn't been triggered in
        // the current turn of the event loop
        if(callbacks.length === 1) {
          div.setAttribute('a', attr = !attr);
        }
      }
    };
  }

  util$1.nextTick = util$1.setImmediate;
})();

// check if running under Node.js
util$1.isNodejs =
  typeof process !== 'undefined' && process.versions && process.versions.node;


// 'self' will also work in Web Workers (instance of WorkerGlobalScope) while
// it will point to `window` in the main thread.
// To remain compatible with older browsers, we fall back to 'window' if 'self'
// is not available.
util$1.globalScope = (function() {
  if(util$1.isNodejs) {
    return index.commonjsGlobal;
  }

  return typeof self === 'undefined' ? window : self;
})();

// define isArray
util$1.isArray = Array.isArray || function(x) {
  return Object.prototype.toString.call(x) === '[object Array]';
};

// define isArrayBuffer
util$1.isArrayBuffer = function(x) {
  return typeof ArrayBuffer !== 'undefined' && x instanceof ArrayBuffer;
};

// define isArrayBufferView
util$1.isArrayBufferView = function(x) {
  return x && util$1.isArrayBuffer(x.buffer) && x.byteLength !== undefined;
};

/**
 * Ensure a bits param is 8, 16, 24, or 32. Used to validate input for
 * algorithms where bit manipulation, JavaScript limitations, and/or algorithm
 * design only allow for byte operations of a limited size.
 *
 * @param n number of bits.
 *
 * Throw Error if n invalid.
 */
function _checkBitsParam(n) {
  if(!(n === 8 || n === 16 || n === 24 || n === 32)) {
    throw new Error('Only 8, 16, 24, or 32 bits supported: ' + n);
  }
}

// TODO: set ByteBuffer to best available backing
util$1.ByteBuffer = ByteStringBuffer;

/** Buffer w/BinaryString backing */

/**
 * Constructor for a binary string backed byte buffer.
 *
 * @param [b] the bytes to wrap (either encoded as string, one byte per
 *          character, or as an ArrayBuffer or Typed Array).
 */
function ByteStringBuffer(b) {
  // TODO: update to match DataBuffer API

  // the data in this buffer
  this.data = '';
  // the pointer for reading from this buffer
  this.read = 0;

  if(typeof b === 'string') {
    this.data = b;
  } else if(util$1.isArrayBuffer(b) || util$1.isArrayBufferView(b)) {
    if(typeof Buffer !== 'undefined' && b instanceof Buffer) {
      this.data = b.toString('binary');
    } else {
      // convert native buffer to forge buffer
      // FIXME: support native buffers internally instead
      var arr = new Uint8Array(b);
      try {
        this.data = String.fromCharCode.apply(null, arr);
      } catch(e) {
        for(var i = 0; i < arr.length; ++i) {
          this.putByte(arr[i]);
        }
      }
    }
  } else if(b instanceof ByteStringBuffer ||
    (typeof b === 'object' && typeof b.data === 'string' &&
    typeof b.read === 'number')) {
    // copy existing buffer
    this.data = b.data;
    this.read = b.read;
  }

  // used for v8 optimization
  this._constructedStringLength = 0;
}
util$1.ByteStringBuffer = ByteStringBuffer;

/* Note: This is an optimization for V8-based browsers. When V8 concatenates
  a string, the strings are only joined logically using a "cons string" or
  "constructed/concatenated string". These containers keep references to one
  another and can result in very large memory usage. For example, if a 2MB
  string is constructed by concatenating 4 bytes together at a time, the
  memory usage will be ~44MB; so ~22x increase. The strings are only joined
  together when an operation requiring their joining takes place, such as
  substr(). This function is called when adding data to this buffer to ensure
  these types of strings are periodically joined to reduce the memory
  footprint. */
var _MAX_CONSTRUCTED_STRING_LENGTH = 4096;
util$1.ByteStringBuffer.prototype._optimizeConstructedString = function(x) {
  this._constructedStringLength += x;
  if(this._constructedStringLength > _MAX_CONSTRUCTED_STRING_LENGTH) {
    // this substr() should cause the constructed string to join
    this.data.substr(0, 1);
    this._constructedStringLength = 0;
  }
};

/**
 * Gets the number of bytes in this buffer.
 *
 * @return the number of bytes in this buffer.
 */
util$1.ByteStringBuffer.prototype.length = function() {
  return this.data.length - this.read;
};

/**
 * Gets whether or not this buffer is empty.
 *
 * @return true if this buffer is empty, false if not.
 */
util$1.ByteStringBuffer.prototype.isEmpty = function() {
  return this.length() <= 0;
};

/**
 * Puts a byte in this buffer.
 *
 * @param b the byte to put.
 *
 * @return this buffer.
 */
util$1.ByteStringBuffer.prototype.putByte = function(b) {
  return this.putBytes(String.fromCharCode(b));
};

/**
 * Puts a byte in this buffer N times.
 *
 * @param b the byte to put.
 * @param n the number of bytes of value b to put.
 *
 * @return this buffer.
 */
util$1.ByteStringBuffer.prototype.fillWithByte = function(b, n) {
  b = String.fromCharCode(b);
  var d = this.data;
  while(n > 0) {
    if(n & 1) {
      d += b;
    }
    n >>>= 1;
    if(n > 0) {
      b += b;
    }
  }
  this.data = d;
  this._optimizeConstructedString(n);
  return this;
};

/**
 * Puts bytes in this buffer.
 *
 * @param bytes the bytes (as a binary encoded string) to put.
 *
 * @return this buffer.
 */
util$1.ByteStringBuffer.prototype.putBytes = function(bytes) {
  this.data += bytes;
  this._optimizeConstructedString(bytes.length);
  return this;
};

/**
 * Puts a UTF-16 encoded string into this buffer.
 *
 * @param str the string to put.
 *
 * @return this buffer.
 */
util$1.ByteStringBuffer.prototype.putString = function(str) {
  return this.putBytes(util$1.encodeUtf8(str));
};

/**
 * Puts a 16-bit integer in this buffer in big-endian order.
 *
 * @param i the 16-bit integer.
 *
 * @return this buffer.
 */
util$1.ByteStringBuffer.prototype.putInt16 = function(i) {
  return this.putBytes(
    String.fromCharCode(i >> 8 & 0xFF) +
    String.fromCharCode(i & 0xFF));
};

/**
 * Puts a 24-bit integer in this buffer in big-endian order.
 *
 * @param i the 24-bit integer.
 *
 * @return this buffer.
 */
util$1.ByteStringBuffer.prototype.putInt24 = function(i) {
  return this.putBytes(
    String.fromCharCode(i >> 16 & 0xFF) +
    String.fromCharCode(i >> 8 & 0xFF) +
    String.fromCharCode(i & 0xFF));
};

/**
 * Puts a 32-bit integer in this buffer in big-endian order.
 *
 * @param i the 32-bit integer.
 *
 * @return this buffer.
 */
util$1.ByteStringBuffer.prototype.putInt32 = function(i) {
  return this.putBytes(
    String.fromCharCode(i >> 24 & 0xFF) +
    String.fromCharCode(i >> 16 & 0xFF) +
    String.fromCharCode(i >> 8 & 0xFF) +
    String.fromCharCode(i & 0xFF));
};

/**
 * Puts a 16-bit integer in this buffer in little-endian order.
 *
 * @param i the 16-bit integer.
 *
 * @return this buffer.
 */
util$1.ByteStringBuffer.prototype.putInt16Le = function(i) {
  return this.putBytes(
    String.fromCharCode(i & 0xFF) +
    String.fromCharCode(i >> 8 & 0xFF));
};

/**
 * Puts a 24-bit integer in this buffer in little-endian order.
 *
 * @param i the 24-bit integer.
 *
 * @return this buffer.
 */
util$1.ByteStringBuffer.prototype.putInt24Le = function(i) {
  return this.putBytes(
    String.fromCharCode(i & 0xFF) +
    String.fromCharCode(i >> 8 & 0xFF) +
    String.fromCharCode(i >> 16 & 0xFF));
};

/**
 * Puts a 32-bit integer in this buffer in little-endian order.
 *
 * @param i the 32-bit integer.
 *
 * @return this buffer.
 */
util$1.ByteStringBuffer.prototype.putInt32Le = function(i) {
  return this.putBytes(
    String.fromCharCode(i & 0xFF) +
    String.fromCharCode(i >> 8 & 0xFF) +
    String.fromCharCode(i >> 16 & 0xFF) +
    String.fromCharCode(i >> 24 & 0xFF));
};

/**
 * Puts an n-bit integer in this buffer in big-endian order.
 *
 * @param i the n-bit integer.
 * @param n the number of bits in the integer (8, 16, 24, or 32).
 *
 * @return this buffer.
 */
util$1.ByteStringBuffer.prototype.putInt = function(i, n) {
  _checkBitsParam(n);
  var bytes = '';
  do {
    n -= 8;
    bytes += String.fromCharCode((i >> n) & 0xFF);
  } while(n > 0);
  return this.putBytes(bytes);
};

/**
 * Puts a signed n-bit integer in this buffer in big-endian order. Two's
 * complement representation is used.
 *
 * @param i the n-bit integer.
 * @param n the number of bits in the integer (8, 16, 24, or 32).
 *
 * @return this buffer.
 */
util$1.ByteStringBuffer.prototype.putSignedInt = function(i, n) {
  // putInt checks n
  if(i < 0) {
    i += 2 << (n - 1);
  }
  return this.putInt(i, n);
};

/**
 * Puts the given buffer into this buffer.
 *
 * @param buffer the buffer to put into this one.
 *
 * @return this buffer.
 */
util$1.ByteStringBuffer.prototype.putBuffer = function(buffer) {
  return this.putBytes(buffer.getBytes());
};

/**
 * Gets a byte from this buffer and advances the read pointer by 1.
 *
 * @return the byte.
 */
util$1.ByteStringBuffer.prototype.getByte = function() {
  return this.data.charCodeAt(this.read++);
};

/**
 * Gets a uint16 from this buffer in big-endian order and advances the read
 * pointer by 2.
 *
 * @return the uint16.
 */
util$1.ByteStringBuffer.prototype.getInt16 = function() {
  var rval = (
    this.data.charCodeAt(this.read) << 8 ^
    this.data.charCodeAt(this.read + 1));
  this.read += 2;
  return rval;
};

/**
 * Gets a uint24 from this buffer in big-endian order and advances the read
 * pointer by 3.
 *
 * @return the uint24.
 */
util$1.ByteStringBuffer.prototype.getInt24 = function() {
  var rval = (
    this.data.charCodeAt(this.read) << 16 ^
    this.data.charCodeAt(this.read + 1) << 8 ^
    this.data.charCodeAt(this.read + 2));
  this.read += 3;
  return rval;
};

/**
 * Gets a uint32 from this buffer in big-endian order and advances the read
 * pointer by 4.
 *
 * @return the word.
 */
util$1.ByteStringBuffer.prototype.getInt32 = function() {
  var rval = (
    this.data.charCodeAt(this.read) << 24 ^
    this.data.charCodeAt(this.read + 1) << 16 ^
    this.data.charCodeAt(this.read + 2) << 8 ^
    this.data.charCodeAt(this.read + 3));
  this.read += 4;
  return rval;
};

/**
 * Gets a uint16 from this buffer in little-endian order and advances the read
 * pointer by 2.
 *
 * @return the uint16.
 */
util$1.ByteStringBuffer.prototype.getInt16Le = function() {
  var rval = (
    this.data.charCodeAt(this.read) ^
    this.data.charCodeAt(this.read + 1) << 8);
  this.read += 2;
  return rval;
};

/**
 * Gets a uint24 from this buffer in little-endian order and advances the read
 * pointer by 3.
 *
 * @return the uint24.
 */
util$1.ByteStringBuffer.prototype.getInt24Le = function() {
  var rval = (
    this.data.charCodeAt(this.read) ^
    this.data.charCodeAt(this.read + 1) << 8 ^
    this.data.charCodeAt(this.read + 2) << 16);
  this.read += 3;
  return rval;
};

/**
 * Gets a uint32 from this buffer in little-endian order and advances the read
 * pointer by 4.
 *
 * @return the word.
 */
util$1.ByteStringBuffer.prototype.getInt32Le = function() {
  var rval = (
    this.data.charCodeAt(this.read) ^
    this.data.charCodeAt(this.read + 1) << 8 ^
    this.data.charCodeAt(this.read + 2) << 16 ^
    this.data.charCodeAt(this.read + 3) << 24);
  this.read += 4;
  return rval;
};

/**
 * Gets an n-bit integer from this buffer in big-endian order and advances the
 * read pointer by ceil(n/8).
 *
 * @param n the number of bits in the integer (8, 16, 24, or 32).
 *
 * @return the integer.
 */
util$1.ByteStringBuffer.prototype.getInt = function(n) {
  _checkBitsParam(n);
  var rval = 0;
  do {
    // TODO: Use (rval * 0x100) if adding support for 33 to 53 bits.
    rval = (rval << 8) + this.data.charCodeAt(this.read++);
    n -= 8;
  } while(n > 0);
  return rval;
};

/**
 * Gets a signed n-bit integer from this buffer in big-endian order, using
 * two's complement, and advances the read pointer by n/8.
 *
 * @param n the number of bits in the integer (8, 16, 24, or 32).
 *
 * @return the integer.
 */
util$1.ByteStringBuffer.prototype.getSignedInt = function(n) {
  // getInt checks n
  var x = this.getInt(n);
  var max = 2 << (n - 2);
  if(x >= max) {
    x -= max << 1;
  }
  return x;
};

/**
 * Reads bytes out as a binary encoded string and clears them from the
 * buffer. Note that the resulting string is binary encoded (in node.js this
 * encoding is referred to as `binary`, it is *not* `utf8`).
 *
 * @param count the number of bytes to read, undefined or null for all.
 *
 * @return a binary encoded string of bytes.
 */
util$1.ByteStringBuffer.prototype.getBytes = function(count) {
  var rval;
  if(count) {
    // read count bytes
    count = Math.min(this.length(), count);
    rval = this.data.slice(this.read, this.read + count);
    this.read += count;
  } else if(count === 0) {
    rval = '';
  } else {
    // read all bytes, optimize to only copy when needed
    rval = (this.read === 0) ? this.data : this.data.slice(this.read);
    this.clear();
  }
  return rval;
};

/**
 * Gets a binary encoded string of the bytes from this buffer without
 * modifying the read pointer.
 *
 * @param count the number of bytes to get, omit to get all.
 *
 * @return a string full of binary encoded characters.
 */
util$1.ByteStringBuffer.prototype.bytes = function(count) {
  return (typeof(count) === 'undefined' ?
    this.data.slice(this.read) :
    this.data.slice(this.read, this.read + count));
};

/**
 * Gets a byte at the given index without modifying the read pointer.
 *
 * @param i the byte index.
 *
 * @return the byte.
 */
util$1.ByteStringBuffer.prototype.at = function(i) {
  return this.data.charCodeAt(this.read + i);
};

/**
 * Puts a byte at the given index without modifying the read pointer.
 *
 * @param i the byte index.
 * @param b the byte to put.
 *
 * @return this buffer.
 */
util$1.ByteStringBuffer.prototype.setAt = function(i, b) {
  this.data = this.data.substr(0, this.read + i) +
    String.fromCharCode(b) +
    this.data.substr(this.read + i + 1);
  return this;
};

/**
 * Gets the last byte without modifying the read pointer.
 *
 * @return the last byte.
 */
util$1.ByteStringBuffer.prototype.last = function() {
  return this.data.charCodeAt(this.data.length - 1);
};

/**
 * Creates a copy of this buffer.
 *
 * @return the copy.
 */
util$1.ByteStringBuffer.prototype.copy = function() {
  var c = util$1.createBuffer(this.data);
  c.read = this.read;
  return c;
};

/**
 * Compacts this buffer.
 *
 * @return this buffer.
 */
util$1.ByteStringBuffer.prototype.compact = function() {
  if(this.read > 0) {
    this.data = this.data.slice(this.read);
    this.read = 0;
  }
  return this;
};

/**
 * Clears this buffer.
 *
 * @return this buffer.
 */
util$1.ByteStringBuffer.prototype.clear = function() {
  this.data = '';
  this.read = 0;
  return this;
};

/**
 * Shortens this buffer by triming bytes off of the end of this buffer.
 *
 * @param count the number of bytes to trim off.
 *
 * @return this buffer.
 */
util$1.ByteStringBuffer.prototype.truncate = function(count) {
  var len = Math.max(0, this.length() - count);
  this.data = this.data.substr(this.read, len);
  this.read = 0;
  return this;
};

/**
 * Converts this buffer to a hexadecimal string.
 *
 * @return a hexadecimal string.
 */
util$1.ByteStringBuffer.prototype.toHex = function() {
  var rval = '';
  for(var i = this.read; i < this.data.length; ++i) {
    var b = this.data.charCodeAt(i);
    if(b < 16) {
      rval += '0';
    }
    rval += b.toString(16);
  }
  return rval;
};

/**
 * Converts this buffer to a UTF-16 string (standard JavaScript string).
 *
 * @return a UTF-16 string.
 */
util$1.ByteStringBuffer.prototype.toString = function() {
  return util$1.decodeUtf8(this.bytes());
};

/** End Buffer w/BinaryString backing */

/** Buffer w/UInt8Array backing */

/**
 * FIXME: Experimental. Do not use yet.
 *
 * Constructor for an ArrayBuffer-backed byte buffer.
 *
 * The buffer may be constructed from a string, an ArrayBuffer, DataView, or a
 * TypedArray.
 *
 * If a string is given, its encoding should be provided as an option,
 * otherwise it will default to 'binary'. A 'binary' string is encoded such
 * that each character is one byte in length and size.
 *
 * If an ArrayBuffer, DataView, or TypedArray is given, it will be used
 * *directly* without any copying. Note that, if a write to the buffer requires
 * more space, the buffer will allocate a new backing ArrayBuffer to
 * accommodate. The starting read and write offsets for the buffer may be
 * given as options.
 *
 * @param [b] the initial bytes for this buffer.
 * @param options the options to use:
 *          [readOffset] the starting read offset to use (default: 0).
 *          [writeOffset] the starting write offset to use (default: the
 *            length of the first parameter).
 *          [growSize] the minimum amount, in bytes, to grow the buffer by to
 *            accommodate writes (default: 1024).
 *          [encoding] the encoding ('binary', 'utf8', 'utf16', 'hex') for the
 *            first parameter, if it is a string (default: 'binary').
 */
function DataBuffer(b, options) {
  // default options
  options = options || {};

  // pointers for read from/write to buffer
  this.read = options.readOffset || 0;
  this.growSize = options.growSize || 1024;

  var isArrayBuffer = util$1.isArrayBuffer(b);
  var isArrayBufferView = util$1.isArrayBufferView(b);
  if(isArrayBuffer || isArrayBufferView) {
    // use ArrayBuffer directly
    if(isArrayBuffer) {
      this.data = new DataView(b);
    } else {
      // TODO: adjust read/write offset based on the type of view
      // or specify that this must be done in the options ... that the
      // offsets are byte-based
      this.data = new DataView(b.buffer, b.byteOffset, b.byteLength);
    }
    this.write = ('writeOffset' in options ?
      options.writeOffset : this.data.byteLength);
    return;
  }

  // initialize to empty array buffer and add any given bytes using putBytes
  this.data = new DataView(new ArrayBuffer(0));
  this.write = 0;

  if(b !== null && b !== undefined) {
    this.putBytes(b);
  }

  if('writeOffset' in options) {
    this.write = options.writeOffset;
  }
}
util$1.DataBuffer = DataBuffer;

/**
 * Gets the number of bytes in this buffer.
 *
 * @return the number of bytes in this buffer.
 */
util$1.DataBuffer.prototype.length = function() {
  return this.write - this.read;
};

/**
 * Gets whether or not this buffer is empty.
 *
 * @return true if this buffer is empty, false if not.
 */
util$1.DataBuffer.prototype.isEmpty = function() {
  return this.length() <= 0;
};

/**
 * Ensures this buffer has enough empty space to accommodate the given number
 * of bytes. An optional parameter may be given that indicates a minimum
 * amount to grow the buffer if necessary. If the parameter is not given,
 * the buffer will be grown by some previously-specified default amount
 * or heuristic.
 *
 * @param amount the number of bytes to accommodate.
 * @param [growSize] the minimum amount, in bytes, to grow the buffer by if
 *          necessary.
 */
util$1.DataBuffer.prototype.accommodate = function(amount, growSize) {
  if(this.length() >= amount) {
    return this;
  }
  growSize = Math.max(growSize || this.growSize, amount);

  // grow buffer
  var src = new Uint8Array(
    this.data.buffer, this.data.byteOffset, this.data.byteLength);
  var dst = new Uint8Array(this.length() + growSize);
  dst.set(src);
  this.data = new DataView(dst.buffer);

  return this;
};

/**
 * Puts a byte in this buffer.
 *
 * @param b the byte to put.
 *
 * @return this buffer.
 */
util$1.DataBuffer.prototype.putByte = function(b) {
  this.accommodate(1);
  this.data.setUint8(this.write++, b);
  return this;
};

/**
 * Puts a byte in this buffer N times.
 *
 * @param b the byte to put.
 * @param n the number of bytes of value b to put.
 *
 * @return this buffer.
 */
util$1.DataBuffer.prototype.fillWithByte = function(b, n) {
  this.accommodate(n);
  for(var i = 0; i < n; ++i) {
    this.data.setUint8(b);
  }
  return this;
};

/**
 * Puts bytes in this buffer. The bytes may be given as a string, an
 * ArrayBuffer, a DataView, or a TypedArray.
 *
 * @param bytes the bytes to put.
 * @param [encoding] the encoding for the first parameter ('binary', 'utf8',
 *          'utf16', 'hex'), if it is a string (default: 'binary').
 *
 * @return this buffer.
 */
util$1.DataBuffer.prototype.putBytes = function(bytes, encoding) {
  if(util$1.isArrayBufferView(bytes)) {
    var src = new Uint8Array(bytes.buffer, bytes.byteOffset, bytes.byteLength);
    var len = src.byteLength - src.byteOffset;
    this.accommodate(len);
    var dst = new Uint8Array(this.data.buffer, this.write);
    dst.set(src);
    this.write += len;
    return this;
  }

  if(util$1.isArrayBuffer(bytes)) {
    var src = new Uint8Array(bytes);
    this.accommodate(src.byteLength);
    var dst = new Uint8Array(this.data.buffer);
    dst.set(src, this.write);
    this.write += src.byteLength;
    return this;
  }

  // bytes is a util.DataBuffer or equivalent
  if(bytes instanceof util$1.DataBuffer ||
    (typeof bytes === 'object' &&
    typeof bytes.read === 'number' && typeof bytes.write === 'number' &&
    util$1.isArrayBufferView(bytes.data))) {
    var src = new Uint8Array(bytes.data.byteLength, bytes.read, bytes.length());
    this.accommodate(src.byteLength);
    var dst = new Uint8Array(bytes.data.byteLength, this.write);
    dst.set(src);
    this.write += src.byteLength;
    return this;
  }

  if(bytes instanceof util$1.ByteStringBuffer) {
    // copy binary string and process as the same as a string parameter below
    bytes = bytes.data;
    encoding = 'binary';
  }

  // string conversion
  encoding = encoding || 'binary';
  if(typeof bytes === 'string') {
    var view;

    // decode from string
    if(encoding === 'hex') {
      this.accommodate(Math.ceil(bytes.length / 2));
      view = new Uint8Array(this.data.buffer, this.write);
      this.write += util$1.binary.hex.decode(bytes, view, this.write);
      return this;
    }
    if(encoding === 'base64') {
      this.accommodate(Math.ceil(bytes.length / 4) * 3);
      view = new Uint8Array(this.data.buffer, this.write);
      this.write += util$1.binary.base64.decode(bytes, view, this.write);
      return this;
    }

    // encode text as UTF-8 bytes
    if(encoding === 'utf8') {
      // encode as UTF-8 then decode string as raw binary
      bytes = util$1.encodeUtf8(bytes);
      encoding = 'binary';
    }

    // decode string as raw binary
    if(encoding === 'binary' || encoding === 'raw') {
      // one byte per character
      this.accommodate(bytes.length);
      view = new Uint8Array(this.data.buffer, this.write);
      this.write += util$1.binary.raw.decode(view);
      return this;
    }

    // encode text as UTF-16 bytes
    if(encoding === 'utf16') {
      // two bytes per character
      this.accommodate(bytes.length * 2);
      view = new Uint16Array(this.data.buffer, this.write);
      this.write += util$1.text.utf16.encode(view);
      return this;
    }

    throw new Error('Invalid encoding: ' + encoding);
  }

  throw Error('Invalid parameter: ' + bytes);
};

/**
 * Puts the given buffer into this buffer.
 *
 * @param buffer the buffer to put into this one.
 *
 * @return this buffer.
 */
util$1.DataBuffer.prototype.putBuffer = function(buffer) {
  this.putBytes(buffer);
  buffer.clear();
  return this;
};

/**
 * Puts a string into this buffer.
 *
 * @param str the string to put.
 * @param [encoding] the encoding for the string (default: 'utf16').
 *
 * @return this buffer.
 */
util$1.DataBuffer.prototype.putString = function(str) {
  return this.putBytes(str, 'utf16');
};

/**
 * Puts a 16-bit integer in this buffer in big-endian order.
 *
 * @param i the 16-bit integer.
 *
 * @return this buffer.
 */
util$1.DataBuffer.prototype.putInt16 = function(i) {
  this.accommodate(2);
  this.data.setInt16(this.write, i);
  this.write += 2;
  return this;
};

/**
 * Puts a 24-bit integer in this buffer in big-endian order.
 *
 * @param i the 24-bit integer.
 *
 * @return this buffer.
 */
util$1.DataBuffer.prototype.putInt24 = function(i) {
  this.accommodate(3);
  this.data.setInt16(this.write, i >> 8 & 0xFFFF);
  this.data.setInt8(this.write, i >> 16 & 0xFF);
  this.write += 3;
  return this;
};

/**
 * Puts a 32-bit integer in this buffer in big-endian order.
 *
 * @param i the 32-bit integer.
 *
 * @return this buffer.
 */
util$1.DataBuffer.prototype.putInt32 = function(i) {
  this.accommodate(4);
  this.data.setInt32(this.write, i);
  this.write += 4;
  return this;
};

/**
 * Puts a 16-bit integer in this buffer in little-endian order.
 *
 * @param i the 16-bit integer.
 *
 * @return this buffer.
 */
util$1.DataBuffer.prototype.putInt16Le = function(i) {
  this.accommodate(2);
  this.data.setInt16(this.write, i, true);
  this.write += 2;
  return this;
};

/**
 * Puts a 24-bit integer in this buffer in little-endian order.
 *
 * @param i the 24-bit integer.
 *
 * @return this buffer.
 */
util$1.DataBuffer.prototype.putInt24Le = function(i) {
  this.accommodate(3);
  this.data.setInt8(this.write, i >> 16 & 0xFF);
  this.data.setInt16(this.write, i >> 8 & 0xFFFF, true);
  this.write += 3;
  return this;
};

/**
 * Puts a 32-bit integer in this buffer in little-endian order.
 *
 * @param i the 32-bit integer.
 *
 * @return this buffer.
 */
util$1.DataBuffer.prototype.putInt32Le = function(i) {
  this.accommodate(4);
  this.data.setInt32(this.write, i, true);
  this.write += 4;
  return this;
};

/**
 * Puts an n-bit integer in this buffer in big-endian order.
 *
 * @param i the n-bit integer.
 * @param n the number of bits in the integer (8, 16, 24, or 32).
 *
 * @return this buffer.
 */
util$1.DataBuffer.prototype.putInt = function(i, n) {
  _checkBitsParam(n);
  this.accommodate(n / 8);
  do {
    n -= 8;
    this.data.setInt8(this.write++, (i >> n) & 0xFF);
  } while(n > 0);
  return this;
};

/**
 * Puts a signed n-bit integer in this buffer in big-endian order. Two's
 * complement representation is used.
 *
 * @param i the n-bit integer.
 * @param n the number of bits in the integer.
 *
 * @return this buffer.
 */
util$1.DataBuffer.prototype.putSignedInt = function(i, n) {
  _checkBitsParam(n);
  this.accommodate(n / 8);
  if(i < 0) {
    i += 2 << (n - 1);
  }
  return this.putInt(i, n);
};

/**
 * Gets a byte from this buffer and advances the read pointer by 1.
 *
 * @return the byte.
 */
util$1.DataBuffer.prototype.getByte = function() {
  return this.data.getInt8(this.read++);
};

/**
 * Gets a uint16 from this buffer in big-endian order and advances the read
 * pointer by 2.
 *
 * @return the uint16.
 */
util$1.DataBuffer.prototype.getInt16 = function() {
  var rval = this.data.getInt16(this.read);
  this.read += 2;
  return rval;
};

/**
 * Gets a uint24 from this buffer in big-endian order and advances the read
 * pointer by 3.
 *
 * @return the uint24.
 */
util$1.DataBuffer.prototype.getInt24 = function() {
  var rval = (
    this.data.getInt16(this.read) << 8 ^
    this.data.getInt8(this.read + 2));
  this.read += 3;
  return rval;
};

/**
 * Gets a uint32 from this buffer in big-endian order and advances the read
 * pointer by 4.
 *
 * @return the word.
 */
util$1.DataBuffer.prototype.getInt32 = function() {
  var rval = this.data.getInt32(this.read);
  this.read += 4;
  return rval;
};

/**
 * Gets a uint16 from this buffer in little-endian order and advances the read
 * pointer by 2.
 *
 * @return the uint16.
 */
util$1.DataBuffer.prototype.getInt16Le = function() {
  var rval = this.data.getInt16(this.read, true);
  this.read += 2;
  return rval;
};

/**
 * Gets a uint24 from this buffer in little-endian order and advances the read
 * pointer by 3.
 *
 * @return the uint24.
 */
util$1.DataBuffer.prototype.getInt24Le = function() {
  var rval = (
    this.data.getInt8(this.read) ^
    this.data.getInt16(this.read + 1, true) << 8);
  this.read += 3;
  return rval;
};

/**
 * Gets a uint32 from this buffer in little-endian order and advances the read
 * pointer by 4.
 *
 * @return the word.
 */
util$1.DataBuffer.prototype.getInt32Le = function() {
  var rval = this.data.getInt32(this.read, true);
  this.read += 4;
  return rval;
};

/**
 * Gets an n-bit integer from this buffer in big-endian order and advances the
 * read pointer by n/8.
 *
 * @param n the number of bits in the integer (8, 16, 24, or 32).
 *
 * @return the integer.
 */
util$1.DataBuffer.prototype.getInt = function(n) {
  _checkBitsParam(n);
  var rval = 0;
  do {
    // TODO: Use (rval * 0x100) if adding support for 33 to 53 bits.
    rval = (rval << 8) + this.data.getInt8(this.read++);
    n -= 8;
  } while(n > 0);
  return rval;
};

/**
 * Gets a signed n-bit integer from this buffer in big-endian order, using
 * two's complement, and advances the read pointer by n/8.
 *
 * @param n the number of bits in the integer (8, 16, 24, or 32).
 *
 * @return the integer.
 */
util$1.DataBuffer.prototype.getSignedInt = function(n) {
  // getInt checks n
  var x = this.getInt(n);
  var max = 2 << (n - 2);
  if(x >= max) {
    x -= max << 1;
  }
  return x;
};

/**
 * Reads bytes out as a binary encoded string and clears them from the
 * buffer.
 *
 * @param count the number of bytes to read, undefined or null for all.
 *
 * @return a binary encoded string of bytes.
 */
util$1.DataBuffer.prototype.getBytes = function(count) {
  // TODO: deprecate this method, it is poorly named and
  // this.toString('binary') replaces it
  // add a toTypedArray()/toArrayBuffer() function
  var rval;
  if(count) {
    // read count bytes
    count = Math.min(this.length(), count);
    rval = this.data.slice(this.read, this.read + count);
    this.read += count;
  } else if(count === 0) {
    rval = '';
  } else {
    // read all bytes, optimize to only copy when needed
    rval = (this.read === 0) ? this.data : this.data.slice(this.read);
    this.clear();
  }
  return rval;
};

/**
 * Gets a binary encoded string of the bytes from this buffer without
 * modifying the read pointer.
 *
 * @param count the number of bytes to get, omit to get all.
 *
 * @return a string full of binary encoded characters.
 */
util$1.DataBuffer.prototype.bytes = function(count) {
  // TODO: deprecate this method, it is poorly named, add "getString()"
  return (typeof(count) === 'undefined' ?
    this.data.slice(this.read) :
    this.data.slice(this.read, this.read + count));
};

/**
 * Gets a byte at the given index without modifying the read pointer.
 *
 * @param i the byte index.
 *
 * @return the byte.
 */
util$1.DataBuffer.prototype.at = function(i) {
  return this.data.getUint8(this.read + i);
};

/**
 * Puts a byte at the given index without modifying the read pointer.
 *
 * @param i the byte index.
 * @param b the byte to put.
 *
 * @return this buffer.
 */
util$1.DataBuffer.prototype.setAt = function(i, b) {
  this.data.setUint8(i, b);
  return this;
};

/**
 * Gets the last byte without modifying the read pointer.
 *
 * @return the last byte.
 */
util$1.DataBuffer.prototype.last = function() {
  return this.data.getUint8(this.write - 1);
};

/**
 * Creates a copy of this buffer.
 *
 * @return the copy.
 */
util$1.DataBuffer.prototype.copy = function() {
  return new util$1.DataBuffer(this);
};

/**
 * Compacts this buffer.
 *
 * @return this buffer.
 */
util$1.DataBuffer.prototype.compact = function() {
  if(this.read > 0) {
    var src = new Uint8Array(this.data.buffer, this.read);
    var dst = new Uint8Array(src.byteLength);
    dst.set(src);
    this.data = new DataView(dst);
    this.write -= this.read;
    this.read = 0;
  }
  return this;
};

/**
 * Clears this buffer.
 *
 * @return this buffer.
 */
util$1.DataBuffer.prototype.clear = function() {
  this.data = new DataView(new ArrayBuffer(0));
  this.read = this.write = 0;
  return this;
};

/**
 * Shortens this buffer by triming bytes off of the end of this buffer.
 *
 * @param count the number of bytes to trim off.
 *
 * @return this buffer.
 */
util$1.DataBuffer.prototype.truncate = function(count) {
  this.write = Math.max(0, this.length() - count);
  this.read = Math.min(this.read, this.write);
  return this;
};

/**
 * Converts this buffer to a hexadecimal string.
 *
 * @return a hexadecimal string.
 */
util$1.DataBuffer.prototype.toHex = function() {
  var rval = '';
  for(var i = this.read; i < this.data.byteLength; ++i) {
    var b = this.data.getUint8(i);
    if(b < 16) {
      rval += '0';
    }
    rval += b.toString(16);
  }
  return rval;
};

/**
 * Converts this buffer to a string, using the given encoding. If no
 * encoding is given, 'utf8' (UTF-8) is used.
 *
 * @param [encoding] the encoding to use: 'binary', 'utf8', 'utf16', 'hex',
 *          'base64' (default: 'utf8').
 *
 * @return a string representation of the bytes in this buffer.
 */
util$1.DataBuffer.prototype.toString = function(encoding) {
  var view = new Uint8Array(this.data, this.read, this.length());
  encoding = encoding || 'utf8';

  // encode to string
  if(encoding === 'binary' || encoding === 'raw') {
    return util$1.binary.raw.encode(view);
  }
  if(encoding === 'hex') {
    return util$1.binary.hex.encode(view);
  }
  if(encoding === 'base64') {
    return util$1.binary.base64.encode(view);
  }

  // decode to text
  if(encoding === 'utf8') {
    return util$1.text.utf8.decode(view);
  }
  if(encoding === 'utf16') {
    return util$1.text.utf16.decode(view);
  }

  throw new Error('Invalid encoding: ' + encoding);
};

/** End Buffer w/UInt8Array backing */

/**
 * Creates a buffer that stores bytes. A value may be given to populate the
 * buffer with data. This value can either be string of encoded bytes or a
 * regular string of characters. When passing a string of binary encoded
 * bytes, the encoding `raw` should be given. This is also the default. When
 * passing a string of characters, the encoding `utf8` should be given.
 *
 * @param [input] a string with encoded bytes to store in the buffer.
 * @param [encoding] (default: 'raw', other: 'utf8').
 */
util$1.createBuffer = function(input, encoding) {
  // TODO: deprecate, use new ByteBuffer() instead
  encoding = encoding || 'raw';
  if(input !== undefined && encoding === 'utf8') {
    input = util$1.encodeUtf8(input);
  }
  return new util$1.ByteBuffer(input);
};

/**
 * Fills a string with a particular value. If you want the string to be a byte
 * string, pass in String.fromCharCode(theByte).
 *
 * @param c the character to fill the string with, use String.fromCharCode
 *          to fill the string with a byte value.
 * @param n the number of characters of value c to fill with.
 *
 * @return the filled string.
 */
util$1.fillString = function(c, n) {
  var s = '';
  while(n > 0) {
    if(n & 1) {
      s += c;
    }
    n >>>= 1;
    if(n > 0) {
      c += c;
    }
  }
  return s;
};

/**
 * Performs a per byte XOR between two byte strings and returns the result as a
 * string of bytes.
 *
 * @param s1 first string of bytes.
 * @param s2 second string of bytes.
 * @param n the number of bytes to XOR.
 *
 * @return the XOR'd result.
 */
util$1.xorBytes = function(s1, s2, n) {
  var s3 = '';
  var b = '';
  var t = '';
  var i = 0;
  var c = 0;
  for(; n > 0; --n, ++i) {
    b = s1.charCodeAt(i) ^ s2.charCodeAt(i);
    if(c >= 10) {
      s3 += t;
      t = '';
      c = 0;
    }
    t += String.fromCharCode(b);
    ++c;
  }
  s3 += t;
  return s3;
};

/**
 * Converts a hex string into a 'binary' encoded string of bytes.
 *
 * @param hex the hexadecimal string to convert.
 *
 * @return the binary-encoded string of bytes.
 */
util$1.hexToBytes = function(hex) {
  // TODO: deprecate: "Deprecated. Use util.binary.hex.decode instead."
  var rval = '';
  var i = 0;
  if(hex.length & 1 == 1) {
    // odd number of characters, convert first character alone
    i = 1;
    rval += String.fromCharCode(parseInt(hex[0], 16));
  }
  // convert 2 characters (1 byte) at a time
  for(; i < hex.length; i += 2) {
    rval += String.fromCharCode(parseInt(hex.substr(i, 2), 16));
  }
  return rval;
};

/**
 * Converts a 'binary' encoded string of bytes to hex.
 *
 * @param bytes the byte string to convert.
 *
 * @return the string of hexadecimal characters.
 */
util$1.bytesToHex = function(bytes) {
  // TODO: deprecate: "Deprecated. Use util.binary.hex.encode instead."
  return util$1.createBuffer(bytes).toHex();
};

/**
 * Converts an 32-bit integer to 4-big-endian byte string.
 *
 * @param i the integer.
 *
 * @return the byte string.
 */
util$1.int32ToBytes = function(i) {
  return (
    String.fromCharCode(i >> 24 & 0xFF) +
    String.fromCharCode(i >> 16 & 0xFF) +
    String.fromCharCode(i >> 8 & 0xFF) +
    String.fromCharCode(i & 0xFF));
};

// base64 characters, reverse mapping
var _base64 =
  'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=';
var _base64Idx = [
/*43 -43 = 0*/
/*'+',  1,  2,  3,'/' */
   62, -1, -1, -1, 63,

/*'0','1','2','3','4','5','6','7','8','9' */
   52, 53, 54, 55, 56, 57, 58, 59, 60, 61,

/*15, 16, 17,'=', 19, 20, 21 */
  -1, -1, -1, 64, -1, -1, -1,

/*65 - 43 = 22*/
/*'A','B','C','D','E','F','G','H','I','J','K','L','M', */
   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,

/*'N','O','P','Q','R','S','T','U','V','W','X','Y','Z' */
   13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,

/*91 - 43 = 48 */
/*48, 49, 50, 51, 52, 53 */
  -1, -1, -1, -1, -1, -1,

/*97 - 43 = 54*/
/*'a','b','c','d','e','f','g','h','i','j','k','l','m' */
   26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,

/*'n','o','p','q','r','s','t','u','v','w','x','y','z' */
   39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
];

// base58 characters (Bitcoin alphabet)
var _base58 = '123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz';

/**
 * Base64 encodes a 'binary' encoded string of bytes.
 *
 * @param input the binary encoded string of bytes to base64-encode.
 * @param maxline the maximum number of encoded characters per line to use,
 *          defaults to none.
 *
 * @return the base64-encoded output.
 */
util$1.encode64 = function(input, maxline) {
  // TODO: deprecate: "Deprecated. Use util.binary.base64.encode instead."
  var line = '';
  var output = '';
  var chr1, chr2, chr3;
  var i = 0;
  while(i < input.length) {
    chr1 = input.charCodeAt(i++);
    chr2 = input.charCodeAt(i++);
    chr3 = input.charCodeAt(i++);

    // encode 4 character group
    line += _base64.charAt(chr1 >> 2);
    line += _base64.charAt(((chr1 & 3) << 4) | (chr2 >> 4));
    if(isNaN(chr2)) {
      line += '==';
    } else {
      line += _base64.charAt(((chr2 & 15) << 2) | (chr3 >> 6));
      line += isNaN(chr3) ? '=' : _base64.charAt(chr3 & 63);
    }

    if(maxline && line.length > maxline) {
      output += line.substr(0, maxline) + '\r\n';
      line = line.substr(maxline);
    }
  }
  output += line;
  return output;
};

/**
 * Base64 decodes a string into a 'binary' encoded string of bytes.
 *
 * @param input the base64-encoded input.
 *
 * @return the binary encoded string.
 */
util$1.decode64 = function(input) {
  // TODO: deprecate: "Deprecated. Use util.binary.base64.decode instead."

  // remove all non-base64 characters
  input = input.replace(/[^A-Za-z0-9\+\/\=]/g, '');

  var output = '';
  var enc1, enc2, enc3, enc4;
  var i = 0;

  while(i < input.length) {
    enc1 = _base64Idx[input.charCodeAt(i++) - 43];
    enc2 = _base64Idx[input.charCodeAt(i++) - 43];
    enc3 = _base64Idx[input.charCodeAt(i++) - 43];
    enc4 = _base64Idx[input.charCodeAt(i++) - 43];

    output += String.fromCharCode((enc1 << 2) | (enc2 >> 4));
    if(enc3 !== 64) {
      // decoded at least 2 bytes
      output += String.fromCharCode(((enc2 & 15) << 4) | (enc3 >> 2));
      if(enc4 !== 64) {
        // decoded 3 bytes
        output += String.fromCharCode(((enc3 & 3) << 6) | enc4);
      }
    }
  }

  return output;
};

/**
 * Encodes the given string of characters (a standard JavaScript
 * string) as a binary encoded string where the bytes represent
 * a UTF-8 encoded string of characters. Non-ASCII characters will be
 * encoded as multiple bytes according to UTF-8.
 *
 * @param str a standard string of characters to encode.
 *
 * @return the binary encoded string.
 */
util$1.encodeUtf8 = function(str) {
  return unescape(encodeURIComponent(str));
};

/**
 * Decodes a binary encoded string that contains bytes that
 * represent a UTF-8 encoded string of characters -- into a
 * string of characters (a standard JavaScript string).
 *
 * @param str the binary encoded string to decode.
 *
 * @return the resulting standard string of characters.
 */
util$1.decodeUtf8 = function(str) {
  return decodeURIComponent(escape(str));
};

// binary encoding/decoding tools
// FIXME: Experimental. Do not use yet.
util$1.binary = {
  raw: {},
  hex: {},
  base64: {},
  base58: {},
  baseN : {
    encode: baseN.encode,
    decode: baseN.decode
  }
};

/**
 * Encodes a Uint8Array as a binary-encoded string. This encoding uses
 * a value between 0 and 255 for each character.
 *
 * @param bytes the Uint8Array to encode.
 *
 * @return the binary-encoded string.
 */
util$1.binary.raw.encode = function(bytes) {
  return String.fromCharCode.apply(null, bytes);
};

/**
 * Decodes a binary-encoded string to a Uint8Array. This encoding uses
 * a value between 0 and 255 for each character.
 *
 * @param str the binary-encoded string to decode.
 * @param [output] an optional Uint8Array to write the output to; if it
 *          is too small, an exception will be thrown.
 * @param [offset] the start offset for writing to the output (default: 0).
 *
 * @return the Uint8Array or the number of bytes written if output was given.
 */
util$1.binary.raw.decode = function(str, output, offset) {
  var out = output;
  if(!out) {
    out = new Uint8Array(str.length);
  }
  offset = offset || 0;
  var j = offset;
  for(var i = 0; i < str.length; ++i) {
    out[j++] = str.charCodeAt(i);
  }
  return output ? (j - offset) : out;
};

/**
 * Encodes a 'binary' string, ArrayBuffer, DataView, TypedArray, or
 * ByteBuffer as a string of hexadecimal characters.
 *
 * @param bytes the bytes to convert.
 *
 * @return the string of hexadecimal characters.
 */
util$1.binary.hex.encode = util$1.bytesToHex;

/**
 * Decodes a hex-encoded string to a Uint8Array.
 *
 * @param hex the hexadecimal string to convert.
 * @param [output] an optional Uint8Array to write the output to; if it
 *          is too small, an exception will be thrown.
 * @param [offset] the start offset for writing to the output (default: 0).
 *
 * @return the Uint8Array or the number of bytes written if output was given.
 */
util$1.binary.hex.decode = function(hex, output, offset) {
  var out = output;
  if(!out) {
    out = new Uint8Array(Math.ceil(hex.length / 2));
  }
  offset = offset || 0;
  var i = 0, j = offset;
  if(hex.length & 1) {
    // odd number of characters, convert first character alone
    i = 1;
    out[j++] = parseInt(hex[0], 16);
  }
  // convert 2 characters (1 byte) at a time
  for(; i < hex.length; i += 2) {
    out[j++] = parseInt(hex.substr(i, 2), 16);
  }
  return output ? (j - offset) : out;
};

/**
 * Base64-encodes a Uint8Array.
 *
 * @param input the Uint8Array to encode.
 * @param maxline the maximum number of encoded characters per line to use,
 *          defaults to none.
 *
 * @return the base64-encoded output string.
 */
util$1.binary.base64.encode = function(input, maxline) {
  var line = '';
  var output = '';
  var chr1, chr2, chr3;
  var i = 0;
  while(i < input.byteLength) {
    chr1 = input[i++];
    chr2 = input[i++];
    chr3 = input[i++];

    // encode 4 character group
    line += _base64.charAt(chr1 >> 2);
    line += _base64.charAt(((chr1 & 3) << 4) | (chr2 >> 4));
    if(isNaN(chr2)) {
      line += '==';
    } else {
      line += _base64.charAt(((chr2 & 15) << 2) | (chr3 >> 6));
      line += isNaN(chr3) ? '=' : _base64.charAt(chr3 & 63);
    }

    if(maxline && line.length > maxline) {
      output += line.substr(0, maxline) + '\r\n';
      line = line.substr(maxline);
    }
  }
  output += line;
  return output;
};

/**
 * Decodes a base64-encoded string to a Uint8Array.
 *
 * @param input the base64-encoded input string.
 * @param [output] an optional Uint8Array to write the output to; if it
 *          is too small, an exception will be thrown.
 * @param [offset] the start offset for writing to the output (default: 0).
 *
 * @return the Uint8Array or the number of bytes written if output was given.
 */
util$1.binary.base64.decode = function(input, output, offset) {
  var out = output;
  if(!out) {
    out = new Uint8Array(Math.ceil(input.length / 4) * 3);
  }

  // remove all non-base64 characters
  input = input.replace(/[^A-Za-z0-9\+\/\=]/g, '');

  offset = offset || 0;
  var enc1, enc2, enc3, enc4;
  var i = 0, j = offset;

  while(i < input.length) {
    enc1 = _base64Idx[input.charCodeAt(i++) - 43];
    enc2 = _base64Idx[input.charCodeAt(i++) - 43];
    enc3 = _base64Idx[input.charCodeAt(i++) - 43];
    enc4 = _base64Idx[input.charCodeAt(i++) - 43];

    out[j++] = (enc1 << 2) | (enc2 >> 4);
    if(enc3 !== 64) {
      // decoded at least 2 bytes
      out[j++] = ((enc2 & 15) << 4) | (enc3 >> 2);
      if(enc4 !== 64) {
        // decoded 3 bytes
        out[j++] = ((enc3 & 3) << 6) | enc4;
      }
    }
  }

  // make sure result is the exact decoded length
  return output ? (j - offset) : out.subarray(0, j);
};

// add support for base58 encoding/decoding with Bitcoin alphabet
util$1.binary.base58.encode = function(input, maxline) {
  return util$1.binary.baseN.encode(input, _base58, maxline);
};
util$1.binary.base58.decode = function(input, maxline) {
  return util$1.binary.baseN.decode(input, _base58, maxline);
};

// text encoding/decoding tools
// FIXME: Experimental. Do not use yet.
util$1.text = {
  utf8: {},
  utf16: {}
};

/**
 * Encodes the given string as UTF-8 in a Uint8Array.
 *
 * @param str the string to encode.
 * @param [output] an optional Uint8Array to write the output to; if it
 *          is too small, an exception will be thrown.
 * @param [offset] the start offset for writing to the output (default: 0).
 *
 * @return the Uint8Array or the number of bytes written if output was given.
 */
util$1.text.utf8.encode = function(str, output, offset) {
  str = util$1.encodeUtf8(str);
  var out = output;
  if(!out) {
    out = new Uint8Array(str.length);
  }
  offset = offset || 0;
  var j = offset;
  for(var i = 0; i < str.length; ++i) {
    out[j++] = str.charCodeAt(i);
  }
  return output ? (j - offset) : out;
};

/**
 * Decodes the UTF-8 contents from a Uint8Array.
 *
 * @param bytes the Uint8Array to decode.
 *
 * @return the resulting string.
 */
util$1.text.utf8.decode = function(bytes) {
  return util$1.decodeUtf8(String.fromCharCode.apply(null, bytes));
};

/**
 * Encodes the given string as UTF-16 in a Uint8Array.
 *
 * @param str the string to encode.
 * @param [output] an optional Uint8Array to write the output to; if it
 *          is too small, an exception will be thrown.
 * @param [offset] the start offset for writing to the output (default: 0).
 *
 * @return the Uint8Array or the number of bytes written if output was given.
 */
util$1.text.utf16.encode = function(str, output, offset) {
  var out = output;
  if(!out) {
    out = new Uint8Array(str.length * 2);
  }
  var view = new Uint16Array(out.buffer);
  offset = offset || 0;
  var j = offset;
  var k = offset;
  for(var i = 0; i < str.length; ++i) {
    view[k++] = str.charCodeAt(i);
    j += 2;
  }
  return output ? (j - offset) : out;
};

/**
 * Decodes the UTF-16 contents from a Uint8Array.
 *
 * @param bytes the Uint8Array to decode.
 *
 * @return the resulting string.
 */
util$1.text.utf16.decode = function(bytes) {
  return String.fromCharCode.apply(null, new Uint16Array(bytes.buffer));
};

/**
 * Deflates the given data using a flash interface.
 *
 * @param api the flash interface.
 * @param bytes the data.
 * @param raw true to return only raw deflate data, false to include zlib
 *          header and trailer.
 *
 * @return the deflated data as a string.
 */
util$1.deflate = function(api, bytes, raw) {
  bytes = util$1.decode64(api.deflate(util$1.encode64(bytes)).rval);

  // strip zlib header and trailer if necessary
  if(raw) {
    // zlib header is 2 bytes (CMF,FLG) where FLG indicates that
    // there is a 4-byte DICT (alder-32) block before the data if
    // its 5th bit is set
    var start = 2;
    var flg = bytes.charCodeAt(1);
    if(flg & 0x20) {
      start = 6;
    }
    // zlib trailer is 4 bytes of adler-32
    bytes = bytes.substring(start, bytes.length - 4);
  }

  return bytes;
};

/**
 * Inflates the given data using a flash interface.
 *
 * @param api the flash interface.
 * @param bytes the data.
 * @param raw true if the incoming data has no zlib header or trailer and is
 *          raw DEFLATE data.
 *
 * @return the inflated data as a string, null on error.
 */
util$1.inflate = function(api, bytes, raw) {
  // TODO: add zlib header and trailer if necessary/possible
  var rval = api.inflate(util$1.encode64(bytes)).rval;
  return (rval === null) ? null : util$1.decode64(rval);
};

/**
 * Sets a storage object.
 *
 * @param api the storage interface.
 * @param id the storage ID to use.
 * @param obj the storage object, null to remove.
 */
var _setStorageObject = function(api, id, obj) {
  if(!api) {
    throw new Error('WebStorage not available.');
  }

  var rval;
  if(obj === null) {
    rval = api.removeItem(id);
  } else {
    // json-encode and base64-encode object
    obj = util$1.encode64(JSON.stringify(obj));
    rval = api.setItem(id, obj);
  }

  // handle potential flash error
  if(typeof(rval) !== 'undefined' && rval.rval !== true) {
    var error = new Error(rval.error.message);
    error.id = rval.error.id;
    error.name = rval.error.name;
    throw error;
  }
};

/**
 * Gets a storage object.
 *
 * @param api the storage interface.
 * @param id the storage ID to use.
 *
 * @return the storage object entry or null if none exists.
 */
var _getStorageObject = function(api, id) {
  if(!api) {
    throw new Error('WebStorage not available.');
  }

  // get the existing entry
  var rval = api.getItem(id);

  /* Note: We check api.init because we can't do (api == localStorage)
    on IE because of "Class doesn't support Automation" exception. Only
    the flash api has an init method so this works too, but we need a
    better solution in the future. */

  // flash returns item wrapped in an object, handle special case
  if(api.init) {
    if(rval.rval === null) {
      if(rval.error) {
        var error = new Error(rval.error.message);
        error.id = rval.error.id;
        error.name = rval.error.name;
        throw error;
      }
      // no error, but also no item
      rval = null;
    } else {
      rval = rval.rval;
    }
  }

  // handle decoding
  if(rval !== null) {
    // base64-decode and json-decode data
    rval = JSON.parse(util$1.decode64(rval));
  }

  return rval;
};

/**
 * Stores an item in local storage.
 *
 * @param api the storage interface.
 * @param id the storage ID to use.
 * @param key the key for the item.
 * @param data the data for the item (any javascript object/primitive).
 */
var _setItem = function(api, id, key, data) {
  // get storage object
  var obj = _getStorageObject(api, id);
  if(obj === null) {
    // create a new storage object
    obj = {};
  }
  // update key
  obj[key] = data;

  // set storage object
  _setStorageObject(api, id, obj);
};

/**
 * Gets an item from local storage.
 *
 * @param api the storage interface.
 * @param id the storage ID to use.
 * @param key the key for the item.
 *
 * @return the item.
 */
var _getItem = function(api, id, key) {
  // get storage object
  var rval = _getStorageObject(api, id);
  if(rval !== null) {
    // return data at key
    rval = (key in rval) ? rval[key] : null;
  }

  return rval;
};

/**
 * Removes an item from local storage.
 *
 * @param api the storage interface.
 * @param id the storage ID to use.
 * @param key the key for the item.
 */
var _removeItem = function(api, id, key) {
  // get storage object
  var obj = _getStorageObject(api, id);
  if(obj !== null && key in obj) {
    // remove key
    delete obj[key];

    // see if entry has no keys remaining
    var empty = true;
    for(var prop in obj) {
      empty = false;
      break;
    }
    if(empty) {
      // remove entry entirely if no keys are left
      obj = null;
    }

    // set storage object
    _setStorageObject(api, id, obj);
  }
};

/**
 * Clears the local disk storage identified by the given ID.
 *
 * @param api the storage interface.
 * @param id the storage ID to use.
 */
var _clearItems = function(api, id) {
  _setStorageObject(api, id, null);
};

/**
 * Calls a storage function.
 *
 * @param func the function to call.
 * @param args the arguments for the function.
 * @param location the location argument.
 *
 * @return the return value from the function.
 */
var _callStorageFunction = function(func, args, location) {
  var rval = null;

  // default storage types
  if(typeof(location) === 'undefined') {
    location = ['web', 'flash'];
  }

  // apply storage types in order of preference
  var type;
  var done = false;
  var exception = null;
  for(var idx in location) {
    type = location[idx];
    try {
      if(type === 'flash' || type === 'both') {
        if(args[0] === null) {
          throw new Error('Flash local storage not available.');
        }
        rval = func.apply(this, args);
        done = (type === 'flash');
      }
      if(type === 'web' || type === 'both') {
        args[0] = localStorage;
        rval = func.apply(this, args);
        done = true;
      }
    } catch(ex) {
      exception = ex;
    }
    if(done) {
      break;
    }
  }

  if(!done) {
    throw exception;
  }

  return rval;
};

/**
 * Stores an item on local disk.
 *
 * The available types of local storage include 'flash', 'web', and 'both'.
 *
 * The type 'flash' refers to flash local storage (SharedObject). In order
 * to use flash local storage, the 'api' parameter must be valid. The type
 * 'web' refers to WebStorage, if supported by the browser. The type 'both'
 * refers to storing using both 'flash' and 'web', not just one or the
 * other.
 *
 * The location array should list the storage types to use in order of
 * preference:
 *
 * ['flash']: flash only storage
 * ['web']: web only storage
 * ['both']: try to store in both
 * ['flash','web']: store in flash first, but if not available, 'web'
 * ['web','flash']: store in web first, but if not available, 'flash'
 *
 * The location array defaults to: ['web', 'flash']
 *
 * @param api the flash interface, null to use only WebStorage.
 * @param id the storage ID to use.
 * @param key the key for the item.
 * @param data the data for the item (any javascript object/primitive).
 * @param location an array with the preferred types of storage to use.
 */
util$1.setItem = function(api, id, key, data, location) {
  _callStorageFunction(_setItem, arguments, location);
};

/**
 * Gets an item on local disk.
 *
 * Set setItem() for details on storage types.
 *
 * @param api the flash interface, null to use only WebStorage.
 * @param id the storage ID to use.
 * @param key the key for the item.
 * @param location an array with the preferred types of storage to use.
 *
 * @return the item.
 */
util$1.getItem = function(api, id, key, location) {
  return _callStorageFunction(_getItem, arguments, location);
};

/**
 * Removes an item on local disk.
 *
 * Set setItem() for details on storage types.
 *
 * @param api the flash interface.
 * @param id the storage ID to use.
 * @param key the key for the item.
 * @param location an array with the preferred types of storage to use.
 */
util$1.removeItem = function(api, id, key, location) {
  _callStorageFunction(_removeItem, arguments, location);
};

/**
 * Clears the local disk storage identified by the given ID.
 *
 * Set setItem() for details on storage types.
 *
 * @param api the flash interface if flash is available.
 * @param id the storage ID to use.
 * @param location an array with the preferred types of storage to use.
 */
util$1.clearItems = function(api, id, location) {
  _callStorageFunction(_clearItems, arguments, location);
};

/**
 * Check if an object is empty.
 *
 * Taken from:
 * http://stackoverflow.com/questions/679915/how-do-i-test-for-an-empty-javascript-object-from-json/679937#679937
 *
 * @param object the object to check.
 */
util$1.isEmpty = function(obj) {
  for(var prop in obj) {
    if(obj.hasOwnProperty(prop)) {
      return false;
    }
  }
  return true;
};

/**
 * Format with simple printf-style interpolation.
 *
 * %%: literal '%'
 * %s,%o: convert next argument into a string.
 *
 * @param format the string to format.
 * @param ... arguments to interpolate into the format string.
 */
util$1.format = function(format) {
  var re = /%./g;
  // current match
  var match;
  // current part
  var part;
  // current arg index
  var argi = 0;
  // collected parts to recombine later
  var parts = [];
  // last index found
  var last = 0;
  // loop while matches remain
  while((match = re.exec(format))) {
    part = format.substring(last, re.lastIndex - 2);
    // don't add empty strings (ie, parts between %s%s)
    if(part.length > 0) {
      parts.push(part);
    }
    last = re.lastIndex;
    // switch on % code
    var code = match[0][1];
    switch(code) {
    case 's':
    case 'o':
      // check if enough arguments were given
      if(argi < arguments.length) {
        parts.push(arguments[argi++ + 1]);
      } else {
        parts.push('<?>');
      }
      break;
    // FIXME: do proper formating for numbers, etc
    //case 'f':
    //case 'd':
    case '%':
      parts.push('%');
      break;
    default:
      parts.push('<%' + code + '?>');
    }
  }
  // add trailing part of format string
  parts.push(format.substring(last));
  return parts.join('');
};

/**
 * Formats a number.
 *
 * http://snipplr.com/view/5945/javascript-numberformat--ported-from-php/
 */
util$1.formatNumber = function(number, decimals, dec_point, thousands_sep) {
  // http://kevin.vanzonneveld.net
  // +   original by: Jonas Raoni Soares Silva (http://www.jsfromhell.com)
  // +   improved by: Kevin van Zonneveld (http://kevin.vanzonneveld.net)
  // +     bugfix by: Michael White (http://crestidg.com)
  // +     bugfix by: Benjamin Lupton
  // +     bugfix by: Allan Jensen (http://www.winternet.no)
  // +    revised by: Jonas Raoni Soares Silva (http://www.jsfromhell.com)
  // *     example 1: number_format(1234.5678, 2, '.', '');
  // *     returns 1: 1234.57

  var n = number, c = isNaN(decimals = Math.abs(decimals)) ? 2 : decimals;
  var d = dec_point === undefined ? ',' : dec_point;
  var t = thousands_sep === undefined ?
   '.' : thousands_sep, s = n < 0 ? '-' : '';
  var i = parseInt((n = Math.abs(+n || 0).toFixed(c)), 10) + '';
  var j = (i.length > 3) ? i.length % 3 : 0;
  return s + (j ? i.substr(0, j) + t : '') +
    i.substr(j).replace(/(\d{3})(?=\d)/g, '$1' + t) +
    (c ? d + Math.abs(n - i).toFixed(c).slice(2) : '');
};

/**
 * Formats a byte size.
 *
 * http://snipplr.com/view/5949/format-humanize-file-byte-size-presentation-in-javascript/
 */
util$1.formatSize = function(size) {
  if(size >= 1073741824) {
    size = util$1.formatNumber(size / 1073741824, 2, '.', '') + ' GiB';
  } else if(size >= 1048576) {
    size = util$1.formatNumber(size / 1048576, 2, '.', '') + ' MiB';
  } else if(size >= 1024) {
    size = util$1.formatNumber(size / 1024, 0) + ' KiB';
  } else {
    size = util$1.formatNumber(size, 0) + ' bytes';
  }
  return size;
};

/**
 * Converts an IPv4 or IPv6 string representation into bytes (in network order).
 *
 * @param ip the IPv4 or IPv6 address to convert.
 *
 * @return the 4-byte IPv6 or 16-byte IPv6 address or null if the address can't
 *         be parsed.
 */
util$1.bytesFromIP = function(ip) {
  if(ip.indexOf('.') !== -1) {
    return util$1.bytesFromIPv4(ip);
  }
  if(ip.indexOf(':') !== -1) {
    return util$1.bytesFromIPv6(ip);
  }
  return null;
};

/**
 * Converts an IPv4 string representation into bytes (in network order).
 *
 * @param ip the IPv4 address to convert.
 *
 * @return the 4-byte address or null if the address can't be parsed.
 */
util$1.bytesFromIPv4 = function(ip) {
  ip = ip.split('.');
  if(ip.length !== 4) {
    return null;
  }
  var b = util$1.createBuffer();
  for(var i = 0; i < ip.length; ++i) {
    var num = parseInt(ip[i], 10);
    if(isNaN(num)) {
      return null;
    }
    b.putByte(num);
  }
  return b.getBytes();
};

/**
 * Converts an IPv6 string representation into bytes (in network order).
 *
 * @param ip the IPv6 address to convert.
 *
 * @return the 16-byte address or null if the address can't be parsed.
 */
util$1.bytesFromIPv6 = function(ip) {
  var blanks = 0;
  ip = ip.split(':').filter(function(e) {
    if(e.length === 0) ++blanks;
    return true;
  });
  var zeros = (8 - ip.length + blanks) * 2;
  var b = util$1.createBuffer();
  for(var i = 0; i < 8; ++i) {
    if(!ip[i] || ip[i].length === 0) {
      b.fillWithByte(0, zeros);
      zeros = 0;
      continue;
    }
    var bytes = util$1.hexToBytes(ip[i]);
    if(bytes.length < 2) {
      b.putByte(0);
    }
    b.putBytes(bytes);
  }
  return b.getBytes();
};

/**
 * Converts 4-bytes into an IPv4 string representation or 16-bytes into
 * an IPv6 string representation. The bytes must be in network order.
 *
 * @param bytes the bytes to convert.
 *
 * @return the IPv4 or IPv6 string representation if 4 or 16 bytes,
 *         respectively, are given, otherwise null.
 */
util$1.bytesToIP = function(bytes) {
  if(bytes.length === 4) {
    return util$1.bytesToIPv4(bytes);
  }
  if(bytes.length === 16) {
    return util$1.bytesToIPv6(bytes);
  }
  return null;
};

/**
 * Converts 4-bytes into an IPv4 string representation. The bytes must be
 * in network order.
 *
 * @param bytes the bytes to convert.
 *
 * @return the IPv4 string representation or null for an invalid # of bytes.
 */
util$1.bytesToIPv4 = function(bytes) {
  if(bytes.length !== 4) {
    return null;
  }
  var ip = [];
  for(var i = 0; i < bytes.length; ++i) {
    ip.push(bytes.charCodeAt(i));
  }
  return ip.join('.');
};

/**
 * Converts 16-bytes into an IPv16 string representation. The bytes must be
 * in network order.
 *
 * @param bytes the bytes to convert.
 *
 * @return the IPv16 string representation or null for an invalid # of bytes.
 */
util$1.bytesToIPv6 = function(bytes) {
  if(bytes.length !== 16) {
    return null;
  }
  var ip = [];
  var zeroGroups = [];
  var zeroMaxGroup = 0;
  for(var i = 0; i < bytes.length; i += 2) {
    var hex = util$1.bytesToHex(bytes[i] + bytes[i + 1]);
    // canonicalize zero representation
    while(hex[0] === '0' && hex !== '0') {
      hex = hex.substr(1);
    }
    if(hex === '0') {
      var last = zeroGroups[zeroGroups.length - 1];
      var idx = ip.length;
      if(!last || idx !== last.end + 1) {
        zeroGroups.push({start: idx, end: idx});
      } else {
        last.end = idx;
        if((last.end - last.start) >
          (zeroGroups[zeroMaxGroup].end - zeroGroups[zeroMaxGroup].start)) {
          zeroMaxGroup = zeroGroups.length - 1;
        }
      }
    }
    ip.push(hex);
  }
  if(zeroGroups.length > 0) {
    var group = zeroGroups[zeroMaxGroup];
    // only shorten group of length > 0
    if(group.end - group.start > 0) {
      ip.splice(group.start, group.end - group.start + 1, '');
      if(group.start === 0) {
        ip.unshift('');
      }
      if(group.end === 7) {
        ip.push('');
      }
    }
  }
  return ip.join(':');
};

/**
 * Estimates the number of processes that can be run concurrently. If
 * creating Web Workers, keep in mind that the main JavaScript process needs
 * its own core.
 *
 * @param options the options to use:
 *          update true to force an update (not use the cached value).
 * @param callback(err, max) called once the operation completes.
 */
util$1.estimateCores = function(options, callback) {
  if(typeof options === 'function') {
    callback = options;
    options = {};
  }
  options = options || {};
  if('cores' in util$1 && !options.update) {
    return callback(null, util$1.cores);
  }
  if(typeof navigator !== 'undefined' &&
    'hardwareConcurrency' in navigator &&
    navigator.hardwareConcurrency > 0) {
    util$1.cores = navigator.hardwareConcurrency;
    return callback(null, util$1.cores);
  }
  if(typeof Worker === 'undefined') {
    // workers not available
    util$1.cores = 1;
    return callback(null, util$1.cores);
  }
  if(typeof Blob === 'undefined') {
    // can't estimate, default to 2
    util$1.cores = 2;
    return callback(null, util$1.cores);
  }

  // create worker concurrency estimation code as blob
  var blobUrl = URL.createObjectURL(new Blob(['(',
    function() {
      self.addEventListener('message', function(e) {
        // run worker for 4 ms
        var st = Date.now();
        var et = st + 4;
        self.postMessage({st: st, et: et});
      });
    }.toString(),
  ')()'], {type: 'application/javascript'}));

  // take 5 samples using 16 workers
  sample([], 5, 16);

  function sample(max, samples, numWorkers) {
    if(samples === 0) {
      // get overlap average
      var avg = Math.floor(max.reduce(function(avg, x) {
        return avg + x;
      }, 0) / max.length);
      util$1.cores = Math.max(1, avg);
      URL.revokeObjectURL(blobUrl);
      return callback(null, util$1.cores);
    }
    map(numWorkers, function(err, results) {
      max.push(reduce(numWorkers, results));
      sample(max, samples - 1, numWorkers);
    });
  }

  function map(numWorkers, callback) {
    var workers = [];
    var results = [];
    for(var i = 0; i < numWorkers; ++i) {
      var worker = new Worker(blobUrl);
      worker.addEventListener('message', function(e) {
        results.push(e.data);
        if(results.length === numWorkers) {
          for(var i = 0; i < numWorkers; ++i) {
            workers[i].terminate();
          }
          callback(null, results);
        }
      });
      workers.push(worker);
    }
    for(var i = 0; i < numWorkers; ++i) {
      workers[i].postMessage(i);
    }
  }

  function reduce(numWorkers, results) {
    // find overlapping time windows
    var overlaps = [];
    for(var n = 0; n < numWorkers; ++n) {
      var r1 = results[n];
      var overlap = overlaps[n] = [];
      for(var i = 0; i < numWorkers; ++i) {
        if(n === i) {
          continue;
        }
        var r2 = results[i];
        if((r1.st > r2.st && r1.st < r2.et) ||
          (r2.st > r1.st && r2.st < r1.et)) {
          overlap.push(i);
        }
      }
    }
    // get maximum overlaps ... don't include overlapping worker itself
    // as the main JS process was also being scheduled during the work and
    // would have to be subtracted from the estimate anyway
    return overlaps.reduce(function(max, overlap) {
      return Math.max(max, overlap.length);
    }, 0);
  }
};

/**
 * Object IDs for ASN.1.
 *
 * @author Dave Longley
 *
 * Copyright (c) 2010-2013 Digital Bazaar, Inc.
 */

var forge$q = forge$s;

forge$q.pki = forge$q.pki || {};
var oids$2 = forge$q.pki.oids = forge$q.oids = forge$q.oids || {};

// set id to name mapping and name to id mapping
function _IN(id, name) {
  oids$2[id] = name;
  oids$2[name] = id;
}
// set id to name mapping only
function _I_(id, name) {
  oids$2[id] = name;
}

// algorithm OIDs
_IN('1.2.840.113549.1.1.1', 'rsaEncryption');
// Note: md2 & md4 not implemented
//_IN('1.2.840.113549.1.1.2', 'md2WithRSAEncryption');
//_IN('1.2.840.113549.1.1.3', 'md4WithRSAEncryption');
_IN('1.2.840.113549.1.1.4', 'md5WithRSAEncryption');
_IN('1.2.840.113549.1.1.5', 'sha1WithRSAEncryption');
_IN('1.2.840.113549.1.1.7', 'RSAES-OAEP');
_IN('1.2.840.113549.1.1.8', 'mgf1');
_IN('1.2.840.113549.1.1.9', 'pSpecified');
_IN('1.2.840.113549.1.1.10', 'RSASSA-PSS');
_IN('1.2.840.113549.1.1.11', 'sha256WithRSAEncryption');
_IN('1.2.840.113549.1.1.12', 'sha384WithRSAEncryption');
_IN('1.2.840.113549.1.1.13', 'sha512WithRSAEncryption');
// Edwards-curve Digital Signature Algorithm (EdDSA) Ed25519
_IN('1.3.101.112', 'EdDSA25519');

_IN('1.2.840.10040.4.3', 'dsa-with-sha1');

_IN('1.3.14.3.2.7', 'desCBC');

_IN('1.3.14.3.2.26', 'sha1');
// Deprecated equivalent of sha1WithRSAEncryption
_IN('1.3.14.3.2.29', 'sha1WithRSASignature');
_IN('2.16.840.1.101.3.4.2.1', 'sha256');
_IN('2.16.840.1.101.3.4.2.2', 'sha384');
_IN('2.16.840.1.101.3.4.2.3', 'sha512');
_IN('2.16.840.1.101.3.4.2.4', 'sha224');
_IN('2.16.840.1.101.3.4.2.5', 'sha512-224');
_IN('2.16.840.1.101.3.4.2.6', 'sha512-256');
_IN('1.2.840.113549.2.2', 'md2');
_IN('1.2.840.113549.2.5', 'md5');

// pkcs#7 content types
_IN('1.2.840.113549.1.7.1', 'data');
_IN('1.2.840.113549.1.7.2', 'signedData');
_IN('1.2.840.113549.1.7.3', 'envelopedData');
_IN('1.2.840.113549.1.7.4', 'signedAndEnvelopedData');
_IN('1.2.840.113549.1.7.5', 'digestedData');
_IN('1.2.840.113549.1.7.6', 'encryptedData');

// pkcs#9 oids
_IN('1.2.840.113549.1.9.1', 'emailAddress');
_IN('1.2.840.113549.1.9.2', 'unstructuredName');
_IN('1.2.840.113549.1.9.3', 'contentType');
_IN('1.2.840.113549.1.9.4', 'messageDigest');
_IN('1.2.840.113549.1.9.5', 'signingTime');
_IN('1.2.840.113549.1.9.6', 'counterSignature');
_IN('1.2.840.113549.1.9.7', 'challengePassword');
_IN('1.2.840.113549.1.9.8', 'unstructuredAddress');
_IN('1.2.840.113549.1.9.14', 'extensionRequest');

_IN('1.2.840.113549.1.9.20', 'friendlyName');
_IN('1.2.840.113549.1.9.21', 'localKeyId');
_IN('1.2.840.113549.1.9.22.1', 'x509Certificate');

// pkcs#12 safe bags
_IN('1.2.840.113549.1.12.10.1.1', 'keyBag');
_IN('1.2.840.113549.1.12.10.1.2', 'pkcs8ShroudedKeyBag');
_IN('1.2.840.113549.1.12.10.1.3', 'certBag');
_IN('1.2.840.113549.1.12.10.1.4', 'crlBag');
_IN('1.2.840.113549.1.12.10.1.5', 'secretBag');
_IN('1.2.840.113549.1.12.10.1.6', 'safeContentsBag');

// password-based-encryption for pkcs#12
_IN('1.2.840.113549.1.5.13', 'pkcs5PBES2');
_IN('1.2.840.113549.1.5.12', 'pkcs5PBKDF2');

_IN('1.2.840.113549.1.12.1.1', 'pbeWithSHAAnd128BitRC4');
_IN('1.2.840.113549.1.12.1.2', 'pbeWithSHAAnd40BitRC4');
_IN('1.2.840.113549.1.12.1.3', 'pbeWithSHAAnd3-KeyTripleDES-CBC');
_IN('1.2.840.113549.1.12.1.4', 'pbeWithSHAAnd2-KeyTripleDES-CBC');
_IN('1.2.840.113549.1.12.1.5', 'pbeWithSHAAnd128BitRC2-CBC');
_IN('1.2.840.113549.1.12.1.6', 'pbewithSHAAnd40BitRC2-CBC');

// hmac OIDs
_IN('1.2.840.113549.2.7', 'hmacWithSHA1');
_IN('1.2.840.113549.2.8', 'hmacWithSHA224');
_IN('1.2.840.113549.2.9', 'hmacWithSHA256');
_IN('1.2.840.113549.2.10', 'hmacWithSHA384');
_IN('1.2.840.113549.2.11', 'hmacWithSHA512');

// symmetric key algorithm oids
_IN('1.2.840.113549.3.7', 'des-EDE3-CBC');
_IN('2.16.840.1.101.3.4.1.2', 'aes128-CBC');
_IN('2.16.840.1.101.3.4.1.22', 'aes192-CBC');
_IN('2.16.840.1.101.3.4.1.42', 'aes256-CBC');

// certificate issuer/subject OIDs
_IN('2.5.4.3', 'commonName');
_IN('2.5.4.4', 'surname');
_IN('2.5.4.5', 'serialNumber');
_IN('2.5.4.6', 'countryName');
_IN('2.5.4.7', 'localityName');
_IN('2.5.4.8', 'stateOrProvinceName');
_IN('2.5.4.9', 'streetAddress');
_IN('2.5.4.10', 'organizationName');
_IN('2.5.4.11', 'organizationalUnitName');
_IN('2.5.4.12', 'title');
_IN('2.5.4.13', 'description');
_IN('2.5.4.15', 'businessCategory');
_IN('2.5.4.17', 'postalCode');
_IN('2.5.4.42', 'givenName');
_IN('1.3.6.1.4.1.311.60.2.1.2', 'jurisdictionOfIncorporationStateOrProvinceName');
_IN('1.3.6.1.4.1.311.60.2.1.3', 'jurisdictionOfIncorporationCountryName');

// X.509 extension OIDs
_IN('2.16.840.1.113730.1.1', 'nsCertType');
_IN('2.16.840.1.113730.1.13', 'nsComment'); // deprecated in theory; still widely used
_I_('2.5.29.1', 'authorityKeyIdentifier'); // deprecated, use .35
_I_('2.5.29.2', 'keyAttributes'); // obsolete use .37 or .15
_I_('2.5.29.3', 'certificatePolicies'); // deprecated, use .32
_I_('2.5.29.4', 'keyUsageRestriction'); // obsolete use .37 or .15
_I_('2.5.29.5', 'policyMapping'); // deprecated use .33
_I_('2.5.29.6', 'subtreesConstraint'); // obsolete use .30
_I_('2.5.29.7', 'subjectAltName'); // deprecated use .17
_I_('2.5.29.8', 'issuerAltName'); // deprecated use .18
_I_('2.5.29.9', 'subjectDirectoryAttributes');
_I_('2.5.29.10', 'basicConstraints'); // deprecated use .19
_I_('2.5.29.11', 'nameConstraints'); // deprecated use .30
_I_('2.5.29.12', 'policyConstraints'); // deprecated use .36
_I_('2.5.29.13', 'basicConstraints'); // deprecated use .19
_IN('2.5.29.14', 'subjectKeyIdentifier');
_IN('2.5.29.15', 'keyUsage');
_I_('2.5.29.16', 'privateKeyUsagePeriod');
_IN('2.5.29.17', 'subjectAltName');
_IN('2.5.29.18', 'issuerAltName');
_IN('2.5.29.19', 'basicConstraints');
_I_('2.5.29.20', 'cRLNumber');
_I_('2.5.29.21', 'cRLReason');
_I_('2.5.29.22', 'expirationDate');
_I_('2.5.29.23', 'instructionCode');
_I_('2.5.29.24', 'invalidityDate');
_I_('2.5.29.25', 'cRLDistributionPoints'); // deprecated use .31
_I_('2.5.29.26', 'issuingDistributionPoint'); // deprecated use .28
_I_('2.5.29.27', 'deltaCRLIndicator');
_I_('2.5.29.28', 'issuingDistributionPoint');
_I_('2.5.29.29', 'certificateIssuer');
_I_('2.5.29.30', 'nameConstraints');
_IN('2.5.29.31', 'cRLDistributionPoints');
_IN('2.5.29.32', 'certificatePolicies');
_I_('2.5.29.33', 'policyMappings');
_I_('2.5.29.34', 'policyConstraints'); // deprecated use .36
_IN('2.5.29.35', 'authorityKeyIdentifier');
_I_('2.5.29.36', 'policyConstraints');
_IN('2.5.29.37', 'extKeyUsage');
_I_('2.5.29.46', 'freshestCRL');
_I_('2.5.29.54', 'inhibitAnyPolicy');

// extKeyUsage purposes
_IN('1.3.6.1.4.1.11129.2.4.2', 'timestampList');
_IN('1.3.6.1.5.5.7.1.1', 'authorityInfoAccess');
_IN('1.3.6.1.5.5.7.3.1', 'serverAuth');
_IN('1.3.6.1.5.5.7.3.2', 'clientAuth');
_IN('1.3.6.1.5.5.7.3.3', 'codeSigning');
_IN('1.3.6.1.5.5.7.3.4', 'emailProtection');
_IN('1.3.6.1.5.5.7.3.8', 'timeStamping');

/**
 * Javascript implementation of Abstract Syntax Notation Number One.
 *
 * @author Dave Longley
 *
 * Copyright (c) 2010-2015 Digital Bazaar, Inc.
 *
 * An API for storing data using the Abstract Syntax Notation Number One
 * format using DER (Distinguished Encoding Rules) encoding. This encoding is
 * commonly used to store data for PKI, i.e. X.509 Certificates, and this
 * implementation exists for that purpose.
 *
 * Abstract Syntax Notation Number One (ASN.1) is used to define the abstract
 * syntax of information without restricting the way the information is encoded
 * for transmission. It provides a standard that allows for open systems
 * communication. ASN.1 defines the syntax of information data and a number of
 * simple data types as well as a notation for describing them and specifying
 * values for them.
 *
 * The RSA algorithm creates public and private keys that are often stored in
 * X.509 or PKCS#X formats -- which use ASN.1 (encoded in DER format). This
 * class provides the most basic functionality required to store and load DSA
 * keys that are encoded according to ASN.1.
 *
 * The most common binary encodings for ASN.1 are BER (Basic Encoding Rules)
 * and DER (Distinguished Encoding Rules). DER is just a subset of BER that
 * has stricter requirements for how data must be encoded.
 *
 * Each ASN.1 structure has a tag (a byte identifying the ASN.1 structure type)
 * and a byte array for the value of this ASN1 structure which may be data or a
 * list of ASN.1 structures.
 *
 * Each ASN.1 structure using BER is (Tag-Length-Value):
 *
 * | byte 0 | bytes X | bytes Y |
 * |--------|---------|----------
 * |  tag   | length  |  value  |
 *
 * ASN.1 allows for tags to be of "High-tag-number form" which allows a tag to
 * be two or more octets, but that is not supported by this class. A tag is
 * only 1 byte. Bits 1-5 give the tag number (ie the data type within a
 * particular 'class'), 6 indicates whether or not the ASN.1 value is
 * constructed from other ASN.1 values, and bits 7 and 8 give the 'class'. If
 * bits 7 and 8 are both zero, the class is UNIVERSAL. If only bit 7 is set,
 * then the class is APPLICATION. If only bit 8 is set, then the class is
 * CONTEXT_SPECIFIC. If both bits 7 and 8 are set, then the class is PRIVATE.
 * The tag numbers for the data types for the class UNIVERSAL are listed below:
 *
 * UNIVERSAL 0 Reserved for use by the encoding rules
 * UNIVERSAL 1 Boolean type
 * UNIVERSAL 2 Integer type
 * UNIVERSAL 3 Bitstring type
 * UNIVERSAL 4 Octetstring type
 * UNIVERSAL 5 Null type
 * UNIVERSAL 6 Object identifier type
 * UNIVERSAL 7 Object descriptor type
 * UNIVERSAL 8 External type and Instance-of type
 * UNIVERSAL 9 Real type
 * UNIVERSAL 10 Enumerated type
 * UNIVERSAL 11 Embedded-pdv type
 * UNIVERSAL 12 UTF8String type
 * UNIVERSAL 13 Relative object identifier type
 * UNIVERSAL 14-15 Reserved for future editions
 * UNIVERSAL 16 Sequence and Sequence-of types
 * UNIVERSAL 17 Set and Set-of types
 * UNIVERSAL 18-22, 25-30 Character string types
 * UNIVERSAL 23-24 Time types
 *
 * The length of an ASN.1 structure is specified after the tag identifier.
 * There is a definite form and an indefinite form. The indefinite form may
 * be used if the encoding is constructed and not all immediately available.
 * The indefinite form is encoded using a length byte with only the 8th bit
 * set. The end of the constructed object is marked using end-of-contents
 * octets (two zero bytes).
 *
 * The definite form looks like this:
 *
 * The length may take up 1 or more bytes, it depends on the length of the
 * value of the ASN.1 structure. DER encoding requires that if the ASN.1
 * structure has a value that has a length greater than 127, more than 1 byte
 * will be used to store its length, otherwise just one byte will be used.
 * This is strict.
 *
 * In the case that the length of the ASN.1 value is less than 127, 1 octet
 * (byte) is used to store the "short form" length. The 8th bit has a value of
 * 0 indicating the length is "short form" and not "long form" and bits 7-1
 * give the length of the data. (The 8th bit is the left-most, most significant
 * bit: also known as big endian or network format).
 *
 * In the case that the length of the ASN.1 value is greater than 127, 2 to
 * 127 octets (bytes) are used to store the "long form" length. The first
 * byte's 8th bit is set to 1 to indicate the length is "long form." Bits 7-1
 * give the number of additional octets. All following octets are in base 256
 * with the most significant digit first (typical big-endian binary unsigned
 * integer storage). So, for instance, if the length of a value was 257, the
 * first byte would be set to:
 *
 * 10000010 = 130 = 0x82.
 *
 * This indicates there are 2 octets (base 256) for the length. The second and
 * third bytes (the octets just mentioned) would store the length in base 256:
 *
 * octet 2: 00000001 = 1 * 256^1 = 256
 * octet 3: 00000001 = 1 * 256^0 = 1
 * total = 257
 *
 * The algorithm for converting a js integer value of 257 to base-256 is:
 *
 * var value = 257;
 * var bytes = [];
 * bytes[0] = (value >>> 8) & 0xFF; // most significant byte first
 * bytes[1] = value & 0xFF;        // least significant byte last
 *
 * On the ASN.1 UNIVERSAL Object Identifier (OID) type:
 *
 * An OID can be written like: "value1.value2.value3...valueN"
 *
 * The DER encoding rules:
 *
 * The first byte has the value 40 * value1 + value2.
 * The following bytes, if any, encode the remaining values. Each value is
 * encoded in base 128, most significant digit first (big endian), with as
 * few digits as possible, and the most significant bit of each byte set
 * to 1 except the last in each value's encoding. For example: Given the
 * OID "1.2.840.113549", its DER encoding is (remember each byte except the
 * last one in each encoding is OR'd with 0x80):
 *
 * byte 1: 40 * 1 + 2 = 42 = 0x2A.
 * bytes 2-3: 128 * 6 + 72 = 840 = 6 72 = 6 72 = 0x0648 = 0x8648
 * bytes 4-6: 16384 * 6 + 128 * 119 + 13 = 6 119 13 = 0x06770D = 0x86F70D
 *
 * The final value is: 0x2A864886F70D.
 * The full OID (including ASN.1 tag and length of 6 bytes) is:
 * 0x06062A864886F70D
 */

var forge$p = forge$s;



/* ASN.1 API */
var asn1$6 = forge$p.asn1 = forge$p.asn1 || {};

/**
 * ASN.1 classes.
 */
asn1$6.Class = {
  UNIVERSAL:        0x00,
  APPLICATION:      0x40,
  CONTEXT_SPECIFIC: 0x80,
  PRIVATE:          0xC0
};

/**
 * ASN.1 types. Not all types are supported by this implementation, only
 * those necessary to implement a simple PKI are implemented.
 */
asn1$6.Type = {
  NONE:             0,
  BOOLEAN:          1,
  INTEGER:          2,
  BITSTRING:        3,
  OCTETSTRING:      4,
  NULL:             5,
  OID:              6,
  ODESC:            7,
  EXTERNAL:         8,
  REAL:             9,
  ENUMERATED:      10,
  EMBEDDED:        11,
  UTF8:            12,
  ROID:            13,
  SEQUENCE:        16,
  SET:             17,
  PRINTABLESTRING: 19,
  IA5STRING:       22,
  UTCTIME:         23,
  GENERALIZEDTIME: 24,
  BMPSTRING:       30
};

/**
 * Creates a new asn1 object.
 *
 * @param tagClass the tag class for the object.
 * @param type the data type (tag number) for the object.
 * @param constructed true if the asn1 object is in constructed form.
 * @param value the value for the object, if it is not constructed.
 * @param [options] the options to use:
 *          [bitStringContents] the plain BIT STRING content including padding
 *            byte.
 *
 * @return the asn1 object.
 */
asn1$6.create = function(tagClass, type, constructed, value, options) {
  /* An asn1 object has a tagClass, a type, a constructed flag, and a
    value. The value's type depends on the constructed flag. If
    constructed, it will contain a list of other asn1 objects. If not,
    it will contain the ASN.1 value as an array of bytes formatted
    according to the ASN.1 data type. */

  // remove undefined values
  if(forge$p.util.isArray(value)) {
    var tmp = [];
    for(var i = 0; i < value.length; ++i) {
      if(value[i] !== undefined) {
        tmp.push(value[i]);
      }
    }
    value = tmp;
  }

  var obj = {
    tagClass: tagClass,
    type: type,
    constructed: constructed,
    composed: constructed || forge$p.util.isArray(value),
    value: value
  };
  if(options && 'bitStringContents' in options) {
    // TODO: copy byte buffer if it's a buffer not a string
    obj.bitStringContents = options.bitStringContents;
    // TODO: add readonly flag to avoid this overhead
    // save copy to detect changes
    obj.original = asn1$6.copy(obj);
  }
  return obj;
};

/**
 * Copies an asn1 object.
 *
 * @param obj the asn1 object.
 * @param [options] copy options:
 *          [excludeBitStringContents] true to not copy bitStringContents
 *
 * @return the a copy of the asn1 object.
 */
asn1$6.copy = function(obj, options) {
  var copy;

  if(forge$p.util.isArray(obj)) {
    copy = [];
    for(var i = 0; i < obj.length; ++i) {
      copy.push(asn1$6.copy(obj[i], options));
    }
    return copy;
  }

  if(typeof obj === 'string') {
    // TODO: copy byte buffer if it's a buffer not a string
    return obj;
  }

  copy = {
    tagClass: obj.tagClass,
    type: obj.type,
    constructed: obj.constructed,
    composed: obj.composed,
    value: asn1$6.copy(obj.value, options)
  };
  if(options && !options.excludeBitStringContents) {
    // TODO: copy byte buffer if it's a buffer not a string
    copy.bitStringContents = obj.bitStringContents;
  }
  return copy;
};

/**
 * Compares asn1 objects for equality.
 *
 * Note this function does not run in constant time.
 *
 * @param obj1 the first asn1 object.
 * @param obj2 the second asn1 object.
 * @param [options] compare options:
 *          [includeBitStringContents] true to compare bitStringContents
 *
 * @return true if the asn1 objects are equal.
 */
asn1$6.equals = function(obj1, obj2, options) {
  if(forge$p.util.isArray(obj1)) {
    if(!forge$p.util.isArray(obj2)) {
      return false;
    }
    if(obj1.length !== obj2.length) {
      return false;
    }
    for(var i = 0; i < obj1.length; ++i) {
      if(!asn1$6.equals(obj1[i], obj2[i])) {
        return false;
      }
    }
    return true;
  }

  if(typeof obj1 !== typeof obj2) {
    return false;
  }

  if(typeof obj1 === 'string') {
    return obj1 === obj2;
  }

  var equal = obj1.tagClass === obj2.tagClass &&
    obj1.type === obj2.type &&
    obj1.constructed === obj2.constructed &&
    obj1.composed === obj2.composed &&
    asn1$6.equals(obj1.value, obj2.value);
  if(options && options.includeBitStringContents) {
    equal = equal && (obj1.bitStringContents === obj2.bitStringContents);
  }

  return equal;
};

/**
 * Gets the length of a BER-encoded ASN.1 value.
 *
 * In case the length is not specified, undefined is returned.
 *
 * @param b the BER-encoded ASN.1 byte buffer, starting with the first
 *          length byte.
 *
 * @return the length of the BER-encoded ASN.1 value or undefined.
 */
asn1$6.getBerValueLength = function(b) {
  // TODO: move this function and related DER/BER functions to a der.js
  // file; better abstract ASN.1 away from der/ber.
  var b2 = b.getByte();
  if(b2 === 0x80) {
    return undefined;
  }

  // see if the length is "short form" or "long form" (bit 8 set)
  var length;
  var longForm = b2 & 0x80;
  if(!longForm) {
    // length is just the first byte
    length = b2;
  } else {
    // the number of bytes the length is specified in bits 7 through 1
    // and each length byte is in big-endian base-256
    length = b.getInt((b2 & 0x7F) << 3);
  }
  return length;
};

/**
 * Check if the byte buffer has enough bytes. Throws an Error if not.
 *
 * @param bytes the byte buffer to parse from.
 * @param remaining the bytes remaining in the current parsing state.
 * @param n the number of bytes the buffer must have.
 */
function _checkBufferLength(bytes, remaining, n) {
  if(n > remaining) {
    var error = new Error('Too few bytes to parse DER.');
    error.available = bytes.length();
    error.remaining = remaining;
    error.requested = n;
    throw error;
  }
}

/**
 * Gets the length of a BER-encoded ASN.1 value.
 *
 * In case the length is not specified, undefined is returned.
 *
 * @param bytes the byte buffer to parse from.
 * @param remaining the bytes remaining in the current parsing state.
 *
 * @return the length of the BER-encoded ASN.1 value or undefined.
 */
var _getValueLength = function(bytes, remaining) {
  // TODO: move this function and related DER/BER functions to a der.js
  // file; better abstract ASN.1 away from der/ber.
  // fromDer already checked that this byte exists
  var b2 = bytes.getByte();
  remaining--;
  if(b2 === 0x80) {
    return undefined;
  }

  // see if the length is "short form" or "long form" (bit 8 set)
  var length;
  var longForm = b2 & 0x80;
  if(!longForm) {
    // length is just the first byte
    length = b2;
  } else {
    // the number of bytes the length is specified in bits 7 through 1
    // and each length byte is in big-endian base-256
    var longFormBytes = b2 & 0x7F;
    _checkBufferLength(bytes, remaining, longFormBytes);
    length = bytes.getInt(longFormBytes << 3);
  }
  // FIXME: this will only happen for 32 bit getInt with high bit set
  if(length < 0) {
    throw new Error('Negative length: ' + length);
  }
  return length;
};

/**
 * Parses an asn1 object from a byte buffer in DER format.
 *
 * @param bytes the byte buffer to parse from.
 * @param [strict] true to be strict when checking value lengths, false to
 *          allow truncated values (default: true).
 * @param [options] object with options or boolean strict flag
 *          [strict] true to be strict when checking value lengths, false to
 *            allow truncated values (default: true).
 *          [parseAllBytes] true to ensure all bytes are parsed
 *            (default: true)
 *          [decodeBitStrings] true to attempt to decode the content of
 *            BIT STRINGs (not OCTET STRINGs) using strict mode. Note that
 *            without schema support to understand the data context this can
 *            erroneously decode values that happen to be valid ASN.1. This
 *            flag will be deprecated or removed as soon as schema support is
 *            available. (default: true)
 *
 * @throws Will throw an error for various malformed input conditions.
 *
 * @return the parsed asn1 object.
 */
asn1$6.fromDer = function(bytes, options) {
  if(options === undefined) {
    options = {
      strict: true,
      parseAllBytes: true,
      decodeBitStrings: true
    };
  }
  if(typeof options === 'boolean') {
    options = {
      strict: options,
      parseAllBytes: true,
      decodeBitStrings: true
    };
  }
  if(!('strict' in options)) {
    options.strict = true;
  }
  if(!('parseAllBytes' in options)) {
    options.parseAllBytes = true;
  }
  if(!('decodeBitStrings' in options)) {
    options.decodeBitStrings = true;
  }

  // wrap in buffer if needed
  if(typeof bytes === 'string') {
    bytes = forge$p.util.createBuffer(bytes);
  }

  var byteCount = bytes.length();
  var value = _fromDer(bytes, bytes.length(), 0, options);
  if(options.parseAllBytes && bytes.length() !== 0) {
    var error = new Error('Unparsed DER bytes remain after ASN.1 parsing.');
    error.byteCount = byteCount;
    error.remaining = bytes.length();
    throw error;
  }
  return value;
};

/**
 * Internal function to parse an asn1 object from a byte buffer in DER format.
 *
 * @param bytes the byte buffer to parse from.
 * @param remaining the number of bytes remaining for this chunk.
 * @param depth the current parsing depth.
 * @param options object with same options as fromDer().
 *
 * @return the parsed asn1 object.
 */
function _fromDer(bytes, remaining, depth, options) {
  // temporary storage for consumption calculations
  var start;

  // minimum length for ASN.1 DER structure is 2
  _checkBufferLength(bytes, remaining, 2);

  // get the first byte
  var b1 = bytes.getByte();
  // consumed one byte
  remaining--;

  // get the tag class
  var tagClass = (b1 & 0xC0);

  // get the type (bits 1-5)
  var type = b1 & 0x1F;

  // get the variable value length and adjust remaining bytes
  start = bytes.length();
  var length = _getValueLength(bytes, remaining);
  remaining -= start - bytes.length();

  // ensure there are enough bytes to get the value
  if(length !== undefined && length > remaining) {
    if(options.strict) {
      var error = new Error('Too few bytes to read ASN.1 value.');
      error.available = bytes.length();
      error.remaining = remaining;
      error.requested = length;
      throw error;
    }
    // Note: be lenient with truncated values and use remaining state bytes
    length = remaining;
  }

  // value storage
  var value;
  // possible BIT STRING contents storage
  var bitStringContents;

  // constructed flag is bit 6 (32 = 0x20) of the first byte
  var constructed = ((b1 & 0x20) === 0x20);
  if(constructed) {
    // parse child asn1 objects from the value
    value = [];
    if(length === undefined) {
      // asn1 object of indefinite length, read until end tag
      for(;;) {
        _checkBufferLength(bytes, remaining, 2);
        if(bytes.bytes(2) === String.fromCharCode(0, 0)) {
          bytes.getBytes(2);
          remaining -= 2;
          break;
        }
        start = bytes.length();
        value.push(_fromDer(bytes, remaining, depth + 1, options));
        remaining -= start - bytes.length();
      }
    } else {
      // parsing asn1 object of definite length
      while(length > 0) {
        start = bytes.length();
        value.push(_fromDer(bytes, length, depth + 1, options));
        remaining -= start - bytes.length();
        length -= start - bytes.length();
      }
    }
  }

  // if a BIT STRING, save the contents including padding
  if(value === undefined && tagClass === asn1$6.Class.UNIVERSAL &&
    type === asn1$6.Type.BITSTRING) {
    bitStringContents = bytes.bytes(length);
  }

  // determine if a non-constructed value should be decoded as a composed
  // value that contains other ASN.1 objects. BIT STRINGs (and OCTET STRINGs)
  // can be used this way.
  if(value === undefined && options.decodeBitStrings &&
    tagClass === asn1$6.Class.UNIVERSAL &&
    // FIXME: OCTET STRINGs not yet supported here
    // .. other parts of forge expect to decode OCTET STRINGs manually
    (type === asn1$6.Type.BITSTRING /*|| type === asn1.Type.OCTETSTRING*/) &&
    length > 1) {
    // save read position
    var savedRead = bytes.read;
    var savedRemaining = remaining;
    var unused = 0;
    if(type === asn1$6.Type.BITSTRING) {
      /* The first octet gives the number of bits by which the length of the
        bit string is less than the next multiple of eight (this is called
        the "number of unused bits").

        The second and following octets give the value of the bit string
        converted to an octet string. */
      _checkBufferLength(bytes, remaining, 1);
      unused = bytes.getByte();
      remaining--;
    }
    // if all bits are used, maybe the BIT/OCTET STRING holds ASN.1 objs
    if(unused === 0) {
      try {
        // attempt to parse child asn1 object from the value
        // (stored in array to signal composed value)
        start = bytes.length();
        var subOptions = {
          // enforce strict mode to avoid parsing ASN.1 from plain data
          strict: true,
          decodeBitStrings: true
        };
        var composed = _fromDer(bytes, remaining, depth + 1, subOptions);
        var used = start - bytes.length();
        remaining -= used;
        if(type == asn1$6.Type.BITSTRING) {
          used++;
        }

        // if the data all decoded and the class indicates UNIVERSAL or
        // CONTEXT_SPECIFIC then assume we've got an encapsulated ASN.1 object
        var tc = composed.tagClass;
        if(used === length &&
          (tc === asn1$6.Class.UNIVERSAL || tc === asn1$6.Class.CONTEXT_SPECIFIC)) {
          value = [composed];
        }
      } catch(ex) {
      }
    }
    if(value === undefined) {
      // restore read position
      bytes.read = savedRead;
      remaining = savedRemaining;
    }
  }

  if(value === undefined) {
    // asn1 not constructed or composed, get raw value
    // TODO: do DER to OID conversion and vice-versa in .toDer?

    if(length === undefined) {
      if(options.strict) {
        throw new Error('Non-constructed ASN.1 object of indefinite length.');
      }
      // be lenient and use remaining state bytes
      length = remaining;
    }

    if(type === asn1$6.Type.BMPSTRING) {
      value = '';
      for(; length > 0; length -= 2) {
        _checkBufferLength(bytes, remaining, 2);
        value += String.fromCharCode(bytes.getInt16());
        remaining -= 2;
      }
    } else {
      value = bytes.getBytes(length);
      remaining -= length;
    }
  }

  // add BIT STRING contents if available
  var asn1Options = bitStringContents === undefined ? null : {
    bitStringContents: bitStringContents
  };

  // create and return asn1 object
  return asn1$6.create(tagClass, type, constructed, value, asn1Options);
}

/**
 * Converts the given asn1 object to a buffer of bytes in DER format.
 *
 * @param asn1 the asn1 object to convert to bytes.
 *
 * @return the buffer of bytes.
 */
asn1$6.toDer = function(obj) {
  var bytes = forge$p.util.createBuffer();

  // build the first byte
  var b1 = obj.tagClass | obj.type;

  // for storing the ASN.1 value
  var value = forge$p.util.createBuffer();

  // use BIT STRING contents if available and data not changed
  var useBitStringContents = false;
  if('bitStringContents' in obj) {
    useBitStringContents = true;
    if(obj.original) {
      useBitStringContents = asn1$6.equals(obj, obj.original);
    }
  }

  if(useBitStringContents) {
    value.putBytes(obj.bitStringContents);
  } else if(obj.composed) {
    // if composed, use each child asn1 object's DER bytes as value
    // turn on 6th bit (0x20 = 32) to indicate asn1 is constructed
    // from other asn1 objects
    if(obj.constructed) {
      b1 |= 0x20;
    } else {
      // type is a bit string, add unused bits of 0x00
      value.putByte(0x00);
    }

    // add all of the child DER bytes together
    for(var i = 0; i < obj.value.length; ++i) {
      if(obj.value[i] !== undefined) {
        value.putBuffer(asn1$6.toDer(obj.value[i]));
      }
    }
  } else {
    // use asn1.value directly
    if(obj.type === asn1$6.Type.BMPSTRING) {
      for(var i = 0; i < obj.value.length; ++i) {
        value.putInt16(obj.value.charCodeAt(i));
      }
    } else {
      // ensure integer is minimally-encoded
      // TODO: should all leading bytes be stripped vs just one?
      // .. ex '00 00 01' => '01'?
      if(obj.type === asn1$6.Type.INTEGER &&
        obj.value.length > 1 &&
        // leading 0x00 for positive integer
        ((obj.value.charCodeAt(0) === 0 &&
        (obj.value.charCodeAt(1) & 0x80) === 0) ||
        // leading 0xFF for negative integer
        (obj.value.charCodeAt(0) === 0xFF &&
        (obj.value.charCodeAt(1) & 0x80) === 0x80))) {
        value.putBytes(obj.value.substr(1));
      } else {
        value.putBytes(obj.value);
      }
    }
  }

  // add tag byte
  bytes.putByte(b1);

  // use "short form" encoding
  if(value.length() <= 127) {
    // one byte describes the length
    // bit 8 = 0 and bits 7-1 = length
    bytes.putByte(value.length() & 0x7F);
  } else {
    // use "long form" encoding
    // 2 to 127 bytes describe the length
    // first byte: bit 8 = 1 and bits 7-1 = # of additional bytes
    // other bytes: length in base 256, big-endian
    var len = value.length();
    var lenBytes = '';
    do {
      lenBytes += String.fromCharCode(len & 0xFF);
      len = len >>> 8;
    } while(len > 0);

    // set first byte to # bytes used to store the length and turn on
    // bit 8 to indicate long-form length is used
    bytes.putByte(lenBytes.length | 0x80);

    // concatenate length bytes in reverse since they were generated
    // little endian and we need big endian
    for(var i = lenBytes.length - 1; i >= 0; --i) {
      bytes.putByte(lenBytes.charCodeAt(i));
    }
  }

  // concatenate value bytes
  bytes.putBuffer(value);
  return bytes;
};

/**
 * Converts an OID dot-separated string to a byte buffer. The byte buffer
 * contains only the DER-encoded value, not any tag or length bytes.
 *
 * @param oid the OID dot-separated string.
 *
 * @return the byte buffer.
 */
asn1$6.oidToDer = function(oid) {
  // split OID into individual values
  var values = oid.split('.');
  var bytes = forge$p.util.createBuffer();

  // first byte is 40 * value1 + value2
  bytes.putByte(40 * parseInt(values[0], 10) + parseInt(values[1], 10));
  // other bytes are each value in base 128 with 8th bit set except for
  // the last byte for each value
  var last, valueBytes, value, b;
  for(var i = 2; i < values.length; ++i) {
    // produce value bytes in reverse because we don't know how many
    // bytes it will take to store the value
    last = true;
    valueBytes = [];
    value = parseInt(values[i], 10);
    do {
      b = value & 0x7F;
      value = value >>> 7;
      // if value is not last, then turn on 8th bit
      if(!last) {
        b |= 0x80;
      }
      valueBytes.push(b);
      last = false;
    } while(value > 0);

    // add value bytes in reverse (needs to be in big endian)
    for(var n = valueBytes.length - 1; n >= 0; --n) {
      bytes.putByte(valueBytes[n]);
    }
  }

  return bytes;
};

/**
 * Converts a DER-encoded byte buffer to an OID dot-separated string. The
 * byte buffer should contain only the DER-encoded value, not any tag or
 * length bytes.
 *
 * @param bytes the byte buffer.
 *
 * @return the OID dot-separated string.
 */
asn1$6.derToOid = function(bytes) {
  var oid;

  // wrap in buffer if needed
  if(typeof bytes === 'string') {
    bytes = forge$p.util.createBuffer(bytes);
  }

  // first byte is 40 * value1 + value2
  var b = bytes.getByte();
  oid = Math.floor(b / 40) + '.' + (b % 40);

  // other bytes are each value in base 128 with 8th bit set except for
  // the last byte for each value
  var value = 0;
  while(bytes.length() > 0) {
    b = bytes.getByte();
    value = value << 7;
    // not the last byte for the value
    if(b & 0x80) {
      value += b & 0x7F;
    } else {
      // last byte
      oid += '.' + (value + b);
      value = 0;
    }
  }

  return oid;
};

/**
 * Converts a UTCTime value to a date.
 *
 * Note: GeneralizedTime has 4 digits for the year and is used for X.509
 * dates past 2049. Parsing that structure hasn't been implemented yet.
 *
 * @param utc the UTCTime value to convert.
 *
 * @return the date.
 */
asn1$6.utcTimeToDate = function(utc) {
  /* The following formats can be used:

    YYMMDDhhmmZ
    YYMMDDhhmm+hh'mm'
    YYMMDDhhmm-hh'mm'
    YYMMDDhhmmssZ
    YYMMDDhhmmss+hh'mm'
    YYMMDDhhmmss-hh'mm'

    Where:

    YY is the least significant two digits of the year
    MM is the month (01 to 12)
    DD is the day (01 to 31)
    hh is the hour (00 to 23)
    mm are the minutes (00 to 59)
    ss are the seconds (00 to 59)
    Z indicates that local time is GMT, + indicates that local time is
    later than GMT, and - indicates that local time is earlier than GMT
    hh' is the absolute value of the offset from GMT in hours
    mm' is the absolute value of the offset from GMT in minutes */
  var date = new Date();

  // if YY >= 50 use 19xx, if YY < 50 use 20xx
  var year = parseInt(utc.substr(0, 2), 10);
  year = (year >= 50) ? 1900 + year : 2000 + year;
  var MM = parseInt(utc.substr(2, 2), 10) - 1; // use 0-11 for month
  var DD = parseInt(utc.substr(4, 2), 10);
  var hh = parseInt(utc.substr(6, 2), 10);
  var mm = parseInt(utc.substr(8, 2), 10);
  var ss = 0;

  // not just YYMMDDhhmmZ
  if(utc.length > 11) {
    // get character after minutes
    var c = utc.charAt(10);
    var end = 10;

    // see if seconds are present
    if(c !== '+' && c !== '-') {
      // get seconds
      ss = parseInt(utc.substr(10, 2), 10);
      end += 2;
    }
  }

  // update date
  date.setUTCFullYear(year, MM, DD);
  date.setUTCHours(hh, mm, ss, 0);

  if(end) {
    // get +/- after end of time
    c = utc.charAt(end);
    if(c === '+' || c === '-') {
      // get hours+minutes offset
      var hhoffset = parseInt(utc.substr(end + 1, 2), 10);
      var mmoffset = parseInt(utc.substr(end + 4, 2), 10);

      // calculate offset in milliseconds
      var offset = hhoffset * 60 + mmoffset;
      offset *= 60000;

      // apply offset
      if(c === '+') {
        date.setTime(+date - offset);
      } else {
        date.setTime(+date + offset);
      }
    }
  }

  return date;
};

/**
 * Converts a GeneralizedTime value to a date.
 *
 * @param gentime the GeneralizedTime value to convert.
 *
 * @return the date.
 */
asn1$6.generalizedTimeToDate = function(gentime) {
  /* The following formats can be used:

    YYYYMMDDHHMMSS
    YYYYMMDDHHMMSS.fff
    YYYYMMDDHHMMSSZ
    YYYYMMDDHHMMSS.fffZ
    YYYYMMDDHHMMSS+hh'mm'
    YYYYMMDDHHMMSS.fff+hh'mm'
    YYYYMMDDHHMMSS-hh'mm'
    YYYYMMDDHHMMSS.fff-hh'mm'

    Where:

    YYYY is the year
    MM is the month (01 to 12)
    DD is the day (01 to 31)
    hh is the hour (00 to 23)
    mm are the minutes (00 to 59)
    ss are the seconds (00 to 59)
    .fff is the second fraction, accurate to three decimal places
    Z indicates that local time is GMT, + indicates that local time is
    later than GMT, and - indicates that local time is earlier than GMT
    hh' is the absolute value of the offset from GMT in hours
    mm' is the absolute value of the offset from GMT in minutes */
  var date = new Date();

  var YYYY = parseInt(gentime.substr(0, 4), 10);
  var MM = parseInt(gentime.substr(4, 2), 10) - 1; // use 0-11 for month
  var DD = parseInt(gentime.substr(6, 2), 10);
  var hh = parseInt(gentime.substr(8, 2), 10);
  var mm = parseInt(gentime.substr(10, 2), 10);
  var ss = parseInt(gentime.substr(12, 2), 10);
  var fff = 0;
  var offset = 0;
  var isUTC = false;

  if(gentime.charAt(gentime.length - 1) === 'Z') {
    isUTC = true;
  }

  var end = gentime.length - 5, c = gentime.charAt(end);
  if(c === '+' || c === '-') {
    // get hours+minutes offset
    var hhoffset = parseInt(gentime.substr(end + 1, 2), 10);
    var mmoffset = parseInt(gentime.substr(end + 4, 2), 10);

    // calculate offset in milliseconds
    offset = hhoffset * 60 + mmoffset;
    offset *= 60000;

    // apply offset
    if(c === '+') {
      offset *= -1;
    }

    isUTC = true;
  }

  // check for second fraction
  if(gentime.charAt(14) === '.') {
    fff = parseFloat(gentime.substr(14), 10) * 1000;
  }

  if(isUTC) {
    date.setUTCFullYear(YYYY, MM, DD);
    date.setUTCHours(hh, mm, ss, fff);

    // apply offset
    date.setTime(+date + offset);
  } else {
    date.setFullYear(YYYY, MM, DD);
    date.setHours(hh, mm, ss, fff);
  }

  return date;
};

/**
 * Converts a date to a UTCTime value.
 *
 * Note: GeneralizedTime has 4 digits for the year and is used for X.509
 * dates past 2049. Converting to a GeneralizedTime hasn't been
 * implemented yet.
 *
 * @param date the date to convert.
 *
 * @return the UTCTime value.
 */
asn1$6.dateToUtcTime = function(date) {
  // TODO: validate; currently assumes proper format
  if(typeof date === 'string') {
    return date;
  }

  var rval = '';

  // create format YYMMDDhhmmssZ
  var format = [];
  format.push(('' + date.getUTCFullYear()).substr(2));
  format.push('' + (date.getUTCMonth() + 1));
  format.push('' + date.getUTCDate());
  format.push('' + date.getUTCHours());
  format.push('' + date.getUTCMinutes());
  format.push('' + date.getUTCSeconds());

  // ensure 2 digits are used for each format entry
  for(var i = 0; i < format.length; ++i) {
    if(format[i].length < 2) {
      rval += '0';
    }
    rval += format[i];
  }
  rval += 'Z';

  return rval;
};

/**
 * Converts a date to a GeneralizedTime value.
 *
 * @param date the date to convert.
 *
 * @return the GeneralizedTime value as a string.
 */
asn1$6.dateToGeneralizedTime = function(date) {
  // TODO: validate; currently assumes proper format
  if(typeof date === 'string') {
    return date;
  }

  var rval = '';

  // create format YYYYMMDDHHMMSSZ
  var format = [];
  format.push('' + date.getUTCFullYear());
  format.push('' + (date.getUTCMonth() + 1));
  format.push('' + date.getUTCDate());
  format.push('' + date.getUTCHours());
  format.push('' + date.getUTCMinutes());
  format.push('' + date.getUTCSeconds());

  // ensure 2 digits are used for each format entry
  for(var i = 0; i < format.length; ++i) {
    if(format[i].length < 2) {
      rval += '0';
    }
    rval += format[i];
  }
  rval += 'Z';

  return rval;
};

/**
 * Converts a javascript integer to a DER-encoded byte buffer to be used
 * as the value for an INTEGER type.
 *
 * @param x the integer.
 *
 * @return the byte buffer.
 */
asn1$6.integerToDer = function(x) {
  var rval = forge$p.util.createBuffer();
  if(x >= -0x80 && x < 0x80) {
    return rval.putSignedInt(x, 8);
  }
  if(x >= -0x8000 && x < 0x8000) {
    return rval.putSignedInt(x, 16);
  }
  if(x >= -0x800000 && x < 0x800000) {
    return rval.putSignedInt(x, 24);
  }
  if(x >= -0x80000000 && x < 0x80000000) {
    return rval.putSignedInt(x, 32);
  }
  var error = new Error('Integer too large; max is 32-bits.');
  error.integer = x;
  throw error;
};

/**
 * Converts a DER-encoded byte buffer to a javascript integer. This is
 * typically used to decode the value of an INTEGER type.
 *
 * @param bytes the byte buffer.
 *
 * @return the integer.
 */
asn1$6.derToInteger = function(bytes) {
  // wrap in buffer if needed
  if(typeof bytes === 'string') {
    bytes = forge$p.util.createBuffer(bytes);
  }

  var n = bytes.length() * 8;
  if(n > 32) {
    throw new Error('Integer too large; max is 32-bits.');
  }
  return bytes.getSignedInt(n);
};

/**
 * Validates that the given ASN.1 object is at least a super set of the
 * given ASN.1 structure. Only tag classes and types are checked. An
 * optional map may also be provided to capture ASN.1 values while the
 * structure is checked.
 *
 * To capture an ASN.1 value, set an object in the validator's 'capture'
 * parameter to the key to use in the capture map. To capture the full
 * ASN.1 object, specify 'captureAsn1'. To capture BIT STRING bytes, including
 * the leading unused bits counter byte, specify 'captureBitStringContents'.
 * To capture BIT STRING bytes, without the leading unused bits counter byte,
 * specify 'captureBitStringValue'.
 *
 * Objects in the validator may set a field 'optional' to true to indicate
 * that it isn't necessary to pass validation.
 *
 * @param obj the ASN.1 object to validate.
 * @param v the ASN.1 structure validator.
 * @param capture an optional map to capture values in.
 * @param errors an optional array for storing validation errors.
 *
 * @return true on success, false on failure.
 */
asn1$6.validate = function(obj, v, capture, errors) {
  var rval = false;

  // ensure tag class and type are the same if specified
  if((obj.tagClass === v.tagClass || typeof(v.tagClass) === 'undefined') &&
    (obj.type === v.type || typeof(v.type) === 'undefined')) {
    // ensure constructed flag is the same if specified
    if(obj.constructed === v.constructed ||
      typeof(v.constructed) === 'undefined') {
      rval = true;

      // handle sub values
      if(v.value && forge$p.util.isArray(v.value)) {
        var j = 0;
        for(var i = 0; rval && i < v.value.length; ++i) {
          rval = v.value[i].optional || false;
          if(obj.value[j]) {
            rval = asn1$6.validate(obj.value[j], v.value[i], capture, errors);
            if(rval) {
              ++j;
            } else if(v.value[i].optional) {
              rval = true;
            }
          }
          if(!rval && errors) {
            errors.push(
              '[' + v.name + '] ' +
              'Tag class "' + v.tagClass + '", type "' +
              v.type + '" expected value length "' +
              v.value.length + '", got "' +
              obj.value.length + '"');
          }
        }
      }

      if(rval && capture) {
        if(v.capture) {
          capture[v.capture] = obj.value;
        }
        if(v.captureAsn1) {
          capture[v.captureAsn1] = obj;
        }
        if(v.captureBitStringContents && 'bitStringContents' in obj) {
          capture[v.captureBitStringContents] = obj.bitStringContents;
        }
        if(v.captureBitStringValue && 'bitStringContents' in obj) {
          if(obj.bitStringContents.length < 2) {
            capture[v.captureBitStringValue] = '';
          } else {
            // FIXME: support unused bits with data shifting
            var unused = obj.bitStringContents.charCodeAt(0);
            if(unused !== 0) {
              throw new Error(
                'captureBitStringValue only supported for zero unused bits');
            }
            capture[v.captureBitStringValue] = obj.bitStringContents.slice(1);
          }
        }
      }
    } else if(errors) {
      errors.push(
        '[' + v.name + '] ' +
        'Expected constructed "' + v.constructed + '", got "' +
        obj.constructed + '"');
    }
  } else if(errors) {
    if(obj.tagClass !== v.tagClass) {
      errors.push(
        '[' + v.name + '] ' +
        'Expected tag class "' + v.tagClass + '", got "' +
        obj.tagClass + '"');
    }
    if(obj.type !== v.type) {
      errors.push(
        '[' + v.name + '] ' +
        'Expected type "' + v.type + '", got "' + obj.type + '"');
    }
  }
  return rval;
};

// regex for testing for non-latin characters
var _nonLatinRegex = /[^\\u0000-\\u00ff]/;

/**
 * Pretty prints an ASN.1 object to a string.
 *
 * @param obj the object to write out.
 * @param level the level in the tree.
 * @param indentation the indentation to use.
 *
 * @return the string.
 */
asn1$6.prettyPrint = function(obj, level, indentation) {
  var rval = '';

  // set default level and indentation
  level = level || 0;
  indentation = indentation || 2;

  // start new line for deep levels
  if(level > 0) {
    rval += '\n';
  }

  // create indent
  var indent = '';
  for(var i = 0; i < level * indentation; ++i) {
    indent += ' ';
  }

  // print class:type
  rval += indent + 'Tag: ';
  switch(obj.tagClass) {
  case asn1$6.Class.UNIVERSAL:
    rval += 'Universal:';
    break;
  case asn1$6.Class.APPLICATION:
    rval += 'Application:';
    break;
  case asn1$6.Class.CONTEXT_SPECIFIC:
    rval += 'Context-Specific:';
    break;
  case asn1$6.Class.PRIVATE:
    rval += 'Private:';
    break;
  }

  if(obj.tagClass === asn1$6.Class.UNIVERSAL) {
    rval += obj.type;

    // known types
    switch(obj.type) {
    case asn1$6.Type.NONE:
      rval += ' (None)';
      break;
    case asn1$6.Type.BOOLEAN:
      rval += ' (Boolean)';
      break;
    case asn1$6.Type.INTEGER:
      rval += ' (Integer)';
      break;
    case asn1$6.Type.BITSTRING:
      rval += ' (Bit string)';
      break;
    case asn1$6.Type.OCTETSTRING:
      rval += ' (Octet string)';
      break;
    case asn1$6.Type.NULL:
      rval += ' (Null)';
      break;
    case asn1$6.Type.OID:
      rval += ' (Object Identifier)';
      break;
    case asn1$6.Type.ODESC:
      rval += ' (Object Descriptor)';
      break;
    case asn1$6.Type.EXTERNAL:
      rval += ' (External or Instance of)';
      break;
    case asn1$6.Type.REAL:
      rval += ' (Real)';
      break;
    case asn1$6.Type.ENUMERATED:
      rval += ' (Enumerated)';
      break;
    case asn1$6.Type.EMBEDDED:
      rval += ' (Embedded PDV)';
      break;
    case asn1$6.Type.UTF8:
      rval += ' (UTF8)';
      break;
    case asn1$6.Type.ROID:
      rval += ' (Relative Object Identifier)';
      break;
    case asn1$6.Type.SEQUENCE:
      rval += ' (Sequence)';
      break;
    case asn1$6.Type.SET:
      rval += ' (Set)';
      break;
    case asn1$6.Type.PRINTABLESTRING:
      rval += ' (Printable String)';
      break;
    case asn1$6.Type.IA5String:
      rval += ' (IA5String (ASCII))';
      break;
    case asn1$6.Type.UTCTIME:
      rval += ' (UTC time)';
      break;
    case asn1$6.Type.GENERALIZEDTIME:
      rval += ' (Generalized time)';
      break;
    case asn1$6.Type.BMPSTRING:
      rval += ' (BMP String)';
      break;
    }
  } else {
    rval += obj.type;
  }

  rval += '\n';
  rval += indent + 'Constructed: ' + obj.constructed + '\n';

  if(obj.composed) {
    var subvalues = 0;
    var sub = '';
    for(var i = 0; i < obj.value.length; ++i) {
      if(obj.value[i] !== undefined) {
        subvalues += 1;
        sub += asn1$6.prettyPrint(obj.value[i], level + 1, indentation);
        if((i + 1) < obj.value.length) {
          sub += ',';
        }
      }
    }
    rval += indent + 'Sub values: ' + subvalues + sub;
  } else {
    rval += indent + 'Value: ';
    if(obj.type === asn1$6.Type.OID) {
      var oid = asn1$6.derToOid(obj.value);
      rval += oid;
      if(forge$p.pki && forge$p.pki.oids) {
        if(oid in forge$p.pki.oids) {
          rval += ' (' + forge$p.pki.oids[oid] + ') ';
        }
      }
    }
    if(obj.type === asn1$6.Type.INTEGER) {
      try {
        rval += asn1$6.derToInteger(obj.value);
      } catch(ex) {
        rval += '0x' + forge$p.util.bytesToHex(obj.value);
      }
    } else if(obj.type === asn1$6.Type.BITSTRING) {
      // TODO: shift bits as needed to display without padding
      if(obj.value.length > 1) {
        // remove unused bits field
        rval += '0x' + forge$p.util.bytesToHex(obj.value.slice(1));
      } else {
        rval += '(none)';
      }
      // show unused bit count
      if(obj.value.length > 0) {
        var unused = obj.value.charCodeAt(0);
        if(unused == 1) {
          rval += ' (1 unused bit shown)';
        } else if(unused > 1) {
          rval += ' (' + unused + ' unused bits shown)';
        }
      }
    } else if(obj.type === asn1$6.Type.OCTETSTRING) {
      if(!_nonLatinRegex.test(obj.value)) {
        rval += '(' + obj.value + ') ';
      }
      rval += '0x' + forge$p.util.bytesToHex(obj.value);
    } else if(obj.type === asn1$6.Type.UTF8) {
      try {
        rval += forge$p.util.decodeUtf8(obj.value);
      } catch(e) {
        if(e.message === 'URI malformed') {
          rval +=
            '0x' + forge$p.util.bytesToHex(obj.value) + ' (malformed UTF8)';
        } else {
          throw e;
        }
      }
    } else if(obj.type === asn1$6.Type.PRINTABLESTRING ||
      obj.type === asn1$6.Type.IA5String) {
      rval += obj.value;
    } else if(_nonLatinRegex.test(obj.value)) {
      rval += '0x' + forge$p.util.bytesToHex(obj.value);
    } else if(obj.value.length === 0) {
      rval += '[null]';
    } else {
      rval += obj.value;
    }
  }

  return rval;
};

/**
 * Cipher base API.
 *
 * @author Dave Longley
 *
 * Copyright (c) 2010-2014 Digital Bazaar, Inc.
 */

var forge$o = forge$s;


forge$o.cipher = forge$o.cipher || {};

// registered algorithms
forge$o.cipher.algorithms = forge$o.cipher.algorithms || {};

/**
 * Creates a cipher object that can be used to encrypt data using the given
 * algorithm and key. The algorithm may be provided as a string value for a
 * previously registered algorithm or it may be given as a cipher algorithm
 * API object.
 *
 * @param algorithm the algorithm to use, either a string or an algorithm API
 *          object.
 * @param key the key to use, as a binary-encoded string of bytes or a
 *          byte buffer.
 *
 * @return the cipher.
 */
forge$o.cipher.createCipher = function(algorithm, key) {
  var api = algorithm;
  if(typeof api === 'string') {
    api = forge$o.cipher.getAlgorithm(api);
    if(api) {
      api = api();
    }
  }
  if(!api) {
    throw new Error('Unsupported algorithm: ' + algorithm);
  }

  // assume block cipher
  return new forge$o.cipher.BlockCipher({
    algorithm: api,
    key: key,
    decrypt: false
  });
};

/**
 * Creates a decipher object that can be used to decrypt data using the given
 * algorithm and key. The algorithm may be provided as a string value for a
 * previously registered algorithm or it may be given as a cipher algorithm
 * API object.
 *
 * @param algorithm the algorithm to use, either a string or an algorithm API
 *          object.
 * @param key the key to use, as a binary-encoded string of bytes or a
 *          byte buffer.
 *
 * @return the cipher.
 */
forge$o.cipher.createDecipher = function(algorithm, key) {
  var api = algorithm;
  if(typeof api === 'string') {
    api = forge$o.cipher.getAlgorithm(api);
    if(api) {
      api = api();
    }
  }
  if(!api) {
    throw new Error('Unsupported algorithm: ' + algorithm);
  }

  // assume block cipher
  return new forge$o.cipher.BlockCipher({
    algorithm: api,
    key: key,
    decrypt: true
  });
};

/**
 * Registers an algorithm by name. If the name was already registered, the
 * algorithm API object will be overwritten.
 *
 * @param name the name of the algorithm.
 * @param algorithm the algorithm API object.
 */
forge$o.cipher.registerAlgorithm = function(name, algorithm) {
  name = name.toUpperCase();
  forge$o.cipher.algorithms[name] = algorithm;
};

/**
 * Gets a registered algorithm by name.
 *
 * @param name the name of the algorithm.
 *
 * @return the algorithm, if found, null if not.
 */
forge$o.cipher.getAlgorithm = function(name) {
  name = name.toUpperCase();
  if(name in forge$o.cipher.algorithms) {
    return forge$o.cipher.algorithms[name];
  }
  return null;
};

var BlockCipher = forge$o.cipher.BlockCipher = function(options) {
  this.algorithm = options.algorithm;
  this.mode = this.algorithm.mode;
  this.blockSize = this.mode.blockSize;
  this._finish = false;
  this._input = null;
  this.output = null;
  this._op = options.decrypt ? this.mode.decrypt : this.mode.encrypt;
  this._decrypt = options.decrypt;
  this.algorithm.initialize(options);
};

/**
 * Starts or restarts the encryption or decryption process, whichever
 * was previously configured.
 *
 * For non-GCM mode, the IV may be a binary-encoded string of bytes, an array
 * of bytes, a byte buffer, or an array of 32-bit integers. If the IV is in
 * bytes, then it must be Nb (16) bytes in length. If the IV is given in as
 * 32-bit integers, then it must be 4 integers long.
 *
 * Note: an IV is not required or used in ECB mode.
 *
 * For GCM-mode, the IV must be given as a binary-encoded string of bytes or
 * a byte buffer. The number of bytes should be 12 (96 bits) as recommended
 * by NIST SP-800-38D but another length may be given.
 *
 * @param options the options to use:
 *          iv the initialization vector to use as a binary-encoded string of
 *            bytes, null to reuse the last ciphered block from a previous
 *            update() (this "residue" method is for legacy support only).
 *          additionalData additional authentication data as a binary-encoded
 *            string of bytes, for 'GCM' mode, (default: none).
 *          tagLength desired length of authentication tag, in bits, for
 *            'GCM' mode (0-128, default: 128).
 *          tag the authentication tag to check if decrypting, as a
 *             binary-encoded string of bytes.
 *          output the output the buffer to write to, null to create one.
 */
BlockCipher.prototype.start = function(options) {
  options = options || {};
  var opts = {};
  for(var key in options) {
    opts[key] = options[key];
  }
  opts.decrypt = this._decrypt;
  this._finish = false;
  this._input = forge$o.util.createBuffer();
  this.output = options.output || forge$o.util.createBuffer();
  this.mode.start(opts);
};

/**
 * Updates the next block according to the cipher mode.
 *
 * @param input the buffer to read from.
 */
BlockCipher.prototype.update = function(input) {
  if(input) {
    // input given, so empty it into the input buffer
    this._input.putBuffer(input);
  }

  // do cipher operation until it needs more input and not finished
  while(!this._op.call(this.mode, this._input, this.output, this._finish) &&
    !this._finish) {}

  // free consumed memory from input buffer
  this._input.compact();
};

/**
 * Finishes encrypting or decrypting.
 *
 * @param pad a padding function to use in CBC mode, null for default,
 *          signature(blockSize, buffer, decrypt).
 *
 * @return true if successful, false on error.
 */
BlockCipher.prototype.finish = function(pad) {
  // backwards-compatibility w/deprecated padding API
  // Note: will overwrite padding functions even after another start() call
  if(pad && (this.mode.name === 'ECB' || this.mode.name === 'CBC')) {
    this.mode.pad = function(input) {
      return pad(this.blockSize, input, false);
    };
    this.mode.unpad = function(output) {
      return pad(this.blockSize, output, true);
    };
  }

  // build options for padding and afterFinish functions
  var options = {};
  options.decrypt = this._decrypt;

  // get # of bytes that won't fill a block
  options.overflow = this._input.length() % this.blockSize;

  if(!this._decrypt && this.mode.pad) {
    if(!this.mode.pad(this._input, options)) {
      return false;
    }
  }

  // do final update
  this._finish = true;
  this.update();

  if(this._decrypt && this.mode.unpad) {
    if(!this.mode.unpad(this.output, options)) {
      return false;
    }
  }

  if(this.mode.afterFinish) {
    if(!this.mode.afterFinish(this.output, options)) {
      return false;
    }
  }

  return true;
};

/**
 * Supported cipher modes.
 *
 * @author Dave Longley
 *
 * Copyright (c) 2010-2014 Digital Bazaar, Inc.
 */

var forge$n = forge$s;


forge$n.cipher = forge$n.cipher || {};

// supported cipher modes
var modes = forge$n.cipher.modes = forge$n.cipher.modes || {};

/** Electronic codebook (ECB) (Don't use this; it's not secure) **/

modes.ecb = function(options) {
  options = options || {};
  this.name = 'ECB';
  this.cipher = options.cipher;
  this.blockSize = options.blockSize || 16;
  this._ints = this.blockSize / 4;
  this._inBlock = new Array(this._ints);
  this._outBlock = new Array(this._ints);
};

modes.ecb.prototype.start = function(options) {};

modes.ecb.prototype.encrypt = function(input, output, finish) {
  // not enough input to encrypt
  if(input.length() < this.blockSize && !(finish && input.length() > 0)) {
    return true;
  }

  // get next block
  for(var i = 0; i < this._ints; ++i) {
    this._inBlock[i] = input.getInt32();
  }

  // encrypt block
  this.cipher.encrypt(this._inBlock, this._outBlock);

  // write output
  for(var i = 0; i < this._ints; ++i) {
    output.putInt32(this._outBlock[i]);
  }
};

modes.ecb.prototype.decrypt = function(input, output, finish) {
  // not enough input to decrypt
  if(input.length() < this.blockSize && !(finish && input.length() > 0)) {
    return true;
  }

  // get next block
  for(var i = 0; i < this._ints; ++i) {
    this._inBlock[i] = input.getInt32();
  }

  // decrypt block
  this.cipher.decrypt(this._inBlock, this._outBlock);

  // write output
  for(var i = 0; i < this._ints; ++i) {
    output.putInt32(this._outBlock[i]);
  }
};

modes.ecb.prototype.pad = function(input, options) {
  // add PKCS#7 padding to block (each pad byte is the
  // value of the number of pad bytes)
  var padding = (input.length() === this.blockSize ?
    this.blockSize : (this.blockSize - input.length()));
  input.fillWithByte(padding, padding);
  return true;
};

modes.ecb.prototype.unpad = function(output, options) {
  // check for error: input data not a multiple of blockSize
  if(options.overflow > 0) {
    return false;
  }

  // ensure padding byte count is valid
  var len = output.length();
  var count = output.at(len - 1);
  if(count > (this.blockSize << 2)) {
    return false;
  }

  // trim off padding bytes
  output.truncate(count);
  return true;
};

/** Cipher-block Chaining (CBC) **/

modes.cbc = function(options) {
  options = options || {};
  this.name = 'CBC';
  this.cipher = options.cipher;
  this.blockSize = options.blockSize || 16;
  this._ints = this.blockSize / 4;
  this._inBlock = new Array(this._ints);
  this._outBlock = new Array(this._ints);
};

modes.cbc.prototype.start = function(options) {
  // Note: legacy support for using IV residue (has security flaws)
  // if IV is null, reuse block from previous processing
  if(options.iv === null) {
    // must have a previous block
    if(!this._prev) {
      throw new Error('Invalid IV parameter.');
    }
    this._iv = this._prev.slice(0);
  } else if(!('iv' in options)) {
    throw new Error('Invalid IV parameter.');
  } else {
    // save IV as "previous" block
    this._iv = transformIV(options.iv, this.blockSize);
    this._prev = this._iv.slice(0);
  }
};

modes.cbc.prototype.encrypt = function(input, output, finish) {
  // not enough input to encrypt
  if(input.length() < this.blockSize && !(finish && input.length() > 0)) {
    return true;
  }

  // get next block
  // CBC XOR's IV (or previous block) with plaintext
  for(var i = 0; i < this._ints; ++i) {
    this._inBlock[i] = this._prev[i] ^ input.getInt32();
  }

  // encrypt block
  this.cipher.encrypt(this._inBlock, this._outBlock);

  // write output, save previous block
  for(var i = 0; i < this._ints; ++i) {
    output.putInt32(this._outBlock[i]);
  }
  this._prev = this._outBlock;
};

modes.cbc.prototype.decrypt = function(input, output, finish) {
  // not enough input to decrypt
  if(input.length() < this.blockSize && !(finish && input.length() > 0)) {
    return true;
  }

  // get next block
  for(var i = 0; i < this._ints; ++i) {
    this._inBlock[i] = input.getInt32();
  }

  // decrypt block
  this.cipher.decrypt(this._inBlock, this._outBlock);

  // write output, save previous ciphered block
  // CBC XOR's IV (or previous block) with ciphertext
  for(var i = 0; i < this._ints; ++i) {
    output.putInt32(this._prev[i] ^ this._outBlock[i]);
  }
  this._prev = this._inBlock.slice(0);
};

modes.cbc.prototype.pad = function(input, options) {
  // add PKCS#7 padding to block (each pad byte is the
  // value of the number of pad bytes)
  var padding = (input.length() === this.blockSize ?
    this.blockSize : (this.blockSize - input.length()));
  input.fillWithByte(padding, padding);
  return true;
};

modes.cbc.prototype.unpad = function(output, options) {
  // check for error: input data not a multiple of blockSize
  if(options.overflow > 0) {
    return false;
  }

  // ensure padding byte count is valid
  var len = output.length();
  var count = output.at(len - 1);
  if(count > (this.blockSize << 2)) {
    return false;
  }

  // trim off padding bytes
  output.truncate(count);
  return true;
};

/** Cipher feedback (CFB) **/

modes.cfb = function(options) {
  options = options || {};
  this.name = 'CFB';
  this.cipher = options.cipher;
  this.blockSize = options.blockSize || 16;
  this._ints = this.blockSize / 4;
  this._inBlock = null;
  this._outBlock = new Array(this._ints);
  this._partialBlock = new Array(this._ints);
  this._partialOutput = forge$n.util.createBuffer();
  this._partialBytes = 0;
};

modes.cfb.prototype.start = function(options) {
  if(!('iv' in options)) {
    throw new Error('Invalid IV parameter.');
  }
  // use IV as first input
  this._iv = transformIV(options.iv, this.blockSize);
  this._inBlock = this._iv.slice(0);
  this._partialBytes = 0;
};

modes.cfb.prototype.encrypt = function(input, output, finish) {
  // not enough input to encrypt
  var inputLength = input.length();
  if(inputLength === 0) {
    return true;
  }

  // encrypt block
  this.cipher.encrypt(this._inBlock, this._outBlock);

  // handle full block
  if(this._partialBytes === 0 && inputLength >= this.blockSize) {
    // XOR input with output, write input as output
    for(var i = 0; i < this._ints; ++i) {
      this._inBlock[i] = input.getInt32() ^ this._outBlock[i];
      output.putInt32(this._inBlock[i]);
    }
    return;
  }

  // handle partial block
  var partialBytes = (this.blockSize - inputLength) % this.blockSize;
  if(partialBytes > 0) {
    partialBytes = this.blockSize - partialBytes;
  }

  // XOR input with output, write input as partial output
  this._partialOutput.clear();
  for(var i = 0; i < this._ints; ++i) {
    this._partialBlock[i] = input.getInt32() ^ this._outBlock[i];
    this._partialOutput.putInt32(this._partialBlock[i]);
  }

  if(partialBytes > 0) {
    // block still incomplete, restore input buffer
    input.read -= this.blockSize;
  } else {
    // block complete, update input block
    for(var i = 0; i < this._ints; ++i) {
      this._inBlock[i] = this._partialBlock[i];
    }
  }

  // skip any previous partial bytes
  if(this._partialBytes > 0) {
    this._partialOutput.getBytes(this._partialBytes);
  }

  if(partialBytes > 0 && !finish) {
    output.putBytes(this._partialOutput.getBytes(
      partialBytes - this._partialBytes));
    this._partialBytes = partialBytes;
    return true;
  }

  output.putBytes(this._partialOutput.getBytes(
    inputLength - this._partialBytes));
  this._partialBytes = 0;
};

modes.cfb.prototype.decrypt = function(input, output, finish) {
  // not enough input to decrypt
  var inputLength = input.length();
  if(inputLength === 0) {
    return true;
  }

  // encrypt block (CFB always uses encryption mode)
  this.cipher.encrypt(this._inBlock, this._outBlock);

  // handle full block
  if(this._partialBytes === 0 && inputLength >= this.blockSize) {
    // XOR input with output, write input as output
    for(var i = 0; i < this._ints; ++i) {
      this._inBlock[i] = input.getInt32();
      output.putInt32(this._inBlock[i] ^ this._outBlock[i]);
    }
    return;
  }

  // handle partial block
  var partialBytes = (this.blockSize - inputLength) % this.blockSize;
  if(partialBytes > 0) {
    partialBytes = this.blockSize - partialBytes;
  }

  // XOR input with output, write input as partial output
  this._partialOutput.clear();
  for(var i = 0; i < this._ints; ++i) {
    this._partialBlock[i] = input.getInt32();
    this._partialOutput.putInt32(this._partialBlock[i] ^ this._outBlock[i]);
  }

  if(partialBytes > 0) {
    // block still incomplete, restore input buffer
    input.read -= this.blockSize;
  } else {
    // block complete, update input block
    for(var i = 0; i < this._ints; ++i) {
      this._inBlock[i] = this._partialBlock[i];
    }
  }

  // skip any previous partial bytes
  if(this._partialBytes > 0) {
    this._partialOutput.getBytes(this._partialBytes);
  }

  if(partialBytes > 0 && !finish) {
    output.putBytes(this._partialOutput.getBytes(
      partialBytes - this._partialBytes));
    this._partialBytes = partialBytes;
    return true;
  }

  output.putBytes(this._partialOutput.getBytes(
    inputLength - this._partialBytes));
  this._partialBytes = 0;
};

/** Output feedback (OFB) **/

modes.ofb = function(options) {
  options = options || {};
  this.name = 'OFB';
  this.cipher = options.cipher;
  this.blockSize = options.blockSize || 16;
  this._ints = this.blockSize / 4;
  this._inBlock = null;
  this._outBlock = new Array(this._ints);
  this._partialOutput = forge$n.util.createBuffer();
  this._partialBytes = 0;
};

modes.ofb.prototype.start = function(options) {
  if(!('iv' in options)) {
    throw new Error('Invalid IV parameter.');
  }
  // use IV as first input
  this._iv = transformIV(options.iv, this.blockSize);
  this._inBlock = this._iv.slice(0);
  this._partialBytes = 0;
};

modes.ofb.prototype.encrypt = function(input, output, finish) {
  // not enough input to encrypt
  var inputLength = input.length();
  if(input.length() === 0) {
    return true;
  }

  // encrypt block (OFB always uses encryption mode)
  this.cipher.encrypt(this._inBlock, this._outBlock);

  // handle full block
  if(this._partialBytes === 0 && inputLength >= this.blockSize) {
    // XOR input with output and update next input
    for(var i = 0; i < this._ints; ++i) {
      output.putInt32(input.getInt32() ^ this._outBlock[i]);
      this._inBlock[i] = this._outBlock[i];
    }
    return;
  }

  // handle partial block
  var partialBytes = (this.blockSize - inputLength) % this.blockSize;
  if(partialBytes > 0) {
    partialBytes = this.blockSize - partialBytes;
  }

  // XOR input with output
  this._partialOutput.clear();
  for(var i = 0; i < this._ints; ++i) {
    this._partialOutput.putInt32(input.getInt32() ^ this._outBlock[i]);
  }

  if(partialBytes > 0) {
    // block still incomplete, restore input buffer
    input.read -= this.blockSize;
  } else {
    // block complete, update input block
    for(var i = 0; i < this._ints; ++i) {
      this._inBlock[i] = this._outBlock[i];
    }
  }

  // skip any previous partial bytes
  if(this._partialBytes > 0) {
    this._partialOutput.getBytes(this._partialBytes);
  }

  if(partialBytes > 0 && !finish) {
    output.putBytes(this._partialOutput.getBytes(
      partialBytes - this._partialBytes));
    this._partialBytes = partialBytes;
    return true;
  }

  output.putBytes(this._partialOutput.getBytes(
    inputLength - this._partialBytes));
  this._partialBytes = 0;
};

modes.ofb.prototype.decrypt = modes.ofb.prototype.encrypt;

/** Counter (CTR) **/

modes.ctr = function(options) {
  options = options || {};
  this.name = 'CTR';
  this.cipher = options.cipher;
  this.blockSize = options.blockSize || 16;
  this._ints = this.blockSize / 4;
  this._inBlock = null;
  this._outBlock = new Array(this._ints);
  this._partialOutput = forge$n.util.createBuffer();
  this._partialBytes = 0;
};

modes.ctr.prototype.start = function(options) {
  if(!('iv' in options)) {
    throw new Error('Invalid IV parameter.');
  }
  // use IV as first input
  this._iv = transformIV(options.iv, this.blockSize);
  this._inBlock = this._iv.slice(0);
  this._partialBytes = 0;
};

modes.ctr.prototype.encrypt = function(input, output, finish) {
  // not enough input to encrypt
  var inputLength = input.length();
  if(inputLength === 0) {
    return true;
  }

  // encrypt block (CTR always uses encryption mode)
  this.cipher.encrypt(this._inBlock, this._outBlock);

  // handle full block
  if(this._partialBytes === 0 && inputLength >= this.blockSize) {
    // XOR input with output
    for(var i = 0; i < this._ints; ++i) {
      output.putInt32(input.getInt32() ^ this._outBlock[i]);
    }
  } else {
    // handle partial block
    var partialBytes = (this.blockSize - inputLength) % this.blockSize;
    if(partialBytes > 0) {
      partialBytes = this.blockSize - partialBytes;
    }

    // XOR input with output
    this._partialOutput.clear();
    for(var i = 0; i < this._ints; ++i) {
      this._partialOutput.putInt32(input.getInt32() ^ this._outBlock[i]);
    }

    if(partialBytes > 0) {
      // block still incomplete, restore input buffer
      input.read -= this.blockSize;
    }

    // skip any previous partial bytes
    if(this._partialBytes > 0) {
      this._partialOutput.getBytes(this._partialBytes);
    }

    if(partialBytes > 0 && !finish) {
      output.putBytes(this._partialOutput.getBytes(
        partialBytes - this._partialBytes));
      this._partialBytes = partialBytes;
      return true;
    }

    output.putBytes(this._partialOutput.getBytes(
      inputLength - this._partialBytes));
    this._partialBytes = 0;
  }

  // block complete, increment counter (input block)
  inc32(this._inBlock);
};

modes.ctr.prototype.decrypt = modes.ctr.prototype.encrypt;

/** Galois/Counter Mode (GCM) **/

modes.gcm = function(options) {
  options = options || {};
  this.name = 'GCM';
  this.cipher = options.cipher;
  this.blockSize = options.blockSize || 16;
  this._ints = this.blockSize / 4;
  this._inBlock = new Array(this._ints);
  this._outBlock = new Array(this._ints);
  this._partialOutput = forge$n.util.createBuffer();
  this._partialBytes = 0;

  // R is actually this value concatenated with 120 more zero bits, but
  // we only XOR against R so the other zeros have no effect -- we just
  // apply this value to the first integer in a block
  this._R = 0xE1000000;
};

modes.gcm.prototype.start = function(options) {
  if(!('iv' in options)) {
    throw new Error('Invalid IV parameter.');
  }
  // ensure IV is a byte buffer
  var iv = forge$n.util.createBuffer(options.iv);

  // no ciphered data processed yet
  this._cipherLength = 0;

  // default additional data is none
  var additionalData;
  if('additionalData' in options) {
    additionalData = forge$n.util.createBuffer(options.additionalData);
  } else {
    additionalData = forge$n.util.createBuffer();
  }

  // default tag length is 128 bits
  if('tagLength' in options) {
    this._tagLength = options.tagLength;
  } else {
    this._tagLength = 128;
  }

  // if tag is given, ensure tag matches tag length
  this._tag = null;
  if(options.decrypt) {
    // save tag to check later
    this._tag = forge$n.util.createBuffer(options.tag).getBytes();
    if(this._tag.length !== (this._tagLength / 8)) {
      throw new Error('Authentication tag does not match tag length.');
    }
  }

  // create tmp storage for hash calculation
  this._hashBlock = new Array(this._ints);

  // no tag generated yet
  this.tag = null;

  // generate hash subkey
  // (apply block cipher to "zero" block)
  this._hashSubkey = new Array(this._ints);
  this.cipher.encrypt([0, 0, 0, 0], this._hashSubkey);

  // generate table M
  // use 4-bit tables (32 component decomposition of a 16 byte value)
  // 8-bit tables take more space and are known to have security
  // vulnerabilities (in native implementations)
  this.componentBits = 4;
  this._m = this.generateHashTable(this._hashSubkey, this.componentBits);

  // Note: support IV length different from 96 bits? (only supporting
  // 96 bits is recommended by NIST SP-800-38D)
  // generate J_0
  var ivLength = iv.length();
  if(ivLength === 12) {
    // 96-bit IV
    this._j0 = [iv.getInt32(), iv.getInt32(), iv.getInt32(), 1];
  } else {
    // IV is NOT 96-bits
    this._j0 = [0, 0, 0, 0];
    while(iv.length() > 0) {
      this._j0 = this.ghash(
        this._hashSubkey, this._j0,
        [iv.getInt32(), iv.getInt32(), iv.getInt32(), iv.getInt32()]);
    }
    this._j0 = this.ghash(
      this._hashSubkey, this._j0, [0, 0].concat(from64To32(ivLength * 8)));
  }

  // generate ICB (initial counter block)
  this._inBlock = this._j0.slice(0);
  inc32(this._inBlock);
  this._partialBytes = 0;

  // consume authentication data
  additionalData = forge$n.util.createBuffer(additionalData);
  // save additional data length as a BE 64-bit number
  this._aDataLength = from64To32(additionalData.length() * 8);
  // pad additional data to 128 bit (16 byte) block size
  var overflow = additionalData.length() % this.blockSize;
  if(overflow) {
    additionalData.fillWithByte(0, this.blockSize - overflow);
  }
  this._s = [0, 0, 0, 0];
  while(additionalData.length() > 0) {
    this._s = this.ghash(this._hashSubkey, this._s, [
      additionalData.getInt32(),
      additionalData.getInt32(),
      additionalData.getInt32(),
      additionalData.getInt32()
    ]);
  }
};

modes.gcm.prototype.encrypt = function(input, output, finish) {
  // not enough input to encrypt
  var inputLength = input.length();
  if(inputLength === 0) {
    return true;
  }

  // encrypt block
  this.cipher.encrypt(this._inBlock, this._outBlock);

  // handle full block
  if(this._partialBytes === 0 && inputLength >= this.blockSize) {
    // XOR input with output
    for(var i = 0; i < this._ints; ++i) {
      output.putInt32(this._outBlock[i] ^= input.getInt32());
    }
    this._cipherLength += this.blockSize;
  } else {
    // handle partial block
    var partialBytes = (this.blockSize - inputLength) % this.blockSize;
    if(partialBytes > 0) {
      partialBytes = this.blockSize - partialBytes;
    }

    // XOR input with output
    this._partialOutput.clear();
    for(var i = 0; i < this._ints; ++i) {
      this._partialOutput.putInt32(input.getInt32() ^ this._outBlock[i]);
    }

    if(partialBytes <= 0 || finish) {
      // handle overflow prior to hashing
      if(finish) {
        // get block overflow
        var overflow = inputLength % this.blockSize;
        this._cipherLength += overflow;
        // truncate for hash function
        this._partialOutput.truncate(this.blockSize - overflow);
      } else {
        this._cipherLength += this.blockSize;
      }

      // get output block for hashing
      for(var i = 0; i < this._ints; ++i) {
        this._outBlock[i] = this._partialOutput.getInt32();
      }
      this._partialOutput.read -= this.blockSize;
    }

    // skip any previous partial bytes
    if(this._partialBytes > 0) {
      this._partialOutput.getBytes(this._partialBytes);
    }

    if(partialBytes > 0 && !finish) {
      // block still incomplete, restore input buffer, get partial output,
      // and return early
      input.read -= this.blockSize;
      output.putBytes(this._partialOutput.getBytes(
        partialBytes - this._partialBytes));
      this._partialBytes = partialBytes;
      return true;
    }

    output.putBytes(this._partialOutput.getBytes(
      inputLength - this._partialBytes));
    this._partialBytes = 0;
  }

  // update hash block S
  this._s = this.ghash(this._hashSubkey, this._s, this._outBlock);

  // increment counter (input block)
  inc32(this._inBlock);
};

modes.gcm.prototype.decrypt = function(input, output, finish) {
  // not enough input to decrypt
  var inputLength = input.length();
  if(inputLength < this.blockSize && !(finish && inputLength > 0)) {
    return true;
  }

  // encrypt block (GCM always uses encryption mode)
  this.cipher.encrypt(this._inBlock, this._outBlock);

  // increment counter (input block)
  inc32(this._inBlock);

  // update hash block S
  this._hashBlock[0] = input.getInt32();
  this._hashBlock[1] = input.getInt32();
  this._hashBlock[2] = input.getInt32();
  this._hashBlock[3] = input.getInt32();
  this._s = this.ghash(this._hashSubkey, this._s, this._hashBlock);

  // XOR hash input with output
  for(var i = 0; i < this._ints; ++i) {
    output.putInt32(this._outBlock[i] ^ this._hashBlock[i]);
  }

  // increment cipher data length
  if(inputLength < this.blockSize) {
    this._cipherLength += inputLength % this.blockSize;
  } else {
    this._cipherLength += this.blockSize;
  }
};

modes.gcm.prototype.afterFinish = function(output, options) {
  var rval = true;

  // handle overflow
  if(options.decrypt && options.overflow) {
    output.truncate(this.blockSize - options.overflow);
  }

  // handle authentication tag
  this.tag = forge$n.util.createBuffer();

  // concatenate additional data length with cipher length
  var lengths = this._aDataLength.concat(from64To32(this._cipherLength * 8));

  // include lengths in hash
  this._s = this.ghash(this._hashSubkey, this._s, lengths);

  // do GCTR(J_0, S)
  var tag = [];
  this.cipher.encrypt(this._j0, tag);
  for(var i = 0; i < this._ints; ++i) {
    this.tag.putInt32(this._s[i] ^ tag[i]);
  }

  // trim tag to length
  this.tag.truncate(this.tag.length() % (this._tagLength / 8));

  // check authentication tag
  if(options.decrypt && this.tag.bytes() !== this._tag) {
    rval = false;
  }

  return rval;
};

/**
 * See NIST SP-800-38D 6.3 (Algorithm 1). This function performs Galois
 * field multiplication. The field, GF(2^128), is defined by the polynomial:
 *
 * x^128 + x^7 + x^2 + x + 1
 *
 * Which is represented in little-endian binary form as: 11100001 (0xe1). When
 * the value of a coefficient is 1, a bit is set. The value R, is the
 * concatenation of this value and 120 zero bits, yielding a 128-bit value
 * which matches the block size.
 *
 * This function will multiply two elements (vectors of bytes), X and Y, in
 * the field GF(2^128). The result is initialized to zero. For each bit of
 * X (out of 128), x_i, if x_i is set, then the result is multiplied (XOR'd)
 * by the current value of Y. For each bit, the value of Y will be raised by
 * a power of x (multiplied by the polynomial x). This can be achieved by
 * shifting Y once to the right. If the current value of Y, prior to being
 * multiplied by x, has 0 as its LSB, then it is a 127th degree polynomial.
 * Otherwise, we must divide by R after shifting to find the remainder.
 *
 * @param x the first block to multiply by the second.
 * @param y the second block to multiply by the first.
 *
 * @return the block result of the multiplication.
 */
modes.gcm.prototype.multiply = function(x, y) {
  var z_i = [0, 0, 0, 0];
  var v_i = y.slice(0);

  // calculate Z_128 (block has 128 bits)
  for(var i = 0; i < 128; ++i) {
    // if x_i is 0, Z_{i+1} = Z_i (unchanged)
    // else Z_{i+1} = Z_i ^ V_i
    // get x_i by finding 32-bit int position, then left shift 1 by remainder
    var x_i = x[(i / 32) | 0] & (1 << (31 - i % 32));
    if(x_i) {
      z_i[0] ^= v_i[0];
      z_i[1] ^= v_i[1];
      z_i[2] ^= v_i[2];
      z_i[3] ^= v_i[3];
    }

    // if LSB(V_i) is 1, V_i = V_i >> 1
    // else V_i = (V_i >> 1) ^ R
    this.pow(v_i, v_i);
  }

  return z_i;
};

modes.gcm.prototype.pow = function(x, out) {
  // if LSB(x) is 1, x = x >>> 1
  // else x = (x >>> 1) ^ R
  var lsb = x[3] & 1;

  // always do x >>> 1:
  // starting with the rightmost integer, shift each integer to the right
  // one bit, pulling in the bit from the integer to the left as its top
  // most bit (do this for the last 3 integers)
  for(var i = 3; i > 0; --i) {
    out[i] = (x[i] >>> 1) | ((x[i - 1] & 1) << 31);
  }
  // shift the first integer normally
  out[0] = x[0] >>> 1;

  // if lsb was not set, then polynomial had a degree of 127 and doesn't
  // need to divided; otherwise, XOR with R to find the remainder; we only
  // need to XOR the first integer since R technically ends w/120 zero bits
  if(lsb) {
    out[0] ^= this._R;
  }
};

modes.gcm.prototype.tableMultiply = function(x) {
  // assumes 4-bit tables are used
  var z = [0, 0, 0, 0];
  for(var i = 0; i < 32; ++i) {
    var idx = (i / 8) | 0;
    var x_i = (x[idx] >>> ((7 - (i % 8)) * 4)) & 0xF;
    var ah = this._m[i][x_i];
    z[0] ^= ah[0];
    z[1] ^= ah[1];
    z[2] ^= ah[2];
    z[3] ^= ah[3];
  }
  return z;
};

/**
 * A continuing version of the GHASH algorithm that operates on a single
 * block. The hash block, last hash value (Ym) and the new block to hash
 * are given.
 *
 * @param h the hash block.
 * @param y the previous value for Ym, use [0, 0, 0, 0] for a new hash.
 * @param x the block to hash.
 *
 * @return the hashed value (Ym).
 */
modes.gcm.prototype.ghash = function(h, y, x) {
  y[0] ^= x[0];
  y[1] ^= x[1];
  y[2] ^= x[2];
  y[3] ^= x[3];
  return this.tableMultiply(y);
  //return this.multiply(y, h);
};

/**
 * Precomputes a table for multiplying against the hash subkey. This
 * mechanism provides a substantial speed increase over multiplication
 * performed without a table. The table-based multiplication this table is
 * for solves X * H by multiplying each component of X by H and then
 * composing the results together using XOR.
 *
 * This function can be used to generate tables with different bit sizes
 * for the components, however, this implementation assumes there are
 * 32 components of X (which is a 16 byte vector), therefore each component
 * takes 4-bits (so the table is constructed with bits=4).
 *
 * @param h the hash subkey.
 * @param bits the bit size for a component.
 */
modes.gcm.prototype.generateHashTable = function(h, bits) {
  // TODO: There are further optimizations that would use only the
  // first table M_0 (or some variant) along with a remainder table;
  // this can be explored in the future
  var multiplier = 8 / bits;
  var perInt = 4 * multiplier;
  var size = 16 * multiplier;
  var m = new Array(size);
  for(var i = 0; i < size; ++i) {
    var tmp = [0, 0, 0, 0];
    var idx = (i / perInt) | 0;
    var shft = ((perInt - 1 - (i % perInt)) * bits);
    tmp[idx] = (1 << (bits - 1)) << shft;
    m[i] = this.generateSubHashTable(this.multiply(tmp, h), bits);
  }
  return m;
};

/**
 * Generates a table for multiplying against the hash subkey for one
 * particular component (out of all possible component values).
 *
 * @param mid the pre-multiplied value for the middle key of the table.
 * @param bits the bit size for a component.
 */
modes.gcm.prototype.generateSubHashTable = function(mid, bits) {
  // compute the table quickly by minimizing the number of
  // POW operations -- they only need to be performed for powers of 2,
  // all other entries can be composed from those powers using XOR
  var size = 1 << bits;
  var half = size >>> 1;
  var m = new Array(size);
  m[half] = mid.slice(0);
  var i = half >>> 1;
  while(i > 0) {
    // raise m0[2 * i] and store in m0[i]
    this.pow(m[2 * i], m[i] = []);
    i >>= 1;
  }
  i = 2;
  while(i < half) {
    for(var j = 1; j < i; ++j) {
      var m_i = m[i];
      var m_j = m[j];
      m[i + j] = [
        m_i[0] ^ m_j[0],
        m_i[1] ^ m_j[1],
        m_i[2] ^ m_j[2],
        m_i[3] ^ m_j[3]
      ];
    }
    i *= 2;
  }
  m[0] = [0, 0, 0, 0];
  /* Note: We could avoid storing these by doing composition during multiply
  calculate top half using composition by speed is preferred. */
  for(i = half + 1; i < size; ++i) {
    var c = m[i ^ half];
    m[i] = [mid[0] ^ c[0], mid[1] ^ c[1], mid[2] ^ c[2], mid[3] ^ c[3]];
  }
  return m;
};

/** Utility functions */

function transformIV(iv, blockSize) {
  if(typeof iv === 'string') {
    // convert iv string into byte buffer
    iv = forge$n.util.createBuffer(iv);
  }

  if(forge$n.util.isArray(iv) && iv.length > 4) {
    // convert iv byte array into byte buffer
    var tmp = iv;
    iv = forge$n.util.createBuffer();
    for(var i = 0; i < tmp.length; ++i) {
      iv.putByte(tmp[i]);
    }
  }

  if(iv.length() < blockSize) {
    throw new Error(
      'Invalid IV length; got ' + iv.length() +
      ' bytes and expected ' + blockSize + ' bytes.');
  }

  if(!forge$n.util.isArray(iv)) {
    // convert iv byte buffer into 32-bit integer array
    var ints = [];
    var blocks = blockSize / 4;
    for(var i = 0; i < blocks; ++i) {
      ints.push(iv.getInt32());
    }
    iv = ints;
  }

  return iv;
}

function inc32(block) {
  // increment last 32 bits of block only
  block[block.length - 1] = (block[block.length - 1] + 1) & 0xFFFFFFFF;
}

function from64To32(num) {
  // convert 64-bit number to two BE Int32s
  return [(num / 0x100000000) | 0, num & 0xFFFFFFFF];
}

/**
 * Advanced Encryption Standard (AES) implementation.
 *
 * This implementation is based on the public domain library 'jscrypto' which
 * was written by:
 *
 * Emily Stark (estark@stanford.edu)
 * Mike Hamburg (mhamburg@stanford.edu)
 * Dan Boneh (dabo@cs.stanford.edu)
 *
 * Parts of this code are based on the OpenSSL implementation of AES:
 * http://www.openssl.org
 *
 * @author Dave Longley
 *
 * Copyright (c) 2010-2014 Digital Bazaar, Inc.
 */

var forge$m = forge$s;




/* AES API */
forge$m.aes = forge$m.aes || {};

/**
 * Deprecated. Instead, use:
 *
 * var cipher = forge.cipher.createCipher('AES-<mode>', key);
 * cipher.start({iv: iv});
 *
 * Creates an AES cipher object to encrypt data using the given symmetric key.
 * The output will be stored in the 'output' member of the returned cipher.
 *
 * The key and iv may be given as a string of bytes, an array of bytes,
 * a byte buffer, or an array of 32-bit words.
 *
 * @param key the symmetric key to use.
 * @param iv the initialization vector to use.
 * @param output the buffer to write to, null to create one.
 * @param mode the cipher mode to use (default: 'CBC').
 *
 * @return the cipher.
 */
forge$m.aes.startEncrypting = function(key, iv, output, mode) {
  var cipher = _createCipher$1({
    key: key,
    output: output,
    decrypt: false,
    mode: mode
  });
  cipher.start(iv);
  return cipher;
};

/**
 * Deprecated. Instead, use:
 *
 * var cipher = forge.cipher.createCipher('AES-<mode>', key);
 *
 * Creates an AES cipher object to encrypt data using the given symmetric key.
 *
 * The key may be given as a string of bytes, an array of bytes, a
 * byte buffer, or an array of 32-bit words.
 *
 * @param key the symmetric key to use.
 * @param mode the cipher mode to use (default: 'CBC').
 *
 * @return the cipher.
 */
forge$m.aes.createEncryptionCipher = function(key, mode) {
  return _createCipher$1({
    key: key,
    output: null,
    decrypt: false,
    mode: mode
  });
};

/**
 * Deprecated. Instead, use:
 *
 * var decipher = forge.cipher.createDecipher('AES-<mode>', key);
 * decipher.start({iv: iv});
 *
 * Creates an AES cipher object to decrypt data using the given symmetric key.
 * The output will be stored in the 'output' member of the returned cipher.
 *
 * The key and iv may be given as a string of bytes, an array of bytes,
 * a byte buffer, or an array of 32-bit words.
 *
 * @param key the symmetric key to use.
 * @param iv the initialization vector to use.
 * @param output the buffer to write to, null to create one.
 * @param mode the cipher mode to use (default: 'CBC').
 *
 * @return the cipher.
 */
forge$m.aes.startDecrypting = function(key, iv, output, mode) {
  var cipher = _createCipher$1({
    key: key,
    output: output,
    decrypt: true,
    mode: mode
  });
  cipher.start(iv);
  return cipher;
};

/**
 * Deprecated. Instead, use:
 *
 * var decipher = forge.cipher.createDecipher('AES-<mode>', key);
 *
 * Creates an AES cipher object to decrypt data using the given symmetric key.
 *
 * The key may be given as a string of bytes, an array of bytes, a
 * byte buffer, or an array of 32-bit words.
 *
 * @param key the symmetric key to use.
 * @param mode the cipher mode to use (default: 'CBC').
 *
 * @return the cipher.
 */
forge$m.aes.createDecryptionCipher = function(key, mode) {
  return _createCipher$1({
    key: key,
    output: null,
    decrypt: true,
    mode: mode
  });
};

/**
 * Creates a new AES cipher algorithm object.
 *
 * @param name the name of the algorithm.
 * @param mode the mode factory function.
 *
 * @return the AES algorithm object.
 */
forge$m.aes.Algorithm = function(name, mode) {
  if(!init) {
    initialize();
  }
  var self = this;
  self.name = name;
  self.mode = new mode({
    blockSize: 16,
    cipher: {
      encrypt: function(inBlock, outBlock) {
        return _updateBlock$1(self._w, inBlock, outBlock, false);
      },
      decrypt: function(inBlock, outBlock) {
        return _updateBlock$1(self._w, inBlock, outBlock, true);
      }
    }
  });
  self._init = false;
};

/**
 * Initializes this AES algorithm by expanding its key.
 *
 * @param options the options to use.
 *          key the key to use with this algorithm.
 *          decrypt true if the algorithm should be initialized for decryption,
 *            false for encryption.
 */
forge$m.aes.Algorithm.prototype.initialize = function(options) {
  if(this._init) {
    return;
  }

  var key = options.key;
  var tmp;

  /* Note: The key may be a string of bytes, an array of bytes, a byte
    buffer, or an array of 32-bit integers. If the key is in bytes, then
    it must be 16, 24, or 32 bytes in length. If it is in 32-bit
    integers, it must be 4, 6, or 8 integers long. */

  if(typeof key === 'string' &&
    (key.length === 16 || key.length === 24 || key.length === 32)) {
    // convert key string into byte buffer
    key = forge$m.util.createBuffer(key);
  } else if(forge$m.util.isArray(key) &&
    (key.length === 16 || key.length === 24 || key.length === 32)) {
    // convert key integer array into byte buffer
    tmp = key;
    key = forge$m.util.createBuffer();
    for(var i = 0; i < tmp.length; ++i) {
      key.putByte(tmp[i]);
    }
  }

  // convert key byte buffer into 32-bit integer array
  if(!forge$m.util.isArray(key)) {
    tmp = key;
    key = [];

    // key lengths of 16, 24, 32 bytes allowed
    var len = tmp.length();
    if(len === 16 || len === 24 || len === 32) {
      len = len >>> 2;
      for(var i = 0; i < len; ++i) {
        key.push(tmp.getInt32());
      }
    }
  }

  // key must be an array of 32-bit integers by now
  if(!forge$m.util.isArray(key) ||
    !(key.length === 4 || key.length === 6 || key.length === 8)) {
    throw new Error('Invalid key parameter.');
  }

  // encryption operation is always used for these modes
  var mode = this.mode.name;
  var encryptOp = (['CFB', 'OFB', 'CTR', 'GCM'].indexOf(mode) !== -1);

  // do key expansion
  this._w = _expandKey(key, options.decrypt && !encryptOp);
  this._init = true;
};

/**
 * Expands a key. Typically only used for testing.
 *
 * @param key the symmetric key to expand, as an array of 32-bit words.
 * @param decrypt true to expand for decryption, false for encryption.
 *
 * @return the expanded key.
 */
forge$m.aes._expandKey = function(key, decrypt) {
  if(!init) {
    initialize();
  }
  return _expandKey(key, decrypt);
};

/**
 * Updates a single block. Typically only used for testing.
 *
 * @param w the expanded key to use.
 * @param input an array of block-size 32-bit words.
 * @param output an array of block-size 32-bit words.
 * @param decrypt true to decrypt, false to encrypt.
 */
forge$m.aes._updateBlock = _updateBlock$1;

/** Register AES algorithms **/

registerAlgorithm$1('AES-ECB', forge$m.cipher.modes.ecb);
registerAlgorithm$1('AES-CBC', forge$m.cipher.modes.cbc);
registerAlgorithm$1('AES-CFB', forge$m.cipher.modes.cfb);
registerAlgorithm$1('AES-OFB', forge$m.cipher.modes.ofb);
registerAlgorithm$1('AES-CTR', forge$m.cipher.modes.ctr);
registerAlgorithm$1('AES-GCM', forge$m.cipher.modes.gcm);

function registerAlgorithm$1(name, mode) {
  var factory = function() {
    return new forge$m.aes.Algorithm(name, mode);
  };
  forge$m.cipher.registerAlgorithm(name, factory);
}

/** AES implementation **/

var init = false; // not yet initialized
var Nb = 4;       // number of words comprising the state (AES = 4)
var sbox;         // non-linear substitution table used in key expansion
var isbox;        // inversion of sbox
var rcon;         // round constant word array
var mix;          // mix-columns table
var imix;         // inverse mix-columns table

/**
 * Performs initialization, ie: precomputes tables to optimize for speed.
 *
 * One way to understand how AES works is to imagine that 'addition' and
 * 'multiplication' are interfaces that require certain mathematical
 * properties to hold true (ie: they are associative) but they might have
 * different implementations and produce different kinds of results ...
 * provided that their mathematical properties remain true. AES defines
 * its own methods of addition and multiplication but keeps some important
 * properties the same, ie: associativity and distributivity. The
 * explanation below tries to shed some light on how AES defines addition
 * and multiplication of bytes and 32-bit words in order to perform its
 * encryption and decryption algorithms.
 *
 * The basics:
 *
 * The AES algorithm views bytes as binary representations of polynomials
 * that have either 1 or 0 as the coefficients. It defines the addition
 * or subtraction of two bytes as the XOR operation. It also defines the
 * multiplication of two bytes as a finite field referred to as GF(2^8)
 * (Note: 'GF' means "Galois Field" which is a field that contains a finite
 * number of elements so GF(2^8) has 256 elements).
 *
 * This means that any two bytes can be represented as binary polynomials;
 * when they multiplied together and modularly reduced by an irreducible
 * polynomial of the 8th degree, the results are the field GF(2^8). The
 * specific irreducible polynomial that AES uses in hexadecimal is 0x11b.
 * This multiplication is associative with 0x01 as the identity:
 *
 * (b * 0x01 = GF(b, 0x01) = b).
 *
 * The operation GF(b, 0x02) can be performed at the byte level by left
 * shifting b once and then XOR'ing it (to perform the modular reduction)
 * with 0x11b if b is >= 128. Repeated application of the multiplication
 * of 0x02 can be used to implement the multiplication of any two bytes.
 *
 * For instance, multiplying 0x57 and 0x13, denoted as GF(0x57, 0x13), can
 * be performed by factoring 0x13 into 0x01, 0x02, and 0x10. Then these
 * factors can each be multiplied by 0x57 and then added together. To do
 * the multiplication, values for 0x57 multiplied by each of these 3 factors
 * can be precomputed and stored in a table. To add them, the values from
 * the table are XOR'd together.
 *
 * AES also defines addition and multiplication of words, that is 4-byte
 * numbers represented as polynomials of 3 degrees where the coefficients
 * are the values of the bytes.
 *
 * The word [a0, a1, a2, a3] is a polynomial a3x^3 + a2x^2 + a1x + a0.
 *
 * Addition is performed by XOR'ing like powers of x. Multiplication
 * is performed in two steps, the first is an algebriac expansion as
 * you would do normally (where addition is XOR). But the result is
 * a polynomial larger than 3 degrees and thus it cannot fit in a word. So
 * next the result is modularly reduced by an AES-specific polynomial of
 * degree 4 which will always produce a polynomial of less than 4 degrees
 * such that it will fit in a word. In AES, this polynomial is x^4 + 1.
 *
 * The modular product of two polynomials 'a' and 'b' is thus:
 *
 * d(x) = d3x^3 + d2x^2 + d1x + d0
 * with
 * d0 = GF(a0, b0) ^ GF(a3, b1) ^ GF(a2, b2) ^ GF(a1, b3)
 * d1 = GF(a1, b0) ^ GF(a0, b1) ^ GF(a3, b2) ^ GF(a2, b3)
 * d2 = GF(a2, b0) ^ GF(a1, b1) ^ GF(a0, b2) ^ GF(a3, b3)
 * d3 = GF(a3, b0) ^ GF(a2, b1) ^ GF(a1, b2) ^ GF(a0, b3)
 *
 * As a matrix:
 *
 * [d0] = [a0 a3 a2 a1][b0]
 * [d1]   [a1 a0 a3 a2][b1]
 * [d2]   [a2 a1 a0 a3][b2]
 * [d3]   [a3 a2 a1 a0][b3]
 *
 * Special polynomials defined by AES (0x02 == {02}):
 * a(x)    = {03}x^3 + {01}x^2 + {01}x + {02}
 * a^-1(x) = {0b}x^3 + {0d}x^2 + {09}x + {0e}.
 *
 * These polynomials are used in the MixColumns() and InverseMixColumns()
 * operations, respectively, to cause each element in the state to affect
 * the output (referred to as diffusing).
 *
 * RotWord() uses: a0 = a1 = a2 = {00} and a3 = {01}, which is the
 * polynomial x3.
 *
 * The ShiftRows() method modifies the last 3 rows in the state (where
 * the state is 4 words with 4 bytes per word) by shifting bytes cyclically.
 * The 1st byte in the second row is moved to the end of the row. The 1st
 * and 2nd bytes in the third row are moved to the end of the row. The 1st,
 * 2nd, and 3rd bytes are moved in the fourth row.
 *
 * More details on how AES arithmetic works:
 *
 * In the polynomial representation of binary numbers, XOR performs addition
 * and subtraction and multiplication in GF(2^8) denoted as GF(a, b)
 * corresponds with the multiplication of polynomials modulo an irreducible
 * polynomial of degree 8. In other words, for AES, GF(a, b) will multiply
 * polynomial 'a' with polynomial 'b' and then do a modular reduction by
 * an AES-specific irreducible polynomial of degree 8.
 *
 * A polynomial is irreducible if its only divisors are one and itself. For
 * the AES algorithm, this irreducible polynomial is:
 *
 * m(x) = x^8 + x^4 + x^3 + x + 1,
 *
 * or {01}{1b} in hexadecimal notation, where each coefficient is a bit:
 * 100011011 = 283 = 0x11b.
 *
 * For example, GF(0x57, 0x83) = 0xc1 because
 *
 * 0x57 = 87  = 01010111 = x^6 + x^4 + x^2 + x + 1
 * 0x85 = 131 = 10000101 = x^7 + x + 1
 *
 * (x^6 + x^4 + x^2 + x + 1) * (x^7 + x + 1)
 * =  x^13 + x^11 + x^9 + x^8 + x^7 +
 *    x^7 + x^5 + x^3 + x^2 + x +
 *    x^6 + x^4 + x^2 + x + 1
 * =  x^13 + x^11 + x^9 + x^8 + x^6 + x^5 + x^4 + x^3 + 1 = y
 *    y modulo (x^8 + x^4 + x^3 + x + 1)
 * =  x^7 + x^6 + 1.
 *
 * The modular reduction by m(x) guarantees the result will be a binary
 * polynomial of less than degree 8, so that it can fit in a byte.
 *
 * The operation to multiply a binary polynomial b with x (the polynomial
 * x in binary representation is 00000010) is:
 *
 * b_7x^8 + b_6x^7 + b_5x^6 + b_4x^5 + b_3x^4 + b_2x^3 + b_1x^2 + b_0x^1
 *
 * To get GF(b, x) we must reduce that by m(x). If b_7 is 0 (that is the
 * most significant bit is 0 in b) then the result is already reduced. If
 * it is 1, then we can reduce it by subtracting m(x) via an XOR.
 *
 * It follows that multiplication by x (00000010 or 0x02) can be implemented
 * by performing a left shift followed by a conditional bitwise XOR with
 * 0x1b. This operation on bytes is denoted by xtime(). Multiplication by
 * higher powers of x can be implemented by repeated application of xtime().
 *
 * By adding intermediate results, multiplication by any constant can be
 * implemented. For instance:
 *
 * GF(0x57, 0x13) = 0xfe because:
 *
 * xtime(b) = (b & 128) ? (b << 1 ^ 0x11b) : (b << 1)
 *
 * Note: We XOR with 0x11b instead of 0x1b because in javascript our
 * datatype for b can be larger than 1 byte, so a left shift will not
 * automatically eliminate bits that overflow a byte ... by XOR'ing the
 * overflow bit with 1 (the extra one from 0x11b) we zero it out.
 *
 * GF(0x57, 0x02) = xtime(0x57) = 0xae
 * GF(0x57, 0x04) = xtime(0xae) = 0x47
 * GF(0x57, 0x08) = xtime(0x47) = 0x8e
 * GF(0x57, 0x10) = xtime(0x8e) = 0x07
 *
 * GF(0x57, 0x13) = GF(0x57, (0x01 ^ 0x02 ^ 0x10))
 *
 * And by the distributive property (since XOR is addition and GF() is
 * multiplication):
 *
 * = GF(0x57, 0x01) ^ GF(0x57, 0x02) ^ GF(0x57, 0x10)
 * = 0x57 ^ 0xae ^ 0x07
 * = 0xfe.
 */
function initialize() {
  init = true;

  /* Populate the Rcon table. These are the values given by
    [x^(i-1),{00},{00},{00}] where x^(i-1) are powers of x (and x = 0x02)
    in the field of GF(2^8), where i starts at 1.

    rcon[0] = [0x00, 0x00, 0x00, 0x00]
    rcon[1] = [0x01, 0x00, 0x00, 0x00] 2^(1-1) = 2^0 = 1
    rcon[2] = [0x02, 0x00, 0x00, 0x00] 2^(2-1) = 2^1 = 2
    ...
    rcon[9]  = [0x1B, 0x00, 0x00, 0x00] 2^(9-1)  = 2^8 = 0x1B
    rcon[10] = [0x36, 0x00, 0x00, 0x00] 2^(10-1) = 2^9 = 0x36

    We only store the first byte because it is the only one used.
  */
  rcon = [0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36];

  // compute xtime table which maps i onto GF(i, 0x02)
  var xtime = new Array(256);
  for(var i = 0; i < 128; ++i) {
    xtime[i] = i << 1;
    xtime[i + 128] = (i + 128) << 1 ^ 0x11B;
  }

  // compute all other tables
  sbox = new Array(256);
  isbox = new Array(256);
  mix = new Array(4);
  imix = new Array(4);
  for(var i = 0; i < 4; ++i) {
    mix[i] = new Array(256);
    imix[i] = new Array(256);
  }
  var e = 0, ei = 0, e2, e4, e8, sx, sx2, me, ime;
  for(var i = 0; i < 256; ++i) {
    /* We need to generate the SubBytes() sbox and isbox tables so that
      we can perform byte substitutions. This requires us to traverse
      all of the elements in GF, find their multiplicative inverses,
      and apply to each the following affine transformation:

      bi' = bi ^ b(i + 4) mod 8 ^ b(i + 5) mod 8 ^ b(i + 6) mod 8 ^
            b(i + 7) mod 8 ^ ci
      for 0 <= i < 8, where bi is the ith bit of the byte, and ci is the
      ith bit of a byte c with the value {63} or {01100011}.

      It is possible to traverse every possible value in a Galois field
      using what is referred to as a 'generator'. There are many
      generators (128 out of 256): 3,5,6,9,11,82 to name a few. To fully
      traverse GF we iterate 255 times, multiplying by our generator
      each time.

      On each iteration we can determine the multiplicative inverse for
      the current element.

      Suppose there is an element in GF 'e'. For a given generator 'g',
      e = g^x. The multiplicative inverse of e is g^(255 - x). It turns
      out that if use the inverse of a generator as another generator
      it will produce all of the corresponding multiplicative inverses
      at the same time. For this reason, we choose 5 as our inverse
      generator because it only requires 2 multiplies and 1 add and its
      inverse, 82, requires relatively few operations as well.

      In order to apply the affine transformation, the multiplicative
      inverse 'ei' of 'e' can be repeatedly XOR'd (4 times) with a
      bit-cycling of 'ei'. To do this 'ei' is first stored in 's' and
      'x'. Then 's' is left shifted and the high bit of 's' is made the
      low bit. The resulting value is stored in 's'. Then 'x' is XOR'd
      with 's' and stored in 'x'. On each subsequent iteration the same
      operation is performed. When 4 iterations are complete, 'x' is
      XOR'd with 'c' (0x63) and the transformed value is stored in 'x'.
      For example:

      s = 01000001
      x = 01000001

      iteration 1: s = 10000010, x ^= s
      iteration 2: s = 00000101, x ^= s
      iteration 3: s = 00001010, x ^= s
      iteration 4: s = 00010100, x ^= s
      x ^= 0x63

      This can be done with a loop where s = (s << 1) | (s >> 7). However,
      it can also be done by using a single 16-bit (in this case 32-bit)
      number 'sx'. Since XOR is an associative operation, we can set 'sx'
      to 'ei' and then XOR it with 'sx' left-shifted 1,2,3, and 4 times.
      The most significant bits will flow into the high 8 bit positions
      and be correctly XOR'd with one another. All that remains will be
      to cycle the high 8 bits by XOR'ing them all with the lower 8 bits
      afterwards.

      At the same time we're populating sbox and isbox we can precompute
      the multiplication we'll need to do to do MixColumns() later.
    */

    // apply affine transformation
    sx = ei ^ (ei << 1) ^ (ei << 2) ^ (ei << 3) ^ (ei << 4);
    sx = (sx >> 8) ^ (sx & 255) ^ 0x63;

    // update tables
    sbox[e] = sx;
    isbox[sx] = e;

    /* Mixing columns is done using matrix multiplication. The columns
      that are to be mixed are each a single word in the current state.
      The state has Nb columns (4 columns). Therefore each column is a
      4 byte word. So to mix the columns in a single column 'c' where
      its rows are r0, r1, r2, and r3, we use the following matrix
      multiplication:

      [2 3 1 1]*[r0,c]=[r'0,c]
      [1 2 3 1] [r1,c] [r'1,c]
      [1 1 2 3] [r2,c] [r'2,c]
      [3 1 1 2] [r3,c] [r'3,c]

      r0, r1, r2, and r3 are each 1 byte of one of the words in the
      state (a column). To do matrix multiplication for each mixed
      column c' we multiply the corresponding row from the left matrix
      with the corresponding column from the right matrix. In total, we
      get 4 equations:

      r0,c' = 2*r0,c + 3*r1,c + 1*r2,c + 1*r3,c
      r1,c' = 1*r0,c + 2*r1,c + 3*r2,c + 1*r3,c
      r2,c' = 1*r0,c + 1*r1,c + 2*r2,c + 3*r3,c
      r3,c' = 3*r0,c + 1*r1,c + 1*r2,c + 2*r3,c

      As usual, the multiplication is as previously defined and the
      addition is XOR. In order to optimize mixing columns we can store
      the multiplication results in tables. If you think of the whole
      column as a word (it might help to visualize by mentally rotating
      the equations above by counterclockwise 90 degrees) then you can
      see that it would be useful to map the multiplications performed on
      each byte (r0, r1, r2, r3) onto a word as well. For instance, we
      could map 2*r0,1*r0,1*r0,3*r0 onto a word by storing 2*r0 in the
      highest 8 bits and 3*r0 in the lowest 8 bits (with the other two
      respectively in the middle). This means that a table can be
      constructed that uses r0 as an index to the word. We can do the
      same with r1, r2, and r3, creating a total of 4 tables.

      To construct a full c', we can just look up each byte of c in
      their respective tables and XOR the results together.

      Also, to build each table we only have to calculate the word
      for 2,1,1,3 for every byte ... which we can do on each iteration
      of this loop since we will iterate over every byte. After we have
      calculated 2,1,1,3 we can get the results for the other tables
      by cycling the byte at the end to the beginning. For instance
      we can take the result of table 2,1,1,3 and produce table 3,2,1,1
      by moving the right most byte to the left most position just like
      how you can imagine the 3 moved out of 2,1,1,3 and to the front
      to produce 3,2,1,1.

      There is another optimization in that the same multiples of
      the current element we need in order to advance our generator
      to the next iteration can be reused in performing the 2,1,1,3
      calculation. We also calculate the inverse mix column tables,
      with e,9,d,b being the inverse of 2,1,1,3.

      When we're done, and we need to actually mix columns, the first
      byte of each state word should be put through mix[0] (2,1,1,3),
      the second through mix[1] (3,2,1,1) and so forth. Then they should
      be XOR'd together to produce the fully mixed column.
    */

    // calculate mix and imix table values
    sx2 = xtime[sx];
    e2 = xtime[e];
    e4 = xtime[e2];
    e8 = xtime[e4];
    me =
      (sx2 << 24) ^  // 2
      (sx << 16) ^   // 1
      (sx << 8) ^    // 1
      (sx ^ sx2);    // 3
    ime =
      (e2 ^ e4 ^ e8) << 24 ^  // E (14)
      (e ^ e8) << 16 ^        // 9
      (e ^ e4 ^ e8) << 8 ^    // D (13)
      (e ^ e2 ^ e8);          // B (11)
    // produce each of the mix tables by rotating the 2,1,1,3 value
    for(var n = 0; n < 4; ++n) {
      mix[n][e] = me;
      imix[n][sx] = ime;
      // cycle the right most byte to the left most position
      // ie: 2,1,1,3 becomes 3,2,1,1
      me = me << 24 | me >>> 8;
      ime = ime << 24 | ime >>> 8;
    }

    // get next element and inverse
    if(e === 0) {
      // 1 is the inverse of 1
      e = ei = 1;
    } else {
      // e = 2e + 2*2*2*(10e)) = multiply e by 82 (chosen generator)
      // ei = ei + 2*2*ei = multiply ei by 5 (inverse generator)
      e = e2 ^ xtime[xtime[xtime[e2 ^ e8]]];
      ei ^= xtime[xtime[ei]];
    }
  }
}

/**
 * Generates a key schedule using the AES key expansion algorithm.
 *
 * The AES algorithm takes the Cipher Key, K, and performs a Key Expansion
 * routine to generate a key schedule. The Key Expansion generates a total
 * of Nb*(Nr + 1) words: the algorithm requires an initial set of Nb words,
 * and each of the Nr rounds requires Nb words of key data. The resulting
 * key schedule consists of a linear array of 4-byte words, denoted [wi ],
 * with i in the range 0 <= i < Nb(Nr + 1).
 *
 * KeyExpansion(byte key[4*Nk], word w[Nb*(Nr+1)], Nk)
 * AES-128 (Nb=4, Nk=4, Nr=10)
 * AES-192 (Nb=4, Nk=6, Nr=12)
 * AES-256 (Nb=4, Nk=8, Nr=14)
 * Note: Nr=Nk+6.
 *
 * Nb is the number of columns (32-bit words) comprising the State (or
 * number of bytes in a block). For AES, Nb=4.
 *
 * @param key the key to schedule (as an array of 32-bit words).
 * @param decrypt true to modify the key schedule to decrypt, false not to.
 *
 * @return the generated key schedule.
 */
function _expandKey(key, decrypt) {
  // copy the key's words to initialize the key schedule
  var w = key.slice(0);

  /* RotWord() will rotate a word, moving the first byte to the last
    byte's position (shifting the other bytes left).

    We will be getting the value of Rcon at i / Nk. 'i' will iterate
    from Nk to (Nb * Nr+1). Nk = 4 (4 byte key), Nb = 4 (4 words in
    a block), Nr = Nk + 6 (10). Therefore 'i' will iterate from
    4 to 44 (exclusive). Each time we iterate 4 times, i / Nk will
    increase by 1. We use a counter iNk to keep track of this.
   */

  // go through the rounds expanding the key
  var temp, iNk = 1;
  var Nk = w.length;
  var Nr1 = Nk + 6 + 1;
  var end = Nb * Nr1;
  for(var i = Nk; i < end; ++i) {
    temp = w[i - 1];
    if(i % Nk === 0) {
      // temp = SubWord(RotWord(temp)) ^ Rcon[i / Nk]
      temp =
        sbox[temp >>> 16 & 255] << 24 ^
        sbox[temp >>> 8 & 255] << 16 ^
        sbox[temp & 255] << 8 ^
        sbox[temp >>> 24] ^ (rcon[iNk] << 24);
      iNk++;
    } else if(Nk > 6 && (i % Nk === 4)) {
      // temp = SubWord(temp)
      temp =
        sbox[temp >>> 24] << 24 ^
        sbox[temp >>> 16 & 255] << 16 ^
        sbox[temp >>> 8 & 255] << 8 ^
        sbox[temp & 255];
    }
    w[i] = w[i - Nk] ^ temp;
  }

  /* When we are updating a cipher block we always use the code path for
     encryption whether we are decrypting or not (to shorten code and
     simplify the generation of look up tables). However, because there
     are differences in the decryption algorithm, other than just swapping
     in different look up tables, we must transform our key schedule to
     account for these changes:

     1. The decryption algorithm gets its key rounds in reverse order.
     2. The decryption algorithm adds the round key before mixing columns
       instead of afterwards.

     We don't need to modify our key schedule to handle the first case,
     we can just traverse the key schedule in reverse order when decrypting.

     The second case requires a little work.

     The tables we built for performing rounds will take an input and then
     perform SubBytes() and MixColumns() or, for the decrypt version,
     InvSubBytes() and InvMixColumns(). But the decrypt algorithm requires
     us to AddRoundKey() before InvMixColumns(). This means we'll need to
     apply some transformations to the round key to inverse-mix its columns
     so they'll be correct for moving AddRoundKey() to after the state has
     had its columns inverse-mixed.

     To inverse-mix the columns of the state when we're decrypting we use a
     lookup table that will apply InvSubBytes() and InvMixColumns() at the
     same time. However, the round key's bytes are not inverse-substituted
     in the decryption algorithm. To get around this problem, we can first
     substitute the bytes in the round key so that when we apply the
     transformation via the InvSubBytes()+InvMixColumns() table, it will
     undo our substitution leaving us with the original value that we
     want -- and then inverse-mix that value.

     This change will correctly alter our key schedule so that we can XOR
     each round key with our already transformed decryption state. This
     allows us to use the same code path as the encryption algorithm.

     We make one more change to the decryption key. Since the decryption
     algorithm runs in reverse from the encryption algorithm, we reverse
     the order of the round keys to avoid having to iterate over the key
     schedule backwards when running the encryption algorithm later in
     decryption mode. In addition to reversing the order of the round keys,
     we also swap each round key's 2nd and 4th rows. See the comments
     section where rounds are performed for more details about why this is
     done. These changes are done inline with the other substitution
     described above.
  */
  if(decrypt) {
    var tmp;
    var m0 = imix[0];
    var m1 = imix[1];
    var m2 = imix[2];
    var m3 = imix[3];
    var wnew = w.slice(0);
    end = w.length;
    for(var i = 0, wi = end - Nb; i < end; i += Nb, wi -= Nb) {
      // do not sub the first or last round key (round keys are Nb
      // words) as no column mixing is performed before they are added,
      // but do change the key order
      if(i === 0 || i === (end - Nb)) {
        wnew[i] = w[wi];
        wnew[i + 1] = w[wi + 3];
        wnew[i + 2] = w[wi + 2];
        wnew[i + 3] = w[wi + 1];
      } else {
        // substitute each round key byte because the inverse-mix
        // table will inverse-substitute it (effectively cancel the
        // substitution because round key bytes aren't sub'd in
        // decryption mode) and swap indexes 3 and 1
        for(var n = 0; n < Nb; ++n) {
          tmp = w[wi + n];
          wnew[i + (3&-n)] =
            m0[sbox[tmp >>> 24]] ^
            m1[sbox[tmp >>> 16 & 255]] ^
            m2[sbox[tmp >>> 8 & 255]] ^
            m3[sbox[tmp & 255]];
        }
      }
    }
    w = wnew;
  }

  return w;
}

/**
 * Updates a single block (16 bytes) using AES. The update will either
 * encrypt or decrypt the block.
 *
 * @param w the key schedule.
 * @param input the input block (an array of 32-bit words).
 * @param output the updated output block.
 * @param decrypt true to decrypt the block, false to encrypt it.
 */
function _updateBlock$1(w, input, output, decrypt) {
  /*
  Cipher(byte in[4*Nb], byte out[4*Nb], word w[Nb*(Nr+1)])
  begin
    byte state[4,Nb]
    state = in
    AddRoundKey(state, w[0, Nb-1])
    for round = 1 step 1 to Nr-1
      SubBytes(state)
      ShiftRows(state)
      MixColumns(state)
      AddRoundKey(state, w[round*Nb, (round+1)*Nb-1])
    end for
    SubBytes(state)
    ShiftRows(state)
    AddRoundKey(state, w[Nr*Nb, (Nr+1)*Nb-1])
    out = state
  end

  InvCipher(byte in[4*Nb], byte out[4*Nb], word w[Nb*(Nr+1)])
  begin
    byte state[4,Nb]
    state = in
    AddRoundKey(state, w[Nr*Nb, (Nr+1)*Nb-1])
    for round = Nr-1 step -1 downto 1
      InvShiftRows(state)
      InvSubBytes(state)
      AddRoundKey(state, w[round*Nb, (round+1)*Nb-1])
      InvMixColumns(state)
    end for
    InvShiftRows(state)
    InvSubBytes(state)
    AddRoundKey(state, w[0, Nb-1])
    out = state
  end
  */

  // Encrypt: AddRoundKey(state, w[0, Nb-1])
  // Decrypt: AddRoundKey(state, w[Nr*Nb, (Nr+1)*Nb-1])
  var Nr = w.length / 4 - 1;
  var m0, m1, m2, m3, sub;
  if(decrypt) {
    m0 = imix[0];
    m1 = imix[1];
    m2 = imix[2];
    m3 = imix[3];
    sub = isbox;
  } else {
    m0 = mix[0];
    m1 = mix[1];
    m2 = mix[2];
    m3 = mix[3];
    sub = sbox;
  }
  var a, b, c, d, a2, b2, c2;
  a = input[0] ^ w[0];
  b = input[decrypt ? 3 : 1] ^ w[1];
  c = input[2] ^ w[2];
  d = input[decrypt ? 1 : 3] ^ w[3];
  var i = 3;

  /* In order to share code we follow the encryption algorithm when both
    encrypting and decrypting. To account for the changes required in the
    decryption algorithm, we use different lookup tables when decrypting
    and use a modified key schedule to account for the difference in the
    order of transformations applied when performing rounds. We also get
    key rounds in reverse order (relative to encryption). */
  for(var round = 1; round < Nr; ++round) {
    /* As described above, we'll be using table lookups to perform the
      column mixing. Each column is stored as a word in the state (the
      array 'input' has one column as a word at each index). In order to
      mix a column, we perform these transformations on each row in c,
      which is 1 byte in each word. The new column for c0 is c'0:

               m0      m1      m2      m3
      r0,c'0 = 2*r0,c0 + 3*r1,c0 + 1*r2,c0 + 1*r3,c0
      r1,c'0 = 1*r0,c0 + 2*r1,c0 + 3*r2,c0 + 1*r3,c0
      r2,c'0 = 1*r0,c0 + 1*r1,c0 + 2*r2,c0 + 3*r3,c0
      r3,c'0 = 3*r0,c0 + 1*r1,c0 + 1*r2,c0 + 2*r3,c0

      So using mix tables where c0 is a word with r0 being its upper
      8 bits and r3 being its lower 8 bits:

      m0[c0 >> 24] will yield this word: [2*r0,1*r0,1*r0,3*r0]
      ...
      m3[c0 & 255] will yield this word: [1*r3,1*r3,3*r3,2*r3]

      Therefore to mix the columns in each word in the state we
      do the following (& 255 omitted for brevity):
      c'0,r0 = m0[c0 >> 24] ^ m1[c1 >> 16] ^ m2[c2 >> 8] ^ m3[c3]
      c'0,r1 = m0[c0 >> 24] ^ m1[c1 >> 16] ^ m2[c2 >> 8] ^ m3[c3]
      c'0,r2 = m0[c0 >> 24] ^ m1[c1 >> 16] ^ m2[c2 >> 8] ^ m3[c3]
      c'0,r3 = m0[c0 >> 24] ^ m1[c1 >> 16] ^ m2[c2 >> 8] ^ m3[c3]

      However, before mixing, the algorithm requires us to perform
      ShiftRows(). The ShiftRows() transformation cyclically shifts the
      last 3 rows of the state over different offsets. The first row
      (r = 0) is not shifted.

      s'_r,c = s_r,(c + shift(r, Nb) mod Nb
      for 0 < r < 4 and 0 <= c < Nb and
      shift(1, 4) = 1
      shift(2, 4) = 2
      shift(3, 4) = 3.

      This causes the first byte in r = 1 to be moved to the end of
      the row, the first 2 bytes in r = 2 to be moved to the end of
      the row, the first 3 bytes in r = 3 to be moved to the end of
      the row:

      r1: [c0 c1 c2 c3] => [c1 c2 c3 c0]
      r2: [c0 c1 c2 c3]    [c2 c3 c0 c1]
      r3: [c0 c1 c2 c3]    [c3 c0 c1 c2]

      We can make these substitutions inline with our column mixing to
      generate an updated set of equations to produce each word in the
      state (note the columns have changed positions):

      c0 c1 c2 c3 => c0 c1 c2 c3
      c0 c1 c2 c3    c1 c2 c3 c0  (cycled 1 byte)
      c0 c1 c2 c3    c2 c3 c0 c1  (cycled 2 bytes)
      c0 c1 c2 c3    c3 c0 c1 c2  (cycled 3 bytes)

      Therefore:

      c'0 = 2*r0,c0 + 3*r1,c1 + 1*r2,c2 + 1*r3,c3
      c'0 = 1*r0,c0 + 2*r1,c1 + 3*r2,c2 + 1*r3,c3
      c'0 = 1*r0,c0 + 1*r1,c1 + 2*r2,c2 + 3*r3,c3
      c'0 = 3*r0,c0 + 1*r1,c1 + 1*r2,c2 + 2*r3,c3

      c'1 = 2*r0,c1 + 3*r1,c2 + 1*r2,c3 + 1*r3,c0
      c'1 = 1*r0,c1 + 2*r1,c2 + 3*r2,c3 + 1*r3,c0
      c'1 = 1*r0,c1 + 1*r1,c2 + 2*r2,c3 + 3*r3,c0
      c'1 = 3*r0,c1 + 1*r1,c2 + 1*r2,c3 + 2*r3,c0

      ... and so forth for c'2 and c'3. The important distinction is
      that the columns are cycling, with c0 being used with the m0
      map when calculating c0, but c1 being used with the m0 map when
      calculating c1 ... and so forth.

      When performing the inverse we transform the mirror image and
      skip the bottom row, instead of the top one, and move upwards:

      c3 c2 c1 c0 => c0 c3 c2 c1  (cycled 3 bytes) *same as encryption
      c3 c2 c1 c0    c1 c0 c3 c2  (cycled 2 bytes)
      c3 c2 c1 c0    c2 c1 c0 c3  (cycled 1 byte)  *same as encryption
      c3 c2 c1 c0    c3 c2 c1 c0

      If you compare the resulting matrices for ShiftRows()+MixColumns()
      and for InvShiftRows()+InvMixColumns() the 2nd and 4th columns are
      different (in encrypt mode vs. decrypt mode). So in order to use
      the same code to handle both encryption and decryption, we will
      need to do some mapping.

      If in encryption mode we let a=c0, b=c1, c=c2, d=c3, and r<N> be
      a row number in the state, then the resulting matrix in encryption
      mode for applying the above transformations would be:

      r1: a b c d
      r2: b c d a
      r3: c d a b
      r4: d a b c

      If we did the same in decryption mode we would get:

      r1: a d c b
      r2: b a d c
      r3: c b a d
      r4: d c b a

      If instead we swap d and b (set b=c3 and d=c1), then we get:

      r1: a b c d
      r2: d a b c
      r3: c d a b
      r4: b c d a

      Now the 1st and 3rd rows are the same as the encryption matrix. All
      we need to do then to make the mapping exactly the same is to swap
      the 2nd and 4th rows when in decryption mode. To do this without
      having to do it on each iteration, we swapped the 2nd and 4th rows
      in the decryption key schedule. We also have to do the swap above
      when we first pull in the input and when we set the final output. */
    a2 =
      m0[a >>> 24] ^
      m1[b >>> 16 & 255] ^
      m2[c >>> 8 & 255] ^
      m3[d & 255] ^ w[++i];
    b2 =
      m0[b >>> 24] ^
      m1[c >>> 16 & 255] ^
      m2[d >>> 8 & 255] ^
      m3[a & 255] ^ w[++i];
    c2 =
      m0[c >>> 24] ^
      m1[d >>> 16 & 255] ^
      m2[a >>> 8 & 255] ^
      m3[b & 255] ^ w[++i];
    d =
      m0[d >>> 24] ^
      m1[a >>> 16 & 255] ^
      m2[b >>> 8 & 255] ^
      m3[c & 255] ^ w[++i];
    a = a2;
    b = b2;
    c = c2;
  }

  /*
    Encrypt:
    SubBytes(state)
    ShiftRows(state)
    AddRoundKey(state, w[Nr*Nb, (Nr+1)*Nb-1])

    Decrypt:
    InvShiftRows(state)
    InvSubBytes(state)
    AddRoundKey(state, w[0, Nb-1])
   */
  // Note: rows are shifted inline
  output[0] =
    (sub[a >>> 24] << 24) ^
    (sub[b >>> 16 & 255] << 16) ^
    (sub[c >>> 8 & 255] << 8) ^
    (sub[d & 255]) ^ w[++i];
  output[decrypt ? 3 : 1] =
    (sub[b >>> 24] << 24) ^
    (sub[c >>> 16 & 255] << 16) ^
    (sub[d >>> 8 & 255] << 8) ^
    (sub[a & 255]) ^ w[++i];
  output[2] =
    (sub[c >>> 24] << 24) ^
    (sub[d >>> 16 & 255] << 16) ^
    (sub[a >>> 8 & 255] << 8) ^
    (sub[b & 255]) ^ w[++i];
  output[decrypt ? 1 : 3] =
    (sub[d >>> 24] << 24) ^
    (sub[a >>> 16 & 255] << 16) ^
    (sub[b >>> 8 & 255] << 8) ^
    (sub[c & 255]) ^ w[++i];
}

/**
 * Deprecated. Instead, use:
 *
 * forge.cipher.createCipher('AES-<mode>', key);
 * forge.cipher.createDecipher('AES-<mode>', key);
 *
 * Creates a deprecated AES cipher object. This object's mode will default to
 * CBC (cipher-block-chaining).
 *
 * The key and iv may be given as a string of bytes, an array of bytes, a
 * byte buffer, or an array of 32-bit words.
 *
 * @param options the options to use.
 *          key the symmetric key to use.
 *          output the buffer to write to.
 *          decrypt true for decryption, false for encryption.
 *          mode the cipher mode to use (default: 'CBC').
 *
 * @return the cipher.
 */
function _createCipher$1(options) {
  options = options || {};
  var mode = (options.mode || 'CBC').toUpperCase();
  var algorithm = 'AES-' + mode;

  var cipher;
  if(options.decrypt) {
    cipher = forge$m.cipher.createDecipher(algorithm, options.key);
  } else {
    cipher = forge$m.cipher.createCipher(algorithm, options.key);
  }

  // backwards compatible start API
  var start = cipher.start;
  cipher.start = function(iv, options) {
    // backwards compatibility: support second arg as output buffer
    var output = null;
    if(options instanceof forge$m.util.ByteBuffer) {
      output = options;
      options = {};
    }
    options = options || {};
    options.output = output;
    options.iv = iv;
    start.call(cipher, options);
  };

  return cipher;
}

/**
 * DES (Data Encryption Standard) implementation.
 *
 * This implementation supports DES as well as 3DES-EDE in ECB and CBC mode.
 * It is based on the BSD-licensed implementation by Paul Tero:
 *
 * Paul Tero, July 2001
 * http://www.tero.co.uk/des/
 *
 * Optimised for performance with large blocks by
 * Michael Hayworth, November 2001
 * http://www.netdealing.com
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @author Stefan Siegl
 * @author Dave Longley
 *
 * Copyright (c) 2012 Stefan Siegl <stesie@brokenpipe.de>
 * Copyright (c) 2012-2014 Digital Bazaar, Inc.
 */

var forge$l = forge$s;




/* DES API */
forge$l.des = forge$l.des || {};

/**
 * Deprecated. Instead, use:
 *
 * var cipher = forge.cipher.createCipher('DES-<mode>', key);
 * cipher.start({iv: iv});
 *
 * Creates an DES cipher object to encrypt data using the given symmetric key.
 * The output will be stored in the 'output' member of the returned cipher.
 *
 * The key and iv may be given as binary-encoded strings of bytes or
 * byte buffers.
 *
 * @param key the symmetric key to use (64 or 192 bits).
 * @param iv the initialization vector to use.
 * @param output the buffer to write to, null to create one.
 * @param mode the cipher mode to use (default: 'CBC' if IV is
 *          given, 'ECB' if null).
 *
 * @return the cipher.
 */
forge$l.des.startEncrypting = function(key, iv, output, mode) {
  var cipher = _createCipher({
    key: key,
    output: output,
    decrypt: false,
    mode: mode || (iv === null ? 'ECB' : 'CBC')
  });
  cipher.start(iv);
  return cipher;
};

/**
 * Deprecated. Instead, use:
 *
 * var cipher = forge.cipher.createCipher('DES-<mode>', key);
 *
 * Creates an DES cipher object to encrypt data using the given symmetric key.
 *
 * The key may be given as a binary-encoded string of bytes or a byte buffer.
 *
 * @param key the symmetric key to use (64 or 192 bits).
 * @param mode the cipher mode to use (default: 'CBC').
 *
 * @return the cipher.
 */
forge$l.des.createEncryptionCipher = function(key, mode) {
  return _createCipher({
    key: key,
    output: null,
    decrypt: false,
    mode: mode
  });
};

/**
 * Deprecated. Instead, use:
 *
 * var decipher = forge.cipher.createDecipher('DES-<mode>', key);
 * decipher.start({iv: iv});
 *
 * Creates an DES cipher object to decrypt data using the given symmetric key.
 * The output will be stored in the 'output' member of the returned cipher.
 *
 * The key and iv may be given as binary-encoded strings of bytes or
 * byte buffers.
 *
 * @param key the symmetric key to use (64 or 192 bits).
 * @param iv the initialization vector to use.
 * @param output the buffer to write to, null to create one.
 * @param mode the cipher mode to use (default: 'CBC' if IV is
 *          given, 'ECB' if null).
 *
 * @return the cipher.
 */
forge$l.des.startDecrypting = function(key, iv, output, mode) {
  var cipher = _createCipher({
    key: key,
    output: output,
    decrypt: true,
    mode: mode || (iv === null ? 'ECB' : 'CBC')
  });
  cipher.start(iv);
  return cipher;
};

/**
 * Deprecated. Instead, use:
 *
 * var decipher = forge.cipher.createDecipher('DES-<mode>', key);
 *
 * Creates an DES cipher object to decrypt data using the given symmetric key.
 *
 * The key may be given as a binary-encoded string of bytes or a byte buffer.
 *
 * @param key the symmetric key to use (64 or 192 bits).
 * @param mode the cipher mode to use (default: 'CBC').
 *
 * @return the cipher.
 */
forge$l.des.createDecryptionCipher = function(key, mode) {
  return _createCipher({
    key: key,
    output: null,
    decrypt: true,
    mode: mode
  });
};

/**
 * Creates a new DES cipher algorithm object.
 *
 * @param name the name of the algorithm.
 * @param mode the mode factory function.
 *
 * @return the DES algorithm object.
 */
forge$l.des.Algorithm = function(name, mode) {
  var self = this;
  self.name = name;
  self.mode = new mode({
    blockSize: 8,
    cipher: {
      encrypt: function(inBlock, outBlock) {
        return _updateBlock(self._keys, inBlock, outBlock, false);
      },
      decrypt: function(inBlock, outBlock) {
        return _updateBlock(self._keys, inBlock, outBlock, true);
      }
    }
  });
  self._init = false;
};

/**
 * Initializes this DES algorithm by expanding its key.
 *
 * @param options the options to use.
 *          key the key to use with this algorithm.
 *          decrypt true if the algorithm should be initialized for decryption,
 *            false for encryption.
 */
forge$l.des.Algorithm.prototype.initialize = function(options) {
  if(this._init) {
    return;
  }

  var key = forge$l.util.createBuffer(options.key);
  if(this.name.indexOf('3DES') === 0) {
    if(key.length() !== 24) {
      throw new Error('Invalid Triple-DES key size: ' + key.length() * 8);
    }
  }

  // do key expansion to 16 or 48 subkeys (single or triple DES)
  this._keys = _createKeys(key);
  this._init = true;
};

/** Register DES algorithms **/

registerAlgorithm('DES-ECB', forge$l.cipher.modes.ecb);
registerAlgorithm('DES-CBC', forge$l.cipher.modes.cbc);
registerAlgorithm('DES-CFB', forge$l.cipher.modes.cfb);
registerAlgorithm('DES-OFB', forge$l.cipher.modes.ofb);
registerAlgorithm('DES-CTR', forge$l.cipher.modes.ctr);

registerAlgorithm('3DES-ECB', forge$l.cipher.modes.ecb);
registerAlgorithm('3DES-CBC', forge$l.cipher.modes.cbc);
registerAlgorithm('3DES-CFB', forge$l.cipher.modes.cfb);
registerAlgorithm('3DES-OFB', forge$l.cipher.modes.ofb);
registerAlgorithm('3DES-CTR', forge$l.cipher.modes.ctr);

function registerAlgorithm(name, mode) {
  var factory = function() {
    return new forge$l.des.Algorithm(name, mode);
  };
  forge$l.cipher.registerAlgorithm(name, factory);
}

/** DES implementation **/

var spfunction1 = [0x1010400,0,0x10000,0x1010404,0x1010004,0x10404,0x4,0x10000,0x400,0x1010400,0x1010404,0x400,0x1000404,0x1010004,0x1000000,0x4,0x404,0x1000400,0x1000400,0x10400,0x10400,0x1010000,0x1010000,0x1000404,0x10004,0x1000004,0x1000004,0x10004,0,0x404,0x10404,0x1000000,0x10000,0x1010404,0x4,0x1010000,0x1010400,0x1000000,0x1000000,0x400,0x1010004,0x10000,0x10400,0x1000004,0x400,0x4,0x1000404,0x10404,0x1010404,0x10004,0x1010000,0x1000404,0x1000004,0x404,0x10404,0x1010400,0x404,0x1000400,0x1000400,0,0x10004,0x10400,0,0x1010004];
var spfunction2 = [-0x7fef7fe0,-0x7fff8000,0x8000,0x108020,0x100000,0x20,-0x7fefffe0,-0x7fff7fe0,-0x7fffffe0,-0x7fef7fe0,-0x7fef8000,-0x80000000,-0x7fff8000,0x100000,0x20,-0x7fefffe0,0x108000,0x100020,-0x7fff7fe0,0,-0x80000000,0x8000,0x108020,-0x7ff00000,0x100020,-0x7fffffe0,0,0x108000,0x8020,-0x7fef8000,-0x7ff00000,0x8020,0,0x108020,-0x7fefffe0,0x100000,-0x7fff7fe0,-0x7ff00000,-0x7fef8000,0x8000,-0x7ff00000,-0x7fff8000,0x20,-0x7fef7fe0,0x108020,0x20,0x8000,-0x80000000,0x8020,-0x7fef8000,0x100000,-0x7fffffe0,0x100020,-0x7fff7fe0,-0x7fffffe0,0x100020,0x108000,0,-0x7fff8000,0x8020,-0x80000000,-0x7fefffe0,-0x7fef7fe0,0x108000];
var spfunction3 = [0x208,0x8020200,0,0x8020008,0x8000200,0,0x20208,0x8000200,0x20008,0x8000008,0x8000008,0x20000,0x8020208,0x20008,0x8020000,0x208,0x8000000,0x8,0x8020200,0x200,0x20200,0x8020000,0x8020008,0x20208,0x8000208,0x20200,0x20000,0x8000208,0x8,0x8020208,0x200,0x8000000,0x8020200,0x8000000,0x20008,0x208,0x20000,0x8020200,0x8000200,0,0x200,0x20008,0x8020208,0x8000200,0x8000008,0x200,0,0x8020008,0x8000208,0x20000,0x8000000,0x8020208,0x8,0x20208,0x20200,0x8000008,0x8020000,0x8000208,0x208,0x8020000,0x20208,0x8,0x8020008,0x20200];
var spfunction4 = [0x802001,0x2081,0x2081,0x80,0x802080,0x800081,0x800001,0x2001,0,0x802000,0x802000,0x802081,0x81,0,0x800080,0x800001,0x1,0x2000,0x800000,0x802001,0x80,0x800000,0x2001,0x2080,0x800081,0x1,0x2080,0x800080,0x2000,0x802080,0x802081,0x81,0x800080,0x800001,0x802000,0x802081,0x81,0,0,0x802000,0x2080,0x800080,0x800081,0x1,0x802001,0x2081,0x2081,0x80,0x802081,0x81,0x1,0x2000,0x800001,0x2001,0x802080,0x800081,0x2001,0x2080,0x800000,0x802001,0x80,0x800000,0x2000,0x802080];
var spfunction5 = [0x100,0x2080100,0x2080000,0x42000100,0x80000,0x100,0x40000000,0x2080000,0x40080100,0x80000,0x2000100,0x40080100,0x42000100,0x42080000,0x80100,0x40000000,0x2000000,0x40080000,0x40080000,0,0x40000100,0x42080100,0x42080100,0x2000100,0x42080000,0x40000100,0,0x42000000,0x2080100,0x2000000,0x42000000,0x80100,0x80000,0x42000100,0x100,0x2000000,0x40000000,0x2080000,0x42000100,0x40080100,0x2000100,0x40000000,0x42080000,0x2080100,0x40080100,0x100,0x2000000,0x42080000,0x42080100,0x80100,0x42000000,0x42080100,0x2080000,0,0x40080000,0x42000000,0x80100,0x2000100,0x40000100,0x80000,0,0x40080000,0x2080100,0x40000100];
var spfunction6 = [0x20000010,0x20400000,0x4000,0x20404010,0x20400000,0x10,0x20404010,0x400000,0x20004000,0x404010,0x400000,0x20000010,0x400010,0x20004000,0x20000000,0x4010,0,0x400010,0x20004010,0x4000,0x404000,0x20004010,0x10,0x20400010,0x20400010,0,0x404010,0x20404000,0x4010,0x404000,0x20404000,0x20000000,0x20004000,0x10,0x20400010,0x404000,0x20404010,0x400000,0x4010,0x20000010,0x400000,0x20004000,0x20000000,0x4010,0x20000010,0x20404010,0x404000,0x20400000,0x404010,0x20404000,0,0x20400010,0x10,0x4000,0x20400000,0x404010,0x4000,0x400010,0x20004010,0,0x20404000,0x20000000,0x400010,0x20004010];
var spfunction7 = [0x200000,0x4200002,0x4000802,0,0x800,0x4000802,0x200802,0x4200800,0x4200802,0x200000,0,0x4000002,0x2,0x4000000,0x4200002,0x802,0x4000800,0x200802,0x200002,0x4000800,0x4000002,0x4200000,0x4200800,0x200002,0x4200000,0x800,0x802,0x4200802,0x200800,0x2,0x4000000,0x200800,0x4000000,0x200800,0x200000,0x4000802,0x4000802,0x4200002,0x4200002,0x2,0x200002,0x4000000,0x4000800,0x200000,0x4200800,0x802,0x200802,0x4200800,0x802,0x4000002,0x4200802,0x4200000,0x200800,0,0x2,0x4200802,0,0x200802,0x4200000,0x800,0x4000002,0x4000800,0x800,0x200002];
var spfunction8 = [0x10001040,0x1000,0x40000,0x10041040,0x10000000,0x10001040,0x40,0x10000000,0x40040,0x10040000,0x10041040,0x41000,0x10041000,0x41040,0x1000,0x40,0x10040000,0x10000040,0x10001000,0x1040,0x41000,0x40040,0x10040040,0x10041000,0x1040,0,0,0x10040040,0x10000040,0x10001000,0x41040,0x40000,0x41040,0x40000,0x10041000,0x1000,0x40,0x10040040,0x1000,0x41040,0x10001000,0x40,0x10000040,0x10040000,0x10040040,0x10000000,0x40000,0x10001040,0,0x10041040,0x40040,0x10000040,0x10040000,0x10001000,0x10001040,0,0x10041040,0x41000,0x41000,0x1040,0x1040,0x40040,0x10000000,0x10041000];

/**
 * Create necessary sub keys.
 *
 * @param key the 64-bit or 192-bit key.
 *
 * @return the expanded keys.
 */
function _createKeys(key) {
  var pc2bytes0  = [0,0x4,0x20000000,0x20000004,0x10000,0x10004,0x20010000,0x20010004,0x200,0x204,0x20000200,0x20000204,0x10200,0x10204,0x20010200,0x20010204],
      pc2bytes1  = [0,0x1,0x100000,0x100001,0x4000000,0x4000001,0x4100000,0x4100001,0x100,0x101,0x100100,0x100101,0x4000100,0x4000101,0x4100100,0x4100101],
      pc2bytes2  = [0,0x8,0x800,0x808,0x1000000,0x1000008,0x1000800,0x1000808,0,0x8,0x800,0x808,0x1000000,0x1000008,0x1000800,0x1000808],
      pc2bytes3  = [0,0x200000,0x8000000,0x8200000,0x2000,0x202000,0x8002000,0x8202000,0x20000,0x220000,0x8020000,0x8220000,0x22000,0x222000,0x8022000,0x8222000],
      pc2bytes4  = [0,0x40000,0x10,0x40010,0,0x40000,0x10,0x40010,0x1000,0x41000,0x1010,0x41010,0x1000,0x41000,0x1010,0x41010],
      pc2bytes5  = [0,0x400,0x20,0x420,0,0x400,0x20,0x420,0x2000000,0x2000400,0x2000020,0x2000420,0x2000000,0x2000400,0x2000020,0x2000420],
      pc2bytes6  = [0,0x10000000,0x80000,0x10080000,0x2,0x10000002,0x80002,0x10080002,0,0x10000000,0x80000,0x10080000,0x2,0x10000002,0x80002,0x10080002],
      pc2bytes7  = [0,0x10000,0x800,0x10800,0x20000000,0x20010000,0x20000800,0x20010800,0x20000,0x30000,0x20800,0x30800,0x20020000,0x20030000,0x20020800,0x20030800],
      pc2bytes8  = [0,0x40000,0,0x40000,0x2,0x40002,0x2,0x40002,0x2000000,0x2040000,0x2000000,0x2040000,0x2000002,0x2040002,0x2000002,0x2040002],
      pc2bytes9  = [0,0x10000000,0x8,0x10000008,0,0x10000000,0x8,0x10000008,0x400,0x10000400,0x408,0x10000408,0x400,0x10000400,0x408,0x10000408],
      pc2bytes10 = [0,0x20,0,0x20,0x100000,0x100020,0x100000,0x100020,0x2000,0x2020,0x2000,0x2020,0x102000,0x102020,0x102000,0x102020],
      pc2bytes11 = [0,0x1000000,0x200,0x1000200,0x200000,0x1200000,0x200200,0x1200200,0x4000000,0x5000000,0x4000200,0x5000200,0x4200000,0x5200000,0x4200200,0x5200200],
      pc2bytes12 = [0,0x1000,0x8000000,0x8001000,0x80000,0x81000,0x8080000,0x8081000,0x10,0x1010,0x8000010,0x8001010,0x80010,0x81010,0x8080010,0x8081010],
      pc2bytes13 = [0,0x4,0x100,0x104,0,0x4,0x100,0x104,0x1,0x5,0x101,0x105,0x1,0x5,0x101,0x105];

  // how many iterations (1 for des, 3 for triple des)
  // changed by Paul 16/6/2007 to use Triple DES for 9+ byte keys
  var iterations = key.length() > 8 ? 3 : 1;

  // stores the return keys
  var keys = [];

  // now define the left shifts which need to be done
  var shifts = [0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0];

  var n = 0, tmp;
  for(var j = 0; j < iterations; j++) {
    var left = key.getInt32();
    var right = key.getInt32();

    tmp = ((left >>> 4) ^ right) & 0x0f0f0f0f;
    right ^= tmp;
    left ^= (tmp << 4);

    tmp = ((right >>> -16) ^ left) & 0x0000ffff;
    left ^= tmp;
    right ^= (tmp << -16);

    tmp = ((left >>> 2) ^ right) & 0x33333333;
    right ^= tmp;
    left ^= (tmp << 2);

    tmp = ((right >>> -16) ^ left) & 0x0000ffff;
    left ^= tmp;
    right ^= (tmp << -16);

    tmp = ((left >>> 1) ^ right) & 0x55555555;
    right ^= tmp;
    left ^= (tmp << 1);

    tmp = ((right >>> 8) ^ left) & 0x00ff00ff;
    left ^= tmp;
    right ^= (tmp << 8);

    tmp = ((left >>> 1) ^ right) & 0x55555555;
    right ^= tmp;
    left ^= (tmp << 1);

    // right needs to be shifted and OR'd with last four bits of left
    tmp = (left << 8) | ((right >>> 20) & 0x000000f0);

    // left needs to be put upside down
    left = ((right << 24) | ((right << 8) & 0xff0000) |
      ((right >>> 8) & 0xff00) | ((right >>> 24) & 0xf0));
    right = tmp;

    // now go through and perform these shifts on the left and right keys
    for(var i = 0; i < shifts.length; ++i) {
      //shift the keys either one or two bits to the left
      if(shifts[i]) {
        left = (left << 2) | (left >>> 26);
        right = (right << 2) | (right >>> 26);
      } else {
        left = (left << 1) | (left >>> 27);
        right = (right << 1) | (right >>> 27);
      }
      left &= -0xf;
      right &= -0xf;

      // now apply PC-2, in such a way that E is easier when encrypting or
      // decrypting this conversion will look like PC-2 except only the last 6
      // bits of each byte are used rather than 48 consecutive bits and the
      // order of lines will be according to how the S selection functions will
      // be applied: S2, S4, S6, S8, S1, S3, S5, S7
      var lefttmp = (
        pc2bytes0[left >>> 28] | pc2bytes1[(left >>> 24) & 0xf] |
        pc2bytes2[(left >>> 20) & 0xf] | pc2bytes3[(left >>> 16) & 0xf] |
        pc2bytes4[(left >>> 12) & 0xf] | pc2bytes5[(left >>> 8) & 0xf] |
        pc2bytes6[(left >>> 4) & 0xf]);
      var righttmp = (
        pc2bytes7[right >>> 28] | pc2bytes8[(right >>> 24) & 0xf] |
        pc2bytes9[(right >>> 20) & 0xf] | pc2bytes10[(right >>> 16) & 0xf] |
        pc2bytes11[(right >>> 12) & 0xf] | pc2bytes12[(right >>> 8) & 0xf] |
        pc2bytes13[(right >>> 4) & 0xf]);
      tmp = ((righttmp >>> 16) ^ lefttmp) & 0x0000ffff;
      keys[n++] = lefttmp ^ tmp;
      keys[n++] = righttmp ^ (tmp << 16);
    }
  }

  return keys;
}

/**
 * Updates a single block (1 byte) using DES. The update will either
 * encrypt or decrypt the block.
 *
 * @param keys the expanded keys.
 * @param input the input block (an array of 32-bit words).
 * @param output the updated output block.
 * @param decrypt true to decrypt the block, false to encrypt it.
 */
function _updateBlock(keys, input, output, decrypt) {
  // set up loops for single or triple DES
  var iterations = keys.length === 32 ? 3 : 9;
  var looping;
  if(iterations === 3) {
    looping = decrypt ? [30, -2, -2] : [0, 32, 2];
  } else {
    looping = (decrypt ?
      [94, 62, -2, 32, 64, 2, 30, -2, -2] :
      [0, 32, 2, 62, 30, -2, 64, 96, 2]);
  }

  var tmp;

  var left = input[0];
  var right = input[1];

  // first each 64 bit chunk of the message must be permuted according to IP
  tmp = ((left >>> 4) ^ right) & 0x0f0f0f0f;
  right ^= tmp;
  left ^= (tmp << 4);

  tmp = ((left >>> 16) ^ right) & 0x0000ffff;
  right ^= tmp;
  left ^= (tmp << 16);

  tmp = ((right >>> 2) ^ left) & 0x33333333;
  left ^= tmp;
  right ^= (tmp << 2);

  tmp = ((right >>> 8) ^ left) & 0x00ff00ff;
  left ^= tmp;
  right ^= (tmp << 8);

  tmp = ((left >>> 1) ^ right) & 0x55555555;
  right ^= tmp;
  left ^= (tmp << 1);

  // rotate left 1 bit
  left = ((left << 1) | (left >>> 31));
  right = ((right << 1) | (right >>> 31));

  for(var j = 0; j < iterations; j += 3) {
    var endloop = looping[j + 1];
    var loopinc = looping[j + 2];

    // now go through and perform the encryption or decryption
    for(var i = looping[j]; i != endloop; i += loopinc) {
      var right1 = right ^ keys[i];
      var right2 = ((right >>> 4) | (right << 28)) ^ keys[i + 1];

      // passing these bytes through the S selection functions
      tmp = left;
      left = right;
      right = tmp ^ (
        spfunction2[(right1 >>> 24) & 0x3f] |
        spfunction4[(right1 >>> 16) & 0x3f] |
        spfunction6[(right1 >>>  8) & 0x3f] |
        spfunction8[right1 & 0x3f] |
        spfunction1[(right2 >>> 24) & 0x3f] |
        spfunction3[(right2 >>> 16) & 0x3f] |
        spfunction5[(right2 >>>  8) & 0x3f] |
        spfunction7[right2 & 0x3f]);
    }
    // unreverse left and right
    tmp = left;
    left = right;
    right = tmp;
  }

  // rotate right 1 bit
  left = ((left >>> 1) | (left << 31));
  right = ((right >>> 1) | (right << 31));

  // now perform IP-1, which is IP in the opposite direction
  tmp = ((left >>> 1) ^ right) & 0x55555555;
  right ^= tmp;
  left ^= (tmp << 1);

  tmp = ((right >>> 8) ^ left) & 0x00ff00ff;
  left ^= tmp;
  right ^= (tmp << 8);

  tmp = ((right >>> 2) ^ left) & 0x33333333;
  left ^= tmp;
  right ^= (tmp << 2);

  tmp = ((left >>> 16) ^ right) & 0x0000ffff;
  right ^= tmp;
  left ^= (tmp << 16);

  tmp = ((left >>> 4) ^ right) & 0x0f0f0f0f;
  right ^= tmp;
  left ^= (tmp << 4);

  output[0] = left;
  output[1] = right;
}

/**
 * Deprecated. Instead, use:
 *
 * forge.cipher.createCipher('DES-<mode>', key);
 * forge.cipher.createDecipher('DES-<mode>', key);
 *
 * Creates a deprecated DES cipher object. This object's mode will default to
 * CBC (cipher-block-chaining).
 *
 * The key may be given as a binary-encoded string of bytes or a byte buffer.
 *
 * @param options the options to use.
 *          key the symmetric key to use (64 or 192 bits).
 *          output the buffer to write to.
 *          decrypt true for decryption, false for encryption.
 *          mode the cipher mode to use (default: 'CBC').
 *
 * @return the cipher.
 */
function _createCipher(options) {
  options = options || {};
  var mode = (options.mode || 'CBC').toUpperCase();
  var algorithm = 'DES-' + mode;

  var cipher;
  if(options.decrypt) {
    cipher = forge$l.cipher.createDecipher(algorithm, options.key);
  } else {
    cipher = forge$l.cipher.createCipher(algorithm, options.key);
  }

  // backwards compatible start API
  var start = cipher.start;
  cipher.start = function(iv, options) {
    // backwards compatibility: support second arg as output buffer
    var output = null;
    if(options instanceof forge$l.util.ByteBuffer) {
      output = options;
      options = {};
    }
    options = options || {};
    options.output = output;
    options.iv = iv;
    start.call(cipher, options);
  };

  return cipher;
}

/**
 * Node.js module for Forge message digests.
 *
 * @author Dave Longley
 *
 * Copyright 2011-2017 Digital Bazaar, Inc.
 */

var forge$k = forge$s;

forge$k.md = forge$k.md || {};
forge$k.md.algorithms = forge$k.md.algorithms || {};

/**
 * Hash-based Message Authentication Code implementation. Requires a message
 * digest object that can be obtained, for example, from forge.md.sha1 or
 * forge.md.md5.
 *
 * @author Dave Longley
 *
 * Copyright (c) 2010-2012 Digital Bazaar, Inc. All rights reserved.
 */

var forge$j = forge$s;



/* HMAC API */
var hmac = forge$j.hmac = forge$j.hmac || {};

/**
 * Creates an HMAC object that uses the given message digest object.
 *
 * @return an HMAC object.
 */
hmac.create = function() {
  // the hmac key to use
  var _key = null;

  // the message digest to use
  var _md = null;

  // the inner padding
  var _ipadding = null;

  // the outer padding
  var _opadding = null;

  // hmac context
  var ctx = {};

  /**
   * Starts or restarts the HMAC with the given key and message digest.
   *
   * @param md the message digest to use, null to reuse the previous one,
   *           a string to use builtin 'sha1', 'md5', 'sha256'.
   * @param key the key to use as a string, array of bytes, byte buffer,
   *           or null to reuse the previous key.
   */
  ctx.start = function(md, key) {
    if(md !== null) {
      if(typeof md === 'string') {
        // create builtin message digest
        md = md.toLowerCase();
        if(md in forge$j.md.algorithms) {
          _md = forge$j.md.algorithms[md].create();
        } else {
          throw new Error('Unknown hash algorithm "' + md + '"');
        }
      } else {
        // store message digest
        _md = md;
      }
    }

    if(key === null) {
      // reuse previous key
      key = _key;
    } else {
      if(typeof key === 'string') {
        // convert string into byte buffer
        key = forge$j.util.createBuffer(key);
      } else if(forge$j.util.isArray(key)) {
        // convert byte array into byte buffer
        var tmp = key;
        key = forge$j.util.createBuffer();
        for(var i = 0; i < tmp.length; ++i) {
          key.putByte(tmp[i]);
        }
      }

      // if key is longer than blocksize, hash it
      var keylen = key.length();
      if(keylen > _md.blockLength) {
        _md.start();
        _md.update(key.bytes());
        key = _md.digest();
      }

      // mix key into inner and outer padding
      // ipadding = [0x36 * blocksize] ^ key
      // opadding = [0x5C * blocksize] ^ key
      _ipadding = forge$j.util.createBuffer();
      _opadding = forge$j.util.createBuffer();
      keylen = key.length();
      for(var i = 0; i < keylen; ++i) {
        var tmp = key.at(i);
        _ipadding.putByte(0x36 ^ tmp);
        _opadding.putByte(0x5C ^ tmp);
      }

      // if key is shorter than blocksize, add additional padding
      if(keylen < _md.blockLength) {
        var tmp = _md.blockLength - keylen;
        for(var i = 0; i < tmp; ++i) {
          _ipadding.putByte(0x36);
          _opadding.putByte(0x5C);
        }
      }
      _key = key;
      _ipadding = _ipadding.bytes();
      _opadding = _opadding.bytes();
    }

    // digest is done like so: hash(opadding | hash(ipadding | message))

    // prepare to do inner hash
    // hash(ipadding | message)
    _md.start();
    _md.update(_ipadding);
  };

  /**
   * Updates the HMAC with the given message bytes.
   *
   * @param bytes the bytes to update with.
   */
  ctx.update = function(bytes) {
    _md.update(bytes);
  };

  /**
   * Produces the Message Authentication Code (MAC).
   *
   * @return a byte buffer containing the digest value.
   */
  ctx.getMac = function() {
    // digest is done like so: hash(opadding | hash(ipadding | message))
    // here we do the outer hashing
    var inner = _md.digest().bytes();
    _md.start();
    _md.update(_opadding);
    _md.update(inner);
    return _md.digest();
  };
  // alias for getMac
  ctx.digest = ctx.getMac;

  return ctx;
};

/**
 * Password-Based Key-Derivation Function #2 implementation.
 *
 * See RFC 2898 for details.
 *
 * @author Dave Longley
 *
 * Copyright (c) 2010-2013 Digital Bazaar, Inc.
 */

var forge$i = forge$s;




var pkcs5 = forge$i.pkcs5 = forge$i.pkcs5 || {};

var crypto;
if(forge$i.util.isNodejs && !forge$i.options.usePureJavaScript) {
  crypto = require$$1__default;
}

/**
 * Derives a key from a password.
 *
 * @param p the password as a binary-encoded string of bytes.
 * @param s the salt as a binary-encoded string of bytes.
 * @param c the iteration count, a positive integer.
 * @param dkLen the intended length, in bytes, of the derived key,
 *          (max: 2^32 - 1) * hash length of the PRF.
 * @param [md] the message digest (or algorithm identifier as a string) to use
 *          in the PRF, defaults to SHA-1.
 * @param [callback(err, key)] presence triggers asynchronous version, called
 *          once the operation completes.
 *
 * @return the derived key, as a binary-encoded string of bytes, for the
 *           synchronous version (if no callback is specified).
 */
forge$i.pbkdf2 = pkcs5.pbkdf2 = function(
  p, s, c, dkLen, md, callback) {
  if(typeof md === 'function') {
    callback = md;
    md = null;
  }

  // use native implementation if possible and not disabled, note that
  // some node versions only support SHA-1, others allow digest to be changed
  if(forge$i.util.isNodejs && !forge$i.options.usePureJavaScript &&
    crypto.pbkdf2 && (md === null || typeof md !== 'object') &&
    (crypto.pbkdf2Sync.length > 4 || (!md || md === 'sha1'))) {
    if(typeof md !== 'string') {
      // default prf to SHA-1
      md = 'sha1';
    }
    p = Buffer.from(p, 'binary');
    s = Buffer.from(s, 'binary');
    if(!callback) {
      if(crypto.pbkdf2Sync.length === 4) {
        return crypto.pbkdf2Sync(p, s, c, dkLen).toString('binary');
      }
      return crypto.pbkdf2Sync(p, s, c, dkLen, md).toString('binary');
    }
    if(crypto.pbkdf2Sync.length === 4) {
      return crypto.pbkdf2(p, s, c, dkLen, function(err, key) {
        if(err) {
          return callback(err);
        }
        callback(null, key.toString('binary'));
      });
    }
    return crypto.pbkdf2(p, s, c, dkLen, md, function(err, key) {
      if(err) {
        return callback(err);
      }
      callback(null, key.toString('binary'));
    });
  }

  if(typeof md === 'undefined' || md === null) {
    // default prf to SHA-1
    md = 'sha1';
  }
  if(typeof md === 'string') {
    if(!(md in forge$i.md.algorithms)) {
      throw new Error('Unknown hash algorithm: ' + md);
    }
    md = forge$i.md[md].create();
  }

  var hLen = md.digestLength;

  /* 1. If dkLen > (2^32 - 1) * hLen, output "derived key too long" and
    stop. */
  if(dkLen > (0xFFFFFFFF * hLen)) {
    var err = new Error('Derived key is too long.');
    if(callback) {
      return callback(err);
    }
    throw err;
  }

  /* 2. Let len be the number of hLen-octet blocks in the derived key,
    rounding up, and let r be the number of octets in the last
    block:

    len = CEIL(dkLen / hLen),
    r = dkLen - (len - 1) * hLen. */
  var len = Math.ceil(dkLen / hLen);
  var r = dkLen - (len - 1) * hLen;

  /* 3. For each block of the derived key apply the function F defined
    below to the password P, the salt S, the iteration count c, and
    the block index to compute the block:

    T_1 = F(P, S, c, 1),
    T_2 = F(P, S, c, 2),
    ...
    T_len = F(P, S, c, len),

    where the function F is defined as the exclusive-or sum of the
    first c iterates of the underlying pseudorandom function PRF
    applied to the password P and the concatenation of the salt S
    and the block index i:

    F(P, S, c, i) = u_1 XOR u_2 XOR ... XOR u_c

    where

    u_1 = PRF(P, S || INT(i)),
    u_2 = PRF(P, u_1),
    ...
    u_c = PRF(P, u_{c-1}).

    Here, INT(i) is a four-octet encoding of the integer i, most
    significant octet first. */
  var prf = forge$i.hmac.create();
  prf.start(md, p);
  var dk = '';
  var xor, u_c, u_c1;

  // sync version
  if(!callback) {
    for(var i = 1; i <= len; ++i) {
      // PRF(P, S || INT(i)) (first iteration)
      prf.start(null, null);
      prf.update(s);
      prf.update(forge$i.util.int32ToBytes(i));
      xor = u_c1 = prf.digest().getBytes();

      // PRF(P, u_{c-1}) (other iterations)
      for(var j = 2; j <= c; ++j) {
        prf.start(null, null);
        prf.update(u_c1);
        u_c = prf.digest().getBytes();
        // F(p, s, c, i)
        xor = forge$i.util.xorBytes(xor, u_c, hLen);
        u_c1 = u_c;
      }

      /* 4. Concatenate the blocks and extract the first dkLen octets to
        produce a derived key DK:

        DK = T_1 || T_2 ||  ...  || T_len<0..r-1> */
      dk += (i < len) ? xor : xor.substr(0, r);
    }
    /* 5. Output the derived key DK. */
    return dk;
  }

  // async version
  var i = 1, j;
  function outer() {
    if(i > len) {
      // done
      return callback(null, dk);
    }

    // PRF(P, S || INT(i)) (first iteration)
    prf.start(null, null);
    prf.update(s);
    prf.update(forge$i.util.int32ToBytes(i));
    xor = u_c1 = prf.digest().getBytes();

    // PRF(P, u_{c-1}) (other iterations)
    j = 2;
    inner();
  }

  function inner() {
    if(j <= c) {
      prf.start(null, null);
      prf.update(u_c1);
      u_c = prf.digest().getBytes();
      // F(p, s, c, i)
      xor = forge$i.util.xorBytes(xor, u_c, hLen);
      u_c1 = u_c;
      ++j;
      return forge$i.util.setImmediate(inner);
    }

    /* 4. Concatenate the blocks and extract the first dkLen octets to
      produce a derived key DK:

      DK = T_1 || T_2 ||  ...  || T_len<0..r-1> */
    dk += (i < len) ? xor : xor.substr(0, r);

    ++i;
    outer();
  }

  outer();
};

/**
 * Javascript implementation of basic PEM (Privacy Enhanced Mail) algorithms.
 *
 * See: RFC 1421.
 *
 * @author Dave Longley
 *
 * Copyright (c) 2013-2014 Digital Bazaar, Inc.
 *
 * A Forge PEM object has the following fields:
 *
 * type: identifies the type of message (eg: "RSA PRIVATE KEY").
 *
 * procType: identifies the type of processing performed on the message,
 *   it has two subfields: version and type, eg: 4,ENCRYPTED.
 *
 * contentDomain: identifies the type of content in the message, typically
 *   only uses the value: "RFC822".
 *
 * dekInfo: identifies the message encryption algorithm and mode and includes
 *   any parameters for the algorithm, it has two subfields: algorithm and
 *   parameters, eg: DES-CBC,F8143EDE5960C597.
 *
 * headers: contains all other PEM encapsulated headers -- where order is
 *   significant (for pairing data like recipient ID + key info).
 *
 * body: the binary-encoded body.
 */

var forge$h = forge$s;


// shortcut for pem API
var pem = forge$h.pem = forge$h.pem || {};

/**
 * Encodes (serializes) the given PEM object.
 *
 * @param msg the PEM message object to encode.
 * @param options the options to use:
 *          maxline the maximum characters per line for the body, (default: 64).
 *
 * @return the PEM-formatted string.
 */
pem.encode = function(msg, options) {
  options = options || {};
  var rval = '-----BEGIN ' + msg.type + '-----\r\n';

  // encode special headers
  var header;
  if(msg.procType) {
    header = {
      name: 'Proc-Type',
      values: [String(msg.procType.version), msg.procType.type]
    };
    rval += foldHeader(header);
  }
  if(msg.contentDomain) {
    header = {name: 'Content-Domain', values: [msg.contentDomain]};
    rval += foldHeader(header);
  }
  if(msg.dekInfo) {
    header = {name: 'DEK-Info', values: [msg.dekInfo.algorithm]};
    if(msg.dekInfo.parameters) {
      header.values.push(msg.dekInfo.parameters);
    }
    rval += foldHeader(header);
  }

  if(msg.headers) {
    // encode all other headers
    for(var i = 0; i < msg.headers.length; ++i) {
      rval += foldHeader(msg.headers[i]);
    }
  }

  // terminate header
  if(msg.procType) {
    rval += '\r\n';
  }

  // add body
  rval += forge$h.util.encode64(msg.body, options.maxline || 64) + '\r\n';

  rval += '-----END ' + msg.type + '-----\r\n';
  return rval;
};

/**
 * Decodes (deserializes) all PEM messages found in the given string.
 *
 * @param str the PEM-formatted string to decode.
 *
 * @return the PEM message objects in an array.
 */
pem.decode = function(str) {
  var rval = [];

  // split string into PEM messages (be lenient w/EOF on BEGIN line)
  var rMessage = /\s*-----BEGIN ([A-Z0-9- ]+)-----\r?\n?([\x21-\x7e\s]+?(?:\r?\n\r?\n))?([:A-Za-z0-9+\/=\s]+?)-----END \1-----/g;
  var rHeader = /([\x21-\x7e]+):\s*([\x21-\x7e\s^:]+)/;
  var rCRLF = /\r?\n/;
  var match;
  while(true) {
    match = rMessage.exec(str);
    if(!match) {
      break;
    }

    // accept "NEW CERTIFICATE REQUEST" as "CERTIFICATE REQUEST"
    // https://datatracker.ietf.org/doc/html/rfc7468#section-7
    var type = match[1];
    if(type === 'NEW CERTIFICATE REQUEST') {
      type = 'CERTIFICATE REQUEST';
    }

    var msg = {
      type: type,
      procType: null,
      contentDomain: null,
      dekInfo: null,
      headers: [],
      body: forge$h.util.decode64(match[3])
    };
    rval.push(msg);

    // no headers
    if(!match[2]) {
      continue;
    }

    // parse headers
    var lines = match[2].split(rCRLF);
    var li = 0;
    while(match && li < lines.length) {
      // get line, trim any rhs whitespace
      var line = lines[li].replace(/\s+$/, '');

      // RFC2822 unfold any following folded lines
      for(var nl = li + 1; nl < lines.length; ++nl) {
        var next = lines[nl];
        if(!/\s/.test(next[0])) {
          break;
        }
        line += next;
        li = nl;
      }

      // parse header
      match = line.match(rHeader);
      if(match) {
        var header = {name: match[1], values: []};
        var values = match[2].split(',');
        for(var vi = 0; vi < values.length; ++vi) {
          header.values.push(ltrim(values[vi]));
        }

        // Proc-Type must be the first header
        if(!msg.procType) {
          if(header.name !== 'Proc-Type') {
            throw new Error('Invalid PEM formatted message. The first ' +
              'encapsulated header must be "Proc-Type".');
          } else if(header.values.length !== 2) {
            throw new Error('Invalid PEM formatted message. The "Proc-Type" ' +
              'header must have two subfields.');
          }
          msg.procType = {version: values[0], type: values[1]};
        } else if(!msg.contentDomain && header.name === 'Content-Domain') {
          // special-case Content-Domain
          msg.contentDomain = values[0] || '';
        } else if(!msg.dekInfo && header.name === 'DEK-Info') {
          // special-case DEK-Info
          if(header.values.length === 0) {
            throw new Error('Invalid PEM formatted message. The "DEK-Info" ' +
              'header must have at least one subfield.');
          }
          msg.dekInfo = {algorithm: values[0], parameters: values[1] || null};
        } else {
          msg.headers.push(header);
        }
      }

      ++li;
    }

    if(msg.procType === 'ENCRYPTED' && !msg.dekInfo) {
      throw new Error('Invalid PEM formatted message. The "DEK-Info" ' +
        'header must be present if "Proc-Type" is "ENCRYPTED".');
    }
  }

  if(rval.length === 0) {
    throw new Error('Invalid PEM formatted message.');
  }

  return rval;
};

function foldHeader(header) {
  var rval = header.name + ': ';

  // ensure values with CRLF are folded
  var values = [];
  var insertSpace = function(match, $1) {
    return ' ' + $1;
  };
  for(var i = 0; i < header.values.length; ++i) {
    values.push(header.values[i].replace(/^(\S+\r\n)/, insertSpace));
  }
  rval += values.join(',') + '\r\n';

  // do folding
  var length = 0;
  var candidate = -1;
  for(var i = 0; i < rval.length; ++i, ++length) {
    if(length > 65 && candidate !== -1) {
      var insert = rval[candidate];
      if(insert === ',') {
        ++candidate;
        rval = rval.substr(0, candidate) + '\r\n ' + rval.substr(candidate);
      } else {
        rval = rval.substr(0, candidate) +
          '\r\n' + insert + rval.substr(candidate + 1);
      }
      length = (i - candidate - 1);
      candidate = -1;
      ++i;
    } else if(rval[i] === ' ' || rval[i] === '\t' || rval[i] === ',') {
      candidate = i;
    }
  }

  return rval;
}

function ltrim(str) {
  return str.replace(/^\s+/, '');
}

/**
 * Secure Hash Algorithm with 256-bit digest (SHA-256) implementation.
 *
 * See FIPS 180-2 for details.
 *
 * @author Dave Longley
 *
 * Copyright (c) 2010-2015 Digital Bazaar, Inc.
 */

var forge$g = forge$s;



var sha256 = forge$g.sha256 = forge$g.sha256 || {};
forge$g.md.sha256 = forge$g.md.algorithms.sha256 = sha256;

/**
 * Creates a SHA-256 message digest object.
 *
 * @return a message digest object.
 */
sha256.create = function() {
  // do initialization as necessary
  if(!_initialized$1) {
    _init$1();
  }

  // SHA-256 state contains eight 32-bit integers
  var _state = null;

  // input buffer
  var _input = forge$g.util.createBuffer();

  // used for word storage
  var _w = new Array(64);

  // message digest object
  var md = {
    algorithm: 'sha256',
    blockLength: 64,
    digestLength: 32,
    // 56-bit length of message so far (does not including padding)
    messageLength: 0,
    // true message length
    fullMessageLength: null,
    // size of message length in bytes
    messageLengthSize: 8
  };

  /**
   * Starts the digest.
   *
   * @return this digest object.
   */
  md.start = function() {
    // up to 56-bit message length for convenience
    md.messageLength = 0;

    // full message length (set md.messageLength64 for backwards-compatibility)
    md.fullMessageLength = md.messageLength64 = [];
    var int32s = md.messageLengthSize / 4;
    for(var i = 0; i < int32s; ++i) {
      md.fullMessageLength.push(0);
    }
    _input = forge$g.util.createBuffer();
    _state = {
      h0: 0x6A09E667,
      h1: 0xBB67AE85,
      h2: 0x3C6EF372,
      h3: 0xA54FF53A,
      h4: 0x510E527F,
      h5: 0x9B05688C,
      h6: 0x1F83D9AB,
      h7: 0x5BE0CD19
    };
    return md;
  };
  // start digest automatically for first time
  md.start();

  /**
   * Updates the digest with the given message input. The given input can
   * treated as raw input (no encoding will be applied) or an encoding of
   * 'utf8' maybe given to encode the input using UTF-8.
   *
   * @param msg the message input to update with.
   * @param encoding the encoding to use (default: 'raw', other: 'utf8').
   *
   * @return this digest object.
   */
  md.update = function(msg, encoding) {
    if(encoding === 'utf8') {
      msg = forge$g.util.encodeUtf8(msg);
    }

    // update message length
    var len = msg.length;
    md.messageLength += len;
    len = [(len / 0x100000000) >>> 0, len >>> 0];
    for(var i = md.fullMessageLength.length - 1; i >= 0; --i) {
      md.fullMessageLength[i] += len[1];
      len[1] = len[0] + ((md.fullMessageLength[i] / 0x100000000) >>> 0);
      md.fullMessageLength[i] = md.fullMessageLength[i] >>> 0;
      len[0] = ((len[1] / 0x100000000) >>> 0);
    }

    // add bytes to input buffer
    _input.putBytes(msg);

    // process bytes
    _update$1(_state, _w, _input);

    // compact input buffer every 2K or if empty
    if(_input.read > 2048 || _input.length() === 0) {
      _input.compact();
    }

    return md;
  };

  /**
   * Produces the digest.
   *
   * @return a byte buffer containing the digest value.
   */
  md.digest = function() {
    /* Note: Here we copy the remaining bytes in the input buffer and
    add the appropriate SHA-256 padding. Then we do the final update
    on a copy of the state so that if the user wants to get
    intermediate digests they can do so. */

    /* Determine the number of bytes that must be added to the message
    to ensure its length is congruent to 448 mod 512. In other words,
    the data to be digested must be a multiple of 512 bits (or 128 bytes).
    This data includes the message, some padding, and the length of the
    message. Since the length of the message will be encoded as 8 bytes (64
    bits), that means that the last segment of the data must have 56 bytes
    (448 bits) of message and padding. Therefore, the length of the message
    plus the padding must be congruent to 448 mod 512 because
    512 - 128 = 448.

    In order to fill up the message length it must be filled with
    padding that begins with 1 bit followed by all 0 bits. Padding
    must *always* be present, so if the message length is already
    congruent to 448 mod 512, then 512 padding bits must be added. */

    var finalBlock = forge$g.util.createBuffer();
    finalBlock.putBytes(_input.bytes());

    // compute remaining size to be digested (include message length size)
    var remaining = (
      md.fullMessageLength[md.fullMessageLength.length - 1] +
      md.messageLengthSize);

    // add padding for overflow blockSize - overflow
    // _padding starts with 1 byte with first bit is set (byte value 128), then
    // there may be up to (blockSize - 1) other pad bytes
    var overflow = remaining & (md.blockLength - 1);
    finalBlock.putBytes(_padding$1.substr(0, md.blockLength - overflow));

    // serialize message length in bits in big-endian order; since length
    // is stored in bytes we multiply by 8 and add carry from next int
    var next, carry;
    var bits = md.fullMessageLength[0] * 8;
    for(var i = 0; i < md.fullMessageLength.length - 1; ++i) {
      next = md.fullMessageLength[i + 1] * 8;
      carry = (next / 0x100000000) >>> 0;
      bits += carry;
      finalBlock.putInt32(bits >>> 0);
      bits = next >>> 0;
    }
    finalBlock.putInt32(bits);

    var s2 = {
      h0: _state.h0,
      h1: _state.h1,
      h2: _state.h2,
      h3: _state.h3,
      h4: _state.h4,
      h5: _state.h5,
      h6: _state.h6,
      h7: _state.h7
    };
    _update$1(s2, _w, finalBlock);
    var rval = forge$g.util.createBuffer();
    rval.putInt32(s2.h0);
    rval.putInt32(s2.h1);
    rval.putInt32(s2.h2);
    rval.putInt32(s2.h3);
    rval.putInt32(s2.h4);
    rval.putInt32(s2.h5);
    rval.putInt32(s2.h6);
    rval.putInt32(s2.h7);
    return rval;
  };

  return md;
};

// sha-256 padding bytes not initialized yet
var _padding$1 = null;
var _initialized$1 = false;

// table of constants
var _k = null;

/**
 * Initializes the constant tables.
 */
function _init$1() {
  // create padding
  _padding$1 = String.fromCharCode(128);
  _padding$1 += forge$g.util.fillString(String.fromCharCode(0x00), 64);

  // create K table for SHA-256
  _k = [
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2];

  // now initialized
  _initialized$1 = true;
}

/**
 * Updates a SHA-256 state with the given byte buffer.
 *
 * @param s the SHA-256 state to update.
 * @param w the array to use to store words.
 * @param bytes the byte buffer to update with.
 */
function _update$1(s, w, bytes) {
  // consume 512 bit (64 byte) chunks
  var t1, t2, s0, s1, ch, maj, i, a, b, c, d, e, f, g, h;
  var len = bytes.length();
  while(len >= 64) {
    // the w array will be populated with sixteen 32-bit big-endian words
    // and then extended into 64 32-bit words according to SHA-256
    for(i = 0; i < 16; ++i) {
      w[i] = bytes.getInt32();
    }
    for(; i < 64; ++i) {
      // XOR word 2 words ago rot right 17, rot right 19, shft right 10
      t1 = w[i - 2];
      t1 =
        ((t1 >>> 17) | (t1 << 15)) ^
        ((t1 >>> 19) | (t1 << 13)) ^
        (t1 >>> 10);
      // XOR word 15 words ago rot right 7, rot right 18, shft right 3
      t2 = w[i - 15];
      t2 =
        ((t2 >>> 7) | (t2 << 25)) ^
        ((t2 >>> 18) | (t2 << 14)) ^
        (t2 >>> 3);
      // sum(t1, word 7 ago, t2, word 16 ago) modulo 2^32
      w[i] = (t1 + w[i - 7] + t2 + w[i - 16]) | 0;
    }

    // initialize hash value for this chunk
    a = s.h0;
    b = s.h1;
    c = s.h2;
    d = s.h3;
    e = s.h4;
    f = s.h5;
    g = s.h6;
    h = s.h7;

    // round function
    for(i = 0; i < 64; ++i) {
      // Sum1(e)
      s1 =
        ((e >>> 6) | (e << 26)) ^
        ((e >>> 11) | (e << 21)) ^
        ((e >>> 25) | (e << 7));
      // Ch(e, f, g) (optimized the same way as SHA-1)
      ch = g ^ (e & (f ^ g));
      // Sum0(a)
      s0 =
        ((a >>> 2) | (a << 30)) ^
        ((a >>> 13) | (a << 19)) ^
        ((a >>> 22) | (a << 10));
      // Maj(a, b, c) (optimized the same way as SHA-1)
      maj = (a & b) | (c & (a ^ b));

      // main algorithm
      t1 = h + s1 + ch + _k[i] + w[i];
      t2 = s0 + maj;
      h = g;
      g = f;
      f = e;
      // `>>> 0` necessary to avoid iOS/Safari 10 optimization bug
      // can't truncate with `| 0`
      e = (d + t1) >>> 0;
      d = c;
      c = b;
      b = a;
      // `>>> 0` necessary to avoid iOS/Safari 10 optimization bug
      // can't truncate with `| 0`
      a = (t1 + t2) >>> 0;
    }

    // update hash state
    s.h0 = (s.h0 + a) | 0;
    s.h1 = (s.h1 + b) | 0;
    s.h2 = (s.h2 + c) | 0;
    s.h3 = (s.h3 + d) | 0;
    s.h4 = (s.h4 + e) | 0;
    s.h5 = (s.h5 + f) | 0;
    s.h6 = (s.h6 + g) | 0;
    s.h7 = (s.h7 + h) | 0;
    len -= 64;
  }
}

/**
 * A javascript implementation of a cryptographically-secure
 * Pseudo Random Number Generator (PRNG). The Fortuna algorithm is followed
 * here though the use of SHA-256 is not enforced; when generating an
 * a PRNG context, the hashing algorithm and block cipher used for
 * the generator are specified via a plugin.
 *
 * @author Dave Longley
 *
 * Copyright (c) 2010-2014 Digital Bazaar, Inc.
 */

var forge$f = forge$s;


var _crypto$1 = null;
if(forge$f.util.isNodejs && !forge$f.options.usePureJavaScript &&
  !process.versions['node-webkit']) {
  _crypto$1 = require$$1__default;
}

/* PRNG API */
var prng = forge$f.prng = forge$f.prng || {};

/**
 * Creates a new PRNG context.
 *
 * A PRNG plugin must be passed in that will provide:
 *
 * 1. A function that initializes the key and seed of a PRNG context. It
 *   will be given a 16 byte key and a 16 byte seed. Any key expansion
 *   or transformation of the seed from a byte string into an array of
 *   integers (or similar) should be performed.
 * 2. The cryptographic function used by the generator. It takes a key and
 *   a seed.
 * 3. A seed increment function. It takes the seed and returns seed + 1.
 * 4. An api to create a message digest.
 *
 * For an example, see random.js.
 *
 * @param plugin the PRNG plugin to use.
 */
prng.create = function(plugin) {
  var ctx = {
    plugin: plugin,
    key: null,
    seed: null,
    time: null,
    // number of reseeds so far
    reseeds: 0,
    // amount of data generated so far
    generated: 0,
    // no initial key bytes
    keyBytes: ''
  };

  // create 32 entropy pools (each is a message digest)
  var md = plugin.md;
  var pools = new Array(32);
  for(var i = 0; i < 32; ++i) {
    pools[i] = md.create();
  }
  ctx.pools = pools;

  // entropy pools are written to cyclically, starting at index 0
  ctx.pool = 0;

  /**
   * Generates random bytes. The bytes may be generated synchronously or
   * asynchronously. Web workers must use the asynchronous interface or
   * else the behavior is undefined.
   *
   * @param count the number of random bytes to generate.
   * @param [callback(err, bytes)] called once the operation completes.
   *
   * @return count random bytes as a string.
   */
  ctx.generate = function(count, callback) {
    // do synchronously
    if(!callback) {
      return ctx.generateSync(count);
    }

    // simple generator using counter-based CBC
    var cipher = ctx.plugin.cipher;
    var increment = ctx.plugin.increment;
    var formatKey = ctx.plugin.formatKey;
    var formatSeed = ctx.plugin.formatSeed;
    var b = forge$f.util.createBuffer();

    // paranoid deviation from Fortuna:
    // reset key for every request to protect previously
    // generated random bytes should the key be discovered;
    // there is no 100ms based reseeding because of this
    // forced reseed for every `generate` call
    ctx.key = null;

    generate();

    function generate(err) {
      if(err) {
        return callback(err);
      }

      // sufficient bytes generated
      if(b.length() >= count) {
        return callback(null, b.getBytes(count));
      }

      // if amount of data generated is greater than 1 MiB, trigger reseed
      if(ctx.generated > 0xfffff) {
        ctx.key = null;
      }

      if(ctx.key === null) {
        // prevent stack overflow
        return forge$f.util.nextTick(function() {
          _reseed(generate);
        });
      }

      // generate the random bytes
      var bytes = cipher(ctx.key, ctx.seed);
      ctx.generated += bytes.length;
      b.putBytes(bytes);

      // generate bytes for a new key and seed
      ctx.key = formatKey(cipher(ctx.key, increment(ctx.seed)));
      ctx.seed = formatSeed(cipher(ctx.key, ctx.seed));

      forge$f.util.setImmediate(generate);
    }
  };

  /**
   * Generates random bytes synchronously.
   *
   * @param count the number of random bytes to generate.
   *
   * @return count random bytes as a string.
   */
  ctx.generateSync = function(count) {
    // simple generator using counter-based CBC
    var cipher = ctx.plugin.cipher;
    var increment = ctx.plugin.increment;
    var formatKey = ctx.plugin.formatKey;
    var formatSeed = ctx.plugin.formatSeed;

    // paranoid deviation from Fortuna:
    // reset key for every request to protect previously
    // generated random bytes should the key be discovered;
    // there is no 100ms based reseeding because of this
    // forced reseed for every `generateSync` call
    ctx.key = null;

    var b = forge$f.util.createBuffer();
    while(b.length() < count) {
      // if amount of data generated is greater than 1 MiB, trigger reseed
      if(ctx.generated > 0xfffff) {
        ctx.key = null;
      }

      if(ctx.key === null) {
        _reseedSync();
      }

      // generate the random bytes
      var bytes = cipher(ctx.key, ctx.seed);
      ctx.generated += bytes.length;
      b.putBytes(bytes);

      // generate bytes for a new key and seed
      ctx.key = formatKey(cipher(ctx.key, increment(ctx.seed)));
      ctx.seed = formatSeed(cipher(ctx.key, ctx.seed));
    }

    return b.getBytes(count);
  };

  /**
   * Private function that asynchronously reseeds a generator.
   *
   * @param callback(err) called once the operation completes.
   */
  function _reseed(callback) {
    if(ctx.pools[0].messageLength >= 32) {
      _seed();
      return callback();
    }
    // not enough seed data...
    var needed = (32 - ctx.pools[0].messageLength) << 5;
    ctx.seedFile(needed, function(err, bytes) {
      if(err) {
        return callback(err);
      }
      ctx.collect(bytes);
      _seed();
      callback();
    });
  }

  /**
   * Private function that synchronously reseeds a generator.
   */
  function _reseedSync() {
    if(ctx.pools[0].messageLength >= 32) {
      return _seed();
    }
    // not enough seed data...
    var needed = (32 - ctx.pools[0].messageLength) << 5;
    ctx.collect(ctx.seedFileSync(needed));
    _seed();
  }

  /**
   * Private function that seeds a generator once enough bytes are available.
   */
  function _seed() {
    // update reseed count
    ctx.reseeds = (ctx.reseeds === 0xffffffff) ? 0 : ctx.reseeds + 1;

    // goal is to update `key` via:
    // key = hash(key + s)
    //   where 's' is all collected entropy from selected pools, then...

    // create a plugin-based message digest
    var md = ctx.plugin.md.create();

    // consume current key bytes
    md.update(ctx.keyBytes);

    // digest the entropy of pools whose index k meet the
    // condition 'n mod 2^k == 0' where n is the number of reseeds
    var _2powK = 1;
    for(var k = 0; k < 32; ++k) {
      if(ctx.reseeds % _2powK === 0) {
        md.update(ctx.pools[k].digest().getBytes());
        ctx.pools[k].start();
      }
      _2powK = _2powK << 1;
    }

    // get digest for key bytes
    ctx.keyBytes = md.digest().getBytes();

    // paranoid deviation from Fortuna:
    // update `seed` via `seed = hash(key)`
    // instead of initializing to zero once and only
    // ever incrementing it
    md.start();
    md.update(ctx.keyBytes);
    var seedBytes = md.digest().getBytes();

    // update state
    ctx.key = ctx.plugin.formatKey(ctx.keyBytes);
    ctx.seed = ctx.plugin.formatSeed(seedBytes);
    ctx.generated = 0;
  }

  /**
   * The built-in default seedFile. This seedFile is used when entropy
   * is needed immediately.
   *
   * @param needed the number of bytes that are needed.
   *
   * @return the random bytes.
   */
  function defaultSeedFile(needed) {
    // use window.crypto.getRandomValues strong source of entropy if available
    var getRandomValues = null;
    var globalScope = forge$f.util.globalScope;
    var _crypto = globalScope.crypto || globalScope.msCrypto;
    if(_crypto && _crypto.getRandomValues) {
      getRandomValues = function(arr) {
        return _crypto.getRandomValues(arr);
      };
    }

    var b = forge$f.util.createBuffer();
    if(getRandomValues) {
      while(b.length() < needed) {
        // max byte length is 65536 before QuotaExceededError is thrown
        // http://www.w3.org/TR/WebCryptoAPI/#RandomSource-method-getRandomValues
        var count = Math.max(1, Math.min(needed - b.length(), 65536) / 4);
        var entropy = new Uint32Array(Math.floor(count));
        try {
          getRandomValues(entropy);
          for(var i = 0; i < entropy.length; ++i) {
            b.putInt32(entropy[i]);
          }
        } catch(e) {
          /* only ignore QuotaExceededError */
          if(!(typeof QuotaExceededError !== 'undefined' &&
            e instanceof QuotaExceededError)) {
            throw e;
          }
        }
      }
    }

    // be sad and add some weak random data
    if(b.length() < needed) {
      /* Draws from Park-Miller "minimal standard" 31 bit PRNG,
      implemented with David G. Carta's optimization: with 32 bit math
      and without division (Public Domain). */
      var hi, lo, next;
      var seed = Math.floor(Math.random() * 0x010000);
      while(b.length() < needed) {
        lo = 16807 * (seed & 0xFFFF);
        hi = 16807 * (seed >> 16);
        lo += (hi & 0x7FFF) << 16;
        lo += hi >> 15;
        lo = (lo & 0x7FFFFFFF) + (lo >> 31);
        seed = lo & 0xFFFFFFFF;

        // consume lower 3 bytes of seed
        for(var i = 0; i < 3; ++i) {
          // throw in more pseudo random
          next = seed >>> (i << 3);
          next ^= Math.floor(Math.random() * 0x0100);
          b.putByte(next & 0xFF);
        }
      }
    }

    return b.getBytes(needed);
  }
  // initialize seed file APIs
  if(_crypto$1) {
    // use nodejs async API
    ctx.seedFile = function(needed, callback) {
      _crypto$1.randomBytes(needed, function(err, bytes) {
        if(err) {
          return callback(err);
        }
        callback(null, bytes.toString());
      });
    };
    // use nodejs sync API
    ctx.seedFileSync = function(needed) {
      return _crypto$1.randomBytes(needed).toString();
    };
  } else {
    ctx.seedFile = function(needed, callback) {
      try {
        callback(null, defaultSeedFile(needed));
      } catch(e) {
        callback(e);
      }
    };
    ctx.seedFileSync = defaultSeedFile;
  }

  /**
   * Adds entropy to a prng ctx's accumulator.
   *
   * @param bytes the bytes of entropy as a string.
   */
  ctx.collect = function(bytes) {
    // iterate over pools distributing entropy cyclically
    var count = bytes.length;
    for(var i = 0; i < count; ++i) {
      ctx.pools[ctx.pool].update(bytes.substr(i, 1));
      ctx.pool = (ctx.pool === 31) ? 0 : ctx.pool + 1;
    }
  };

  /**
   * Collects an integer of n bits.
   *
   * @param i the integer entropy.
   * @param n the number of bits in the integer.
   */
  ctx.collectInt = function(i, n) {
    var bytes = '';
    for(var x = 0; x < n; x += 8) {
      bytes += String.fromCharCode((i >> x) & 0xFF);
    }
    ctx.collect(bytes);
  };

  /**
   * Registers a Web Worker to receive immediate entropy from the main thread.
   * This method is required until Web Workers can access the native crypto
   * API. This method should be called twice for each created worker, once in
   * the main thread, and once in the worker itself.
   *
   * @param worker the worker to register.
   */
  ctx.registerWorker = function(worker) {
    // worker receives random bytes
    if(worker === self) {
      ctx.seedFile = function(needed, callback) {
        function listener(e) {
          var data = e.data;
          if(data.forge && data.forge.prng) {
            self.removeEventListener('message', listener);
            callback(data.forge.prng.err, data.forge.prng.bytes);
          }
        }
        self.addEventListener('message', listener);
        self.postMessage({forge: {prng: {needed: needed}}});
      };
    } else {
      // main thread sends random bytes upon request
      var listener = function(e) {
        var data = e.data;
        if(data.forge && data.forge.prng) {
          ctx.seedFile(data.forge.prng.needed, function(err, bytes) {
            worker.postMessage({forge: {prng: {err: err, bytes: bytes}}});
          });
        }
      };
      // TODO: do we need to remove the event listener when the worker dies?
      worker.addEventListener('message', listener);
    }
  };

  return ctx;
};

/**
 * An API for getting cryptographically-secure random bytes. The bytes are
 * generated using the Fortuna algorithm devised by Bruce Schneier and
 * Niels Ferguson.
 *
 * Getting strong random bytes is not yet easy to do in javascript. The only
 * truish random entropy that can be collected is from the mouse, keyboard, or
 * from timing with respect to page loads, etc. This generator makes a poor
 * attempt at providing random bytes when those sources haven't yet provided
 * enough entropy to initially seed or to reseed the PRNG.
 *
 * @author Dave Longley
 *
 * Copyright (c) 2009-2014 Digital Bazaar, Inc.
 */

var forge$e = forge$s;





(function() {

// forge.random already defined
if(forge$e.random && forge$e.random.getBytes) {
  return;
}

(function(jQuery) {

// the default prng plugin, uses AES-128
var prng_aes = {};
var _prng_aes_output = new Array(4);
var _prng_aes_buffer = forge$e.util.createBuffer();
prng_aes.formatKey = function(key) {
  // convert the key into 32-bit integers
  var tmp = forge$e.util.createBuffer(key);
  key = new Array(4);
  key[0] = tmp.getInt32();
  key[1] = tmp.getInt32();
  key[2] = tmp.getInt32();
  key[3] = tmp.getInt32();

  // return the expanded key
  return forge$e.aes._expandKey(key, false);
};
prng_aes.formatSeed = function(seed) {
  // convert seed into 32-bit integers
  var tmp = forge$e.util.createBuffer(seed);
  seed = new Array(4);
  seed[0] = tmp.getInt32();
  seed[1] = tmp.getInt32();
  seed[2] = tmp.getInt32();
  seed[3] = tmp.getInt32();
  return seed;
};
prng_aes.cipher = function(key, seed) {
  forge$e.aes._updateBlock(key, seed, _prng_aes_output, false);
  _prng_aes_buffer.putInt32(_prng_aes_output[0]);
  _prng_aes_buffer.putInt32(_prng_aes_output[1]);
  _prng_aes_buffer.putInt32(_prng_aes_output[2]);
  _prng_aes_buffer.putInt32(_prng_aes_output[3]);
  return _prng_aes_buffer.getBytes();
};
prng_aes.increment = function(seed) {
  // FIXME: do we care about carry or signed issues?
  ++seed[3];
  return seed;
};
prng_aes.md = forge$e.md.sha256;

/**
 * Creates a new PRNG.
 */
function spawnPrng() {
  var ctx = forge$e.prng.create(prng_aes);

  /**
   * Gets random bytes. If a native secure crypto API is unavailable, this
   * method tries to make the bytes more unpredictable by drawing from data that
   * can be collected from the user of the browser, eg: mouse movement.
   *
   * If a callback is given, this method will be called asynchronously.
   *
   * @param count the number of random bytes to get.
   * @param [callback(err, bytes)] called once the operation completes.
   *
   * @return the random bytes in a string.
   */
  ctx.getBytes = function(count, callback) {
    return ctx.generate(count, callback);
  };

  /**
   * Gets random bytes asynchronously. If a native secure crypto API is
   * unavailable, this method tries to make the bytes more unpredictable by
   * drawing from data that can be collected from the user of the browser,
   * eg: mouse movement.
   *
   * @param count the number of random bytes to get.
   *
   * @return the random bytes in a string.
   */
  ctx.getBytesSync = function(count) {
    return ctx.generate(count);
  };

  return ctx;
}

// create default prng context
var _ctx = spawnPrng();

// add other sources of entropy only if window.crypto.getRandomValues is not
// available -- otherwise this source will be automatically used by the prng
var getRandomValues = null;
var globalScope = forge$e.util.globalScope;
var _crypto = globalScope.crypto || globalScope.msCrypto;
if(_crypto && _crypto.getRandomValues) {
  getRandomValues = function(arr) {
    return _crypto.getRandomValues(arr);
  };
}

if((!forge$e.util.isNodejs && !getRandomValues)) {

  // get load time entropy
  _ctx.collectInt(+new Date(), 32);

  // add some entropy from navigator object
  if(typeof(navigator) !== 'undefined') {
    var _navBytes = '';
    for(var key in navigator) {
      try {
        if(typeof(navigator[key]) == 'string') {
          _navBytes += navigator[key];
        }
      } catch(e) {
        /* Some navigator keys might not be accessible, e.g. the geolocation
          attribute throws an exception if touched in Mozilla chrome://
          context.

          Silently ignore this and just don't use this as a source of
          entropy. */
      }
    }
    _ctx.collect(_navBytes);
    _navBytes = null;
  }

  // add mouse and keyboard collectors if jquery is available
  if(jQuery) {
    // set up mouse entropy capture
    jQuery().mousemove(function(e) {
      // add mouse coords
      _ctx.collectInt(e.clientX, 16);
      _ctx.collectInt(e.clientY, 16);
    });

    // set up keyboard entropy capture
    jQuery().keypress(function(e) {
      _ctx.collectInt(e.charCode, 8);
    });
  }
}

/* Random API */
if(!forge$e.random) {
  forge$e.random = _ctx;
} else {
  // extend forge.random with _ctx
  for(var key in _ctx) {
    forge$e.random[key] = _ctx[key];
  }
}

// expose spawn PRNG
forge$e.random.createInstance = spawnPrng;

})(typeof(jQuery) !== 'undefined' ? jQuery : null);

})();

/**
 * RC2 implementation.
 *
 * @author Stefan Siegl
 *
 * Copyright (c) 2012 Stefan Siegl <stesie@brokenpipe.de>
 *
 * Information on the RC2 cipher is available from RFC #2268,
 * http://www.ietf.org/rfc/rfc2268.txt
 */

var forge$d = forge$s;


var piTable = [
  0xd9, 0x78, 0xf9, 0xc4, 0x19, 0xdd, 0xb5, 0xed, 0x28, 0xe9, 0xfd, 0x79, 0x4a, 0xa0, 0xd8, 0x9d,
  0xc6, 0x7e, 0x37, 0x83, 0x2b, 0x76, 0x53, 0x8e, 0x62, 0x4c, 0x64, 0x88, 0x44, 0x8b, 0xfb, 0xa2,
  0x17, 0x9a, 0x59, 0xf5, 0x87, 0xb3, 0x4f, 0x13, 0x61, 0x45, 0x6d, 0x8d, 0x09, 0x81, 0x7d, 0x32,
  0xbd, 0x8f, 0x40, 0xeb, 0x86, 0xb7, 0x7b, 0x0b, 0xf0, 0x95, 0x21, 0x22, 0x5c, 0x6b, 0x4e, 0x82,
  0x54, 0xd6, 0x65, 0x93, 0xce, 0x60, 0xb2, 0x1c, 0x73, 0x56, 0xc0, 0x14, 0xa7, 0x8c, 0xf1, 0xdc,
  0x12, 0x75, 0xca, 0x1f, 0x3b, 0xbe, 0xe4, 0xd1, 0x42, 0x3d, 0xd4, 0x30, 0xa3, 0x3c, 0xb6, 0x26,
  0x6f, 0xbf, 0x0e, 0xda, 0x46, 0x69, 0x07, 0x57, 0x27, 0xf2, 0x1d, 0x9b, 0xbc, 0x94, 0x43, 0x03,
  0xf8, 0x11, 0xc7, 0xf6, 0x90, 0xef, 0x3e, 0xe7, 0x06, 0xc3, 0xd5, 0x2f, 0xc8, 0x66, 0x1e, 0xd7,
  0x08, 0xe8, 0xea, 0xde, 0x80, 0x52, 0xee, 0xf7, 0x84, 0xaa, 0x72, 0xac, 0x35, 0x4d, 0x6a, 0x2a,
  0x96, 0x1a, 0xd2, 0x71, 0x5a, 0x15, 0x49, 0x74, 0x4b, 0x9f, 0xd0, 0x5e, 0x04, 0x18, 0xa4, 0xec,
  0xc2, 0xe0, 0x41, 0x6e, 0x0f, 0x51, 0xcb, 0xcc, 0x24, 0x91, 0xaf, 0x50, 0xa1, 0xf4, 0x70, 0x39,
  0x99, 0x7c, 0x3a, 0x85, 0x23, 0xb8, 0xb4, 0x7a, 0xfc, 0x02, 0x36, 0x5b, 0x25, 0x55, 0x97, 0x31,
  0x2d, 0x5d, 0xfa, 0x98, 0xe3, 0x8a, 0x92, 0xae, 0x05, 0xdf, 0x29, 0x10, 0x67, 0x6c, 0xba, 0xc9,
  0xd3, 0x00, 0xe6, 0xcf, 0xe1, 0x9e, 0xa8, 0x2c, 0x63, 0x16, 0x01, 0x3f, 0x58, 0xe2, 0x89, 0xa9,
  0x0d, 0x38, 0x34, 0x1b, 0xab, 0x33, 0xff, 0xb0, 0xbb, 0x48, 0x0c, 0x5f, 0xb9, 0xb1, 0xcd, 0x2e,
  0xc5, 0xf3, 0xdb, 0x47, 0xe5, 0xa5, 0x9c, 0x77, 0x0a, 0xa6, 0x20, 0x68, 0xfe, 0x7f, 0xc1, 0xad
];

var s = [1, 2, 3, 5];

/**
 * Rotate a word left by given number of bits.
 *
 * Bits that are shifted out on the left are put back in on the right
 * hand side.
 *
 * @param word The word to shift left.
 * @param bits The number of bits to shift by.
 * @return The rotated word.
 */
var rol = function(word, bits) {
  return ((word << bits) & 0xffff) | ((word & 0xffff) >> (16 - bits));
};

/**
 * Rotate a word right by given number of bits.
 *
 * Bits that are shifted out on the right are put back in on the left
 * hand side.
 *
 * @param word The word to shift right.
 * @param bits The number of bits to shift by.
 * @return The rotated word.
 */
var ror = function(word, bits) {
  return ((word & 0xffff) >> bits) | ((word << (16 - bits)) & 0xffff);
};

/* RC2 API */
forge$d.rc2 = forge$d.rc2 || {};

/**
 * Perform RC2 key expansion as per RFC #2268, section 2.
 *
 * @param key variable-length user key (between 1 and 128 bytes)
 * @param effKeyBits number of effective key bits (default: 128)
 * @return the expanded RC2 key (ByteBuffer of 128 bytes)
 */
forge$d.rc2.expandKey = function(key, effKeyBits) {
  if(typeof key === 'string') {
    key = forge$d.util.createBuffer(key);
  }
  effKeyBits = effKeyBits || 128;

  /* introduce variables that match the names used in RFC #2268 */
  var L = key;
  var T = key.length();
  var T1 = effKeyBits;
  var T8 = Math.ceil(T1 / 8);
  var TM = 0xff >> (T1 & 0x07);
  var i;

  for(i = T; i < 128; i++) {
    L.putByte(piTable[(L.at(i - 1) + L.at(i - T)) & 0xff]);
  }

  L.setAt(128 - T8, piTable[L.at(128 - T8) & TM]);

  for(i = 127 - T8; i >= 0; i--) {
    L.setAt(i, piTable[L.at(i + 1) ^ L.at(i + T8)]);
  }

  return L;
};

/**
 * Creates a RC2 cipher object.
 *
 * @param key the symmetric key to use (as base for key generation).
 * @param bits the number of effective key bits.
 * @param encrypt false for decryption, true for encryption.
 *
 * @return the cipher.
 */
var createCipher = function(key, bits, encrypt) {
  var _finish = false, _input = null, _output = null, _iv = null;
  var mixRound, mashRound;
  var i, j, K = [];

  /* Expand key and fill into K[] Array */
  key = forge$d.rc2.expandKey(key, bits);
  for(i = 0; i < 64; i++) {
    K.push(key.getInt16Le());
  }

  if(encrypt) {
    /**
     * Perform one mixing round "in place".
     *
     * @param R Array of four words to perform mixing on.
     */
    mixRound = function(R) {
      for(i = 0; i < 4; i++) {
        R[i] += K[j] + (R[(i + 3) % 4] & R[(i + 2) % 4]) +
          ((~R[(i + 3) % 4]) & R[(i + 1) % 4]);
        R[i] = rol(R[i], s[i]);
        j++;
      }
    };

    /**
     * Perform one mashing round "in place".
     *
     * @param R Array of four words to perform mashing on.
     */
    mashRound = function(R) {
      for(i = 0; i < 4; i++) {
        R[i] += K[R[(i + 3) % 4] & 63];
      }
    };
  } else {
    /**
     * Perform one r-mixing round "in place".
     *
     * @param R Array of four words to perform mixing on.
     */
    mixRound = function(R) {
      for(i = 3; i >= 0; i--) {
        R[i] = ror(R[i], s[i]);
        R[i] -= K[j] + (R[(i + 3) % 4] & R[(i + 2) % 4]) +
          ((~R[(i + 3) % 4]) & R[(i + 1) % 4]);
        j--;
      }
    };

    /**
     * Perform one r-mashing round "in place".
     *
     * @param R Array of four words to perform mashing on.
     */
    mashRound = function(R) {
      for(i = 3; i >= 0; i--) {
        R[i] -= K[R[(i + 3) % 4] & 63];
      }
    };
  }

  /**
   * Run the specified cipher execution plan.
   *
   * This function takes four words from the input buffer, applies the IV on
   * it (if requested) and runs the provided execution plan.
   *
   * The plan must be put together in form of a array of arrays.  Where the
   * outer one is simply a list of steps to perform and the inner one needs
   * to have two elements: the first one telling how many rounds to perform,
   * the second one telling what to do (i.e. the function to call).
   *
   * @param {Array} plan The plan to execute.
   */
  var runPlan = function(plan) {
    var R = [];

    /* Get data from input buffer and fill the four words into R */
    for(i = 0; i < 4; i++) {
      var val = _input.getInt16Le();

      if(_iv !== null) {
        if(encrypt) {
          /* We're encrypting, apply the IV first. */
          val ^= _iv.getInt16Le();
        } else {
          /* We're decryption, keep cipher text for next block. */
          _iv.putInt16Le(val);
        }
      }

      R.push(val & 0xffff);
    }

    /* Reset global "j" variable as per spec. */
    j = encrypt ? 0 : 63;

    /* Run execution plan. */
    for(var ptr = 0; ptr < plan.length; ptr++) {
      for(var ctr = 0; ctr < plan[ptr][0]; ctr++) {
        plan[ptr][1](R);
      }
    }

    /* Write back result to output buffer. */
    for(i = 0; i < 4; i++) {
      if(_iv !== null) {
        if(encrypt) {
          /* We're encrypting in CBC-mode, feed back encrypted bytes into
             IV buffer to carry it forward to next block. */
          _iv.putInt16Le(R[i]);
        } else {
          R[i] ^= _iv.getInt16Le();
        }
      }

      _output.putInt16Le(R[i]);
    }
  };

  /* Create cipher object */
  var cipher = null;
  cipher = {
    /**
     * Starts or restarts the encryption or decryption process, whichever
     * was previously configured.
     *
     * To use the cipher in CBC mode, iv may be given either as a string
     * of bytes, or as a byte buffer.  For ECB mode, give null as iv.
     *
     * @param iv the initialization vector to use, null for ECB mode.
     * @param output the output the buffer to write to, null to create one.
     */
    start: function(iv, output) {
      if(iv) {
        /* CBC mode */
        if(typeof iv === 'string') {
          iv = forge$d.util.createBuffer(iv);
        }
      }

      _finish = false;
      _input = forge$d.util.createBuffer();
      _output = output || new forge$d.util.createBuffer();
      _iv = iv;

      cipher.output = _output;
    },

    /**
     * Updates the next block.
     *
     * @param input the buffer to read from.
     */
    update: function(input) {
      if(!_finish) {
        // not finishing, so fill the input buffer with more input
        _input.putBuffer(input);
      }

      while(_input.length() >= 8) {
        runPlan([
            [ 5, mixRound ],
            [ 1, mashRound ],
            [ 6, mixRound ],
            [ 1, mashRound ],
            [ 5, mixRound ]
          ]);
      }
    },

    /**
     * Finishes encrypting or decrypting.
     *
     * @param pad a padding function to use, null for PKCS#7 padding,
     *           signature(blockSize, buffer, decrypt).
     *
     * @return true if successful, false on error.
     */
    finish: function(pad) {
      var rval = true;

      if(encrypt) {
        if(pad) {
          rval = pad(8, _input, !encrypt);
        } else {
          // add PKCS#7 padding to block (each pad byte is the
          // value of the number of pad bytes)
          var padding = (_input.length() === 8) ? 8 : (8 - _input.length());
          _input.fillWithByte(padding, padding);
        }
      }

      if(rval) {
        // do final update
        _finish = true;
        cipher.update();
      }

      if(!encrypt) {
        // check for error: input data not a multiple of block size
        rval = (_input.length() === 0);
        if(rval) {
          if(pad) {
            rval = pad(8, _output, !encrypt);
          } else {
            // ensure padding byte count is valid
            var len = _output.length();
            var count = _output.at(len - 1);

            if(count > len) {
              rval = false;
            } else {
              // trim off padding bytes
              _output.truncate(count);
            }
          }
        }
      }

      return rval;
    }
  };

  return cipher;
};

/**
 * Creates an RC2 cipher object to encrypt data in ECB or CBC mode using the
 * given symmetric key. The output will be stored in the 'output' member
 * of the returned cipher.
 *
 * The key and iv may be given as a string of bytes or a byte buffer.
 * The cipher is initialized to use 128 effective key bits.
 *
 * @param key the symmetric key to use.
 * @param iv the initialization vector to use.
 * @param output the buffer to write to, null to create one.
 *
 * @return the cipher.
 */
forge$d.rc2.startEncrypting = function(key, iv, output) {
  var cipher = forge$d.rc2.createEncryptionCipher(key, 128);
  cipher.start(iv, output);
  return cipher;
};

/**
 * Creates an RC2 cipher object to encrypt data in ECB or CBC mode using the
 * given symmetric key.
 *
 * The key may be given as a string of bytes or a byte buffer.
 *
 * To start encrypting call start() on the cipher with an iv and optional
 * output buffer.
 *
 * @param key the symmetric key to use.
 *
 * @return the cipher.
 */
forge$d.rc2.createEncryptionCipher = function(key, bits) {
  return createCipher(key, bits, true);
};

/**
 * Creates an RC2 cipher object to decrypt data in ECB or CBC mode using the
 * given symmetric key. The output will be stored in the 'output' member
 * of the returned cipher.
 *
 * The key and iv may be given as a string of bytes or a byte buffer.
 * The cipher is initialized to use 128 effective key bits.
 *
 * @param key the symmetric key to use.
 * @param iv the initialization vector to use.
 * @param output the buffer to write to, null to create one.
 *
 * @return the cipher.
 */
forge$d.rc2.startDecrypting = function(key, iv, output) {
  var cipher = forge$d.rc2.createDecryptionCipher(key, 128);
  cipher.start(iv, output);
  return cipher;
};

/**
 * Creates an RC2 cipher object to decrypt data in ECB or CBC mode using the
 * given symmetric key.
 *
 * The key may be given as a string of bytes or a byte buffer.
 *
 * To start decrypting call start() on the cipher with an iv and optional
 * output buffer.
 *
 * @param key the symmetric key to use.
 *
 * @return the cipher.
 */
forge$d.rc2.createDecryptionCipher = function(key, bits) {
  return createCipher(key, bits, false);
};

// Copyright (c) 2005  Tom Wu
// All Rights Reserved.
// See "LICENSE" for details.

// Basic JavaScript BN library - subset useful for RSA encryption.

/*
Licensing (LICENSE)
-------------------

This software is covered under the following copyright:
*/
/*
 * Copyright (c) 2003-2005  Tom Wu
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL TOM WU BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT ADVISED OF
 * THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * In addition, the following condition applies:
 *
 * All redistributions must retain an intact copy of this copyright notice
 * and disclaimer.
 */
/*
Address all questions regarding this license to:

  Tom Wu
  tjw@cs.Stanford.EDU
*/
var forge$c = forge$s;

forge$c.jsbn = forge$c.jsbn || {};

// Bits per digit
var dbits;

// (public) Constructor
function BigInteger$1(a,b,c) {
  this.data = [];
  if(a != null)
    if("number" == typeof a) this.fromNumber(a,b,c);
    else if(b == null && "string" != typeof a) this.fromString(a,256);
    else this.fromString(a,b);
}
forge$c.jsbn.BigInteger = BigInteger$1;

// return new, unset BigInteger
function nbi() { return new BigInteger$1(null); }

// am: Compute w_j += (x*this_i), propagate carries,
// c is initial carry, returns final carry.
// c < 3*dvalue, x < 2*dvalue, this_i < dvalue
// We need to select the fastest one that works in this environment.

// am1: use a single mult and divide to get the high bits,
// max digit bits should be 26 because
// max internal value = 2*dvalue^2-2*dvalue (< 2^53)
function am1(i,x,w,j,c,n) {
  while(--n >= 0) {
    var v = x*this.data[i++]+w.data[j]+c;
    c = Math.floor(v/0x4000000);
    w.data[j++] = v&0x3ffffff;
  }
  return c;
}
// am2 avoids a big mult-and-extract completely.
// Max digit bits should be <= 30 because we do bitwise ops
// on values up to 2*hdvalue^2-hdvalue-1 (< 2^31)
function am2(i,x,w,j,c,n) {
  var xl = x&0x7fff, xh = x>>15;
  while(--n >= 0) {
    var l = this.data[i]&0x7fff;
    var h = this.data[i++]>>15;
    var m = xh*l+h*xl;
    l = xl*l+((m&0x7fff)<<15)+w.data[j]+(c&0x3fffffff);
    c = (l>>>30)+(m>>>15)+xh*h+(c>>>30);
    w.data[j++] = l&0x3fffffff;
  }
  return c;
}
// Alternately, set max digit bits to 28 since some
// browsers slow down when dealing with 32-bit numbers.
function am3(i,x,w,j,c,n) {
  var xl = x&0x3fff, xh = x>>14;
  while(--n >= 0) {
    var l = this.data[i]&0x3fff;
    var h = this.data[i++]>>14;
    var m = xh*l+h*xl;
    l = xl*l+((m&0x3fff)<<14)+w.data[j]+c;
    c = (l>>28)+(m>>14)+xh*h;
    w.data[j++] = l&0xfffffff;
  }
  return c;
}

// node.js (no browser)
if(typeof(navigator) === 'undefined')
{
   BigInteger$1.prototype.am = am3;
   dbits = 28;
} else if((navigator.appName == "Microsoft Internet Explorer")) {
  BigInteger$1.prototype.am = am2;
  dbits = 30;
} else if((navigator.appName != "Netscape")) {
  BigInteger$1.prototype.am = am1;
  dbits = 26;
} else { // Mozilla/Netscape seems to prefer am3
  BigInteger$1.prototype.am = am3;
  dbits = 28;
}

BigInteger$1.prototype.DB = dbits;
BigInteger$1.prototype.DM = ((1<<dbits)-1);
BigInteger$1.prototype.DV = (1<<dbits);

var BI_FP = 52;
BigInteger$1.prototype.FV = Math.pow(2,BI_FP);
BigInteger$1.prototype.F1 = BI_FP-dbits;
BigInteger$1.prototype.F2 = 2*dbits-BI_FP;

// Digit conversions
var BI_RM = "0123456789abcdefghijklmnopqrstuvwxyz";
var BI_RC = new Array();
var rr,vv;
rr = "0".charCodeAt(0);
for(vv = 0; vv <= 9; ++vv) BI_RC[rr++] = vv;
rr = "a".charCodeAt(0);
for(vv = 10; vv < 36; ++vv) BI_RC[rr++] = vv;
rr = "A".charCodeAt(0);
for(vv = 10; vv < 36; ++vv) BI_RC[rr++] = vv;

function int2char(n) { return BI_RM.charAt(n); }
function intAt(s,i) {
  var c = BI_RC[s.charCodeAt(i)];
  return (c==null)?-1:c;
}

// (protected) copy this to r
function bnpCopyTo(r) {
  for(var i = this.t-1; i >= 0; --i) r.data[i] = this.data[i];
  r.t = this.t;
  r.s = this.s;
}

// (protected) set from integer value x, -DV <= x < DV
function bnpFromInt(x) {
  this.t = 1;
  this.s = (x<0)?-1:0;
  if(x > 0) this.data[0] = x;
  else if(x < -1) this.data[0] = x+this.DV;
  else this.t = 0;
}

// return bigint initialized to value
function nbv(i) { var r = nbi(); r.fromInt(i); return r; }

// (protected) set from string and radix
function bnpFromString(s,b) {
  var k;
  if(b == 16) k = 4;
  else if(b == 8) k = 3;
  else if(b == 256) k = 8; // byte array
  else if(b == 2) k = 1;
  else if(b == 32) k = 5;
  else if(b == 4) k = 2;
  else { this.fromRadix(s,b); return; }
  this.t = 0;
  this.s = 0;
  var i = s.length, mi = false, sh = 0;
  while(--i >= 0) {
    var x = (k==8)?s[i]&0xff:intAt(s,i);
    if(x < 0) {
      if(s.charAt(i) == "-") mi = true;
      continue;
    }
    mi = false;
    if(sh == 0)
      this.data[this.t++] = x;
    else if(sh+k > this.DB) {
      this.data[this.t-1] |= (x&((1<<(this.DB-sh))-1))<<sh;
      this.data[this.t++] = (x>>(this.DB-sh));
    } else
      this.data[this.t-1] |= x<<sh;
    sh += k;
    if(sh >= this.DB) sh -= this.DB;
  }
  if(k == 8 && (s[0]&0x80) != 0) {
    this.s = -1;
    if(sh > 0) this.data[this.t-1] |= ((1<<(this.DB-sh))-1)<<sh;
  }
  this.clamp();
  if(mi) BigInteger$1.ZERO.subTo(this,this);
}

// (protected) clamp off excess high words
function bnpClamp() {
  var c = this.s&this.DM;
  while(this.t > 0 && this.data[this.t-1] == c) --this.t;
}

// (public) return string representation in given radix
function bnToString(b) {
  if(this.s < 0) return "-"+this.negate().toString(b);
  var k;
  if(b == 16) k = 4;
  else if(b == 8) k = 3;
  else if(b == 2) k = 1;
  else if(b == 32) k = 5;
  else if(b == 4) k = 2;
  else return this.toRadix(b);
  var km = (1<<k)-1, d, m = false, r = "", i = this.t;
  var p = this.DB-(i*this.DB)%k;
  if(i-- > 0) {
    if(p < this.DB && (d = this.data[i]>>p) > 0) { m = true; r = int2char(d); }
    while(i >= 0) {
      if(p < k) {
        d = (this.data[i]&((1<<p)-1))<<(k-p);
        d |= this.data[--i]>>(p+=this.DB-k);
      } else {
        d = (this.data[i]>>(p-=k))&km;
        if(p <= 0) { p += this.DB; --i; }
      }
      if(d > 0) m = true;
      if(m) r += int2char(d);
    }
  }
  return m?r:"0";
}

// (public) -this
function bnNegate() { var r = nbi(); BigInteger$1.ZERO.subTo(this,r); return r; }

// (public) |this|
function bnAbs() { return (this.s<0)?this.negate():this; }

// (public) return + if this > a, - if this < a, 0 if equal
function bnCompareTo(a) {
  var r = this.s-a.s;
  if(r != 0) return r;
  var i = this.t;
  r = i-a.t;
  if(r != 0) return (this.s<0)?-r:r;
  while(--i >= 0) if((r=this.data[i]-a.data[i]) != 0) return r;
  return 0;
}

// returns bit length of the integer x
function nbits(x) {
  var r = 1, t;
  if((t=x>>>16) != 0) { x = t; r += 16; }
  if((t=x>>8) != 0) { x = t; r += 8; }
  if((t=x>>4) != 0) { x = t; r += 4; }
  if((t=x>>2) != 0) { x = t; r += 2; }
  if((t=x>>1) != 0) { x = t; r += 1; }
  return r;
}

// (public) return the number of bits in "this"
function bnBitLength() {
  if(this.t <= 0) return 0;
  return this.DB*(this.t-1)+nbits(this.data[this.t-1]^(this.s&this.DM));
}

// (protected) r = this << n*DB
function bnpDLShiftTo(n,r) {
  var i;
  for(i = this.t-1; i >= 0; --i) r.data[i+n] = this.data[i];
  for(i = n-1; i >= 0; --i) r.data[i] = 0;
  r.t = this.t+n;
  r.s = this.s;
}

// (protected) r = this >> n*DB
function bnpDRShiftTo(n,r) {
  for(var i = n; i < this.t; ++i) r.data[i-n] = this.data[i];
  r.t = Math.max(this.t-n,0);
  r.s = this.s;
}

// (protected) r = this << n
function bnpLShiftTo(n,r) {
  var bs = n%this.DB;
  var cbs = this.DB-bs;
  var bm = (1<<cbs)-1;
  var ds = Math.floor(n/this.DB), c = (this.s<<bs)&this.DM, i;
  for(i = this.t-1; i >= 0; --i) {
    r.data[i+ds+1] = (this.data[i]>>cbs)|c;
    c = (this.data[i]&bm)<<bs;
  }
  for(i = ds-1; i >= 0; --i) r.data[i] = 0;
  r.data[ds] = c;
  r.t = this.t+ds+1;
  r.s = this.s;
  r.clamp();
}

// (protected) r = this >> n
function bnpRShiftTo(n,r) {
  r.s = this.s;
  var ds = Math.floor(n/this.DB);
  if(ds >= this.t) { r.t = 0; return; }
  var bs = n%this.DB;
  var cbs = this.DB-bs;
  var bm = (1<<bs)-1;
  r.data[0] = this.data[ds]>>bs;
  for(var i = ds+1; i < this.t; ++i) {
    r.data[i-ds-1] |= (this.data[i]&bm)<<cbs;
    r.data[i-ds] = this.data[i]>>bs;
  }
  if(bs > 0) r.data[this.t-ds-1] |= (this.s&bm)<<cbs;
  r.t = this.t-ds;
  r.clamp();
}

// (protected) r = this - a
function bnpSubTo(a,r) {
  var i = 0, c = 0, m = Math.min(a.t,this.t);
  while(i < m) {
    c += this.data[i]-a.data[i];
    r.data[i++] = c&this.DM;
    c >>= this.DB;
  }
  if(a.t < this.t) {
    c -= a.s;
    while(i < this.t) {
      c += this.data[i];
      r.data[i++] = c&this.DM;
      c >>= this.DB;
    }
    c += this.s;
  } else {
    c += this.s;
    while(i < a.t) {
      c -= a.data[i];
      r.data[i++] = c&this.DM;
      c >>= this.DB;
    }
    c -= a.s;
  }
  r.s = (c<0)?-1:0;
  if(c < -1) r.data[i++] = this.DV+c;
  else if(c > 0) r.data[i++] = c;
  r.t = i;
  r.clamp();
}

// (protected) r = this * a, r != this,a (HAC 14.12)
// "this" should be the larger one if appropriate.
function bnpMultiplyTo(a,r) {
  var x = this.abs(), y = a.abs();
  var i = x.t;
  r.t = i+y.t;
  while(--i >= 0) r.data[i] = 0;
  for(i = 0; i < y.t; ++i) r.data[i+x.t] = x.am(0,y.data[i],r,i,0,x.t);
  r.s = 0;
  r.clamp();
  if(this.s != a.s) BigInteger$1.ZERO.subTo(r,r);
}

// (protected) r = this^2, r != this (HAC 14.16)
function bnpSquareTo(r) {
  var x = this.abs();
  var i = r.t = 2*x.t;
  while(--i >= 0) r.data[i] = 0;
  for(i = 0; i < x.t-1; ++i) {
    var c = x.am(i,x.data[i],r,2*i,0,1);
    if((r.data[i+x.t]+=x.am(i+1,2*x.data[i],r,2*i+1,c,x.t-i-1)) >= x.DV) {
      r.data[i+x.t] -= x.DV;
      r.data[i+x.t+1] = 1;
    }
  }
  if(r.t > 0) r.data[r.t-1] += x.am(i,x.data[i],r,2*i,0,1);
  r.s = 0;
  r.clamp();
}

// (protected) divide this by m, quotient and remainder to q, r (HAC 14.20)
// r != q, this != m.  q or r may be null.
function bnpDivRemTo(m,q,r) {
  var pm = m.abs();
  if(pm.t <= 0) return;
  var pt = this.abs();
  if(pt.t < pm.t) {
    if(q != null) q.fromInt(0);
    if(r != null) this.copyTo(r);
    return;
  }
  if(r == null) r = nbi();
  var y = nbi(), ts = this.s, ms = m.s;
  var nsh = this.DB-nbits(pm.data[pm.t-1]);	// normalize modulus
  if(nsh > 0) { pm.lShiftTo(nsh,y); pt.lShiftTo(nsh,r); } else { pm.copyTo(y); pt.copyTo(r); }
  var ys = y.t;
  var y0 = y.data[ys-1];
  if(y0 == 0) return;
  var yt = y0*(1<<this.F1)+((ys>1)?y.data[ys-2]>>this.F2:0);
  var d1 = this.FV/yt, d2 = (1<<this.F1)/yt, e = 1<<this.F2;
  var i = r.t, j = i-ys, t = (q==null)?nbi():q;
  y.dlShiftTo(j,t);
  if(r.compareTo(t) >= 0) {
    r.data[r.t++] = 1;
    r.subTo(t,r);
  }
  BigInteger$1.ONE.dlShiftTo(ys,t);
  t.subTo(y,y);	// "negative" y so we can replace sub with am later
  while(y.t < ys) y.data[y.t++] = 0;
  while(--j >= 0) {
    // Estimate quotient digit
    var qd = (r.data[--i]==y0)?this.DM:Math.floor(r.data[i]*d1+(r.data[i-1]+e)*d2);
    if((r.data[i]+=y.am(0,qd,r,j,0,ys)) < qd) {	// Try it out
      y.dlShiftTo(j,t);
      r.subTo(t,r);
      while(r.data[i] < --qd) r.subTo(t,r);
    }
  }
  if(q != null) {
    r.drShiftTo(ys,q);
    if(ts != ms) BigInteger$1.ZERO.subTo(q,q);
  }
  r.t = ys;
  r.clamp();
  if(nsh > 0) r.rShiftTo(nsh,r);	// Denormalize remainder
  if(ts < 0) BigInteger$1.ZERO.subTo(r,r);
}

// (public) this mod a
function bnMod(a) {
  var r = nbi();
  this.abs().divRemTo(a,null,r);
  if(this.s < 0 && r.compareTo(BigInteger$1.ZERO) > 0) a.subTo(r,r);
  return r;
}

// Modular reduction using "classic" algorithm
function Classic(m) { this.m = m; }
function cConvert(x) {
  if(x.s < 0 || x.compareTo(this.m) >= 0) return x.mod(this.m);
  else return x;
}
function cRevert(x) { return x; }
function cReduce(x) { x.divRemTo(this.m,null,x); }
function cMulTo(x,y,r) { x.multiplyTo(y,r); this.reduce(r); }
function cSqrTo(x,r) { x.squareTo(r); this.reduce(r); }

Classic.prototype.convert = cConvert;
Classic.prototype.revert = cRevert;
Classic.prototype.reduce = cReduce;
Classic.prototype.mulTo = cMulTo;
Classic.prototype.sqrTo = cSqrTo;

// (protected) return "-1/this % 2^DB"; useful for Mont. reduction
// justification:
//         xy == 1 (mod m)
//         xy =  1+km
//   xy(2-xy) = (1+km)(1-km)
// x[y(2-xy)] = 1-k^2m^2
// x[y(2-xy)] == 1 (mod m^2)
// if y is 1/x mod m, then y(2-xy) is 1/x mod m^2
// should reduce x and y(2-xy) by m^2 at each step to keep size bounded.
// JS multiply "overflows" differently from C/C++, so care is needed here.
function bnpInvDigit() {
  if(this.t < 1) return 0;
  var x = this.data[0];
  if((x&1) == 0) return 0;
  var y = x&3;		// y == 1/x mod 2^2
  y = (y*(2-(x&0xf)*y))&0xf;	// y == 1/x mod 2^4
  y = (y*(2-(x&0xff)*y))&0xff;	// y == 1/x mod 2^8
  y = (y*(2-(((x&0xffff)*y)&0xffff)))&0xffff;	// y == 1/x mod 2^16
  // last step - calculate inverse mod DV directly;
  // assumes 16 < DB <= 32 and assumes ability to handle 48-bit ints
  y = (y*(2-x*y%this.DV))%this.DV;		// y == 1/x mod 2^dbits
  // we really want the negative inverse, and -DV < y < DV
  return (y>0)?this.DV-y:-y;
}

// Montgomery reduction
function Montgomery(m) {
  this.m = m;
  this.mp = m.invDigit();
  this.mpl = this.mp&0x7fff;
  this.mph = this.mp>>15;
  this.um = (1<<(m.DB-15))-1;
  this.mt2 = 2*m.t;
}

// xR mod m
function montConvert(x) {
  var r = nbi();
  x.abs().dlShiftTo(this.m.t,r);
  r.divRemTo(this.m,null,r);
  if(x.s < 0 && r.compareTo(BigInteger$1.ZERO) > 0) this.m.subTo(r,r);
  return r;
}

// x/R mod m
function montRevert(x) {
  var r = nbi();
  x.copyTo(r);
  this.reduce(r);
  return r;
}

// x = x/R mod m (HAC 14.32)
function montReduce(x) {
  while(x.t <= this.mt2)	// pad x so am has enough room later
    x.data[x.t++] = 0;
  for(var i = 0; i < this.m.t; ++i) {
    // faster way of calculating u0 = x.data[i]*mp mod DV
    var j = x.data[i]&0x7fff;
    var u0 = (j*this.mpl+(((j*this.mph+(x.data[i]>>15)*this.mpl)&this.um)<<15))&x.DM;
    // use am to combine the multiply-shift-add into one call
    j = i+this.m.t;
    x.data[j] += this.m.am(0,u0,x,i,0,this.m.t);
    // propagate carry
    while(x.data[j] >= x.DV) { x.data[j] -= x.DV; x.data[++j]++; }
  }
  x.clamp();
  x.drShiftTo(this.m.t,x);
  if(x.compareTo(this.m) >= 0) x.subTo(this.m,x);
}

// r = "x^2/R mod m"; x != r
function montSqrTo(x,r) { x.squareTo(r); this.reduce(r); }

// r = "xy/R mod m"; x,y != r
function montMulTo(x,y,r) { x.multiplyTo(y,r); this.reduce(r); }

Montgomery.prototype.convert = montConvert;
Montgomery.prototype.revert = montRevert;
Montgomery.prototype.reduce = montReduce;
Montgomery.prototype.mulTo = montMulTo;
Montgomery.prototype.sqrTo = montSqrTo;

// (protected) true iff this is even
function bnpIsEven() { return ((this.t>0)?(this.data[0]&1):this.s) == 0; }

// (protected) this^e, e < 2^32, doing sqr and mul with "r" (HAC 14.79)
function bnpExp(e,z) {
  if(e > 0xffffffff || e < 1) return BigInteger$1.ONE;
  var r = nbi(), r2 = nbi(), g = z.convert(this), i = nbits(e)-1;
  g.copyTo(r);
  while(--i >= 0) {
    z.sqrTo(r,r2);
    if((e&(1<<i)) > 0) z.mulTo(r2,g,r);
    else { var t = r; r = r2; r2 = t; }
  }
  return z.revert(r);
}

// (public) this^e % m, 0 <= e < 2^32
function bnModPowInt(e,m) {
  var z;
  if(e < 256 || m.isEven()) z = new Classic(m); else z = new Montgomery(m);
  return this.exp(e,z);
}

// protected
BigInteger$1.prototype.copyTo = bnpCopyTo;
BigInteger$1.prototype.fromInt = bnpFromInt;
BigInteger$1.prototype.fromString = bnpFromString;
BigInteger$1.prototype.clamp = bnpClamp;
BigInteger$1.prototype.dlShiftTo = bnpDLShiftTo;
BigInteger$1.prototype.drShiftTo = bnpDRShiftTo;
BigInteger$1.prototype.lShiftTo = bnpLShiftTo;
BigInteger$1.prototype.rShiftTo = bnpRShiftTo;
BigInteger$1.prototype.subTo = bnpSubTo;
BigInteger$1.prototype.multiplyTo = bnpMultiplyTo;
BigInteger$1.prototype.squareTo = bnpSquareTo;
BigInteger$1.prototype.divRemTo = bnpDivRemTo;
BigInteger$1.prototype.invDigit = bnpInvDigit;
BigInteger$1.prototype.isEven = bnpIsEven;
BigInteger$1.prototype.exp = bnpExp;

// public
BigInteger$1.prototype.toString = bnToString;
BigInteger$1.prototype.negate = bnNegate;
BigInteger$1.prototype.abs = bnAbs;
BigInteger$1.prototype.compareTo = bnCompareTo;
BigInteger$1.prototype.bitLength = bnBitLength;
BigInteger$1.prototype.mod = bnMod;
BigInteger$1.prototype.modPowInt = bnModPowInt;

// "constants"
BigInteger$1.ZERO = nbv(0);
BigInteger$1.ONE = nbv(1);

// jsbn2 lib

//Copyright (c) 2005-2009  Tom Wu
//All Rights Reserved.
//See "LICENSE" for details (See jsbn.js for LICENSE).

//Extended JavaScript BN functions, required for RSA private ops.

//Version 1.1: new BigInteger("0", 10) returns "proper" zero

//(public)
function bnClone() { var r = nbi(); this.copyTo(r); return r; }

//(public) return value as integer
function bnIntValue() {
if(this.s < 0) {
 if(this.t == 1) return this.data[0]-this.DV;
 else if(this.t == 0) return -1;
} else if(this.t == 1) return this.data[0];
else if(this.t == 0) return 0;
// assumes 16 < DB < 32
return ((this.data[1]&((1<<(32-this.DB))-1))<<this.DB)|this.data[0];
}

//(public) return value as byte
function bnByteValue() { return (this.t==0)?this.s:(this.data[0]<<24)>>24; }

//(public) return value as short (assumes DB>=16)
function bnShortValue() { return (this.t==0)?this.s:(this.data[0]<<16)>>16; }

//(protected) return x s.t. r^x < DV
function bnpChunkSize(r) { return Math.floor(Math.LN2*this.DB/Math.log(r)); }

//(public) 0 if this == 0, 1 if this > 0
function bnSigNum() {
if(this.s < 0) return -1;
else if(this.t <= 0 || (this.t == 1 && this.data[0] <= 0)) return 0;
else return 1;
}

//(protected) convert to radix string
function bnpToRadix(b) {
if(b == null) b = 10;
if(this.signum() == 0 || b < 2 || b > 36) return "0";
var cs = this.chunkSize(b);
var a = Math.pow(b,cs);
var d = nbv(a), y = nbi(), z = nbi(), r = "";
this.divRemTo(d,y,z);
while(y.signum() > 0) {
 r = (a+z.intValue()).toString(b).substr(1) + r;
 y.divRemTo(d,y,z);
}
return z.intValue().toString(b) + r;
}

//(protected) convert from radix string
function bnpFromRadix(s,b) {
this.fromInt(0);
if(b == null) b = 10;
var cs = this.chunkSize(b);
var d = Math.pow(b,cs), mi = false, j = 0, w = 0;
for(var i = 0; i < s.length; ++i) {
 var x = intAt(s,i);
 if(x < 0) {
   if(s.charAt(i) == "-" && this.signum() == 0) mi = true;
   continue;
 }
 w = b*w+x;
 if(++j >= cs) {
   this.dMultiply(d);
   this.dAddOffset(w,0);
   j = 0;
   w = 0;
 }
}
if(j > 0) {
 this.dMultiply(Math.pow(b,j));
 this.dAddOffset(w,0);
}
if(mi) BigInteger$1.ZERO.subTo(this,this);
}

//(protected) alternate constructor
function bnpFromNumber(a,b,c) {
if("number" == typeof b) {
 // new BigInteger(int,int,RNG)
 if(a < 2) this.fromInt(1);
 else {
   this.fromNumber(a,c);
   if(!this.testBit(a-1))  // force MSB set
     this.bitwiseTo(BigInteger$1.ONE.shiftLeft(a-1),op_or,this);
   if(this.isEven()) this.dAddOffset(1,0); // force odd
   while(!this.isProbablePrime(b)) {
     this.dAddOffset(2,0);
     if(this.bitLength() > a) this.subTo(BigInteger$1.ONE.shiftLeft(a-1),this);
   }
 }
} else {
 // new BigInteger(int,RNG)
 var x = new Array(), t = a&7;
 x.length = (a>>3)+1;
 b.nextBytes(x);
 if(t > 0) x[0] &= ((1<<t)-1); else x[0] = 0;
 this.fromString(x,256);
}
}

//(public) convert to bigendian byte array
function bnToByteArray() {
var i = this.t, r = new Array();
r[0] = this.s;
var p = this.DB-(i*this.DB)%8, d, k = 0;
if(i-- > 0) {
 if(p < this.DB && (d = this.data[i]>>p) != (this.s&this.DM)>>p)
   r[k++] = d|(this.s<<(this.DB-p));
 while(i >= 0) {
   if(p < 8) {
     d = (this.data[i]&((1<<p)-1))<<(8-p);
     d |= this.data[--i]>>(p+=this.DB-8);
   } else {
     d = (this.data[i]>>(p-=8))&0xff;
     if(p <= 0) { p += this.DB; --i; }
   }
   if((d&0x80) != 0) d |= -256;
   if(k == 0 && (this.s&0x80) != (d&0x80)) ++k;
   if(k > 0 || d != this.s) r[k++] = d;
 }
}
return r;
}

function bnEquals(a) { return(this.compareTo(a)==0); }
function bnMin(a) { return (this.compareTo(a)<0)?this:a; }
function bnMax(a) { return (this.compareTo(a)>0)?this:a; }

//(protected) r = this op a (bitwise)
function bnpBitwiseTo(a,op,r) {
var i, f, m = Math.min(a.t,this.t);
for(i = 0; i < m; ++i) r.data[i] = op(this.data[i],a.data[i]);
if(a.t < this.t) {
 f = a.s&this.DM;
 for(i = m; i < this.t; ++i) r.data[i] = op(this.data[i],f);
 r.t = this.t;
} else {
 f = this.s&this.DM;
 for(i = m; i < a.t; ++i) r.data[i] = op(f,a.data[i]);
 r.t = a.t;
}
r.s = op(this.s,a.s);
r.clamp();
}

//(public) this & a
function op_and(x,y) { return x&y; }
function bnAnd(a) { var r = nbi(); this.bitwiseTo(a,op_and,r); return r; }

//(public) this | a
function op_or(x,y) { return x|y; }
function bnOr(a) { var r = nbi(); this.bitwiseTo(a,op_or,r); return r; }

//(public) this ^ a
function op_xor(x,y) { return x^y; }
function bnXor(a) { var r = nbi(); this.bitwiseTo(a,op_xor,r); return r; }

//(public) this & ~a
function op_andnot(x,y) { return x&~y; }
function bnAndNot(a) { var r = nbi(); this.bitwiseTo(a,op_andnot,r); return r; }

//(public) ~this
function bnNot() {
var r = nbi();
for(var i = 0; i < this.t; ++i) r.data[i] = this.DM&~this.data[i];
r.t = this.t;
r.s = ~this.s;
return r;
}

//(public) this << n
function bnShiftLeft(n) {
var r = nbi();
if(n < 0) this.rShiftTo(-n,r); else this.lShiftTo(n,r);
return r;
}

//(public) this >> n
function bnShiftRight(n) {
var r = nbi();
if(n < 0) this.lShiftTo(-n,r); else this.rShiftTo(n,r);
return r;
}

//return index of lowest 1-bit in x, x < 2^31
function lbit(x) {
if(x == 0) return -1;
var r = 0;
if((x&0xffff) == 0) { x >>= 16; r += 16; }
if((x&0xff) == 0) { x >>= 8; r += 8; }
if((x&0xf) == 0) { x >>= 4; r += 4; }
if((x&3) == 0) { x >>= 2; r += 2; }
if((x&1) == 0) ++r;
return r;
}

//(public) returns index of lowest 1-bit (or -1 if none)
function bnGetLowestSetBit() {
for(var i = 0; i < this.t; ++i)
 if(this.data[i] != 0) return i*this.DB+lbit(this.data[i]);
if(this.s < 0) return this.t*this.DB;
return -1;
}

//return number of 1 bits in x
function cbit(x) {
var r = 0;
while(x != 0) { x &= x-1; ++r; }
return r;
}

//(public) return number of set bits
function bnBitCount() {
var r = 0, x = this.s&this.DM;
for(var i = 0; i < this.t; ++i) r += cbit(this.data[i]^x);
return r;
}

//(public) true iff nth bit is set
function bnTestBit(n) {
var j = Math.floor(n/this.DB);
if(j >= this.t) return(this.s!=0);
return((this.data[j]&(1<<(n%this.DB)))!=0);
}

//(protected) this op (1<<n)
function bnpChangeBit(n,op) {
var r = BigInteger$1.ONE.shiftLeft(n);
this.bitwiseTo(r,op,r);
return r;
}

//(public) this | (1<<n)
function bnSetBit(n) { return this.changeBit(n,op_or); }

//(public) this & ~(1<<n)
function bnClearBit(n) { return this.changeBit(n,op_andnot); }

//(public) this ^ (1<<n)
function bnFlipBit(n) { return this.changeBit(n,op_xor); }

//(protected) r = this + a
function bnpAddTo(a,r) {
var i = 0, c = 0, m = Math.min(a.t,this.t);
while(i < m) {
 c += this.data[i]+a.data[i];
 r.data[i++] = c&this.DM;
 c >>= this.DB;
}
if(a.t < this.t) {
 c += a.s;
 while(i < this.t) {
   c += this.data[i];
   r.data[i++] = c&this.DM;
   c >>= this.DB;
 }
 c += this.s;
} else {
 c += this.s;
 while(i < a.t) {
   c += a.data[i];
   r.data[i++] = c&this.DM;
   c >>= this.DB;
 }
 c += a.s;
}
r.s = (c<0)?-1:0;
if(c > 0) r.data[i++] = c;
else if(c < -1) r.data[i++] = this.DV+c;
r.t = i;
r.clamp();
}

//(public) this + a
function bnAdd(a) { var r = nbi(); this.addTo(a,r); return r; }

//(public) this - a
function bnSubtract(a) { var r = nbi(); this.subTo(a,r); return r; }

//(public) this * a
function bnMultiply(a) { var r = nbi(); this.multiplyTo(a,r); return r; }

//(public) this / a
function bnDivide(a) { var r = nbi(); this.divRemTo(a,r,null); return r; }

//(public) this % a
function bnRemainder(a) { var r = nbi(); this.divRemTo(a,null,r); return r; }

//(public) [this/a,this%a]
function bnDivideAndRemainder(a) {
var q = nbi(), r = nbi();
this.divRemTo(a,q,r);
return new Array(q,r);
}

//(protected) this *= n, this >= 0, 1 < n < DV
function bnpDMultiply(n) {
this.data[this.t] = this.am(0,n-1,this,0,0,this.t);
++this.t;
this.clamp();
}

//(protected) this += n << w words, this >= 0
function bnpDAddOffset(n,w) {
if(n == 0) return;
while(this.t <= w) this.data[this.t++] = 0;
this.data[w] += n;
while(this.data[w] >= this.DV) {
 this.data[w] -= this.DV;
 if(++w >= this.t) this.data[this.t++] = 0;
 ++this.data[w];
}
}

//A "null" reducer
function NullExp() {}
function nNop(x) { return x; }
function nMulTo(x,y,r) { x.multiplyTo(y,r); }
function nSqrTo(x,r) { x.squareTo(r); }

NullExp.prototype.convert = nNop;
NullExp.prototype.revert = nNop;
NullExp.prototype.mulTo = nMulTo;
NullExp.prototype.sqrTo = nSqrTo;

//(public) this^e
function bnPow(e) { return this.exp(e,new NullExp()); }

//(protected) r = lower n words of "this * a", a.t <= n
//"this" should be the larger one if appropriate.
function bnpMultiplyLowerTo(a,n,r) {
var i = Math.min(this.t+a.t,n);
r.s = 0; // assumes a,this >= 0
r.t = i;
while(i > 0) r.data[--i] = 0;
var j;
for(j = r.t-this.t; i < j; ++i) r.data[i+this.t] = this.am(0,a.data[i],r,i,0,this.t);
for(j = Math.min(a.t,n); i < j; ++i) this.am(0,a.data[i],r,i,0,n-i);
r.clamp();
}

//(protected) r = "this * a" without lower n words, n > 0
//"this" should be the larger one if appropriate.
function bnpMultiplyUpperTo(a,n,r) {
--n;
var i = r.t = this.t+a.t-n;
r.s = 0; // assumes a,this >= 0
while(--i >= 0) r.data[i] = 0;
for(i = Math.max(n-this.t,0); i < a.t; ++i)
 r.data[this.t+i-n] = this.am(n-i,a.data[i],r,0,0,this.t+i-n);
r.clamp();
r.drShiftTo(1,r);
}

//Barrett modular reduction
function Barrett(m) {
// setup Barrett
this.r2 = nbi();
this.q3 = nbi();
BigInteger$1.ONE.dlShiftTo(2*m.t,this.r2);
this.mu = this.r2.divide(m);
this.m = m;
}

function barrettConvert(x) {
if(x.s < 0 || x.t > 2*this.m.t) return x.mod(this.m);
else if(x.compareTo(this.m) < 0) return x;
else { var r = nbi(); x.copyTo(r); this.reduce(r); return r; }
}

function barrettRevert(x) { return x; }

//x = x mod m (HAC 14.42)
function barrettReduce(x) {
x.drShiftTo(this.m.t-1,this.r2);
if(x.t > this.m.t+1) { x.t = this.m.t+1; x.clamp(); }
this.mu.multiplyUpperTo(this.r2,this.m.t+1,this.q3);
this.m.multiplyLowerTo(this.q3,this.m.t+1,this.r2);
while(x.compareTo(this.r2) < 0) x.dAddOffset(1,this.m.t+1);
x.subTo(this.r2,x);
while(x.compareTo(this.m) >= 0) x.subTo(this.m,x);
}

//r = x^2 mod m; x != r
function barrettSqrTo(x,r) { x.squareTo(r); this.reduce(r); }

//r = x*y mod m; x,y != r
function barrettMulTo(x,y,r) { x.multiplyTo(y,r); this.reduce(r); }

Barrett.prototype.convert = barrettConvert;
Barrett.prototype.revert = barrettRevert;
Barrett.prototype.reduce = barrettReduce;
Barrett.prototype.mulTo = barrettMulTo;
Barrett.prototype.sqrTo = barrettSqrTo;

//(public) this^e % m (HAC 14.85)
function bnModPow(e,m) {
var i = e.bitLength(), k, r = nbv(1), z;
if(i <= 0) return r;
else if(i < 18) k = 1;
else if(i < 48) k = 3;
else if(i < 144) k = 4;
else if(i < 768) k = 5;
else k = 6;
if(i < 8)
 z = new Classic(m);
else if(m.isEven())
 z = new Barrett(m);
else
 z = new Montgomery(m);

// precomputation
var g = new Array(), n = 3, k1 = k-1, km = (1<<k)-1;
g[1] = z.convert(this);
if(k > 1) {
 var g2 = nbi();
 z.sqrTo(g[1],g2);
 while(n <= km) {
   g[n] = nbi();
   z.mulTo(g2,g[n-2],g[n]);
   n += 2;
 }
}

var j = e.t-1, w, is1 = true, r2 = nbi(), t;
i = nbits(e.data[j])-1;
while(j >= 0) {
 if(i >= k1) w = (e.data[j]>>(i-k1))&km;
 else {
   w = (e.data[j]&((1<<(i+1))-1))<<(k1-i);
   if(j > 0) w |= e.data[j-1]>>(this.DB+i-k1);
 }

 n = k;
 while((w&1) == 0) { w >>= 1; --n; }
 if((i -= n) < 0) { i += this.DB; --j; }
 if(is1) {  // ret == 1, don't bother squaring or multiplying it
   g[w].copyTo(r);
   is1 = false;
 } else {
   while(n > 1) { z.sqrTo(r,r2); z.sqrTo(r2,r); n -= 2; }
   if(n > 0) z.sqrTo(r,r2); else { t = r; r = r2; r2 = t; }
   z.mulTo(r2,g[w],r);
 }

 while(j >= 0 && (e.data[j]&(1<<i)) == 0) {
   z.sqrTo(r,r2); t = r; r = r2; r2 = t;
   if(--i < 0) { i = this.DB-1; --j; }
 }
}
return z.revert(r);
}

//(public) gcd(this,a) (HAC 14.54)
function bnGCD(a) {
var x = (this.s<0)?this.negate():this.clone();
var y = (a.s<0)?a.negate():a.clone();
if(x.compareTo(y) < 0) { var t = x; x = y; y = t; }
var i = x.getLowestSetBit(), g = y.getLowestSetBit();
if(g < 0) return x;
if(i < g) g = i;
if(g > 0) {
 x.rShiftTo(g,x);
 y.rShiftTo(g,y);
}
while(x.signum() > 0) {
 if((i = x.getLowestSetBit()) > 0) x.rShiftTo(i,x);
 if((i = y.getLowestSetBit()) > 0) y.rShiftTo(i,y);
 if(x.compareTo(y) >= 0) {
   x.subTo(y,x);
   x.rShiftTo(1,x);
 } else {
   y.subTo(x,y);
   y.rShiftTo(1,y);
 }
}
if(g > 0) y.lShiftTo(g,y);
return y;
}

//(protected) this % n, n < 2^26
function bnpModInt(n) {
if(n <= 0) return 0;
var d = this.DV%n, r = (this.s<0)?n-1:0;
if(this.t > 0)
 if(d == 0) r = this.data[0]%n;
 else for(var i = this.t-1; i >= 0; --i) r = (d*r+this.data[i])%n;
return r;
}

//(public) 1/this % m (HAC 14.61)
function bnModInverse(m) {
var ac = m.isEven();
if((this.isEven() && ac) || m.signum() == 0) return BigInteger$1.ZERO;
var u = m.clone(), v = this.clone();
var a = nbv(1), b = nbv(0), c = nbv(0), d = nbv(1);
while(u.signum() != 0) {
 while(u.isEven()) {
   u.rShiftTo(1,u);
   if(ac) {
     if(!a.isEven() || !b.isEven()) { a.addTo(this,a); b.subTo(m,b); }
     a.rShiftTo(1,a);
   } else if(!b.isEven()) b.subTo(m,b);
   b.rShiftTo(1,b);
 }
 while(v.isEven()) {
   v.rShiftTo(1,v);
   if(ac) {
     if(!c.isEven() || !d.isEven()) { c.addTo(this,c); d.subTo(m,d); }
     c.rShiftTo(1,c);
   } else if(!d.isEven()) d.subTo(m,d);
   d.rShiftTo(1,d);
 }
 if(u.compareTo(v) >= 0) {
   u.subTo(v,u);
   if(ac) a.subTo(c,a);
   b.subTo(d,b);
 } else {
   v.subTo(u,v);
   if(ac) c.subTo(a,c);
   d.subTo(b,d);
 }
}
if(v.compareTo(BigInteger$1.ONE) != 0) return BigInteger$1.ZERO;
if(d.compareTo(m) >= 0) return d.subtract(m);
if(d.signum() < 0) d.addTo(m,d); else return d;
if(d.signum() < 0) return d.add(m); else return d;
}

var lowprimes = [2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,479,487,491,499,503,509];
var lplim = (1<<26)/lowprimes[lowprimes.length-1];

//(public) test primality with certainty >= 1-.5^t
function bnIsProbablePrime(t) {
var i, x = this.abs();
if(x.t == 1 && x.data[0] <= lowprimes[lowprimes.length-1]) {
 for(i = 0; i < lowprimes.length; ++i)
   if(x.data[0] == lowprimes[i]) return true;
 return false;
}
if(x.isEven()) return false;
i = 1;
while(i < lowprimes.length) {
 var m = lowprimes[i], j = i+1;
 while(j < lowprimes.length && m < lplim) m *= lowprimes[j++];
 m = x.modInt(m);
 while(i < j) if(m%lowprimes[i++] == 0) return false;
}
return x.millerRabin(t);
}

//(protected) true if probably prime (HAC 4.24, Miller-Rabin)
function bnpMillerRabin(t) {
var n1 = this.subtract(BigInteger$1.ONE);
var k = n1.getLowestSetBit();
if(k <= 0) return false;
var r = n1.shiftRight(k);
var prng = bnGetPrng();
var a;
for(var i = 0; i < t; ++i) {
 // select witness 'a' at random from between 1 and n1
 do {
   a = new BigInteger$1(this.bitLength(), prng);
 }
 while(a.compareTo(BigInteger$1.ONE) <= 0 || a.compareTo(n1) >= 0);
 var y = a.modPow(r,this);
 if(y.compareTo(BigInteger$1.ONE) != 0 && y.compareTo(n1) != 0) {
   var j = 1;
   while(j++ < k && y.compareTo(n1) != 0) {
     y = y.modPowInt(2,this);
     if(y.compareTo(BigInteger$1.ONE) == 0) return false;
   }
   if(y.compareTo(n1) != 0) return false;
 }
}
return true;
}

// get pseudo random number generator
function bnGetPrng() {
  // create prng with api that matches BigInteger secure random
  return {
    // x is an array to fill with bytes
    nextBytes: function(x) {
      for(var i = 0; i < x.length; ++i) {
        x[i] = Math.floor(Math.random() * 0x0100);
      }
    }
  };
}

//protected
BigInteger$1.prototype.chunkSize = bnpChunkSize;
BigInteger$1.prototype.toRadix = bnpToRadix;
BigInteger$1.prototype.fromRadix = bnpFromRadix;
BigInteger$1.prototype.fromNumber = bnpFromNumber;
BigInteger$1.prototype.bitwiseTo = bnpBitwiseTo;
BigInteger$1.prototype.changeBit = bnpChangeBit;
BigInteger$1.prototype.addTo = bnpAddTo;
BigInteger$1.prototype.dMultiply = bnpDMultiply;
BigInteger$1.prototype.dAddOffset = bnpDAddOffset;
BigInteger$1.prototype.multiplyLowerTo = bnpMultiplyLowerTo;
BigInteger$1.prototype.multiplyUpperTo = bnpMultiplyUpperTo;
BigInteger$1.prototype.modInt = bnpModInt;
BigInteger$1.prototype.millerRabin = bnpMillerRabin;

//public
BigInteger$1.prototype.clone = bnClone;
BigInteger$1.prototype.intValue = bnIntValue;
BigInteger$1.prototype.byteValue = bnByteValue;
BigInteger$1.prototype.shortValue = bnShortValue;
BigInteger$1.prototype.signum = bnSigNum;
BigInteger$1.prototype.toByteArray = bnToByteArray;
BigInteger$1.prototype.equals = bnEquals;
BigInteger$1.prototype.min = bnMin;
BigInteger$1.prototype.max = bnMax;
BigInteger$1.prototype.and = bnAnd;
BigInteger$1.prototype.or = bnOr;
BigInteger$1.prototype.xor = bnXor;
BigInteger$1.prototype.andNot = bnAndNot;
BigInteger$1.prototype.not = bnNot;
BigInteger$1.prototype.shiftLeft = bnShiftLeft;
BigInteger$1.prototype.shiftRight = bnShiftRight;
BigInteger$1.prototype.getLowestSetBit = bnGetLowestSetBit;
BigInteger$1.prototype.bitCount = bnBitCount;
BigInteger$1.prototype.testBit = bnTestBit;
BigInteger$1.prototype.setBit = bnSetBit;
BigInteger$1.prototype.clearBit = bnClearBit;
BigInteger$1.prototype.flipBit = bnFlipBit;
BigInteger$1.prototype.add = bnAdd;
BigInteger$1.prototype.subtract = bnSubtract;
BigInteger$1.prototype.multiply = bnMultiply;
BigInteger$1.prototype.divide = bnDivide;
BigInteger$1.prototype.remainder = bnRemainder;
BigInteger$1.prototype.divideAndRemainder = bnDivideAndRemainder;
BigInteger$1.prototype.modPow = bnModPow;
BigInteger$1.prototype.modInverse = bnModInverse;
BigInteger$1.prototype.pow = bnPow;
BigInteger$1.prototype.gcd = bnGCD;
BigInteger$1.prototype.isProbablePrime = bnIsProbablePrime;

/**
 * Secure Hash Algorithm with 160-bit digest (SHA-1) implementation.
 *
 * @author Dave Longley
 *
 * Copyright (c) 2010-2015 Digital Bazaar, Inc.
 */

var forge$b = forge$s;



var sha1 = forge$b.sha1 = forge$b.sha1 || {};
forge$b.md.sha1 = forge$b.md.algorithms.sha1 = sha1;

/**
 * Creates a SHA-1 message digest object.
 *
 * @return a message digest object.
 */
sha1.create = function() {
  // do initialization as necessary
  if(!_initialized) {
    _init();
  }

  // SHA-1 state contains five 32-bit integers
  var _state = null;

  // input buffer
  var _input = forge$b.util.createBuffer();

  // used for word storage
  var _w = new Array(80);

  // message digest object
  var md = {
    algorithm: 'sha1',
    blockLength: 64,
    digestLength: 20,
    // 56-bit length of message so far (does not including padding)
    messageLength: 0,
    // true message length
    fullMessageLength: null,
    // size of message length in bytes
    messageLengthSize: 8
  };

  /**
   * Starts the digest.
   *
   * @return this digest object.
   */
  md.start = function() {
    // up to 56-bit message length for convenience
    md.messageLength = 0;

    // full message length (set md.messageLength64 for backwards-compatibility)
    md.fullMessageLength = md.messageLength64 = [];
    var int32s = md.messageLengthSize / 4;
    for(var i = 0; i < int32s; ++i) {
      md.fullMessageLength.push(0);
    }
    _input = forge$b.util.createBuffer();
    _state = {
      h0: 0x67452301,
      h1: 0xEFCDAB89,
      h2: 0x98BADCFE,
      h3: 0x10325476,
      h4: 0xC3D2E1F0
    };
    return md;
  };
  // start digest automatically for first time
  md.start();

  /**
   * Updates the digest with the given message input. The given input can
   * treated as raw input (no encoding will be applied) or an encoding of
   * 'utf8' maybe given to encode the input using UTF-8.
   *
   * @param msg the message input to update with.
   * @param encoding the encoding to use (default: 'raw', other: 'utf8').
   *
   * @return this digest object.
   */
  md.update = function(msg, encoding) {
    if(encoding === 'utf8') {
      msg = forge$b.util.encodeUtf8(msg);
    }

    // update message length
    var len = msg.length;
    md.messageLength += len;
    len = [(len / 0x100000000) >>> 0, len >>> 0];
    for(var i = md.fullMessageLength.length - 1; i >= 0; --i) {
      md.fullMessageLength[i] += len[1];
      len[1] = len[0] + ((md.fullMessageLength[i] / 0x100000000) >>> 0);
      md.fullMessageLength[i] = md.fullMessageLength[i] >>> 0;
      len[0] = ((len[1] / 0x100000000) >>> 0);
    }

    // add bytes to input buffer
    _input.putBytes(msg);

    // process bytes
    _update(_state, _w, _input);

    // compact input buffer every 2K or if empty
    if(_input.read > 2048 || _input.length() === 0) {
      _input.compact();
    }

    return md;
  };

  /**
   * Produces the digest.
   *
   * @return a byte buffer containing the digest value.
   */
  md.digest = function() {
    /* Note: Here we copy the remaining bytes in the input buffer and
    add the appropriate SHA-1 padding. Then we do the final update
    on a copy of the state so that if the user wants to get
    intermediate digests they can do so. */

    /* Determine the number of bytes that must be added to the message
    to ensure its length is congruent to 448 mod 512. In other words,
    the data to be digested must be a multiple of 512 bits (or 128 bytes).
    This data includes the message, some padding, and the length of the
    message. Since the length of the message will be encoded as 8 bytes (64
    bits), that means that the last segment of the data must have 56 bytes
    (448 bits) of message and padding. Therefore, the length of the message
    plus the padding must be congruent to 448 mod 512 because
    512 - 128 = 448.

    In order to fill up the message length it must be filled with
    padding that begins with 1 bit followed by all 0 bits. Padding
    must *always* be present, so if the message length is already
    congruent to 448 mod 512, then 512 padding bits must be added. */

    var finalBlock = forge$b.util.createBuffer();
    finalBlock.putBytes(_input.bytes());

    // compute remaining size to be digested (include message length size)
    var remaining = (
      md.fullMessageLength[md.fullMessageLength.length - 1] +
      md.messageLengthSize);

    // add padding for overflow blockSize - overflow
    // _padding starts with 1 byte with first bit is set (byte value 128), then
    // there may be up to (blockSize - 1) other pad bytes
    var overflow = remaining & (md.blockLength - 1);
    finalBlock.putBytes(_padding.substr(0, md.blockLength - overflow));

    // serialize message length in bits in big-endian order; since length
    // is stored in bytes we multiply by 8 and add carry from next int
    var next, carry;
    var bits = md.fullMessageLength[0] * 8;
    for(var i = 0; i < md.fullMessageLength.length - 1; ++i) {
      next = md.fullMessageLength[i + 1] * 8;
      carry = (next / 0x100000000) >>> 0;
      bits += carry;
      finalBlock.putInt32(bits >>> 0);
      bits = next >>> 0;
    }
    finalBlock.putInt32(bits);

    var s2 = {
      h0: _state.h0,
      h1: _state.h1,
      h2: _state.h2,
      h3: _state.h3,
      h4: _state.h4
    };
    _update(s2, _w, finalBlock);
    var rval = forge$b.util.createBuffer();
    rval.putInt32(s2.h0);
    rval.putInt32(s2.h1);
    rval.putInt32(s2.h2);
    rval.putInt32(s2.h3);
    rval.putInt32(s2.h4);
    return rval;
  };

  return md;
};

// sha-1 padding bytes not initialized yet
var _padding = null;
var _initialized = false;

/**
 * Initializes the constant tables.
 */
function _init() {
  // create padding
  _padding = String.fromCharCode(128);
  _padding += forge$b.util.fillString(String.fromCharCode(0x00), 64);

  // now initialized
  _initialized = true;
}

/**
 * Updates a SHA-1 state with the given byte buffer.
 *
 * @param s the SHA-1 state to update.
 * @param w the array to use to store words.
 * @param bytes the byte buffer to update with.
 */
function _update(s, w, bytes) {
  // consume 512 bit (64 byte) chunks
  var t, a, b, c, d, e, f, i;
  var len = bytes.length();
  while(len >= 64) {
    // the w array will be populated with sixteen 32-bit big-endian words
    // and then extended into 80 32-bit words according to SHA-1 algorithm
    // and for 32-79 using Max Locktyukhin's optimization

    // initialize hash value for this chunk
    a = s.h0;
    b = s.h1;
    c = s.h2;
    d = s.h3;
    e = s.h4;

    // round 1
    for(i = 0; i < 16; ++i) {
      t = bytes.getInt32();
      w[i] = t;
      f = d ^ (b & (c ^ d));
      t = ((a << 5) | (a >>> 27)) + f + e + 0x5A827999 + t;
      e = d;
      d = c;
      // `>>> 0` necessary to avoid iOS/Safari 10 optimization bug
      c = ((b << 30) | (b >>> 2)) >>> 0;
      b = a;
      a = t;
    }
    for(; i < 20; ++i) {
      t = (w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16]);
      t = (t << 1) | (t >>> 31);
      w[i] = t;
      f = d ^ (b & (c ^ d));
      t = ((a << 5) | (a >>> 27)) + f + e + 0x5A827999 + t;
      e = d;
      d = c;
      // `>>> 0` necessary to avoid iOS/Safari 10 optimization bug
      c = ((b << 30) | (b >>> 2)) >>> 0;
      b = a;
      a = t;
    }
    // round 2
    for(; i < 32; ++i) {
      t = (w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16]);
      t = (t << 1) | (t >>> 31);
      w[i] = t;
      f = b ^ c ^ d;
      t = ((a << 5) | (a >>> 27)) + f + e + 0x6ED9EBA1 + t;
      e = d;
      d = c;
      // `>>> 0` necessary to avoid iOS/Safari 10 optimization bug
      c = ((b << 30) | (b >>> 2)) >>> 0;
      b = a;
      a = t;
    }
    for(; i < 40; ++i) {
      t = (w[i - 6] ^ w[i - 16] ^ w[i - 28] ^ w[i - 32]);
      t = (t << 2) | (t >>> 30);
      w[i] = t;
      f = b ^ c ^ d;
      t = ((a << 5) | (a >>> 27)) + f + e + 0x6ED9EBA1 + t;
      e = d;
      d = c;
      // `>>> 0` necessary to avoid iOS/Safari 10 optimization bug
      c = ((b << 30) | (b >>> 2)) >>> 0;
      b = a;
      a = t;
    }
    // round 3
    for(; i < 60; ++i) {
      t = (w[i - 6] ^ w[i - 16] ^ w[i - 28] ^ w[i - 32]);
      t = (t << 2) | (t >>> 30);
      w[i] = t;
      f = (b & c) | (d & (b ^ c));
      t = ((a << 5) | (a >>> 27)) + f + e + 0x8F1BBCDC + t;
      e = d;
      d = c;
      // `>>> 0` necessary to avoid iOS/Safari 10 optimization bug
      c = ((b << 30) | (b >>> 2)) >>> 0;
      b = a;
      a = t;
    }
    // round 4
    for(; i < 80; ++i) {
      t = (w[i - 6] ^ w[i - 16] ^ w[i - 28] ^ w[i - 32]);
      t = (t << 2) | (t >>> 30);
      w[i] = t;
      f = b ^ c ^ d;
      t = ((a << 5) | (a >>> 27)) + f + e + 0xCA62C1D6 + t;
      e = d;
      d = c;
      // `>>> 0` necessary to avoid iOS/Safari 10 optimization bug
      c = ((b << 30) | (b >>> 2)) >>> 0;
      b = a;
      a = t;
    }

    // update hash state
    s.h0 = (s.h0 + a) | 0;
    s.h1 = (s.h1 + b) | 0;
    s.h2 = (s.h2 + c) | 0;
    s.h3 = (s.h3 + d) | 0;
    s.h4 = (s.h4 + e) | 0;

    len -= 64;
  }
}

/**
 * Partial implementation of PKCS#1 v2.2: RSA-OEAP
 *
 * Modified but based on the following MIT and BSD licensed code:
 *
 * https://github.com/kjur/jsjws/blob/master/rsa.js:
 *
 * The 'jsjws'(JSON Web Signature JavaScript Library) License
 *
 * Copyright (c) 2012 Kenji Urushima
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * http://webrsa.cvs.sourceforge.net/viewvc/webrsa/Client/RSAES-OAEP.js?content-type=text%2Fplain:
 *
 * RSAES-OAEP.js
 * $Id: RSAES-OAEP.js,v 1.1.1.1 2003/03/19 15:37:20 ellispritchard Exp $
 * JavaScript Implementation of PKCS #1 v2.1 RSA CRYPTOGRAPHY STANDARD (RSA Laboratories, June 14, 2002)
 * Copyright (C) Ellis Pritchard, Guardian Unlimited 2003.
 * Contact: ellis@nukinetics.com
 * Distributed under the BSD License.
 *
 * Official documentation: http://www.rsa.com/rsalabs/node.asp?id=2125
 *
 * @author Evan Jones (http://evanjones.ca/)
 * @author Dave Longley
 *
 * Copyright (c) 2013-2014 Digital Bazaar, Inc.
 */

var forge$a = forge$s;




// shortcut for PKCS#1 API
var pkcs1 = forge$a.pkcs1 = forge$a.pkcs1 || {};

/**
 * Encode the given RSAES-OAEP message (M) using key, with optional label (L)
 * and seed.
 *
 * This method does not perform RSA encryption, it only encodes the message
 * using RSAES-OAEP.
 *
 * @param key the RSA key to use.
 * @param message the message to encode.
 * @param options the options to use:
 *          label an optional label to use.
 *          seed the seed to use.
 *          md the message digest object to use, undefined for SHA-1.
 *          mgf1 optional mgf1 parameters:
 *            md the message digest object to use for MGF1.
 *
 * @return the encoded message bytes.
 */
pkcs1.encode_rsa_oaep = function(key, message, options) {
  // parse arguments
  var label;
  var seed;
  var md;
  var mgf1Md;
  // legacy args (label, seed, md)
  if(typeof options === 'string') {
    label = options;
    seed = arguments[3] || undefined;
    md = arguments[4] || undefined;
  } else if(options) {
    label = options.label || undefined;
    seed = options.seed || undefined;
    md = options.md || undefined;
    if(options.mgf1 && options.mgf1.md) {
      mgf1Md = options.mgf1.md;
    }
  }

  // default OAEP to SHA-1 message digest
  if(!md) {
    md = forge$a.md.sha1.create();
  } else {
    md.start();
  }

  // default MGF-1 to same as OAEP
  if(!mgf1Md) {
    mgf1Md = md;
  }

  // compute length in bytes and check output
  var keyLength = Math.ceil(key.n.bitLength() / 8);
  var maxLength = keyLength - 2 * md.digestLength - 2;
  if(message.length > maxLength) {
    var error = new Error('RSAES-OAEP input message length is too long.');
    error.length = message.length;
    error.maxLength = maxLength;
    throw error;
  }

  if(!label) {
    label = '';
  }
  md.update(label, 'raw');
  var lHash = md.digest();

  var PS = '';
  var PS_length = maxLength - message.length;
  for(var i = 0; i < PS_length; i++) {
    PS += '\x00';
  }

  var DB = lHash.getBytes() + PS + '\x01' + message;

  if(!seed) {
    seed = forge$a.random.getBytes(md.digestLength);
  } else if(seed.length !== md.digestLength) {
    var error = new Error('Invalid RSAES-OAEP seed. The seed length must ' +
      'match the digest length.');
    error.seedLength = seed.length;
    error.digestLength = md.digestLength;
    throw error;
  }

  var dbMask = rsa_mgf1(seed, keyLength - md.digestLength - 1, mgf1Md);
  var maskedDB = forge$a.util.xorBytes(DB, dbMask, DB.length);

  var seedMask = rsa_mgf1(maskedDB, md.digestLength, mgf1Md);
  var maskedSeed = forge$a.util.xorBytes(seed, seedMask, seed.length);

  // return encoded message
  return '\x00' + maskedSeed + maskedDB;
};

/**
 * Decode the given RSAES-OAEP encoded message (EM) using key, with optional
 * label (L).
 *
 * This method does not perform RSA decryption, it only decodes the message
 * using RSAES-OAEP.
 *
 * @param key the RSA key to use.
 * @param em the encoded message to decode.
 * @param options the options to use:
 *          label an optional label to use.
 *          md the message digest object to use for OAEP, undefined for SHA-1.
 *          mgf1 optional mgf1 parameters:
 *            md the message digest object to use for MGF1.
 *
 * @return the decoded message bytes.
 */
pkcs1.decode_rsa_oaep = function(key, em, options) {
  // parse args
  var label;
  var md;
  var mgf1Md;
  // legacy args
  if(typeof options === 'string') {
    label = options;
    md = arguments[3] || undefined;
  } else if(options) {
    label = options.label || undefined;
    md = options.md || undefined;
    if(options.mgf1 && options.mgf1.md) {
      mgf1Md = options.mgf1.md;
    }
  }

  // compute length in bytes
  var keyLength = Math.ceil(key.n.bitLength() / 8);

  if(em.length !== keyLength) {
    var error = new Error('RSAES-OAEP encoded message length is invalid.');
    error.length = em.length;
    error.expectedLength = keyLength;
    throw error;
  }

  // default OAEP to SHA-1 message digest
  if(md === undefined) {
    md = forge$a.md.sha1.create();
  } else {
    md.start();
  }

  // default MGF-1 to same as OAEP
  if(!mgf1Md) {
    mgf1Md = md;
  }

  if(keyLength < 2 * md.digestLength + 2) {
    throw new Error('RSAES-OAEP key is too short for the hash function.');
  }

  if(!label) {
    label = '';
  }
  md.update(label, 'raw');
  var lHash = md.digest().getBytes();

  // split the message into its parts
  var y = em.charAt(0);
  var maskedSeed = em.substring(1, md.digestLength + 1);
  var maskedDB = em.substring(1 + md.digestLength);

  var seedMask = rsa_mgf1(maskedDB, md.digestLength, mgf1Md);
  var seed = forge$a.util.xorBytes(maskedSeed, seedMask, maskedSeed.length);

  var dbMask = rsa_mgf1(seed, keyLength - md.digestLength - 1, mgf1Md);
  var db = forge$a.util.xorBytes(maskedDB, dbMask, maskedDB.length);

  var lHashPrime = db.substring(0, md.digestLength);

  // constant time check that all values match what is expected
  var error = (y !== '\x00');

  // constant time check lHash vs lHashPrime
  for(var i = 0; i < md.digestLength; ++i) {
    error |= (lHash.charAt(i) !== lHashPrime.charAt(i));
  }

  // "constant time" find the 0x1 byte separating the padding (zeros) from the
  // message
  // TODO: It must be possible to do this in a better/smarter way?
  var in_ps = 1;
  var index = md.digestLength;
  for(var j = md.digestLength; j < db.length; j++) {
    var code = db.charCodeAt(j);

    var is_0 = (code & 0x1) ^ 0x1;

    // non-zero if not 0 or 1 in the ps section
    var error_mask = in_ps ? 0xfffe : 0x0000;
    error |= (code & error_mask);

    // latch in_ps to zero after we find 0x1
    in_ps = in_ps & is_0;
    index += in_ps;
  }

  if(error || db.charCodeAt(index) !== 0x1) {
    throw new Error('Invalid RSAES-OAEP padding.');
  }

  return db.substring(index + 1);
};

function rsa_mgf1(seed, maskLength, hash) {
  // default to SHA-1 message digest
  if(!hash) {
    hash = forge$a.md.sha1.create();
  }
  var t = '';
  var count = Math.ceil(maskLength / hash.digestLength);
  for(var i = 0; i < count; ++i) {
    var c = String.fromCharCode(
      (i >> 24) & 0xFF, (i >> 16) & 0xFF, (i >> 8) & 0xFF, i & 0xFF);
    hash.start();
    hash.update(seed + c);
    t += hash.digest().getBytes();
  }
  return t.substring(0, maskLength);
}

/**
 * Prime number generation API.
 *
 * @author Dave Longley
 *
 * Copyright (c) 2014 Digital Bazaar, Inc.
 */

var forge$9 = forge$s;




(function() {

// forge.prime already defined
if(forge$9.prime) {
  return;
}

/* PRIME API */
var prime = forge$9.prime = forge$9.prime || {};

var BigInteger = forge$9.jsbn.BigInteger;

// primes are 30k+i for i = 1, 7, 11, 13, 17, 19, 23, 29
var GCD_30_DELTA = [6, 4, 2, 4, 2, 4, 6, 2];
var THIRTY = new BigInteger(null);
THIRTY.fromInt(30);
var op_or = function(x, y) {return x|y;};

/**
 * Generates a random probable prime with the given number of bits.
 *
 * Alternative algorithms can be specified by name as a string or as an
 * object with custom options like so:
 *
 * {
 *   name: 'PRIMEINC',
 *   options: {
 *     maxBlockTime: <the maximum amount of time to block the main
 *       thread before allowing I/O other JS to run>,
 *     millerRabinTests: <the number of miller-rabin tests to run>,
 *     workerScript: <the worker script URL>,
 *     workers: <the number of web workers (if supported) to use,
 *       -1 to use estimated cores minus one>.
 *     workLoad: the size of the work load, ie: number of possible prime
 *       numbers for each web worker to check per work assignment,
 *       (default: 100).
 *   }
 * }
 *
 * @param bits the number of bits for the prime number.
 * @param options the options to use.
 *          [algorithm] the algorithm to use (default: 'PRIMEINC').
 *          [prng] a custom crypto-secure pseudo-random number generator to use,
 *            that must define "getBytesSync".
 *
 * @return callback(err, num) called once the operation completes.
 */
prime.generateProbablePrime = function(bits, options, callback) {
  if(typeof options === 'function') {
    callback = options;
    options = {};
  }
  options = options || {};

  // default to PRIMEINC algorithm
  var algorithm = options.algorithm || 'PRIMEINC';
  if(typeof algorithm === 'string') {
    algorithm = {name: algorithm};
  }
  algorithm.options = algorithm.options || {};

  // create prng with api that matches BigInteger secure random
  var prng = options.prng || forge$9.random;
  var rng = {
    // x is an array to fill with bytes
    nextBytes: function(x) {
      var b = prng.getBytesSync(x.length);
      for(var i = 0; i < x.length; ++i) {
        x[i] = b.charCodeAt(i);
      }
    }
  };

  if(algorithm.name === 'PRIMEINC') {
    return primeincFindPrime(bits, rng, algorithm.options, callback);
  }

  throw new Error('Invalid prime generation algorithm: ' + algorithm.name);
};

function primeincFindPrime(bits, rng, options, callback) {
  if('workers' in options) {
    return primeincFindPrimeWithWorkers(bits, rng, options, callback);
  }
  return primeincFindPrimeWithoutWorkers(bits, rng, options, callback);
}

function primeincFindPrimeWithoutWorkers(bits, rng, options, callback) {
  // initialize random number
  var num = generateRandom(bits, rng);

  /* Note: All primes are of the form 30k+i for i < 30 and gcd(30, i)=1. The
  number we are given is always aligned at 30k + 1. Each time the number is
  determined not to be prime we add to get to the next 'i', eg: if the number
  was at 30k + 1 we add 6. */
  var deltaIdx = 0;

  // get required number of MR tests
  var mrTests = getMillerRabinTests(num.bitLength());
  if('millerRabinTests' in options) {
    mrTests = options.millerRabinTests;
  }

  // find prime nearest to 'num' for maxBlockTime ms
  // 10 ms gives 5ms of leeway for other calculations before dropping
  // below 60fps (1000/60 == 16.67), but in reality, the number will
  // likely be higher due to an 'atomic' big int modPow
  var maxBlockTime = 10;
  if('maxBlockTime' in options) {
    maxBlockTime = options.maxBlockTime;
  }

  _primeinc(num, bits, rng, deltaIdx, mrTests, maxBlockTime, callback);
}

function _primeinc(num, bits, rng, deltaIdx, mrTests, maxBlockTime, callback) {
  var start = +new Date();
  do {
    // overflow, regenerate random number
    if(num.bitLength() > bits) {
      num = generateRandom(bits, rng);
    }
    // do primality test
    if(num.isProbablePrime(mrTests)) {
      return callback(null, num);
    }
    // get next potential prime
    num.dAddOffset(GCD_30_DELTA[deltaIdx++ % 8], 0);
  } while(maxBlockTime < 0 || (+new Date() - start < maxBlockTime));

  // keep trying later
  forge$9.util.setImmediate(function() {
    _primeinc(num, bits, rng, deltaIdx, mrTests, maxBlockTime, callback);
  });
}

// NOTE: This algorithm is indeterminate in nature because workers
// run in parallel looking at different segments of numbers. Even if this
// algorithm is run twice with the same input from a predictable RNG, it
// may produce different outputs.
function primeincFindPrimeWithWorkers(bits, rng, options, callback) {
  // web workers unavailable
  if(typeof Worker === 'undefined') {
    return primeincFindPrimeWithoutWorkers(bits, rng, options, callback);
  }

  // initialize random number
  var num = generateRandom(bits, rng);

  // use web workers to generate keys
  var numWorkers = options.workers;
  var workLoad = options.workLoad || 100;
  var range = workLoad * 30 / 8;
  var workerScript = options.workerScript || 'forge/prime.worker.js';
  if(numWorkers === -1) {
    return forge$9.util.estimateCores(function(err, cores) {
      if(err) {
        // default to 2
        cores = 2;
      }
      numWorkers = cores - 1;
      generate();
    });
  }
  generate();

  function generate() {
    // require at least 1 worker
    numWorkers = Math.max(1, numWorkers);

    // TODO: consider optimizing by starting workers outside getPrime() ...
    // note that in order to clean up they will have to be made internally
    // asynchronous which may actually be slower

    // start workers immediately
    var workers = [];
    for(var i = 0; i < numWorkers; ++i) {
      // FIXME: fix path or use blob URLs
      workers[i] = new Worker(workerScript);
    }

    // listen for requests from workers and assign ranges to find prime
    for(var i = 0; i < numWorkers; ++i) {
      workers[i].addEventListener('message', workerMessage);
    }

    /* Note: The distribution of random numbers is unknown. Therefore, each
    web worker is continuously allocated a range of numbers to check for a
    random number until one is found.

    Every 30 numbers will be checked just 8 times, because prime numbers
    have the form:

    30k+i, for i < 30 and gcd(30, i)=1 (there are 8 values of i for this)

    Therefore, if we want a web worker to run N checks before asking for
    a new range of numbers, each range must contain N*30/8 numbers.

    For 100 checks (workLoad), this is a range of 375. */

    var found = false;
    function workerMessage(e) {
      // ignore message, prime already found
      if(found) {
        return;
      }
      var data = e.data;
      if(data.found) {
        // terminate all workers
        for(var i = 0; i < workers.length; ++i) {
          workers[i].terminate();
        }
        found = true;
        return callback(null, new BigInteger(data.prime, 16));
      }

      // overflow, regenerate random number
      if(num.bitLength() > bits) {
        num = generateRandom(bits, rng);
      }

      // assign new range to check
      var hex = num.toString(16);

      // start prime search
      e.target.postMessage({
        hex: hex,
        workLoad: workLoad
      });

      num.dAddOffset(range, 0);
    }
  }
}

/**
 * Generates a random number using the given number of bits and RNG.
 *
 * @param bits the number of bits for the number.
 * @param rng the random number generator to use.
 *
 * @return the random number.
 */
function generateRandom(bits, rng) {
  var num = new BigInteger(bits, rng);
  // force MSB set
  var bits1 = bits - 1;
  if(!num.testBit(bits1)) {
    num.bitwiseTo(BigInteger.ONE.shiftLeft(bits1), op_or, num);
  }
  // align number on 30k+1 boundary
  num.dAddOffset(31 - num.mod(THIRTY).byteValue(), 0);
  return num;
}

/**
 * Returns the required number of Miller-Rabin tests to generate a
 * prime with an error probability of (1/2)^80.
 *
 * See Handbook of Applied Cryptography Chapter 4, Table 4.4.
 *
 * @param bits the bit size.
 *
 * @return the required number of iterations.
 */
function getMillerRabinTests(bits) {
  if(bits <= 100) return 27;
  if(bits <= 150) return 18;
  if(bits <= 200) return 15;
  if(bits <= 250) return 12;
  if(bits <= 300) return 9;
  if(bits <= 350) return 8;
  if(bits <= 400) return 7;
  if(bits <= 500) return 6;
  if(bits <= 600) return 5;
  if(bits <= 800) return 4;
  if(bits <= 1250) return 3;
  return 2;
}

})();

/**
 * Javascript implementation of basic RSA algorithms.
 *
 * @author Dave Longley
 *
 * Copyright (c) 2010-2014 Digital Bazaar, Inc.
 *
 * The only algorithm currently supported for PKI is RSA.
 *
 * An RSA key is often stored in ASN.1 DER format. The SubjectPublicKeyInfo
 * ASN.1 structure is composed of an algorithm of type AlgorithmIdentifier
 * and a subjectPublicKey of type bit string.
 *
 * The AlgorithmIdentifier contains an Object Identifier (OID) and parameters
 * for the algorithm, if any. In the case of RSA, there aren't any.
 *
 * SubjectPublicKeyInfo ::= SEQUENCE {
 *   algorithm AlgorithmIdentifier,
 *   subjectPublicKey BIT STRING
 * }
 *
 * AlgorithmIdentifer ::= SEQUENCE {
 *   algorithm OBJECT IDENTIFIER,
 *   parameters ANY DEFINED BY algorithm OPTIONAL
 * }
 *
 * For an RSA public key, the subjectPublicKey is:
 *
 * RSAPublicKey ::= SEQUENCE {
 *   modulus            INTEGER,    -- n
 *   publicExponent     INTEGER     -- e
 * }
 *
 * PrivateKeyInfo ::= SEQUENCE {
 *   version                   Version,
 *   privateKeyAlgorithm       PrivateKeyAlgorithmIdentifier,
 *   privateKey                PrivateKey,
 *   attributes           [0]  IMPLICIT Attributes OPTIONAL
 * }
 *
 * Version ::= INTEGER
 * PrivateKeyAlgorithmIdentifier ::= AlgorithmIdentifier
 * PrivateKey ::= OCTET STRING
 * Attributes ::= SET OF Attribute
 *
 * An RSA private key as the following structure:
 *
 * RSAPrivateKey ::= SEQUENCE {
 *   version Version,
 *   modulus INTEGER, -- n
 *   publicExponent INTEGER, -- e
 *   privateExponent INTEGER, -- d
 *   prime1 INTEGER, -- p
 *   prime2 INTEGER, -- q
 *   exponent1 INTEGER, -- d mod (p-1)
 *   exponent2 INTEGER, -- d mod (q-1)
 *   coefficient INTEGER -- (inverse of q) mod p
 * }
 *
 * Version ::= INTEGER
 *
 * The OID for the RSA key algorithm is: 1.2.840.113549.1.1.1
 */

var forge$8 = forge$s;








if(typeof BigInteger === 'undefined') {
  var BigInteger = forge$8.jsbn.BigInteger;
}

var _crypto = forge$8.util.isNodejs ? require$$1__default : null;

// shortcut for asn.1 API
var asn1$5 = forge$8.asn1;

// shortcut for util API
var util = forge$8.util;

/*
 * RSA encryption and decryption, see RFC 2313.
 */
forge$8.pki = forge$8.pki || {};
forge$8.pki.rsa = forge$8.rsa = forge$8.rsa || {};
var pki$4 = forge$8.pki;

// for finding primes, which are 30k+i for i = 1, 7, 11, 13, 17, 19, 23, 29
var GCD_30_DELTA = [6, 4, 2, 4, 2, 4, 6, 2];

// validator for a PrivateKeyInfo structure
var privateKeyValidator = {
  // PrivateKeyInfo
  name: 'PrivateKeyInfo',
  tagClass: asn1$5.Class.UNIVERSAL,
  type: asn1$5.Type.SEQUENCE,
  constructed: true,
  value: [{
    // Version (INTEGER)
    name: 'PrivateKeyInfo.version',
    tagClass: asn1$5.Class.UNIVERSAL,
    type: asn1$5.Type.INTEGER,
    constructed: false,
    capture: 'privateKeyVersion'
  }, {
    // privateKeyAlgorithm
    name: 'PrivateKeyInfo.privateKeyAlgorithm',
    tagClass: asn1$5.Class.UNIVERSAL,
    type: asn1$5.Type.SEQUENCE,
    constructed: true,
    value: [{
      name: 'AlgorithmIdentifier.algorithm',
      tagClass: asn1$5.Class.UNIVERSAL,
      type: asn1$5.Type.OID,
      constructed: false,
      capture: 'privateKeyOid'
    }]
  }, {
    // PrivateKey
    name: 'PrivateKeyInfo',
    tagClass: asn1$5.Class.UNIVERSAL,
    type: asn1$5.Type.OCTETSTRING,
    constructed: false,
    capture: 'privateKey'
  }]
};

// validator for an RSA private key
var rsaPrivateKeyValidator = {
  // RSAPrivateKey
  name: 'RSAPrivateKey',
  tagClass: asn1$5.Class.UNIVERSAL,
  type: asn1$5.Type.SEQUENCE,
  constructed: true,
  value: [{
    // Version (INTEGER)
    name: 'RSAPrivateKey.version',
    tagClass: asn1$5.Class.UNIVERSAL,
    type: asn1$5.Type.INTEGER,
    constructed: false,
    capture: 'privateKeyVersion'
  }, {
    // modulus (n)
    name: 'RSAPrivateKey.modulus',
    tagClass: asn1$5.Class.UNIVERSAL,
    type: asn1$5.Type.INTEGER,
    constructed: false,
    capture: 'privateKeyModulus'
  }, {
    // publicExponent (e)
    name: 'RSAPrivateKey.publicExponent',
    tagClass: asn1$5.Class.UNIVERSAL,
    type: asn1$5.Type.INTEGER,
    constructed: false,
    capture: 'privateKeyPublicExponent'
  }, {
    // privateExponent (d)
    name: 'RSAPrivateKey.privateExponent',
    tagClass: asn1$5.Class.UNIVERSAL,
    type: asn1$5.Type.INTEGER,
    constructed: false,
    capture: 'privateKeyPrivateExponent'
  }, {
    // prime1 (p)
    name: 'RSAPrivateKey.prime1',
    tagClass: asn1$5.Class.UNIVERSAL,
    type: asn1$5.Type.INTEGER,
    constructed: false,
    capture: 'privateKeyPrime1'
  }, {
    // prime2 (q)
    name: 'RSAPrivateKey.prime2',
    tagClass: asn1$5.Class.UNIVERSAL,
    type: asn1$5.Type.INTEGER,
    constructed: false,
    capture: 'privateKeyPrime2'
  }, {
    // exponent1 (d mod (p-1))
    name: 'RSAPrivateKey.exponent1',
    tagClass: asn1$5.Class.UNIVERSAL,
    type: asn1$5.Type.INTEGER,
    constructed: false,
    capture: 'privateKeyExponent1'
  }, {
    // exponent2 (d mod (q-1))
    name: 'RSAPrivateKey.exponent2',
    tagClass: asn1$5.Class.UNIVERSAL,
    type: asn1$5.Type.INTEGER,
    constructed: false,
    capture: 'privateKeyExponent2'
  }, {
    // coefficient ((inverse of q) mod p)
    name: 'RSAPrivateKey.coefficient',
    tagClass: asn1$5.Class.UNIVERSAL,
    type: asn1$5.Type.INTEGER,
    constructed: false,
    capture: 'privateKeyCoefficient'
  }]
};

// validator for an RSA public key
var rsaPublicKeyValidator = {
  // RSAPublicKey
  name: 'RSAPublicKey',
  tagClass: asn1$5.Class.UNIVERSAL,
  type: asn1$5.Type.SEQUENCE,
  constructed: true,
  value: [{
    // modulus (n)
    name: 'RSAPublicKey.modulus',
    tagClass: asn1$5.Class.UNIVERSAL,
    type: asn1$5.Type.INTEGER,
    constructed: false,
    capture: 'publicKeyModulus'
  }, {
    // publicExponent (e)
    name: 'RSAPublicKey.exponent',
    tagClass: asn1$5.Class.UNIVERSAL,
    type: asn1$5.Type.INTEGER,
    constructed: false,
    capture: 'publicKeyExponent'
  }]
};

// validator for an SubjectPublicKeyInfo structure
// Note: Currently only works with an RSA public key
var publicKeyValidator$1 = forge$8.pki.rsa.publicKeyValidator = {
  name: 'SubjectPublicKeyInfo',
  tagClass: asn1$5.Class.UNIVERSAL,
  type: asn1$5.Type.SEQUENCE,
  constructed: true,
  captureAsn1: 'subjectPublicKeyInfo',
  value: [{
    name: 'SubjectPublicKeyInfo.AlgorithmIdentifier',
    tagClass: asn1$5.Class.UNIVERSAL,
    type: asn1$5.Type.SEQUENCE,
    constructed: true,
    value: [{
      name: 'AlgorithmIdentifier.algorithm',
      tagClass: asn1$5.Class.UNIVERSAL,
      type: asn1$5.Type.OID,
      constructed: false,
      capture: 'publicKeyOid'
    }]
  }, {
    // subjectPublicKey
    name: 'SubjectPublicKeyInfo.subjectPublicKey',
    tagClass: asn1$5.Class.UNIVERSAL,
    type: asn1$5.Type.BITSTRING,
    constructed: false,
    value: [{
      // RSAPublicKey
      name: 'SubjectPublicKeyInfo.subjectPublicKey.RSAPublicKey',
      tagClass: asn1$5.Class.UNIVERSAL,
      type: asn1$5.Type.SEQUENCE,
      constructed: true,
      optional: true,
      captureAsn1: 'rsaPublicKey'
    }]
  }]
};

// validator for a DigestInfo structure
var digestInfoValidator = {
  name: 'DigestInfo',
  tagClass: asn1$5.Class.UNIVERSAL,
  type: asn1$5.Type.SEQUENCE,
  constructed: true,
  value: [{
    name: 'DigestInfo.DigestAlgorithm',
    tagClass: asn1$5.Class.UNIVERSAL,
    type: asn1$5.Type.SEQUENCE,
    constructed: true,
    value: [{
      name: 'DigestInfo.DigestAlgorithm.algorithmIdentifier',
      tagClass: asn1$5.Class.UNIVERSAL,
      type: asn1$5.Type.OID,
      constructed: false,
      capture: 'algorithmIdentifier'
    }, {
      // NULL paramters
      name: 'DigestInfo.DigestAlgorithm.parameters',
      tagClass: asn1$5.Class.UNIVERSAL,
      type: asn1$5.Type.NULL,
      // captured only to check existence for md2 and md5
      capture: 'parameters',
      optional: true,
      constructed: false
    }]
  }, {
    // digest
    name: 'DigestInfo.digest',
    tagClass: asn1$5.Class.UNIVERSAL,
    type: asn1$5.Type.OCTETSTRING,
    constructed: false,
    capture: 'digest'
  }]
};

/**
 * Wrap digest in DigestInfo object.
 *
 * This function implements EMSA-PKCS1-v1_5-ENCODE as per RFC 3447.
 *
 * DigestInfo ::= SEQUENCE {
 *   digestAlgorithm DigestAlgorithmIdentifier,
 *   digest Digest
 * }
 *
 * DigestAlgorithmIdentifier ::= AlgorithmIdentifier
 * Digest ::= OCTET STRING
 *
 * @param md the message digest object with the hash to sign.
 *
 * @return the encoded message (ready for RSA encrytion)
 */
var emsaPkcs1v15encode = function(md) {
  // get the oid for the algorithm
  var oid;
  if(md.algorithm in pki$4.oids) {
    oid = pki$4.oids[md.algorithm];
  } else {
    var error = new Error('Unknown message digest algorithm.');
    error.algorithm = md.algorithm;
    throw error;
  }
  var oidBytes = asn1$5.oidToDer(oid).getBytes();

  // create the digest info
  var digestInfo = asn1$5.create(
    asn1$5.Class.UNIVERSAL, asn1$5.Type.SEQUENCE, true, []);
  var digestAlgorithm = asn1$5.create(
    asn1$5.Class.UNIVERSAL, asn1$5.Type.SEQUENCE, true, []);
  digestAlgorithm.value.push(asn1$5.create(
    asn1$5.Class.UNIVERSAL, asn1$5.Type.OID, false, oidBytes));
  digestAlgorithm.value.push(asn1$5.create(
    asn1$5.Class.UNIVERSAL, asn1$5.Type.NULL, false, ''));
  var digest = asn1$5.create(
    asn1$5.Class.UNIVERSAL, asn1$5.Type.OCTETSTRING,
    false, md.digest().getBytes());
  digestInfo.value.push(digestAlgorithm);
  digestInfo.value.push(digest);

  // encode digest info
  return asn1$5.toDer(digestInfo).getBytes();
};

/**
 * Performs x^c mod n (RSA encryption or decryption operation).
 *
 * @param x the number to raise and mod.
 * @param key the key to use.
 * @param pub true if the key is public, false if private.
 *
 * @return the result of x^c mod n.
 */
var _modPow = function(x, key, pub) {
  if(pub) {
    return x.modPow(key.e, key.n);
  }

  if(!key.p || !key.q) {
    // allow calculation without CRT params (slow)
    return x.modPow(key.d, key.n);
  }

  // pre-compute dP, dQ, and qInv if necessary
  if(!key.dP) {
    key.dP = key.d.mod(key.p.subtract(BigInteger.ONE));
  }
  if(!key.dQ) {
    key.dQ = key.d.mod(key.q.subtract(BigInteger.ONE));
  }
  if(!key.qInv) {
    key.qInv = key.q.modInverse(key.p);
  }

  /* Chinese remainder theorem (CRT) states:

    Suppose n1, n2, ..., nk are positive integers which are pairwise
    coprime (n1 and n2 have no common factors other than 1). For any
    integers x1, x2, ..., xk there exists an integer x solving the
    system of simultaneous congruences (where ~= means modularly
    congruent so a ~= b mod n means a mod n = b mod n):

    x ~= x1 mod n1
    x ~= x2 mod n2
    ...
    x ~= xk mod nk

    This system of congruences has a single simultaneous solution x
    between 0 and n - 1. Furthermore, each xk solution and x itself
    is congruent modulo the product n = n1*n2*...*nk.
    So x1 mod n = x2 mod n = xk mod n = x mod n.

    The single simultaneous solution x can be solved with the following
    equation:

    x = sum(xi*ri*si) mod n where ri = n/ni and si = ri^-1 mod ni.

    Where x is less than n, xi = x mod ni.

    For RSA we are only concerned with k = 2. The modulus n = pq, where
    p and q are coprime. The RSA decryption algorithm is:

    y = x^d mod n

    Given the above:

    x1 = x^d mod p
    r1 = n/p = q
    s1 = q^-1 mod p
    x2 = x^d mod q
    r2 = n/q = p
    s2 = p^-1 mod q

    So y = (x1r1s1 + x2r2s2) mod n
         = ((x^d mod p)q(q^-1 mod p) + (x^d mod q)p(p^-1 mod q)) mod n

    According to Fermat's Little Theorem, if the modulus P is prime,
    for any integer A not evenly divisible by P, A^(P-1) ~= 1 mod P.
    Since A is not divisible by P it follows that if:
    N ~= M mod (P - 1), then A^N mod P = A^M mod P. Therefore:

    A^N mod P = A^(M mod (P - 1)) mod P. (The latter takes less effort
    to calculate). In order to calculate x^d mod p more quickly the
    exponent d mod (p - 1) is stored in the RSA private key (the same
    is done for x^d mod q). These values are referred to as dP and dQ
    respectively. Therefore we now have:

    y = ((x^dP mod p)q(q^-1 mod p) + (x^dQ mod q)p(p^-1 mod q)) mod n

    Since we'll be reducing x^dP by modulo p (same for q) we can also
    reduce x by p (and q respectively) before hand. Therefore, let

    xp = ((x mod p)^dP mod p), and
    xq = ((x mod q)^dQ mod q), yielding:

    y = (xp*q*(q^-1 mod p) + xq*p*(p^-1 mod q)) mod n

    This can be further reduced to a simple algorithm that only
    requires 1 inverse (the q inverse is used) to be used and stored.
    The algorithm is called Garner's algorithm. If qInv is the
    inverse of q, we simply calculate:

    y = (qInv*(xp - xq) mod p) * q + xq

    However, there are two further complications. First, we need to
    ensure that xp > xq to prevent signed BigIntegers from being used
    so we add p until this is true (since we will be mod'ing with
    p anyway). Then, there is a known timing attack on algorithms
    using the CRT. To mitigate this risk, "cryptographic blinding"
    should be used. This requires simply generating a random number r
    between 0 and n-1 and its inverse and multiplying x by r^e before
    calculating y and then multiplying y by r^-1 afterwards. Note that
    r must be coprime with n (gcd(r, n) === 1) in order to have an
    inverse.
  */

  // cryptographic blinding
  var r;
  do {
    r = new BigInteger(
      forge$8.util.bytesToHex(forge$8.random.getBytes(key.n.bitLength() / 8)),
      16);
  } while(r.compareTo(key.n) >= 0 || !r.gcd(key.n).equals(BigInteger.ONE));
  x = x.multiply(r.modPow(key.e, key.n)).mod(key.n);

  // calculate xp and xq
  var xp = x.mod(key.p).modPow(key.dP, key.p);
  var xq = x.mod(key.q).modPow(key.dQ, key.q);

  // xp must be larger than xq to avoid signed bit usage
  while(xp.compareTo(xq) < 0) {
    xp = xp.add(key.p);
  }

  // do last step
  var y = xp.subtract(xq)
    .multiply(key.qInv).mod(key.p)
    .multiply(key.q).add(xq);

  // remove effect of random for cryptographic blinding
  y = y.multiply(r.modInverse(key.n)).mod(key.n);

  return y;
};

/**
 * NOTE: THIS METHOD IS DEPRECATED, use 'sign' on a private key object or
 * 'encrypt' on a public key object instead.
 *
 * Performs RSA encryption.
 *
 * The parameter bt controls whether to put padding bytes before the
 * message passed in. Set bt to either true or false to disable padding
 * completely (in order to handle e.g. EMSA-PSS encoding seperately before),
 * signaling whether the encryption operation is a public key operation
 * (i.e. encrypting data) or not, i.e. private key operation (data signing).
 *
 * For PKCS#1 v1.5 padding pass in the block type to use, i.e. either 0x01
 * (for signing) or 0x02 (for encryption). The key operation mode (private
 * or public) is derived from this flag in that case).
 *
 * @param m the message to encrypt as a byte string.
 * @param key the RSA key to use.
 * @param bt for PKCS#1 v1.5 padding, the block type to use
 *   (0x01 for private key, 0x02 for public),
 *   to disable padding: true = public key, false = private key.
 *
 * @return the encrypted bytes as a string.
 */
pki$4.rsa.encrypt = function(m, key, bt) {
  var pub = bt;
  var eb;

  // get the length of the modulus in bytes
  var k = Math.ceil(key.n.bitLength() / 8);

  if(bt !== false && bt !== true) {
    // legacy, default to PKCS#1 v1.5 padding
    pub = (bt === 0x02);
    eb = _encodePkcs1_v1_5(m, key, bt);
  } else {
    eb = forge$8.util.createBuffer();
    eb.putBytes(m);
  }

  // load encryption block as big integer 'x'
  // FIXME: hex conversion inefficient, get BigInteger w/byte strings
  var x = new BigInteger(eb.toHex(), 16);

  // do RSA encryption
  var y = _modPow(x, key, pub);

  // convert y into the encrypted data byte string, if y is shorter in
  // bytes than k, then prepend zero bytes to fill up ed
  // FIXME: hex conversion inefficient, get BigInteger w/byte strings
  var yhex = y.toString(16);
  var ed = forge$8.util.createBuffer();
  var zeros = k - Math.ceil(yhex.length / 2);
  while(zeros > 0) {
    ed.putByte(0x00);
    --zeros;
  }
  ed.putBytes(forge$8.util.hexToBytes(yhex));
  return ed.getBytes();
};

/**
 * NOTE: THIS METHOD IS DEPRECATED, use 'decrypt' on a private key object or
 * 'verify' on a public key object instead.
 *
 * Performs RSA decryption.
 *
 * The parameter ml controls whether to apply PKCS#1 v1.5 padding
 * or not.  Set ml = false to disable padding removal completely
 * (in order to handle e.g. EMSA-PSS later on) and simply pass back
 * the RSA encryption block.
 *
 * @param ed the encrypted data to decrypt in as a byte string.
 * @param key the RSA key to use.
 * @param pub true for a public key operation, false for private.
 * @param ml the message length, if known, false to disable padding.
 *
 * @return the decrypted message as a byte string.
 */
pki$4.rsa.decrypt = function(ed, key, pub, ml) {
  // get the length of the modulus in bytes
  var k = Math.ceil(key.n.bitLength() / 8);

  // error if the length of the encrypted data ED is not k
  if(ed.length !== k) {
    var error = new Error('Encrypted message length is invalid.');
    error.length = ed.length;
    error.expected = k;
    throw error;
  }

  // convert encrypted data into a big integer
  // FIXME: hex conversion inefficient, get BigInteger w/byte strings
  var y = new BigInteger(forge$8.util.createBuffer(ed).toHex(), 16);

  // y must be less than the modulus or it wasn't the result of
  // a previous mod operation (encryption) using that modulus
  if(y.compareTo(key.n) >= 0) {
    throw new Error('Encrypted message is invalid.');
  }

  // do RSA decryption
  var x = _modPow(y, key, pub);

  // create the encryption block, if x is shorter in bytes than k, then
  // prepend zero bytes to fill up eb
  // FIXME: hex conversion inefficient, get BigInteger w/byte strings
  var xhex = x.toString(16);
  var eb = forge$8.util.createBuffer();
  var zeros = k - Math.ceil(xhex.length / 2);
  while(zeros > 0) {
    eb.putByte(0x00);
    --zeros;
  }
  eb.putBytes(forge$8.util.hexToBytes(xhex));

  if(ml !== false) {
    // legacy, default to PKCS#1 v1.5 padding
    return _decodePkcs1_v1_5(eb.getBytes(), key, pub);
  }

  // return message
  return eb.getBytes();
};

/**
 * Creates an RSA key-pair generation state object. It is used to allow
 * key-generation to be performed in steps. It also allows for a UI to
 * display progress updates.
 *
 * @param bits the size for the private key in bits, defaults to 2048.
 * @param e the public exponent to use, defaults to 65537 (0x10001).
 * @param [options] the options to use.
 *          prng a custom crypto-secure pseudo-random number generator to use,
 *            that must define "getBytesSync".
 *          algorithm the algorithm to use (default: 'PRIMEINC').
 *
 * @return the state object to use to generate the key-pair.
 */
pki$4.rsa.createKeyPairGenerationState = function(bits, e, options) {
  // TODO: migrate step-based prime generation code to forge.prime

  // set default bits
  if(typeof(bits) === 'string') {
    bits = parseInt(bits, 10);
  }
  bits = bits || 2048;

  // create prng with api that matches BigInteger secure random
  options = options || {};
  var prng = options.prng || forge$8.random;
  var rng = {
    // x is an array to fill with bytes
    nextBytes: function(x) {
      var b = prng.getBytesSync(x.length);
      for(var i = 0; i < x.length; ++i) {
        x[i] = b.charCodeAt(i);
      }
    }
  };

  var algorithm = options.algorithm || 'PRIMEINC';

  // create PRIMEINC algorithm state
  var rval;
  if(algorithm === 'PRIMEINC') {
    rval = {
      algorithm: algorithm,
      state: 0,
      bits: bits,
      rng: rng,
      eInt: e || 65537,
      e: new BigInteger(null),
      p: null,
      q: null,
      qBits: bits >> 1,
      pBits: bits - (bits >> 1),
      pqState: 0,
      num: null,
      keys: null
    };
    rval.e.fromInt(rval.eInt);
  } else {
    throw new Error('Invalid key generation algorithm: ' + algorithm);
  }

  return rval;
};

/**
 * Attempts to runs the key-generation algorithm for at most n seconds
 * (approximately) using the given state. When key-generation has completed,
 * the keys will be stored in state.keys.
 *
 * To use this function to update a UI while generating a key or to prevent
 * causing browser lockups/warnings, set "n" to a value other than 0. A
 * simple pattern for generating a key and showing a progress indicator is:
 *
 * var state = pki.rsa.createKeyPairGenerationState(2048);
 * var step = function() {
 *   // step key-generation, run algorithm for 100 ms, repeat
 *   if(!forge.pki.rsa.stepKeyPairGenerationState(state, 100)) {
 *     setTimeout(step, 1);
 *   } else {
 *     // key-generation complete
 *     // TODO: turn off progress indicator here
 *     // TODO: use the generated key-pair in "state.keys"
 *   }
 * };
 * // TODO: turn on progress indicator here
 * setTimeout(step, 0);
 *
 * @param state the state to use.
 * @param n the maximum number of milliseconds to run the algorithm for, 0
 *          to run the algorithm to completion.
 *
 * @return true if the key-generation completed, false if not.
 */
pki$4.rsa.stepKeyPairGenerationState = function(state, n) {
  // set default algorithm if not set
  if(!('algorithm' in state)) {
    state.algorithm = 'PRIMEINC';
  }

  // TODO: migrate step-based prime generation code to forge.prime
  // TODO: abstract as PRIMEINC algorithm

  // do key generation (based on Tom Wu's rsa.js, see jsbn.js license)
  // with some minor optimizations and designed to run in steps

  // local state vars
  var THIRTY = new BigInteger(null);
  THIRTY.fromInt(30);
  var deltaIdx = 0;
  var op_or = function(x, y) {return x | y;};

  // keep stepping until time limit is reached or done
  var t1 = +new Date();
  var t2;
  var total = 0;
  while(state.keys === null && (n <= 0 || total < n)) {
    // generate p or q
    if(state.state === 0) {
      /* Note: All primes are of the form:

        30k+i, for i < 30 and gcd(30, i)=1, where there are 8 values for i

        When we generate a random number, we always align it at 30k + 1. Each
        time the number is determined not to be prime we add to get to the
        next 'i', eg: if the number was at 30k + 1 we add 6. */
      var bits = (state.p === null) ? state.pBits : state.qBits;
      var bits1 = bits - 1;

      // get a random number
      if(state.pqState === 0) {
        state.num = new BigInteger(bits, state.rng);
        // force MSB set
        if(!state.num.testBit(bits1)) {
          state.num.bitwiseTo(
            BigInteger.ONE.shiftLeft(bits1), op_or, state.num);
        }
        // align number on 30k+1 boundary
        state.num.dAddOffset(31 - state.num.mod(THIRTY).byteValue(), 0);
        deltaIdx = 0;

        ++state.pqState;
      } else if(state.pqState === 1) {
        // try to make the number a prime
        if(state.num.bitLength() > bits) {
          // overflow, try again
          state.pqState = 0;
          // do primality test
        } else if(state.num.isProbablePrime(
          _getMillerRabinTests(state.num.bitLength()))) {
          ++state.pqState;
        } else {
          // get next potential prime
          state.num.dAddOffset(GCD_30_DELTA[deltaIdx++ % 8], 0);
        }
      } else if(state.pqState === 2) {
        // ensure number is coprime with e
        state.pqState =
          (state.num.subtract(BigInteger.ONE).gcd(state.e)
            .compareTo(BigInteger.ONE) === 0) ? 3 : 0;
      } else if(state.pqState === 3) {
        // store p or q
        state.pqState = 0;
        if(state.p === null) {
          state.p = state.num;
        } else {
          state.q = state.num;
        }

        // advance state if both p and q are ready
        if(state.p !== null && state.q !== null) {
          ++state.state;
        }
        state.num = null;
      }
    } else if(state.state === 1) {
      // ensure p is larger than q (swap them if not)
      if(state.p.compareTo(state.q) < 0) {
        state.num = state.p;
        state.p = state.q;
        state.q = state.num;
      }
      ++state.state;
    } else if(state.state === 2) {
      // compute phi: (p - 1)(q - 1) (Euler's totient function)
      state.p1 = state.p.subtract(BigInteger.ONE);
      state.q1 = state.q.subtract(BigInteger.ONE);
      state.phi = state.p1.multiply(state.q1);
      ++state.state;
    } else if(state.state === 3) {
      // ensure e and phi are coprime
      if(state.phi.gcd(state.e).compareTo(BigInteger.ONE) === 0) {
        // phi and e are coprime, advance
        ++state.state;
      } else {
        // phi and e aren't coprime, so generate a new p and q
        state.p = null;
        state.q = null;
        state.state = 0;
      }
    } else if(state.state === 4) {
      // create n, ensure n is has the right number of bits
      state.n = state.p.multiply(state.q);

      // ensure n is right number of bits
      if(state.n.bitLength() === state.bits) {
        // success, advance
        ++state.state;
      } else {
        // failed, get new q
        state.q = null;
        state.state = 0;
      }
    } else if(state.state === 5) {
      // set keys
      var d = state.e.modInverse(state.phi);
      state.keys = {
        privateKey: pki$4.rsa.setPrivateKey(
          state.n, state.e, d, state.p, state.q,
          d.mod(state.p1), d.mod(state.q1),
          state.q.modInverse(state.p)),
        publicKey: pki$4.rsa.setPublicKey(state.n, state.e)
      };
    }

    // update timing
    t2 = +new Date();
    total += t2 - t1;
    t1 = t2;
  }

  return state.keys !== null;
};

/**
 * Generates an RSA public-private key pair in a single call.
 *
 * To generate a key-pair in steps (to allow for progress updates and to
 * prevent blocking or warnings in slow browsers) then use the key-pair
 * generation state functions.
 *
 * To generate a key-pair asynchronously (either through web-workers, if
 * available, or by breaking up the work on the main thread), pass a
 * callback function.
 *
 * @param [bits] the size for the private key in bits, defaults to 2048.
 * @param [e] the public exponent to use, defaults to 65537.
 * @param [options] options for key-pair generation, if given then 'bits'
 *            and 'e' must *not* be given:
 *          bits the size for the private key in bits, (default: 2048).
 *          e the public exponent to use, (default: 65537 (0x10001)).
 *          workerScript the worker script URL.
 *          workers the number of web workers (if supported) to use,
 *            (default: 2).
 *          workLoad the size of the work load, ie: number of possible prime
 *            numbers for each web worker to check per work assignment,
 *            (default: 100).
 *          prng a custom crypto-secure pseudo-random number generator to use,
 *            that must define "getBytesSync". Disables use of native APIs.
 *          algorithm the algorithm to use (default: 'PRIMEINC').
 * @param [callback(err, keypair)] called once the operation completes.
 *
 * @return an object with privateKey and publicKey properties.
 */
pki$4.rsa.generateKeyPair = function(bits, e, options, callback) {
  // (bits), (options), (callback)
  if(arguments.length === 1) {
    if(typeof bits === 'object') {
      options = bits;
      bits = undefined;
    } else if(typeof bits === 'function') {
      callback = bits;
      bits = undefined;
    }
  } else if(arguments.length === 2) {
    // (bits, e), (bits, options), (bits, callback), (options, callback)
    if(typeof bits === 'number') {
      if(typeof e === 'function') {
        callback = e;
        e = undefined;
      } else if(typeof e !== 'number') {
        options = e;
        e = undefined;
      }
    } else {
      options = bits;
      callback = e;
      bits = undefined;
      e = undefined;
    }
  } else if(arguments.length === 3) {
    // (bits, e, options), (bits, e, callback), (bits, options, callback)
    if(typeof e === 'number') {
      if(typeof options === 'function') {
        callback = options;
        options = undefined;
      }
    } else {
      callback = options;
      options = e;
      e = undefined;
    }
  }
  options = options || {};
  if(bits === undefined) {
    bits = options.bits || 2048;
  }
  if(e === undefined) {
    e = options.e || 0x10001;
  }

  // use native code if permitted, available, and parameters are acceptable
  if(!options.prng &&
    bits >= 256 && bits <= 16384 && (e === 0x10001 || e === 3)) {
    if(callback) {
      // try native async
      if(_detectNodeCrypto('generateKeyPair')) {
        return _crypto.generateKeyPair('rsa', {
          modulusLength: bits,
          publicExponent: e,
          publicKeyEncoding: {
            type: 'spki',
            format: 'pem'
          },
          privateKeyEncoding: {
            type: 'pkcs8',
            format: 'pem'
          }
        }, function(err, pub, priv) {
          if(err) {
            return callback(err);
          }
          callback(null, {
            privateKey: pki$4.privateKeyFromPem(priv),
            publicKey: pki$4.publicKeyFromPem(pub)
          });
        });
      }
      if(_detectSubtleCrypto('generateKey') &&
        _detectSubtleCrypto('exportKey')) {
        // use standard native generateKey
        return util.globalScope.crypto.subtle.generateKey({
          name: 'RSASSA-PKCS1-v1_5',
          modulusLength: bits,
          publicExponent: _intToUint8Array(e),
          hash: {name: 'SHA-256'}
        }, true /* key can be exported*/, ['sign', 'verify'])
        .then(function(pair) {
          return util.globalScope.crypto.subtle.exportKey(
            'pkcs8', pair.privateKey);
        // avoiding catch(function(err) {...}) to support IE <= 8
        }).then(undefined, function(err) {
          callback(err);
        }).then(function(pkcs8) {
          if(pkcs8) {
            var privateKey = pki$4.privateKeyFromAsn1(
              asn1$5.fromDer(forge$8.util.createBuffer(pkcs8)));
            callback(null, {
              privateKey: privateKey,
              publicKey: pki$4.setRsaPublicKey(privateKey.n, privateKey.e)
            });
          }
        });
      }
      if(_detectSubtleMsCrypto('generateKey') &&
        _detectSubtleMsCrypto('exportKey')) {
        var genOp = util.globalScope.msCrypto.subtle.generateKey({
          name: 'RSASSA-PKCS1-v1_5',
          modulusLength: bits,
          publicExponent: _intToUint8Array(e),
          hash: {name: 'SHA-256'}
        }, true /* key can be exported*/, ['sign', 'verify']);
        genOp.oncomplete = function(e) {
          var pair = e.target.result;
          var exportOp = util.globalScope.msCrypto.subtle.exportKey(
            'pkcs8', pair.privateKey);
          exportOp.oncomplete = function(e) {
            var pkcs8 = e.target.result;
            var privateKey = pki$4.privateKeyFromAsn1(
              asn1$5.fromDer(forge$8.util.createBuffer(pkcs8)));
            callback(null, {
              privateKey: privateKey,
              publicKey: pki$4.setRsaPublicKey(privateKey.n, privateKey.e)
            });
          };
          exportOp.onerror = function(err) {
            callback(err);
          };
        };
        genOp.onerror = function(err) {
          callback(err);
        };
        return;
      }
    } else {
      // try native sync
      if(_detectNodeCrypto('generateKeyPairSync')) {
        var keypair = _crypto.generateKeyPairSync('rsa', {
          modulusLength: bits,
          publicExponent: e,
          publicKeyEncoding: {
            type: 'spki',
            format: 'pem'
          },
          privateKeyEncoding: {
            type: 'pkcs8',
            format: 'pem'
          }
        });
        return {
          privateKey: pki$4.privateKeyFromPem(keypair.privateKey),
          publicKey: pki$4.publicKeyFromPem(keypair.publicKey)
        };
      }
    }
  }

  // use JavaScript implementation
  var state = pki$4.rsa.createKeyPairGenerationState(bits, e, options);
  if(!callback) {
    pki$4.rsa.stepKeyPairGenerationState(state, 0);
    return state.keys;
  }
  _generateKeyPair(state, options, callback);
};

/**
 * Sets an RSA public key from BigIntegers modulus and exponent.
 *
 * @param n the modulus.
 * @param e the exponent.
 *
 * @return the public key.
 */
pki$4.setRsaPublicKey = pki$4.rsa.setPublicKey = function(n, e) {
  var key = {
    n: n,
    e: e
  };

  /**
   * Encrypts the given data with this public key. Newer applications
   * should use the 'RSA-OAEP' decryption scheme, 'RSAES-PKCS1-V1_5' is for
   * legacy applications.
   *
   * @param data the byte string to encrypt.
   * @param scheme the encryption scheme to use:
   *          'RSAES-PKCS1-V1_5' (default),
   *          'RSA-OAEP',
   *          'RAW', 'NONE', or null to perform raw RSA encryption,
   *          an object with an 'encode' property set to a function
   *          with the signature 'function(data, key)' that returns
   *          a binary-encoded string representing the encoded data.
   * @param schemeOptions any scheme-specific options.
   *
   * @return the encrypted byte string.
   */
  key.encrypt = function(data, scheme, schemeOptions) {
    if(typeof scheme === 'string') {
      scheme = scheme.toUpperCase();
    } else if(scheme === undefined) {
      scheme = 'RSAES-PKCS1-V1_5';
    }

    if(scheme === 'RSAES-PKCS1-V1_5') {
      scheme = {
        encode: function(m, key, pub) {
          return _encodePkcs1_v1_5(m, key, 0x02).getBytes();
        }
      };
    } else if(scheme === 'RSA-OAEP' || scheme === 'RSAES-OAEP') {
      scheme = {
        encode: function(m, key) {
          return forge$8.pkcs1.encode_rsa_oaep(key, m, schemeOptions);
        }
      };
    } else if(['RAW', 'NONE', 'NULL', null].indexOf(scheme) !== -1) {
      scheme = {encode: function(e) {return e;}};
    } else if(typeof scheme === 'string') {
      throw new Error('Unsupported encryption scheme: "' + scheme + '".');
    }

    // do scheme-based encoding then rsa encryption
    var e = scheme.encode(data, key, true);
    return pki$4.rsa.encrypt(e, key, true);
  };

  /**
   * Verifies the given signature against the given digest.
   *
   * PKCS#1 supports multiple (currently two) signature schemes:
   * RSASSA-PKCS1-V1_5 and RSASSA-PSS.
   *
   * By default this implementation uses the "old scheme", i.e.
   * RSASSA-PKCS1-V1_5, in which case once RSA-decrypted, the
   * signature is an OCTET STRING that holds a DigestInfo.
   *
   * DigestInfo ::= SEQUENCE {
   *   digestAlgorithm DigestAlgorithmIdentifier,
   *   digest Digest
   * }
   * DigestAlgorithmIdentifier ::= AlgorithmIdentifier
   * Digest ::= OCTET STRING
   *
   * To perform PSS signature verification, provide an instance
   * of Forge PSS object as the scheme parameter.
   *
   * @param digest the message digest hash to compare against the signature,
   *          as a binary-encoded string.
   * @param signature the signature to verify, as a binary-encoded string.
   * @param scheme signature verification scheme to use:
   *          'RSASSA-PKCS1-V1_5' or undefined for RSASSA PKCS#1 v1.5,
   *          a Forge PSS object for RSASSA-PSS,
   *          'NONE' or null for none, DigestInfo will not be expected, but
   *            PKCS#1 v1.5 padding will still be used.
   * @param options optional verify options
   *          _parseAllDigestBytes testing flag to control parsing of all
   *            digest bytes. Unsupported and not for general usage.
   *            (default: true)
   *
   * @return true if the signature was verified, false if not.
   */
  key.verify = function(digest, signature, scheme, options) {
    if(typeof scheme === 'string') {
      scheme = scheme.toUpperCase();
    } else if(scheme === undefined) {
      scheme = 'RSASSA-PKCS1-V1_5';
    }
    if(options === undefined) {
      options = {
        _parseAllDigestBytes: true
      };
    }
    if(!('_parseAllDigestBytes' in options)) {
      options._parseAllDigestBytes = true;
    }

    if(scheme === 'RSASSA-PKCS1-V1_5') {
      scheme = {
        verify: function(digest, d) {
          // remove padding
          d = _decodePkcs1_v1_5(d, key, true);
          // d is ASN.1 BER-encoded DigestInfo
          var obj = asn1$5.fromDer(d, {
            parseAllBytes: options._parseAllDigestBytes
          });

          // validate DigestInfo
          var capture = {};
          var errors = [];
          if(!asn1$5.validate(obj, digestInfoValidator, capture, errors)) {
            var error = new Error(
              'ASN.1 object does not contain a valid RSASSA-PKCS1-v1_5 ' +
              'DigestInfo value.');
            error.errors = errors;
            throw error;
          }
          // check hash algorithm identifier
          // see PKCS1-v1-5DigestAlgorithms in RFC 8017
          // FIXME: add support to vaidator for strict value choices
          var oid = asn1$5.derToOid(capture.algorithmIdentifier);
          if(!(oid === forge$8.oids.md2 ||
            oid === forge$8.oids.md5 ||
            oid === forge$8.oids.sha1 ||
            oid === forge$8.oids.sha224 ||
            oid === forge$8.oids.sha256 ||
            oid === forge$8.oids.sha384 ||
            oid === forge$8.oids.sha512 ||
            oid === forge$8.oids['sha512-224'] ||
            oid === forge$8.oids['sha512-256'])) {
            var error = new Error(
              'Unknown RSASSA-PKCS1-v1_5 DigestAlgorithm identifier.');
            error.oid = oid;
            throw error;
          }

          // special check for md2 and md5 that NULL parameters exist
          if(oid === forge$8.oids.md2 || oid === forge$8.oids.md5) {
            if(!('parameters' in capture)) {
              throw new Error(
                'ASN.1 object does not contain a valid RSASSA-PKCS1-v1_5 ' +
                'DigestInfo value. ' +
                'Missing algorithm identifer NULL parameters.');
            }
          }

          // compare the given digest to the decrypted one
          return digest === capture.digest;
        }
      };
    } else if(scheme === 'NONE' || scheme === 'NULL' || scheme === null) {
      scheme = {
        verify: function(digest, d) {
          // remove padding
          d = _decodePkcs1_v1_5(d, key, true);
          return digest === d;
        }
      };
    }

    // do rsa decryption w/o any decoding, then verify -- which does decoding
    var d = pki$4.rsa.decrypt(signature, key, true, false);
    return scheme.verify(digest, d, key.n.bitLength());
  };

  return key;
};

/**
 * Sets an RSA private key from BigIntegers modulus, exponent, primes,
 * prime exponents, and modular multiplicative inverse.
 *
 * @param n the modulus.
 * @param e the public exponent.
 * @param d the private exponent ((inverse of e) mod n).
 * @param p the first prime.
 * @param q the second prime.
 * @param dP exponent1 (d mod (p-1)).
 * @param dQ exponent2 (d mod (q-1)).
 * @param qInv ((inverse of q) mod p)
 *
 * @return the private key.
 */
pki$4.setRsaPrivateKey = pki$4.rsa.setPrivateKey = function(
  n, e, d, p, q, dP, dQ, qInv) {
  var key = {
    n: n,
    e: e,
    d: d,
    p: p,
    q: q,
    dP: dP,
    dQ: dQ,
    qInv: qInv
  };

  /**
   * Decrypts the given data with this private key. The decryption scheme
   * must match the one used to encrypt the data.
   *
   * @param data the byte string to decrypt.
   * @param scheme the decryption scheme to use:
   *          'RSAES-PKCS1-V1_5' (default),
   *          'RSA-OAEP',
   *          'RAW', 'NONE', or null to perform raw RSA decryption.
   * @param schemeOptions any scheme-specific options.
   *
   * @return the decrypted byte string.
   */
  key.decrypt = function(data, scheme, schemeOptions) {
    if(typeof scheme === 'string') {
      scheme = scheme.toUpperCase();
    } else if(scheme === undefined) {
      scheme = 'RSAES-PKCS1-V1_5';
    }

    // do rsa decryption w/o any decoding
    var d = pki$4.rsa.decrypt(data, key, false, false);

    if(scheme === 'RSAES-PKCS1-V1_5') {
      scheme = {decode: _decodePkcs1_v1_5};
    } else if(scheme === 'RSA-OAEP' || scheme === 'RSAES-OAEP') {
      scheme = {
        decode: function(d, key) {
          return forge$8.pkcs1.decode_rsa_oaep(key, d, schemeOptions);
        }
      };
    } else if(['RAW', 'NONE', 'NULL', null].indexOf(scheme) !== -1) {
      scheme = {decode: function(d) {return d;}};
    } else {
      throw new Error('Unsupported encryption scheme: "' + scheme + '".');
    }

    // decode according to scheme
    return scheme.decode(d, key, false);
  };

  /**
   * Signs the given digest, producing a signature.
   *
   * PKCS#1 supports multiple (currently two) signature schemes:
   * RSASSA-PKCS1-V1_5 and RSASSA-PSS.
   *
   * By default this implementation uses the "old scheme", i.e.
   * RSASSA-PKCS1-V1_5. In order to generate a PSS signature, provide
   * an instance of Forge PSS object as the scheme parameter.
   *
   * @param md the message digest object with the hash to sign.
   * @param scheme the signature scheme to use:
   *          'RSASSA-PKCS1-V1_5' or undefined for RSASSA PKCS#1 v1.5,
   *          a Forge PSS object for RSASSA-PSS,
   *          'NONE' or null for none, DigestInfo will not be used but
   *            PKCS#1 v1.5 padding will still be used.
   *
   * @return the signature as a byte string.
   */
  key.sign = function(md, scheme) {
    /* Note: The internal implementation of RSA operations is being
      transitioned away from a PKCS#1 v1.5 hard-coded scheme. Some legacy
      code like the use of an encoding block identifier 'bt' will eventually
      be removed. */

    // private key operation
    var bt = false;

    if(typeof scheme === 'string') {
      scheme = scheme.toUpperCase();
    }

    if(scheme === undefined || scheme === 'RSASSA-PKCS1-V1_5') {
      scheme = {encode: emsaPkcs1v15encode};
      bt = 0x01;
    } else if(scheme === 'NONE' || scheme === 'NULL' || scheme === null) {
      scheme = {encode: function() {return md;}};
      bt = 0x01;
    }

    // encode and then encrypt
    var d = scheme.encode(md, key.n.bitLength());
    return pki$4.rsa.encrypt(d, key, bt);
  };

  return key;
};

/**
 * Wraps an RSAPrivateKey ASN.1 object in an ASN.1 PrivateKeyInfo object.
 *
 * @param rsaKey the ASN.1 RSAPrivateKey.
 *
 * @return the ASN.1 PrivateKeyInfo.
 */
pki$4.wrapRsaPrivateKey = function(rsaKey) {
  // PrivateKeyInfo
  return asn1$5.create(asn1$5.Class.UNIVERSAL, asn1$5.Type.SEQUENCE, true, [
    // version (0)
    asn1$5.create(asn1$5.Class.UNIVERSAL, asn1$5.Type.INTEGER, false,
      asn1$5.integerToDer(0).getBytes()),
    // privateKeyAlgorithm
    asn1$5.create(asn1$5.Class.UNIVERSAL, asn1$5.Type.SEQUENCE, true, [
      asn1$5.create(
        asn1$5.Class.UNIVERSAL, asn1$5.Type.OID, false,
        asn1$5.oidToDer(pki$4.oids.rsaEncryption).getBytes()),
      asn1$5.create(asn1$5.Class.UNIVERSAL, asn1$5.Type.NULL, false, '')
    ]),
    // PrivateKey
    asn1$5.create(asn1$5.Class.UNIVERSAL, asn1$5.Type.OCTETSTRING, false,
      asn1$5.toDer(rsaKey).getBytes())
  ]);
};

/**
 * Converts a private key from an ASN.1 object.
 *
 * @param obj the ASN.1 representation of a PrivateKeyInfo containing an
 *          RSAPrivateKey or an RSAPrivateKey.
 *
 * @return the private key.
 */
pki$4.privateKeyFromAsn1 = function(obj) {
  // get PrivateKeyInfo
  var capture = {};
  var errors = [];
  if(asn1$5.validate(obj, privateKeyValidator, capture, errors)) {
    obj = asn1$5.fromDer(forge$8.util.createBuffer(capture.privateKey));
  }

  // get RSAPrivateKey
  capture = {};
  errors = [];
  if(!asn1$5.validate(obj, rsaPrivateKeyValidator, capture, errors)) {
    var error = new Error('Cannot read private key. ' +
      'ASN.1 object does not contain an RSAPrivateKey.');
    error.errors = errors;
    throw error;
  }

  // Note: Version is currently ignored.
  // capture.privateKeyVersion
  // FIXME: inefficient, get a BigInteger that uses byte strings
  var n, e, d, p, q, dP, dQ, qInv;
  n = forge$8.util.createBuffer(capture.privateKeyModulus).toHex();
  e = forge$8.util.createBuffer(capture.privateKeyPublicExponent).toHex();
  d = forge$8.util.createBuffer(capture.privateKeyPrivateExponent).toHex();
  p = forge$8.util.createBuffer(capture.privateKeyPrime1).toHex();
  q = forge$8.util.createBuffer(capture.privateKeyPrime2).toHex();
  dP = forge$8.util.createBuffer(capture.privateKeyExponent1).toHex();
  dQ = forge$8.util.createBuffer(capture.privateKeyExponent2).toHex();
  qInv = forge$8.util.createBuffer(capture.privateKeyCoefficient).toHex();

  // set private key
  return pki$4.setRsaPrivateKey(
    new BigInteger(n, 16),
    new BigInteger(e, 16),
    new BigInteger(d, 16),
    new BigInteger(p, 16),
    new BigInteger(q, 16),
    new BigInteger(dP, 16),
    new BigInteger(dQ, 16),
    new BigInteger(qInv, 16));
};

/**
 * Converts a private key to an ASN.1 RSAPrivateKey.
 *
 * @param key the private key.
 *
 * @return the ASN.1 representation of an RSAPrivateKey.
 */
pki$4.privateKeyToAsn1 = pki$4.privateKeyToRSAPrivateKey = function(key) {
  // RSAPrivateKey
  return asn1$5.create(asn1$5.Class.UNIVERSAL, asn1$5.Type.SEQUENCE, true, [
    // version (0 = only 2 primes, 1 multiple primes)
    asn1$5.create(asn1$5.Class.UNIVERSAL, asn1$5.Type.INTEGER, false,
      asn1$5.integerToDer(0).getBytes()),
    // modulus (n)
    asn1$5.create(asn1$5.Class.UNIVERSAL, asn1$5.Type.INTEGER, false,
      _bnToBytes(key.n)),
    // publicExponent (e)
    asn1$5.create(asn1$5.Class.UNIVERSAL, asn1$5.Type.INTEGER, false,
      _bnToBytes(key.e)),
    // privateExponent (d)
    asn1$5.create(asn1$5.Class.UNIVERSAL, asn1$5.Type.INTEGER, false,
      _bnToBytes(key.d)),
    // privateKeyPrime1 (p)
    asn1$5.create(asn1$5.Class.UNIVERSAL, asn1$5.Type.INTEGER, false,
      _bnToBytes(key.p)),
    // privateKeyPrime2 (q)
    asn1$5.create(asn1$5.Class.UNIVERSAL, asn1$5.Type.INTEGER, false,
      _bnToBytes(key.q)),
    // privateKeyExponent1 (dP)
    asn1$5.create(asn1$5.Class.UNIVERSAL, asn1$5.Type.INTEGER, false,
      _bnToBytes(key.dP)),
    // privateKeyExponent2 (dQ)
    asn1$5.create(asn1$5.Class.UNIVERSAL, asn1$5.Type.INTEGER, false,
      _bnToBytes(key.dQ)),
    // coefficient (qInv)
    asn1$5.create(asn1$5.Class.UNIVERSAL, asn1$5.Type.INTEGER, false,
      _bnToBytes(key.qInv))
  ]);
};

/**
 * Converts a public key from an ASN.1 SubjectPublicKeyInfo or RSAPublicKey.
 *
 * @param obj the asn1 representation of a SubjectPublicKeyInfo or RSAPublicKey.
 *
 * @return the public key.
 */
pki$4.publicKeyFromAsn1 = function(obj) {
  // get SubjectPublicKeyInfo
  var capture = {};
  var errors = [];
  if(asn1$5.validate(obj, publicKeyValidator$1, capture, errors)) {
    // get oid
    var oid = asn1$5.derToOid(capture.publicKeyOid);
    if(oid !== pki$4.oids.rsaEncryption) {
      var error = new Error('Cannot read public key. Unknown OID.');
      error.oid = oid;
      throw error;
    }
    obj = capture.rsaPublicKey;
  }

  // get RSA params
  errors = [];
  if(!asn1$5.validate(obj, rsaPublicKeyValidator, capture, errors)) {
    var error = new Error('Cannot read public key. ' +
      'ASN.1 object does not contain an RSAPublicKey.');
    error.errors = errors;
    throw error;
  }

  // FIXME: inefficient, get a BigInteger that uses byte strings
  var n = forge$8.util.createBuffer(capture.publicKeyModulus).toHex();
  var e = forge$8.util.createBuffer(capture.publicKeyExponent).toHex();

  // set public key
  return pki$4.setRsaPublicKey(
    new BigInteger(n, 16),
    new BigInteger(e, 16));
};

/**
 * Converts a public key to an ASN.1 SubjectPublicKeyInfo.
 *
 * @param key the public key.
 *
 * @return the asn1 representation of a SubjectPublicKeyInfo.
 */
pki$4.publicKeyToAsn1 = pki$4.publicKeyToSubjectPublicKeyInfo = function(key) {
  // SubjectPublicKeyInfo
  return asn1$5.create(asn1$5.Class.UNIVERSAL, asn1$5.Type.SEQUENCE, true, [
    // AlgorithmIdentifier
    asn1$5.create(asn1$5.Class.UNIVERSAL, asn1$5.Type.SEQUENCE, true, [
      // algorithm
      asn1$5.create(asn1$5.Class.UNIVERSAL, asn1$5.Type.OID, false,
        asn1$5.oidToDer(pki$4.oids.rsaEncryption).getBytes()),
      // parameters (null)
      asn1$5.create(asn1$5.Class.UNIVERSAL, asn1$5.Type.NULL, false, '')
    ]),
    // subjectPublicKey
    asn1$5.create(asn1$5.Class.UNIVERSAL, asn1$5.Type.BITSTRING, false, [
      pki$4.publicKeyToRSAPublicKey(key)
    ])
  ]);
};

/**
 * Converts a public key to an ASN.1 RSAPublicKey.
 *
 * @param key the public key.
 *
 * @return the asn1 representation of a RSAPublicKey.
 */
pki$4.publicKeyToRSAPublicKey = function(key) {
  // RSAPublicKey
  return asn1$5.create(asn1$5.Class.UNIVERSAL, asn1$5.Type.SEQUENCE, true, [
    // modulus (n)
    asn1$5.create(asn1$5.Class.UNIVERSAL, asn1$5.Type.INTEGER, false,
      _bnToBytes(key.n)),
    // publicExponent (e)
    asn1$5.create(asn1$5.Class.UNIVERSAL, asn1$5.Type.INTEGER, false,
      _bnToBytes(key.e))
  ]);
};

/**
 * Encodes a message using PKCS#1 v1.5 padding.
 *
 * @param m the message to encode.
 * @param key the RSA key to use.
 * @param bt the block type to use, i.e. either 0x01 (for signing) or 0x02
 *          (for encryption).
 *
 * @return the padded byte buffer.
 */
function _encodePkcs1_v1_5(m, key, bt) {
  var eb = forge$8.util.createBuffer();

  // get the length of the modulus in bytes
  var k = Math.ceil(key.n.bitLength() / 8);

  /* use PKCS#1 v1.5 padding */
  if(m.length > (k - 11)) {
    var error = new Error('Message is too long for PKCS#1 v1.5 padding.');
    error.length = m.length;
    error.max = k - 11;
    throw error;
  }

  /* A block type BT, a padding string PS, and the data D shall be
    formatted into an octet string EB, the encryption block:

    EB = 00 || BT || PS || 00 || D

    The block type BT shall be a single octet indicating the structure of
    the encryption block. For this version of the document it shall have
    value 00, 01, or 02. For a private-key operation, the block type
    shall be 00 or 01. For a public-key operation, it shall be 02.

    The padding string PS shall consist of k-3-||D|| octets. For block
    type 00, the octets shall have value 00; for block type 01, they
    shall have value FF; and for block type 02, they shall be
    pseudorandomly generated and nonzero. This makes the length of the
    encryption block EB equal to k. */

  // build the encryption block
  eb.putByte(0x00);
  eb.putByte(bt);

  // create the padding
  var padNum = k - 3 - m.length;
  var padByte;
  // private key op
  if(bt === 0x00 || bt === 0x01) {
    padByte = (bt === 0x00) ? 0x00 : 0xFF;
    for(var i = 0; i < padNum; ++i) {
      eb.putByte(padByte);
    }
  } else {
    // public key op
    // pad with random non-zero values
    while(padNum > 0) {
      var numZeros = 0;
      var padBytes = forge$8.random.getBytes(padNum);
      for(var i = 0; i < padNum; ++i) {
        padByte = padBytes.charCodeAt(i);
        if(padByte === 0) {
          ++numZeros;
        } else {
          eb.putByte(padByte);
        }
      }
      padNum = numZeros;
    }
  }

  // zero followed by message
  eb.putByte(0x00);
  eb.putBytes(m);

  return eb;
}

/**
 * Decodes a message using PKCS#1 v1.5 padding.
 *
 * @param em the message to decode.
 * @param key the RSA key to use.
 * @param pub true if the key is a public key, false if it is private.
 * @param ml the message length, if specified.
 *
 * @return the decoded bytes.
 */
function _decodePkcs1_v1_5(em, key, pub, ml) {
  // get the length of the modulus in bytes
  var k = Math.ceil(key.n.bitLength() / 8);

  /* It is an error if any of the following conditions occurs:

    1. The encryption block EB cannot be parsed unambiguously.
    2. The padding string PS consists of fewer than eight octets
      or is inconsisent with the block type BT.
    3. The decryption process is a public-key operation and the block
      type BT is not 00 or 01, or the decryption process is a
      private-key operation and the block type is not 02.
   */

  // parse the encryption block
  var eb = forge$8.util.createBuffer(em);
  var first = eb.getByte();
  var bt = eb.getByte();
  if(first !== 0x00 ||
    (pub && bt !== 0x00 && bt !== 0x01) ||
    (!pub && bt != 0x02) ||
    (pub && bt === 0x00 && typeof(ml) === 'undefined')) {
    throw new Error('Encryption block is invalid.');
  }

  var padNum = 0;
  if(bt === 0x00) {
    // check all padding bytes for 0x00
    padNum = k - 3 - ml;
    for(var i = 0; i < padNum; ++i) {
      if(eb.getByte() !== 0x00) {
        throw new Error('Encryption block is invalid.');
      }
    }
  } else if(bt === 0x01) {
    // find the first byte that isn't 0xFF, should be after all padding
    padNum = 0;
    while(eb.length() > 1) {
      if(eb.getByte() !== 0xFF) {
        --eb.read;
        break;
      }
      ++padNum;
    }
  } else if(bt === 0x02) {
    // look for 0x00 byte
    padNum = 0;
    while(eb.length() > 1) {
      if(eb.getByte() === 0x00) {
        --eb.read;
        break;
      }
      ++padNum;
    }
  }

  // zero must be 0x00 and padNum must be (k - 3 - message length)
  var zero = eb.getByte();
  if(zero !== 0x00 || padNum !== (k - 3 - eb.length())) {
    throw new Error('Encryption block is invalid.');
  }

  return eb.getBytes();
}

/**
 * Runs the key-generation algorithm asynchronously, either in the background
 * via Web Workers, or using the main thread and setImmediate.
 *
 * @param state the key-pair generation state.
 * @param [options] options for key-pair generation:
 *          workerScript the worker script URL.
 *          workers the number of web workers (if supported) to use,
 *            (default: 2, -1 to use estimated cores minus one).
 *          workLoad the size of the work load, ie: number of possible prime
 *            numbers for each web worker to check per work assignment,
 *            (default: 100).
 * @param callback(err, keypair) called once the operation completes.
 */
function _generateKeyPair(state, options, callback) {
  if(typeof options === 'function') {
    callback = options;
    options = {};
  }
  options = options || {};

  var opts = {
    algorithm: {
      name: options.algorithm || 'PRIMEINC',
      options: {
        workers: options.workers || 2,
        workLoad: options.workLoad || 100,
        workerScript: options.workerScript
      }
    }
  };
  if('prng' in options) {
    opts.prng = options.prng;
  }

  generate();

  function generate() {
    // find p and then q (done in series to simplify)
    getPrime(state.pBits, function(err, num) {
      if(err) {
        return callback(err);
      }
      state.p = num;
      if(state.q !== null) {
        return finish(err, state.q);
      }
      getPrime(state.qBits, finish);
    });
  }

  function getPrime(bits, callback) {
    forge$8.prime.generateProbablePrime(bits, opts, callback);
  }

  function finish(err, num) {
    if(err) {
      return callback(err);
    }

    // set q
    state.q = num;

    // ensure p is larger than q (swap them if not)
    if(state.p.compareTo(state.q) < 0) {
      var tmp = state.p;
      state.p = state.q;
      state.q = tmp;
    }

    // ensure p is coprime with e
    if(state.p.subtract(BigInteger.ONE).gcd(state.e)
      .compareTo(BigInteger.ONE) !== 0) {
      state.p = null;
      generate();
      return;
    }

    // ensure q is coprime with e
    if(state.q.subtract(BigInteger.ONE).gcd(state.e)
      .compareTo(BigInteger.ONE) !== 0) {
      state.q = null;
      getPrime(state.qBits, finish);
      return;
    }

    // compute phi: (p - 1)(q - 1) (Euler's totient function)
    state.p1 = state.p.subtract(BigInteger.ONE);
    state.q1 = state.q.subtract(BigInteger.ONE);
    state.phi = state.p1.multiply(state.q1);

    // ensure e and phi are coprime
    if(state.phi.gcd(state.e).compareTo(BigInteger.ONE) !== 0) {
      // phi and e aren't coprime, so generate a new p and q
      state.p = state.q = null;
      generate();
      return;
    }

    // create n, ensure n is has the right number of bits
    state.n = state.p.multiply(state.q);
    if(state.n.bitLength() !== state.bits) {
      // failed, get new q
      state.q = null;
      getPrime(state.qBits, finish);
      return;
    }

    // set keys
    var d = state.e.modInverse(state.phi);
    state.keys = {
      privateKey: pki$4.rsa.setPrivateKey(
        state.n, state.e, d, state.p, state.q,
        d.mod(state.p1), d.mod(state.q1),
        state.q.modInverse(state.p)),
      publicKey: pki$4.rsa.setPublicKey(state.n, state.e)
    };

    callback(null, state.keys);
  }
}

/**
 * Converts a positive BigInteger into 2's-complement big-endian bytes.
 *
 * @param b the big integer to convert.
 *
 * @return the bytes.
 */
function _bnToBytes(b) {
  // prepend 0x00 if first byte >= 0x80
  var hex = b.toString(16);
  if(hex[0] >= '8') {
    hex = '00' + hex;
  }
  var bytes = forge$8.util.hexToBytes(hex);

  // ensure integer is minimally-encoded
  if(bytes.length > 1 &&
    // leading 0x00 for positive integer
    ((bytes.charCodeAt(0) === 0 &&
    (bytes.charCodeAt(1) & 0x80) === 0) ||
    // leading 0xFF for negative integer
    (bytes.charCodeAt(0) === 0xFF &&
    (bytes.charCodeAt(1) & 0x80) === 0x80))) {
    return bytes.substr(1);
  }
  return bytes;
}

/**
 * Returns the required number of Miller-Rabin tests to generate a
 * prime with an error probability of (1/2)^80.
 *
 * See Handbook of Applied Cryptography Chapter 4, Table 4.4.
 *
 * @param bits the bit size.
 *
 * @return the required number of iterations.
 */
function _getMillerRabinTests(bits) {
  if(bits <= 100) return 27;
  if(bits <= 150) return 18;
  if(bits <= 200) return 15;
  if(bits <= 250) return 12;
  if(bits <= 300) return 9;
  if(bits <= 350) return 8;
  if(bits <= 400) return 7;
  if(bits <= 500) return 6;
  if(bits <= 600) return 5;
  if(bits <= 800) return 4;
  if(bits <= 1250) return 3;
  return 2;
}

/**
 * Performs feature detection on the Node crypto interface.
 *
 * @param fn the feature (function) to detect.
 *
 * @return true if detected, false if not.
 */
function _detectNodeCrypto(fn) {
  return forge$8.util.isNodejs && typeof _crypto[fn] === 'function';
}

/**
 * Performs feature detection on the SubtleCrypto interface.
 *
 * @param fn the feature (function) to detect.
 *
 * @return true if detected, false if not.
 */
function _detectSubtleCrypto(fn) {
  return (typeof util.globalScope !== 'undefined' &&
    typeof util.globalScope.crypto === 'object' &&
    typeof util.globalScope.crypto.subtle === 'object' &&
    typeof util.globalScope.crypto.subtle[fn] === 'function');
}

/**
 * Performs feature detection on the deprecated Microsoft Internet Explorer
 * outdated SubtleCrypto interface. This function should only be used after
 * checking for the modern, standard SubtleCrypto interface.
 *
 * @param fn the feature (function) to detect.
 *
 * @return true if detected, false if not.
 */
function _detectSubtleMsCrypto(fn) {
  return (typeof util.globalScope !== 'undefined' &&
    typeof util.globalScope.msCrypto === 'object' &&
    typeof util.globalScope.msCrypto.subtle === 'object' &&
    typeof util.globalScope.msCrypto.subtle[fn] === 'function');
}

function _intToUint8Array(x) {
  var bytes = forge$8.util.hexToBytes(x.toString(16));
  var buffer = new Uint8Array(bytes.length);
  for(var i = 0; i < bytes.length; ++i) {
    buffer[i] = bytes.charCodeAt(i);
  }
  return buffer;
}

/**
 * Password-based encryption functions.
 *
 * @author Dave Longley
 * @author Stefan Siegl <stesie@brokenpipe.de>
 *
 * Copyright (c) 2010-2013 Digital Bazaar, Inc.
 * Copyright (c) 2012 Stefan Siegl <stesie@brokenpipe.de>
 *
 * An EncryptedPrivateKeyInfo:
 *
 * EncryptedPrivateKeyInfo ::= SEQUENCE {
 *   encryptionAlgorithm  EncryptionAlgorithmIdentifier,
 *   encryptedData        EncryptedData }
 *
 * EncryptionAlgorithmIdentifier ::= AlgorithmIdentifier
 *
 * EncryptedData ::= OCTET STRING
 */

var forge$7 = forge$s;

// shortcut for asn.1 API
var asn1$4 = forge$7.asn1;

/* Password-based encryption implementation. */
var pki$3 = forge$7.pki = forge$7.pki || {};
pki$3.pbe = forge$7.pbe = forge$7.pbe || {};
var oids$1 = pki$3.oids;

// validator for an EncryptedPrivateKeyInfo structure
// Note: Currently only works w/algorithm params
var encryptedPrivateKeyValidator = {
  name: 'EncryptedPrivateKeyInfo',
  tagClass: asn1$4.Class.UNIVERSAL,
  type: asn1$4.Type.SEQUENCE,
  constructed: true,
  value: [{
    name: 'EncryptedPrivateKeyInfo.encryptionAlgorithm',
    tagClass: asn1$4.Class.UNIVERSAL,
    type: asn1$4.Type.SEQUENCE,
    constructed: true,
    value: [{
      name: 'AlgorithmIdentifier.algorithm',
      tagClass: asn1$4.Class.UNIVERSAL,
      type: asn1$4.Type.OID,
      constructed: false,
      capture: 'encryptionOid'
    }, {
      name: 'AlgorithmIdentifier.parameters',
      tagClass: asn1$4.Class.UNIVERSAL,
      type: asn1$4.Type.SEQUENCE,
      constructed: true,
      captureAsn1: 'encryptionParams'
    }]
  }, {
    // encryptedData
    name: 'EncryptedPrivateKeyInfo.encryptedData',
    tagClass: asn1$4.Class.UNIVERSAL,
    type: asn1$4.Type.OCTETSTRING,
    constructed: false,
    capture: 'encryptedData'
  }]
};

// validator for a PBES2Algorithms structure
// Note: Currently only works w/PBKDF2 + AES encryption schemes
var PBES2AlgorithmsValidator = {
  name: 'PBES2Algorithms',
  tagClass: asn1$4.Class.UNIVERSAL,
  type: asn1$4.Type.SEQUENCE,
  constructed: true,
  value: [{
    name: 'PBES2Algorithms.keyDerivationFunc',
    tagClass: asn1$4.Class.UNIVERSAL,
    type: asn1$4.Type.SEQUENCE,
    constructed: true,
    value: [{
      name: 'PBES2Algorithms.keyDerivationFunc.oid',
      tagClass: asn1$4.Class.UNIVERSAL,
      type: asn1$4.Type.OID,
      constructed: false,
      capture: 'kdfOid'
    }, {
      name: 'PBES2Algorithms.params',
      tagClass: asn1$4.Class.UNIVERSAL,
      type: asn1$4.Type.SEQUENCE,
      constructed: true,
      value: [{
        name: 'PBES2Algorithms.params.salt',
        tagClass: asn1$4.Class.UNIVERSAL,
        type: asn1$4.Type.OCTETSTRING,
        constructed: false,
        capture: 'kdfSalt'
      }, {
        name: 'PBES2Algorithms.params.iterationCount',
        tagClass: asn1$4.Class.UNIVERSAL,
        type: asn1$4.Type.INTEGER,
        constructed: false,
        capture: 'kdfIterationCount'
      }, {
        name: 'PBES2Algorithms.params.keyLength',
        tagClass: asn1$4.Class.UNIVERSAL,
        type: asn1$4.Type.INTEGER,
        constructed: false,
        optional: true,
        capture: 'keyLength'
      }, {
        // prf
        name: 'PBES2Algorithms.params.prf',
        tagClass: asn1$4.Class.UNIVERSAL,
        type: asn1$4.Type.SEQUENCE,
        constructed: true,
        optional: true,
        value: [{
          name: 'PBES2Algorithms.params.prf.algorithm',
          tagClass: asn1$4.Class.UNIVERSAL,
          type: asn1$4.Type.OID,
          constructed: false,
          capture: 'prfOid'
        }]
      }]
    }]
  }, {
    name: 'PBES2Algorithms.encryptionScheme',
    tagClass: asn1$4.Class.UNIVERSAL,
    type: asn1$4.Type.SEQUENCE,
    constructed: true,
    value: [{
      name: 'PBES2Algorithms.encryptionScheme.oid',
      tagClass: asn1$4.Class.UNIVERSAL,
      type: asn1$4.Type.OID,
      constructed: false,
      capture: 'encOid'
    }, {
      name: 'PBES2Algorithms.encryptionScheme.iv',
      tagClass: asn1$4.Class.UNIVERSAL,
      type: asn1$4.Type.OCTETSTRING,
      constructed: false,
      capture: 'encIv'
    }]
  }]
};

var pkcs12PbeParamsValidator = {
  name: 'pkcs-12PbeParams',
  tagClass: asn1$4.Class.UNIVERSAL,
  type: asn1$4.Type.SEQUENCE,
  constructed: true,
  value: [{
    name: 'pkcs-12PbeParams.salt',
    tagClass: asn1$4.Class.UNIVERSAL,
    type: asn1$4.Type.OCTETSTRING,
    constructed: false,
    capture: 'salt'
  }, {
    name: 'pkcs-12PbeParams.iterations',
    tagClass: asn1$4.Class.UNIVERSAL,
    type: asn1$4.Type.INTEGER,
    constructed: false,
    capture: 'iterations'
  }]
};

/**
 * Encrypts a ASN.1 PrivateKeyInfo object, producing an EncryptedPrivateKeyInfo.
 *
 * PBES2Algorithms ALGORITHM-IDENTIFIER ::=
 *   { {PBES2-params IDENTIFIED BY id-PBES2}, ...}
 *
 * id-PBES2 OBJECT IDENTIFIER ::= {pkcs-5 13}
 *
 * PBES2-params ::= SEQUENCE {
 *   keyDerivationFunc AlgorithmIdentifier {{PBES2-KDFs}},
 *   encryptionScheme AlgorithmIdentifier {{PBES2-Encs}}
 * }
 *
 * PBES2-KDFs ALGORITHM-IDENTIFIER ::=
 *   { {PBKDF2-params IDENTIFIED BY id-PBKDF2}, ... }
 *
 * PBES2-Encs ALGORITHM-IDENTIFIER ::= { ... }
 *
 * PBKDF2-params ::= SEQUENCE {
 *   salt CHOICE {
 *     specified OCTET STRING,
 *     otherSource AlgorithmIdentifier {{PBKDF2-SaltSources}}
 *   },
 *   iterationCount INTEGER (1..MAX),
 *   keyLength INTEGER (1..MAX) OPTIONAL,
 *   prf AlgorithmIdentifier {{PBKDF2-PRFs}} DEFAULT algid-hmacWithSHA1
 * }
 *
 * @param obj the ASN.1 PrivateKeyInfo object.
 * @param password the password to encrypt with.
 * @param options:
 *          algorithm the encryption algorithm to use
 *            ('aes128', 'aes192', 'aes256', '3des'), defaults to 'aes128'.
 *          count the iteration count to use.
 *          saltSize the salt size to use.
 *          prfAlgorithm the PRF message digest algorithm to use
 *            ('sha1', 'sha224', 'sha256', 'sha384', 'sha512')
 *
 * @return the ASN.1 EncryptedPrivateKeyInfo.
 */
pki$3.encryptPrivateKeyInfo = function(obj, password, options) {
  // set default options
  options = options || {};
  options.saltSize = options.saltSize || 8;
  options.count = options.count || 2048;
  options.algorithm = options.algorithm || 'aes128';
  options.prfAlgorithm = options.prfAlgorithm || 'sha1';

  // generate PBE params
  var salt = forge$7.random.getBytesSync(options.saltSize);
  var count = options.count;
  var countBytes = asn1$4.integerToDer(count);
  var dkLen;
  var encryptionAlgorithm;
  var encryptedData;
  if(options.algorithm.indexOf('aes') === 0 || options.algorithm === 'des') {
    // do PBES2
    var ivLen, encOid, cipherFn;
    switch(options.algorithm) {
    case 'aes128':
      dkLen = 16;
      ivLen = 16;
      encOid = oids$1['aes128-CBC'];
      cipherFn = forge$7.aes.createEncryptionCipher;
      break;
    case 'aes192':
      dkLen = 24;
      ivLen = 16;
      encOid = oids$1['aes192-CBC'];
      cipherFn = forge$7.aes.createEncryptionCipher;
      break;
    case 'aes256':
      dkLen = 32;
      ivLen = 16;
      encOid = oids$1['aes256-CBC'];
      cipherFn = forge$7.aes.createEncryptionCipher;
      break;
    case 'des':
      dkLen = 8;
      ivLen = 8;
      encOid = oids$1['desCBC'];
      cipherFn = forge$7.des.createEncryptionCipher;
      break;
    default:
      var error = new Error('Cannot encrypt private key. Unknown encryption algorithm.');
      error.algorithm = options.algorithm;
      throw error;
    }

    // get PRF message digest
    var prfAlgorithm = 'hmacWith' + options.prfAlgorithm.toUpperCase();
    var md = prfAlgorithmToMessageDigest(prfAlgorithm);

    // encrypt private key using pbe SHA-1 and AES/DES
    var dk = forge$7.pkcs5.pbkdf2(password, salt, count, dkLen, md);
    var iv = forge$7.random.getBytesSync(ivLen);
    var cipher = cipherFn(dk);
    cipher.start(iv);
    cipher.update(asn1$4.toDer(obj));
    cipher.finish();
    encryptedData = cipher.output.getBytes();

    // get PBKDF2-params
    var params = createPbkdf2Params(salt, countBytes, dkLen, prfAlgorithm);

    encryptionAlgorithm = asn1$4.create(
      asn1$4.Class.UNIVERSAL, asn1$4.Type.SEQUENCE, true, [
      asn1$4.create(asn1$4.Class.UNIVERSAL, asn1$4.Type.OID, false,
        asn1$4.oidToDer(oids$1['pkcs5PBES2']).getBytes()),
      asn1$4.create(asn1$4.Class.UNIVERSAL, asn1$4.Type.SEQUENCE, true, [
        // keyDerivationFunc
        asn1$4.create(asn1$4.Class.UNIVERSAL, asn1$4.Type.SEQUENCE, true, [
          asn1$4.create(asn1$4.Class.UNIVERSAL, asn1$4.Type.OID, false,
            asn1$4.oidToDer(oids$1['pkcs5PBKDF2']).getBytes()),
          // PBKDF2-params
          params
        ]),
        // encryptionScheme
        asn1$4.create(asn1$4.Class.UNIVERSAL, asn1$4.Type.SEQUENCE, true, [
          asn1$4.create(asn1$4.Class.UNIVERSAL, asn1$4.Type.OID, false,
            asn1$4.oidToDer(encOid).getBytes()),
          // iv
          asn1$4.create(
            asn1$4.Class.UNIVERSAL, asn1$4.Type.OCTETSTRING, false, iv)
        ])
      ])
    ]);
  } else if(options.algorithm === '3des') {
    // Do PKCS12 PBE
    dkLen = 24;

    var saltBytes = new forge$7.util.ByteBuffer(salt);
    var dk = pki$3.pbe.generatePkcs12Key(password, saltBytes, 1, count, dkLen);
    var iv = pki$3.pbe.generatePkcs12Key(password, saltBytes, 2, count, dkLen);
    var cipher = forge$7.des.createEncryptionCipher(dk);
    cipher.start(iv);
    cipher.update(asn1$4.toDer(obj));
    cipher.finish();
    encryptedData = cipher.output.getBytes();

    encryptionAlgorithm = asn1$4.create(
      asn1$4.Class.UNIVERSAL, asn1$4.Type.SEQUENCE, true, [
      asn1$4.create(asn1$4.Class.UNIVERSAL, asn1$4.Type.OID, false,
        asn1$4.oidToDer(oids$1['pbeWithSHAAnd3-KeyTripleDES-CBC']).getBytes()),
      // pkcs-12PbeParams
      asn1$4.create(asn1$4.Class.UNIVERSAL, asn1$4.Type.SEQUENCE, true, [
        // salt
        asn1$4.create(asn1$4.Class.UNIVERSAL, asn1$4.Type.OCTETSTRING, false, salt),
        // iteration count
        asn1$4.create(asn1$4.Class.UNIVERSAL, asn1$4.Type.INTEGER, false,
          countBytes.getBytes())
      ])
    ]);
  } else {
    var error = new Error('Cannot encrypt private key. Unknown encryption algorithm.');
    error.algorithm = options.algorithm;
    throw error;
  }

  // EncryptedPrivateKeyInfo
  var rval = asn1$4.create(asn1$4.Class.UNIVERSAL, asn1$4.Type.SEQUENCE, true, [
    // encryptionAlgorithm
    encryptionAlgorithm,
    // encryptedData
    asn1$4.create(
      asn1$4.Class.UNIVERSAL, asn1$4.Type.OCTETSTRING, false, encryptedData)
  ]);
  return rval;
};

/**
 * Decrypts a ASN.1 PrivateKeyInfo object.
 *
 * @param obj the ASN.1 EncryptedPrivateKeyInfo object.
 * @param password the password to decrypt with.
 *
 * @return the ASN.1 PrivateKeyInfo on success, null on failure.
 */
pki$3.decryptPrivateKeyInfo = function(obj, password) {
  var rval = null;

  // get PBE params
  var capture = {};
  var errors = [];
  if(!asn1$4.validate(obj, encryptedPrivateKeyValidator, capture, errors)) {
    var error = new Error('Cannot read encrypted private key. ' +
      'ASN.1 object is not a supported EncryptedPrivateKeyInfo.');
    error.errors = errors;
    throw error;
  }

  // get cipher
  var oid = asn1$4.derToOid(capture.encryptionOid);
  var cipher = pki$3.pbe.getCipher(oid, capture.encryptionParams, password);

  // get encrypted data
  var encrypted = forge$7.util.createBuffer(capture.encryptedData);

  cipher.update(encrypted);
  if(cipher.finish()) {
    rval = asn1$4.fromDer(cipher.output);
  }

  return rval;
};

/**
 * Converts a EncryptedPrivateKeyInfo to PEM format.
 *
 * @param epki the EncryptedPrivateKeyInfo.
 * @param maxline the maximum characters per line, defaults to 64.
 *
 * @return the PEM-formatted encrypted private key.
 */
pki$3.encryptedPrivateKeyToPem = function(epki, maxline) {
  // convert to DER, then PEM-encode
  var msg = {
    type: 'ENCRYPTED PRIVATE KEY',
    body: asn1$4.toDer(epki).getBytes()
  };
  return forge$7.pem.encode(msg, {maxline: maxline});
};

/**
 * Converts a PEM-encoded EncryptedPrivateKeyInfo to ASN.1 format. Decryption
 * is not performed.
 *
 * @param pem the EncryptedPrivateKeyInfo in PEM-format.
 *
 * @return the ASN.1 EncryptedPrivateKeyInfo.
 */
pki$3.encryptedPrivateKeyFromPem = function(pem) {
  var msg = forge$7.pem.decode(pem)[0];

  if(msg.type !== 'ENCRYPTED PRIVATE KEY') {
    var error = new Error('Could not convert encrypted private key from PEM; ' +
      'PEM header type is "ENCRYPTED PRIVATE KEY".');
    error.headerType = msg.type;
    throw error;
  }
  if(msg.procType && msg.procType.type === 'ENCRYPTED') {
    throw new Error('Could not convert encrypted private key from PEM; ' +
      'PEM is encrypted.');
  }

  // convert DER to ASN.1 object
  return asn1$4.fromDer(msg.body);
};

/**
 * Encrypts an RSA private key. By default, the key will be wrapped in
 * a PrivateKeyInfo and encrypted to produce a PKCS#8 EncryptedPrivateKeyInfo.
 * This is the standard, preferred way to encrypt a private key.
 *
 * To produce a non-standard PEM-encrypted private key that uses encapsulated
 * headers to indicate the encryption algorithm (old-style non-PKCS#8 OpenSSL
 * private key encryption), set the 'legacy' option to true. Note: Using this
 * option will cause the iteration count to be forced to 1.
 *
 * Note: The 'des' algorithm is supported, but it is not considered to be
 * secure because it only uses a single 56-bit key. If possible, it is highly
 * recommended that a different algorithm be used.
 *
 * @param rsaKey the RSA key to encrypt.
 * @param password the password to use.
 * @param options:
 *          algorithm: the encryption algorithm to use
 *            ('aes128', 'aes192', 'aes256', '3des', 'des').
 *          count: the iteration count to use.
 *          saltSize: the salt size to use.
 *          legacy: output an old non-PKCS#8 PEM-encrypted+encapsulated
 *            headers (DEK-Info) private key.
 *
 * @return the PEM-encoded ASN.1 EncryptedPrivateKeyInfo.
 */
pki$3.encryptRsaPrivateKey = function(rsaKey, password, options) {
  // standard PKCS#8
  options = options || {};
  if(!options.legacy) {
    // encrypt PrivateKeyInfo
    var rval = pki$3.wrapRsaPrivateKey(pki$3.privateKeyToAsn1(rsaKey));
    rval = pki$3.encryptPrivateKeyInfo(rval, password, options);
    return pki$3.encryptedPrivateKeyToPem(rval);
  }

  // legacy non-PKCS#8
  var algorithm;
  var iv;
  var dkLen;
  var cipherFn;
  switch(options.algorithm) {
  case 'aes128':
    algorithm = 'AES-128-CBC';
    dkLen = 16;
    iv = forge$7.random.getBytesSync(16);
    cipherFn = forge$7.aes.createEncryptionCipher;
    break;
  case 'aes192':
    algorithm = 'AES-192-CBC';
    dkLen = 24;
    iv = forge$7.random.getBytesSync(16);
    cipherFn = forge$7.aes.createEncryptionCipher;
    break;
  case 'aes256':
    algorithm = 'AES-256-CBC';
    dkLen = 32;
    iv = forge$7.random.getBytesSync(16);
    cipherFn = forge$7.aes.createEncryptionCipher;
    break;
  case '3des':
    algorithm = 'DES-EDE3-CBC';
    dkLen = 24;
    iv = forge$7.random.getBytesSync(8);
    cipherFn = forge$7.des.createEncryptionCipher;
    break;
  case 'des':
    algorithm = 'DES-CBC';
    dkLen = 8;
    iv = forge$7.random.getBytesSync(8);
    cipherFn = forge$7.des.createEncryptionCipher;
    break;
  default:
    var error = new Error('Could not encrypt RSA private key; unsupported ' +
      'encryption algorithm "' + options.algorithm + '".');
    error.algorithm = options.algorithm;
    throw error;
  }

  // encrypt private key using OpenSSL legacy key derivation
  var dk = forge$7.pbe.opensslDeriveBytes(password, iv.substr(0, 8), dkLen);
  var cipher = cipherFn(dk);
  cipher.start(iv);
  cipher.update(asn1$4.toDer(pki$3.privateKeyToAsn1(rsaKey)));
  cipher.finish();

  var msg = {
    type: 'RSA PRIVATE KEY',
    procType: {
      version: '4',
      type: 'ENCRYPTED'
    },
    dekInfo: {
      algorithm: algorithm,
      parameters: forge$7.util.bytesToHex(iv).toUpperCase()
    },
    body: cipher.output.getBytes()
  };
  return forge$7.pem.encode(msg);
};

/**
 * Decrypts an RSA private key.
 *
 * @param pem the PEM-formatted EncryptedPrivateKeyInfo to decrypt.
 * @param password the password to use.
 *
 * @return the RSA key on success, null on failure.
 */
pki$3.decryptRsaPrivateKey = function(pem, password) {
  var rval = null;

  var msg = forge$7.pem.decode(pem)[0];

  if(msg.type !== 'ENCRYPTED PRIVATE KEY' &&
    msg.type !== 'PRIVATE KEY' &&
    msg.type !== 'RSA PRIVATE KEY') {
    var error = new Error('Could not convert private key from PEM; PEM header type ' +
      'is not "ENCRYPTED PRIVATE KEY", "PRIVATE KEY", or "RSA PRIVATE KEY".');
    error.headerType = error;
    throw error;
  }

  if(msg.procType && msg.procType.type === 'ENCRYPTED') {
    var dkLen;
    var cipherFn;
    switch(msg.dekInfo.algorithm) {
    case 'DES-CBC':
      dkLen = 8;
      cipherFn = forge$7.des.createDecryptionCipher;
      break;
    case 'DES-EDE3-CBC':
      dkLen = 24;
      cipherFn = forge$7.des.createDecryptionCipher;
      break;
    case 'AES-128-CBC':
      dkLen = 16;
      cipherFn = forge$7.aes.createDecryptionCipher;
      break;
    case 'AES-192-CBC':
      dkLen = 24;
      cipherFn = forge$7.aes.createDecryptionCipher;
      break;
    case 'AES-256-CBC':
      dkLen = 32;
      cipherFn = forge$7.aes.createDecryptionCipher;
      break;
    case 'RC2-40-CBC':
      dkLen = 5;
      cipherFn = function(key) {
        return forge$7.rc2.createDecryptionCipher(key, 40);
      };
      break;
    case 'RC2-64-CBC':
      dkLen = 8;
      cipherFn = function(key) {
        return forge$7.rc2.createDecryptionCipher(key, 64);
      };
      break;
    case 'RC2-128-CBC':
      dkLen = 16;
      cipherFn = function(key) {
        return forge$7.rc2.createDecryptionCipher(key, 128);
      };
      break;
    default:
      var error = new Error('Could not decrypt private key; unsupported ' +
        'encryption algorithm "' + msg.dekInfo.algorithm + '".');
      error.algorithm = msg.dekInfo.algorithm;
      throw error;
    }

    // use OpenSSL legacy key derivation
    var iv = forge$7.util.hexToBytes(msg.dekInfo.parameters);
    var dk = forge$7.pbe.opensslDeriveBytes(password, iv.substr(0, 8), dkLen);
    var cipher = cipherFn(dk);
    cipher.start(iv);
    cipher.update(forge$7.util.createBuffer(msg.body));
    if(cipher.finish()) {
      rval = cipher.output.getBytes();
    } else {
      return rval;
    }
  } else {
    rval = msg.body;
  }

  if(msg.type === 'ENCRYPTED PRIVATE KEY') {
    rval = pki$3.decryptPrivateKeyInfo(asn1$4.fromDer(rval), password);
  } else {
    // decryption already performed above
    rval = asn1$4.fromDer(rval);
  }

  if(rval !== null) {
    rval = pki$3.privateKeyFromAsn1(rval);
  }

  return rval;
};

/**
 * Derives a PKCS#12 key.
 *
 * @param password the password to derive the key material from, null or
 *          undefined for none.
 * @param salt the salt, as a ByteBuffer, to use.
 * @param id the PKCS#12 ID byte (1 = key material, 2 = IV, 3 = MAC).
 * @param iter the iteration count.
 * @param n the number of bytes to derive from the password.
 * @param md the message digest to use, defaults to SHA-1.
 *
 * @return a ByteBuffer with the bytes derived from the password.
 */
pki$3.pbe.generatePkcs12Key = function(password, salt, id, iter, n, md) {
  var j, l;

  if(typeof md === 'undefined' || md === null) {
    if(!('sha1' in forge$7.md)) {
      throw new Error('"sha1" hash algorithm unavailable.');
    }
    md = forge$7.md.sha1.create();
  }

  var u = md.digestLength;
  var v = md.blockLength;
  var result = new forge$7.util.ByteBuffer();

  /* Convert password to Unicode byte buffer + trailing 0-byte. */
  var passBuf = new forge$7.util.ByteBuffer();
  if(password !== null && password !== undefined) {
    for(l = 0; l < password.length; l++) {
      passBuf.putInt16(password.charCodeAt(l));
    }
    passBuf.putInt16(0);
  }

  /* Length of salt and password in BYTES. */
  var p = passBuf.length();
  var s = salt.length();

  /* 1. Construct a string, D (the "diversifier"), by concatenating
        v copies of ID. */
  var D = new forge$7.util.ByteBuffer();
  D.fillWithByte(id, v);

  /* 2. Concatenate copies of the salt together to create a string S of length
        v * ceil(s / v) bytes (the final copy of the salt may be trunacted
        to create S).
        Note that if the salt is the empty string, then so is S. */
  var Slen = v * Math.ceil(s / v);
  var S = new forge$7.util.ByteBuffer();
  for(l = 0; l < Slen; l++) {
    S.putByte(salt.at(l % s));
  }

  /* 3. Concatenate copies of the password together to create a string P of
        length v * ceil(p / v) bytes (the final copy of the password may be
        truncated to create P).
        Note that if the password is the empty string, then so is P. */
  var Plen = v * Math.ceil(p / v);
  var P = new forge$7.util.ByteBuffer();
  for(l = 0; l < Plen; l++) {
    P.putByte(passBuf.at(l % p));
  }

  /* 4. Set I=S||P to be the concatenation of S and P. */
  var I = S;
  I.putBuffer(P);

  /* 5. Set c=ceil(n / u). */
  var c = Math.ceil(n / u);

  /* 6. For i=1, 2, ..., c, do the following: */
  for(var i = 1; i <= c; i++) {
    /* a) Set Ai=H^r(D||I). (l.e. the rth hash of D||I, H(H(H(...H(D||I)))) */
    var buf = new forge$7.util.ByteBuffer();
    buf.putBytes(D.bytes());
    buf.putBytes(I.bytes());
    for(var round = 0; round < iter; round++) {
      md.start();
      md.update(buf.getBytes());
      buf = md.digest();
    }

    /* b) Concatenate copies of Ai to create a string B of length v bytes (the
          final copy of Ai may be truncated to create B). */
    var B = new forge$7.util.ByteBuffer();
    for(l = 0; l < v; l++) {
      B.putByte(buf.at(l % u));
    }

    /* c) Treating I as a concatenation I0, I1, ..., Ik-1 of v-byte blocks,
          where k=ceil(s / v) + ceil(p / v), modify I by setting
          Ij=(Ij+B+1) mod 2v for each j.  */
    var k = Math.ceil(s / v) + Math.ceil(p / v);
    var Inew = new forge$7.util.ByteBuffer();
    for(j = 0; j < k; j++) {
      var chunk = new forge$7.util.ByteBuffer(I.getBytes(v));
      var x = 0x1ff;
      for(l = B.length() - 1; l >= 0; l--) {
        x = x >> 8;
        x += B.at(l) + chunk.at(l);
        chunk.setAt(l, x & 0xff);
      }
      Inew.putBuffer(chunk);
    }
    I = Inew;

    /* Add Ai to A. */
    result.putBuffer(buf);
  }

  result.truncate(result.length() - n);
  return result;
};

/**
 * Get new Forge cipher object instance.
 *
 * @param oid the OID (in string notation).
 * @param params the ASN.1 params object.
 * @param password the password to decrypt with.
 *
 * @return new cipher object instance.
 */
pki$3.pbe.getCipher = function(oid, params, password) {
  switch(oid) {
  case pki$3.oids['pkcs5PBES2']:
    return pki$3.pbe.getCipherForPBES2(oid, params, password);

  case pki$3.oids['pbeWithSHAAnd3-KeyTripleDES-CBC']:
  case pki$3.oids['pbewithSHAAnd40BitRC2-CBC']:
    return pki$3.pbe.getCipherForPKCS12PBE(oid, params, password);

  default:
    var error = new Error('Cannot read encrypted PBE data block. Unsupported OID.');
    error.oid = oid;
    error.supportedOids = [
      'pkcs5PBES2',
      'pbeWithSHAAnd3-KeyTripleDES-CBC',
      'pbewithSHAAnd40BitRC2-CBC'
    ];
    throw error;
  }
};

/**
 * Get new Forge cipher object instance according to PBES2 params block.
 *
 * The returned cipher instance is already started using the IV
 * from PBES2 parameter block.
 *
 * @param oid the PKCS#5 PBKDF2 OID (in string notation).
 * @param params the ASN.1 PBES2-params object.
 * @param password the password to decrypt with.
 *
 * @return new cipher object instance.
 */
pki$3.pbe.getCipherForPBES2 = function(oid, params, password) {
  // get PBE params
  var capture = {};
  var errors = [];
  if(!asn1$4.validate(params, PBES2AlgorithmsValidator, capture, errors)) {
    var error = new Error('Cannot read password-based-encryption algorithm ' +
      'parameters. ASN.1 object is not a supported EncryptedPrivateKeyInfo.');
    error.errors = errors;
    throw error;
  }

  // check oids
  oid = asn1$4.derToOid(capture.kdfOid);
  if(oid !== pki$3.oids['pkcs5PBKDF2']) {
    var error = new Error('Cannot read encrypted private key. ' +
      'Unsupported key derivation function OID.');
    error.oid = oid;
    error.supportedOids = ['pkcs5PBKDF2'];
    throw error;
  }
  oid = asn1$4.derToOid(capture.encOid);
  if(oid !== pki$3.oids['aes128-CBC'] &&
    oid !== pki$3.oids['aes192-CBC'] &&
    oid !== pki$3.oids['aes256-CBC'] &&
    oid !== pki$3.oids['des-EDE3-CBC'] &&
    oid !== pki$3.oids['desCBC']) {
    var error = new Error('Cannot read encrypted private key. ' +
      'Unsupported encryption scheme OID.');
    error.oid = oid;
    error.supportedOids = [
      'aes128-CBC', 'aes192-CBC', 'aes256-CBC', 'des-EDE3-CBC', 'desCBC'];
    throw error;
  }

  // set PBE params
  var salt = capture.kdfSalt;
  var count = forge$7.util.createBuffer(capture.kdfIterationCount);
  count = count.getInt(count.length() << 3);
  var dkLen;
  var cipherFn;
  switch(pki$3.oids[oid]) {
  case 'aes128-CBC':
    dkLen = 16;
    cipherFn = forge$7.aes.createDecryptionCipher;
    break;
  case 'aes192-CBC':
    dkLen = 24;
    cipherFn = forge$7.aes.createDecryptionCipher;
    break;
  case 'aes256-CBC':
    dkLen = 32;
    cipherFn = forge$7.aes.createDecryptionCipher;
    break;
  case 'des-EDE3-CBC':
    dkLen = 24;
    cipherFn = forge$7.des.createDecryptionCipher;
    break;
  case 'desCBC':
    dkLen = 8;
    cipherFn = forge$7.des.createDecryptionCipher;
    break;
  }

  // get PRF message digest
  var md = prfOidToMessageDigest(capture.prfOid);

  // decrypt private key using pbe with chosen PRF and AES/DES
  var dk = forge$7.pkcs5.pbkdf2(password, salt, count, dkLen, md);
  var iv = capture.encIv;
  var cipher = cipherFn(dk);
  cipher.start(iv);

  return cipher;
};

/**
 * Get new Forge cipher object instance for PKCS#12 PBE.
 *
 * The returned cipher instance is already started using the key & IV
 * derived from the provided password and PKCS#12 PBE salt.
 *
 * @param oid The PKCS#12 PBE OID (in string notation).
 * @param params The ASN.1 PKCS#12 PBE-params object.
 * @param password The password to decrypt with.
 *
 * @return the new cipher object instance.
 */
pki$3.pbe.getCipherForPKCS12PBE = function(oid, params, password) {
  // get PBE params
  var capture = {};
  var errors = [];
  if(!asn1$4.validate(params, pkcs12PbeParamsValidator, capture, errors)) {
    var error = new Error('Cannot read password-based-encryption algorithm ' +
      'parameters. ASN.1 object is not a supported EncryptedPrivateKeyInfo.');
    error.errors = errors;
    throw error;
  }

  var salt = forge$7.util.createBuffer(capture.salt);
  var count = forge$7.util.createBuffer(capture.iterations);
  count = count.getInt(count.length() << 3);

  var dkLen, dIvLen, cipherFn;
  switch(oid) {
    case pki$3.oids['pbeWithSHAAnd3-KeyTripleDES-CBC']:
      dkLen = 24;
      dIvLen = 8;
      cipherFn = forge$7.des.startDecrypting;
      break;

    case pki$3.oids['pbewithSHAAnd40BitRC2-CBC']:
      dkLen = 5;
      dIvLen = 8;
      cipherFn = function(key, iv) {
        var cipher = forge$7.rc2.createDecryptionCipher(key, 40);
        cipher.start(iv, null);
        return cipher;
      };
      break;

    default:
      var error = new Error('Cannot read PKCS #12 PBE data block. Unsupported OID.');
      error.oid = oid;
      throw error;
  }

  // get PRF message digest
  var md = prfOidToMessageDigest(capture.prfOid);
  var key = pki$3.pbe.generatePkcs12Key(password, salt, 1, count, dkLen, md);
  md.start();
  var iv = pki$3.pbe.generatePkcs12Key(password, salt, 2, count, dIvLen, md);

  return cipherFn(key, iv);
};

/**
 * OpenSSL's legacy key derivation function.
 *
 * See: http://www.openssl.org/docs/crypto/EVP_BytesToKey.html
 *
 * @param password the password to derive the key from.
 * @param salt the salt to use, null for none.
 * @param dkLen the number of bytes needed for the derived key.
 * @param [options] the options to use:
 *          [md] an optional message digest object to use.
 */
pki$3.pbe.opensslDeriveBytes = function(password, salt, dkLen, md) {
  if(typeof md === 'undefined' || md === null) {
    if(!('md5' in forge$7.md)) {
      throw new Error('"md5" hash algorithm unavailable.');
    }
    md = forge$7.md.md5.create();
  }
  if(salt === null) {
    salt = '';
  }
  var digests = [hash(md, password + salt)];
  for(var length = 16, i = 1; length < dkLen; ++i, length += 16) {
    digests.push(hash(md, digests[i - 1] + password + salt));
  }
  return digests.join('').substr(0, dkLen);
};

function hash(md, bytes) {
  return md.start().update(bytes).digest().getBytes();
}

function prfOidToMessageDigest(prfOid) {
  // get PRF algorithm, default to SHA-1
  var prfAlgorithm;
  if(!prfOid) {
    prfAlgorithm = 'hmacWithSHA1';
  } else {
    prfAlgorithm = pki$3.oids[asn1$4.derToOid(prfOid)];
    if(!prfAlgorithm) {
      var error = new Error('Unsupported PRF OID.');
      error.oid = prfOid;
      error.supported = [
        'hmacWithSHA1', 'hmacWithSHA224', 'hmacWithSHA256', 'hmacWithSHA384',
        'hmacWithSHA512'];
      throw error;
    }
  }
  return prfAlgorithmToMessageDigest(prfAlgorithm);
}

function prfAlgorithmToMessageDigest(prfAlgorithm) {
  var factory = forge$7.md;
  switch(prfAlgorithm) {
  case 'hmacWithSHA224':
    factory = forge$7.md.sha512;
  case 'hmacWithSHA1':
  case 'hmacWithSHA256':
  case 'hmacWithSHA384':
  case 'hmacWithSHA512':
    prfAlgorithm = prfAlgorithm.substr(8).toLowerCase();
    break;
  default:
    var error = new Error('Unsupported PRF algorithm.');
    error.algorithm = prfAlgorithm;
    error.supported = [
      'hmacWithSHA1', 'hmacWithSHA224', 'hmacWithSHA256', 'hmacWithSHA384',
      'hmacWithSHA512'];
    throw error;
  }
  if(!factory || !(prfAlgorithm in factory)) {
    throw new Error('Unknown hash algorithm: ' + prfAlgorithm);
  }
  return factory[prfAlgorithm].create();
}

function createPbkdf2Params(salt, countBytes, dkLen, prfAlgorithm) {
  var params = asn1$4.create(asn1$4.Class.UNIVERSAL, asn1$4.Type.SEQUENCE, true, [
    // salt
    asn1$4.create(
      asn1$4.Class.UNIVERSAL, asn1$4.Type.OCTETSTRING, false, salt),
    // iteration count
    asn1$4.create(asn1$4.Class.UNIVERSAL, asn1$4.Type.INTEGER, false,
      countBytes.getBytes())
  ]);
  // when PRF algorithm is not SHA-1 default, add key length and PRF algorithm
  if(prfAlgorithm !== 'hmacWithSHA1') {
    params.value.push(
      // key length
      asn1$4.create(asn1$4.Class.UNIVERSAL, asn1$4.Type.INTEGER, false,
        forge$7.util.hexToBytes(dkLen.toString(16))),
      // AlgorithmIdentifier
      asn1$4.create(asn1$4.Class.UNIVERSAL, asn1$4.Type.SEQUENCE, true, [
        // algorithm
        asn1$4.create(asn1$4.Class.UNIVERSAL, asn1$4.Type.OID, false,
          asn1$4.oidToDer(pki$3.oids[prfAlgorithm]).getBytes()),
        // parameters (null)
        asn1$4.create(asn1$4.Class.UNIVERSAL, asn1$4.Type.NULL, false, '')
      ]));
  }
  return params;
}

/**
 * Javascript implementation of ASN.1 validators for PKCS#7 v1.5.
 *
 * @author Dave Longley
 * @author Stefan Siegl
 *
 * Copyright (c) 2012-2015 Digital Bazaar, Inc.
 * Copyright (c) 2012 Stefan Siegl <stesie@brokenpipe.de>
 *
 * The ASN.1 representation of PKCS#7 is as follows
 * (see RFC #2315 for details, http://www.ietf.org/rfc/rfc2315.txt):
 *
 * A PKCS#7 message consists of a ContentInfo on root level, which may
 * contain any number of further ContentInfo nested into it.
 *
 * ContentInfo ::= SEQUENCE {
 *   contentType                ContentType,
 *   content               [0]  EXPLICIT ANY DEFINED BY contentType OPTIONAL
 * }
 *
 * ContentType ::= OBJECT IDENTIFIER
 *
 * EnvelopedData ::= SEQUENCE {
 *   version                    Version,
 *   recipientInfos             RecipientInfos,
 *   encryptedContentInfo       EncryptedContentInfo
 * }
 *
 * EncryptedData ::= SEQUENCE {
 *   version                    Version,
 *   encryptedContentInfo       EncryptedContentInfo
 * }
 *
 * id-signedData OBJECT IDENTIFIER ::= { iso(1) member-body(2)
 *   us(840) rsadsi(113549) pkcs(1) pkcs7(7) 2 }
 *
 * SignedData ::= SEQUENCE {
 *   version           INTEGER,
 *   digestAlgorithms  DigestAlgorithmIdentifiers,
 *   contentInfo       ContentInfo,
 *   certificates      [0] IMPLICIT Certificates OPTIONAL,
 *   crls              [1] IMPLICIT CertificateRevocationLists OPTIONAL,
 *   signerInfos       SignerInfos
 * }
 *
 * SignerInfos ::= SET OF SignerInfo
 *
 * SignerInfo ::= SEQUENCE {
 *   version                    Version,
 *   issuerAndSerialNumber      IssuerAndSerialNumber,
 *   digestAlgorithm            DigestAlgorithmIdentifier,
 *   authenticatedAttributes    [0] IMPLICIT Attributes OPTIONAL,
 *   digestEncryptionAlgorithm  DigestEncryptionAlgorithmIdentifier,
 *   encryptedDigest            EncryptedDigest,
 *   unauthenticatedAttributes  [1] IMPLICIT Attributes OPTIONAL
 * }
 *
 * EncryptedDigest ::= OCTET STRING
 *
 * Attributes ::= SET OF Attribute
 *
 * Attribute ::= SEQUENCE {
 *   attrType    OBJECT IDENTIFIER,
 *   attrValues  SET OF AttributeValue
 * }
 *
 * AttributeValue ::= ANY
 *
 * Version ::= INTEGER
 *
 * RecipientInfos ::= SET OF RecipientInfo
 *
 * EncryptedContentInfo ::= SEQUENCE {
 *   contentType                 ContentType,
 *   contentEncryptionAlgorithm  ContentEncryptionAlgorithmIdentifier,
 *   encryptedContent       [0]  IMPLICIT EncryptedContent OPTIONAL
 * }
 *
 * ContentEncryptionAlgorithmIdentifier ::= AlgorithmIdentifier
 *
 * The AlgorithmIdentifier contains an Object Identifier (OID) and parameters
 * for the algorithm, if any. In the case of AES and DES3, there is only one,
 * the IV.
 *
 * AlgorithmIdentifer ::= SEQUENCE {
 *    algorithm OBJECT IDENTIFIER,
 *    parameters ANY DEFINED BY algorithm OPTIONAL
 * }
 *
 * EncryptedContent ::= OCTET STRING
 *
 * RecipientInfo ::= SEQUENCE {
 *   version                     Version,
 *   issuerAndSerialNumber       IssuerAndSerialNumber,
 *   keyEncryptionAlgorithm      KeyEncryptionAlgorithmIdentifier,
 *   encryptedKey                EncryptedKey
 * }
 *
 * IssuerAndSerialNumber ::= SEQUENCE {
 *   issuer                      Name,
 *   serialNumber                CertificateSerialNumber
 * }
 *
 * CertificateSerialNumber ::= INTEGER
 *
 * KeyEncryptionAlgorithmIdentifier ::= AlgorithmIdentifier
 *
 * EncryptedKey ::= OCTET STRING
 */

var forge$6 = forge$s;



// shortcut for ASN.1 API
var asn1$3 = forge$6.asn1;

// shortcut for PKCS#7 API
var p7v = forge$6.pkcs7asn1 = forge$6.pkcs7asn1 || {};
forge$6.pkcs7 = forge$6.pkcs7 || {};
forge$6.pkcs7.asn1 = p7v;

var contentInfoValidator$1 = {
  name: 'ContentInfo',
  tagClass: asn1$3.Class.UNIVERSAL,
  type: asn1$3.Type.SEQUENCE,
  constructed: true,
  value: [{
    name: 'ContentInfo.ContentType',
    tagClass: asn1$3.Class.UNIVERSAL,
    type: asn1$3.Type.OID,
    constructed: false,
    capture: 'contentType'
  }, {
    name: 'ContentInfo.content',
    tagClass: asn1$3.Class.CONTEXT_SPECIFIC,
    type: 0,
    constructed: true,
    optional: true,
    captureAsn1: 'content'
  }]
};
p7v.contentInfoValidator = contentInfoValidator$1;

var encryptedContentInfoValidator = {
  name: 'EncryptedContentInfo',
  tagClass: asn1$3.Class.UNIVERSAL,
  type: asn1$3.Type.SEQUENCE,
  constructed: true,
  value: [{
    name: 'EncryptedContentInfo.contentType',
    tagClass: asn1$3.Class.UNIVERSAL,
    type: asn1$3.Type.OID,
    constructed: false,
    capture: 'contentType'
  }, {
    name: 'EncryptedContentInfo.contentEncryptionAlgorithm',
    tagClass: asn1$3.Class.UNIVERSAL,
    type: asn1$3.Type.SEQUENCE,
    constructed: true,
    value: [{
      name: 'EncryptedContentInfo.contentEncryptionAlgorithm.algorithm',
      tagClass: asn1$3.Class.UNIVERSAL,
      type: asn1$3.Type.OID,
      constructed: false,
      capture: 'encAlgorithm'
    }, {
      name: 'EncryptedContentInfo.contentEncryptionAlgorithm.parameter',
      tagClass: asn1$3.Class.UNIVERSAL,
      captureAsn1: 'encParameter'
    }]
  }, {
    name: 'EncryptedContentInfo.encryptedContent',
    tagClass: asn1$3.Class.CONTEXT_SPECIFIC,
    type: 0,
    /* The PKCS#7 structure output by OpenSSL somewhat differs from what
     * other implementations do generate.
     *
     * OpenSSL generates a structure like this:
     * SEQUENCE {
     *    ...
     *    [0]
     *       26 DA 67 D2 17 9C 45 3C B1 2A A8 59 2F 29 33 38
     *       C3 C3 DF 86 71 74 7A 19 9F 40 D0 29 BE 85 90 45
     *       ...
     * }
     *
     * Whereas other implementations (and this PKCS#7 module) generate:
     * SEQUENCE {
     *    ...
     *    [0] {
     *       OCTET STRING
     *          26 DA 67 D2 17 9C 45 3C B1 2A A8 59 2F 29 33 38
     *          C3 C3 DF 86 71 74 7A 19 9F 40 D0 29 BE 85 90 45
     *          ...
     *    }
     * }
     *
     * In order to support both, we just capture the context specific
     * field here.  The OCTET STRING bit is removed below.
     */
    capture: 'encryptedContent',
    captureAsn1: 'encryptedContentAsn1'
  }]
};

p7v.envelopedDataValidator = {
  name: 'EnvelopedData',
  tagClass: asn1$3.Class.UNIVERSAL,
  type: asn1$3.Type.SEQUENCE,
  constructed: true,
  value: [{
    name: 'EnvelopedData.Version',
    tagClass: asn1$3.Class.UNIVERSAL,
    type: asn1$3.Type.INTEGER,
    constructed: false,
    capture: 'version'
  }, {
    name: 'EnvelopedData.RecipientInfos',
    tagClass: asn1$3.Class.UNIVERSAL,
    type: asn1$3.Type.SET,
    constructed: true,
    captureAsn1: 'recipientInfos'
  }].concat(encryptedContentInfoValidator)
};

p7v.encryptedDataValidator = {
  name: 'EncryptedData',
  tagClass: asn1$3.Class.UNIVERSAL,
  type: asn1$3.Type.SEQUENCE,
  constructed: true,
  value: [{
    name: 'EncryptedData.Version',
    tagClass: asn1$3.Class.UNIVERSAL,
    type: asn1$3.Type.INTEGER,
    constructed: false,
    capture: 'version'
  }].concat(encryptedContentInfoValidator)
};

var signerValidator = {
  name: 'SignerInfo',
  tagClass: asn1$3.Class.UNIVERSAL,
  type: asn1$3.Type.SEQUENCE,
  constructed: true,
  value: [{
    name: 'SignerInfo.version',
    tagClass: asn1$3.Class.UNIVERSAL,
    type: asn1$3.Type.INTEGER,
    constructed: false
  }, {
    name: 'SignerInfo.issuerAndSerialNumber',
    tagClass: asn1$3.Class.UNIVERSAL,
    type: asn1$3.Type.SEQUENCE,
    constructed: true,
    value: [{
      name: 'SignerInfo.issuerAndSerialNumber.issuer',
      tagClass: asn1$3.Class.UNIVERSAL,
      type: asn1$3.Type.SEQUENCE,
      constructed: true,
      captureAsn1: 'issuer'
    }, {
      name: 'SignerInfo.issuerAndSerialNumber.serialNumber',
      tagClass: asn1$3.Class.UNIVERSAL,
      type: asn1$3.Type.INTEGER,
      constructed: false,
      capture: 'serial'
    }]
  }, {
    name: 'SignerInfo.digestAlgorithm',
    tagClass: asn1$3.Class.UNIVERSAL,
    type: asn1$3.Type.SEQUENCE,
    constructed: true,
    value: [{
      name: 'SignerInfo.digestAlgorithm.algorithm',
      tagClass: asn1$3.Class.UNIVERSAL,
      type: asn1$3.Type.OID,
      constructed: false,
      capture: 'digestAlgorithm'
    }, {
      name: 'SignerInfo.digestAlgorithm.parameter',
      tagClass: asn1$3.Class.UNIVERSAL,
      constructed: false,
      captureAsn1: 'digestParameter',
      optional: true
    }]
  }, {
    name: 'SignerInfo.authenticatedAttributes',
    tagClass: asn1$3.Class.CONTEXT_SPECIFIC,
    type: 0,
    constructed: true,
    optional: true,
    capture: 'authenticatedAttributes'
  }, {
    name: 'SignerInfo.digestEncryptionAlgorithm',
    tagClass: asn1$3.Class.UNIVERSAL,
    type: asn1$3.Type.SEQUENCE,
    constructed: true,
    capture: 'signatureAlgorithm'
  }, {
    name: 'SignerInfo.encryptedDigest',
    tagClass: asn1$3.Class.UNIVERSAL,
    type: asn1$3.Type.OCTETSTRING,
    constructed: false,
    capture: 'signature'
  }, {
    name: 'SignerInfo.unauthenticatedAttributes',
    tagClass: asn1$3.Class.CONTEXT_SPECIFIC,
    type: 1,
    constructed: true,
    optional: true,
    capture: 'unauthenticatedAttributes'
  }]
};

p7v.signedDataValidator = {
  name: 'SignedData',
  tagClass: asn1$3.Class.UNIVERSAL,
  type: asn1$3.Type.SEQUENCE,
  constructed: true,
  value: [{
    name: 'SignedData.Version',
    tagClass: asn1$3.Class.UNIVERSAL,
    type: asn1$3.Type.INTEGER,
    constructed: false,
    capture: 'version'
  }, {
    name: 'SignedData.DigestAlgorithms',
    tagClass: asn1$3.Class.UNIVERSAL,
    type: asn1$3.Type.SET,
    constructed: true,
    captureAsn1: 'digestAlgorithms'
  },
  contentInfoValidator$1,
  {
    name: 'SignedData.Certificates',
    tagClass: asn1$3.Class.CONTEXT_SPECIFIC,
    type: 0,
    optional: true,
    captureAsn1: 'certificates'
  }, {
    name: 'SignedData.CertificateRevocationLists',
    tagClass: asn1$3.Class.CONTEXT_SPECIFIC,
    type: 1,
    optional: true,
    captureAsn1: 'crls'
  }, {
    name: 'SignedData.SignerInfos',
    tagClass: asn1$3.Class.UNIVERSAL,
    type: asn1$3.Type.SET,
    capture: 'signerInfos',
    optional: true,
    value: [signerValidator]
  }]
};

p7v.recipientInfoValidator = {
  name: 'RecipientInfo',
  tagClass: asn1$3.Class.UNIVERSAL,
  type: asn1$3.Type.SEQUENCE,
  constructed: true,
  value: [{
    name: 'RecipientInfo.version',
    tagClass: asn1$3.Class.UNIVERSAL,
    type: asn1$3.Type.INTEGER,
    constructed: false,
    capture: 'version'
  }, {
    name: 'RecipientInfo.issuerAndSerial',
    tagClass: asn1$3.Class.UNIVERSAL,
    type: asn1$3.Type.SEQUENCE,
    constructed: true,
    value: [{
      name: 'RecipientInfo.issuerAndSerial.issuer',
      tagClass: asn1$3.Class.UNIVERSAL,
      type: asn1$3.Type.SEQUENCE,
      constructed: true,
      captureAsn1: 'issuer'
    }, {
      name: 'RecipientInfo.issuerAndSerial.serialNumber',
      tagClass: asn1$3.Class.UNIVERSAL,
      type: asn1$3.Type.INTEGER,
      constructed: false,
      capture: 'serial'
    }]
  }, {
    name: 'RecipientInfo.keyEncryptionAlgorithm',
    tagClass: asn1$3.Class.UNIVERSAL,
    type: asn1$3.Type.SEQUENCE,
    constructed: true,
    value: [{
      name: 'RecipientInfo.keyEncryptionAlgorithm.algorithm',
      tagClass: asn1$3.Class.UNIVERSAL,
      type: asn1$3.Type.OID,
      constructed: false,
      capture: 'encAlgorithm'
    }, {
      name: 'RecipientInfo.keyEncryptionAlgorithm.parameter',
      tagClass: asn1$3.Class.UNIVERSAL,
      constructed: false,
      captureAsn1: 'encParameter',
      optional: true
    }]
  }, {
    name: 'RecipientInfo.encryptedKey',
    tagClass: asn1$3.Class.UNIVERSAL,
    type: asn1$3.Type.OCTETSTRING,
    constructed: false,
    capture: 'encKey'
  }]
};

/**
 * Javascript implementation of mask generation function MGF1.
 *
 * @author Stefan Siegl
 * @author Dave Longley
 *
 * Copyright (c) 2012 Stefan Siegl <stesie@brokenpipe.de>
 * Copyright (c) 2014 Digital Bazaar, Inc.
 */

var forge$5 = forge$s;


forge$5.mgf = forge$5.mgf || {};
var mgf1 = forge$5.mgf.mgf1 = forge$5.mgf1 = forge$5.mgf1 || {};

/**
 * Creates a MGF1 mask generation function object.
 *
 * @param md the message digest API to use (eg: forge.md.sha1.create()).
 *
 * @return a mask generation function object.
 */
mgf1.create = function(md) {
  var mgf = {
    /**
     * Generate mask of specified length.
     *
     * @param {String} seed The seed for mask generation.
     * @param maskLen Number of bytes to generate.
     * @return {String} The generated mask.
     */
    generate: function(seed, maskLen) {
      /* 2. Let T be the empty octet string. */
      var t = new forge$5.util.ByteBuffer();

      /* 3. For counter from 0 to ceil(maskLen / hLen), do the following: */
      var len = Math.ceil(maskLen / md.digestLength);
      for(var i = 0; i < len; i++) {
        /* a. Convert counter to an octet string C of length 4 octets */
        var c = new forge$5.util.ByteBuffer();
        c.putInt32(i);

        /* b. Concatenate the hash of the seed mgfSeed and C to the octet
         * string T: */
        md.start();
        md.update(seed + c.getBytes());
        t.putBuffer(md.digest());
      }

      /* Output the leading maskLen octets of T as the octet string mask. */
      t.truncate(t.length() - maskLen);
      return t.getBytes();
    }
  };

  return mgf;
};

/**
 * Node.js module for Forge mask generation functions.
 *
 * @author Stefan Siegl
 *
 * Copyright 2012 Stefan Siegl <stesie@brokenpipe.de>
 */

var forge$4 = forge$s;


forge$4.mgf = forge$4.mgf || {};
forge$4.mgf.mgf1 = forge$4.mgf1;

/**
 * Javascript implementation of PKCS#1 PSS signature padding.
 *
 * @author Stefan Siegl
 *
 * Copyright (c) 2012 Stefan Siegl <stesie@brokenpipe.de>
 */

var forge$3 = forge$s;



// shortcut for PSS API
var pss = forge$3.pss = forge$3.pss || {};

/**
 * Creates a PSS signature scheme object.
 *
 * There are several ways to provide a salt for encoding:
 *
 * 1. Specify the saltLength only and the built-in PRNG will generate it.
 * 2. Specify the saltLength and a custom PRNG with 'getBytesSync' defined that
 *   will be used.
 * 3. Specify the salt itself as a forge.util.ByteBuffer.
 *
 * @param options the options to use:
 *          md the message digest object to use, a forge md instance.
 *          mgf the mask generation function to use, a forge mgf instance.
 *          [saltLength] the length of the salt in octets.
 *          [prng] the pseudo-random number generator to use to produce a salt.
 *          [salt] the salt to use when encoding.
 *
 * @return a signature scheme object.
 */
pss.create = function(options) {
  // backwards compatibility w/legacy args: hash, mgf, sLen
  if(arguments.length === 3) {
    options = {
      md: arguments[0],
      mgf: arguments[1],
      saltLength: arguments[2]
    };
  }

  var hash = options.md;
  var mgf = options.mgf;
  var hLen = hash.digestLength;

  var salt_ = options.salt || null;
  if(typeof salt_ === 'string') {
    // assume binary-encoded string
    salt_ = forge$3.util.createBuffer(salt_);
  }

  var sLen;
  if('saltLength' in options) {
    sLen = options.saltLength;
  } else if(salt_ !== null) {
    sLen = salt_.length();
  } else {
    throw new Error('Salt length not specified or specific salt not given.');
  }

  if(salt_ !== null && salt_.length() !== sLen) {
    throw new Error('Given salt length does not match length of given salt.');
  }

  var prng = options.prng || forge$3.random;

  var pssobj = {};

  /**
   * Encodes a PSS signature.
   *
   * This function implements EMSA-PSS-ENCODE as per RFC 3447, section 9.1.1.
   *
   * @param md the message digest object with the hash to sign.
   * @param modsBits the length of the RSA modulus in bits.
   *
   * @return the encoded message as a binary-encoded string of length
   *           ceil((modBits - 1) / 8).
   */
  pssobj.encode = function(md, modBits) {
    var i;
    var emBits = modBits - 1;
    var emLen = Math.ceil(emBits / 8);

    /* 2. Let mHash = Hash(M), an octet string of length hLen. */
    var mHash = md.digest().getBytes();

    /* 3. If emLen < hLen + sLen + 2, output "encoding error" and stop. */
    if(emLen < hLen + sLen + 2) {
      throw new Error('Message is too long to encrypt.');
    }

    /* 4. Generate a random octet string salt of length sLen; if sLen = 0,
     *    then salt is the empty string. */
    var salt;
    if(salt_ === null) {
      salt = prng.getBytesSync(sLen);
    } else {
      salt = salt_.bytes();
    }

    /* 5. Let M' = (0x)00 00 00 00 00 00 00 00 || mHash || salt; */
    var m_ = new forge$3.util.ByteBuffer();
    m_.fillWithByte(0, 8);
    m_.putBytes(mHash);
    m_.putBytes(salt);

    /* 6. Let H = Hash(M'), an octet string of length hLen. */
    hash.start();
    hash.update(m_.getBytes());
    var h = hash.digest().getBytes();

    /* 7. Generate an octet string PS consisting of emLen - sLen - hLen - 2
     *    zero octets.  The length of PS may be 0. */
    var ps = new forge$3.util.ByteBuffer();
    ps.fillWithByte(0, emLen - sLen - hLen - 2);

    /* 8. Let DB = PS || 0x01 || salt; DB is an octet string of length
     *    emLen - hLen - 1. */
    ps.putByte(0x01);
    ps.putBytes(salt);
    var db = ps.getBytes();

    /* 9. Let dbMask = MGF(H, emLen - hLen - 1). */
    var maskLen = emLen - hLen - 1;
    var dbMask = mgf.generate(h, maskLen);

    /* 10. Let maskedDB = DB \xor dbMask. */
    var maskedDB = '';
    for(i = 0; i < maskLen; i++) {
      maskedDB += String.fromCharCode(db.charCodeAt(i) ^ dbMask.charCodeAt(i));
    }

    /* 11. Set the leftmost 8emLen - emBits bits of the leftmost octet in
     *     maskedDB to zero. */
    var mask = (0xFF00 >> (8 * emLen - emBits)) & 0xFF;
    maskedDB = String.fromCharCode(maskedDB.charCodeAt(0) & ~mask) +
      maskedDB.substr(1);

    /* 12. Let EM = maskedDB || H || 0xbc.
     * 13. Output EM. */
    return maskedDB + h + String.fromCharCode(0xbc);
  };

  /**
   * Verifies a PSS signature.
   *
   * This function implements EMSA-PSS-VERIFY as per RFC 3447, section 9.1.2.
   *
   * @param mHash the message digest hash, as a binary-encoded string, to
   *         compare against the signature.
   * @param em the encoded message, as a binary-encoded string
   *          (RSA decryption result).
   * @param modsBits the length of the RSA modulus in bits.
   *
   * @return true if the signature was verified, false if not.
   */
  pssobj.verify = function(mHash, em, modBits) {
    var i;
    var emBits = modBits - 1;
    var emLen = Math.ceil(emBits / 8);

    /* c. Convert the message representative m to an encoded message EM
     *    of length emLen = ceil((modBits - 1) / 8) octets, where modBits
     *    is the length in bits of the RSA modulus n */
    em = em.substr(-emLen);

    /* 3. If emLen < hLen + sLen + 2, output "inconsistent" and stop. */
    if(emLen < hLen + sLen + 2) {
      throw new Error('Inconsistent parameters to PSS signature verification.');
    }

    /* 4. If the rightmost octet of EM does not have hexadecimal value
     *    0xbc, output "inconsistent" and stop. */
    if(em.charCodeAt(emLen - 1) !== 0xbc) {
      throw new Error('Encoded message does not end in 0xBC.');
    }

    /* 5. Let maskedDB be the leftmost emLen - hLen - 1 octets of EM, and
     *    let H be the next hLen octets. */
    var maskLen = emLen - hLen - 1;
    var maskedDB = em.substr(0, maskLen);
    var h = em.substr(maskLen, hLen);

    /* 6. If the leftmost 8emLen - emBits bits of the leftmost octet in
     *    maskedDB are not all equal to zero, output "inconsistent" and stop. */
    var mask = (0xFF00 >> (8 * emLen - emBits)) & 0xFF;
    if((maskedDB.charCodeAt(0) & mask) !== 0) {
      throw new Error('Bits beyond keysize not zero as expected.');
    }

    /* 7. Let dbMask = MGF(H, emLen - hLen - 1). */
    var dbMask = mgf.generate(h, maskLen);

    /* 8. Let DB = maskedDB \xor dbMask. */
    var db = '';
    for(i = 0; i < maskLen; i++) {
      db += String.fromCharCode(maskedDB.charCodeAt(i) ^ dbMask.charCodeAt(i));
    }

    /* 9. Set the leftmost 8emLen - emBits bits of the leftmost octet
     * in DB to zero. */
    db = String.fromCharCode(db.charCodeAt(0) & ~mask) + db.substr(1);

    /* 10. If the emLen - hLen - sLen - 2 leftmost octets of DB are not zero
     * or if the octet at position emLen - hLen - sLen - 1 (the leftmost
     * position is "position 1") does not have hexadecimal value 0x01,
     * output "inconsistent" and stop. */
    var checkLen = emLen - hLen - sLen - 2;
    for(i = 0; i < checkLen; i++) {
      if(db.charCodeAt(i) !== 0x00) {
        throw new Error('Leftmost octets not zero as expected');
      }
    }

    if(db.charCodeAt(checkLen) !== 0x01) {
      throw new Error('Inconsistent PSS signature, 0x01 marker not found');
    }

    /* 11. Let salt be the last sLen octets of DB. */
    var salt = db.substr(-sLen);

    /* 12.  Let M' = (0x)00 00 00 00 00 00 00 00 || mHash || salt */
    var m_ = new forge$3.util.ByteBuffer();
    m_.fillWithByte(0, 8);
    m_.putBytes(mHash);
    m_.putBytes(salt);

    /* 13. Let H' = Hash(M'), an octet string of length hLen. */
    hash.start();
    hash.update(m_.getBytes());
    var h_ = hash.digest().getBytes();

    /* 14. If H = H', output "consistent." Otherwise, output "inconsistent." */
    return h === h_;
  };

  return pssobj;
};

/**
 * Javascript implementation of X.509 and related components (such as
 * Certification Signing Requests) of a Public Key Infrastructure.
 *
 * @author Dave Longley
 *
 * Copyright (c) 2010-2014 Digital Bazaar, Inc.
 *
 * The ASN.1 representation of an X.509v3 certificate is as follows
 * (see RFC 2459):
 *
 * Certificate ::= SEQUENCE {
 *   tbsCertificate       TBSCertificate,
 *   signatureAlgorithm   AlgorithmIdentifier,
 *   signatureValue       BIT STRING
 * }
 *
 * TBSCertificate ::= SEQUENCE {
 *   version         [0]  EXPLICIT Version DEFAULT v1,
 *   serialNumber         CertificateSerialNumber,
 *   signature            AlgorithmIdentifier,
 *   issuer               Name,
 *   validity             Validity,
 *   subject              Name,
 *   subjectPublicKeyInfo SubjectPublicKeyInfo,
 *   issuerUniqueID  [1]  IMPLICIT UniqueIdentifier OPTIONAL,
 *                        -- If present, version shall be v2 or v3
 *   subjectUniqueID [2]  IMPLICIT UniqueIdentifier OPTIONAL,
 *                        -- If present, version shall be v2 or v3
 *   extensions      [3]  EXPLICIT Extensions OPTIONAL
 *                        -- If present, version shall be v3
 * }
 *
 * Version ::= INTEGER  { v1(0), v2(1), v3(2) }
 *
 * CertificateSerialNumber ::= INTEGER
 *
 * Name ::= CHOICE {
 *   // only one possible choice for now
 *   RDNSequence
 * }
 *
 * RDNSequence ::= SEQUENCE OF RelativeDistinguishedName
 *
 * RelativeDistinguishedName ::= SET OF AttributeTypeAndValue
 *
 * AttributeTypeAndValue ::= SEQUENCE {
 *   type     AttributeType,
 *   value    AttributeValue
 * }
 * AttributeType ::= OBJECT IDENTIFIER
 * AttributeValue ::= ANY DEFINED BY AttributeType
 *
 * Validity ::= SEQUENCE {
 *   notBefore      Time,
 *   notAfter       Time
 * }
 *
 * Time ::= CHOICE {
 *   utcTime        UTCTime,
 *   generalTime    GeneralizedTime
 * }
 *
 * UniqueIdentifier ::= BIT STRING
 *
 * SubjectPublicKeyInfo ::= SEQUENCE {
 *   algorithm            AlgorithmIdentifier,
 *   subjectPublicKey     BIT STRING
 * }
 *
 * Extensions ::= SEQUENCE SIZE (1..MAX) OF Extension
 *
 * Extension ::= SEQUENCE {
 *   extnID      OBJECT IDENTIFIER,
 *   critical    BOOLEAN DEFAULT FALSE,
 *   extnValue   OCTET STRING
 * }
 *
 * The only key algorithm currently supported for PKI is RSA.
 *
 * RSASSA-PSS signatures are described in RFC 3447 and RFC 4055.
 *
 * PKCS#10 v1.7 describes certificate signing requests:
 *
 * CertificationRequestInfo:
 *
 * CertificationRequestInfo ::= SEQUENCE {
 *   version       INTEGER { v1(0) } (v1,...),
 *   subject       Name,
 *   subjectPKInfo SubjectPublicKeyInfo{{ PKInfoAlgorithms }},
 *   attributes    [0] Attributes{{ CRIAttributes }}
 * }
 *
 * Attributes { ATTRIBUTE:IOSet } ::= SET OF Attribute{{ IOSet }}
 *
 * CRIAttributes  ATTRIBUTE  ::= {
 *   ... -- add any locally defined attributes here -- }
 *
 * Attribute { ATTRIBUTE:IOSet } ::= SEQUENCE {
 *   type   ATTRIBUTE.&id({IOSet}),
 *   values SET SIZE(1..MAX) OF ATTRIBUTE.&Type({IOSet}{@type})
 * }
 *
 * CertificationRequest ::= SEQUENCE {
 *   certificationRequestInfo CertificationRequestInfo,
 *   signatureAlgorithm AlgorithmIdentifier{{ SignatureAlgorithms }},
 *   signature          BIT STRING
 * }
 */

var forge$2 = forge$s;











// shortcut for asn.1 API
var asn1$2 = forge$2.asn1;

/* Public Key Infrastructure (PKI) implementation. */
var pki$2 = forge$2.pki = forge$2.pki || {};
var oids = pki$2.oids;

// short name OID mappings
var _shortNames = {};
_shortNames['CN'] = oids['commonName'];
_shortNames['commonName'] = 'CN';
_shortNames['C'] = oids['countryName'];
_shortNames['countryName'] = 'C';
_shortNames['L'] = oids['localityName'];
_shortNames['localityName'] = 'L';
_shortNames['ST'] = oids['stateOrProvinceName'];
_shortNames['stateOrProvinceName'] = 'ST';
_shortNames['O'] = oids['organizationName'];
_shortNames['organizationName'] = 'O';
_shortNames['OU'] = oids['organizationalUnitName'];
_shortNames['organizationalUnitName'] = 'OU';
_shortNames['E'] = oids['emailAddress'];
_shortNames['emailAddress'] = 'E';

// validator for an SubjectPublicKeyInfo structure
// Note: Currently only works with an RSA public key
var publicKeyValidator = forge$2.pki.rsa.publicKeyValidator;

// validator for an X.509v3 certificate
var x509CertificateValidator = {
  name: 'Certificate',
  tagClass: asn1$2.Class.UNIVERSAL,
  type: asn1$2.Type.SEQUENCE,
  constructed: true,
  value: [{
    name: 'Certificate.TBSCertificate',
    tagClass: asn1$2.Class.UNIVERSAL,
    type: asn1$2.Type.SEQUENCE,
    constructed: true,
    captureAsn1: 'tbsCertificate',
    value: [{
      name: 'Certificate.TBSCertificate.version',
      tagClass: asn1$2.Class.CONTEXT_SPECIFIC,
      type: 0,
      constructed: true,
      optional: true,
      value: [{
        name: 'Certificate.TBSCertificate.version.integer',
        tagClass: asn1$2.Class.UNIVERSAL,
        type: asn1$2.Type.INTEGER,
        constructed: false,
        capture: 'certVersion'
      }]
    }, {
      name: 'Certificate.TBSCertificate.serialNumber',
      tagClass: asn1$2.Class.UNIVERSAL,
      type: asn1$2.Type.INTEGER,
      constructed: false,
      capture: 'certSerialNumber'
    }, {
      name: 'Certificate.TBSCertificate.signature',
      tagClass: asn1$2.Class.UNIVERSAL,
      type: asn1$2.Type.SEQUENCE,
      constructed: true,
      value: [{
        name: 'Certificate.TBSCertificate.signature.algorithm',
        tagClass: asn1$2.Class.UNIVERSAL,
        type: asn1$2.Type.OID,
        constructed: false,
        capture: 'certinfoSignatureOid'
      }, {
        name: 'Certificate.TBSCertificate.signature.parameters',
        tagClass: asn1$2.Class.UNIVERSAL,
        optional: true,
        captureAsn1: 'certinfoSignatureParams'
      }]
    }, {
      name: 'Certificate.TBSCertificate.issuer',
      tagClass: asn1$2.Class.UNIVERSAL,
      type: asn1$2.Type.SEQUENCE,
      constructed: true,
      captureAsn1: 'certIssuer'
    }, {
      name: 'Certificate.TBSCertificate.validity',
      tagClass: asn1$2.Class.UNIVERSAL,
      type: asn1$2.Type.SEQUENCE,
      constructed: true,
      // Note: UTC and generalized times may both appear so the capture
      // names are based on their detected order, the names used below
      // are only for the common case, which validity time really means
      // "notBefore" and which means "notAfter" will be determined by order
      value: [{
        // notBefore (Time) (UTC time case)
        name: 'Certificate.TBSCertificate.validity.notBefore (utc)',
        tagClass: asn1$2.Class.UNIVERSAL,
        type: asn1$2.Type.UTCTIME,
        constructed: false,
        optional: true,
        capture: 'certValidity1UTCTime'
      }, {
        // notBefore (Time) (generalized time case)
        name: 'Certificate.TBSCertificate.validity.notBefore (generalized)',
        tagClass: asn1$2.Class.UNIVERSAL,
        type: asn1$2.Type.GENERALIZEDTIME,
        constructed: false,
        optional: true,
        capture: 'certValidity2GeneralizedTime'
      }, {
        // notAfter (Time) (only UTC time is supported)
        name: 'Certificate.TBSCertificate.validity.notAfter (utc)',
        tagClass: asn1$2.Class.UNIVERSAL,
        type: asn1$2.Type.UTCTIME,
        constructed: false,
        optional: true,
        capture: 'certValidity3UTCTime'
      }, {
        // notAfter (Time) (only UTC time is supported)
        name: 'Certificate.TBSCertificate.validity.notAfter (generalized)',
        tagClass: asn1$2.Class.UNIVERSAL,
        type: asn1$2.Type.GENERALIZEDTIME,
        constructed: false,
        optional: true,
        capture: 'certValidity4GeneralizedTime'
      }]
    }, {
      // Name (subject) (RDNSequence)
      name: 'Certificate.TBSCertificate.subject',
      tagClass: asn1$2.Class.UNIVERSAL,
      type: asn1$2.Type.SEQUENCE,
      constructed: true,
      captureAsn1: 'certSubject'
    },
    // SubjectPublicKeyInfo
    publicKeyValidator,
    {
      // issuerUniqueID (optional)
      name: 'Certificate.TBSCertificate.issuerUniqueID',
      tagClass: asn1$2.Class.CONTEXT_SPECIFIC,
      type: 1,
      constructed: true,
      optional: true,
      value: [{
        name: 'Certificate.TBSCertificate.issuerUniqueID.id',
        tagClass: asn1$2.Class.UNIVERSAL,
        type: asn1$2.Type.BITSTRING,
        constructed: false,
        // TODO: support arbitrary bit length ids
        captureBitStringValue: 'certIssuerUniqueId'
      }]
    }, {
      // subjectUniqueID (optional)
      name: 'Certificate.TBSCertificate.subjectUniqueID',
      tagClass: asn1$2.Class.CONTEXT_SPECIFIC,
      type: 2,
      constructed: true,
      optional: true,
      value: [{
        name: 'Certificate.TBSCertificate.subjectUniqueID.id',
        tagClass: asn1$2.Class.UNIVERSAL,
        type: asn1$2.Type.BITSTRING,
        constructed: false,
        // TODO: support arbitrary bit length ids
        captureBitStringValue: 'certSubjectUniqueId'
      }]
    }, {
      // Extensions (optional)
      name: 'Certificate.TBSCertificate.extensions',
      tagClass: asn1$2.Class.CONTEXT_SPECIFIC,
      type: 3,
      constructed: true,
      captureAsn1: 'certExtensions',
      optional: true
    }]
  }, {
    // AlgorithmIdentifier (signature algorithm)
    name: 'Certificate.signatureAlgorithm',
    tagClass: asn1$2.Class.UNIVERSAL,
    type: asn1$2.Type.SEQUENCE,
    constructed: true,
    value: [{
      // algorithm
      name: 'Certificate.signatureAlgorithm.algorithm',
      tagClass: asn1$2.Class.UNIVERSAL,
      type: asn1$2.Type.OID,
      constructed: false,
      capture: 'certSignatureOid'
    }, {
      name: 'Certificate.TBSCertificate.signature.parameters',
      tagClass: asn1$2.Class.UNIVERSAL,
      optional: true,
      captureAsn1: 'certSignatureParams'
    }]
  }, {
    // SignatureValue
    name: 'Certificate.signatureValue',
    tagClass: asn1$2.Class.UNIVERSAL,
    type: asn1$2.Type.BITSTRING,
    constructed: false,
    captureBitStringValue: 'certSignature'
  }]
};

var rsassaPssParameterValidator = {
  name: 'rsapss',
  tagClass: asn1$2.Class.UNIVERSAL,
  type: asn1$2.Type.SEQUENCE,
  constructed: true,
  value: [{
    name: 'rsapss.hashAlgorithm',
    tagClass: asn1$2.Class.CONTEXT_SPECIFIC,
    type: 0,
    constructed: true,
    value: [{
      name: 'rsapss.hashAlgorithm.AlgorithmIdentifier',
      tagClass: asn1$2.Class.UNIVERSAL,
      type: asn1$2.Class.SEQUENCE,
      constructed: true,
      optional: true,
      value: [{
        name: 'rsapss.hashAlgorithm.AlgorithmIdentifier.algorithm',
        tagClass: asn1$2.Class.UNIVERSAL,
        type: asn1$2.Type.OID,
        constructed: false,
        capture: 'hashOid'
        /* parameter block omitted, for SHA1 NULL anyhow. */
      }]
    }]
  }, {
    name: 'rsapss.maskGenAlgorithm',
    tagClass: asn1$2.Class.CONTEXT_SPECIFIC,
    type: 1,
    constructed: true,
    value: [{
      name: 'rsapss.maskGenAlgorithm.AlgorithmIdentifier',
      tagClass: asn1$2.Class.UNIVERSAL,
      type: asn1$2.Class.SEQUENCE,
      constructed: true,
      optional: true,
      value: [{
        name: 'rsapss.maskGenAlgorithm.AlgorithmIdentifier.algorithm',
        tagClass: asn1$2.Class.UNIVERSAL,
        type: asn1$2.Type.OID,
        constructed: false,
        capture: 'maskGenOid'
      }, {
        name: 'rsapss.maskGenAlgorithm.AlgorithmIdentifier.params',
        tagClass: asn1$2.Class.UNIVERSAL,
        type: asn1$2.Type.SEQUENCE,
        constructed: true,
        value: [{
          name: 'rsapss.maskGenAlgorithm.AlgorithmIdentifier.params.algorithm',
          tagClass: asn1$2.Class.UNIVERSAL,
          type: asn1$2.Type.OID,
          constructed: false,
          capture: 'maskGenHashOid'
          /* parameter block omitted, for SHA1 NULL anyhow. */
        }]
      }]
    }]
  }, {
    name: 'rsapss.saltLength',
    tagClass: asn1$2.Class.CONTEXT_SPECIFIC,
    type: 2,
    optional: true,
    value: [{
      name: 'rsapss.saltLength.saltLength',
      tagClass: asn1$2.Class.UNIVERSAL,
      type: asn1$2.Class.INTEGER,
      constructed: false,
      capture: 'saltLength'
    }]
  }, {
    name: 'rsapss.trailerField',
    tagClass: asn1$2.Class.CONTEXT_SPECIFIC,
    type: 3,
    optional: true,
    value: [{
      name: 'rsapss.trailer.trailer',
      tagClass: asn1$2.Class.UNIVERSAL,
      type: asn1$2.Class.INTEGER,
      constructed: false,
      capture: 'trailer'
    }]
  }]
};

// validator for a CertificationRequestInfo structure
var certificationRequestInfoValidator = {
  name: 'CertificationRequestInfo',
  tagClass: asn1$2.Class.UNIVERSAL,
  type: asn1$2.Type.SEQUENCE,
  constructed: true,
  captureAsn1: 'certificationRequestInfo',
  value: [{
    name: 'CertificationRequestInfo.integer',
    tagClass: asn1$2.Class.UNIVERSAL,
    type: asn1$2.Type.INTEGER,
    constructed: false,
    capture: 'certificationRequestInfoVersion'
  }, {
    // Name (subject) (RDNSequence)
    name: 'CertificationRequestInfo.subject',
    tagClass: asn1$2.Class.UNIVERSAL,
    type: asn1$2.Type.SEQUENCE,
    constructed: true,
    captureAsn1: 'certificationRequestInfoSubject'
  },
  // SubjectPublicKeyInfo
  publicKeyValidator,
  {
    name: 'CertificationRequestInfo.attributes',
    tagClass: asn1$2.Class.CONTEXT_SPECIFIC,
    type: 0,
    constructed: true,
    optional: true,
    capture: 'certificationRequestInfoAttributes',
    value: [{
      name: 'CertificationRequestInfo.attributes',
      tagClass: asn1$2.Class.UNIVERSAL,
      type: asn1$2.Type.SEQUENCE,
      constructed: true,
      value: [{
        name: 'CertificationRequestInfo.attributes.type',
        tagClass: asn1$2.Class.UNIVERSAL,
        type: asn1$2.Type.OID,
        constructed: false
      }, {
        name: 'CertificationRequestInfo.attributes.value',
        tagClass: asn1$2.Class.UNIVERSAL,
        type: asn1$2.Type.SET,
        constructed: true
      }]
    }]
  }]
};

// validator for a CertificationRequest structure
var certificationRequestValidator = {
  name: 'CertificationRequest',
  tagClass: asn1$2.Class.UNIVERSAL,
  type: asn1$2.Type.SEQUENCE,
  constructed: true,
  captureAsn1: 'csr',
  value: [
    certificationRequestInfoValidator, {
      // AlgorithmIdentifier (signature algorithm)
      name: 'CertificationRequest.signatureAlgorithm',
      tagClass: asn1$2.Class.UNIVERSAL,
      type: asn1$2.Type.SEQUENCE,
      constructed: true,
      value: [{
        // algorithm
        name: 'CertificationRequest.signatureAlgorithm.algorithm',
        tagClass: asn1$2.Class.UNIVERSAL,
        type: asn1$2.Type.OID,
        constructed: false,
        capture: 'csrSignatureOid'
      }, {
        name: 'CertificationRequest.signatureAlgorithm.parameters',
        tagClass: asn1$2.Class.UNIVERSAL,
        optional: true,
        captureAsn1: 'csrSignatureParams'
      }]
    }, {
      // signature
      name: 'CertificationRequest.signature',
      tagClass: asn1$2.Class.UNIVERSAL,
      type: asn1$2.Type.BITSTRING,
      constructed: false,
      captureBitStringValue: 'csrSignature'
    }
  ]
};

/**
 * Converts an RDNSequence of ASN.1 DER-encoded RelativeDistinguishedName
 * sets into an array with objects that have type and value properties.
 *
 * @param rdn the RDNSequence to convert.
 * @param md a message digest to append type and value to if provided.
 */
pki$2.RDNAttributesAsArray = function(rdn, md) {
  var rval = [];

  // each value in 'rdn' in is a SET of RelativeDistinguishedName
  var set, attr, obj;
  for(var si = 0; si < rdn.value.length; ++si) {
    // get the RelativeDistinguishedName set
    set = rdn.value[si];

    // each value in the SET is an AttributeTypeAndValue sequence
    // containing first a type (an OID) and second a value (defined by
    // the OID)
    for(var i = 0; i < set.value.length; ++i) {
      obj = {};
      attr = set.value[i];
      obj.type = asn1$2.derToOid(attr.value[0].value);
      obj.value = attr.value[1].value;
      obj.valueTagClass = attr.value[1].type;
      // if the OID is known, get its name and short name
      if(obj.type in oids) {
        obj.name = oids[obj.type];
        if(obj.name in _shortNames) {
          obj.shortName = _shortNames[obj.name];
        }
      }
      if(md) {
        md.update(obj.type);
        md.update(obj.value);
      }
      rval.push(obj);
    }
  }

  return rval;
};

/**
 * Converts ASN.1 CRIAttributes into an array with objects that have type and
 * value properties.
 *
 * @param attributes the CRIAttributes to convert.
 */
pki$2.CRIAttributesAsArray = function(attributes) {
  var rval = [];

  // each value in 'attributes' in is a SEQUENCE with an OID and a SET
  for(var si = 0; si < attributes.length; ++si) {
    // get the attribute sequence
    var seq = attributes[si];

    // each value in the SEQUENCE containing first a type (an OID) and
    // second a set of values (defined by the OID)
    var type = asn1$2.derToOid(seq.value[0].value);
    var values = seq.value[1].value;
    for(var vi = 0; vi < values.length; ++vi) {
      var obj = {};
      obj.type = type;
      obj.value = values[vi].value;
      obj.valueTagClass = values[vi].type;
      // if the OID is known, get its name and short name
      if(obj.type in oids) {
        obj.name = oids[obj.type];
        if(obj.name in _shortNames) {
          obj.shortName = _shortNames[obj.name];
        }
      }
      // parse extensions
      if(obj.type === oids.extensionRequest) {
        obj.extensions = [];
        for(var ei = 0; ei < obj.value.length; ++ei) {
          obj.extensions.push(pki$2.certificateExtensionFromAsn1(obj.value[ei]));
        }
      }
      rval.push(obj);
    }
  }

  return rval;
};

/**
 * Gets an issuer or subject attribute from its name, type, or short name.
 *
 * @param obj the issuer or subject object.
 * @param options a short name string or an object with:
 *          shortName the short name for the attribute.
 *          name the name for the attribute.
 *          type the type for the attribute.
 *
 * @return the attribute.
 */
function _getAttribute(obj, options) {
  if(typeof options === 'string') {
    options = {shortName: options};
  }

  var rval = null;
  var attr;
  for(var i = 0; rval === null && i < obj.attributes.length; ++i) {
    attr = obj.attributes[i];
    if(options.type && options.type === attr.type) {
      rval = attr;
    } else if(options.name && options.name === attr.name) {
      rval = attr;
    } else if(options.shortName && options.shortName === attr.shortName) {
      rval = attr;
    }
  }
  return rval;
}

/**
 * Converts signature parameters from ASN.1 structure.
 *
 * Currently only RSASSA-PSS supported.  The PKCS#1 v1.5 signature scheme had
 * no parameters.
 *
 * RSASSA-PSS-params  ::=  SEQUENCE  {
 *   hashAlgorithm      [0] HashAlgorithm DEFAULT
 *                             sha1Identifier,
 *   maskGenAlgorithm   [1] MaskGenAlgorithm DEFAULT
 *                             mgf1SHA1Identifier,
 *   saltLength         [2] INTEGER DEFAULT 20,
 *   trailerField       [3] INTEGER DEFAULT 1
 * }
 *
 * HashAlgorithm  ::=  AlgorithmIdentifier
 *
 * MaskGenAlgorithm  ::=  AlgorithmIdentifier
 *
 * AlgorithmIdentifer ::= SEQUENCE {
 *   algorithm OBJECT IDENTIFIER,
 *   parameters ANY DEFINED BY algorithm OPTIONAL
 * }
 *
 * @param oid The OID specifying the signature algorithm
 * @param obj The ASN.1 structure holding the parameters
 * @param fillDefaults Whether to use return default values where omitted
 * @return signature parameter object
 */
var _readSignatureParameters = function(oid, obj, fillDefaults) {
  var params = {};

  if(oid !== oids['RSASSA-PSS']) {
    return params;
  }

  if(fillDefaults) {
    params = {
      hash: {
        algorithmOid: oids['sha1']
      },
      mgf: {
        algorithmOid: oids['mgf1'],
        hash: {
          algorithmOid: oids['sha1']
        }
      },
      saltLength: 20
    };
  }

  var capture = {};
  var errors = [];
  if(!asn1$2.validate(obj, rsassaPssParameterValidator, capture, errors)) {
    var error = new Error('Cannot read RSASSA-PSS parameter block.');
    error.errors = errors;
    throw error;
  }

  if(capture.hashOid !== undefined) {
    params.hash = params.hash || {};
    params.hash.algorithmOid = asn1$2.derToOid(capture.hashOid);
  }

  if(capture.maskGenOid !== undefined) {
    params.mgf = params.mgf || {};
    params.mgf.algorithmOid = asn1$2.derToOid(capture.maskGenOid);
    params.mgf.hash = params.mgf.hash || {};
    params.mgf.hash.algorithmOid = asn1$2.derToOid(capture.maskGenHashOid);
  }

  if(capture.saltLength !== undefined) {
    params.saltLength = capture.saltLength.charCodeAt(0);
  }

  return params;
};

/**
 * Create signature digest for OID.
 *
 * @param options
 *   signatureOid: the OID specifying the signature algorithm.
 *   type: a human readable type for error messages
 * @return a created md instance. throws if unknown oid.
 */
var _createSignatureDigest = function(options) {
  switch(oids[options.signatureOid]) {
    case 'sha1WithRSAEncryption':
    // deprecated alias
    case 'sha1WithRSASignature':
      return forge$2.md.sha1.create();
    case 'md5WithRSAEncryption':
      return forge$2.md.md5.create();
    case 'sha256WithRSAEncryption':
      return forge$2.md.sha256.create();
    case 'sha384WithRSAEncryption':
      return forge$2.md.sha384.create();
    case 'sha512WithRSAEncryption':
      return forge$2.md.sha512.create();
    case 'RSASSA-PSS':
      return forge$2.md.sha256.create();
    default:
      var error = new Error(
        'Could not compute ' + options.type + ' digest. ' +
        'Unknown signature OID.');
      error.signatureOid = options.signatureOid;
      throw error;
  }
};

/**
 * Verify signature on certificate or CSR.
 *
 * @param options:
 *   certificate the certificate or CSR to verify.
 *   md the signature digest.
 *   signature the signature
 * @return a created md instance. throws if unknown oid.
 */
var _verifySignature = function(options) {
  var cert = options.certificate;
  var scheme;

  switch(cert.signatureOid) {
    case oids.sha1WithRSAEncryption:
    // deprecated alias
    case oids.sha1WithRSASignature:
      /* use PKCS#1 v1.5 padding scheme */
      break;
    case oids['RSASSA-PSS']:
      var hash, mgf;

      /* initialize mgf */
      hash = oids[cert.signatureParameters.mgf.hash.algorithmOid];
      if(hash === undefined || forge$2.md[hash] === undefined) {
        var error = new Error('Unsupported MGF hash function.');
        error.oid = cert.signatureParameters.mgf.hash.algorithmOid;
        error.name = hash;
        throw error;
      }

      mgf = oids[cert.signatureParameters.mgf.algorithmOid];
      if(mgf === undefined || forge$2.mgf[mgf] === undefined) {
        var error = new Error('Unsupported MGF function.');
        error.oid = cert.signatureParameters.mgf.algorithmOid;
        error.name = mgf;
        throw error;
      }

      mgf = forge$2.mgf[mgf].create(forge$2.md[hash].create());

      /* initialize hash function */
      hash = oids[cert.signatureParameters.hash.algorithmOid];
      if(hash === undefined || forge$2.md[hash] === undefined) {
        var error = new Error('Unsupported RSASSA-PSS hash function.');
        error.oid = cert.signatureParameters.hash.algorithmOid;
        error.name = hash;
        throw error;
      }

      scheme = forge$2.pss.create(
        forge$2.md[hash].create(), mgf, cert.signatureParameters.saltLength
      );
      break;
  }

  // verify signature on cert using public key
  return cert.publicKey.verify(
    options.md.digest().getBytes(), options.signature, scheme
  );
};

/**
 * Converts an X.509 certificate from PEM format.
 *
 * Note: If the certificate is to be verified then compute hash should
 * be set to true. This will scan the TBSCertificate part of the ASN.1
 * object while it is converted so it doesn't need to be converted back
 * to ASN.1-DER-encoding later.
 *
 * @param pem the PEM-formatted certificate.
 * @param computeHash true to compute the hash for verification.
 * @param strict true to be strict when checking ASN.1 value lengths, false to
 *          allow truncated values (default: true).
 *
 * @return the certificate.
 */
pki$2.certificateFromPem = function(pem, computeHash, strict) {
  var msg = forge$2.pem.decode(pem)[0];

  if(msg.type !== 'CERTIFICATE' &&
    msg.type !== 'X509 CERTIFICATE' &&
    msg.type !== 'TRUSTED CERTIFICATE') {
    var error = new Error(
      'Could not convert certificate from PEM; PEM header type ' +
      'is not "CERTIFICATE", "X509 CERTIFICATE", or "TRUSTED CERTIFICATE".');
    error.headerType = msg.type;
    throw error;
  }
  if(msg.procType && msg.procType.type === 'ENCRYPTED') {
    throw new Error(
      'Could not convert certificate from PEM; PEM is encrypted.');
  }

  // convert DER to ASN.1 object
  var obj = asn1$2.fromDer(msg.body, strict);

  return pki$2.certificateFromAsn1(obj, computeHash);
};

/**
 * Converts an X.509 certificate to PEM format.
 *
 * @param cert the certificate.
 * @param maxline the maximum characters per line, defaults to 64.
 *
 * @return the PEM-formatted certificate.
 */
pki$2.certificateToPem = function(cert, maxline) {
  // convert to ASN.1, then DER, then PEM-encode
  var msg = {
    type: 'CERTIFICATE',
    body: asn1$2.toDer(pki$2.certificateToAsn1(cert)).getBytes()
  };
  return forge$2.pem.encode(msg, {maxline: maxline});
};

/**
 * Converts an RSA public key from PEM format.
 *
 * @param pem the PEM-formatted public key.
 *
 * @return the public key.
 */
pki$2.publicKeyFromPem = function(pem) {
  var msg = forge$2.pem.decode(pem)[0];

  if(msg.type !== 'PUBLIC KEY' && msg.type !== 'RSA PUBLIC KEY') {
    var error = new Error('Could not convert public key from PEM; PEM header ' +
      'type is not "PUBLIC KEY" or "RSA PUBLIC KEY".');
    error.headerType = msg.type;
    throw error;
  }
  if(msg.procType && msg.procType.type === 'ENCRYPTED') {
    throw new Error('Could not convert public key from PEM; PEM is encrypted.');
  }

  // convert DER to ASN.1 object
  var obj = asn1$2.fromDer(msg.body);

  return pki$2.publicKeyFromAsn1(obj);
};

/**
 * Converts an RSA public key to PEM format (using a SubjectPublicKeyInfo).
 *
 * @param key the public key.
 * @param maxline the maximum characters per line, defaults to 64.
 *
 * @return the PEM-formatted public key.
 */
pki$2.publicKeyToPem = function(key, maxline) {
  // convert to ASN.1, then DER, then PEM-encode
  var msg = {
    type: 'PUBLIC KEY',
    body: asn1$2.toDer(pki$2.publicKeyToAsn1(key)).getBytes()
  };
  return forge$2.pem.encode(msg, {maxline: maxline});
};

/**
 * Converts an RSA public key to PEM format (using an RSAPublicKey).
 *
 * @param key the public key.
 * @param maxline the maximum characters per line, defaults to 64.
 *
 * @return the PEM-formatted public key.
 */
pki$2.publicKeyToRSAPublicKeyPem = function(key, maxline) {
  // convert to ASN.1, then DER, then PEM-encode
  var msg = {
    type: 'RSA PUBLIC KEY',
    body: asn1$2.toDer(pki$2.publicKeyToRSAPublicKey(key)).getBytes()
  };
  return forge$2.pem.encode(msg, {maxline: maxline});
};

/**
 * Gets a fingerprint for the given public key.
 *
 * @param options the options to use.
 *          [md] the message digest object to use (defaults to forge.md.sha1).
 *          [type] the type of fingerprint, such as 'RSAPublicKey',
 *            'SubjectPublicKeyInfo' (defaults to 'RSAPublicKey').
 *          [encoding] an alternative output encoding, such as 'hex'
 *            (defaults to none, outputs a byte buffer).
 *          [delimiter] the delimiter to use between bytes for 'hex' encoded
 *            output, eg: ':' (defaults to none).
 *
 * @return the fingerprint as a byte buffer or other encoding based on options.
 */
pki$2.getPublicKeyFingerprint = function(key, options) {
  options = options || {};
  var md = options.md || forge$2.md.sha1.create();
  var type = options.type || 'RSAPublicKey';

  var bytes;
  switch(type) {
    case 'RSAPublicKey':
      bytes = asn1$2.toDer(pki$2.publicKeyToRSAPublicKey(key)).getBytes();
      break;
    case 'SubjectPublicKeyInfo':
      bytes = asn1$2.toDer(pki$2.publicKeyToAsn1(key)).getBytes();
      break;
    default:
      throw new Error('Unknown fingerprint type "' + options.type + '".');
  }

  // hash public key bytes
  md.start();
  md.update(bytes);
  var digest = md.digest();
  if(options.encoding === 'hex') {
    var hex = digest.toHex();
    if(options.delimiter) {
      return hex.match(/.{2}/g).join(options.delimiter);
    }
    return hex;
  } else if(options.encoding === 'binary') {
    return digest.getBytes();
  } else if(options.encoding) {
    throw new Error('Unknown encoding "' + options.encoding + '".');
  }
  return digest;
};

/**
 * Converts a PKCS#10 certification request (CSR) from PEM format.
 *
 * Note: If the certification request is to be verified then compute hash
 * should be set to true. This will scan the CertificationRequestInfo part of
 * the ASN.1 object while it is converted so it doesn't need to be converted
 * back to ASN.1-DER-encoding later.
 *
 * @param pem the PEM-formatted certificate.
 * @param computeHash true to compute the hash for verification.
 * @param strict true to be strict when checking ASN.1 value lengths, false to
 *          allow truncated values (default: true).
 *
 * @return the certification request (CSR).
 */
pki$2.certificationRequestFromPem = function(pem, computeHash, strict) {
  var msg = forge$2.pem.decode(pem)[0];

  if(msg.type !== 'CERTIFICATE REQUEST') {
    var error = new Error('Could not convert certification request from PEM; ' +
      'PEM header type is not "CERTIFICATE REQUEST".');
    error.headerType = msg.type;
    throw error;
  }
  if(msg.procType && msg.procType.type === 'ENCRYPTED') {
    throw new Error('Could not convert certification request from PEM; ' +
      'PEM is encrypted.');
  }

  // convert DER to ASN.1 object
  var obj = asn1$2.fromDer(msg.body, strict);

  return pki$2.certificationRequestFromAsn1(obj, computeHash);
};

/**
 * Converts a PKCS#10 certification request (CSR) to PEM format.
 *
 * @param csr the certification request.
 * @param maxline the maximum characters per line, defaults to 64.
 *
 * @return the PEM-formatted certification request.
 */
pki$2.certificationRequestToPem = function(csr, maxline) {
  // convert to ASN.1, then DER, then PEM-encode
  var msg = {
    type: 'CERTIFICATE REQUEST',
    body: asn1$2.toDer(pki$2.certificationRequestToAsn1(csr)).getBytes()
  };
  return forge$2.pem.encode(msg, {maxline: maxline});
};

/**
 * Creates an empty X.509v3 RSA certificate.
 *
 * @return the certificate.
 */
pki$2.createCertificate = function() {
  var cert = {};
  cert.version = 0x02;
  cert.serialNumber = '00';
  cert.signatureOid = null;
  cert.signature = null;
  cert.siginfo = {};
  cert.siginfo.algorithmOid = null;
  cert.validity = {};
  cert.validity.notBefore = new Date();
  cert.validity.notAfter = new Date();

  cert.issuer = {};
  cert.issuer.getField = function(sn) {
    return _getAttribute(cert.issuer, sn);
  };
  cert.issuer.addField = function(attr) {
    _fillMissingFields([attr]);
    cert.issuer.attributes.push(attr);
  };
  cert.issuer.attributes = [];
  cert.issuer.hash = null;

  cert.subject = {};
  cert.subject.getField = function(sn) {
    return _getAttribute(cert.subject, sn);
  };
  cert.subject.addField = function(attr) {
    _fillMissingFields([attr]);
    cert.subject.attributes.push(attr);
  };
  cert.subject.attributes = [];
  cert.subject.hash = null;

  cert.extensions = [];
  cert.publicKey = null;
  cert.md = null;

  /**
   * Sets the subject of this certificate.
   *
   * @param attrs the array of subject attributes to use.
   * @param uniqueId an optional a unique ID to use.
   */
  cert.setSubject = function(attrs, uniqueId) {
    // set new attributes, clear hash
    _fillMissingFields(attrs);
    cert.subject.attributes = attrs;
    delete cert.subject.uniqueId;
    if(uniqueId) {
      // TODO: support arbitrary bit length ids
      cert.subject.uniqueId = uniqueId;
    }
    cert.subject.hash = null;
  };

  /**
   * Sets the issuer of this certificate.
   *
   * @param attrs the array of issuer attributes to use.
   * @param uniqueId an optional a unique ID to use.
   */
  cert.setIssuer = function(attrs, uniqueId) {
    // set new attributes, clear hash
    _fillMissingFields(attrs);
    cert.issuer.attributes = attrs;
    delete cert.issuer.uniqueId;
    if(uniqueId) {
      // TODO: support arbitrary bit length ids
      cert.issuer.uniqueId = uniqueId;
    }
    cert.issuer.hash = null;
  };

  /**
   * Sets the extensions of this certificate.
   *
   * @param exts the array of extensions to use.
   */
  cert.setExtensions = function(exts) {
    for(var i = 0; i < exts.length; ++i) {
      _fillMissingExtensionFields(exts[i], {cert: cert});
    }
    // set new extensions
    cert.extensions = exts;
  };

  /**
   * Gets an extension by its name or id.
   *
   * @param options the name to use or an object with:
   *          name the name to use.
   *          id the id to use.
   *
   * @return the extension or null if not found.
   */
  cert.getExtension = function(options) {
    if(typeof options === 'string') {
      options = {name: options};
    }

    var rval = null;
    var ext;
    for(var i = 0; rval === null && i < cert.extensions.length; ++i) {
      ext = cert.extensions[i];
      if(options.id && ext.id === options.id) {
        rval = ext;
      } else if(options.name && ext.name === options.name) {
        rval = ext;
      }
    }
    return rval;
  };

  /**
   * Signs this certificate using the given private key.
   *
   * @param key the private key to sign with.
   * @param md the message digest object to use (defaults to forge.md.sha1).
   */
  cert.sign = function(key, md) {
    // TODO: get signature OID from private key
    cert.md = md || forge$2.md.sha1.create();
    var algorithmOid = oids[cert.md.algorithm + 'WithRSAEncryption'];
    if(!algorithmOid) {
      var error = new Error('Could not compute certificate digest. ' +
        'Unknown message digest algorithm OID.');
      error.algorithm = cert.md.algorithm;
      throw error;
    }
    cert.signatureOid = cert.siginfo.algorithmOid = algorithmOid;

    // get TBSCertificate, convert to DER
    cert.tbsCertificate = pki$2.getTBSCertificate(cert);
    var bytes = asn1$2.toDer(cert.tbsCertificate);

    // digest and sign
    cert.md.update(bytes.getBytes());
    cert.signature = key.sign(cert.md);
  };

  /**
   * Attempts verify the signature on the passed certificate using this
   * certificate's public key.
   *
   * @param child the certificate to verify.
   *
   * @return true if verified, false if not.
   */
  cert.verify = function(child) {
    var rval = false;

    if(!cert.issued(child)) {
      var issuer = child.issuer;
      var subject = cert.subject;
      var error = new Error(
        'The parent certificate did not issue the given child ' +
        'certificate; the child certificate\'s issuer does not match the ' +
        'parent\'s subject.');
      error.expectedIssuer = subject.attributes;
      error.actualIssuer = issuer.attributes;
      throw error;
    }

    var md = child.md;
    if(md === null) {
      // create digest for OID signature types
      md = _createSignatureDigest({
        signatureOid: child.signatureOid,
        type: 'certificate'
      });

      // produce DER formatted TBSCertificate and digest it
      var tbsCertificate = child.tbsCertificate || pki$2.getTBSCertificate(child);
      var bytes = asn1$2.toDer(tbsCertificate);
      md.update(bytes.getBytes());
    }

    if(md !== null) {
      rval = _verifySignature({
        certificate: cert, md: md, signature: child.signature
      });
    }

    return rval;
  };

  /**
   * Returns true if this certificate's issuer matches the passed
   * certificate's subject. Note that no signature check is performed.
   *
   * @param parent the certificate to check.
   *
   * @return true if this certificate's issuer matches the passed certificate's
   *         subject.
   */
  cert.isIssuer = function(parent) {
    var rval = false;

    var i = cert.issuer;
    var s = parent.subject;

    // compare hashes if present
    if(i.hash && s.hash) {
      rval = (i.hash === s.hash);
    } else if(i.attributes.length === s.attributes.length) {
      // all attributes are the same so issuer matches subject
      rval = true;
      var iattr, sattr;
      for(var n = 0; rval && n < i.attributes.length; ++n) {
        iattr = i.attributes[n];
        sattr = s.attributes[n];
        if(iattr.type !== sattr.type || iattr.value !== sattr.value) {
          // attribute mismatch
          rval = false;
        }
      }
    }

    return rval;
  };

  /**
   * Returns true if this certificate's subject matches the issuer of the
   * given certificate). Note that not signature check is performed.
   *
   * @param child the certificate to check.
   *
   * @return true if this certificate's subject matches the passed
   *         certificate's issuer.
   */
  cert.issued = function(child) {
    return child.isIssuer(cert);
  };

  /**
   * Generates the subjectKeyIdentifier for this certificate as byte buffer.
   *
   * @return the subjectKeyIdentifier for this certificate as byte buffer.
   */
  cert.generateSubjectKeyIdentifier = function() {
    /* See: 4.2.1.2 section of the the RFC3280, keyIdentifier is either:

      (1) The keyIdentifier is composed of the 160-bit SHA-1 hash of the
        value of the BIT STRING subjectPublicKey (excluding the tag,
        length, and number of unused bits).

      (2) The keyIdentifier is composed of a four bit type field with
        the value 0100 followed by the least significant 60 bits of the
        SHA-1 hash of the value of the BIT STRING subjectPublicKey
        (excluding the tag, length, and number of unused bit string bits).
    */

    // skipping the tag, length, and number of unused bits is the same
    // as just using the RSAPublicKey (for RSA keys, which are the
    // only ones supported)
    return pki$2.getPublicKeyFingerprint(cert.publicKey, {type: 'RSAPublicKey'});
  };

  /**
   * Verifies the subjectKeyIdentifier extension value for this certificate
   * against its public key. If no extension is found, false will be
   * returned.
   *
   * @return true if verified, false if not.
   */
  cert.verifySubjectKeyIdentifier = function() {
    var oid = oids['subjectKeyIdentifier'];
    for(var i = 0; i < cert.extensions.length; ++i) {
      var ext = cert.extensions[i];
      if(ext.id === oid) {
        var ski = cert.generateSubjectKeyIdentifier().getBytes();
        return (forge$2.util.hexToBytes(ext.subjectKeyIdentifier) === ski);
      }
    }
    return false;
  };

  return cert;
};

/**
 * Converts an X.509v3 RSA certificate from an ASN.1 object.
 *
 * Note: If the certificate is to be verified then compute hash should
 * be set to true. There is currently no implementation for converting
 * a certificate back to ASN.1 so the TBSCertificate part of the ASN.1
 * object needs to be scanned before the cert object is created.
 *
 * @param obj the asn1 representation of an X.509v3 RSA certificate.
 * @param computeHash true to compute the hash for verification.
 *
 * @return the certificate.
 */
pki$2.certificateFromAsn1 = function(obj, computeHash) {
  // validate certificate and capture data
  var capture = {};
  var errors = [];
  if(!asn1$2.validate(obj, x509CertificateValidator, capture, errors)) {
    var error = new Error('Cannot read X.509 certificate. ' +
      'ASN.1 object is not an X509v3 Certificate.');
    error.errors = errors;
    throw error;
  }

  // get oid
  var oid = asn1$2.derToOid(capture.publicKeyOid);
  if(oid !== pki$2.oids.rsaEncryption) {
    throw new Error('Cannot read public key. OID is not RSA.');
  }

  // create certificate
  var cert = pki$2.createCertificate();
  cert.version = capture.certVersion ?
    capture.certVersion.charCodeAt(0) : 0;
  var serial = forge$2.util.createBuffer(capture.certSerialNumber);
  cert.serialNumber = serial.toHex();
  cert.signatureOid = forge$2.asn1.derToOid(capture.certSignatureOid);
  cert.signatureParameters = _readSignatureParameters(
    cert.signatureOid, capture.certSignatureParams, true);
  cert.siginfo.algorithmOid = forge$2.asn1.derToOid(capture.certinfoSignatureOid);
  cert.siginfo.parameters = _readSignatureParameters(cert.siginfo.algorithmOid,
    capture.certinfoSignatureParams, false);
  cert.signature = capture.certSignature;

  var validity = [];
  if(capture.certValidity1UTCTime !== undefined) {
    validity.push(asn1$2.utcTimeToDate(capture.certValidity1UTCTime));
  }
  if(capture.certValidity2GeneralizedTime !== undefined) {
    validity.push(asn1$2.generalizedTimeToDate(
      capture.certValidity2GeneralizedTime));
  }
  if(capture.certValidity3UTCTime !== undefined) {
    validity.push(asn1$2.utcTimeToDate(capture.certValidity3UTCTime));
  }
  if(capture.certValidity4GeneralizedTime !== undefined) {
    validity.push(asn1$2.generalizedTimeToDate(
      capture.certValidity4GeneralizedTime));
  }
  if(validity.length > 2) {
    throw new Error('Cannot read notBefore/notAfter validity times; more ' +
      'than two times were provided in the certificate.');
  }
  if(validity.length < 2) {
    throw new Error('Cannot read notBefore/notAfter validity times; they ' +
      'were not provided as either UTCTime or GeneralizedTime.');
  }
  cert.validity.notBefore = validity[0];
  cert.validity.notAfter = validity[1];

  // keep TBSCertificate to preserve signature when exporting
  cert.tbsCertificate = capture.tbsCertificate;

  if(computeHash) {
    // create digest for OID signature type
    cert.md = _createSignatureDigest({
      signatureOid: cert.signatureOid,
      type: 'certificate'
    });

    // produce DER formatted TBSCertificate and digest it
    var bytes = asn1$2.toDer(cert.tbsCertificate);
    cert.md.update(bytes.getBytes());
  }

  // handle issuer, build issuer message digest
  var imd = forge$2.md.sha1.create();
  var ibytes = asn1$2.toDer(capture.certIssuer);
  imd.update(ibytes.getBytes());
  cert.issuer.getField = function(sn) {
    return _getAttribute(cert.issuer, sn);
  };
  cert.issuer.addField = function(attr) {
    _fillMissingFields([attr]);
    cert.issuer.attributes.push(attr);
  };
  cert.issuer.attributes = pki$2.RDNAttributesAsArray(capture.certIssuer);
  if(capture.certIssuerUniqueId) {
    cert.issuer.uniqueId = capture.certIssuerUniqueId;
  }
  cert.issuer.hash = imd.digest().toHex();

  // handle subject, build subject message digest
  var smd = forge$2.md.sha1.create();
  var sbytes = asn1$2.toDer(capture.certSubject);
  smd.update(sbytes.getBytes());
  cert.subject.getField = function(sn) {
    return _getAttribute(cert.subject, sn);
  };
  cert.subject.addField = function(attr) {
    _fillMissingFields([attr]);
    cert.subject.attributes.push(attr);
  };
  cert.subject.attributes = pki$2.RDNAttributesAsArray(capture.certSubject);
  if(capture.certSubjectUniqueId) {
    cert.subject.uniqueId = capture.certSubjectUniqueId;
  }
  cert.subject.hash = smd.digest().toHex();

  // handle extensions
  if(capture.certExtensions) {
    cert.extensions = pki$2.certificateExtensionsFromAsn1(capture.certExtensions);
  } else {
    cert.extensions = [];
  }

  // convert RSA public key from ASN.1
  cert.publicKey = pki$2.publicKeyFromAsn1(capture.subjectPublicKeyInfo);

  return cert;
};

/**
 * Converts an ASN.1 extensions object (with extension sequences as its
 * values) into an array of extension objects with types and values.
 *
 * Supported extensions:
 *
 * id-ce-keyUsage OBJECT IDENTIFIER ::=  { id-ce 15 }
 * KeyUsage ::= BIT STRING {
 *   digitalSignature        (0),
 *   nonRepudiation          (1),
 *   keyEncipherment         (2),
 *   dataEncipherment        (3),
 *   keyAgreement            (4),
 *   keyCertSign             (5),
 *   cRLSign                 (6),
 *   encipherOnly            (7),
 *   decipherOnly            (8)
 * }
 *
 * id-ce-basicConstraints OBJECT IDENTIFIER ::=  { id-ce 19 }
 * BasicConstraints ::= SEQUENCE {
 *   cA                      BOOLEAN DEFAULT FALSE,
 *   pathLenConstraint       INTEGER (0..MAX) OPTIONAL
 * }
 *
 * subjectAltName EXTENSION ::= {
 *   SYNTAX GeneralNames
 *   IDENTIFIED BY id-ce-subjectAltName
 * }
 *
 * GeneralNames ::= SEQUENCE SIZE (1..MAX) OF GeneralName
 *
 * GeneralName ::= CHOICE {
 *   otherName      [0] INSTANCE OF OTHER-NAME,
 *   rfc822Name     [1] IA5String,
 *   dNSName        [2] IA5String,
 *   x400Address    [3] ORAddress,
 *   directoryName  [4] Name,
 *   ediPartyName   [5] EDIPartyName,
 *   uniformResourceIdentifier [6] IA5String,
 *   IPAddress      [7] OCTET STRING,
 *   registeredID   [8] OBJECT IDENTIFIER
 * }
 *
 * OTHER-NAME ::= TYPE-IDENTIFIER
 *
 * EDIPartyName ::= SEQUENCE {
 *   nameAssigner [0] DirectoryString {ub-name} OPTIONAL,
 *   partyName    [1] DirectoryString {ub-name}
 * }
 *
 * @param exts the extensions ASN.1 with extension sequences to parse.
 *
 * @return the array.
 */
pki$2.certificateExtensionsFromAsn1 = function(exts) {
  var rval = [];
  for(var i = 0; i < exts.value.length; ++i) {
    // get extension sequence
    var extseq = exts.value[i];
    for(var ei = 0; ei < extseq.value.length; ++ei) {
      rval.push(pki$2.certificateExtensionFromAsn1(extseq.value[ei]));
    }
  }

  return rval;
};

/**
 * Parses a single certificate extension from ASN.1.
 *
 * @param ext the extension in ASN.1 format.
 *
 * @return the parsed extension as an object.
 */
pki$2.certificateExtensionFromAsn1 = function(ext) {
  // an extension has:
  // [0] extnID      OBJECT IDENTIFIER
  // [1] critical    BOOLEAN DEFAULT FALSE
  // [2] extnValue   OCTET STRING
  var e = {};
  e.id = asn1$2.derToOid(ext.value[0].value);
  e.critical = false;
  if(ext.value[1].type === asn1$2.Type.BOOLEAN) {
    e.critical = (ext.value[1].value.charCodeAt(0) !== 0x00);
    e.value = ext.value[2].value;
  } else {
    e.value = ext.value[1].value;
  }
  // if the oid is known, get its name
  if(e.id in oids) {
    e.name = oids[e.id];

    // handle key usage
    if(e.name === 'keyUsage') {
      // get value as BIT STRING
      var ev = asn1$2.fromDer(e.value);
      var b2 = 0x00;
      var b3 = 0x00;
      if(ev.value.length > 1) {
        // skip first byte, just indicates unused bits which
        // will be padded with 0s anyway
        // get bytes with flag bits
        b2 = ev.value.charCodeAt(1);
        b3 = ev.value.length > 2 ? ev.value.charCodeAt(2) : 0;
      }
      // set flags
      e.digitalSignature = (b2 & 0x80) === 0x80;
      e.nonRepudiation = (b2 & 0x40) === 0x40;
      e.keyEncipherment = (b2 & 0x20) === 0x20;
      e.dataEncipherment = (b2 & 0x10) === 0x10;
      e.keyAgreement = (b2 & 0x08) === 0x08;
      e.keyCertSign = (b2 & 0x04) === 0x04;
      e.cRLSign = (b2 & 0x02) === 0x02;
      e.encipherOnly = (b2 & 0x01) === 0x01;
      e.decipherOnly = (b3 & 0x80) === 0x80;
    } else if(e.name === 'basicConstraints') {
      // handle basic constraints
      // get value as SEQUENCE
      var ev = asn1$2.fromDer(e.value);
      // get cA BOOLEAN flag (defaults to false)
      if(ev.value.length > 0 && ev.value[0].type === asn1$2.Type.BOOLEAN) {
        e.cA = (ev.value[0].value.charCodeAt(0) !== 0x00);
      } else {
        e.cA = false;
      }
      // get path length constraint
      var value = null;
      if(ev.value.length > 0 && ev.value[0].type === asn1$2.Type.INTEGER) {
        value = ev.value[0].value;
      } else if(ev.value.length > 1) {
        value = ev.value[1].value;
      }
      if(value !== null) {
        e.pathLenConstraint = asn1$2.derToInteger(value);
      }
    } else if(e.name === 'extKeyUsage') {
      // handle extKeyUsage
      // value is a SEQUENCE of OIDs
      var ev = asn1$2.fromDer(e.value);
      for(var vi = 0; vi < ev.value.length; ++vi) {
        var oid = asn1$2.derToOid(ev.value[vi].value);
        if(oid in oids) {
          e[oids[oid]] = true;
        } else {
          e[oid] = true;
        }
      }
    } else if(e.name === 'nsCertType') {
      // handle nsCertType
      // get value as BIT STRING
      var ev = asn1$2.fromDer(e.value);
      var b2 = 0x00;
      if(ev.value.length > 1) {
        // skip first byte, just indicates unused bits which
        // will be padded with 0s anyway
        // get bytes with flag bits
        b2 = ev.value.charCodeAt(1);
      }
      // set flags
      e.client = (b2 & 0x80) === 0x80;
      e.server = (b2 & 0x40) === 0x40;
      e.email = (b2 & 0x20) === 0x20;
      e.objsign = (b2 & 0x10) === 0x10;
      e.reserved = (b2 & 0x08) === 0x08;
      e.sslCA = (b2 & 0x04) === 0x04;
      e.emailCA = (b2 & 0x02) === 0x02;
      e.objCA = (b2 & 0x01) === 0x01;
    } else if(
      e.name === 'subjectAltName' ||
      e.name === 'issuerAltName') {
      // handle subjectAltName/issuerAltName
      e.altNames = [];

      // ev is a SYNTAX SEQUENCE
      var gn;
      var ev = asn1$2.fromDer(e.value);
      for(var n = 0; n < ev.value.length; ++n) {
        // get GeneralName
        gn = ev.value[n];

        var altName = {
          type: gn.type,
          value: gn.value
        };
        e.altNames.push(altName);

        // Note: Support for types 1,2,6,7,8
        switch(gn.type) {
          // rfc822Name
          case 1:
          // dNSName
          case 2:
          // uniformResourceIdentifier (URI)
          case 6:
            break;
          // IPAddress
          case 7:
            // convert to IPv4/IPv6 string representation
            altName.ip = forge$2.util.bytesToIP(gn.value);
            break;
          // registeredID
          case 8:
            altName.oid = asn1$2.derToOid(gn.value);
            break;
            // unsupported
        }
      }
    } else if(e.name === 'subjectKeyIdentifier') {
      // value is an OCTETSTRING w/the hash of the key-type specific
      // public key structure (eg: RSAPublicKey)
      var ev = asn1$2.fromDer(e.value);
      e.subjectKeyIdentifier = forge$2.util.bytesToHex(ev.value);
    }
  }
  return e;
};

/**
 * Converts a PKCS#10 certification request (CSR) from an ASN.1 object.
 *
 * Note: If the certification request is to be verified then compute hash
 * should be set to true. There is currently no implementation for converting
 * a certificate back to ASN.1 so the CertificationRequestInfo part of the
 * ASN.1 object needs to be scanned before the csr object is created.
 *
 * @param obj the asn1 representation of a PKCS#10 certification request (CSR).
 * @param computeHash true to compute the hash for verification.
 *
 * @return the certification request (CSR).
 */
pki$2.certificationRequestFromAsn1 = function(obj, computeHash) {
  // validate certification request and capture data
  var capture = {};
  var errors = [];
  if(!asn1$2.validate(obj, certificationRequestValidator, capture, errors)) {
    var error = new Error('Cannot read PKCS#10 certificate request. ' +
      'ASN.1 object is not a PKCS#10 CertificationRequest.');
    error.errors = errors;
    throw error;
  }

  // get oid
  var oid = asn1$2.derToOid(capture.publicKeyOid);
  if(oid !== pki$2.oids.rsaEncryption) {
    throw new Error('Cannot read public key. OID is not RSA.');
  }

  // create certification request
  var csr = pki$2.createCertificationRequest();
  csr.version = capture.csrVersion ? capture.csrVersion.charCodeAt(0) : 0;
  csr.signatureOid = forge$2.asn1.derToOid(capture.csrSignatureOid);
  csr.signatureParameters = _readSignatureParameters(
    csr.signatureOid, capture.csrSignatureParams, true);
  csr.siginfo.algorithmOid = forge$2.asn1.derToOid(capture.csrSignatureOid);
  csr.siginfo.parameters = _readSignatureParameters(
    csr.siginfo.algorithmOid, capture.csrSignatureParams, false);
  csr.signature = capture.csrSignature;

  // keep CertificationRequestInfo to preserve signature when exporting
  csr.certificationRequestInfo = capture.certificationRequestInfo;

  if(computeHash) {
    // create digest for OID signature type
    csr.md = _createSignatureDigest({
      signatureOid: csr.signatureOid,
      type: 'certification request'
    });

    // produce DER formatted CertificationRequestInfo and digest it
    var bytes = asn1$2.toDer(csr.certificationRequestInfo);
    csr.md.update(bytes.getBytes());
  }

  // handle subject, build subject message digest
  var smd = forge$2.md.sha1.create();
  csr.subject.getField = function(sn) {
    return _getAttribute(csr.subject, sn);
  };
  csr.subject.addField = function(attr) {
    _fillMissingFields([attr]);
    csr.subject.attributes.push(attr);
  };
  csr.subject.attributes = pki$2.RDNAttributesAsArray(
    capture.certificationRequestInfoSubject, smd);
  csr.subject.hash = smd.digest().toHex();

  // convert RSA public key from ASN.1
  csr.publicKey = pki$2.publicKeyFromAsn1(capture.subjectPublicKeyInfo);

  // convert attributes from ASN.1
  csr.getAttribute = function(sn) {
    return _getAttribute(csr, sn);
  };
  csr.addAttribute = function(attr) {
    _fillMissingFields([attr]);
    csr.attributes.push(attr);
  };
  csr.attributes = pki$2.CRIAttributesAsArray(
    capture.certificationRequestInfoAttributes || []);

  return csr;
};

/**
 * Creates an empty certification request (a CSR or certificate signing
 * request). Once created, its public key and attributes can be set and then
 * it can be signed.
 *
 * @return the empty certification request.
 */
pki$2.createCertificationRequest = function() {
  var csr = {};
  csr.version = 0x00;
  csr.signatureOid = null;
  csr.signature = null;
  csr.siginfo = {};
  csr.siginfo.algorithmOid = null;

  csr.subject = {};
  csr.subject.getField = function(sn) {
    return _getAttribute(csr.subject, sn);
  };
  csr.subject.addField = function(attr) {
    _fillMissingFields([attr]);
    csr.subject.attributes.push(attr);
  };
  csr.subject.attributes = [];
  csr.subject.hash = null;

  csr.publicKey = null;
  csr.attributes = [];
  csr.getAttribute = function(sn) {
    return _getAttribute(csr, sn);
  };
  csr.addAttribute = function(attr) {
    _fillMissingFields([attr]);
    csr.attributes.push(attr);
  };
  csr.md = null;

  /**
   * Sets the subject of this certification request.
   *
   * @param attrs the array of subject attributes to use.
   */
  csr.setSubject = function(attrs) {
    // set new attributes
    _fillMissingFields(attrs);
    csr.subject.attributes = attrs;
    csr.subject.hash = null;
  };

  /**
   * Sets the attributes of this certification request.
   *
   * @param attrs the array of attributes to use.
   */
  csr.setAttributes = function(attrs) {
    // set new attributes
    _fillMissingFields(attrs);
    csr.attributes = attrs;
  };

  /**
   * Signs this certification request using the given private key.
   *
   * @param key the private key to sign with.
   * @param md the message digest object to use (defaults to forge.md.sha1).
   */
  csr.sign = function(key, md) {
    // TODO: get signature OID from private key
    csr.md = md || forge$2.md.sha1.create();
    var algorithmOid = oids[csr.md.algorithm + 'WithRSAEncryption'];
    if(!algorithmOid) {
      var error = new Error('Could not compute certification request digest. ' +
        'Unknown message digest algorithm OID.');
      error.algorithm = csr.md.algorithm;
      throw error;
    }
    csr.signatureOid = csr.siginfo.algorithmOid = algorithmOid;

    // get CertificationRequestInfo, convert to DER
    csr.certificationRequestInfo = pki$2.getCertificationRequestInfo(csr);
    var bytes = asn1$2.toDer(csr.certificationRequestInfo);

    // digest and sign
    csr.md.update(bytes.getBytes());
    csr.signature = key.sign(csr.md);
  };

  /**
   * Attempts verify the signature on the passed certification request using
   * its public key.
   *
   * A CSR that has been exported to a file in PEM format can be verified using
   * OpenSSL using this command:
   *
   * openssl req -in <the-csr-pem-file> -verify -noout -text
   *
   * @return true if verified, false if not.
   */
  csr.verify = function() {
    var rval = false;

    var md = csr.md;
    if(md === null) {
      md = _createSignatureDigest({
        signatureOid: csr.signatureOid,
        type: 'certification request'
      });

      // produce DER formatted CertificationRequestInfo and digest it
      var cri = csr.certificationRequestInfo ||
        pki$2.getCertificationRequestInfo(csr);
      var bytes = asn1$2.toDer(cri);
      md.update(bytes.getBytes());
    }

    if(md !== null) {
      rval = _verifySignature({
        certificate: csr, md: md, signature: csr.signature
      });
    }

    return rval;
  };

  return csr;
};

/**
 * Converts an X.509 subject or issuer to an ASN.1 RDNSequence.
 *
 * @param obj the subject or issuer (distinguished name).
 *
 * @return the ASN.1 RDNSequence.
 */
function _dnToAsn1(obj) {
  // create an empty RDNSequence
  var rval = asn1$2.create(
    asn1$2.Class.UNIVERSAL, asn1$2.Type.SEQUENCE, true, []);

  // iterate over attributes
  var attr, set;
  var attrs = obj.attributes;
  for(var i = 0; i < attrs.length; ++i) {
    attr = attrs[i];
    var value = attr.value;

    // reuse tag class for attribute value if available
    var valueTagClass = asn1$2.Type.PRINTABLESTRING;
    if('valueTagClass' in attr) {
      valueTagClass = attr.valueTagClass;

      if(valueTagClass === asn1$2.Type.UTF8) {
        value = forge$2.util.encodeUtf8(value);
      }
      // FIXME: handle more encodings
    }

    // create a RelativeDistinguishedName set
    // each value in the set is an AttributeTypeAndValue first
    // containing the type (an OID) and second the value
    set = asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.SET, true, [
      asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.SEQUENCE, true, [
        // AttributeType
        asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.OID, false,
          asn1$2.oidToDer(attr.type).getBytes()),
        // AttributeValue
        asn1$2.create(asn1$2.Class.UNIVERSAL, valueTagClass, false, value)
      ])
    ]);
    rval.value.push(set);
  }

  return rval;
}

/**
 * Fills in missing fields in attributes.
 *
 * @param attrs the attributes to fill missing fields in.
 */
function _fillMissingFields(attrs) {
  var attr;
  for(var i = 0; i < attrs.length; ++i) {
    attr = attrs[i];

    // populate missing name
    if(typeof attr.name === 'undefined') {
      if(attr.type && attr.type in pki$2.oids) {
        attr.name = pki$2.oids[attr.type];
      } else if(attr.shortName && attr.shortName in _shortNames) {
        attr.name = pki$2.oids[_shortNames[attr.shortName]];
      }
    }

    // populate missing type (OID)
    if(typeof attr.type === 'undefined') {
      if(attr.name && attr.name in pki$2.oids) {
        attr.type = pki$2.oids[attr.name];
      } else {
        var error = new Error('Attribute type not specified.');
        error.attribute = attr;
        throw error;
      }
    }

    // populate missing shortname
    if(typeof attr.shortName === 'undefined') {
      if(attr.name && attr.name in _shortNames) {
        attr.shortName = _shortNames[attr.name];
      }
    }

    // convert extensions to value
    if(attr.type === oids.extensionRequest) {
      attr.valueConstructed = true;
      attr.valueTagClass = asn1$2.Type.SEQUENCE;
      if(!attr.value && attr.extensions) {
        attr.value = [];
        for(var ei = 0; ei < attr.extensions.length; ++ei) {
          attr.value.push(pki$2.certificateExtensionToAsn1(
            _fillMissingExtensionFields(attr.extensions[ei])));
        }
      }
    }

    if(typeof attr.value === 'undefined') {
      var error = new Error('Attribute value not specified.');
      error.attribute = attr;
      throw error;
    }
  }
}

/**
 * Fills in missing fields in certificate extensions.
 *
 * @param e the extension.
 * @param [options] the options to use.
 *          [cert] the certificate the extensions are for.
 *
 * @return the extension.
 */
function _fillMissingExtensionFields(e, options) {
  options = options || {};

  // populate missing name
  if(typeof e.name === 'undefined') {
    if(e.id && e.id in pki$2.oids) {
      e.name = pki$2.oids[e.id];
    }
  }

  // populate missing id
  if(typeof e.id === 'undefined') {
    if(e.name && e.name in pki$2.oids) {
      e.id = pki$2.oids[e.name];
    } else {
      var error = new Error('Extension ID not specified.');
      error.extension = e;
      throw error;
    }
  }

  if(typeof e.value !== 'undefined') {
    return e;
  }

  // handle missing value:

  // value is a BIT STRING
  if(e.name === 'keyUsage') {
    // build flags
    var unused = 0;
    var b2 = 0x00;
    var b3 = 0x00;
    if(e.digitalSignature) {
      b2 |= 0x80;
      unused = 7;
    }
    if(e.nonRepudiation) {
      b2 |= 0x40;
      unused = 6;
    }
    if(e.keyEncipherment) {
      b2 |= 0x20;
      unused = 5;
    }
    if(e.dataEncipherment) {
      b2 |= 0x10;
      unused = 4;
    }
    if(e.keyAgreement) {
      b2 |= 0x08;
      unused = 3;
    }
    if(e.keyCertSign) {
      b2 |= 0x04;
      unused = 2;
    }
    if(e.cRLSign) {
      b2 |= 0x02;
      unused = 1;
    }
    if(e.encipherOnly) {
      b2 |= 0x01;
      unused = 0;
    }
    if(e.decipherOnly) {
      b3 |= 0x80;
      unused = 7;
    }

    // create bit string
    var value = String.fromCharCode(unused);
    if(b3 !== 0) {
      value += String.fromCharCode(b2) + String.fromCharCode(b3);
    } else if(b2 !== 0) {
      value += String.fromCharCode(b2);
    }
    e.value = asn1$2.create(
      asn1$2.Class.UNIVERSAL, asn1$2.Type.BITSTRING, false, value);
  } else if(e.name === 'basicConstraints') {
    // basicConstraints is a SEQUENCE
    e.value = asn1$2.create(
      asn1$2.Class.UNIVERSAL, asn1$2.Type.SEQUENCE, true, []);
    // cA BOOLEAN flag defaults to false
    if(e.cA) {
      e.value.value.push(asn1$2.create(
        asn1$2.Class.UNIVERSAL, asn1$2.Type.BOOLEAN, false,
        String.fromCharCode(0xFF)));
    }
    if('pathLenConstraint' in e) {
      e.value.value.push(asn1$2.create(
        asn1$2.Class.UNIVERSAL, asn1$2.Type.INTEGER, false,
        asn1$2.integerToDer(e.pathLenConstraint).getBytes()));
    }
  } else if(e.name === 'extKeyUsage') {
    // extKeyUsage is a SEQUENCE of OIDs
    e.value = asn1$2.create(
      asn1$2.Class.UNIVERSAL, asn1$2.Type.SEQUENCE, true, []);
    var seq = e.value.value;
    for(var key in e) {
      if(e[key] !== true) {
        continue;
      }
      // key is name in OID map
      if(key in oids) {
        seq.push(asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.OID,
          false, asn1$2.oidToDer(oids[key]).getBytes()));
      } else if(key.indexOf('.') !== -1) {
        // assume key is an OID
        seq.push(asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.OID,
          false, asn1$2.oidToDer(key).getBytes()));
      }
    }
  } else if(e.name === 'nsCertType') {
    // nsCertType is a BIT STRING
    // build flags
    var unused = 0;
    var b2 = 0x00;

    if(e.client) {
      b2 |= 0x80;
      unused = 7;
    }
    if(e.server) {
      b2 |= 0x40;
      unused = 6;
    }
    if(e.email) {
      b2 |= 0x20;
      unused = 5;
    }
    if(e.objsign) {
      b2 |= 0x10;
      unused = 4;
    }
    if(e.reserved) {
      b2 |= 0x08;
      unused = 3;
    }
    if(e.sslCA) {
      b2 |= 0x04;
      unused = 2;
    }
    if(e.emailCA) {
      b2 |= 0x02;
      unused = 1;
    }
    if(e.objCA) {
      b2 |= 0x01;
      unused = 0;
    }

    // create bit string
    var value = String.fromCharCode(unused);
    if(b2 !== 0) {
      value += String.fromCharCode(b2);
    }
    e.value = asn1$2.create(
      asn1$2.Class.UNIVERSAL, asn1$2.Type.BITSTRING, false, value);
  } else if(e.name === 'subjectAltName' || e.name === 'issuerAltName') {
    // SYNTAX SEQUENCE
    e.value = asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.SEQUENCE, true, []);

    var altName;
    for(var n = 0; n < e.altNames.length; ++n) {
      altName = e.altNames[n];
      var value = altName.value;
      // handle IP
      if(altName.type === 7 && altName.ip) {
        value = forge$2.util.bytesFromIP(altName.ip);
        if(value === null) {
          var error = new Error(
            'Extension "ip" value is not a valid IPv4 or IPv6 address.');
          error.extension = e;
          throw error;
        }
      } else if(altName.type === 8) {
        // handle OID
        if(altName.oid) {
          value = asn1$2.oidToDer(asn1$2.oidToDer(altName.oid));
        } else {
          // deprecated ... convert value to OID
          value = asn1$2.oidToDer(value);
        }
      }
      e.value.value.push(asn1$2.create(
        asn1$2.Class.CONTEXT_SPECIFIC, altName.type, false,
        value));
    }
  } else if(e.name === 'nsComment' && options.cert) {
    // sanity check value is ASCII (req'd) and not too big
    if(!(/^[\x00-\x7F]*$/.test(e.comment)) ||
      (e.comment.length < 1) || (e.comment.length > 128)) {
      throw new Error('Invalid "nsComment" content.');
    }
    // IA5STRING opaque comment
    e.value = asn1$2.create(
      asn1$2.Class.UNIVERSAL, asn1$2.Type.IA5STRING, false, e.comment);
  } else if(e.name === 'subjectKeyIdentifier' && options.cert) {
    var ski = options.cert.generateSubjectKeyIdentifier();
    e.subjectKeyIdentifier = ski.toHex();
    // OCTETSTRING w/digest
    e.value = asn1$2.create(
      asn1$2.Class.UNIVERSAL, asn1$2.Type.OCTETSTRING, false, ski.getBytes());
  } else if(e.name === 'authorityKeyIdentifier' && options.cert) {
    // SYNTAX SEQUENCE
    e.value = asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.SEQUENCE, true, []);
    var seq = e.value.value;

    if(e.keyIdentifier) {
      var keyIdentifier = (e.keyIdentifier === true ?
        options.cert.generateSubjectKeyIdentifier().getBytes() :
        e.keyIdentifier);
      seq.push(
        asn1$2.create(asn1$2.Class.CONTEXT_SPECIFIC, 0, false, keyIdentifier));
    }

    if(e.authorityCertIssuer) {
      var authorityCertIssuer = [
        asn1$2.create(asn1$2.Class.CONTEXT_SPECIFIC, 4, true, [
          _dnToAsn1(e.authorityCertIssuer === true ?
            options.cert.issuer : e.authorityCertIssuer)
        ])
      ];
      seq.push(
        asn1$2.create(asn1$2.Class.CONTEXT_SPECIFIC, 1, true, authorityCertIssuer));
    }

    if(e.serialNumber) {
      var serialNumber = forge$2.util.hexToBytes(e.serialNumber === true ?
        options.cert.serialNumber : e.serialNumber);
      seq.push(
        asn1$2.create(asn1$2.Class.CONTEXT_SPECIFIC, 2, false, serialNumber));
    }
  } else if(e.name === 'cRLDistributionPoints') {
    e.value = asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.SEQUENCE, true, []);
    var seq = e.value.value;

    // Create sub SEQUENCE of DistributionPointName
    var subSeq = asn1$2.create(
      asn1$2.Class.UNIVERSAL, asn1$2.Type.SEQUENCE, true, []);

    // Create fullName CHOICE
    var fullNameGeneralNames = asn1$2.create(
      asn1$2.Class.CONTEXT_SPECIFIC, 0, true, []);
    var altName;
    for(var n = 0; n < e.altNames.length; ++n) {
      altName = e.altNames[n];
      var value = altName.value;
      // handle IP
      if(altName.type === 7 && altName.ip) {
        value = forge$2.util.bytesFromIP(altName.ip);
        if(value === null) {
          var error = new Error(
            'Extension "ip" value is not a valid IPv4 or IPv6 address.');
          error.extension = e;
          throw error;
        }
      } else if(altName.type === 8) {
        // handle OID
        if(altName.oid) {
          value = asn1$2.oidToDer(asn1$2.oidToDer(altName.oid));
        } else {
          // deprecated ... convert value to OID
          value = asn1$2.oidToDer(value);
        }
      }
      fullNameGeneralNames.value.push(asn1$2.create(
        asn1$2.Class.CONTEXT_SPECIFIC, altName.type, false,
        value));
    }

    // Add to the parent SEQUENCE
    subSeq.value.push(asn1$2.create(
      asn1$2.Class.CONTEXT_SPECIFIC, 0, true, [fullNameGeneralNames]));
    seq.push(subSeq);
  }

  // ensure value has been defined by now
  if(typeof e.value === 'undefined') {
    var error = new Error('Extension value not specified.');
    error.extension = e;
    throw error;
  }

  return e;
}

/**
 * Convert signature parameters object to ASN.1
 *
 * @param {String} oid Signature algorithm OID
 * @param params The signature parametrs object
 * @return ASN.1 object representing signature parameters
 */
function _signatureParametersToAsn1(oid, params) {
  switch(oid) {
    case oids['RSASSA-PSS']:
      var parts = [];

      if(params.hash.algorithmOid !== undefined) {
        parts.push(asn1$2.create(asn1$2.Class.CONTEXT_SPECIFIC, 0, true, [
          asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.SEQUENCE, true, [
            asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.OID, false,
              asn1$2.oidToDer(params.hash.algorithmOid).getBytes()),
            asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.NULL, false, '')
          ])
        ]));
      }

      if(params.mgf.algorithmOid !== undefined) {
        parts.push(asn1$2.create(asn1$2.Class.CONTEXT_SPECIFIC, 1, true, [
          asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.SEQUENCE, true, [
            asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.OID, false,
              asn1$2.oidToDer(params.mgf.algorithmOid).getBytes()),
            asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.SEQUENCE, true, [
              asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.OID, false,
                asn1$2.oidToDer(params.mgf.hash.algorithmOid).getBytes()),
              asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.NULL, false, '')
            ])
          ])
        ]));
      }

      if(params.saltLength !== undefined) {
        parts.push(asn1$2.create(asn1$2.Class.CONTEXT_SPECIFIC, 2, true, [
          asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.INTEGER, false,
            asn1$2.integerToDer(params.saltLength).getBytes())
        ]));
      }

      return asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.SEQUENCE, true, parts);

    default:
      return asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.NULL, false, '');
  }
}

/**
 * Converts a certification request's attributes to an ASN.1 set of
 * CRIAttributes.
 *
 * @param csr certification request.
 *
 * @return the ASN.1 set of CRIAttributes.
 */
function _CRIAttributesToAsn1(csr) {
  // create an empty context-specific container
  var rval = asn1$2.create(asn1$2.Class.CONTEXT_SPECIFIC, 0, true, []);

  // no attributes, return empty container
  if(csr.attributes.length === 0) {
    return rval;
  }

  // each attribute has a sequence with a type and a set of values
  var attrs = csr.attributes;
  for(var i = 0; i < attrs.length; ++i) {
    var attr = attrs[i];
    var value = attr.value;

    // reuse tag class for attribute value if available
    var valueTagClass = asn1$2.Type.UTF8;
    if('valueTagClass' in attr) {
      valueTagClass = attr.valueTagClass;
    }
    if(valueTagClass === asn1$2.Type.UTF8) {
      value = forge$2.util.encodeUtf8(value);
    }
    var valueConstructed = false;
    if('valueConstructed' in attr) {
      valueConstructed = attr.valueConstructed;
    }
    // FIXME: handle more encodings

    // create a RelativeDistinguishedName set
    // each value in the set is an AttributeTypeAndValue first
    // containing the type (an OID) and second the value
    var seq = asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.SEQUENCE, true, [
      // AttributeType
      asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.OID, false,
        asn1$2.oidToDer(attr.type).getBytes()),
      asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.SET, true, [
        // AttributeValue
        asn1$2.create(
          asn1$2.Class.UNIVERSAL, valueTagClass, valueConstructed, value)
      ])
    ]);
    rval.value.push(seq);
  }

  return rval;
}

var jan_1_1950 = new Date('1950-01-01T00:00:00Z');
var jan_1_2050 = new Date('2050-01-01T00:00:00Z');

/**
 * Converts a Date object to ASN.1
 * Handles the different format before and after 1st January 2050
 *
 * @param date date object.
 *
 * @return the ASN.1 object representing the date.
 */
function _dateToAsn1(date) {
  if(date >= jan_1_1950 && date < jan_1_2050) {
    return asn1$2.create(
      asn1$2.Class.UNIVERSAL, asn1$2.Type.UTCTIME, false,
      asn1$2.dateToUtcTime(date));
  } else {
    return asn1$2.create(
      asn1$2.Class.UNIVERSAL, asn1$2.Type.GENERALIZEDTIME, false,
      asn1$2.dateToGeneralizedTime(date));
  }
}

/**
 * Gets the ASN.1 TBSCertificate part of an X.509v3 certificate.
 *
 * @param cert the certificate.
 *
 * @return the asn1 TBSCertificate.
 */
pki$2.getTBSCertificate = function(cert) {
  // TBSCertificate
  var notBefore = _dateToAsn1(cert.validity.notBefore);
  var notAfter = _dateToAsn1(cert.validity.notAfter);
  var tbs = asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.SEQUENCE, true, [
    // version
    asn1$2.create(asn1$2.Class.CONTEXT_SPECIFIC, 0, true, [
      // integer
      asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.INTEGER, false,
        asn1$2.integerToDer(cert.version).getBytes())
    ]),
    // serialNumber
    asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.INTEGER, false,
      forge$2.util.hexToBytes(cert.serialNumber)),
    // signature
    asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.SEQUENCE, true, [
      // algorithm
      asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.OID, false,
        asn1$2.oidToDer(cert.siginfo.algorithmOid).getBytes()),
      // parameters
      _signatureParametersToAsn1(
        cert.siginfo.algorithmOid, cert.siginfo.parameters)
    ]),
    // issuer
    _dnToAsn1(cert.issuer),
    // validity
    asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.SEQUENCE, true, [
      notBefore,
      notAfter
    ]),
    // subject
    _dnToAsn1(cert.subject),
    // SubjectPublicKeyInfo
    pki$2.publicKeyToAsn1(cert.publicKey)
  ]);

  if(cert.issuer.uniqueId) {
    // issuerUniqueID (optional)
    tbs.value.push(
      asn1$2.create(asn1$2.Class.CONTEXT_SPECIFIC, 1, true, [
        asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.BITSTRING, false,
          // TODO: support arbitrary bit length ids
          String.fromCharCode(0x00) +
          cert.issuer.uniqueId
        )
      ])
    );
  }
  if(cert.subject.uniqueId) {
    // subjectUniqueID (optional)
    tbs.value.push(
      asn1$2.create(asn1$2.Class.CONTEXT_SPECIFIC, 2, true, [
        asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.BITSTRING, false,
          // TODO: support arbitrary bit length ids
          String.fromCharCode(0x00) +
          cert.subject.uniqueId
        )
      ])
    );
  }

  if(cert.extensions.length > 0) {
    // extensions (optional)
    tbs.value.push(pki$2.certificateExtensionsToAsn1(cert.extensions));
  }

  return tbs;
};

/**
 * Gets the ASN.1 CertificationRequestInfo part of a
 * PKCS#10 CertificationRequest.
 *
 * @param csr the certification request.
 *
 * @return the asn1 CertificationRequestInfo.
 */
pki$2.getCertificationRequestInfo = function(csr) {
  // CertificationRequestInfo
  var cri = asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.SEQUENCE, true, [
    // version
    asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.INTEGER, false,
      asn1$2.integerToDer(csr.version).getBytes()),
    // subject
    _dnToAsn1(csr.subject),
    // SubjectPublicKeyInfo
    pki$2.publicKeyToAsn1(csr.publicKey),
    // attributes
    _CRIAttributesToAsn1(csr)
  ]);

  return cri;
};

/**
 * Converts a DistinguishedName (subject or issuer) to an ASN.1 object.
 *
 * @param dn the DistinguishedName.
 *
 * @return the asn1 representation of a DistinguishedName.
 */
pki$2.distinguishedNameToAsn1 = function(dn) {
  return _dnToAsn1(dn);
};

/**
 * Converts an X.509v3 RSA certificate to an ASN.1 object.
 *
 * @param cert the certificate.
 *
 * @return the asn1 representation of an X.509v3 RSA certificate.
 */
pki$2.certificateToAsn1 = function(cert) {
  // prefer cached TBSCertificate over generating one
  var tbsCertificate = cert.tbsCertificate || pki$2.getTBSCertificate(cert);

  // Certificate
  return asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.SEQUENCE, true, [
    // TBSCertificate
    tbsCertificate,
    // AlgorithmIdentifier (signature algorithm)
    asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.SEQUENCE, true, [
      // algorithm
      asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.OID, false,
        asn1$2.oidToDer(cert.signatureOid).getBytes()),
      // parameters
      _signatureParametersToAsn1(cert.signatureOid, cert.signatureParameters)
    ]),
    // SignatureValue
    asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.BITSTRING, false,
      String.fromCharCode(0x00) + cert.signature)
  ]);
};

/**
 * Converts X.509v3 certificate extensions to ASN.1.
 *
 * @param exts the extensions to convert.
 *
 * @return the extensions in ASN.1 format.
 */
pki$2.certificateExtensionsToAsn1 = function(exts) {
  // create top-level extension container
  var rval = asn1$2.create(asn1$2.Class.CONTEXT_SPECIFIC, 3, true, []);

  // create extension sequence (stores a sequence for each extension)
  var seq = asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.SEQUENCE, true, []);
  rval.value.push(seq);

  for(var i = 0; i < exts.length; ++i) {
    seq.value.push(pki$2.certificateExtensionToAsn1(exts[i]));
  }

  return rval;
};

/**
 * Converts a single certificate extension to ASN.1.
 *
 * @param ext the extension to convert.
 *
 * @return the extension in ASN.1 format.
 */
pki$2.certificateExtensionToAsn1 = function(ext) {
  // create a sequence for each extension
  var extseq = asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.SEQUENCE, true, []);

  // extnID (OID)
  extseq.value.push(asn1$2.create(
    asn1$2.Class.UNIVERSAL, asn1$2.Type.OID, false,
    asn1$2.oidToDer(ext.id).getBytes()));

  // critical defaults to false
  if(ext.critical) {
    // critical BOOLEAN DEFAULT FALSE
    extseq.value.push(asn1$2.create(
      asn1$2.Class.UNIVERSAL, asn1$2.Type.BOOLEAN, false,
      String.fromCharCode(0xFF)));
  }

  var value = ext.value;
  if(typeof ext.value !== 'string') {
    // value is asn.1
    value = asn1$2.toDer(value).getBytes();
  }

  // extnValue (OCTET STRING)
  extseq.value.push(asn1$2.create(
    asn1$2.Class.UNIVERSAL, asn1$2.Type.OCTETSTRING, false, value));

  return extseq;
};

/**
 * Converts a PKCS#10 certification request to an ASN.1 object.
 *
 * @param csr the certification request.
 *
 * @return the asn1 representation of a certification request.
 */
pki$2.certificationRequestToAsn1 = function(csr) {
  // prefer cached CertificationRequestInfo over generating one
  var cri = csr.certificationRequestInfo ||
    pki$2.getCertificationRequestInfo(csr);

  // Certificate
  return asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.SEQUENCE, true, [
    // CertificationRequestInfo
    cri,
    // AlgorithmIdentifier (signature algorithm)
    asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.SEQUENCE, true, [
      // algorithm
      asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.OID, false,
        asn1$2.oidToDer(csr.signatureOid).getBytes()),
      // parameters
      _signatureParametersToAsn1(csr.signatureOid, csr.signatureParameters)
    ]),
    // signature
    asn1$2.create(asn1$2.Class.UNIVERSAL, asn1$2.Type.BITSTRING, false,
      String.fromCharCode(0x00) + csr.signature)
  ]);
};

/**
 * Creates a CA store.
 *
 * @param certs an optional array of certificate objects or PEM-formatted
 *          certificate strings to add to the CA store.
 *
 * @return the CA store.
 */
pki$2.createCaStore = function(certs) {
  // create CA store
  var caStore = {
    // stored certificates
    certs: {}
  };

  /**
   * Gets the certificate that issued the passed certificate or its
   * 'parent'.
   *
   * @param cert the certificate to get the parent for.
   *
   * @return the parent certificate or null if none was found.
   */
  caStore.getIssuer = function(cert) {
    var rval = getBySubject(cert.issuer);

    // see if there are multiple matches
    /*if(forge.util.isArray(rval)) {
      // TODO: resolve multiple matches by checking
      // authorityKey/subjectKey/issuerUniqueID/other identifiers, etc.
      // FIXME: or alternatively do authority key mapping
      // if possible (X.509v1 certs can't work?)
      throw new Error('Resolving multiple issuer matches not implemented yet.');
    }*/

    return rval;
  };

  /**
   * Adds a trusted certificate to the store.
   *
   * @param cert the certificate to add as a trusted certificate (either a
   *          pki.certificate object or a PEM-formatted certificate).
   */
  caStore.addCertificate = function(cert) {
    // convert from pem if necessary
    if(typeof cert === 'string') {
      cert = forge$2.pki.certificateFromPem(cert);
    }

    ensureSubjectHasHash(cert.subject);

    if(!caStore.hasCertificate(cert)) { // avoid duplicate certificates in store
      if(cert.subject.hash in caStore.certs) {
        // subject hash already exists, append to array
        var tmp = caStore.certs[cert.subject.hash];
        if(!forge$2.util.isArray(tmp)) {
          tmp = [tmp];
        }
        tmp.push(cert);
        caStore.certs[cert.subject.hash] = tmp;
      } else {
        caStore.certs[cert.subject.hash] = cert;
      }
    }
  };

  /**
   * Checks to see if the given certificate is in the store.
   *
   * @param cert the certificate to check (either a pki.certificate or a
   *          PEM-formatted certificate).
   *
   * @return true if the certificate is in the store, false if not.
   */
  caStore.hasCertificate = function(cert) {
    // convert from pem if necessary
    if(typeof cert === 'string') {
      cert = forge$2.pki.certificateFromPem(cert);
    }

    var match = getBySubject(cert.subject);
    if(!match) {
      return false;
    }
    if(!forge$2.util.isArray(match)) {
      match = [match];
    }
    // compare DER-encoding of certificates
    var der1 = asn1$2.toDer(pki$2.certificateToAsn1(cert)).getBytes();
    for(var i = 0; i < match.length; ++i) {
      var der2 = asn1$2.toDer(pki$2.certificateToAsn1(match[i])).getBytes();
      if(der1 === der2) {
        return true;
      }
    }
    return false;
  };

  /**
   * Lists all of the certificates kept in the store.
   *
   * @return an array of all of the pki.certificate objects in the store.
   */
  caStore.listAllCertificates = function() {
    var certList = [];

    for(var hash in caStore.certs) {
      if(caStore.certs.hasOwnProperty(hash)) {
        var value = caStore.certs[hash];
        if(!forge$2.util.isArray(value)) {
          certList.push(value);
        } else {
          for(var i = 0; i < value.length; ++i) {
            certList.push(value[i]);
          }
        }
      }
    }

    return certList;
  };

  /**
   * Removes a certificate from the store.
   *
   * @param cert the certificate to remove (either a pki.certificate or a
   *          PEM-formatted certificate).
   *
   * @return the certificate that was removed or null if the certificate
   *           wasn't in store.
   */
  caStore.removeCertificate = function(cert) {
    var result;

    // convert from pem if necessary
    if(typeof cert === 'string') {
      cert = forge$2.pki.certificateFromPem(cert);
    }
    ensureSubjectHasHash(cert.subject);
    if(!caStore.hasCertificate(cert)) {
      return null;
    }

    var match = getBySubject(cert.subject);

    if(!forge$2.util.isArray(match)) {
      result = caStore.certs[cert.subject.hash];
      delete caStore.certs[cert.subject.hash];
      return result;
    }

    // compare DER-encoding of certificates
    var der1 = asn1$2.toDer(pki$2.certificateToAsn1(cert)).getBytes();
    for(var i = 0; i < match.length; ++i) {
      var der2 = asn1$2.toDer(pki$2.certificateToAsn1(match[i])).getBytes();
      if(der1 === der2) {
        result = match[i];
        match.splice(i, 1);
      }
    }
    if(match.length === 0) {
      delete caStore.certs[cert.subject.hash];
    }

    return result;
  };

  function getBySubject(subject) {
    ensureSubjectHasHash(subject);
    return caStore.certs[subject.hash] || null;
  }

  function ensureSubjectHasHash(subject) {
    // produce subject hash if it doesn't exist
    if(!subject.hash) {
      var md = forge$2.md.sha1.create();
      subject.attributes = pki$2.RDNAttributesAsArray(_dnToAsn1(subject), md);
      subject.hash = md.digest().toHex();
    }
  }

  // auto-add passed in certs
  if(certs) {
    // parse PEM-formatted certificates as necessary
    for(var i = 0; i < certs.length; ++i) {
      var cert = certs[i];
      caStore.addCertificate(cert);
    }
  }

  return caStore;
};

/**
 * Certificate verification errors, based on TLS.
 */
pki$2.certificateError = {
  bad_certificate: 'forge.pki.BadCertificate',
  unsupported_certificate: 'forge.pki.UnsupportedCertificate',
  certificate_revoked: 'forge.pki.CertificateRevoked',
  certificate_expired: 'forge.pki.CertificateExpired',
  certificate_unknown: 'forge.pki.CertificateUnknown',
  unknown_ca: 'forge.pki.UnknownCertificateAuthority'
};

/**
 * Verifies a certificate chain against the given Certificate Authority store
 * with an optional custom verify callback.
 *
 * @param caStore a certificate store to verify against.
 * @param chain the certificate chain to verify, with the root or highest
 *          authority at the end (an array of certificates).
 * @param options a callback to be called for every certificate in the chain or
 *                  an object with:
 *                  verify a callback to be called for every certificate in the
 *                    chain
 *                  validityCheckDate the date against which the certificate
 *                    validity period should be checked. Pass null to not check
 *                    the validity period. By default, the current date is used.
 *
 * The verify callback has the following signature:
 *
 * verified - Set to true if certificate was verified, otherwise the
 *   pki.certificateError for why the certificate failed.
 * depth - The current index in the chain, where 0 is the end point's cert.
 * certs - The certificate chain, *NOTE* an empty chain indicates an anonymous
 *   end point.
 *
 * The function returns true on success and on failure either the appropriate
 * pki.certificateError or an object with 'error' set to the appropriate
 * pki.certificateError and 'message' set to a custom error message.
 *
 * @return true if successful, error thrown if not.
 */
pki$2.verifyCertificateChain = function(caStore, chain, options) {
  /* From: RFC3280 - Internet X.509 Public Key Infrastructure Certificate
    Section 6: Certification Path Validation
    See inline parentheticals related to this particular implementation.

    The primary goal of path validation is to verify the binding between
    a subject distinguished name or a subject alternative name and subject
    public key, as represented in the end entity certificate, based on the
    public key of the trust anchor. This requires obtaining a sequence of
    certificates that support that binding. That sequence should be provided
    in the passed 'chain'. The trust anchor should be in the given CA
    store. The 'end entity' certificate is the certificate provided by the
    end point (typically a server) and is the first in the chain.

    To meet this goal, the path validation process verifies, among other
    things, that a prospective certification path (a sequence of n
    certificates or a 'chain') satisfies the following conditions:

    (a) for all x in {1, ..., n-1}, the subject of certificate x is
          the issuer of certificate x+1;

    (b) certificate 1 is issued by the trust anchor;

    (c) certificate n is the certificate to be validated; and

    (d) for all x in {1, ..., n}, the certificate was valid at the
          time in question.

    Note that here 'n' is index 0 in the chain and 1 is the last certificate
    in the chain and it must be signed by a certificate in the connection's
    CA store.

    The path validation process also determines the set of certificate
    policies that are valid for this path, based on the certificate policies
    extension, policy mapping extension, policy constraints extension, and
    inhibit any-policy extension.

    Note: Policy mapping extension not supported (Not Required).

    Note: If the certificate has an unsupported critical extension, then it
    must be rejected.

    Note: A certificate is self-issued if the DNs that appear in the subject
    and issuer fields are identical and are not empty.

    The path validation algorithm assumes the following seven inputs are
    provided to the path processing logic. What this specific implementation
    will use is provided parenthetically:

    (a) a prospective certification path of length n (the 'chain')
    (b) the current date/time: ('now').
    (c) user-initial-policy-set: A set of certificate policy identifiers
          naming the policies that are acceptable to the certificate user.
          The user-initial-policy-set contains the special value any-policy
          if the user is not concerned about certificate policy
          (Not implemented. Any policy is accepted).
    (d) trust anchor information, describing a CA that serves as a trust
          anchor for the certification path. The trust anchor information
          includes:

      (1)  the trusted issuer name,
      (2)  the trusted public key algorithm,
      (3)  the trusted public key, and
      (4)  optionally, the trusted public key parameters associated
             with the public key.

      (Trust anchors are provided via certificates in the CA store).

      The trust anchor information may be provided to the path processing
      procedure in the form of a self-signed certificate. The trusted anchor
      information is trusted because it was delivered to the path processing
      procedure by some trustworthy out-of-band procedure. If the trusted
      public key algorithm requires parameters, then the parameters are
      provided along with the trusted public key (No parameters used in this
      implementation).

    (e) initial-policy-mapping-inhibit, which indicates if policy mapping is
          allowed in the certification path.
          (Not implemented, no policy checking)

    (f) initial-explicit-policy, which indicates if the path must be valid
          for at least one of the certificate policies in the user-initial-
          policy-set.
          (Not implemented, no policy checking)

    (g) initial-any-policy-inhibit, which indicates whether the
          anyPolicy OID should be processed if it is included in a
          certificate.
          (Not implemented, so any policy is valid provided that it is
          not marked as critical) */

  /* Basic Path Processing:

    For each certificate in the 'chain', the following is checked:

    1. The certificate validity period includes the current time.
    2. The certificate was signed by its parent (where the parent is either
       the next in the chain or from the CA store). Allow processing to
       continue to the next step if no parent is found but the certificate is
       in the CA store.
    3. TODO: The certificate has not been revoked.
    4. The certificate issuer name matches the parent's subject name.
    5. TODO: If the certificate is self-issued and not the final certificate
       in the chain, skip this step, otherwise verify that the subject name
       is within one of the permitted subtrees of X.500 distinguished names
       and that each of the alternative names in the subjectAltName extension
       (critical or non-critical) is within one of the permitted subtrees for
       that name type.
    6. TODO: If the certificate is self-issued and not the final certificate
       in the chain, skip this step, otherwise verify that the subject name
       is not within one of the excluded subtrees for X.500 distinguished
       names and none of the subjectAltName extension names are excluded for
       that name type.
    7. The other steps in the algorithm for basic path processing involve
       handling the policy extension which is not presently supported in this
       implementation. Instead, if a critical policy extension is found, the
       certificate is rejected as not supported.
    8. If the certificate is not the first or if its the only certificate in
       the chain (having no parent from the CA store or is self-signed) and it
       has a critical key usage extension, verify that the keyCertSign bit is
       set. If the key usage extension exists, verify that the basic
       constraints extension exists. If the basic constraints extension exists,
       verify that the cA flag is set. If pathLenConstraint is set, ensure that
       the number of certificates that precede in the chain (come earlier
       in the chain as implemented below), excluding the very first in the
       chain (typically the end-entity one), isn't greater than the
       pathLenConstraint. This constraint limits the number of intermediate
       CAs that may appear below a CA before only end-entity certificates
       may be issued. */

  // if a verify callback is passed as the third parameter, package it within
  // the options object. This is to support a legacy function signature that
  // expected the verify callback as the third parameter.
  if(typeof options === 'function') {
    options = {verify: options};
  }
  options = options || {};

  // copy cert chain references to another array to protect against changes
  // in verify callback
  chain = chain.slice(0);
  var certs = chain.slice(0);

  var validityCheckDate = options.validityCheckDate;
  // if no validityCheckDate is specified, default to the current date. Make
  // sure to maintain the value null because it indicates that the validity
  // period should not be checked.
  if(typeof validityCheckDate === 'undefined') {
    validityCheckDate = new Date();
  }

  // verify each cert in the chain using its parent, where the parent
  // is either the next in the chain or from the CA store
  var first = true;
  var error = null;
  var depth = 0;
  do {
    var cert = chain.shift();
    var parent = null;
    var selfSigned = false;

    if(validityCheckDate) {
      // 1. check valid time
      if(validityCheckDate < cert.validity.notBefore ||
         validityCheckDate > cert.validity.notAfter) {
        error = {
          message: 'Certificate is not valid yet or has expired.',
          error: pki$2.certificateError.certificate_expired,
          notBefore: cert.validity.notBefore,
          notAfter: cert.validity.notAfter,
          // TODO: we might want to reconsider renaming 'now' to
          // 'validityCheckDate' should this API be changed in the future.
          now: validityCheckDate
        };
      }
    }

    // 2. verify with parent from chain or CA store
    if(error === null) {
      parent = chain[0] || caStore.getIssuer(cert);
      if(parent === null) {
        // check for self-signed cert
        if(cert.isIssuer(cert)) {
          selfSigned = true;
          parent = cert;
        }
      }

      if(parent) {
        // FIXME: current CA store implementation might have multiple
        // certificates where the issuer can't be determined from the
        // certificate (happens rarely with, eg: old certificates) so normalize
        // by always putting parents into an array
        // TODO: there's may be an extreme degenerate case currently uncovered
        // where an old intermediate certificate seems to have a matching parent
        // but none of the parents actually verify ... but the intermediate
        // is in the CA and it should pass this check; needs investigation
        var parents = parent;
        if(!forge$2.util.isArray(parents)) {
          parents = [parents];
        }

        // try to verify with each possible parent (typically only one)
        var verified = false;
        while(!verified && parents.length > 0) {
          parent = parents.shift();
          try {
            verified = parent.verify(cert);
          } catch(ex) {
            // failure to verify, don't care why, try next one
          }
        }

        if(!verified) {
          error = {
            message: 'Certificate signature is invalid.',
            error: pki$2.certificateError.bad_certificate
          };
        }
      }

      if(error === null && (!parent || selfSigned) &&
        !caStore.hasCertificate(cert)) {
        // no parent issuer and certificate itself is not trusted
        error = {
          message: 'Certificate is not trusted.',
          error: pki$2.certificateError.unknown_ca
        };
      }
    }

    // TODO: 3. check revoked

    // 4. check for matching issuer/subject
    if(error === null && parent && !cert.isIssuer(parent)) {
      // parent is not issuer
      error = {
        message: 'Certificate issuer is invalid.',
        error: pki$2.certificateError.bad_certificate
      };
    }

    // 5. TODO: check names with permitted names tree

    // 6. TODO: check names against excluded names tree

    // 7. check for unsupported critical extensions
    if(error === null) {
      // supported extensions
      var se = {
        keyUsage: true,
        basicConstraints: true
      };
      for(var i = 0; error === null && i < cert.extensions.length; ++i) {
        var ext = cert.extensions[i];
        if(ext.critical && !(ext.name in se)) {
          error = {
            message:
              'Certificate has an unsupported critical extension.',
            error: pki$2.certificateError.unsupported_certificate
          };
        }
      }
    }

    // 8. check for CA if cert is not first or is the only certificate
    // remaining in chain with no parent or is self-signed
    if(error === null &&
      (!first || (chain.length === 0 && (!parent || selfSigned)))) {
      // first check keyUsage extension and then basic constraints
      var bcExt = cert.getExtension('basicConstraints');
      var keyUsageExt = cert.getExtension('keyUsage');
      if(keyUsageExt !== null) {
        // keyCertSign must be true and there must be a basic
        // constraints extension
        if(!keyUsageExt.keyCertSign || bcExt === null) {
          // bad certificate
          error = {
            message:
              'Certificate keyUsage or basicConstraints conflict ' +
              'or indicate that the certificate is not a CA. ' +
              'If the certificate is the only one in the chain or ' +
              'isn\'t the first then the certificate must be a ' +
              'valid CA.',
            error: pki$2.certificateError.bad_certificate
          };
        }
      }
      // basic constraints cA flag must be set
      if(error === null && bcExt !== null && !bcExt.cA) {
        // bad certificate
        error = {
          message:
            'Certificate basicConstraints indicates the certificate ' +
            'is not a CA.',
          error: pki$2.certificateError.bad_certificate
        };
      }
      // if error is not null and keyUsage is available, then we know it
      // has keyCertSign and there is a basic constraints extension too,
      // which means we can check pathLenConstraint (if it exists)
      if(error === null && keyUsageExt !== null &&
        'pathLenConstraint' in bcExt) {
        // pathLen is the maximum # of intermediate CA certs that can be
        // found between the current certificate and the end-entity (depth 0)
        // certificate; this number does not include the end-entity (depth 0,
        // last in the chain) even if it happens to be a CA certificate itself
        var pathLen = depth - 1;
        if(pathLen > bcExt.pathLenConstraint) {
          // pathLenConstraint violated, bad certificate
          error = {
            message:
              'Certificate basicConstraints pathLenConstraint violated.',
            error: pki$2.certificateError.bad_certificate
          };
        }
      }
    }

    // call application callback
    var vfd = (error === null) ? true : error.error;
    var ret = options.verify ? options.verify(vfd, depth, certs) : vfd;
    if(ret === true) {
      // clear any set error
      error = null;
    } else {
      // if passed basic tests, set default message and alert
      if(vfd === true) {
        error = {
          message: 'The application rejected the certificate.',
          error: pki$2.certificateError.bad_certificate
        };
      }

      // check for custom error info
      if(ret || ret === 0) {
        // set custom message and error
        if(typeof ret === 'object' && !forge$2.util.isArray(ret)) {
          if(ret.message) {
            error.message = ret.message;
          }
          if(ret.error) {
            error.error = ret.error;
          }
        } else if(typeof ret === 'string') {
          // set custom error
          error.error = ret;
        }
      }

      // throw error
      throw error;
    }

    // no longer first cert in chain
    first = false;
    ++depth;
  } while(chain.length > 0);

  return true;
};

/**
 * Javascript implementation of PKCS#12.
 *
 * @author Dave Longley
 * @author Stefan Siegl <stesie@brokenpipe.de>
 *
 * Copyright (c) 2010-2014 Digital Bazaar, Inc.
 * Copyright (c) 2012 Stefan Siegl <stesie@brokenpipe.de>
 *
 * The ASN.1 representation of PKCS#12 is as follows
 * (see ftp://ftp.rsasecurity.com/pub/pkcs/pkcs-12/pkcs-12-tc1.pdf for details)
 *
 * PFX ::= SEQUENCE {
 *   version  INTEGER {v3(3)}(v3,...),
 *   authSafe ContentInfo,
 *   macData  MacData OPTIONAL
 * }
 *
 * MacData ::= SEQUENCE {
 *   mac DigestInfo,
 *   macSalt OCTET STRING,
 *   iterations INTEGER DEFAULT 1
 * }
 * Note: The iterations default is for historical reasons and its use is
 * deprecated. A higher value, like 1024, is recommended.
 *
 * DigestInfo is defined in PKCS#7 as follows:
 *
 * DigestInfo ::= SEQUENCE {
 *   digestAlgorithm DigestAlgorithmIdentifier,
 *   digest Digest
 * }
 *
 * DigestAlgorithmIdentifier ::= AlgorithmIdentifier
 *
 * The AlgorithmIdentifier contains an Object Identifier (OID) and parameters
 * for the algorithm, if any. In the case of SHA1 there is none.
 *
 * AlgorithmIdentifer ::= SEQUENCE {
 *    algorithm OBJECT IDENTIFIER,
 *    parameters ANY DEFINED BY algorithm OPTIONAL
 * }
 *
 * Digest ::= OCTET STRING
 *
 *
 * ContentInfo ::= SEQUENCE {
 *   contentType ContentType,
 *   content     [0] EXPLICIT ANY DEFINED BY contentType OPTIONAL
 * }
 *
 * ContentType ::= OBJECT IDENTIFIER
 *
 * AuthenticatedSafe ::= SEQUENCE OF ContentInfo
 * -- Data if unencrypted
 * -- EncryptedData if password-encrypted
 * -- EnvelopedData if public key-encrypted
 *
 *
 * SafeContents ::= SEQUENCE OF SafeBag
 *
 * SafeBag ::= SEQUENCE {
 *   bagId     BAG-TYPE.&id ({PKCS12BagSet})
 *   bagValue  [0] EXPLICIT BAG-TYPE.&Type({PKCS12BagSet}{@bagId}),
 *   bagAttributes SET OF PKCS12Attribute OPTIONAL
 * }
 *
 * PKCS12Attribute ::= SEQUENCE {
 *   attrId ATTRIBUTE.&id ({PKCS12AttrSet}),
 *   attrValues SET OF ATTRIBUTE.&Type ({PKCS12AttrSet}{@attrId})
 * } -- This type is compatible with the X.500 type 'Attribute'
 *
 * PKCS12AttrSet ATTRIBUTE ::= {
 *   friendlyName | -- from PKCS #9
 *   localKeyId, -- from PKCS #9
 *   ... -- Other attributes are allowed
 * }
 *
 * CertBag ::= SEQUENCE {
 *   certId    BAG-TYPE.&id   ({CertTypes}),
 *   certValue [0] EXPLICIT BAG-TYPE.&Type ({CertTypes}{@certId})
 * }
 *
 * x509Certificate BAG-TYPE ::= {OCTET STRING IDENTIFIED BY {certTypes 1}}
 *   -- DER-encoded X.509 certificate stored in OCTET STRING
 *
 * sdsiCertificate BAG-TYPE ::= {IA5String IDENTIFIED BY {certTypes 2}}
 * -- Base64-encoded SDSI certificate stored in IA5String
 *
 * CertTypes BAG-TYPE ::= {
 *   x509Certificate |
 *   sdsiCertificate,
 *   ... -- For future extensions
 * }
 */

var forge$1 = forge$s;











// shortcut for asn.1 & PKI API
var asn1$1 = forge$1.asn1;
var pki$1 = forge$1.pki;

// shortcut for PKCS#12 API
var p12 = forge$1.pkcs12 = forge$1.pkcs12 || {};

var contentInfoValidator = {
  name: 'ContentInfo',
  tagClass: asn1$1.Class.UNIVERSAL,
  type: asn1$1.Type.SEQUENCE,  // a ContentInfo
  constructed: true,
  value: [{
    name: 'ContentInfo.contentType',
    tagClass: asn1$1.Class.UNIVERSAL,
    type: asn1$1.Type.OID,
    constructed: false,
    capture: 'contentType'
  }, {
    name: 'ContentInfo.content',
    tagClass: asn1$1.Class.CONTEXT_SPECIFIC,
    constructed: true,
    captureAsn1: 'content'
  }]
};

var pfxValidator = {
  name: 'PFX',
  tagClass: asn1$1.Class.UNIVERSAL,
  type: asn1$1.Type.SEQUENCE,
  constructed: true,
  value: [{
    name: 'PFX.version',
    tagClass: asn1$1.Class.UNIVERSAL,
    type: asn1$1.Type.INTEGER,
    constructed: false,
    capture: 'version'
  },
  contentInfoValidator, {
    name: 'PFX.macData',
    tagClass: asn1$1.Class.UNIVERSAL,
    type: asn1$1.Type.SEQUENCE,
    constructed: true,
    optional: true,
    captureAsn1: 'mac',
    value: [{
      name: 'PFX.macData.mac',
      tagClass: asn1$1.Class.UNIVERSAL,
      type: asn1$1.Type.SEQUENCE,  // DigestInfo
      constructed: true,
      value: [{
        name: 'PFX.macData.mac.digestAlgorithm',
        tagClass: asn1$1.Class.UNIVERSAL,
        type: asn1$1.Type.SEQUENCE,  // DigestAlgorithmIdentifier
        constructed: true,
        value: [{
          name: 'PFX.macData.mac.digestAlgorithm.algorithm',
          tagClass: asn1$1.Class.UNIVERSAL,
          type: asn1$1.Type.OID,
          constructed: false,
          capture: 'macAlgorithm'
        }, {
          name: 'PFX.macData.mac.digestAlgorithm.parameters',
          tagClass: asn1$1.Class.UNIVERSAL,
          captureAsn1: 'macAlgorithmParameters'
        }]
      }, {
        name: 'PFX.macData.mac.digest',
        tagClass: asn1$1.Class.UNIVERSAL,
        type: asn1$1.Type.OCTETSTRING,
        constructed: false,
        capture: 'macDigest'
      }]
    }, {
      name: 'PFX.macData.macSalt',
      tagClass: asn1$1.Class.UNIVERSAL,
      type: asn1$1.Type.OCTETSTRING,
      constructed: false,
      capture: 'macSalt'
    }, {
      name: 'PFX.macData.iterations',
      tagClass: asn1$1.Class.UNIVERSAL,
      type: asn1$1.Type.INTEGER,
      constructed: false,
      optional: true,
      capture: 'macIterations'
    }]
  }]
};

var safeBagValidator = {
  name: 'SafeBag',
  tagClass: asn1$1.Class.UNIVERSAL,
  type: asn1$1.Type.SEQUENCE,
  constructed: true,
  value: [{
    name: 'SafeBag.bagId',
    tagClass: asn1$1.Class.UNIVERSAL,
    type: asn1$1.Type.OID,
    constructed: false,
    capture: 'bagId'
  }, {
    name: 'SafeBag.bagValue',
    tagClass: asn1$1.Class.CONTEXT_SPECIFIC,
    constructed: true,
    captureAsn1: 'bagValue'
  }, {
    name: 'SafeBag.bagAttributes',
    tagClass: asn1$1.Class.UNIVERSAL,
    type: asn1$1.Type.SET,
    constructed: true,
    optional: true,
    capture: 'bagAttributes'
  }]
};

var attributeValidator = {
  name: 'Attribute',
  tagClass: asn1$1.Class.UNIVERSAL,
  type: asn1$1.Type.SEQUENCE,
  constructed: true,
  value: [{
    name: 'Attribute.attrId',
    tagClass: asn1$1.Class.UNIVERSAL,
    type: asn1$1.Type.OID,
    constructed: false,
    capture: 'oid'
  }, {
    name: 'Attribute.attrValues',
    tagClass: asn1$1.Class.UNIVERSAL,
    type: asn1$1.Type.SET,
    constructed: true,
    capture: 'values'
  }]
};

var certBagValidator = {
  name: 'CertBag',
  tagClass: asn1$1.Class.UNIVERSAL,
  type: asn1$1.Type.SEQUENCE,
  constructed: true,
  value: [{
    name: 'CertBag.certId',
    tagClass: asn1$1.Class.UNIVERSAL,
    type: asn1$1.Type.OID,
    constructed: false,
    capture: 'certId'
  }, {
    name: 'CertBag.certValue',
    tagClass: asn1$1.Class.CONTEXT_SPECIFIC,
    constructed: true,
    /* So far we only support X.509 certificates (which are wrapped in
       an OCTET STRING, hence hard code that here). */
    value: [{
      name: 'CertBag.certValue[0]',
      tagClass: asn1$1.Class.UNIVERSAL,
      type: asn1$1.Class.OCTETSTRING,
      constructed: false,
      capture: 'cert'
    }]
  }]
};

/**
 * Search SafeContents structure for bags with matching attributes.
 *
 * The search can optionally be narrowed by a certain bag type.
 *
 * @param safeContents the SafeContents structure to search in.
 * @param attrName the name of the attribute to compare against.
 * @param attrValue the attribute value to search for.
 * @param [bagType] bag type to narrow search by.
 *
 * @return an array of matching bags.
 */
function _getBagsByAttribute(safeContents, attrName, attrValue, bagType) {
  var result = [];

  for(var i = 0; i < safeContents.length; i++) {
    for(var j = 0; j < safeContents[i].safeBags.length; j++) {
      var bag = safeContents[i].safeBags[j];
      if(bagType !== undefined && bag.type !== bagType) {
        continue;
      }
      // only filter by bag type, no attribute specified
      if(attrName === null) {
        result.push(bag);
        continue;
      }
      if(bag.attributes[attrName] !== undefined &&
        bag.attributes[attrName].indexOf(attrValue) >= 0) {
        result.push(bag);
      }
    }
  }

  return result;
}

/**
 * Converts a PKCS#12 PFX in ASN.1 notation into a PFX object.
 *
 * @param obj The PKCS#12 PFX in ASN.1 notation.
 * @param strict true to use strict DER decoding, false not to (default: true).
 * @param {String} password Password to decrypt with (optional).
 *
 * @return PKCS#12 PFX object.
 */
p12.pkcs12FromAsn1 = function(obj, strict, password) {
  // handle args
  if(typeof strict === 'string') {
    password = strict;
    strict = true;
  } else if(strict === undefined) {
    strict = true;
  }

  // validate PFX and capture data
  var capture = {};
  var errors = [];
  if(!asn1$1.validate(obj, pfxValidator, capture, errors)) {
    var error = new Error('Cannot read PKCS#12 PFX. ' +
      'ASN.1 object is not an PKCS#12 PFX.');
    error.errors = error;
    throw error;
  }

  var pfx = {
    version: capture.version.charCodeAt(0),
    safeContents: [],

    /**
     * Gets bags with matching attributes.
     *
     * @param filter the attributes to filter by:
     *          [localKeyId] the localKeyId to search for.
     *          [localKeyIdHex] the localKeyId in hex to search for.
     *          [friendlyName] the friendly name to search for.
     *          [bagType] bag type to narrow each attribute search by.
     *
     * @return a map of attribute type to an array of matching bags or, if no
     *           attribute was given but a bag type, the map key will be the
     *           bag type.
     */
    getBags: function(filter) {
      var rval = {};

      var localKeyId;
      if('localKeyId' in filter) {
        localKeyId = filter.localKeyId;
      } else if('localKeyIdHex' in filter) {
        localKeyId = forge$1.util.hexToBytes(filter.localKeyIdHex);
      }

      // filter on bagType only
      if(localKeyId === undefined && !('friendlyName' in filter) &&
        'bagType' in filter) {
        rval[filter.bagType] = _getBagsByAttribute(
          pfx.safeContents, null, null, filter.bagType);
      }

      if(localKeyId !== undefined) {
        rval.localKeyId = _getBagsByAttribute(
          pfx.safeContents, 'localKeyId',
          localKeyId, filter.bagType);
      }
      if('friendlyName' in filter) {
        rval.friendlyName = _getBagsByAttribute(
          pfx.safeContents, 'friendlyName',
          filter.friendlyName, filter.bagType);
      }

      return rval;
    },

    /**
     * DEPRECATED: use getBags() instead.
     *
     * Get bags with matching friendlyName attribute.
     *
     * @param friendlyName the friendly name to search for.
     * @param [bagType] bag type to narrow search by.
     *
     * @return an array of bags with matching friendlyName attribute.
     */
    getBagsByFriendlyName: function(friendlyName, bagType) {
      return _getBagsByAttribute(
        pfx.safeContents, 'friendlyName', friendlyName, bagType);
    },

    /**
     * DEPRECATED: use getBags() instead.
     *
     * Get bags with matching localKeyId attribute.
     *
     * @param localKeyId the localKeyId to search for.
     * @param [bagType] bag type to narrow search by.
     *
     * @return an array of bags with matching localKeyId attribute.
     */
    getBagsByLocalKeyId: function(localKeyId, bagType) {
      return _getBagsByAttribute(
        pfx.safeContents, 'localKeyId', localKeyId, bagType);
    }
  };

  if(capture.version.charCodeAt(0) !== 3) {
    var error = new Error('PKCS#12 PFX of version other than 3 not supported.');
    error.version = capture.version.charCodeAt(0);
    throw error;
  }

  if(asn1$1.derToOid(capture.contentType) !== pki$1.oids.data) {
    var error = new Error('Only PKCS#12 PFX in password integrity mode supported.');
    error.oid = asn1$1.derToOid(capture.contentType);
    throw error;
  }

  var data = capture.content.value[0];
  if(data.tagClass !== asn1$1.Class.UNIVERSAL ||
     data.type !== asn1$1.Type.OCTETSTRING) {
    throw new Error('PKCS#12 authSafe content data is not an OCTET STRING.');
  }
  data = _decodePkcs7Data(data);

  // check for MAC
  if(capture.mac) {
    var md = null;
    var macKeyBytes = 0;
    var macAlgorithm = asn1$1.derToOid(capture.macAlgorithm);
    switch(macAlgorithm) {
    case pki$1.oids.sha1:
      md = forge$1.md.sha1.create();
      macKeyBytes = 20;
      break;
    case pki$1.oids.sha256:
      md = forge$1.md.sha256.create();
      macKeyBytes = 32;
      break;
    case pki$1.oids.sha384:
      md = forge$1.md.sha384.create();
      macKeyBytes = 48;
      break;
    case pki$1.oids.sha512:
      md = forge$1.md.sha512.create();
      macKeyBytes = 64;
      break;
    case pki$1.oids.md5:
      md = forge$1.md.md5.create();
      macKeyBytes = 16;
      break;
    }
    if(md === null) {
      throw new Error('PKCS#12 uses unsupported MAC algorithm: ' + macAlgorithm);
    }

    // verify MAC (iterations default to 1)
    var macSalt = new forge$1.util.ByteBuffer(capture.macSalt);
    var macIterations = (('macIterations' in capture) ?
      parseInt(forge$1.util.bytesToHex(capture.macIterations), 16) : 1);
    var macKey = p12.generateKey(
      password, macSalt, 3, macIterations, macKeyBytes, md);
    var mac = forge$1.hmac.create();
    mac.start(md, macKey);
    mac.update(data.value);
    var macValue = mac.getMac();
    if(macValue.getBytes() !== capture.macDigest) {
      throw new Error('PKCS#12 MAC could not be verified. Invalid password?');
    }
  }

  _decodeAuthenticatedSafe(pfx, data.value, strict, password);
  return pfx;
};

/**
 * Decodes PKCS#7 Data. PKCS#7 (RFC 2315) defines "Data" as an OCTET STRING,
 * but it is sometimes an OCTET STRING that is composed/constructed of chunks,
 * each its own OCTET STRING. This is BER-encoding vs. DER-encoding. This
 * function transforms this corner-case into the usual simple,
 * non-composed/constructed OCTET STRING.
 *
 * This function may be moved to ASN.1 at some point to better deal with
 * more BER-encoding issues, should they arise.
 *
 * @param data the ASN.1 Data object to transform.
 */
function _decodePkcs7Data(data) {
  // handle special case of "chunked" data content: an octet string composed
  // of other octet strings
  if(data.composed || data.constructed) {
    var value = forge$1.util.createBuffer();
    for(var i = 0; i < data.value.length; ++i) {
      value.putBytes(data.value[i].value);
    }
    data.composed = data.constructed = false;
    data.value = value.getBytes();
  }
  return data;
}

/**
 * Decode PKCS#12 AuthenticatedSafe (BER encoded) into PFX object.
 *
 * The AuthenticatedSafe is a BER-encoded SEQUENCE OF ContentInfo.
 *
 * @param pfx The PKCS#12 PFX object to fill.
 * @param {String} authSafe BER-encoded AuthenticatedSafe.
 * @param strict true to use strict DER decoding, false not to.
 * @param {String} password Password to decrypt with (optional).
 */
function _decodeAuthenticatedSafe(pfx, authSafe, strict, password) {
  authSafe = asn1$1.fromDer(authSafe, strict);  /* actually it's BER encoded */

  if(authSafe.tagClass !== asn1$1.Class.UNIVERSAL ||
     authSafe.type !== asn1$1.Type.SEQUENCE ||
     authSafe.constructed !== true) {
    throw new Error('PKCS#12 AuthenticatedSafe expected to be a ' +
      'SEQUENCE OF ContentInfo');
  }

  for(var i = 0; i < authSafe.value.length; i++) {
    var contentInfo = authSafe.value[i];

    // validate contentInfo and capture data
    var capture = {};
    var errors = [];
    if(!asn1$1.validate(contentInfo, contentInfoValidator, capture, errors)) {
      var error = new Error('Cannot read ContentInfo.');
      error.errors = errors;
      throw error;
    }

    var obj = {
      encrypted: false
    };
    var safeContents = null;
    var data = capture.content.value[0];
    switch(asn1$1.derToOid(capture.contentType)) {
    case pki$1.oids.data:
      if(data.tagClass !== asn1$1.Class.UNIVERSAL ||
         data.type !== asn1$1.Type.OCTETSTRING) {
        throw new Error('PKCS#12 SafeContents Data is not an OCTET STRING.');
      }
      safeContents = _decodePkcs7Data(data).value;
      break;
    case pki$1.oids.encryptedData:
      safeContents = _decryptSafeContents(data, password);
      obj.encrypted = true;
      break;
    default:
      var error = new Error('Unsupported PKCS#12 contentType.');
      error.contentType = asn1$1.derToOid(capture.contentType);
      throw error;
    }

    obj.safeBags = _decodeSafeContents(safeContents, strict, password);
    pfx.safeContents.push(obj);
  }
}

/**
 * Decrypt PKCS#7 EncryptedData structure.
 *
 * @param data ASN.1 encoded EncryptedContentInfo object.
 * @param password The user-provided password.
 *
 * @return The decrypted SafeContents (ASN.1 object).
 */
function _decryptSafeContents(data, password) {
  var capture = {};
  var errors = [];
  if(!asn1$1.validate(
    data, forge$1.pkcs7.asn1.encryptedDataValidator, capture, errors)) {
    var error = new Error('Cannot read EncryptedContentInfo.');
    error.errors = errors;
    throw error;
  }

  var oid = asn1$1.derToOid(capture.contentType);
  if(oid !== pki$1.oids.data) {
    var error = new Error(
      'PKCS#12 EncryptedContentInfo ContentType is not Data.');
    error.oid = oid;
    throw error;
  }

  // get cipher
  oid = asn1$1.derToOid(capture.encAlgorithm);
  var cipher = pki$1.pbe.getCipher(oid, capture.encParameter, password);

  // get encrypted data
  var encryptedContentAsn1 = _decodePkcs7Data(capture.encryptedContentAsn1);
  var encrypted = forge$1.util.createBuffer(encryptedContentAsn1.value);

  cipher.update(encrypted);
  if(!cipher.finish()) {
    throw new Error('Failed to decrypt PKCS#12 SafeContents.');
  }

  return cipher.output.getBytes();
}

/**
 * Decode PKCS#12 SafeContents (BER-encoded) into array of Bag objects.
 *
 * The safeContents is a BER-encoded SEQUENCE OF SafeBag.
 *
 * @param {String} safeContents BER-encoded safeContents.
 * @param strict true to use strict DER decoding, false not to.
 * @param {String} password Password to decrypt with (optional).
 *
 * @return {Array} Array of Bag objects.
 */
function _decodeSafeContents(safeContents, strict, password) {
  // if strict and no safe contents, return empty safes
  if(!strict && safeContents.length === 0) {
    return [];
  }

  // actually it's BER-encoded
  safeContents = asn1$1.fromDer(safeContents, strict);

  if(safeContents.tagClass !== asn1$1.Class.UNIVERSAL ||
    safeContents.type !== asn1$1.Type.SEQUENCE ||
    safeContents.constructed !== true) {
    throw new Error(
      'PKCS#12 SafeContents expected to be a SEQUENCE OF SafeBag.');
  }

  var res = [];
  for(var i = 0; i < safeContents.value.length; i++) {
    var safeBag = safeContents.value[i];

    // validate SafeBag and capture data
    var capture = {};
    var errors = [];
    if(!asn1$1.validate(safeBag, safeBagValidator, capture, errors)) {
      var error = new Error('Cannot read SafeBag.');
      error.errors = errors;
      throw error;
    }

    /* Create bag object and push to result array. */
    var bag = {
      type: asn1$1.derToOid(capture.bagId),
      attributes: _decodeBagAttributes(capture.bagAttributes)
    };
    res.push(bag);

    var validator, decoder;
    var bagAsn1 = capture.bagValue.value[0];
    switch(bag.type) {
      case pki$1.oids.pkcs8ShroudedKeyBag:
        /* bagAsn1 has a EncryptedPrivateKeyInfo, which we need to decrypt.
           Afterwards we can handle it like a keyBag,
           which is a PrivateKeyInfo. */
        bagAsn1 = pki$1.decryptPrivateKeyInfo(bagAsn1, password);
        if(bagAsn1 === null) {
          throw new Error(
            'Unable to decrypt PKCS#8 ShroudedKeyBag, wrong password?');
        }

        /* fall through */
      case pki$1.oids.keyBag:
        /* A PKCS#12 keyBag is a simple PrivateKeyInfo as understood by our
           PKI module, hence we don't have to do validation/capturing here,
           just pass what we already got. */
        try {
          bag.key = pki$1.privateKeyFromAsn1(bagAsn1);
        } catch(e) {
          // ignore unknown key type, pass asn1 value
          bag.key = null;
          bag.asn1 = bagAsn1;
        }
        continue;  /* Nothing more to do. */

      case pki$1.oids.certBag:
        /* A PKCS#12 certBag can wrap both X.509 and sdsi certificates.
           Therefore put the SafeBag content through another validator to
           capture the fields.  Afterwards check & store the results. */
        validator = certBagValidator;
        decoder = function() {
          if(asn1$1.derToOid(capture.certId) !== pki$1.oids.x509Certificate) {
            var error = new Error(
              'Unsupported certificate type, only X.509 supported.');
            error.oid = asn1$1.derToOid(capture.certId);
            throw error;
          }

          // true=produce cert hash
          var certAsn1 = asn1$1.fromDer(capture.cert, strict);
          try {
            bag.cert = pki$1.certificateFromAsn1(certAsn1, true);
          } catch(e) {
            // ignore unknown cert type, pass asn1 value
            bag.cert = null;
            bag.asn1 = certAsn1;
          }
        };
        break;

      default:
        var error = new Error('Unsupported PKCS#12 SafeBag type.');
        error.oid = bag.type;
        throw error;
    }

    /* Validate SafeBag value (i.e. CertBag, etc.) and capture data if needed. */
    if(validator !== undefined &&
       !asn1$1.validate(bagAsn1, validator, capture, errors)) {
      var error = new Error('Cannot read PKCS#12 ' + validator.name);
      error.errors = errors;
      throw error;
    }

    /* Call decoder function from above to store the results. */
    decoder();
  }

  return res;
}

/**
 * Decode PKCS#12 SET OF PKCS12Attribute into JavaScript object.
 *
 * @param attributes SET OF PKCS12Attribute (ASN.1 object).
 *
 * @return the decoded attributes.
 */
function _decodeBagAttributes(attributes) {
  var decodedAttrs = {};

  if(attributes !== undefined) {
    for(var i = 0; i < attributes.length; ++i) {
      var capture = {};
      var errors = [];
      if(!asn1$1.validate(attributes[i], attributeValidator, capture, errors)) {
        var error = new Error('Cannot read PKCS#12 BagAttribute.');
        error.errors = errors;
        throw error;
      }

      var oid = asn1$1.derToOid(capture.oid);
      if(pki$1.oids[oid] === undefined) {
        // unsupported attribute type, ignore.
        continue;
      }

      decodedAttrs[pki$1.oids[oid]] = [];
      for(var j = 0; j < capture.values.length; ++j) {
        decodedAttrs[pki$1.oids[oid]].push(capture.values[j].value);
      }
    }
  }

  return decodedAttrs;
}

/**
 * Wraps a private key and certificate in a PKCS#12 PFX wrapper. If a
 * password is provided then the private key will be encrypted.
 *
 * An entire certificate chain may also be included. To do this, pass
 * an array for the "cert" parameter where the first certificate is
 * the one that is paired with the private key and each subsequent one
 * verifies the previous one. The certificates may be in PEM format or
 * have been already parsed by Forge.
 *
 * @todo implement password-based-encryption for the whole package
 *
 * @param key the private key.
 * @param cert the certificate (may be an array of certificates in order
 *          to specify a certificate chain).
 * @param password the password to use, null for none.
 * @param options:
 *          algorithm the encryption algorithm to use
 *            ('aes128', 'aes192', 'aes256', '3des'), defaults to 'aes128'.
 *          count the iteration count to use.
 *          saltSize the salt size to use.
 *          useMac true to include a MAC, false not to, defaults to true.
 *          localKeyId the local key ID to use, in hex.
 *          friendlyName the friendly name to use.
 *          generateLocalKeyId true to generate a random local key ID,
 *            false not to, defaults to true.
 *
 * @return the PKCS#12 PFX ASN.1 object.
 */
p12.toPkcs12Asn1 = function(key, cert, password, options) {
  // set default options
  options = options || {};
  options.saltSize = options.saltSize || 8;
  options.count = options.count || 2048;
  options.algorithm = options.algorithm || options.encAlgorithm || 'aes128';
  if(!('useMac' in options)) {
    options.useMac = true;
  }
  if(!('localKeyId' in options)) {
    options.localKeyId = null;
  }
  if(!('generateLocalKeyId' in options)) {
    options.generateLocalKeyId = true;
  }

  var localKeyId = options.localKeyId;
  var bagAttrs;
  if(localKeyId !== null) {
    localKeyId = forge$1.util.hexToBytes(localKeyId);
  } else if(options.generateLocalKeyId) {
    // use SHA-1 of paired cert, if available
    if(cert) {
      var pairedCert = forge$1.util.isArray(cert) ? cert[0] : cert;
      if(typeof pairedCert === 'string') {
        pairedCert = pki$1.certificateFromPem(pairedCert);
      }
      var sha1 = forge$1.md.sha1.create();
      sha1.update(asn1$1.toDer(pki$1.certificateToAsn1(pairedCert)).getBytes());
      localKeyId = sha1.digest().getBytes();
    } else {
      // FIXME: consider using SHA-1 of public key (which can be generated
      // from private key components), see: cert.generateSubjectKeyIdentifier
      // generate random bytes
      localKeyId = forge$1.random.getBytes(20);
    }
  }

  var attrs = [];
  if(localKeyId !== null) {
    attrs.push(
      // localKeyID
      asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.SEQUENCE, true, [
        // attrId
        asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.OID, false,
          asn1$1.oidToDer(pki$1.oids.localKeyId).getBytes()),
        // attrValues
        asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.SET, true, [
          asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.OCTETSTRING, false,
            localKeyId)
        ])
      ]));
  }
  if('friendlyName' in options) {
    attrs.push(
      // friendlyName
      asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.SEQUENCE, true, [
        // attrId
        asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.OID, false,
          asn1$1.oidToDer(pki$1.oids.friendlyName).getBytes()),
        // attrValues
        asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.SET, true, [
          asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.BMPSTRING, false,
            options.friendlyName)
        ])
      ]));
  }

  if(attrs.length > 0) {
    bagAttrs = asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.SET, true, attrs);
  }

  // collect contents for AuthenticatedSafe
  var contents = [];

  // create safe bag(s) for certificate chain
  var chain = [];
  if(cert !== null) {
    if(forge$1.util.isArray(cert)) {
      chain = cert;
    } else {
      chain = [cert];
    }
  }

  var certSafeBags = [];
  for(var i = 0; i < chain.length; ++i) {
    // convert cert from PEM as necessary
    cert = chain[i];
    if(typeof cert === 'string') {
      cert = pki$1.certificateFromPem(cert);
    }

    // SafeBag
    var certBagAttrs = (i === 0) ? bagAttrs : undefined;
    var certAsn1 = pki$1.certificateToAsn1(cert);
    var certSafeBag =
      asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.SEQUENCE, true, [
        // bagId
        asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.OID, false,
          asn1$1.oidToDer(pki$1.oids.certBag).getBytes()),
        // bagValue
        asn1$1.create(asn1$1.Class.CONTEXT_SPECIFIC, 0, true, [
          // CertBag
          asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.SEQUENCE, true, [
            // certId
            asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.OID, false,
              asn1$1.oidToDer(pki$1.oids.x509Certificate).getBytes()),
            // certValue (x509Certificate)
            asn1$1.create(asn1$1.Class.CONTEXT_SPECIFIC, 0, true, [
              asn1$1.create(
                asn1$1.Class.UNIVERSAL, asn1$1.Type.OCTETSTRING, false,
                asn1$1.toDer(certAsn1).getBytes())
            ])])]),
        // bagAttributes (OPTIONAL)
        certBagAttrs
      ]);
    certSafeBags.push(certSafeBag);
  }

  if(certSafeBags.length > 0) {
    // SafeContents
    var certSafeContents = asn1$1.create(
      asn1$1.Class.UNIVERSAL, asn1$1.Type.SEQUENCE, true, certSafeBags);

    // ContentInfo
    var certCI =
      // PKCS#7 ContentInfo
      asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.SEQUENCE, true, [
        // contentType
        asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.OID, false,
          // OID for the content type is 'data'
          asn1$1.oidToDer(pki$1.oids.data).getBytes()),
        // content
        asn1$1.create(asn1$1.Class.CONTEXT_SPECIFIC, 0, true, [
          asn1$1.create(
            asn1$1.Class.UNIVERSAL, asn1$1.Type.OCTETSTRING, false,
            asn1$1.toDer(certSafeContents).getBytes())
        ])
      ]);
    contents.push(certCI);
  }

  // create safe contents for private key
  var keyBag = null;
  if(key !== null) {
    // SafeBag
    var pkAsn1 = pki$1.wrapRsaPrivateKey(pki$1.privateKeyToAsn1(key));
    if(password === null) {
      // no encryption
      keyBag = asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.SEQUENCE, true, [
        // bagId
        asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.OID, false,
          asn1$1.oidToDer(pki$1.oids.keyBag).getBytes()),
        // bagValue
        asn1$1.create(asn1$1.Class.CONTEXT_SPECIFIC, 0, true, [
          // PrivateKeyInfo
          pkAsn1
        ]),
        // bagAttributes (OPTIONAL)
        bagAttrs
      ]);
    } else {
      // encrypted PrivateKeyInfo
      keyBag = asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.SEQUENCE, true, [
        // bagId
        asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.OID, false,
          asn1$1.oidToDer(pki$1.oids.pkcs8ShroudedKeyBag).getBytes()),
        // bagValue
        asn1$1.create(asn1$1.Class.CONTEXT_SPECIFIC, 0, true, [
          // EncryptedPrivateKeyInfo
          pki$1.encryptPrivateKeyInfo(pkAsn1, password, options)
        ]),
        // bagAttributes (OPTIONAL)
        bagAttrs
      ]);
    }

    // SafeContents
    var keySafeContents =
      asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.SEQUENCE, true, [keyBag]);

    // ContentInfo
    var keyCI =
      // PKCS#7 ContentInfo
      asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.SEQUENCE, true, [
        // contentType
        asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.OID, false,
          // OID for the content type is 'data'
          asn1$1.oidToDer(pki$1.oids.data).getBytes()),
        // content
        asn1$1.create(asn1$1.Class.CONTEXT_SPECIFIC, 0, true, [
          asn1$1.create(
            asn1$1.Class.UNIVERSAL, asn1$1.Type.OCTETSTRING, false,
            asn1$1.toDer(keySafeContents).getBytes())
        ])
      ]);
    contents.push(keyCI);
  }

  // create AuthenticatedSafe by stringing together the contents
  var safe = asn1$1.create(
    asn1$1.Class.UNIVERSAL, asn1$1.Type.SEQUENCE, true, contents);

  var macData;
  if(options.useMac) {
    // MacData
    var sha1 = forge$1.md.sha1.create();
    var macSalt = new forge$1.util.ByteBuffer(
      forge$1.random.getBytes(options.saltSize));
    var count = options.count;
    // 160-bit key
    var key = p12.generateKey(password, macSalt, 3, count, 20);
    var mac = forge$1.hmac.create();
    mac.start(sha1, key);
    mac.update(asn1$1.toDer(safe).getBytes());
    var macValue = mac.getMac();
    macData = asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.SEQUENCE, true, [
      // mac DigestInfo
      asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.SEQUENCE, true, [
        // digestAlgorithm
        asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.SEQUENCE, true, [
          // algorithm = SHA-1
          asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.OID, false,
            asn1$1.oidToDer(pki$1.oids.sha1).getBytes()),
          // parameters = Null
          asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.NULL, false, '')
        ]),
        // digest
        asn1$1.create(
          asn1$1.Class.UNIVERSAL, asn1$1.Type.OCTETSTRING,
          false, macValue.getBytes())
      ]),
      // macSalt OCTET STRING
      asn1$1.create(
        asn1$1.Class.UNIVERSAL, asn1$1.Type.OCTETSTRING, false, macSalt.getBytes()),
      // iterations INTEGER (XXX: Only support count < 65536)
      asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.INTEGER, false,
        asn1$1.integerToDer(count).getBytes()
      )
    ]);
  }

  // PFX
  return asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.SEQUENCE, true, [
    // version (3)
    asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.INTEGER, false,
      asn1$1.integerToDer(3).getBytes()),
    // PKCS#7 ContentInfo
    asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.SEQUENCE, true, [
      // contentType
      asn1$1.create(asn1$1.Class.UNIVERSAL, asn1$1.Type.OID, false,
        // OID for the content type is 'data'
        asn1$1.oidToDer(pki$1.oids.data).getBytes()),
      // content
      asn1$1.create(asn1$1.Class.CONTEXT_SPECIFIC, 0, true, [
        asn1$1.create(
          asn1$1.Class.UNIVERSAL, asn1$1.Type.OCTETSTRING, false,
          asn1$1.toDer(safe).getBytes())
      ])
    ]),
    macData
  ]);
};

/**
 * Derives a PKCS#12 key.
 *
 * @param password the password to derive the key material from, null or
 *          undefined for none.
 * @param salt the salt, as a ByteBuffer, to use.
 * @param id the PKCS#12 ID byte (1 = key material, 2 = IV, 3 = MAC).
 * @param iter the iteration count.
 * @param n the number of bytes to derive from the password.
 * @param md the message digest to use, defaults to SHA-1.
 *
 * @return a ByteBuffer with the bytes derived from the password.
 */
p12.generateKey = forge$1.pbe.generatePkcs12Key;

/**
 * Javascript implementation of a basic Public Key Infrastructure, including
 * support for RSA public and private keys.
 *
 * @author Dave Longley
 *
 * Copyright (c) 2010-2013 Digital Bazaar, Inc.
 */

var forge = forge$s;











// shortcut for asn.1 API
var asn1 = forge.asn1;

/* Public Key Infrastructure (PKI) implementation. */
var pki = forge.pki = forge.pki || {};

/**
 * NOTE: THIS METHOD IS DEPRECATED. Use pem.decode() instead.
 *
 * Converts PEM-formatted data to DER.
 *
 * @param pem the PEM-formatted data.
 *
 * @return the DER-formatted data.
 */
pki.pemToDer = function(pem) {
  var msg = forge.pem.decode(pem)[0];
  if(msg.procType && msg.procType.type === 'ENCRYPTED') {
    throw new Error('Could not convert PEM to DER; PEM is encrypted.');
  }
  return forge.util.createBuffer(msg.body);
};

/**
 * Converts an RSA private key from PEM format.
 *
 * @param pem the PEM-formatted private key.
 *
 * @return the private key.
 */
pki.privateKeyFromPem = function(pem) {
  var msg = forge.pem.decode(pem)[0];

  if(msg.type !== 'PRIVATE KEY' && msg.type !== 'RSA PRIVATE KEY') {
    var error = new Error('Could not convert private key from PEM; PEM ' +
      'header type is not "PRIVATE KEY" or "RSA PRIVATE KEY".');
    error.headerType = msg.type;
    throw error;
  }
  if(msg.procType && msg.procType.type === 'ENCRYPTED') {
    throw new Error('Could not convert private key from PEM; PEM is encrypted.');
  }

  // convert DER to ASN.1 object
  var obj = asn1.fromDer(msg.body);

  return pki.privateKeyFromAsn1(obj);
};

/**
 * Converts an RSA private key to PEM format.
 *
 * @param key the private key.
 * @param maxline the maximum characters per line, defaults to 64.
 *
 * @return the PEM-formatted private key.
 */
pki.privateKeyToPem = function(key, maxline) {
  // convert to ASN.1, then DER, then PEM-encode
  var msg = {
    type: 'RSA PRIVATE KEY',
    body: asn1.toDer(pki.privateKeyToAsn1(key)).getBytes()
  };
  return forge.pem.encode(msg, {maxline: maxline});
};

/**
 * Converts a PrivateKeyInfo to PEM format.
 *
 * @param pki the PrivateKeyInfo.
 * @param maxline the maximum characters per line, defaults to 64.
 *
 * @return the PEM-formatted private key.
 */
pki.privateKeyInfoToPem = function(pki, maxline) {
  // convert to DER, then PEM-encode
  var msg = {
    type: 'PRIVATE KEY',
    body: asn1.toDer(pki).getBytes()
  };
  return forge.pem.encode(msg, {maxline: maxline});
};

// Simplified fork of selfsigned with inlined options and partial
// a hexString is considered negative if it's most significant bit is 1
// because serial numbers use ones' complement notation
// this RFC in section 4.1.2.2 requires serial numbers to be positive
// http://www.ietf.org/rfc/rfc5280.txt
function toPositiveHex(hexString) {
    let mostSignificativeHexAsInt = parseInt(hexString[0], 16);
    if (mostSignificativeHexAsInt < 8) {
        return hexString;
    }
    mostSignificativeHexAsInt -= 8;
    return mostSignificativeHexAsInt.toString() + hexString.substring(1);
}
function createCertificate() {
    const days = 30;
    const keySize = 2048;
    const extensions = [
        // {
        //   name: 'basicConstraints',
        //   cA: true,
        // },
        {
            name: 'keyUsage',
            keyCertSign: true,
            digitalSignature: true,
            nonRepudiation: true,
            keyEncipherment: true,
            dataEncipherment: true
        },
        {
            name: 'extKeyUsage',
            serverAuth: true,
            clientAuth: true,
            codeSigning: true,
            timeStamping: true
        },
        {
            name: 'subjectAltName',
            altNames: [
                {
                    // type 2 is DNS
                    type: 2,
                    value: 'localhost'
                },
                {
                    type: 2,
                    value: 'localhost.localdomain'
                },
                {
                    type: 2,
                    value: 'lvh.me'
                },
                {
                    type: 2,
                    value: '*.lvh.me'
                },
                {
                    type: 2,
                    value: '[::1]'
                },
                {
                    // type 7 is IP
                    type: 7,
                    ip: '127.0.0.1'
                },
                {
                    type: 7,
                    ip: 'fe80::1'
                }
            ]
        }
    ];
    const attrs = [
        {
            name: 'commonName',
            value: 'example.org'
        },
        {
            name: 'countryName',
            value: 'US'
        },
        {
            shortName: 'ST',
            value: 'Virginia'
        },
        {
            name: 'localityName',
            value: 'Blacksburg'
        },
        {
            name: 'organizationName',
            value: 'Test'
        },
        {
            shortName: 'OU',
            value: 'Test'
        }
    ];
    const keyPair = forge$s.pki.rsa.generateKeyPair(keySize);
    const cert = forge$s.pki.createCertificate();
    cert.serialNumber = toPositiveHex(forge$s.util.bytesToHex(forge$s.random.getBytesSync(9))); // the serial number can be decimal or hex (if preceded by 0x)
    cert.validity.notBefore = new Date();
    cert.validity.notAfter = new Date();
    cert.validity.notAfter.setDate(cert.validity.notBefore.getDate() + days);
    cert.setSubject(attrs);
    cert.setIssuer(attrs);
    cert.publicKey = keyPair.publicKey;
    cert.setExtensions(extensions);
    const algorithm = forge$s.md.sha256.create();
    cert.sign(keyPair.privateKey, algorithm);
    const privateKeyPem = forge$s.pki.privateKeyToPem(keyPair.privateKey);
    const certPem = forge$s.pki.certificateToPem(cert);
    return privateKeyPem + certPem;
}

exports.createCertificate = createCertificate;

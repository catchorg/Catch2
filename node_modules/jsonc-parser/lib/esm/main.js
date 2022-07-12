/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
'use strict';
import * as formatter from './impl/format';
import * as edit from './impl/edit';
import * as scanner from './impl/scanner';
import * as parser from './impl/parser';
/**
 * Creates a JSON scanner on the given text.
 * If ignoreTrivia is set, whitespaces or comments are ignored.
 */
export var createScanner = scanner.createScanner;
/**
 * For a given offset, evaluate the location in the JSON document. Each segment in the location path is either a property name or an array index.
 */
export var getLocation = parser.getLocation;
/**
 * Parses the given text and returns the object the JSON content represents. On invalid input, the parser tries to be as fault tolerant as possible, but still return a result.
 * Therefore, always check the errors list to find out if the input was valid.
 */
export var parse = parser.parse;
/**
 * Parses the given text and returns a tree representation the JSON content. On invalid input, the parser tries to be as fault tolerant as possible, but still return a result.
 */
export var parseTree = parser.parseTree;
/**
 * Finds the node at the given path in a JSON DOM.
 */
export var findNodeAtLocation = parser.findNodeAtLocation;
/**
 * Finds the innermost node at the given offset. If includeRightBound is set, also finds nodes that end at the given offset.
 */
export var findNodeAtOffset = parser.findNodeAtOffset;
/**
 * Gets the JSON path of the given JSON DOM node
 */
export var getNodePath = parser.getNodePath;
/**
 * Evaluates the JavaScript object of the given JSON DOM node
 */
export var getNodeValue = parser.getNodeValue;
/**
 * Parses the given text and invokes the visitor functions for each object, array and literal reached.
 */
export var visit = parser.visit;
/**
 * Takes JSON with JavaScript-style comments and remove
 * them. Optionally replaces every none-newline character
 * of comments with a replaceCharacter
 */
export var stripComments = parser.stripComments;
export function printParseErrorCode(code) {
    switch (code) {
        case 1 /* ParseErrorCode.InvalidSymbol */: return 'InvalidSymbol';
        case 2 /* ParseErrorCode.InvalidNumberFormat */: return 'InvalidNumberFormat';
        case 3 /* ParseErrorCode.PropertyNameExpected */: return 'PropertyNameExpected';
        case 4 /* ParseErrorCode.ValueExpected */: return 'ValueExpected';
        case 5 /* ParseErrorCode.ColonExpected */: return 'ColonExpected';
        case 6 /* ParseErrorCode.CommaExpected */: return 'CommaExpected';
        case 7 /* ParseErrorCode.CloseBraceExpected */: return 'CloseBraceExpected';
        case 8 /* ParseErrorCode.CloseBracketExpected */: return 'CloseBracketExpected';
        case 9 /* ParseErrorCode.EndOfFileExpected */: return 'EndOfFileExpected';
        case 10 /* ParseErrorCode.InvalidCommentToken */: return 'InvalidCommentToken';
        case 11 /* ParseErrorCode.UnexpectedEndOfComment */: return 'UnexpectedEndOfComment';
        case 12 /* ParseErrorCode.UnexpectedEndOfString */: return 'UnexpectedEndOfString';
        case 13 /* ParseErrorCode.UnexpectedEndOfNumber */: return 'UnexpectedEndOfNumber';
        case 14 /* ParseErrorCode.InvalidUnicode */: return 'InvalidUnicode';
        case 15 /* ParseErrorCode.InvalidEscapeCharacter */: return 'InvalidEscapeCharacter';
        case 16 /* ParseErrorCode.InvalidCharacter */: return 'InvalidCharacter';
    }
    return '<unknown ParseErrorCode>';
}
/**
 * Computes the edit operations needed to format a JSON document.
 *
 * @param documentText The input text
 * @param range The range to format or `undefined` to format the full content
 * @param options The formatting options
 * @returns The edit operations describing the formatting changes to the original document following the format described in {@linkcode EditResult}.
 * To apply the edit operations to the input, use {@linkcode applyEdits}.
 */
export function format(documentText, range, options) {
    return formatter.format(documentText, range, options);
}
/**
 * Computes the edit operations needed to modify a value in the JSON document.
 *
 * @param documentText The input text
 * @param path The path of the value to change. The path represents either to the document root, a property or an array item.
 * If the path points to an non-existing property or item, it will be created.
 * @param value The new value for the specified property or item. If the value is undefined,
 * the property or item will be removed.
 * @param options Options
 * @returns The edit operations describing the changes to the original document, following the format described in {@linkcode EditResult}.
 * To apply the edit operations to the input, use {@linkcode applyEdits}.
 */
export function modify(text, path, value, options) {
    return edit.setProperty(text, path, value, options);
}
/**
 * Applies edits to an input string.
 * @param text The input text
 * @param edits Edit operations following the format described in {@linkcode EditResult}.
 * @returns The text with the applied edits.
 * @throws An error if the edit operations are not well-formed as described in {@linkcode EditResult}.
 */
export function applyEdits(text, edits) {
    for (var i = edits.length - 1; i >= 0; i--) {
        text = edit.applyEdit(text, edits[i]);
    }
    return text;
}

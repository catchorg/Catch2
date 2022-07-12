(function (factory) {
    if (typeof module === "object" && typeof module.exports === "object") {
        var v = factory(require, exports);
        if (v !== undefined) module.exports = v;
    }
    else if (typeof define === "function" && define.amd) {
        define(["require", "exports", "./impl/format", "./impl/edit", "./impl/scanner", "./impl/parser"], factory);
    }
})(function (require, exports) {
    /*---------------------------------------------------------------------------------------------
     *  Copyright (c) Microsoft Corporation. All rights reserved.
     *  Licensed under the MIT License. See License.txt in the project root for license information.
     *--------------------------------------------------------------------------------------------*/
    'use strict';
    Object.defineProperty(exports, "__esModule", { value: true });
    exports.applyEdits = exports.modify = exports.format = exports.printParseErrorCode = exports.stripComments = exports.visit = exports.getNodeValue = exports.getNodePath = exports.findNodeAtOffset = exports.findNodeAtLocation = exports.parseTree = exports.parse = exports.getLocation = exports.createScanner = void 0;
    var formatter = require("./impl/format");
    var edit = require("./impl/edit");
    var scanner = require("./impl/scanner");
    var parser = require("./impl/parser");
    /**
     * Creates a JSON scanner on the given text.
     * If ignoreTrivia is set, whitespaces or comments are ignored.
     */
    exports.createScanner = scanner.createScanner;
    /**
     * For a given offset, evaluate the location in the JSON document. Each segment in the location path is either a property name or an array index.
     */
    exports.getLocation = parser.getLocation;
    /**
     * Parses the given text and returns the object the JSON content represents. On invalid input, the parser tries to be as fault tolerant as possible, but still return a result.
     * Therefore, always check the errors list to find out if the input was valid.
     */
    exports.parse = parser.parse;
    /**
     * Parses the given text and returns a tree representation the JSON content. On invalid input, the parser tries to be as fault tolerant as possible, but still return a result.
     */
    exports.parseTree = parser.parseTree;
    /**
     * Finds the node at the given path in a JSON DOM.
     */
    exports.findNodeAtLocation = parser.findNodeAtLocation;
    /**
     * Finds the innermost node at the given offset. If includeRightBound is set, also finds nodes that end at the given offset.
     */
    exports.findNodeAtOffset = parser.findNodeAtOffset;
    /**
     * Gets the JSON path of the given JSON DOM node
     */
    exports.getNodePath = parser.getNodePath;
    /**
     * Evaluates the JavaScript object of the given JSON DOM node
     */
    exports.getNodeValue = parser.getNodeValue;
    /**
     * Parses the given text and invokes the visitor functions for each object, array and literal reached.
     */
    exports.visit = parser.visit;
    /**
     * Takes JSON with JavaScript-style comments and remove
     * them. Optionally replaces every none-newline character
     * of comments with a replaceCharacter
     */
    exports.stripComments = parser.stripComments;
    function printParseErrorCode(code) {
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
    exports.printParseErrorCode = printParseErrorCode;
    /**
     * Computes the edit operations needed to format a JSON document.
     *
     * @param documentText The input text
     * @param range The range to format or `undefined` to format the full content
     * @param options The formatting options
     * @returns The edit operations describing the formatting changes to the original document following the format described in {@linkcode EditResult}.
     * To apply the edit operations to the input, use {@linkcode applyEdits}.
     */
    function format(documentText, range, options) {
        return formatter.format(documentText, range, options);
    }
    exports.format = format;
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
    function modify(text, path, value, options) {
        return edit.setProperty(text, path, value, options);
    }
    exports.modify = modify;
    /**
     * Applies edits to an input string.
     * @param text The input text
     * @param edits Edit operations following the format described in {@linkcode EditResult}.
     * @returns The text with the applied edits.
     * @throws An error if the edit operations are not well-formed as described in {@linkcode EditResult}.
     */
    function applyEdits(text, edits) {
        for (var i = edits.length - 1; i >= 0; i--) {
            text = edit.applyEdit(text, edits[i]);
        }
        return text;
    }
    exports.applyEdits = applyEdits;
});

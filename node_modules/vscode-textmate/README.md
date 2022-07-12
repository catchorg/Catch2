# VSCode TextMate [![Build Status](https://dev.azure.com/ms/vscode-textmate/_apis/build/status/microsoft.vscode-textmate?branchName=master)](https://dev.azure.com/ms/vscode-textmate/_build/latest?definitionId=172&branchName=master)

An interpreter for grammar files as defined by TextMate. TextMate grammars use the oniguruma dialect (https://github.com/kkos/oniguruma). Supports loading grammar files from JSON or PLIST format. This library is used in VS Code. Cross - grammar injections are currently not supported.

## Installing

```sh
npm install vscode-textmate
```

## Using

```javascript
const fs = require('fs');
const vsctm = require('./release/main');
const oniguruma = require('oniguruma');

/**
 * Utility to read a file as a promise
 */
function readFile(path) {
    return new Promise((resolve, reject) => {
        fs.readFile(path, (error, data) => error ? reject(error) : resolve(data));
    })
}

// Create a registry that can create a grammar from a scope name.
const registry = new vsctm.Registry({
    onigLib: Promise.resolve({
        createOnigScanner: (sources) => new oniguruma.OnigScanner(sources),
        createOnigString: (str) => new oniguruma.OnigString(str)
    }),
    loadGrammar: (scopeName) => {
        if (scopeName === 'source.js') {
            // https://github.com/textmate/javascript.tmbundle/blob/master/Syntaxes/JavaScript.plist
            return readFile('./JavaScript.plist').then(data => vsctm.parseRawGrammar(data.toString()))
        }
        console.log(`Unknown scope name: ${scopeName}`);
        return null;
    }
});

// Load the JavaScript grammar and any other grammars included by it async.
registry.loadGrammar('source.js').then(grammar => {
    const text = [
        `function sayHello(name) {`,
        `\treturn "Hello, " + name;`,
        `}`
    ];
    let ruleStack = vsctm.INITIAL;
    for (let i = 0; i < text.length; i++) {
        const line = text[i];
        const lineTokens = grammar.tokenizeLine(line, ruleStack);
        console.log(`\nTokenizing line: ${line}`);
        for (let j = 0; j < lineTokens.tokens.length; j++) {
            const token = lineTokens.tokens[j];
            console.log(` - token from ${token.startIndex} to ${token.endIndex} ` +
              `(${line.substring(token.startIndex, token.endIndex)}) ` +
              `with scopes ${token.scopes.join(', ')}`
            );
        }
        ruleStack = lineTokens.ruleStack;
    }
});

/* OUTPUT:

Unknown scope name: source.js.regexp

Tokenizing line: function sayHello(name) {
 - token from 0 to 8 (function) with scopes source.js, meta.function.js, storage.type.function.js
 - token from 8 to 9 ( ) with scopes source.js, meta.function.js
 - token from 9 to 17 (sayHello) with scopes source.js, meta.function.js, entity.name.function.js
 - token from 17 to 18 (() with scopes source.js, meta.function.js, punctuation.definition.parameters.begin.js
 - token from 18 to 22 (name) with scopes source.js, meta.function.js, variable.parameter.function.js
 - token from 22 to 23 ()) with scopes source.js, meta.function.js, punctuation.definition.parameters.end.js
 - token from 23 to 24 ( ) with scopes source.js
 - token from 24 to 25 ({) with scopes source.js, punctuation.section.scope.begin.js

Tokenizing line:        return "Hello, " + name;
 - token from 0 to 1 (  ) with scopes source.js
 - token from 1 to 7 (return) with scopes source.js, keyword.control.js
 - token from 7 to 8 ( ) with scopes source.js
 - token from 8 to 9 (") with scopes source.js, string.quoted.double.js, punctuation.definition.string.begin.js
 - token from 9 to 16 (Hello, ) with scopes source.js, string.quoted.double.js
 - token from 16 to 17 (") with scopes source.js, string.quoted.double.js, punctuation.definition.string.end.js
 - token from 17 to 18 ( ) with scopes source.js
 - token from 18 to 19 (+) with scopes source.js, keyword.operator.arithmetic.js
 - token from 19 to 20 ( ) with scopes source.js
 - token from 20 to 24 (name) with scopes source.js, support.constant.dom.js
 - token from 24 to 25 (;) with scopes source.js, punctuation.terminator.statement.js

Tokenizing line: }
 - token from 0 to 1 (}) with scopes source.js, punctuation.section.scope.end.js

*/

```

## For grammar authors

See [vscode-tmgrammar-test](https://github.com/PanAeon/vscode-tmgrammar-test) that can help you write unit tests against your grammar.

## API doc

See [the main.ts file](./src/main.ts)

## Developing

* Clone the repository
* Run `npm install`
* Compile in the background with `npm run watch`
* Run tests with `npm test`
* Run benchmark with `npm run benchmark`
* Troubleshoot a grammar with `npm run inspect -- PATH_TO_GRAMMAR PATH_TO_FILE`

## Code of Conduct

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.


## License
[MIT](https://github.com/Microsoft/vscode-textmate/blob/master/LICENSE.md)


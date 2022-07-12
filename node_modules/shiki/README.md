<p>
  <h2 align="center">Shiki</h2>
</p>
<p align="center">
  Shiki is a beautiful Syntax Highlighter. <a href="http://shiki.matsu.io">Demo</a>.
</p>

## Usage

```sh
npm i shiki
# yarn add shiki
```

```js
const shiki = require('shiki')

shiki
  .getHighlighter({
    theme: 'nord'
  })
  .then(highlighter => {
    console.log(highlighter.codeToHtml(`console.log('shiki');`, { lang: 'js' }))
  })

// <pre class="shiki" style="background-color: #2e3440"><code>
//   <!-- Highlighted Code -->
// </code></pre>
```

```html
<script src="https://unpkg.com/shiki"></script>
<script>
  shiki
    .getHighlighter({
      theme: 'nord'
    })
    .then(highlighter => {
      const code = highlighter.codeToHtml(`console.log('shiki');`, { lang: 'js' })
      document.getElementById('output').innerHTML = code
    })
</script>
```

Clone [shikijs/shiki-starter](https://github.com/shikijs/shiki-starter) to play with Shiki, or try it out on [Repl.it](https://repl.it/@octref/shiki-starter).

## Seen

- Shiki Docs: https://shiki.matsu.io
- Interactive Demo on CodeSandbox (with Next.js): https://codesandbox.io/s/shiki-next-js-cir0y
- [VS Code website](https://code.visualstudio.com), such as in the [Notebook API page](https://code.visualstudio.com/api/extension-guides/notebook).
- [TypeScript website](https://www.typescriptlang.org), such as in the [Basic Types documentation page](https://www.typescriptlang.org/docs/handbook/basic-types.html#tuple).
- [Markdown Preview Shiki Highlighting](https://marketplace.visualstudio.com/items?itemName=bierner.markdown-Shiki), a VS Code plugin to use Shiki's highlighting in Markdown preview.
- [Shiki LaTeX](https://www.npmjs.com/package/shiki-latex), a Shiki renderer for [LaTeX](https://www.latex-project.org). Compatible with [minted](https://github.com/gpoore/minted), replacing [Pygments](https://pygments.org).
- [Fatih Kalifa's website](https://fatihkalifa.com/typescript-twoslash)
- [Blockstack Documentation](https://docs.blockstack.org/)

## Contributing

- [Add a theme](./docs/themes.md#add)
- [Add a language grammar](./docs/languages.md#add)

## Credits

- Shiki uses [vscode-oniguruma](https://github.com/microsoft/vscode-oniguruma)
- A lot of code is based on [vscode-textmate](https://github.com/Microsoft/vscode-textmate)

## Sponsorship

If you find Shiki useful, please consider sponsoring my Open Source development. Thank you 🙏

https://github.com/sponsors/octref

## License

MIT © [Pine Wu](https://github.com/octref)

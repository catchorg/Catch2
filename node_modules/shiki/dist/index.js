'use strict';

Object.defineProperty(exports, '__esModule', { value: true });

var vscodeOniguruma = require('vscode-oniguruma');
var vscodeTextmate = require('vscode-textmate');

const themes = [
    'css-variables',
    'dark-plus',
    'dracula-soft',
    'dracula',
    'github-dark-dimmed',
    'github-dark',
    'github-light',
    'light-plus',
    'material-darker',
    'material-default',
    'material-lighter',
    'material-ocean',
    'material-palenight',
    'min-dark',
    'min-light',
    'monokai',
    'nord',
    'one-dark-pro',
    'poimandres',
    'rose-pine-dawn',
    'rose-pine-moon',
    'rose-pine',
    'slack-dark',
    'slack-ochin',
    'solarized-dark',
    'solarized-light',
    'vitesse-dark',
    'vitesse-light'
];

const languages = [
    {
        id: 'abap',
        scopeName: 'source.abap',
        path: 'abap.tmLanguage.json',
        samplePath: 'abap.sample'
    },
    {
        id: 'actionscript-3',
        scopeName: 'source.actionscript.3',
        path: 'actionscript-3.tmLanguage.json',
        samplePath: 'actionscript-3.sample'
    },
    {
        id: 'ada',
        scopeName: 'source.ada',
        path: 'ada.tmLanguage.json',
        samplePath: 'ada.sample'
    },
    {
        id: 'apache',
        scopeName: 'source.apacheconf',
        path: 'apache.tmLanguage.json'
    },
    {
        id: 'apex',
        scopeName: 'source.apex',
        path: 'apex.tmLanguage.json',
        samplePath: 'apex.sample'
    },
    {
        id: 'apl',
        scopeName: 'source.apl',
        path: 'apl.tmLanguage.json',
        embeddedLangs: ['html', 'xml', 'css', 'javascript', 'json']
    },
    {
        id: 'applescript',
        scopeName: 'source.applescript',
        path: 'applescript.tmLanguage.json',
        samplePath: 'applescript.sample'
    },
    {
        id: 'asm',
        scopeName: 'source.asm.x86_64',
        path: 'asm.tmLanguage.json',
        samplePath: 'asm.sample'
    },
    {
        id: 'astro',
        scopeName: 'text.html.astro',
        path: 'astro.tmLanguage.json',
        samplePath: 'astro.sample',
        embeddedLangs: ['css', 'javascript', 'less', 'sass', 'scss', 'stylus', 'typescript', 'tsx']
    },
    {
        id: 'awk',
        scopeName: 'source.awk',
        path: 'awk.tmLanguage.json',
        samplePath: 'awk.sample'
    },
    {
        id: 'ballerina',
        scopeName: 'source.ballerina',
        path: 'ballerina.tmLanguage.json',
        samplePath: 'ballerina.sample'
    },
    {
        id: 'bat',
        scopeName: 'source.batchfile',
        path: 'bat.tmLanguage.json',
        samplePath: 'bat.sample',
        aliases: ['batch']
    },
    {
        id: 'berry',
        scopeName: 'source.berry',
        path: 'berry.tmLanguage.json',
        samplePath: 'berry.sample',
        aliases: ['be']
    },
    {
        id: 'bibtex',
        scopeName: 'text.bibtex',
        path: 'bibtex.tmLanguage.json'
    },
    {
        id: 'bicep',
        scopeName: 'source.bicep',
        path: 'bicep.tmLanguage.json',
        samplePath: 'bicep.sample'
    },
    {
        id: 'c',
        scopeName: 'source.c',
        path: 'c.tmLanguage.json',
        samplePath: 'c.sample'
    },
    {
        id: 'clojure',
        scopeName: 'source.clojure',
        path: 'clojure.tmLanguage.json',
        samplePath: 'clojure.sample',
        aliases: ['clj']
    },
    {
        id: 'cobol',
        scopeName: 'source.cobol',
        path: 'cobol.tmLanguage.json',
        samplePath: 'cobol.sample',
        embeddedLangs: ['sql', 'html', 'java']
    },
    {
        id: 'codeql',
        scopeName: 'source.ql',
        path: 'codeql.tmLanguage.json',
        samplePath: 'codeql.sample',
        aliases: ['ql']
    },
    {
        id: 'coffee',
        scopeName: 'source.coffee',
        path: 'coffee.tmLanguage.json',
        samplePath: 'coffee.sample',
        embeddedLangs: ['javascript']
    },
    {
        id: 'cpp',
        scopeName: 'source.cpp',
        path: 'cpp.tmLanguage.json',
        samplePath: 'cpp.sample',
        embeddedLangs: ['sql']
    },
    {
        id: 'crystal',
        scopeName: 'source.crystal',
        path: 'crystal.tmLanguage.json',
        samplePath: 'crystal.sample',
        embeddedLangs: ['html', 'sql', 'css', 'c', 'javascript', 'shellscript']
    },
    {
        id: 'csharp',
        scopeName: 'source.cs',
        path: 'csharp.tmLanguage.json',
        samplePath: 'csharp.sample',
        aliases: ['c#']
    },
    {
        id: 'css',
        scopeName: 'source.css',
        path: 'css.tmLanguage.json',
        samplePath: 'css.sample'
    },
    {
        id: 'cue',
        scopeName: 'source.cue',
        path: 'cue.tmLanguage.json',
        samplePath: 'cue.sample'
    },
    {
        id: 'd',
        scopeName: 'source.d',
        path: 'd.tmLanguage.json',
        samplePath: 'd.sample'
    },
    {
        id: 'dart',
        scopeName: 'source.dart',
        path: 'dart.tmLanguage.json',
        samplePath: 'dart.sample'
    },
    {
        id: 'diff',
        scopeName: 'source.diff',
        path: 'diff.tmLanguage.json',
        samplePath: 'diff.sample'
    },
    {
        id: 'docker',
        scopeName: 'source.dockerfile',
        path: 'docker.tmLanguage.json',
        samplePath: 'docker.sample'
    },
    {
        id: 'dream-maker',
        scopeName: 'source.dm',
        path: 'dream-maker.tmLanguage.json'
    },
    {
        id: 'elixir',
        scopeName: 'source.elixir',
        path: 'elixir.tmLanguage.json',
        samplePath: 'elixir.sample',
        embeddedLangs: ['html']
    },
    {
        id: 'elm',
        scopeName: 'source.elm',
        path: 'elm.tmLanguage.json',
        samplePath: 'elm.sample'
    },
    {
        id: 'erb',
        scopeName: 'text.html.erb',
        path: 'erb.tmLanguage.json',
        samplePath: 'erb.sample',
        embeddedLangs: ['html', 'ruby']
    },
    {
        id: 'erlang',
        scopeName: 'source.erlang',
        path: 'erlang.tmLanguage.json',
        samplePath: 'erlang.sample'
    },
    {
        id: 'fish',
        scopeName: 'source.fish',
        path: 'fish.tmLanguage.json',
        samplePath: 'fish.sample'
    },
    {
        id: 'fsharp',
        scopeName: 'source.fsharp',
        path: 'fsharp.tmLanguage.json',
        samplePath: 'fsharp.sample',
        aliases: ['f#'],
        embeddedLangs: ['markdown']
    },
    {
        id: 'gherkin',
        scopeName: 'text.gherkin.feature',
        path: 'gherkin.tmLanguage.json'
    },
    {
        id: 'git-commit',
        scopeName: 'text.git-commit',
        path: 'git-commit.tmLanguage.json',
        embeddedLangs: ['diff']
    },
    {
        id: 'git-rebase',
        scopeName: 'text.git-rebase',
        path: 'git-rebase.tmLanguage.json',
        embeddedLangs: ['shellscript']
    },
    {
        id: 'gnuplot',
        scopeName: 'source.gnuplot',
        path: 'gnuplot.tmLanguage.json'
    },
    {
        id: 'go',
        scopeName: 'source.go',
        path: 'go.tmLanguage.json',
        samplePath: 'go.sample'
    },
    {
        id: 'graphql',
        scopeName: 'source.graphql',
        path: 'graphql.tmLanguage.json',
        embeddedLangs: ['javascript', 'typescript', 'jsx', 'tsx']
    },
    {
        id: 'groovy',
        scopeName: 'source.groovy',
        path: 'groovy.tmLanguage.json'
    },
    {
        id: 'hack',
        scopeName: 'source.hack',
        path: 'hack.tmLanguage.json',
        embeddedLangs: ['html', 'sql']
    },
    {
        id: 'haml',
        scopeName: 'text.haml',
        path: 'haml.tmLanguage.json',
        embeddedLangs: ['ruby', 'javascript', 'sass', 'coffee', 'markdown', 'css']
    },
    {
        id: 'handlebars',
        scopeName: 'text.html.handlebars',
        path: 'handlebars.tmLanguage.json',
        aliases: ['hbs'],
        embeddedLangs: ['html', 'css', 'javascript', 'yaml']
    },
    {
        id: 'haskell',
        scopeName: 'source.haskell',
        path: 'haskell.tmLanguage.json'
    },
    {
        id: 'hcl',
        scopeName: 'source.hcl',
        path: 'hcl.tmLanguage.json'
    },
    {
        id: 'hlsl',
        scopeName: 'source.hlsl',
        path: 'hlsl.tmLanguage.json'
    },
    {
        id: 'html',
        scopeName: 'text.html.basic',
        path: 'html.tmLanguage.json',
        samplePath: 'html.sample',
        embeddedLangs: ['javascript', 'css']
    },
    {
        id: 'ini',
        scopeName: 'source.ini',
        path: 'ini.tmLanguage.json'
    },
    {
        id: 'java',
        scopeName: 'source.java',
        path: 'java.tmLanguage.json',
        samplePath: 'java.sample'
    },
    {
        id: 'javascript',
        scopeName: 'source.js',
        path: 'javascript.tmLanguage.json',
        samplePath: 'javascript.sample',
        aliases: ['js']
    },
    {
        id: 'jinja-html',
        scopeName: 'text.html.jinja',
        path: 'jinja-html.tmLanguage.json',
        embeddedLangs: ['html']
    },
    {
        id: 'json',
        scopeName: 'source.json',
        path: 'json.tmLanguage.json'
    },
    {
        id: 'jsonc',
        scopeName: 'source.json.comments',
        path: 'jsonc.tmLanguage.json'
    },
    {
        id: 'jsonnet',
        scopeName: 'source.jsonnet',
        path: 'jsonnet.tmLanguage.json'
    },
    {
        id: 'jssm',
        scopeName: 'source.jssm',
        path: 'jssm.tmLanguage.json',
        samplePath: 'jssm.sample',
        aliases: ['fsl']
    },
    {
        id: 'jsx',
        scopeName: 'source.js.jsx',
        path: 'jsx.tmLanguage.json'
    },
    {
        id: 'julia',
        scopeName: 'source.julia',
        path: 'julia.tmLanguage.json',
        embeddedLangs: ['cpp', 'python', 'javascript', 'r', 'sql']
    },
    {
        id: 'jupyter',
        scopeName: 'source.jupyter',
        path: 'jupyter.tmLanguage.json',
        embeddedLangs: ['json']
    },
    {
        id: 'kotlin',
        scopeName: 'source.kotlin',
        path: 'kotlin.tmLanguage.json'
    },
    {
        id: 'latex',
        scopeName: 'text.tex.latex',
        path: 'latex.tmLanguage.json',
        embeddedLangs: ['tex', 'css', 'html', 'java', 'javascript', 'typescript', 'lua', 'python', 'julia', 'ruby', 'xml', 'yaml', 'cpp', 'haskell', 'scala', 'gnuplot']
    },
    {
        id: 'less',
        scopeName: 'source.css.less',
        path: 'less.tmLanguage.json',
        embeddedLangs: ['css']
    },
    {
        id: 'lisp',
        scopeName: 'source.lisp',
        path: 'lisp.tmLanguage.json'
    },
    {
        id: 'logo',
        scopeName: 'source.logo',
        path: 'logo.tmLanguage.json'
    },
    {
        id: 'lua',
        scopeName: 'source.lua',
        path: 'lua.tmLanguage.json',
        embeddedLangs: ['c']
    },
    {
        id: 'make',
        scopeName: 'source.makefile',
        path: 'make.tmLanguage.json',
        aliases: ['makefile']
    },
    {
        id: 'markdown',
        scopeName: 'text.html.markdown',
        path: 'markdown.tmLanguage.json',
        aliases: ['md'],
        embeddedLangs: ['css', 'html', 'ini', 'java', 'lua', 'make', 'perl', 'r', 'ruby', 'php', 'sql', 'vb', 'xml', 'xsl', 'yaml', 'bat', 'clojure', 'coffee', 'c', 'cpp', 'diff', 'docker', 'git-commit', 'git-rebase', 'go', 'groovy', 'pug', 'javascript', 'json', 'jsonc', 'less', 'objective-c', 'swift', 'scss', 'raku', 'powershell', 'python', 'rust', 'scala', 'shellscript', 'typescript', 'tsx', 'csharp', 'fsharp', 'dart', 'handlebars', 'erlang', 'elixir', 'latex', 'bibtex']
    },
    {
        id: 'marko',
        scopeName: 'text.marko',
        path: 'marko.tmLanguage.json',
        samplePath: 'marko.sample',
        embeddedLangs: ['css', 'less', 'scss', 'javascript']
    },
    {
        id: 'matlab',
        scopeName: 'source.matlab',
        path: 'matlab.tmLanguage.json'
    },
    {
        id: 'mdx',
        scopeName: 'text.html.markdown.jsx',
        path: 'mdx.tmLanguage.json',
        embeddedLangs: ['jsx', 'markdown']
    },
    {
        id: 'nginx',
        scopeName: 'source.nginx',
        path: 'nginx.tmLanguage.json',
        embeddedLangs: ['lua']
    },
    {
        id: 'nim',
        scopeName: 'source.nim',
        path: 'nim.tmLanguage.json',
        embeddedLangs: ['c', 'html', 'xml', 'javascript', 'css', 'markdown']
    },
    {
        id: 'nix',
        scopeName: 'source.nix',
        path: 'nix.tmLanguage.json'
    },
    {
        id: 'objective-c',
        scopeName: 'source.objc',
        path: 'objective-c.tmLanguage.json',
        aliases: ['objc']
    },
    {
        id: 'objective-cpp',
        scopeName: 'source.objcpp',
        path: 'objective-cpp.tmLanguage.json'
    },
    {
        id: 'ocaml',
        scopeName: 'source.ocaml',
        path: 'ocaml.tmLanguage.json'
    },
    {
        id: 'pascal',
        scopeName: 'source.pascal',
        path: 'pascal.tmLanguage.json'
    },
    {
        id: 'perl',
        scopeName: 'source.perl',
        path: 'perl.tmLanguage.json',
        embeddedLangs: ['html', 'xml', 'css', 'javascript', 'sql']
    },
    {
        id: 'php',
        scopeName: 'source.php',
        path: 'php.tmLanguage.json',
        embeddedLangs: ['html', 'xml', 'sql', 'javascript', 'json', 'css']
    },
    {
        id: 'plsql',
        scopeName: 'source.plsql.oracle',
        path: 'plsql.tmLanguage.json'
    },
    {
        id: 'postcss',
        scopeName: 'source.css.postcss',
        path: 'postcss.tmLanguage.json'
    },
    {
        id: 'powershell',
        scopeName: 'source.powershell',
        path: 'powershell.tmLanguage.json',
        aliases: ['ps', 'ps1']
    },
    {
        id: 'prisma',
        scopeName: 'source.prisma',
        path: 'prisma.tmLanguage.json',
        samplePath: 'prisma.sample'
    },
    {
        id: 'prolog',
        scopeName: 'source.prolog',
        path: 'prolog.tmLanguage.json'
    },
    {
        id: 'pug',
        scopeName: 'text.pug',
        path: 'pug.tmLanguage.json',
        aliases: ['jade'],
        embeddedLangs: ['javascript', 'css', 'sass', 'stylus', 'coffee', 'html']
    },
    {
        id: 'puppet',
        scopeName: 'source.puppet',
        path: 'puppet.tmLanguage.json'
    },
    {
        id: 'purescript',
        scopeName: 'source.purescript',
        path: 'purescript.tmLanguage.json'
    },
    {
        id: 'python',
        scopeName: 'source.python',
        path: 'python.tmLanguage.json',
        samplePath: 'python.sample',
        aliases: ['py']
    },
    {
        id: 'r',
        scopeName: 'source.r',
        path: 'r.tmLanguage.json'
    },
    {
        id: 'raku',
        scopeName: 'source.perl.6',
        path: 'raku.tmLanguage.json',
        aliases: ['perl6']
    },
    {
        id: 'razor',
        scopeName: 'text.aspnetcorerazor',
        path: 'razor.tmLanguage.json',
        embeddedLangs: ['html', 'csharp']
    },
    {
        id: 'rel',
        scopeName: 'source.rel',
        path: 'rel.tmLanguage.json',
        samplePath: 'rel.sample'
    },
    {
        id: 'riscv',
        scopeName: 'source.riscv',
        path: 'riscv.tmLanguage.json'
    },
    {
        id: 'ruby',
        scopeName: 'source.ruby',
        path: 'ruby.tmLanguage.json',
        samplePath: 'ruby.sample',
        aliases: ['rb'],
        embeddedLangs: ['html', 'xml', 'sql', 'css', 'c', 'javascript', 'shellscript', 'lua']
    },
    {
        id: 'rust',
        scopeName: 'source.rust',
        path: 'rust.tmLanguage.json',
        aliases: ['rs']
    },
    {
        id: 'sas',
        scopeName: 'source.sas',
        path: 'sas.tmLanguage.json',
        embeddedLangs: ['sql']
    },
    {
        id: 'sass',
        scopeName: 'source.sass',
        path: 'sass.tmLanguage.json'
    },
    {
        id: 'scala',
        scopeName: 'source.scala',
        path: 'scala.tmLanguage.json'
    },
    {
        id: 'scheme',
        scopeName: 'source.scheme',
        path: 'scheme.tmLanguage.json'
    },
    {
        id: 'scss',
        scopeName: 'source.css.scss',
        path: 'scss.tmLanguage.json',
        embeddedLangs: ['css']
    },
    {
        id: 'shaderlab',
        scopeName: 'source.shaderlab',
        path: 'shaderlab.tmLanguage.json',
        aliases: ['shader'],
        embeddedLangs: ['hlsl']
    },
    {
        id: 'shellscript',
        scopeName: 'source.shell',
        path: 'shellscript.tmLanguage.json',
        aliases: ['shell', 'bash', 'sh', 'zsh'],
        embeddedLangs: ['ruby', 'python', 'applescript', 'html', 'markdown']
    },
    {
        id: 'smalltalk',
        scopeName: 'source.smalltalk',
        path: 'smalltalk.tmLanguage.json'
    },
    {
        id: 'solidity',
        scopeName: 'source.solidity',
        path: 'solidity.tmLanguage.json'
    },
    {
        id: 'sparql',
        scopeName: 'source.sparql',
        path: 'sparql.tmLanguage.json',
        samplePath: 'sparql.sample',
        embeddedLangs: ['turtle']
    },
    {
        id: 'sql',
        scopeName: 'source.sql',
        path: 'sql.tmLanguage.json'
    },
    {
        id: 'ssh-config',
        scopeName: 'source.ssh-config',
        path: 'ssh-config.tmLanguage.json'
    },
    {
        id: 'stata',
        scopeName: 'source.stata',
        path: 'stata.tmLanguage.json',
        samplePath: 'stata.sample',
        embeddedLangs: ['sql']
    },
    {
        id: 'stylus',
        scopeName: 'source.stylus',
        path: 'stylus.tmLanguage.json',
        aliases: ['styl']
    },
    {
        id: 'svelte',
        scopeName: 'source.svelte',
        path: 'svelte.tmLanguage.json',
        embeddedLangs: ['javascript', 'typescript', 'coffee', 'stylus', 'sass', 'css', 'scss', 'less', 'postcss', 'pug', 'markdown']
    },
    {
        id: 'swift',
        scopeName: 'source.swift',
        path: 'swift.tmLanguage.json'
    },
    {
        id: 'system-verilog',
        scopeName: 'source.systemverilog',
        path: 'system-verilog.tmLanguage.json'
    },
    {
        id: 'tasl',
        scopeName: 'source.tasl',
        path: 'tasl.tmLanguage.json',
        samplePath: 'tasl.sample'
    },
    {
        id: 'tcl',
        scopeName: 'source.tcl',
        path: 'tcl.tmLanguage.json'
    },
    {
        id: 'tex',
        scopeName: 'text.tex',
        path: 'tex.tmLanguage.json',
        embeddedLangs: ['r']
    },
    {
        id: 'toml',
        scopeName: 'source.toml',
        path: 'toml.tmLanguage.json'
    },
    {
        id: 'tsx',
        scopeName: 'source.tsx',
        path: 'tsx.tmLanguage.json',
        samplePath: 'tsx.sample'
    },
    {
        id: 'turtle',
        scopeName: 'source.turtle',
        path: 'turtle.tmLanguage.json',
        samplePath: 'turtle.sample'
    },
    {
        id: 'twig',
        scopeName: 'text.html.twig',
        path: 'twig.tmLanguage.json',
        embeddedLangs: ['css', 'javascript', 'php', 'python', 'ruby']
    },
    {
        id: 'typescript',
        scopeName: 'source.ts',
        path: 'typescript.tmLanguage.json',
        aliases: ['ts']
    },
    {
        id: 'vb',
        scopeName: 'source.asp.vb.net',
        path: 'vb.tmLanguage.json',
        aliases: ['cmd']
    },
    {
        id: 'verilog',
        scopeName: 'source.verilog',
        path: 'verilog.tmLanguage.json'
    },
    {
        id: 'vhdl',
        scopeName: 'source.vhdl',
        path: 'vhdl.tmLanguage.json'
    },
    {
        id: 'viml',
        scopeName: 'source.viml',
        path: 'viml.tmLanguage.json',
        aliases: ['vim', 'vimscript']
    },
    {
        id: 'vue-html',
        scopeName: 'text.html.vue-html',
        path: 'vue-html.tmLanguage.json',
        embeddedLangs: ['vue', 'javascript']
    },
    {
        id: 'vue',
        scopeName: 'source.vue',
        path: 'vue.tmLanguage.json',
        embeddedLangs: ['json', 'markdown', 'pug', 'haml', 'vue-html', 'sass', 'scss', 'less', 'stylus', 'postcss', 'css', 'typescript', 'coffee', 'javascript']
    },
    {
        id: 'wasm',
        scopeName: 'source.wat',
        path: 'wasm.tmLanguage.json'
    },
    {
        id: 'wenyan',
        scopeName: 'source.wenyan',
        path: 'wenyan.tmLanguage.json',
        aliases: ['文言']
    },
    {
        id: 'xml',
        scopeName: 'text.xml',
        path: 'xml.tmLanguage.json',
        embeddedLangs: ['java']
    },
    {
        id: 'xsl',
        scopeName: 'text.xml.xsl',
        path: 'xsl.tmLanguage.json',
        embeddedLangs: ['xml']
    },
    {
        id: 'yaml',
        scopeName: 'source.yaml',
        path: 'yaml.tmLanguage.json'
    },
    {
        id: 'zenscript',
        scopeName: 'source.zenscript',
        path: 'zenscript.tmLanguage.json',
        samplePath: 'zenscript.sample'
    }
];

exports.FontStyle = void 0;
(function (FontStyle) {
    FontStyle[FontStyle["NotSet"] = -1] = "NotSet";
    FontStyle[FontStyle["None"] = 0] = "None";
    FontStyle[FontStyle["Italic"] = 1] = "Italic";
    FontStyle[FontStyle["Bold"] = 2] = "Bold";
    FontStyle[FontStyle["Underline"] = 4] = "Underline";
})(exports.FontStyle || (exports.FontStyle = {}));
class StackElementMetadata {
    static toBinaryStr(metadata) {
        let r = metadata.toString(2);
        while (r.length < 32) {
            r = '0' + r;
        }
        return r;
    }
    static printMetadata(metadata) {
        let languageId = StackElementMetadata.getLanguageId(metadata);
        let tokenType = StackElementMetadata.getTokenType(metadata);
        let fontStyle = StackElementMetadata.getFontStyle(metadata);
        let foreground = StackElementMetadata.getForeground(metadata);
        let background = StackElementMetadata.getBackground(metadata);
        console.log({
            languageId: languageId,
            tokenType: tokenType,
            fontStyle: fontStyle,
            foreground: foreground,
            background: background
        });
    }
    static getLanguageId(metadata) {
        return (metadata & 255 /* LANGUAGEID_MASK */) >>> 0 /* LANGUAGEID_OFFSET */;
    }
    static getTokenType(metadata) {
        return (metadata & 1792 /* TOKEN_TYPE_MASK */) >>> 8 /* TOKEN_TYPE_OFFSET */;
    }
    static getFontStyle(metadata) {
        return (metadata & 14336 /* FONT_STYLE_MASK */) >>> 11 /* FONT_STYLE_OFFSET */;
    }
    static getForeground(metadata) {
        return (metadata & 8372224 /* FOREGROUND_MASK */) >>> 14 /* FOREGROUND_OFFSET */;
    }
    static getBackground(metadata) {
        return (metadata & 4286578688 /* BACKGROUND_MASK */) >>> 23 /* BACKGROUND_OFFSET */;
    }
    static set(metadata, languageId, tokenType, fontStyle, foreground, background) {
        let _languageId = StackElementMetadata.getLanguageId(metadata);
        let _tokenType = StackElementMetadata.getTokenType(metadata);
        let _fontStyle = StackElementMetadata.getFontStyle(metadata);
        let _foreground = StackElementMetadata.getForeground(metadata);
        let _background = StackElementMetadata.getBackground(metadata);
        if (languageId !== 0) {
            _languageId = languageId;
        }
        if (tokenType !== 0 /* Other */) {
            _tokenType =
                tokenType === 8 /* MetaEmbedded */ ? 0 /* Other */ : tokenType;
        }
        if (fontStyle !== exports.FontStyle.NotSet) {
            _fontStyle = fontStyle;
        }
        if (foreground !== 0) {
            _foreground = foreground;
        }
        if (background !== 0) {
            _background = background;
        }
        return (((_languageId << 0 /* LANGUAGEID_OFFSET */) |
            (_tokenType << 8 /* TOKEN_TYPE_OFFSET */) |
            (_fontStyle << 11 /* FONT_STYLE_OFFSET */) |
            (_foreground << 14 /* FOREGROUND_OFFSET */) |
            (_background << 23 /* BACKGROUND_OFFSET */)) >>>
            0);
    }
}

function trimEndSlash(str) {
    if (str.endsWith('/') || str.endsWith('\\'))
        return str.slice(0, -1);
    return str;
}
function trimStartDot(str) {
    if (str.startsWith('./'))
        return str.slice(2);
    return str;
}
function dirname(str) {
    const parts = str.split(/[\/\\]/g);
    return parts[parts.length - 2];
}
function join(...parts) {
    return parts.map(trimEndSlash).map(trimStartDot).join('/');
}
function groupBy(elements, keyGetter) {
    const map = new Map();
    for (const element of elements) {
        const key = keyGetter(element);
        if (map.has(key)) {
            const group = map.get(key);
            group.push(element);
        }
        else {
            map.set(key, [element]);
        }
    }
    return map;
}

/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
/**
 * Creates a JSON scanner on the given text.
 * If ignoreTrivia is set, whitespaces or comments are ignored.
 */
function createScanner(text, ignoreTrivia) {
    if (ignoreTrivia === void 0) { ignoreTrivia = false; }
    var len = text.length;
    var pos = 0, value = '', tokenOffset = 0, token = 16 /* Unknown */, lineNumber = 0, lineStartOffset = 0, tokenLineStartOffset = 0, prevTokenLineStartOffset = 0, scanError = 0 /* None */;
    function scanHexDigits(count, exact) {
        var digits = 0;
        var value = 0;
        while (digits < count || !exact) {
            var ch = text.charCodeAt(pos);
            if (ch >= 48 /* _0 */ && ch <= 57 /* _9 */) {
                value = value * 16 + ch - 48 /* _0 */;
            }
            else if (ch >= 65 /* A */ && ch <= 70 /* F */) {
                value = value * 16 + ch - 65 /* A */ + 10;
            }
            else if (ch >= 97 /* a */ && ch <= 102 /* f */) {
                value = value * 16 + ch - 97 /* a */ + 10;
            }
            else {
                break;
            }
            pos++;
            digits++;
        }
        if (digits < count) {
            value = -1;
        }
        return value;
    }
    function setPosition(newPosition) {
        pos = newPosition;
        value = '';
        tokenOffset = 0;
        token = 16 /* Unknown */;
        scanError = 0 /* None */;
    }
    function scanNumber() {
        var start = pos;
        if (text.charCodeAt(pos) === 48 /* _0 */) {
            pos++;
        }
        else {
            pos++;
            while (pos < text.length && isDigit(text.charCodeAt(pos))) {
                pos++;
            }
        }
        if (pos < text.length && text.charCodeAt(pos) === 46 /* dot */) {
            pos++;
            if (pos < text.length && isDigit(text.charCodeAt(pos))) {
                pos++;
                while (pos < text.length && isDigit(text.charCodeAt(pos))) {
                    pos++;
                }
            }
            else {
                scanError = 3 /* UnexpectedEndOfNumber */;
                return text.substring(start, pos);
            }
        }
        var end = pos;
        if (pos < text.length && (text.charCodeAt(pos) === 69 /* E */ || text.charCodeAt(pos) === 101 /* e */)) {
            pos++;
            if (pos < text.length && text.charCodeAt(pos) === 43 /* plus */ || text.charCodeAt(pos) === 45 /* minus */) {
                pos++;
            }
            if (pos < text.length && isDigit(text.charCodeAt(pos))) {
                pos++;
                while (pos < text.length && isDigit(text.charCodeAt(pos))) {
                    pos++;
                }
                end = pos;
            }
            else {
                scanError = 3 /* UnexpectedEndOfNumber */;
            }
        }
        return text.substring(start, end);
    }
    function scanString() {
        var result = '', start = pos;
        while (true) {
            if (pos >= len) {
                result += text.substring(start, pos);
                scanError = 2 /* UnexpectedEndOfString */;
                break;
            }
            var ch = text.charCodeAt(pos);
            if (ch === 34 /* doubleQuote */) {
                result += text.substring(start, pos);
                pos++;
                break;
            }
            if (ch === 92 /* backslash */) {
                result += text.substring(start, pos);
                pos++;
                if (pos >= len) {
                    scanError = 2 /* UnexpectedEndOfString */;
                    break;
                }
                var ch2 = text.charCodeAt(pos++);
                switch (ch2) {
                    case 34 /* doubleQuote */:
                        result += '\"';
                        break;
                    case 92 /* backslash */:
                        result += '\\';
                        break;
                    case 47 /* slash */:
                        result += '/';
                        break;
                    case 98 /* b */:
                        result += '\b';
                        break;
                    case 102 /* f */:
                        result += '\f';
                        break;
                    case 110 /* n */:
                        result += '\n';
                        break;
                    case 114 /* r */:
                        result += '\r';
                        break;
                    case 116 /* t */:
                        result += '\t';
                        break;
                    case 117 /* u */:
                        var ch3 = scanHexDigits(4, true);
                        if (ch3 >= 0) {
                            result += String.fromCharCode(ch3);
                        }
                        else {
                            scanError = 4 /* InvalidUnicode */;
                        }
                        break;
                    default:
                        scanError = 5 /* InvalidEscapeCharacter */;
                }
                start = pos;
                continue;
            }
            if (ch >= 0 && ch <= 0x1f) {
                if (isLineBreak(ch)) {
                    result += text.substring(start, pos);
                    scanError = 2 /* UnexpectedEndOfString */;
                    break;
                }
                else {
                    scanError = 6 /* InvalidCharacter */;
                    // mark as error but continue with string
                }
            }
            pos++;
        }
        return result;
    }
    function scanNext() {
        value = '';
        scanError = 0 /* None */;
        tokenOffset = pos;
        lineStartOffset = lineNumber;
        prevTokenLineStartOffset = tokenLineStartOffset;
        if (pos >= len) {
            // at the end
            tokenOffset = len;
            return token = 17 /* EOF */;
        }
        var code = text.charCodeAt(pos);
        // trivia: whitespace
        if (isWhiteSpace(code)) {
            do {
                pos++;
                value += String.fromCharCode(code);
                code = text.charCodeAt(pos);
            } while (isWhiteSpace(code));
            return token = 15 /* Trivia */;
        }
        // trivia: newlines
        if (isLineBreak(code)) {
            pos++;
            value += String.fromCharCode(code);
            if (code === 13 /* carriageReturn */ && text.charCodeAt(pos) === 10 /* lineFeed */) {
                pos++;
                value += '\n';
            }
            lineNumber++;
            tokenLineStartOffset = pos;
            return token = 14 /* LineBreakTrivia */;
        }
        switch (code) {
            // tokens: []{}:,
            case 123 /* openBrace */:
                pos++;
                return token = 1 /* OpenBraceToken */;
            case 125 /* closeBrace */:
                pos++;
                return token = 2 /* CloseBraceToken */;
            case 91 /* openBracket */:
                pos++;
                return token = 3 /* OpenBracketToken */;
            case 93 /* closeBracket */:
                pos++;
                return token = 4 /* CloseBracketToken */;
            case 58 /* colon */:
                pos++;
                return token = 6 /* ColonToken */;
            case 44 /* comma */:
                pos++;
                return token = 5 /* CommaToken */;
            // strings
            case 34 /* doubleQuote */:
                pos++;
                value = scanString();
                return token = 10 /* StringLiteral */;
            // comments
            case 47 /* slash */:
                var start = pos - 1;
                // Single-line comment
                if (text.charCodeAt(pos + 1) === 47 /* slash */) {
                    pos += 2;
                    while (pos < len) {
                        if (isLineBreak(text.charCodeAt(pos))) {
                            break;
                        }
                        pos++;
                    }
                    value = text.substring(start, pos);
                    return token = 12 /* LineCommentTrivia */;
                }
                // Multi-line comment
                if (text.charCodeAt(pos + 1) === 42 /* asterisk */) {
                    pos += 2;
                    var safeLength = len - 1; // For lookahead.
                    var commentClosed = false;
                    while (pos < safeLength) {
                        var ch = text.charCodeAt(pos);
                        if (ch === 42 /* asterisk */ && text.charCodeAt(pos + 1) === 47 /* slash */) {
                            pos += 2;
                            commentClosed = true;
                            break;
                        }
                        pos++;
                        if (isLineBreak(ch)) {
                            if (ch === 13 /* carriageReturn */ && text.charCodeAt(pos) === 10 /* lineFeed */) {
                                pos++;
                            }
                            lineNumber++;
                            tokenLineStartOffset = pos;
                        }
                    }
                    if (!commentClosed) {
                        pos++;
                        scanError = 1 /* UnexpectedEndOfComment */;
                    }
                    value = text.substring(start, pos);
                    return token = 13 /* BlockCommentTrivia */;
                }
                // just a single slash
                value += String.fromCharCode(code);
                pos++;
                return token = 16 /* Unknown */;
            // numbers
            case 45 /* minus */:
                value += String.fromCharCode(code);
                pos++;
                if (pos === len || !isDigit(text.charCodeAt(pos))) {
                    return token = 16 /* Unknown */;
                }
            // found a minus, followed by a number so
            // we fall through to proceed with scanning
            // numbers
            case 48 /* _0 */:
            case 49 /* _1 */:
            case 50 /* _2 */:
            case 51 /* _3 */:
            case 52 /* _4 */:
            case 53 /* _5 */:
            case 54 /* _6 */:
            case 55 /* _7 */:
            case 56 /* _8 */:
            case 57 /* _9 */:
                value += scanNumber();
                return token = 11 /* NumericLiteral */;
            // literals and unknown symbols
            default:
                // is a literal? Read the full word.
                while (pos < len && isUnknownContentCharacter(code)) {
                    pos++;
                    code = text.charCodeAt(pos);
                }
                if (tokenOffset !== pos) {
                    value = text.substring(tokenOffset, pos);
                    // keywords: true, false, null
                    switch (value) {
                        case 'true': return token = 8 /* TrueKeyword */;
                        case 'false': return token = 9 /* FalseKeyword */;
                        case 'null': return token = 7 /* NullKeyword */;
                    }
                    return token = 16 /* Unknown */;
                }
                // some
                value += String.fromCharCode(code);
                pos++;
                return token = 16 /* Unknown */;
        }
    }
    function isUnknownContentCharacter(code) {
        if (isWhiteSpace(code) || isLineBreak(code)) {
            return false;
        }
        switch (code) {
            case 125 /* closeBrace */:
            case 93 /* closeBracket */:
            case 123 /* openBrace */:
            case 91 /* openBracket */:
            case 34 /* doubleQuote */:
            case 58 /* colon */:
            case 44 /* comma */:
            case 47 /* slash */:
                return false;
        }
        return true;
    }
    function scanNextNonTrivia() {
        var result;
        do {
            result = scanNext();
        } while (result >= 12 /* LineCommentTrivia */ && result <= 15 /* Trivia */);
        return result;
    }
    return {
        setPosition: setPosition,
        getPosition: function () { return pos; },
        scan: ignoreTrivia ? scanNextNonTrivia : scanNext,
        getToken: function () { return token; },
        getTokenValue: function () { return value; },
        getTokenOffset: function () { return tokenOffset; },
        getTokenLength: function () { return pos - tokenOffset; },
        getTokenStartLine: function () { return lineStartOffset; },
        getTokenStartCharacter: function () { return tokenOffset - prevTokenLineStartOffset; },
        getTokenError: function () { return scanError; },
    };
}
function isWhiteSpace(ch) {
    return ch === 32 /* space */ || ch === 9 /* tab */ || ch === 11 /* verticalTab */ || ch === 12 /* formFeed */ ||
        ch === 160 /* nonBreakingSpace */ || ch === 5760 /* ogham */ || ch >= 8192 /* enQuad */ && ch <= 8203 /* zeroWidthSpace */ ||
        ch === 8239 /* narrowNoBreakSpace */ || ch === 8287 /* mathematicalSpace */ || ch === 12288 /* ideographicSpace */ || ch === 65279 /* byteOrderMark */;
}
function isLineBreak(ch) {
    return ch === 10 /* lineFeed */ || ch === 13 /* carriageReturn */ || ch === 8232 /* lineSeparator */ || ch === 8233 /* paragraphSeparator */;
}
function isDigit(ch) {
    return ch >= 48 /* _0 */ && ch <= 57 /* _9 */;
}

/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
var ParseOptions;
(function (ParseOptions) {
    ParseOptions.DEFAULT = {
        allowTrailingComma: false
    };
})(ParseOptions || (ParseOptions = {}));
/**
 * Parses the given text and returns the object the JSON content represents. On invalid input, the parser tries to be as fault tolerant as possible, but still return a result.
 * Therefore always check the errors list to find out if the input was valid.
 */
function parse$1(text, errors, options) {
    if (errors === void 0) { errors = []; }
    if (options === void 0) { options = ParseOptions.DEFAULT; }
    var currentProperty = null;
    var currentParent = [];
    var previousParents = [];
    function onValue(value) {
        if (Array.isArray(currentParent)) {
            currentParent.push(value);
        }
        else if (currentProperty !== null) {
            currentParent[currentProperty] = value;
        }
    }
    var visitor = {
        onObjectBegin: function () {
            var object = {};
            onValue(object);
            previousParents.push(currentParent);
            currentParent = object;
            currentProperty = null;
        },
        onObjectProperty: function (name) {
            currentProperty = name;
        },
        onObjectEnd: function () {
            currentParent = previousParents.pop();
        },
        onArrayBegin: function () {
            var array = [];
            onValue(array);
            previousParents.push(currentParent);
            currentParent = array;
            currentProperty = null;
        },
        onArrayEnd: function () {
            currentParent = previousParents.pop();
        },
        onLiteralValue: onValue,
        onError: function (error, offset, length) {
            errors.push({ error: error, offset: offset, length: length });
        }
    };
    visit(text, visitor, options);
    return currentParent[0];
}
/**
 * Parses the given text and invokes the visitor functions for each object, array and literal reached.
 */
function visit(text, visitor, options) {
    if (options === void 0) { options = ParseOptions.DEFAULT; }
    var _scanner = createScanner(text, false);
    function toNoArgVisit(visitFunction) {
        return visitFunction ? function () { return visitFunction(_scanner.getTokenOffset(), _scanner.getTokenLength(), _scanner.getTokenStartLine(), _scanner.getTokenStartCharacter()); } : function () { return true; };
    }
    function toOneArgVisit(visitFunction) {
        return visitFunction ? function (arg) { return visitFunction(arg, _scanner.getTokenOffset(), _scanner.getTokenLength(), _scanner.getTokenStartLine(), _scanner.getTokenStartCharacter()); } : function () { return true; };
    }
    var onObjectBegin = toNoArgVisit(visitor.onObjectBegin), onObjectProperty = toOneArgVisit(visitor.onObjectProperty), onObjectEnd = toNoArgVisit(visitor.onObjectEnd), onArrayBegin = toNoArgVisit(visitor.onArrayBegin), onArrayEnd = toNoArgVisit(visitor.onArrayEnd), onLiteralValue = toOneArgVisit(visitor.onLiteralValue), onSeparator = toOneArgVisit(visitor.onSeparator), onComment = toNoArgVisit(visitor.onComment), onError = toOneArgVisit(visitor.onError);
    var disallowComments = options && options.disallowComments;
    var allowTrailingComma = options && options.allowTrailingComma;
    function scanNext() {
        while (true) {
            var token = _scanner.scan();
            switch (_scanner.getTokenError()) {
                case 4 /* InvalidUnicode */:
                    handleError(14 /* InvalidUnicode */);
                    break;
                case 5 /* InvalidEscapeCharacter */:
                    handleError(15 /* InvalidEscapeCharacter */);
                    break;
                case 3 /* UnexpectedEndOfNumber */:
                    handleError(13 /* UnexpectedEndOfNumber */);
                    break;
                case 1 /* UnexpectedEndOfComment */:
                    if (!disallowComments) {
                        handleError(11 /* UnexpectedEndOfComment */);
                    }
                    break;
                case 2 /* UnexpectedEndOfString */:
                    handleError(12 /* UnexpectedEndOfString */);
                    break;
                case 6 /* InvalidCharacter */:
                    handleError(16 /* InvalidCharacter */);
                    break;
            }
            switch (token) {
                case 12 /* LineCommentTrivia */:
                case 13 /* BlockCommentTrivia */:
                    if (disallowComments) {
                        handleError(10 /* InvalidCommentToken */);
                    }
                    else {
                        onComment();
                    }
                    break;
                case 16 /* Unknown */:
                    handleError(1 /* InvalidSymbol */);
                    break;
                case 15 /* Trivia */:
                case 14 /* LineBreakTrivia */:
                    break;
                default:
                    return token;
            }
        }
    }
    function handleError(error, skipUntilAfter, skipUntil) {
        if (skipUntilAfter === void 0) { skipUntilAfter = []; }
        if (skipUntil === void 0) { skipUntil = []; }
        onError(error);
        if (skipUntilAfter.length + skipUntil.length > 0) {
            var token = _scanner.getToken();
            while (token !== 17 /* EOF */) {
                if (skipUntilAfter.indexOf(token) !== -1) {
                    scanNext();
                    break;
                }
                else if (skipUntil.indexOf(token) !== -1) {
                    break;
                }
                token = scanNext();
            }
        }
    }
    function parseString(isValue) {
        var value = _scanner.getTokenValue();
        if (isValue) {
            onLiteralValue(value);
        }
        else {
            onObjectProperty(value);
        }
        scanNext();
        return true;
    }
    function parseLiteral() {
        switch (_scanner.getToken()) {
            case 11 /* NumericLiteral */:
                var tokenValue = _scanner.getTokenValue();
                var value = Number(tokenValue);
                if (isNaN(value)) {
                    handleError(2 /* InvalidNumberFormat */);
                    value = 0;
                }
                onLiteralValue(value);
                break;
            case 7 /* NullKeyword */:
                onLiteralValue(null);
                break;
            case 8 /* TrueKeyword */:
                onLiteralValue(true);
                break;
            case 9 /* FalseKeyword */:
                onLiteralValue(false);
                break;
            default:
                return false;
        }
        scanNext();
        return true;
    }
    function parseProperty() {
        if (_scanner.getToken() !== 10 /* StringLiteral */) {
            handleError(3 /* PropertyNameExpected */, [], [2 /* CloseBraceToken */, 5 /* CommaToken */]);
            return false;
        }
        parseString(false);
        if (_scanner.getToken() === 6 /* ColonToken */) {
            onSeparator(':');
            scanNext(); // consume colon
            if (!parseValue()) {
                handleError(4 /* ValueExpected */, [], [2 /* CloseBraceToken */, 5 /* CommaToken */]);
            }
        }
        else {
            handleError(5 /* ColonExpected */, [], [2 /* CloseBraceToken */, 5 /* CommaToken */]);
        }
        return true;
    }
    function parseObject() {
        onObjectBegin();
        scanNext(); // consume open brace
        var needsComma = false;
        while (_scanner.getToken() !== 2 /* CloseBraceToken */ && _scanner.getToken() !== 17 /* EOF */) {
            if (_scanner.getToken() === 5 /* CommaToken */) {
                if (!needsComma) {
                    handleError(4 /* ValueExpected */, [], []);
                }
                onSeparator(',');
                scanNext(); // consume comma
                if (_scanner.getToken() === 2 /* CloseBraceToken */ && allowTrailingComma) {
                    break;
                }
            }
            else if (needsComma) {
                handleError(6 /* CommaExpected */, [], []);
            }
            if (!parseProperty()) {
                handleError(4 /* ValueExpected */, [], [2 /* CloseBraceToken */, 5 /* CommaToken */]);
            }
            needsComma = true;
        }
        onObjectEnd();
        if (_scanner.getToken() !== 2 /* CloseBraceToken */) {
            handleError(7 /* CloseBraceExpected */, [2 /* CloseBraceToken */], []);
        }
        else {
            scanNext(); // consume close brace
        }
        return true;
    }
    function parseArray() {
        onArrayBegin();
        scanNext(); // consume open bracket
        var needsComma = false;
        while (_scanner.getToken() !== 4 /* CloseBracketToken */ && _scanner.getToken() !== 17 /* EOF */) {
            if (_scanner.getToken() === 5 /* CommaToken */) {
                if (!needsComma) {
                    handleError(4 /* ValueExpected */, [], []);
                }
                onSeparator(',');
                scanNext(); // consume comma
                if (_scanner.getToken() === 4 /* CloseBracketToken */ && allowTrailingComma) {
                    break;
                }
            }
            else if (needsComma) {
                handleError(6 /* CommaExpected */, [], []);
            }
            if (!parseValue()) {
                handleError(4 /* ValueExpected */, [], [4 /* CloseBracketToken */, 5 /* CommaToken */]);
            }
            needsComma = true;
        }
        onArrayEnd();
        if (_scanner.getToken() !== 4 /* CloseBracketToken */) {
            handleError(8 /* CloseBracketExpected */, [4 /* CloseBracketToken */], []);
        }
        else {
            scanNext(); // consume close bracket
        }
        return true;
    }
    function parseValue() {
        switch (_scanner.getToken()) {
            case 3 /* OpenBracketToken */:
                return parseArray();
            case 1 /* OpenBraceToken */:
                return parseObject();
            case 10 /* StringLiteral */:
                return parseString(true);
            default:
                return parseLiteral();
        }
    }
    scanNext();
    if (_scanner.getToken() === 17 /* EOF */) {
        if (options.allowEmptyContent) {
            return true;
        }
        handleError(4 /* ValueExpected */, [], []);
        return false;
    }
    if (!parseValue()) {
        handleError(4 /* ValueExpected */, [], []);
        return false;
    }
    if (_scanner.getToken() !== 17 /* EOF */) {
        handleError(9 /* EndOfFileExpected */, [], []);
    }
    return true;
}

/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
/**
 * Parses the given text and returns the object the JSON content represents. On invalid input, the parser tries to be as fault tolerant as possible, but still return a result.
 * Therefore, always check the errors list to find out if the input was valid.
 */
var parse = parse$1;

const isWebWorker = typeof self !== 'undefined' && typeof self.WorkerGlobalScope !== 'undefined';
const isBrowser = isWebWorker ||
    (typeof window !== 'undefined' &&
        typeof window.document !== 'undefined' &&
        typeof fetch !== 'undefined');
// to be replaced by rollup
let CDN_ROOT = '';
let WASM = '';
/**
 * Set the route for loading the assets
 * URL should end with `/`
 *
 * For example:
 * ```ts
 * setCDN('https://unpkg.com/shiki/') // use unpkg
 * setCDN('/assets/shiki/') // serve by yourself
 * ```
 */
function setCDN(root) {
    CDN_ROOT = root;
}
/**
 * Explicitly set the source for loading the oniguruma web assembly module.
 *
 * Accepts Url or ArrayBuffer
 */
function setWasm(path) {
    WASM = path;
}
let _onigurumaPromise = null;
async function getOniguruma() {
    if (!_onigurumaPromise) {
        let loader;
        if (isBrowser) {
            if (typeof WASM === 'string') {
                loader = vscodeOniguruma.loadWASM({
                    data: await fetch(_resolvePath('dist/onig.wasm')).then(r => r.arrayBuffer())
                });
            }
            else {
                loader = vscodeOniguruma.loadWASM(WASM);
            }
        }
        else {
            const path = require('path');
            const wasmPath = path.join(require.resolve('vscode-oniguruma'), '../onig.wasm');
            const fs = require('fs');
            const wasmBin = fs.readFileSync(wasmPath).buffer;
            loader = vscodeOniguruma.loadWASM(wasmBin);
        }
        _onigurumaPromise = loader.then(() => {
            return {
                createOnigScanner(patterns) {
                    return vscodeOniguruma.createOnigScanner(patterns);
                },
                createOnigString(s) {
                    return vscodeOniguruma.createOnigString(s);
                }
            };
        });
    }
    return _onigurumaPromise;
}
function _resolvePath(filepath) {
    if (isBrowser) {
        if (!CDN_ROOT) {
            console.warn('[Shiki] no CDN provider found, use `setCDN()` to specify the CDN for loading the resources before calling `getHighlighter()`');
        }
        return `${CDN_ROOT}${filepath}`;
    }
    else {
        const path = require('path');
        if (path.isAbsolute(filepath)) {
            return filepath;
        }
        else {
            return path.resolve(__dirname, '..', filepath);
        }
    }
}
/**
 * @param filepath assert path related to ./packages/shiki
 */
async function _fetchAssets(filepath) {
    const path = _resolvePath(filepath);
    if (isBrowser) {
        return await fetch(path).then(r => r.text());
    }
    else {
        const fs = require('fs');
        return await fs.promises.readFile(path, 'utf-8');
    }
}
async function _fetchJSONAssets(filepath) {
    const errors = [];
    const rawTheme = parse(await _fetchAssets(filepath), errors, {
        allowTrailingComma: true
    });
    if (errors.length) {
        throw errors[0];
    }
    return rawTheme;
}
/**
 * @param themePath related path to theme.json
 */
async function fetchTheme(themePath) {
    let theme = await _fetchJSONAssets(themePath);
    const shikiTheme = toShikiTheme(theme);
    if (shikiTheme.include) {
        const includedTheme = await fetchTheme(join(dirname(themePath), shikiTheme.include));
        if (includedTheme.settings) {
            shikiTheme.settings = includedTheme.settings.concat(shikiTheme.settings);
        }
        if (includedTheme.bg && !shikiTheme.bg) {
            shikiTheme.bg = includedTheme.bg;
        }
        if (includedTheme.colors) {
            shikiTheme.colors = Object.assign(Object.assign({}, includedTheme.colors), shikiTheme.colors);
        }
        delete shikiTheme.include;
    }
    return shikiTheme;
}
async function fetchGrammar(filepath) {
    return await _fetchJSONAssets(filepath);
}
function repairTheme(theme) {
    // Has the default no-scope setting with fallback colors
    if (!theme.settings)
        theme.settings = [];
    if (theme.settings[0] && theme.settings[0].settings && !theme.settings[0].scope) {
        return;
    }
    // Push a no-scope setting with fallback colors
    theme.settings.unshift({
        settings: {
            foreground: theme.fg,
            background: theme.bg
        }
    });
}
function toShikiTheme(rawTheme) {
    const type = rawTheme.type || 'dark';
    const shikiTheme = Object.assign(Object.assign({ name: rawTheme.name, type }, rawTheme), getThemeDefaultColors(rawTheme));
    if (rawTheme.include) {
        shikiTheme.include = rawTheme.include;
    }
    if (rawTheme.tokenColors) {
        shikiTheme.settings = rawTheme.tokenColors;
        delete shikiTheme.tokenColors;
    }
    repairTheme(shikiTheme);
    return shikiTheme;
}
/**
 * https://github.com/microsoft/vscode/blob/f7f05dee53fb33fe023db2e06e30a89d3094488f/src/vs/platform/theme/common/colorRegistry.ts#L258-L268
 */
const VSCODE_FALLBACK_EDITOR_FG = { light: '#333333', dark: '#bbbbbb' };
const VSCODE_FALLBACK_EDITOR_BG = { light: '#fffffe', dark: '#1e1e1e' };
function getThemeDefaultColors(theme) {
    var _a, _b, _c, _d, _e, _f;
    let fg, bg;
    /**
     * First try:
     * Theme might contain a global `tokenColor` without `name` or `scope`
     * Used as default value for foreground/background
     */
    let settings = theme.settings ? theme.settings : theme.tokenColors;
    const globalSetting = settings
        ? settings.find(s => {
            return !s.name && !s.scope;
        })
        : undefined;
    if ((_a = globalSetting === null || globalSetting === void 0 ? void 0 : globalSetting.settings) === null || _a === void 0 ? void 0 : _a.foreground) {
        fg = globalSetting.settings.foreground;
    }
    if ((_b = globalSetting === null || globalSetting === void 0 ? void 0 : globalSetting.settings) === null || _b === void 0 ? void 0 : _b.background) {
        bg = globalSetting.settings.background;
    }
    /**
     * Second try:
     * If there's no global `tokenColor` without `name` or `scope`
     * Use `editor.foreground` and `editor.background`
     */
    if (!fg && ((_d = (_c = theme) === null || _c === void 0 ? void 0 : _c.colors) === null || _d === void 0 ? void 0 : _d['editor.foreground'])) {
        fg = theme.colors['editor.foreground'];
    }
    if (!bg && ((_f = (_e = theme) === null || _e === void 0 ? void 0 : _e.colors) === null || _f === void 0 ? void 0 : _f['editor.background'])) {
        bg = theme.colors['editor.background'];
    }
    /**
     * Last try:
     * If there's no fg/bg color specified in theme, use default
     */
    if (!fg) {
        fg = theme.type === 'light' ? VSCODE_FALLBACK_EDITOR_FG.light : VSCODE_FALLBACK_EDITOR_FG.dark;
    }
    if (!bg) {
        bg = theme.type === 'light' ? VSCODE_FALLBACK_EDITOR_BG.light : VSCODE_FALLBACK_EDITOR_BG.dark;
    }
    return {
        fg,
        bg
    };
}

/*---------------------------------------------------------
 * Copyright (C) Microsoft Corporation. All rights reserved.
 *--------------------------------------------------------*/
class Resolver {
    constructor(onigLibPromise, onigLibName) {
        this.languagesPath = 'languages/';
        this.languageMap = {};
        this.scopeToLangMap = {};
        this._onigLibPromise = onigLibPromise;
        this._onigLibName = onigLibName;
    }
    get onigLib() {
        return this._onigLibPromise;
    }
    getOnigLibName() {
        return this._onigLibName;
    }
    getLangRegistration(langIdOrAlias) {
        return this.languageMap[langIdOrAlias];
    }
    async loadGrammar(scopeName) {
        const lang = this.scopeToLangMap[scopeName];
        if (!lang) {
            return null;
        }
        if (lang.grammar) {
            return lang.grammar;
        }
        const g = await fetchGrammar(languages.includes(lang) ? `${this.languagesPath}${lang.path}` : lang.path);
        lang.grammar = g;
        return g;
    }
    addLanguage(l) {
        this.languageMap[l.id] = l;
        if (l.aliases) {
            l.aliases.forEach(a => {
                this.languageMap[a] = l;
            });
        }
        this.scopeToLangMap[l.scopeName] = l;
    }
}

/*---------------------------------------------------------
 * Copyright (C) Microsoft Corporation. All rights reserved.
 *--------------------------------------------------------*/
function tokenizeWithTheme(theme, colorMap, fileContents, grammar, options) {
    let lines = fileContents.split(/\r\n|\r|\n/);
    let ruleStack = vscodeTextmate.INITIAL;
    let actual = [];
    let final = [];
    for (let i = 0, len = lines.length; i < len; i++) {
        let line = lines[i];
        if (line === '') {
            actual = [];
            final.push([]);
            continue;
        }
        let resultWithScopes;
        let tokensWithScopes;
        let tokensWithScopesIndex;
        if (options.includeExplanation) {
            resultWithScopes = grammar.tokenizeLine(line, ruleStack);
            tokensWithScopes = resultWithScopes.tokens;
            tokensWithScopesIndex = 0;
        }
        let result = grammar.tokenizeLine2(line, ruleStack);
        let tokensLength = result.tokens.length / 2;
        for (let j = 0; j < tokensLength; j++) {
            let startIndex = result.tokens[2 * j];
            let nextStartIndex = j + 1 < tokensLength ? result.tokens[2 * j + 2] : line.length;
            if (startIndex === nextStartIndex) {
                continue;
            }
            let metadata = result.tokens[2 * j + 1];
            let foreground = StackElementMetadata.getForeground(metadata);
            let foregroundColor = colorMap[foreground];
            let fontStyle = StackElementMetadata.getFontStyle(metadata);
            let explanation = [];
            if (options.includeExplanation) {
                let offset = 0;
                while (startIndex + offset < nextStartIndex) {
                    let tokenWithScopes = tokensWithScopes[tokensWithScopesIndex];
                    let tokenWithScopesText = line.substring(tokenWithScopes.startIndex, tokenWithScopes.endIndex);
                    offset += tokenWithScopesText.length;
                    explanation.push({
                        content: tokenWithScopesText,
                        scopes: explainThemeScopes(theme, tokenWithScopes.scopes)
                    });
                    tokensWithScopesIndex++;
                }
            }
            actual.push({
                content: line.substring(startIndex, nextStartIndex),
                color: foregroundColor,
                fontStyle,
                explanation: explanation
            });
        }
        final.push(actual);
        actual = [];
        ruleStack = result.ruleStack;
    }
    return final;
}
function explainThemeScopes(theme, scopes) {
    let result = [];
    for (let i = 0, len = scopes.length; i < len; i++) {
        let parentScopes = scopes.slice(0, i);
        let scope = scopes[i];
        result[i] = {
            scopeName: scope,
            themeMatches: explainThemeScope(theme, scope, parentScopes)
        };
    }
    return result;
}
function matchesOne(selector, scope) {
    let selectorPrefix = selector + '.';
    if (selector === scope || scope.substring(0, selectorPrefix.length) === selectorPrefix) {
        return true;
    }
    return false;
}
function matches(selector, selectorParentScopes, scope, parentScopes) {
    if (!matchesOne(selector, scope)) {
        return false;
    }
    let selectorParentIndex = selectorParentScopes.length - 1;
    let parentIndex = parentScopes.length - 1;
    while (selectorParentIndex >= 0 && parentIndex >= 0) {
        if (matchesOne(selectorParentScopes[selectorParentIndex], parentScopes[parentIndex])) {
            selectorParentIndex--;
        }
        parentIndex--;
    }
    if (selectorParentIndex === -1) {
        return true;
    }
    return false;
}
function explainThemeScope(theme, scope, parentScopes) {
    let result = [], resultLen = 0;
    for (let i = 0, len = theme.settings.length; i < len; i++) {
        let setting = theme.settings[i];
        let selectors;
        if (typeof setting.scope === 'string') {
            selectors = setting.scope.split(/,/).map(scope => scope.trim());
        }
        else if (Array.isArray(setting.scope)) {
            selectors = setting.scope;
        }
        else {
            continue;
        }
        for (let j = 0, lenJ = selectors.length; j < lenJ; j++) {
            let rawSelector = selectors[j];
            let rawSelectorPieces = rawSelector.split(/ /);
            let selector = rawSelectorPieces[rawSelectorPieces.length - 1];
            let selectorParentScopes = rawSelectorPieces.slice(0, rawSelectorPieces.length - 1);
            if (matches(selector, selectorParentScopes, scope, parentScopes)) {
                // match!
                result[resultLen++] = setting;
                // break the loop
                j = lenJ;
            }
        }
    }
    return result;
}

function renderToHtml(lines, options = {}) {
    var _a;
    const bg = options.bg || '#fff';
    const optionsByLineNumber = groupBy((_a = options.lineOptions) !== null && _a !== void 0 ? _a : [], option => option.line);
    let html = '';
    html += `<pre class="shiki" style="background-color: ${bg}">`;
    if (options.langId) {
        html += `<div class="language-id">${options.langId}</div>`;
    }
    html += `<code>`;
    lines.forEach((l, lineIndex) => {
        var _a;
        const lineNumber = lineIndex + 1;
        const lineOptions = (_a = optionsByLineNumber.get(lineNumber)) !== null && _a !== void 0 ? _a : [];
        const lineClasses = getLineClasses(lineOptions).join(' ');
        html += `<span class="${lineClasses}">`;
        l.forEach(token => {
            const cssDeclarations = [`color: ${token.color || options.fg}`];
            if (token.fontStyle & exports.FontStyle.Italic) {
                cssDeclarations.push('font-style: italic');
            }
            if (token.fontStyle & exports.FontStyle.Bold) {
                cssDeclarations.push('font-weight: bold');
            }
            if (token.fontStyle & exports.FontStyle.Underline) {
                cssDeclarations.push('text-decoration: underline');
            }
            html += `<span style="${cssDeclarations.join('; ')}">${escapeHtml(token.content)}</span>`;
        });
        html += `</span>\n`;
    });
    html = html.replace(/\n*$/, ''); // Get rid of final new lines
    html += `</code></pre>`;
    return html;
}
const htmlEscapes = {
    '&': '&amp;',
    '<': '&lt;',
    '>': '&gt;',
    '"': '&quot;',
    "'": '&#39;'
};
function escapeHtml(html) {
    return html.replace(/[&<>"']/g, chr => htmlEscapes[chr]);
}
function getLineClasses(lineOptions) {
    var _a;
    const lineClasses = new Set(['line']);
    for (const lineOption of lineOptions) {
        for (const lineClass of (_a = lineOption.classes) !== null && _a !== void 0 ? _a : []) {
            lineClasses.add(lineClass);
        }
    }
    return Array.from(lineClasses);
}

class Registry extends vscodeTextmate.Registry {
    constructor(_resolver) {
        super(_resolver);
        this._resolver = _resolver;
        this.themesPath = 'themes/';
        this._resolvedThemes = {};
        this._resolvedGrammars = {};
    }
    getTheme(theme) {
        if (typeof theme === 'string') {
            return this._resolvedThemes[theme];
        }
        else {
            return theme;
        }
    }
    async loadTheme(theme) {
        if (typeof theme === 'string') {
            if (!this._resolvedThemes[theme]) {
                this._resolvedThemes[theme] = await fetchTheme(`${this.themesPath}${theme}.json`);
            }
            return this._resolvedThemes[theme];
        }
        else {
            theme = toShikiTheme(theme);
            if (theme.name) {
                this._resolvedThemes[theme.name] = theme;
            }
            return theme;
        }
    }
    async loadThemes(themes) {
        return await Promise.all(themes.map(theme => this.loadTheme(theme)));
    }
    getLoadedThemes() {
        return Object.keys(this._resolvedThemes);
    }
    getGrammar(name) {
        return this._resolvedGrammars[name];
    }
    async loadLanguage(lang) {
        const g = await this.loadGrammar(lang.scopeName);
        this._resolvedGrammars[lang.id] = g;
        if (lang.aliases) {
            lang.aliases.forEach(la => {
                this._resolvedGrammars[la] = g;
            });
        }
    }
    async loadLanguages(langs) {
        for (const lang of langs) {
            this._resolver.addLanguage(lang);
        }
        for (const lang of langs) {
            await this.loadLanguage(lang);
        }
    }
    getLoadedLanguages() {
        return Object.keys(this._resolvedGrammars);
    }
}

function resolveLang(lang) {
    return typeof lang === 'string'
        ? languages.find(l => { var _a; return l.id === lang || ((_a = l.aliases) === null || _a === void 0 ? void 0 : _a.includes(lang)); })
        : lang;
}
function resolveOptions(options) {
    var _a;
    let _languages = languages;
    let _themes = options.themes || [];
    if ((_a = options.langs) === null || _a === void 0 ? void 0 : _a.length) {
        _languages = options.langs.map(resolveLang);
    }
    if (options.theme) {
        _themes.unshift(options.theme);
    }
    if (!_themes.length) {
        _themes = ['nord'];
    }
    return { _languages, _themes };
}
async function getHighlighter(options) {
    var _a, _b;
    const { _languages, _themes } = resolveOptions(options);
    const _resolver = new Resolver(getOniguruma(), 'vscode-oniguruma');
    const _registry = new Registry(_resolver);
    if ((_a = options.paths) === null || _a === void 0 ? void 0 : _a.themes) {
        _registry.themesPath = options.paths.themes;
    }
    if ((_b = options.paths) === null || _b === void 0 ? void 0 : _b.languages) {
        _resolver.languagesPath = options.paths.languages;
    }
    const themes = await _registry.loadThemes(_themes);
    const _defaultTheme = themes[0];
    let _currentTheme;
    await _registry.loadLanguages(_languages);
    /**
     * Shiki was designed for VS Code, so CSS variables are not currently supported.
     * See: https://github.com/shikijs/shiki/pull/212#issuecomment-906924986
     *
     * Instead, we work around this by using valid hex color codes as lookups in a
     * final "repair" step which translates those codes to the correct CSS variables.
     */
    const COLOR_REPLACEMENTS = {
        '#000001': 'var(--shiki-color-text)',
        '#000002': 'var(--shiki-color-background)',
        '#000004': 'var(--shiki-token-constant)',
        '#000005': 'var(--shiki-token-string)',
        '#000006': 'var(--shiki-token-comment)',
        '#000007': 'var(--shiki-token-keyword)',
        '#000008': 'var(--shiki-token-parameter)',
        '#000009': 'var(--shiki-token-function)',
        '#000010': 'var(--shiki-token-string-expression)',
        '#000011': 'var(--shiki-token-punctuation)',
        '#000012': 'var(--shiki-token-link)'
    };
    function fixCssVariablesTheme(theme, colorMap) {
        theme.bg = COLOR_REPLACEMENTS[theme.bg] || theme.bg;
        theme.fg = COLOR_REPLACEMENTS[theme.fg] || theme.fg;
        colorMap.forEach((val, i) => {
            colorMap[i] = COLOR_REPLACEMENTS[val] || val;
        });
    }
    function getTheme(theme) {
        const _theme = theme ? _registry.getTheme(theme) : _defaultTheme;
        if (!_theme) {
            throw Error(`No theme registration for ${theme}`);
        }
        if (!_currentTheme || _currentTheme.name !== _theme.name) {
            _registry.setTheme(_theme);
            _currentTheme = _theme;
        }
        const _colorMap = _registry.getColorMap();
        if (_theme.name === 'css-variables') {
            fixCssVariablesTheme(_theme, _colorMap);
        }
        return { _theme, _colorMap };
    }
    function getGrammar(lang) {
        const _grammar = _registry.getGrammar(lang);
        if (!_grammar) {
            throw Error(`No language registration for ${lang}`);
        }
        return { _grammar };
    }
    function codeToThemedTokens(code, lang = 'text', theme, options = { includeExplanation: true }) {
        if (isPlaintext(lang)) {
            const lines = code.split(/\r\n|\r|\n/);
            return [...lines.map(line => [{ content: line }])];
        }
        const { _grammar } = getGrammar(lang);
        const { _theme, _colorMap } = getTheme(theme);
        return tokenizeWithTheme(_theme, _colorMap, code, _grammar, options);
    }
    function codeToHtml(code, arg1 = 'text', arg2) {
        let options;
        // codeToHtml(code, options?) overload
        if (typeof arg1 === 'object') {
            options = arg1;
        }
        // codeToHtml(code, lang?, theme?) overload
        else {
            options = {
                lang: arg1,
                theme: arg2
            };
        }
        const tokens = codeToThemedTokens(code, options.lang, options.theme, {
            includeExplanation: false
        });
        const { _theme } = getTheme(options.theme);
        return renderToHtml(tokens, {
            fg: _theme.fg,
            bg: _theme.bg,
            lineOptions: options === null || options === void 0 ? void 0 : options.lineOptions
        });
    }
    async function loadTheme(theme) {
        await _registry.loadTheme(theme);
    }
    async function loadLanguage(lang) {
        const _lang = resolveLang(lang);
        _resolver.addLanguage(_lang);
        await _registry.loadLanguage(_lang);
    }
    function getLoadedThemes() {
        return _registry.getLoadedThemes();
    }
    function getLoadedLanguages() {
        return _registry.getLoadedLanguages();
    }
    function getBackgroundColor(theme) {
        const { _theme } = getTheme(theme);
        return _theme.bg;
    }
    function getForegroundColor(theme) {
        const { _theme } = getTheme(theme);
        return _theme.fg;
    }
    return {
        codeToThemedTokens,
        codeToHtml,
        getTheme: (theme) => {
            return getTheme(theme)._theme;
        },
        loadTheme,
        loadLanguage,
        getBackgroundColor,
        getForegroundColor,
        getLoadedThemes,
        getLoadedLanguages
    };
}
function isPlaintext(lang) {
    return !lang || ['plaintext', 'txt', 'text'].includes(lang);
}

/** @deprecated use setWasm instead, will be removed in a future version */
function setOnigasmWASM(path) {
    setWasm(path);
}

exports.BUNDLED_LANGUAGES = languages;
exports.BUNDLED_THEMES = themes;
exports.getHighlighter = getHighlighter;
exports.loadTheme = fetchTheme;
exports.renderToHtml = renderToHtml;
exports.setCDN = setCDN;
exports.setOnigasmWASM = setOnigasmWASM;
exports.setWasm = setWasm;
exports.toShikiTheme = toShikiTheme;

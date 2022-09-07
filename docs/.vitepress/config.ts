import { defineConfig } from 'vitepress'

export default defineConfig({
    lang: 'en-US',
    title: 'Catch2',
    titleTemplate: 'Catch2', // otherwise false
    description: 'Catch2, c++ / cpp unit test open source project.',
    appearance: true, // dark mode enabled
    base: '/Catch2/',
    lastUpdated: true,
    // markdown
    // https://vitepress.vuejs.org/config/app-configs.html#markdown
    markdown: {
        theme: 'material-palenight',
        lineNumbers: true
    },

    themeConfig: {
        nav: [
            { text: 'Contribute', link: 'https://github.com/catchorg/Catch2'}
        ],
        socialLinks: [
            { icon: 'github', link: 'https://github.com/catchorg/Catch2' },
            //{ icon: 'linkedin', link: 'https://www.linkedin.com/in/ryankert01/'}
        ],
        sidebar: [
            {
              text: 'Guide',
              items: [
                { text: 'Introduction', link: '/intro' },
                { text: 'Getting Started', link: '/tutorial' }
              ]
            },
            {
                text: 'Writing tests',
                items: [
                    { text: 'Assertion macros', link: '/assertions' },
                    { text: 'Matchers', link: '/matchers' },
                    { text: 'Logging macros', link: '/logging' },
                    { text: 'Test cases and sections', link: '/test-cases-and-sections' },
                    { text: 'Test fixtures', link: '/test-fixtures' },
                    { text: 'Reporters', link: '/reporters' },
                    { text: 'Event Listeners', link: '/event-listeners' },
                    { text: 'Data Generators', link: '/generators' },
                    { text: 'Other macros', link: '/other-macros' },
                    { text: 'Micro benchmarking', link: '/benchmarks' }
                ]
            },
            {
                text: 'Fine tuning',
                items: [
                    { text: 'Supplying your own main()', link: '/own-main' },
                    { text: 'Compile-time configuration', link: '/configuration' },
                    { text: 'String Conversions', link: '/tostring' },
                ]
            },
            {
                text: 'Running Command Line',
                items: [
                    { text: 'Command Line', link: '/command-line' },
                ]
            },
            {
                text: 'Odds and ends',
                items: [
                    { text: 'Frequently Asked Questions (FAQ)', link: '/faq' },
                    { text: 'Best practices and other tips', link: '/usage-tips' },
                    { text: 'CMake integration', link: '/cmake-integration' },
                    { text: 'CI and other miscellaneous pieces', link: '/ci-and-misc' },
                    { text: 'Known limitations', link: '/limitations' }

                ]
            },
            {
                text: 'Other',
                items: [
                    { text: 'Why Catch2?', link: '/why-catch.md' },
                    { text: 'Migrating from v2 to v3', link: '/migrate-v2-to-v3' },
                    { text: 'Open Source Projects using Catch2', link: '/opensource-users' },
                    { text: 'Commercial Projects using Catch2', link: '/commercial-users' },
                    { text: 'Contributing', link: '/contributing' },
                    { text: 'Release Notes', link: '/release-notes' },
                    { text: 'Deprecations and incoming changes', link: '/deprecations' },
                ]
            },
        ]
    }
});


<script setup lang="ts">
import type { DefaultTheme } from 'vitepress/theme'
import docsearch from '@docsearch/js'
import { onMounted } from 'vue'
import { useRouter, useRoute, useData } from 'vitepress'

const router = useRouter()
const route = useRoute()
const { theme } = useData()

onMounted(() => {
  initialize(theme.value.algolia)
  setTimeout(poll, 16)
})

function poll() {
  // programmatically open the search box after initialize
  const e = new Event('keydown') as any

  e.key = 'k'
  e.metaKey = true

  window.dispatchEvent(e)

  setTimeout(() => {
    if (!document.querySelector('.DocSearch-Modal')) {
      poll()
    }
  }, 16)
}

function initialize(userOptions: DefaultTheme.AlgoliaSearchOptions) {
  // note: multi-lang search support is removed since the theme
  // doesn't support multiple locales as of now.
  const options = Object.assign({}, userOptions, {
    container: '#docsearch',

    navigator: {
      navigate({ itemUrl }: { itemUrl: string }) {
        const { pathname: hitPathname } = new URL(
          window.location.origin + itemUrl
        )

        // router doesn't handle same-page navigation so we use the native
        // browser location API for anchor navigation
        if (route.path === hitPathname) {
          window.location.assign(window.location.origin + itemUrl)
        } else {
          router.go(itemUrl)
        }
      }
    },

    transformItems(items: any[]) {
      return items.map((item) => {
        return Object.assign({}, item, {
          url: getRelativePath(item.url)
        })
      })
    },

    hitComponent({ hit, children }: { hit: any; children: any }) {
      const relativeHit = hit.url.startsWith('http')
        ? getRelativePath(hit.url as string)
        : hit.url

      return {
        __v: null,
        type: 'a',
        ref: undefined,
        constructor: undefined,
        key: undefined,

        props: {
          href: hit.url,

          onClick(event: MouseEvent) {
            if (isSpecialClick(event)) {
              return
            }

            // we rely on the native link scrolling when user is already on
            // the right anchor because Router doesn't support duplicated
            // history entries.
            if (route.path === relativeHit) {
              return
            }

            // if the hits goes to another page, we prevent the native link
            // behavior to leverage the Router loading feature.
            if (route.path !== relativeHit) {
              event.preventDefault()
            }

            router.go(relativeHit)
          },

          children
        }
      }
    }
  })

  docsearch(options)
}

function isSpecialClick(event: MouseEvent) {
  return (
    event.button === 1 ||
    event.altKey ||
    event.ctrlKey ||
    event.metaKey ||
    event.shiftKey
  )
}

function getRelativePath(absoluteUrl: string) {
  const { pathname, hash } = new URL(absoluteUrl)

  return pathname + hash
}
</script>

<template>
  <div id="docsearch" />
</template>

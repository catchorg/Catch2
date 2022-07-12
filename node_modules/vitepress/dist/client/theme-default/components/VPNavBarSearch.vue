<script lang="ts" setup>
import '@docsearch/css'
import { defineAsyncComponent, ref, onMounted, onUnmounted } from 'vue'
import { useData } from 'vitepress'

const VPAlgoliaSearchBox = __ALGOLIA__
  ? defineAsyncComponent(() => import('./VPAlgoliaSearchBox.vue'))
  : () => null

const { theme } = useData()

// to avoid loading the docsearch js upfront (which is more than 1/3 of the
// payload), we delay initializing it until the user has actually clicked or
// hit the hotkey to invoke it.
const loaded = ref(false)

const metaKey = ref()

onMounted(() => {
  if (!theme.value.algolia) {
    return
  }

  // meta key detect (same logic as in @docsearch/js)
  metaKey.value.textContent = /(Mac|iPhone|iPod|iPad)/i.test(navigator.platform)
    ? '⌘'
    : 'Ctrl'

  const handleSearchHotKey = (e: KeyboardEvent) => {
    if (e.key === 'k' && (e.ctrlKey || e.metaKey)) {
      e.preventDefault()
      load()
      remove()
    }
  }

  const remove = () => {
    window.removeEventListener('keydown', handleSearchHotKey)
  }

  window.addEventListener('keydown', handleSearchHotKey)

  onUnmounted(remove)
})

function load() {
  if (!loaded.value) {
    loaded.value = true
  }
}
</script>

<template>
  <div v-if="theme.algolia" class="VPNavBarSearch">
    <VPAlgoliaSearchBox v-if="loaded" />

    <div v-else id="docsearch" @click="load">
      <button
        type="button"
        class="DocSearch DocSearch-Button"
        aria-label="Search"
      >
        <span class="DocSearch-Button-Container">
          <svg
            class="DocSearch-Search-Icon"
            width="20"
            height="20"
            viewBox="0 0 20 20"
          >
            <path
              d="M14.386 14.386l4.0877 4.0877-4.0877-4.0877c-2.9418 2.9419-7.7115 2.9419-10.6533 0-2.9419-2.9418-2.9419-7.7115 0-10.6533 2.9418-2.9419 7.7115-2.9419 10.6533 0 2.9419 2.9418 2.9419 7.7115 0 10.6533z"
              stroke="currentColor"
              fill="none"
              fill-rule="evenodd"
              stroke-linecap="round"
              stroke-linejoin="round"
            />
          </svg>
          <span class="DocSearch-Button-Placeholder">Search</span>
        </span>
        <span class="DocSearch-Button-Keys">
          <kbd class="DocSearch-Button-Key" ref="metaKey">Meta</kbd>
          <kbd class="DocSearch-Button-Key">K</kbd>
        </span>
      </button>
    </div>
  </div>
</template>

<style>
.VPNavBarSearch {
  display: flex;
  align-items: center;
}

@media (min-width: 768px) {
  .VPNavBarSearch {
    flex-grow: 1;
    padding-left: 24px;
  }
}

@media (min-width: 960px) {
  .VPNavBarSearch {
    padding-left: 32px;
  }
}

.DocSearch {
  --docsearch-primary-color: var(--vp-c-brand);
  --docsearch-highlight-color: var(--docsearch-primary-color);
  --docsearch-text-color: var(--vp-c-text-1);
  --docsearch-muted-color: var(--vp-c-text-2);
  --docsearch-searchbox-shadow: none;
  --docsearch-searchbox-focus-background: transparent;
  --docsearch-key-gradient: transparent;
  --docsearch-key-shadow: none;
  --docsearch-modal-background: var(--vp-c-bg-soft);
  --docsearch-footer-background: var(--vp-c-bg);
}

.dark .DocSearch {
  --docsearch-modal-shadow: none;
  --docsearch-footer-shadow: none;
  --docsearch-logo-color: var(--vp-c-text-2);
  --docsearch-hit-background: var(--vp-c-bg-mute);
  --docsearch-hit-color: var(--vp-c-text-2);
  --docsearch-hit-shadow: none;
}

.DocSearch-Button {
  display: flex;
  justify-content: center;
  align-items: center;
  margin: 0;
  padding: 0;
  width: 32px;
  height: 55px;
  background: transparent;
  transition: border-color 0.25s;
}

.DocSearch-Button:hover {
  background: transparent;
}

.DocSearch-Button:focus {
  outline: 1px dotted;
  outline: 5px auto -webkit-focus-ring-color;
}

.DocSearch-Button:focus:not(:focus-visible) {
  outline: none !important;
}

@media (min-width: 768px) {
  .DocSearch-Button {
    justify-content: flex-start;
    border: 1px solid transparent;
    border-radius: 8px;
    padding: 0 10px 0 12px;
    width: 100%;
    height: 40px;
    background-color: var(--vp-c-bg-alt);
  }

  .DocSearch-Button:hover {
    border-color: var(--vp-c-brand);
    background: var(--vp-c-bg-alt);
  }
}

.DocSearch-Button .DocSearch-Button-Container {
  display: flex;
  align-items: center;
}

.DocSearch-Button .DocSearch-Search-Icon {
  position: relative;
  width: 16px;
  height: 16px;
  color: var(--vp-c-text-1);
  fill: currentColor;
  transition: color 0.5s;
}

.DocSearch-Button:hover .DocSearch-Search-Icon {
  color: var(--vp-c-text-1);
}

@media (min-width: 768px) {
  .DocSearch-Button .DocSearch-Search-Icon {
    top: 1px;
    margin-right: 8px;
    width: 14px;
    height: 14px;
    color: var(--vp-c-text-2);
  }
}

.DocSearch-Button .DocSearch-Button-Placeholder {
  display: none;
  margin-top: 2px;
  padding: 0 16px 0 0;
  font-size: 13px;
  font-weight: 500;
  color: var(--vp-c-text-2);
  transition: color 0.5s;
}

.DocSearch-Button:hover .DocSearch-Button-Placeholder {
  color: var(--vp-c-text-1);
}

@media (min-width: 768px) {
  .DocSearch-Button .DocSearch-Button-Placeholder {
    display: inline-block;
  }
}

.DocSearch-Button .DocSearch-Button-Keys {
  display: none;
  min-width: auto;
}

@media (min-width: 768px) {
  .DocSearch-Button .DocSearch-Button-Keys {
    display: flex;
    align-items: center;
  }
}

.DocSearch-Button .DocSearch-Button-Key {
  display: block;
  margin: 2px 0 0 0;
  border: 1px solid var(--vp-c-divider);
  border-right: none;
  border-radius: 4px 0 0 4px;
  padding-left: 6px;
  min-width: 0;
  width: auto;
  height: 22px;
  line-height: 22px;
  font-family: var(--vp-font-family-base);
  font-size: 12px;
  font-weight: 500;
  transition: color 0.5s, border-color 0.5s;
}

.DocSearch-Button .DocSearch-Button-Key + .DocSearch-Button-Key {
  border-right: 1px solid var(--vp-c-divider);
  border-left: none;
  border-radius: 0 4px 4px 0;
  padding-left: 2px;
  padding-right: 6px;
}

.dark .DocSearch-Footer {
  border-top: 1px solid var(--vp-c-divider);
}

.DocSearch-Form {
  border: 1px solid var(--vp-c-brand);
  background-color: var(--vp-c-white);
}

.dark .DocSearch-Form {
  background-color: var(--vp-c-bg-mute);
}
</style>

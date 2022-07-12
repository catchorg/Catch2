<script setup lang="ts">
import { provide } from 'vue'
import { useNav } from '../composables/nav'
import { useSidebar } from '../composables/sidebar'
import VPNavBar from './VPNavBar.vue'
import VPNavScreen from './VPNavScreen.vue'

const { isScreenOpen, closeScreen, toggleScreen } = useNav()
const { hasSidebar } = useSidebar()

provide('close-screen', closeScreen)
</script>

<template>
  <header class="VPNav" :class="{ 'no-sidebar' : !hasSidebar }">
    <VPNavBar :is-screen-open="isScreenOpen" @toggle-screen="toggleScreen" />
    <VPNavScreen :open="isScreenOpen" />
  </header>
</template>

<style scoped>
.VPNav {
  position: relative;
  top: 0;
  left: 0;
  z-index: var(--vp-z-index-nav);
  width: 100%;
}

@media (min-width: 960px) {
  .VPNav {
    position: fixed;
  }

  .VPNav.no-sidebar {
    -webkit-backdrop-filter: saturate(50%) blur(8px);
    backdrop-filter: saturate(50%) blur(8px);
    background: rgba(255, 255, 255, 0.7);
  }

  .dark .VPNav.no-sidebar {
    background: rgba(36, 36, 36, 0.7);
  }

  @supports not (backdrop-filter: saturate(50%) blur(8px)) {
    .VPNav.no-sidebar {
      background: rgba(255, 255, 255, 0.95);
    }

    .dark .VPNav.no-sidebar {
      background: rgba(36, 36, 36, 0.95);
    }
  }
}
</style>

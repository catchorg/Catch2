<script lang="ts" setup>
import { ref, watchPostEffect, nextTick } from 'vue'
import { useSidebar } from '../composables/sidebar'
import VPSidebarGroup from './VPSidebarGroup.vue'

const { sidebar, hasSidebar } = useSidebar()

const props = defineProps<{
  open: boolean
}>()

// a11y: focus Nav element when menu has opened
let navEl = ref<(Element & { focus(): void }) | null>(null)

watchPostEffect(async () => {
  if (props.open) {
    await nextTick()
    navEl.value?.focus()
  }
})
</script>

<template>
  <aside
    v-if="hasSidebar"
    class="VPSidebar"
    :class="{ open }"
    ref="navEl"
    @click.stop
  >
    <nav class="nav" id="VPSidebarNav" aria-labelledby="sidebar-aria-label" tabindex="-1">
      <span class="visually-hidden" id="sidebar-aria-label">
        Sidebar Navigation
      </span>

      <div v-for="group in sidebar" :key="group.text" class="group">
        <VPSidebarGroup
          :text="group.text"
          :items="group.items"
          :collapsible="group.collapsible"
          :collapsed="group.collapsed"
        />
      </div>
    </nav>
  </aside>
</template>

<style scoped>
.VPSidebar {
  position: fixed;
  top: 0;
  bottom: 0;
  left: 0;
  z-index: var(--vp-z-index-sidebar);
  padding: 32px 32px 96px;
  width: calc(100vw - 64px);
  max-width: 320px;
  background-color: var(--vp-c-bg);
  opacity: 0;
  box-shadow: var(--vp-c-shadow-3);
  overflow-x: hidden;
  overflow-y: auto;
  transform: translateX(-100%);
  transition: opacity 0.5s, transform 0.25s ease;
}

.VPSidebar.open {
  opacity: 1;
  visibility: visible;
  transform: translateX(0);
  transition: opacity 0.25s,
              transform 0.5s cubic-bezier(0.19, 1, 0.22, 1);
}

.dark .VPSidebar {
  box-shadow: var(--vp-shadow-1);
}

@media (min-width: 960px) {
  .VPSidebar {
    z-index: 1;
    padding-top: var(--vp-nav-height-desktop);
    padding-bottom: 128px;
    width: var(--vp-sidebar-width);
    max-width: 100%;
    width: var(--vp-sidebar-width);
    background-color: var(--vp-c-bg-alt);
    opacity: 1;
    visibility: visible;
    box-shadow: none;
    transform: translateX(0);
  }
}

@media (min-width: 1440px) {
  .VPSidebar {
    padding-left: calc((100% - (var(--vp-layout-max-width) - 64px)) / 2);
    width: calc((100% - (var(--vp-layout-max-width) - 64px)) / 2 + var(--vp-sidebar-width) - 32px);
  }
}

.nav {
  outline: 0;
}

.group + .group {
  margin-top: 32px;
  border-top: 1px solid var(--vp-c-divider-light);
  padding-top: 10px;
}

@media (min-width: 960px) {
  .group {
    padding-top: 10px;
    width: calc(var(--vp-sidebar-width) - 64px);
  }

  .group + .group {
    margin-top: 24px;
  }
}
</style>

<script lang="ts" setup>
import { useSidebar } from '../composables/sidebar'
import VPIconAlignLeft from './icons/VPIconAlignLeft.vue'

defineProps<{
  open: boolean
}>()

defineEmits<{
  (e: 'open-menu'): void
}>()

const { hasSidebar } = useSidebar()

function scrollToTop() {
  window.scrollTo({ top: 0, left: 0, behavior: 'smooth' })
}
</script>

<template>
  <div v-if="hasSidebar" class="VPLocalNav">
    <button
      class="menu"
      :aria-expanded="open"
      aria-controls="VPSidebarNav"
      @click="$emit('open-menu')"
    >
      <VPIconAlignLeft class="menu-icon" />
      <span class="menu-text">Menu</span>
    </button>

    <a class="top-link" href="#" @click="scrollToTop">
      Return to top
    </a>
  </div>
</template>

<style scoped>
.VPLocalNav {
  position: sticky;
  top: 0;
  left: 0;
  z-index: var(--vp-z-index-local-nav);
  display: flex;
  justify-content: space-between;
  align-items: center;
  border-bottom: 1px solid var(--vp-c-divider-light);
  width: 100%;
  background-color: var(--vp-c-bg);
  transition: border-color 0.5s, background-color 0.5s;
}

@media (min-width: 960px) {
  .VPLocalNav {
    display: none;
  }
}

.menu {
  display: flex;
  align-items: center;
  padding: 12px 24px 11px;
  line-height: 24px;
  font-size: 12px;
  font-weight: 500;
  color: var(--vp-c-text-2);
  transition: color 0.5s;
}

.menu:hover {
  color: var(--vp-c-text-1);
  transition: color 0.25s;
}

@media (min-width: 768px) {
  .menu {
    padding: 0 32px;
  }
}

.menu-icon {
  margin-right: 8px;
  width: 16px;
  height: 16px;
  fill: currentColor;
}

.top-link {
  display: block;
  padding: 12px 24px 11px;
  line-height: 24px;
  font-size: 12px;
  font-weight: 500;
  color: var(--vp-c-text-2);
  transition: color 0.5s;
}

.top-link:hover {
  color: var(--vp-c-text-1);
  transition: color 0.25s;
}

@media (min-width: 768px) {
  .top-link {
    padding: 12px 32px 11px;
  }
}
</style>

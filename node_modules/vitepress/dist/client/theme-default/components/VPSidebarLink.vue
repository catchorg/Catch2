<script lang="ts" setup>
import type { DefaultTheme } from 'vitepress/theme'
import { inject } from 'vue'
import { useData } from 'vitepress'
import { isActive } from '../support/utils'
import VPLink from './VPLink.vue'

defineProps<{
  item: DefaultTheme.SidebarItem
}>()

const { page } = useData()

const closeSideBar = inject('close-sidebar') as () => void
</script>

<template>
  <VPLink
    :class="{ active: isActive(page.relativePath, item.link) }"
    :href="item.link"
    @click="closeSideBar"
  >
    <span class="link-text">{{ item.text }}</span>
  </VPLink>
</template>

<style scoped>
.link {
  display: block;
  padding: 4px 0;
  color: var(--vp-c-text-2);
  transition: color 0.5s;
}

.link:hover {
  color: var(--vp-c-text-1);
}

.link.active {
  color: var(--vp-c-brand);
}

.link :deep(.icon) {
  width: 12px;
  height: 12px;
  fill: currentColor;
}

.link-text {
  line-height: 20px;
  font-size: 14px;
  font-weight: 500;
}
</style>

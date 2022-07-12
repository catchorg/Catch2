<script lang="ts" setup>
import type { DefaultTheme } from 'vitepress/theme'
import { ref, watchEffect } from 'vue'
import VPIconPlusSquare from './icons/VPIconPlusSquare.vue'
import VPIconMinusSquare from './icons/VPIconMinusSquare.vue'
import VPSidebarLink from './VPSidebarLink.vue'

const props = defineProps<{
  text?: string
  items: DefaultTheme.SidebarItem[]
  collapsible?: boolean
  collapsed?: boolean
}>()

const collapsed = ref(false)
watchEffect(() => {
  collapsed.value = !!(props.collapsible && props.collapsed)
})

function toggle() {
  if (props.collapsible) {
    collapsed.value = !collapsed.value
  }
}
</script>

<template>
  <section class="VPSidebarGroup" :class="{ collapsible, collapsed }">
    <div
      v-if="text"
      class="title"
      :role="collapsible ? 'button' : undefined"
      @click="toggle"
    >
      <h2 class="title-text">{{ text }}</h2>
      <div class="action">
        <VPIconMinusSquare class="icon minus" />
        <VPIconPlusSquare class="icon plus" />
      </div>
    </div>

    <div class="items">
      <template v-for="item in items" :key="item.link">
        <VPSidebarLink :item="item" />
      </template>
    </div>
  </section>
</template>

<style scoped>
.title {
  display: flex;
  justify-content: space-between;
  align-items: flex-start;
  z-index: 2;
}

.title-text {
  padding-top: 6px;
  padding-bottom: 6px;
  line-height: 20px;
  font-size: 14px;
  font-weight: 700;
  color: var(--vp-c-text-1);
}

.action {
  display: none;
  position: relative;
  margin-right: -8px;
  border-radius: 4px;
  width: 32px;
  height: 32px;
  color: var(--vp-c-text-3);
  transition: color 0.25s;
}

.VPSidebarGroup.collapsible .action {
  display: block;
}

.title:hover .action {
  color: var(--vp-c-text-2);
}

.icon {
  position: absolute;
  top: 8px;
  left: 8px;
  width: 16px;
  height: 16px;
  fill: currentColor;
}

.icon.minus { opacity: 1; }
.icon.plus  { opacity: 0; }

.VPSidebarGroup.collapsed .icon.minus { opacity: 0; }
.VPSidebarGroup.collapsed .icon.plus  { opacity: 1; }

.items {
  overflow: hidden;
}

.VPSidebarGroup.collapsed .items {
  margin-bottom: -22px;
  max-height: 0;
}

@media (min-width: 960px) {
  .VPSidebarGroup.collapsed .items {
    margin-bottom: -14px;
  }
}
</style>

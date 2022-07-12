<script lang="ts" setup>
import { ref } from 'vue'
import { useFlyout } from '../composables/flyout'
import VPIconChevronDown from './icons/VPIconChevronDown.vue'
import VPIconMoreHorizontal from './icons/VPIconMoreHorizontal.vue'
import VPMenu from './VPMenu.vue'

defineProps<{
  icon?: any
  button?: string
  label?: string
  items?: any[]
}>()

const open = ref(false)
const el = ref<HTMLElement>()

useFlyout({ el, onBlur })

function onBlur() {
  open.value = false
}
</script>

<template>
  <div
    class="VPFlyout"
    ref="el"
    @mouseenter="open = true"
    @mouseleave="open = false"
  >
    <button
      type="button"
      class="button"
      aria-haspopup="true"
      :aria-expanded="open"
      :aria-label="label"
      @click="open = !open"
    >
      <span v-if="button || icon" class="text">
        <component v-if="icon" :is="icon" class="option-icon" />
        {{ button }}
        <VPIconChevronDown class="text-icon" />
      </span>

      <VPIconMoreHorizontal v-else class="icon" />
    </button>

    <div class="menu">
      <VPMenu :items="items">
        <slot />
      </VPMenu>
    </div>
  </div>
</template>

<style scoped>
.VPFlyout {
  position: relative;
}

.VPFlyout:hover {
  color: var(--vp-c-bland);
  transition: color 0.25s;
}

.VPFlyout:hover .text {
  color: var(--vp-c-text-2);
}

.VPFlyout:hover .icon {
  fill: var(--vp-c-text-2);
}

.VPFlyout.active .text {
  color: var(--vp-c-brand);
}

.VPFlyout.active:hover .text {
  color: var(--vp-c-brand-dark);
}

.VPFlyout:hover .menu,
.button[aria-expanded="true"] + .menu {
  opacity: 1;
  visibility: visible;
  transform: translateY(0);
}

.button {
  display: flex;
  align-items: center;
  padding: 0 12px;
  height: var(--vp-nav-height-mobile);
  color: var(--vp-c-text-1);
  transition: color 0.5s;
}

@media (min-width: 960px) {
  .button {
    height: var(--vp-nav-height-desktop);
  }
}

.text {
  display: flex;
  align-items: center;
  line-height: var(--vp-nav-height-mobile);
  font-size: 14px;
  font-weight: 500;
  color: var(--vp-c-text-1);
  transition: color 0.25s;
}

@media (min-width: 960px) {
  .text {
    line-height: var(--vp-nav-height-desktop);
  }
}

.option-icon {
  margin-right: 0px;
  width: 16px;
  height: 16px;
  fill: currentColor;
}

.text-icon {
  margin-left: 4px;
  width: 14px;
  height: 14px;
  fill: currentColor;
}

.icon {
  width: 20px;
  height: 20px;
  fill: currentColor;
  transition: fill 0.25s;
}

.menu {
  position: absolute;
  top: calc(var(--vp-nav-height-mobile) / 2 + 20px);
  right: 0;
  opacity: 0;
  visibility: hidden;
  transition: opacity 0.25s, visibility 0.25s, transform 0.25s;
}

@media (min-width: 960px) {
  .menu {
    top: calc(var(--vp-nav-height-desktop) / 2 + 20px);
  }
}
</style>

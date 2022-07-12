<script lang="ts" setup>
import { APPEARANCE_KEY } from '../../shared'
import VPSwitch from './VPSwitch.vue'
import VPIconSun from './icons/VPIconSun.vue'
import VPIconMoon from './icons/VPIconMoon.vue'

const toggle = typeof localStorage !== 'undefined' ? useAppearance() : () => {}

function useAppearance() {
  const query = window.matchMedia('(prefers-color-scheme: dark)')
  const classList = document.documentElement.classList

  let userPreference = localStorage.getItem(APPEARANCE_KEY) || 'auto'

  let isDark = userPreference === 'auto'
    ? query.matches
    : userPreference === 'dark'

  query.onchange = (e) => {
    if (userPreference === 'auto') {
      setClass((isDark = e.matches))
    }
  }

  function toggle() {
    setClass((isDark = !isDark))

    userPreference = isDark
      ? query.matches ? 'auto' : 'dark'
      : query.matches ? 'light' : 'auto'

    localStorage.setItem(APPEARANCE_KEY, userPreference)
  }

  function setClass(dark: boolean): void {
    classList[dark ? 'add' : 'remove']('dark')
  }

  return toggle
}
</script>

<template>
  <VPSwitch
    class="VPSwitchAppearance"
    aria-label="toggle dark mode"
    @click="toggle"
  >
    <VPIconSun class="sun" />
    <VPIconMoon class="moon" />
  </VPSwitch>
</template>

<style scoped>
.sun {
  opacity: 1;
}

.moon {
  opacity: 0;
}

.dark .sun {
  opacity: 0;
}

.dark .moon {
  opacity: 1;
}

.dark .VPSwitchAppearance :deep(.check) {
  transform: translateX(18px);
}
</style>

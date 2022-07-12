<script setup lang="ts">
import { ref, watch, onMounted } from 'vue'
import { useData } from 'vitepress'
import { useAside } from '../composables/aside'

const { theme } = useData()
const carbonOptions = theme.value.carbonAds
const { isAsideEnabled } = useAside()
const container = ref()

let hasInitalized = false

function init() {
  if (!hasInitalized) {
    hasInitalized = true
    const s = document.createElement('script')
    s.id = '_carbonads_js'
    s.src = `//cdn.carbonads.com/carbon.js?serve=${carbonOptions.code}&placement=${carbonOptions.placement}`
    s.async = true
    container.value.appendChild(s)
  }
}

// no need to account for option changes during dev, we can just
// refresh the page
if (carbonOptions) {
  onMounted(() => {
    // if the page is loaded when aside is active, load carbon directly.
    // otherwise, only load it if the page resizes to wide enough. this avoids
    // loading carbon at all on mobile where it's never shown
    if (isAsideEnabled.value) {
      init()
    } else {
      watch(isAsideEnabled, (wide) => wide && init())
    }
  })
}
</script>

<template>
  <div class="VPCarbonAds" ref="container" />
</template>

<style>
.VPCarbonAds {
  display: flex;
  justify-content: center;
  align-items: center;
  padding: 24px;
  border-radius: 12px;
  min-height: 240px;
  text-align: center;
  line-height: 18px;
  font-size: 12px;
  font-weight: 500;
  background-color: var(--vp-c-bg-soft);
  transition: color 0.5s, background-color 0.5s;
}

.VPCarbonAds img {
  margin: 0 auto;
  border-radius: 6px;
}

.VPCarbonAds .carbon-text {
  display: block;
  margin: 0 auto;
  padding-top: 12px;
  color: var(--vp-c-text-1);
  transition: color 0.25s;
}

.VPCarbonAds .carbon-text:hover {
  color: var(--vp-c-brand);
}

.VPCarbonAds .carbon-poweredby {
  display: block;
  padding-top: 6px;
  font-size: 11px;
  font-weight: 500;
  color: var(--vp-c-text-2);
  text-transform: uppercase;
  transition: color 0.25s;
}

.VPCarbonAds .carbon-poweredby:hover {
  color: var(--vp-c-text-1);
}
</style>

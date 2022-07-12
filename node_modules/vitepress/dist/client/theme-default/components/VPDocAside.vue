<script setup lang="ts">
import { useData } from 'vitepress'
import VPDocAsideOutline from './VPDocAsideOutline.vue'
import VPDocAsideCarbonAds from './VPDocAsideCarbonAds.vue'

const { page, theme } = useData()
</script>

<template>
  <div class="VPDocAside">
    <slot name="aside-top" />

    <slot name="aside-outline-before" />
    <VPDocAsideOutline v-if="page.headers.length" />
    <slot name="aside-outline-after" />

    <div class="spacer" />

    <slot name="aside-ads-before" />
    <VPDocAsideCarbonAds v-if="theme.carbonAds" />
    <slot name="aside-ads-after" />

    <slot name="aside-bottom" />
  </div>
</template>

<style scoped>
.VPDocAside {
  display: flex;
  flex-direction: column;
  flex-grow: 1;
}

.spacer {
  flex-grow: 1;
}

.VPDocAside :deep(.spacer + .VPDocAsideSponsors),
.VPDocAside :deep(.spacer + .VPDocAsideCarbonAds) {
  margin-top: 24px;
}

.VPDocAside :deep(.VPDocAsideSponsors + .VPDocAsideCarbonAds) {
  margin-top: 16px;
}
</style>

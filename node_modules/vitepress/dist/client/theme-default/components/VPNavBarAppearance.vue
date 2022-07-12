<script lang="ts" setup>
import { useData } from 'vitepress'
import VPSwitchAppearance from './VPSwitchAppearance.vue'

const { site } = useData()
</script>

<template>
  <div v-if="site.appearance" class="VPNavBarAppearance">
    <VPSwitchAppearance />
  </div>
</template>

<style scoped>
.VPNavBarAppearance {
  display: none;
}

@media (min-width: 1280px) {
  .VPNavBarAppearance {
    display: flex;
    align-items: center;
  }
}
</style>

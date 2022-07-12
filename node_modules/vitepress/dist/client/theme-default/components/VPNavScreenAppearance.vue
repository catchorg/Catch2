<script lang="ts" setup>
import { useData } from 'vitepress'
import VPSwitchAppearance from './VPSwitchAppearance.vue'

const { site } = useData()
</script>

<template>
  <div v-if="site.appearance" class="VPNavScreenAppearance">
    <p class="text">Appearance</p>
    <VPSwitchAppearance />
  </div>
</template>

<style scoped>
.VPNavScreenAppearance {
  display: flex;
  justify-content: space-between;
  align-items: center;
  border-radius: 8px;
  padding: 12px 14px 12px 16px;
  background-color: var(--vp-c-bg-soft);
  transition: background-color 0.5s;
}

.text {
  line-height: 24px;
  font-size: 12px;
  font-weight: 500;
  color: var(--vp-c-text-2);
  transition: color 0.5s;
}
</style>

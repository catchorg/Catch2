<script lang="ts" setup>
import { inject } from 'vue'
import VPLink from './VPLink.vue'

defineProps<{
  text: string
  link: string
}>()

const closeScreen = inject('close-screen') as () => void
</script>

<template>
  <VPLink class="VPNavScreenMenuGroupLink" :href="link" @click="closeScreen">
    {{ text }}
  </VPLink>
</template>

<style scoped>
.VPNavScreenMenuGroupLink {
  display: block;
  line-height: 32px;
  font-size: 13px;
  font-weight: 400;
  color: var(--vp-c-text-1);
  transition: color 0.25s;
  margin-left: 12px;
}

.VPNavScreenMenuGroupLink:hover {
  color: var(--vp-c-brand);
}
</style>

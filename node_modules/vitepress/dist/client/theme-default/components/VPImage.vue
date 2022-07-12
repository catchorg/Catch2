<script setup lang="ts">
import type { DefaultTheme } from 'vitepress/theme'
import { withBase } from 'vitepress'

defineProps<{
  image: DefaultTheme.ThemeableImage
}>()
</script>

<script lang="ts">
export default {
  inheritAttrs: false
}
</script>

<template>
  <template v-if="image">
    <img
      v-if="typeof image === 'string' || 'src' in image"
      class="VPImage"
      v-bind="typeof image === 'string' ? $attrs : { ...image, ...$attrs }"
      :src="withBase(typeof image === 'string' ? image : image.src)"
    />
    <template v-else>
      <VPImage class="dark" :image="image.dark" v-bind="$attrs" />
      <VPImage class="light" :image="image.light" v-bind="$attrs" />
    </template>
  </template>
</template>

<style scoped>
html:not(.dark) .VPImage.dark {
  display: none;
}
.dark .VPImage.light {
  display: none;
}
</style>

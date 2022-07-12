<script lang="ts" setup>
import type { DefaultTheme } from 'vitepress/theme'
import VPSocialLink from './VPSocialLink.vue'

defineProps<{
  links: DefaultTheme.SocialLink[]
}>()
</script>

<template>
  <div class="VPSocialLinks">
    <VPSocialLink
      v-for="{ link, icon } in links"
      :key="link"
      :icon="icon"
      :link="link"
    />
  </div>
</template>

<style scoped>
.VPSocialLinks {
  display: flex;
  flex-wrap: wrap;
  justify-content: center;
}
</style>

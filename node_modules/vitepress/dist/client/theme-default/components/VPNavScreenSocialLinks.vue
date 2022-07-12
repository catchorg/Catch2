<script lang="ts" setup>
import { useData } from 'vitepress'
import VPSocialLinks from './VPSocialLinks.vue'

const { theme } = useData()
</script>

<template>
  <VPSocialLinks
    v-if="theme.socialLinks"
    class="VPNavScreenSocialLinks"
    :links="theme.socialLinks"
  />
</template>

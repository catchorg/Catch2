<script lang="ts" setup>
import type { DefaultTheme } from 'vitepress/theme'
import VPIconDiscord from './icons/VPIconDiscord.vue'
import VPIconFacebook from './icons/VPIconFacebook.vue'
import VPIconGitHub from './icons/VPIconGitHub.vue'
import VPIconLinkedIn from './icons/VPIconLinkedIn.vue'
import VPIconInstagram from './icons/VPIconInstagram.vue'
import VPIconSlack from './icons/VPIconSlack.vue'
import VPIconTwitter from './icons/VPIconTwitter.vue'
import VPIconYouTube from './icons/VPIconYouTube.vue'

defineProps<{
  icon: DefaultTheme.SocialLinkIcon
  link: string
}>()

const icons = {
  discord: VPIconDiscord,
  facebook: VPIconFacebook,
  github: VPIconGitHub,
  instagram: VPIconInstagram,
  linkedin: VPIconLinkedIn,
  slack: VPIconSlack,
  twitter: VPIconTwitter,
  youtube: VPIconYouTube
}
</script>

<template>
  <a
    class="VPSocialLink"
    :href="link"
    :title="icon"
    target="_blank"
    rel="noopener noreferrer"
  >
    <component :is="icons[icon]" class="icon" />
    <span class="visually-hidden">{{ icon }}</span>
  </a>
</template>

<style scoped>
.VPSocialLink {
  display: flex;
  justify-content: center;
  align-items: center;
  width: 36px;
  height: 36px;
  color: var(--vp-c-text-2);
  transition: color .5s;
}

.VPSocialLink:hover {
  color: var(--vp-c-text-1);
  transition: color .25s;
}

.icon {
  width: 20px;
  height: 20px;
  fill: currentColor;
}
</style>

<script lang="ts" setup>
import { useData } from 'vitepress'
import VPIconLanguages from './icons/VPIconLanguages.vue'
import VPFlyout from './VPFlyout.vue'
import VPMenuLink from './VPMenuLink.vue'

const { theme } = useData()
</script>

<template>
  <VPFlyout
    v-if="theme.localeLinks"
    class="VPNavBarTranslations"
    :icon="VPIconLanguages"
  >
    <div class="items">
      <p class="title">{{ theme.localeLinks.text }}</p>

      <template v-for="locale in theme.localeLinks.items" :key="locale.link">
        <VPMenuLink :item="locale" />
      </template>
    </div>
  </VPFlyout>
</template>

<style scoped>
.VPNavBarTranslations {
  display: none;
}

@media (min-width: 1280px) {
  .VPNavBarTranslations {
    display: flex;
    align-items: center;
  }
}

.title {
  padding: 0 24px 0 12px;
  line-height: 32px;
  font-size: 14px;
  font-weight: 700;
  color: var(--vp-c-text-1);
}
</style>

<script setup lang="ts">
import { ref } from 'vue'
import { useData } from 'vitepress'
import VPIconChevronDown from './icons/VPIconChevronDown.vue'
import VPIconLanguages from './icons/VPIconLanguages.vue'

const { theme } = useData()

const isOpen = ref(false)

function toggle() {
  isOpen.value = !isOpen.value
}
</script>

<template>
  <div v-if="theme.localeLinks" class="VPNavScreenTranslations" :class="{ open: isOpen }">
    <button class="title" @click="toggle">
      <VPIconLanguages class="icon lang" />
      {{ theme.localeLinks.text }}
      <VPIconChevronDown class="icon chevron" />
    </button>

    <ul class="list">
      <li v-for="locale in theme.localeLinks.items" :key="locale.link" class="item">
        <a class="link" :href="locale.link">{{ locale.text }}</a>
      </li>
    </ul>
  </div>
</template>

<style scoped>
.VPNavScreenTranslations {
  height: 24px;
  overflow: hidden;
}

.VPNavScreenTranslations.open {
  height: auto;
}

.title {
  display: flex;
  align-items: center;
  font-size: 14px;
  font-weight: 500;
  color: var(--vp-c-text-1);
}

.icon {
  width: 16px;
  height: 16px;
  fill: currentColor;
}

.icon.lang {
  margin-right: 8px;
}

.icon.chevron {
  margin-left: 4px;
}

.list {
  padding: 4px 0 0 24px;
}

.link {
  line-height: 32px;
  font-size: 13px;
  color: var(--vp-c-text-1);
}
</style>

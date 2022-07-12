<script setup lang="ts">
defineProps<{
  icon?: string
  title: string
  details: string
}>()
</script>

<template>
  <article class="VPFeature">
    <div v-if="icon" class="icon">{{ icon }}</div>
    <h2 class="title">{{ title }}</h2>
    <p class="details">{{ details }}</p>
  </article>
</template>

<style scoped>
.VPFeature {
  border: 1px solid var(--vp-c-bg-soft);
  border-radius: 12px;
  padding: 24px;
  height: 100%;
  background-color: var(--vp-c-bg-soft);
}

.icon {
  display: flex;
  justify-content: center;
  align-items: center;
  margin-bottom: 20px;
  border-radius: 6px;
  background-color: var(--vp-c-gray-light-4);
  width: 48px;
  height: 48px;
  font-size: 24px;
}

.dark .icon {
  background-color: var(--vp-c-bg);
}

.title {
  line-height: 24px;
  font-size: 16px;
  font-weight: 600;
}

.details {
  padding-top: 8px;
  line-height: 24px;
  font-size: 14px;
  font-weight: 500;
  color: var(--vp-c-text-2);
}
</style>

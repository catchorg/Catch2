<script setup lang="ts">
import { ref, watchEffect, onMounted } from 'vue'
import { useData } from 'vitepress'

const { theme, page } = useData()

const date = new Date(page.value.lastUpdated!)
const isoDatetime = date.toISOString()
const datetime = ref('')

// set time on mounted hook because the locale string might be different
// based on end user and will lead to potential hydration mismatch if
// calculated at build time
onMounted(() => {
  watchEffect(() => {
    datetime.value = date.toLocaleString(window.navigator.language)
  })
})
</script>

<template>
  <p class="VPLastUpdated">
    {{ theme.lastUpdatedText ?? 'Last updated' }}:
    <time :datatime="isoDatetime">{{ datetime }}</time>
  </p>
</template>

<style scoped>
.VPLastUpdated {
  line-height: 24px;
  font-size: 14px;
  font-weight: 500;
  color: var(--vp-c-text-2);
}

@media (min-width: 640px) {
  .VPLastUpdated {
    line-height: 32px;
    font-size: 14px;
    font-weight: 500;
  }
}
</style>

<script lang="ts" setup>
import { computed } from 'vue'
import { normalizeLink } from '../support/utils'
import VPIconExternalLink from './icons/VPIconExternalLink.vue'

const props = defineProps<{
  href?: string
  noIcon?: boolean
}>()

const isExternal = computed(() => props.href && /^[a-z]+:/i.test(props.href))
</script>

<template>
  <component
    :is="href ? 'a' : 'span'"
    class="VPLink"
    :class="{ link: href }"
    :href="href ? normalizeLink(href) : undefined"
    :target="isExternal ? '_blank' : undefined"
    :rel="isExternal ? 'noopener noreferrer' : undefined"
  >
    <slot />
    <VPIconExternalLink v-if="isExternal && !noIcon" class="icon" />
  </component>
</template>

<style scoped>
.icon {
  display: inline-block;
  margin-top: -1px;
  margin-left: 4px;
  width: 11px;
  height: 11px;
  fill: var(--vp-c-text-3);
  transition: fill 0.25s;
}
</style>

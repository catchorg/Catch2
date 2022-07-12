<script setup lang="ts">
import { computed } from 'vue'
import VPSponsorsGrid from './VPSponsorsGrid.vue'

export interface Sponsors {
  tier?: string
  size?: 'small' | 'medium' | 'big'
  items: Sponsor[]
}

export interface Sponsor {
  name: string
  img: string
  url: string
}

const props = defineProps<{
  mode?: 'normal' | 'aside'
  tier?: string
  size?: 'xmini' | 'small' | 'medium' | 'big'
  data: Sponsors[] | Sponsor[]
}>()

const sponsors = computed(() => {
  const isSponsors = props.data.some((s) => {
    return !!(s as Sponsors).items
  })

  if (isSponsors) {
    return props.data
  }

  return [{
    tier: props.tier,
    size: props.size,
    items: props.data
  }]
})
</script>

<template>
  <div class="VPSponsors vp-sponsor" :class="[mode ?? 'normal']">
    <section
      v-for="(sponsor, index) in sponsors"
      :key="index"
      class="vp-sponsor-section"
    >
      <h3 v-if="sponsor.tier" class="vp-sponsor-tier">{{ sponsor.tier }}</h3>
      <VPSponsorsGrid :size="sponsor.size" :data="sponsor.items" />
    </section>
  </div>
</template>

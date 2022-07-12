<script setup lang="ts">
import VPSponsors from './VPSponsors.vue'

export interface Sponsors {
  tier?: string
  size?: 'xmini' | 'mini' | 'small'
  items: Sponsor[]
}

export interface Sponsor {
  name: string
  img: string
  url: string
}

defineProps<{
  tier?: string
  size?: 'xmini' | 'mini' | 'small'
  data: Sponsors[] | Sponsor[]
}>()
</script>

<template>
  <div class="VPDocAsideSponsors">
    <VPSponsors
      mode="aside"
      :tier="tier"
      :size="size"
      :data="data"
    />
  </div>
</template>

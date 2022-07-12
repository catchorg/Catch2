<script lang="ts" setup>
import { useData } from 'vitepress'
import VPNavScreenMenuLink from './VPNavScreenMenuLink.vue'
import VPNavScreenMenuGroup from './VPNavScreenMenuGroup.vue'

const { theme } = useData()
</script>

<template>
  <nav v-if="theme.nav" class="VPNavScreenMenu">
    <template v-for="item in theme.nav" :key="item.text">
      <VPNavScreenMenuLink
        v-if="'link' in item"
        :text="item.text"
        :link="item.link"
      />
      <VPNavScreenMenuGroup
        v-else
        :text="item.text || ''"
        :items="item.items"
      />
    </template>
  </nav>
</template>

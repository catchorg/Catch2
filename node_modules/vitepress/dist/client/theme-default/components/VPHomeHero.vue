<script setup lang="ts">
import { useData } from 'vitepress'
import VPHero from './VPHero.vue'

const { frontmatter: fm } = useData()
</script>

<template>
  <VPHero
    v-if="fm.hero"
    class="VPHomeHero"
    :name="fm.hero.name"
    :text="fm.hero.text"
    :tagline="fm.hero.tagline"
    :image="fm.hero.image"
    :actions="fm.hero.actions"
  />
</template>

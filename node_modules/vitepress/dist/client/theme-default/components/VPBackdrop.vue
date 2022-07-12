<script lang="ts" setup>
defineProps<{
  show: boolean
}>()
</script>

<template>
  <transition name="fade">
    <div v-if="show" class="VPBackdrop" />
  </transition>
</template>

<style scoped>
.VPBackdrop {
  position: fixed;
  top: 0;
  right: 0;
  bottom: 0;
  left: 0;
  z-index: var(--vp-z-index-backdrop);
  background: rgba(0, 0, 0, .6);
  transition: opacity 0.5s;
}

.VPBackdrop.fade-enter-from,
.VPBackdrop.fade-leave-to {
  opacity: 0;
}

.VPBackdrop.fade-leave-active {
  transition-duration: .25s;
}

@media (min-width: 1280px) {
  .VPBackdrop {
    display: none;
  }
}
</style>

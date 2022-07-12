<script setup lang="ts">
import { useRoute } from 'vitepress'
import { useSidebar } from '../composables/sidebar'
import VPDocAside from './VPDocAside.vue'
import VPDocFooter from './VPDocFooter.vue'

const { path } = useRoute()
const { hasSidebar } = useSidebar()

const pageName = path.replace(/[./]+/g, '_').replace(/_html$/, '')
</script>

<template>
  <div class="VPDoc" :class="{ 'has-sidebar': hasSidebar }">
    <div class="container">
      <div class="aside">
        <div class="aside-curtain" />
        <div class="aside-container">
          <div class="aside-content">
            <VPDocAside>
              <template #aside-top><slot name="aside-top" /></template>
              <template #aside-bottom><slot name="aside-bottom" /></template>
              <template #aside-outline-before><slot name="aside-outline-before" /></template>
              <template #aside-outline-after><slot name="aside-outline-after" /></template>
              <template #aside-ads-before><slot name="aside-ads-before" /></template>
              <template #aside-ads-after><slot name="aside-ads-after" /></template>
            </VPDocAside>
          </div>
        </div>
      </div>

      <div class="content">
        <div class="content-container">
          <slot name="doc-before" />

          <main class="main">
            <Content class="vp-doc" :class="pageName" />
          </main>

          <VPDocFooter />

          <slot name="doc-after" />
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.VPDoc {
  padding: 32px 24px 96px;
  width: 100%;
}

@media (min-width: 768px) {
  .VPDoc {
    padding: 48px 32px 128px;
  }
}

@media (min-width: 960px) {
  .VPDoc {
    padding: 32px 32px 0;
  }

  .VPDoc:not(.has-sidebar) .container {
    display: flex;
    justify-content: center;
    max-width: 992px;
  }

  .VPDoc:not(.has-sidebar) .aside {
    display: block;
  }

  .VPDoc:not(.has-sidebar) .content {
    max-width: 752px;
  }
}

@media (min-width: 1280px) {
  .VPDoc .container {
    display: flex;
    justify-content: center;
  }

  .VPDoc .aside {
    display: block;
  }
}

@media (min-width: 1440px) {
  .VPDoc:not(.has-sidebar) .content {
    max-width: 784px;
  }

  .VPDoc:not(.has-sidebar) .container {
    max-width: 1104px;
  }
}

.container {
  margin: 0 auto;
  width: 100%;
}

.aside {
  position: relative;
  display: none;
  order: 2;
  flex-grow: 1;
  padding-left: 32px;
  width: 100%;
  max-width: 256px;
}

.aside-container {
  position: sticky;
  top: 0;
  margin-top: calc(var(--vp-nav-height-desktop) * -1 - 32px);
  padding-top: calc(var(--vp-nav-height-desktop) + 32px);
  height: 100vh;
  overflow-x: hidden;
  overflow-y: auto;
  scrollbar-width: none;
}

.aside-container::-webkit-scrollbar {
  display: none;
}

.aside-curtain {
  position: fixed;
  bottom: 0;
  z-index: 10;
  width: 224px;
  height: 32px;
  background: linear-gradient(transparent, var(--vp-c-bg) 70%);
}

.aside-content {
  display: flex;
  flex-direction: column;
  min-height: calc(100vh - (var(--vp-nav-height-desktop) + 32px));
  padding-bottom: 32px;
}

.content {
  position: relative;
  margin: 0 auto;
  width: 100%;
}

@media (min-width: 960px) {
  .content {
    padding: 0 32px 128px;
  }
}

@media (min-width: 1280px) {
  .content {
    order: 1;
    margin: 0;
    min-width: 640px;
  }
}

.content-container {
  margin: 0 auto;
  max-width: 688px;
}
</style>

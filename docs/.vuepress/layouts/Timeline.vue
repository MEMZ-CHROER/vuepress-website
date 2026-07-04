<script setup>
import { useBlogType } from '@vuepress/plugin-blog/client'
import ParentLayout from '@vuepress/theme-default/layouts/Layout.vue'
import ArticleList from '../components/ArticleList.vue'

const timelines = useBlogType('timeline')
</script>

<template>
  <ParentLayout>
    <template #page>
      <main class="page timeline-page">
        <div class="page-header">
          <h1 class="timeline-title">⏳ 时间线</h1>
          <p class="page-subtitle">共 {{ timelines.items.length }} 篇有日期记录的文章</p>
        </div>

        <div class="timeline-bar">
          <div class="timeline-progress">
            <span class="timeline-dot"></span>
            <span class="timeline-line"></span>
          </div>
        </div>

        <ArticleList :items="timelines.items" is-timeline />
      </main>
    </template>
  </ParentLayout>
</template>

<style lang="scss" scoped>
.timeline-page {
  padding: 1rem 0;
}

.page-header {
  text-align: center;
  padding: 1.5rem 1rem 0.5rem;
}

.timeline-title {
  font-size: 2rem;
  font-weight: 800;
  background: linear-gradient(135deg, var(--c-brand), #a78bfa);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  background-clip: text;
  margin: 0;
}

.page-subtitle {
  color: var(--c-text-lighter);
  font-size: 0.9rem;
  margin-top: 0.3rem;
}

.timeline-bar {
  display: flex;
  justify-content: center;
  padding: 0.5rem 0 1rem;
}

.timeline-progress {
  display: flex;
  align-items: center;
  gap: 4px;
}

.timeline-dot {
  width: 10px;
  height: 10px;
  border-radius: 50%;
  background: var(--c-brand);
  box-shadow: 0 0 0 3px rgba(99, 102, 241, 0.2);
}

.timeline-line {
  width: 60px;
  height: 2px;
  background: linear-gradient(90deg, var(--c-brand), transparent);
  border-radius: 1px;
}
</style>

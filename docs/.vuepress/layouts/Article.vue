<script setup>
import { useBlogType } from '@vuepress/plugin-blog/client'
import ParentLayout from '@vuepress/theme-default/layouts/Layout.vue'
import ArticleList from '../components/ArticleList.vue'

const articles = useBlogType('article')
</script>

<template>
  <ParentLayout>
    <template #page>
      <main class="page article-page">
        <div class="page-header">
          <h1 class="page-title">📖 文章</h1>
          <p class="page-subtitle">共 {{ articles.items.length }} 篇文章</p>
        </div>
        <ArticleList :items="articles.items" />
      </main>
    </template>
  </ParentLayout>
</template>

<style lang="scss" scoped>
.article-page {
  padding: 1rem 0;
}

.page-header {
  text-align: center;
  padding: 1.5rem 1rem 0.5rem;
}

.page-title {
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
</style>

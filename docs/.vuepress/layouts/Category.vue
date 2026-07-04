<script setup>
import { useBlogCategory } from '@vuepress/plugin-blog/client'
import ParentLayout from '@vuepress/theme-default/layouts/Layout.vue'
import { RouteLink, useRoute } from 'vuepress/client'
import ArticleList from '../components/ArticleList.vue'

const route = useRoute()
const categoryMap = useBlogCategory('category')
</script>

<template>
  <ParentLayout>
    <template #page>
      <main class="page category-page">
        <div class="page-header">
          <h1 class="page-title">📂 分类</h1>
          <p class="page-subtitle">共 {{ Object.keys(categoryMap.map).length }} 个分类</p>
        </div>

        <div class="category-wrapper">
          <RouteLink
            v-for="({ items, path }, name) in categoryMap.map"
            :key="name"
            :to="path"
            :active="route.path === path"
            class="category-chip"
          >
            <span class="category-name">{{ name }}</span>
            <span class="category-num">{{ items.length }}</span>
          </RouteLink>
        </div>

        <ArticleList :items="categoryMap.currentItems ?? []" />
      </main>
    </template>
  </ParentLayout>
</template>

<style lang="scss" scoped>
.category-page {
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

.category-wrapper {
  display: flex;
  flex-wrap: wrap;
  gap: 0.6rem;
  justify-content: center;
  padding: 1rem 2rem 1.5rem;
  max-width: 800px;
  margin: 0 auto;
}

.category-chip {
  display: inline-flex;
  align-items: center;
  gap: 6px;
  padding: 0.5rem 1.2rem;
  border-radius: 50px;
  font-size: 0.9rem;
  font-weight: 500;
  background: var(--c-bg-lighter);
  border: 1.5px solid var(--c-border);
  color: var(--c-text-light) !important;
  cursor: pointer;
  transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
  box-shadow: var(--shadow-sm, 0 1px 2px rgba(0, 0, 0, 0.04));
  text-decoration: none !important;

  &:hover {
    border-color: var(--c-brand-light);
    color: var(--c-brand) !important;
    background: rgba(99, 102, 241, 0.06);
    transform: translateY(-2px);
    box-shadow: 0 4px 14px rgba(99, 102, 241, 0.15);
  }

  &.route-link-active {
    background: linear-gradient(135deg, var(--c-brand), var(--c-brand-light));
    color: #fff !important;
    border-color: transparent;
    box-shadow: 0 4px 14px rgba(99, 102, 241, 0.3);
  }

  .category-num {
    display: inline-flex;
    align-items: center;
    justify-content: center;
    min-width: 1.3rem;
    height: 1.4rem;
    padding: 0 0.35rem;
    border-radius: 20px;
    font-size: 0.7rem;
    font-weight: 700;
    background: rgba(0, 0, 0, 0.06);
  }

  &.route-link-active .category-num {
    background: rgba(255, 255, 255, 0.2);
    color: #fff;
  }
}
</style>

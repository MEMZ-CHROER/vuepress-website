<script setup>
import { useBlogCategory } from "@vuepress/plugin-blog/client";
import ParentLayout from "@vuepress/theme-default/layouts/Layout.vue";
import { RouteLink, useRoute } from "vuepress/client";
import ArticleList from "../components/ArticleList.vue";

const route = useRoute();
const tagMap = useBlogCategory("tag");
</script>

<template>
  <ParentLayout>
    <template #page>
      <main class="page tag-page">
        <div class="page-header">
          <h1 class="page-title">🏷️ 标签</h1>
          <p class="page-subtitle">共 {{ Object.keys(tagMap.map).length }} 个标签</p>
        </div>

        <div class="tag-wrapper">
          <RouteLink
            v-for="({ items, path }, name) in tagMap.map"
            :key="name"
            :to="path"
            :active="route.path === path"
            class="tag-chip"
            :style="getTagStyle(items.length)"
          >
            <span class="tag-name">#{{ name }}</span>
            <span class="tag-num">{{ items.length }}</span>
          </RouteLink>
        </div>

        <ArticleList :items="tagMap.currentItems ?? []" />
      </main>
    </template>
  </ParentLayout>
</template>

<script>
export default {
  methods: {
    getTagStyle(count) {
      // 根据文章数量调整标签大小
      const max = 20;
      const min = 6;
      const clamped = Math.min(count, max);
      const ratio = Math.max(0.3, clamped / max);
      const fontSize = 0.8 + ratio * 0.35;
      return { fontSize: `${fontSize}rem` };
    },
  },
};
</script>

<style lang="scss" scoped>
.tag-page {
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

.tag-wrapper {
  display: flex;
  flex-wrap: wrap;
  gap: 0.5rem;
  justify-content: center;
  padding: 1rem 2rem 1.5rem;
  max-width: 700px;
  margin: 0 auto;
}

.tag-chip {
  display: inline-flex;
  align-items: center;
  gap: 4px;
  padding: 0.35rem 1rem;
  border-radius: 50px;
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
    transform: translateY(-2px) scale(1.02);
    box-shadow: 0 4px 14px rgba(99, 102, 241, 0.15);
  }

  &.route-link-active {
    background: linear-gradient(135deg, var(--c-brand), var(--c-brand-light));
    color: #fff !important;
    border-color: transparent;
    box-shadow: 0 4px 14px rgba(99, 102, 241, 0.3);
  }

  .tag-name {
    line-height: 1;
  }

  .tag-num {
    display: inline-flex;
    align-items: center;
    justify-content: center;
    min-width: 1.1rem;
    height: 1.2rem;
    padding: 0 0.3rem;
    border-radius: 20px;
    font-size: 0.65rem;
    font-weight: 700;
    background: rgba(0, 0, 0, 0.06);
  }

  &.route-link-active .tag-num {
    background: rgba(255, 255, 255, 0.2);
    color: #fff;
  }
}
</style>

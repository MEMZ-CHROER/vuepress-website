<script setup>
import { usePageFrontmatter } from 'vuepress/client'
import ParentLayout from '@vuepress/theme-default/layouts/Layout.vue'

const frontmatter = usePageFrontmatter()
</script>

<template>
  <ParentLayout>
    <!-- 页面底部插入评论区（排除首页和特定页面） -->
    <template #page-bottom>
      <div v-if="!frontmatter.home && frontmatter.comment !== false" class="comment-section">
        <Comment />
      </div>
    </template>
  </ParentLayout>
</template>

<style scoped>
.comment-section {
  margin-top: 2rem;
}
</style>

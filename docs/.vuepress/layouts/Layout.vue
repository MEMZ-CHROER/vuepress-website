<script setup>
import { usePageFrontmatter } from "vuepress/client";
import { Layout } from "vuepress-theme-hope/client";

const frontmatter = usePageFrontmatter();
</script>

<template>
  <Layout>
    <!-- 页面底部插入评论区（排除首页和特定页面） -->
    <template #pageBottom>
      <div v-if="!frontmatter.home && frontmatter.comment !== false" class="comment-section">
        <Comment />
      </div>
    </template>
  </Layout>

  <!-- 全局彩蛋 -->
  <EasterEgg />

  <!-- 前台编辑（登录后右下角可编辑此页） -->
  <FrontEditor />
</template>

<style scoped>
.comment-section {
  margin-top: 2rem;
}
</style>

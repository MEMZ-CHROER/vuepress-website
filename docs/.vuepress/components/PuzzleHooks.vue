<script setup>
import { onMounted } from "vue";

/**
 * 全站谜题钩子：
 * 666 解锁后，若开启了黄金模式，给 <html> 挂上 .pzl-gold。
 * 真正的全局样式在 .vuepress/styles/index.scss 里。
 */
onMounted(() => {
  try {
    if (localStorage.getItem("lxy_gold") === "1") {
      document.documentElement.classList.add("pzl-gold");
    }
  } catch {
    /* ignore */
  }
});
</script>

<template>
  <span style="display: none" aria-hidden="true">666</span>
</template>

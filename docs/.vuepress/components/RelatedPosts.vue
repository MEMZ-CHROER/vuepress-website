<script setup>
import { ref, computed, onMounted } from "vue";
import { useArticles } from "vuepress-theme-hope/blog";

const articles = useArticles();
const recs = ref([]);

function pickRandom(items, n) {
  const arr = items.slice();
  // 洗牌后取前 n
  for (let i = arr.length - 1; i > 0; i--) {
    const j = Math.floor(Math.random() * (i + 1));
    [arr[i], arr[j]] = [arr[j], arr[i]];
  }
  return arr.slice(0, n);
}

onMounted(() => {
  const data = articles.value;
  if (data && data.items && data.items.length) {
    recs.value = pickRandom(data.items, 3).map((a) => ({
      path: a.path,
      title: (a.info && (a.info.title || a.info.shortTitle)) || "未命名文章",
    }));
  }
});
</script>

<template>
  <div class="rec-panel">
    <h3 class="rec-title">🎲 随机推荐</h3>
    <div v-if="recs.length" class="rec-list">
      <a v-for="(r, i) in recs" :key="i" :href="r.path" class="rec-item">
        <span class="rec-num">{{ i + 1 }}</span>
        <span class="rec-name">{{ r.title }}</span>
      </a>
    </div>
    <p v-else class="rec-empty">加载中...</p>
  </div>
</template>

<style scoped>
.rec-panel {
  max-width: 800px;
  margin: 1.5rem auto;
  padding: 0 1rem;
}
.rec-title {
  text-align: center;
  font-size: 1.1rem;
  font-weight: 700;
  color: var(--c-text-dark);
  margin-bottom: 1rem;
}
.rec-list {
  display: flex;
  flex-direction: column;
  gap: 0.5rem;
}
.rec-item {
  display: flex;
  align-items: center;
  gap: 0.6rem;
  padding: 0.6rem 0.9rem;
  background: var(--c-bg-lighter);
  border: 1px solid var(--c-border);
  border-radius: 8px;
  text-decoration: none;
  color: var(--c-text-dark);
  transition: all 0.25s ease;
}
.rec-item:hover {
  border-color: var(--c-brand, #6366f1);
  transform: translateX(4px);
  color: var(--c-brand, #6366f1);
}
.rec-num {
  flex-shrink: 0;
  width: 1.4rem;
  height: 1.4rem;
  display: flex;
  align-items: center;
  justify-content: center;
  border-radius: 50%;
  background: var(--c-brand, #6366f1);
  color: #fff;
  font-size: 0.75rem;
  font-weight: 700;
}
.rec-name {
  font-size: 0.9rem;
  font-weight: 500;
}
.rec-empty {
  text-align: center;
  color: var(--c-text-lighter);
  font-size: 0.85rem;
}
</style>

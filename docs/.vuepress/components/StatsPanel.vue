<script setup>
import { ref } from "vue";

const items = ref([
  { icon: "📝", label: "文章", value: 40, desc: "篇原创内容", color: "#6366f1" },
  { icon: "📂", label: "分类", value: 6, desc: "个分类", color: "#8b5cf6" },
  { icon: "🏷️", label: "标签", value: 18, desc: "个标签", color: "#ec4899" },
  { icon: "📄", label: "页面", value: 141, desc: "页收录", color: "#14b8a6" },
  { icon: "🎨", label: "坦克", value: 33, desc: "辆原创手绘", color: "#f97316" },
  { icon: "🔗", label: "友链", value: 5, desc: "个小伙伴", color: "#06b6d4" },
]);
</script>

<template>
  <div class="stats-panel">
    <h3 class="stats-title">📊 站点统计</h3>
    <div class="stats-grid">
      <div v-for="s in items" :key="s.label" class="stat-card" :style="{ '--stat-color': s.color }">
        <div class="stat-icon">{{ s.icon }}</div>
        <div class="stat-value">{{ s.value }}</div>
        <div class="stat-label">{{ s.label }}</div>
        <div class="stat-desc">{{ s.desc }}</div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.stats-panel {
  max-width: 800px;
  margin: 1.5rem auto;
  padding: 0 1rem;
}
.stats-title {
  text-align: center;
  font-size: 1.1rem;
  font-weight: 700;
  color: var(--c-text-dark);
  margin-bottom: 1rem;
}
.stats-grid {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 0.75rem;
}
.stat-card {
  background: var(--c-bg-lighter);
  border: 1px solid var(--c-border);
  border-radius: var(--radius-md, 10px);
  padding: 1rem 0.8rem;
  text-align: center;
  transition: all 0.3s cubic-bezier(0.34, 1.56, 0.64, 1);
  box-shadow: var(--shadow-sm, 0 1px 2px rgba(0, 0, 0, 0.04));
  position: relative;
  overflow: hidden;
}
.stat-card::before {
  content: "";
  position: absolute;
  top: 0;
  left: 0;
  right: 0;
  height: 3px;
  background: var(--stat-color);
  opacity: 0.6;
  transition: opacity 0.3s ease;
}
.stat-card:hover {
  transform: translateY(-4px);
  box-shadow: 0 8px 24px rgba(0, 0, 0, 0.08);
  border-color: var(--stat-color);
}
.stat-card:hover::before {
  opacity: 1;
}
.stat-icon {
  font-size: 1.5rem;
  margin-bottom: 0.3rem;
}
.stat-value {
  font-size: 1.6rem;
  font-weight: 800;
  color: var(--stat-color);
  line-height: 1.2;
}
.stat-label {
  font-size: 0.8rem;
  font-weight: 600;
  color: var(--c-text-light);
  margin-top: 0.1rem;
}
.stat-desc {
  font-size: 0.7rem;
  color: var(--c-text-lighter);
  margin-top: 0.15rem;
}
@media (max-width: 500px) {
  .stats-grid {
    grid-template-columns: repeat(2, 1fr);
  }
  .stat-value {
    font-size: 1.3rem;
  }
}
</style>

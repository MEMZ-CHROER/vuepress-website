<script setup>
import { ref } from "vue";
import { useRoute } from "vuepress/client";

const route = useRoute();
// 取路径，例如 /posts/intro.html -> posts/intro
const slug = route.path
  .replace(/^\//, "")
  .replace(/\.html$/, "")
  .replace(/\/$/, "");

const loading = ref(false);
const summary = ref("");
const errorMsg = ref("");

async function fetchSummary() {
  loading.value = true;
  summary.value = "";
  errorMsg.value = "";
  try {
    const resp = await fetch(`/api/summary/${encodeURIComponent(slug)}`);
    const data = await resp.json();
    if (resp.ok) {
      summary.value = data.summary;
    } else {
      errorMsg.value = data.error || `请求错误 ${resp.status}`;
    }
  } catch (e) {
    errorMsg.value = e.message;
  } finally {
    loading.value = false;
  }
}
</script>

<template>
  <div class="summary-button">
    <button @click="fetchSummary" :disabled="loading">
      {{ loading ? "生成中..." : "生成摘要" }}
    </button>
    <p v-if="summary" class="summary-text">{{ summary }}</p>
    <p v-if="errorMsg" class="error-text">{{ errorMsg }}</p>
  </div>
</template>

<style scoped>
.summary-button {
  margin: 1rem 0;
  text-align: center;
}
.summary-button button {
  padding: 0.4rem 0.8rem;
  background: var(--c-brand);
  color: #fff;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}
.summary-button button:disabled {
  opacity: 0.6;
  cursor: not-allowed;
}
.summary-text {
  margin-top: 0.6rem;
  font-size: 0.9rem;
  line-height: 1.4;
  color: var(--c-text);
}
.error-text {
  margin-top: 0.6rem;
  color: var(--c-danger);
}
</style>

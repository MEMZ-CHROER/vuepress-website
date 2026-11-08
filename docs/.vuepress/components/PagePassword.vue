<script setup>
import { ref, onMounted, computed } from "vue";
import { useRoute } from "vue-router";
import { usePageData } from "vuepress/client";

const route = useRoute();
const page = usePageData();

const apiBase = "https://www.liuxiyu.cn/api/page-password";
const slug = computed(() => {
  const s = route.path.replace(/\/$/, "").replace(/\.html$/, "") || "/";
  return encodeURIComponent(s);
});

const loading = ref(true);
const needsPw = ref(false);
const locked = ref(false); // 需要密码且尚未通过
const wrong = ref(false);
const input = ref("");
const errorMsg = ref("");

async function check() {
  try {
    const r = await fetch(`${apiBase}/${slug.value}?cookie=1`);
    const d = await r.json();
    needsPw.value = !!d.needsPw;
    locked.value = d.needsPw && !d.validSession;
  } catch {
    // 网络错误时不锁页面，允许访问
  } finally {
    loading.value = false;
  }
}

async function submit() {
  wrong.value = false;
  errorMsg.value = "";
  try {
    const r = await fetch(`${apiBase}/${slug.value}`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ password: input.value }),
    });
    const d = await r.json();
    if (r.ok && d.ok) {
      locked.value = false;
    } else {
      wrong.value = true;
      errorMsg.value = d.error || "密码错误";
    }
  } catch {
    errorMsg.value = "网络错误";
    wrong.value = true;
  }
}
</script>

<template>
  <div class="pp-overlay" v-if="loading">
    <div class="pp-box">
      <p class="pp-status">加载中…</p>
    </div>
  </div>
  <template v-else-if="locked">
    <div class="pp-overlay">
      <div class="pp-box">
        <h1>🔒 此页面已加密</h1>
        <p class="pp-desc">请输入访问密码以继续浏览。</p>
        <form @submit.prevent="submit">
          <input
            v-model="input"
            type="password"
            placeholder="输入密码"
            autocomplete="current-password"
            autofocus
          />
          <button type="submit">验证</button>
        </form>
        <p v-if="wrong" class="pp-err">{{ errorMsg }}</p>
      </div>
    </div>
    <!-- 阻止下方内容渲染 -->
    <div style="display:none"></div>
  </template>
</template>

<style scoped>
.pp-overlay {
  position: fixed;
  inset: 0;
  z-index: 9999;
  background: #0c0c0c;
  display: flex;
  align-items: center;
  justify-content: center;
  font-family: "JetBrains Mono", "Fira Code", monospace;
}
.pp-box {
  max-width: 400px;
  width: 90%;
  padding: 40px 32px;
  background: #0f1a0f;
  border: 1px solid #1a3a1a;
  text-align: center;
  color: #cce8cc;
}
.pp-box h1 {
  font-size: 22px;
  color: #00ff41;
  margin: 0 0 12px;
}
.pp-desc {
  font-size: 14px;
  opacity: 0.7;
  margin-bottom: 20px;
}
.pp-box input {
  width: 100%;
  padding: 10px 12px;
  background: #0c0c0c;
  border: 1px solid #1a3a3a;
  color: #cce8cc;
  font-size: 15px;
  font-family: inherit;
  box-sizing: border-box;
  outline: none;
}
.pp-box input:focus {
  border-color: #00ff41;
}
.pp-box button {
  width: 100%;
  padding: 12px;
  margin-top: 12px;
  background: #00ff41;
  color: #000;
  border: none;
  font-size: 15px;
  font-weight: bold;
  cursor: pointer;
  font-family: inherit;
}
.pp-box button:hover {
  background: #00cc33;
}
.pp-err {
  color: #e74c3c;
  font-size: 13px;
  margin-top: 12px;
}
.pp-status {
  color: #888;
  font-size: 14px;
}
</style>

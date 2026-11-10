<script setup>
import { onMounted, ref, watch } from "vue";
import { useRoute } from "vuepress/client";

/**
 * 第九关：URL 参数里的门。
 * 访问任意页面时带上 ?pzl=door9 即可触发。
 */
const route = useRoute();
const visible = ref(false);

function check() {
  visible.value = String(route.query.pzl || "") === "door9";
}

onMounted(check);
watch(() => route.query.pzl, check);

function close() {
  visible.value = false;
  // 把参数从地址栏抹掉，避免刷新反复弹出
  try {
    const url = new URL(window.location.href);
    url.searchParams.delete("pzl");
    window.history.replaceState({}, "", url.pathname + url.search + url.hash);
  } catch {
    /* ignore */
  }
}
</script>

<template>
  <Transition name="door-fade">
    <div v-if="visible" class="door-overlay" @click.self="close">
      <div class="door-box">
        <div class="door-kicker">DOOR // 09</div>
        <h2>参数之门</h2>
        <p class="door-text">
          你找到了藏在 URL 里的一扇门。<br />
          它不在导航栏里，也不在搜索里——只在一个参数后面。
        </p>
        <div class="door-answer">第九片答案：<b>DOOR</b></div>
        <p class="door-text">
          最后一扇门不在页面里，它在时间里。<br />
          去 GitHub 找到标题含 PROJECT 666 的那次提交，<br />
          把正文每一行第一个字母连起来。
        </p>
        <button class="door-btn" @click="close">收下，关门</button>
      </div>
    </div>
  </Transition>
</template>

<style scoped>
.door-overlay {
  position: fixed;
  inset: 0;
  z-index: 200;
  display: grid;
  place-items: center;
  padding: 1rem;
  background: rgba(2, 2, 10, 0.72);
  backdrop-filter: blur(6px);
}
.door-box {
  width: min(520px, 100%);
  padding: 1.8rem 1.6rem;
  border-radius: 16px;
  border: 1px solid rgba(99, 102, 241, 0.5);
  background:
    radial-gradient(circle at 50% 0%, rgba(99, 102, 241, 0.16), transparent 65%),
    #0a0a1c;
  box-shadow: 0 0 50px rgba(99, 102, 241, 0.25);
  color: #cbd5e1;
  font-family: "JetBrains Mono", ui-monospace, monospace;
}
.door-kicker {
  font-size: 0.66rem;
  letter-spacing: 0.28em;
  color: #818cf8;
}
.door-box h2 {
  margin: 0.4rem 0 0.8rem;
  font-size: 1.6rem;
  font-weight: 900;
  letter-spacing: 0.06em;
  color: #e0e7ff;
}
.door-text {
  font-size: 0.8rem;
  line-height: 1.9;
  color: #94a3b8;
}
.door-answer {
  margin: 1rem 0;
  padding: 0.8rem 1rem;
  border-radius: 10px;
  border: 1px solid rgba(34, 197, 94, 0.35);
  background: rgba(34, 197, 94, 0.08);
  color: #86efac;
  font-size: 0.82rem;
  letter-spacing: 0.06em;
}
.door-answer b {
  color: #4ade80;
  letter-spacing: 0.2em;
}
.door-btn {
  margin-top: 0.6rem;
  padding: 0.55rem 1.1rem;
  border-radius: 999px;
  border: 1px solid rgba(99, 102, 241, 0.5);
  background: rgba(99, 102, 241, 0.16);
  color: #c7d2fe;
  font-family: inherit;
  font-size: 0.76rem;
  font-weight: 700;
  letter-spacing: 0.08em;
  cursor: pointer;
}
.door-btn:hover {
  background: rgba(99, 102, 241, 0.28);
}
.door-fade-enter-active,
.door-fade-leave-active {
  transition: opacity 0.25s ease;
}
.door-fade-enter-from,
.door-fade-leave-to {
  opacity: 0;
}
</style>

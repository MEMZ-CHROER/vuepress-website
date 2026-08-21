<script setup>
import { ref, onMounted, watch, nextTick } from "vue";
import { useRoute } from "vuepress/client";

const route = useRoute();
const commentEl = ref(null);
const TWIKOO_URL = "https://twikoochat-production.up.railway.app";

onMounted(() => {
  if (TWIKOO_URL) loadTwikoo();
});

watch(
  () => route.path,
  () => {
    setTimeout(() => {
      loadTwikoo();
    }, 500);
  },
);

function loadTwikoo() {
  if (!TWIKOO_URL) return;
  commentEl.value.innerHTML = "";

  // 加载 Twikoo 脚本
  const script = document.createElement("script");
  script.src = "https://unpkg.com/twikoo@1.7.13/dist/twikoo.all.min.js";
  script.onload = () => {
    if (window.twikoo) {
      window.twikoo.init({
        el: commentEl.value,
        envId: TWIKOO_URL,
        lang: "zh-CN",
        path: route.path,
      });
    }
  };
  document.head.appendChild(script);
}
</script>

<template>
  <div class="comment-container">
    <h2 class="comment-title">💬 评论区</h2>

    <!-- 未配置时显示引导 -->
    <div v-if="!TWIKOO_URL" class="comment-setup-guide">
      <div class="setup-card">
        <h3>🚀 部署评论后端（免费）</h3>
        <p>评论使用 Twikoo，支持 <strong>QQ / 微信 / 邮箱 / GitHub</strong> 登录，国内网络友好。</p>
        <p>只需一步：</p>
        <ol>
          <li>
            打开
            <a
              href="https://vercel.com/import/project?template=https://github.com/imaegoo/twikoo/tree/main/vercel-template"
              target="_blank"
            >
              Twikoo Vercel 模板
            </a>
            ，用 GitHub 账号登录 Vercel 并一键部署
          </li>
          <li>部署成功后拿到域名 <code>xxx.vercel.app</code></li>
          <li>
            编辑 <code>docs/.vuepress/components/Comment.vue</code>， 把第 8 行的
            <code>TWIKOO_URL</code> 填成你的 Vercel 地址
          </li>
        </ol>
        <p class="setup-tip">💡 部署完重启 dev server 即可生效</p>
      </div>
    </div>

    <!-- Twikoo 评论区容器 -->
    <div ref="commentEl" class="twikoo-wrapper"></div>
  </div>
</template>

<style scoped>
.comment-container {
  max-width: 800px;
  margin: 2rem auto;
  padding: 0 1rem;
}

.comment-title {
  font-size: 1.3rem;
  font-weight: 700;
  color: var(--c-text-dark);
  padding-bottom: 0.5rem;
  border-bottom: 2px solid var(--c-border);
  margin-bottom: 1.2rem;
}

.comment-setup-guide {
  text-align: center;
}

.setup-card {
  background: var(--c-bg-lighter);
  border: 1px solid var(--c-border);
  border-radius: var(--radius-md, 10px);
  padding: 1.5rem;
  text-align: left;
  box-shadow: var(--shadow-card, 0 1px 3px rgba(0, 0, 0, 0.06));
}

.setup-card h3 {
  margin-top: 0;
  font-size: 1.1rem;
}

.setup-card ol {
  padding-left: 1.2rem;
  line-height: 2;
}

.setup-card a {
  color: var(--c-brand);
  font-weight: 600;
}

.setup-card code {
  background: rgba(99, 102, 241, 0.08);
  padding: 0.15rem 0.4rem;
  border-radius: 4px;
  font-size: 0.85em;
}

.setup-tip {
  margin-bottom: 0;
  font-size: 0.9rem;
  color: var(--c-text-lighter);
}

.twikoo-wrapper {
  min-height: 200px;
}
</style>

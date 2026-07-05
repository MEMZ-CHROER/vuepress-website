<script setup>
import { ref, onMounted, watch } from 'vue'
import { useRoute } from 'vuepress/client'

const route = useRoute()
const commentEl = ref(null)

// Giscus 配置 — ✏️ 按下方说明替换 repoId 和 categoryId
const GISCUS_CONFIG = {
  repo: 'MEMZ-CHROER/vuepress-website',
  repoId: '',
  category: 'Announcements',
  categoryId: '',
  mapping: 'pathname',
  strict: '1',
  reactionsEnabled: '1',
  emitMetadata: '0',
  inputPosition: 'top',
  lang: 'zh-CN',
  theme: 'light',
}

function loadGiscus() {
  if (!GISCUS_CONFIG.repoId || !GISCUS_CONFIG.categoryId) return

  // 清除旧的 giscus 脚本
  commentEl.value.innerHTML = ''

  const script = document.createElement('script')
  script.src = 'https://giscus.app/client.js'
  script.setAttribute('data-repo', GISCUS_CONFIG.repo)
  script.setAttribute('data-repo-id', GISCUS_CONFIG.repoId)
  script.setAttribute('data-category', GISCUS_CONFIG.category)
  script.setAttribute('data-category-id', GISCUS_CONFIG.categoryId)
  script.setAttribute('data-mapping', GISCUS_CONFIG.mapping)
  script.setAttribute('data-strict', GISCUS_CONFIG.strict)
  script.setAttribute('data-reactions-enabled', GISCUS_CONFIG.reactionsEnabled)
  script.setAttribute('data-emit-metadata', GISCUS_CONFIG.emitMetadata)
  script.setAttribute('data-input-position', GISCUS_CONFIG.inputPosition)
  script.setAttribute('data-lang', GISCUS_CONFIG.lang)
  script.setAttribute('data-theme', GISCUS_CONFIG.theme)
  script.setAttribute('crossorigin', 'anonymous')
  script.async = true
  commentEl.value.appendChild(script)
}

onMounted(() => {
  if (GISCUS_CONFIG.repoId && GISCUS_CONFIG.categoryId) {
    loadGiscus()
  }
})

// 路由变化时重新加载
watch(() => route.path, () => {
  setTimeout(loadGiscus, 300)
})
</script>

<template>
  <div class="comment-container">
    <h2 class="comment-title">💬 评论区</h2>

    <!-- 未配置时显示引导 -->
    <div v-if="!GISCUS_CONFIG.repoId || !GISCUS_CONFIG.categoryId" class="comment-setup-guide">
      <div class="setup-card">
        <h3>⚙️ 评论系统待配置</h3>
        <p>评论功能基于 Giscus（GitHub Discussions），需要两步配置：</p>
        <ol>
          <li>
            <strong>开启 Discussions</strong> — 前往
            <a :href="`https://github.com/${GISCUS_CONFIG.repo}/settings`" target="_blank">
              仓库 Settings
            </a>
            ，勾选 Discussions
          </li>
          <li>
            <strong>安装 Giscus App</strong> — 打开
            <a href="https://giscus.app/zh-CN" target="_blank">giscus.app</a>
            ，授权仓库后获取 <code>repoId</code> 和 <code>categoryId</code>
          </li>
          <li>
            <strong>填入组件</strong> — 编辑
            <code>docs/.vuepress/components/Comment.vue</code>，
            把 <code>repoId</code> 和 <code>categoryId</code> 填到第 9~10 行
          </li>
        </ol>
        <p class="setup-tip">💡 配置好后重启 dev server 即可生效</p>
      </div>
    </div>

    <!-- Giscus 评论区 -->
    <div ref="commentEl" class="giscus-wrapper"></div>
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
  line-height: 1.8;
}

.setup-card a {
  color: var(--c-brand);
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

.giscus-wrapper {
  min-height: 200px;
}
</style>

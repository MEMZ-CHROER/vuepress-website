<script setup>
import { ref, onMounted, computed } from "vue";
import { usePageData } from "vuepress/client";

const API_BASE = "https://blog-api.liuxiyu.cn";
const REPO = "MEMZ-CHROER/vuepress-website";
const BRANCH = "master";

const page = usePageData();

// 当前页面对应的 md 路径（VuePress 提供源文件相对路径）
const mdPath = computed(() => {
  const f = page.value.filePathRelative || "";
  return f ? f : null;
});

// 状态
const showBtn = ref(false);       // 是否显示编辑按钮（登录后）
const showLogin = ref(false);     // 登录弹窗
const showEditor = ref(false);    // 编辑弹窗
const loginUser = ref("");
const loginPass = ref("");
const loginError = ref("");
const busy = ref(false);
const session = ref(null);
const editorContent = ref("");
const editorTitle = ref("");
const editorPreview = ref("");
const editorStatus = ref("");
const loadingContent = ref(false);

const hasPerm = (p) =>
  session.value &&
  (session.value.permissions === "all" ||
    (session.value.permissions || "").indexOf(p) > -1);

// ---- 登录 ----
function openLogin() { showLogin.value = true; showEditor.value = false; }
function closeLogin() { showLogin.value = false; loginError.value = ""; }

async function doLogin() {
  loginError.value = "";
  if (!loginUser.value || !loginPass.value) { loginError.value = "请输入用户名和密码"; return; }
  busy.value = true;
  try {
    const r = await fetch(API_BASE + "/api/auth/login", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ username: loginUser.value, password: loginPass.value }),
    });
    const d = await r.json();
    if (d.token) {
      session.value = d;
      localStorage.setItem("lxy_front_session", JSON.stringify(d));
      showLogin.value = false;
      loginUser.value = ""; loginPass.value = "";
    } else {
      loginError.value = d.error || "登录失败";
    }
  } catch (e) {
    loginError.value = "网络错误: " + e.message;
  }
  busy.value = false;
}

function logout() {
  session.value = null;
  localStorage.removeItem("lxy_front_session");
  showEditor.value = false;
}

// ---- 编辑 ----
function openEditor() {
  if (!session.value) { openLogin(); return; }
  if (!mdPath.value) { alert("此页面没有对应的可编辑源文件"); return; }
  showEditor.value = true;
  loadContent();
}

function closeEditor() { showEditor.value = false; editorStatus.value = ""; }

// 读当前页 md 内容
async function loadContent() {
  loadingContent.value = true;
  editorStatus.value = "加载中...";
  try {
    const r = await fetch(API_BASE + "/raw/repos/" + REPO + "/contents/" + mdPath.value + "?ref=" + BRANCH);
    if (!r.ok) throw new Error("HTTP " + r.status);
    const text = await r.text();
    const parsed = parseFrontmatter(text);
    editorTitle.value = parsed.meta.title || "";
    editorContent.value = parsed.body;
    editorStatus.value = "";
  } catch (e) {
    editorStatus.value = "读取失败: " + e.message;
  }
  loadingContent.value = false;
}

// frontmatter 解析
function parseFrontmatter(raw) {
  const m = String(raw || "").match(/^---\n([\s\S]*?)\n---\n?([\s\S]*)$/);
  if (!m) return { meta: {}, body: raw || "" };
  const meta = {};
  m[1].split("\n").forEach((line) => {
    const kv = line.match(/^\s*(\w+)\s*:\s*(.+)$/);
    if (kv) meta[kv[1]] = kv[2].trim().replace(/^["']|["']$/g, "");
  });
  return { meta, body: (m[2] || "").replace(/^\n/, "") };
}

function buildFrontmatter(meta, body) {
  let fm = "---\n";
  if (meta.title) fm += "title: " + meta.title + "\n";
  // 保留原始 frontmatter 里的其他字段（简单起见保留原 meta 全部）
  Object.keys(meta).forEach((k) => {
    if (k !== "title") fm += k + ": " + meta[k] + "\n";
  });
  fm += "---\n\n" + (body || "").trimStart();
  return fm;
}

function b64enc(s) { return btoa(unescape(encodeURIComponent(s))); }

// 保存到 GitHub（复用 admin 的 worker 代理逻辑）
async function saveContent() {
  if (!session.value) return;
  editorStatus.value = "保存中...";
  // 重新读取原文件的 sha（GitHub 更新需要）
  try {
    const metaRes = await fetch(API_BASE + "/api/repos/" + REPO + "/contents/" + mdPath.value + "?ref=" + BRANCH, {
      headers: { Accept: "application/vnd.github.v3+json" },
    });
    const meta = await metaRes.json();
    const full = buildFrontmatter({ ...parseFrontmatter(editorContent.value).meta, title: editorTitle.value }, editorContent.value);
    const body = {
      message: "docs: 前台编辑 " + mdPath.value,
      content: b64enc(full),
      branch: BRANCH,
      sha: meta.sha,
      _token: session.value.token,
    };
    const r = await fetch(API_BASE + "/api/repos/" + REPO + "/contents/" + mdPath.value, {
      method: "PUT",
      headers: { "Content-Type": "application/json", Accept: "application/vnd.github.v3+json" },
      body: JSON.stringify(body),
    });
    const d = await r.json();
    if (d.content) {
      editorStatus.value = "✅ 已保存，构建部署中（1-2 分钟生效）";
    } else {
      editorStatus.value = "❌ 保存失败: " + (d.message || "未知错误");
    }
  } catch (e) {
    editorStatus.value = "❌ 保存失败: " + e.message;
  }
}

// 预览渲染（简单 Markdown）
function updatePreview() {
  const md = editorContent.value || "";
  const esc = (s) => s.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;");
  let html = "";
  md.split("\n").forEach((line) => {
    const l = esc(line);
    if (/^#\s/.test(l)) html += "<h1>" + l.replace(/^#\s/, "") + "</h1>";
    else if (/^##\s/.test(l)) html += "<h2>" + l.replace(/^##\s/, "") + "</h2>";
    else if (/^###\s/.test(l)) html += "<h3>" + l.replace(/^###\s/, "") + "</h3>";
    else if (/^-\s/.test(l)) html += "<li>" + l.replace(/^-\s/, "") + "</li>";
    else if (l.trim() === "") html += "";
    else html += "<p>" + l + "</p>";
  });
  editorPreview.value = html;
}

onMounted(() => {
  // 恢复会话
  try {
    const s = localStorage.getItem("lxy_front_session");
    if (s) session.value = JSON.parse(s);
  } catch (e) {}
  showBtn.value = true;
});
</script>

<template>
  <div class="front-editor">
    <!-- 右下角悬浮按钮 -->
    <button v-if="showBtn" class="fe-fab" @click="session ? openEditor() : openLogin()" :title="session ? '编辑此页' : '登录后编辑'">
      {{ session ? "✏️" : "🔐" }}
    </button>

    <!-- 登录弹窗 -->
    <div v-if="showLogin" class="fe-overlay" @click.self="closeLogin">
      <div class="fe-modal fe-login">
        <h3>🔐 登录以编辑</h3>
        <input v-model="loginUser" type="text" placeholder="用户名" @keyup.enter="doLogin" />
        <input v-model="loginPass" type="password" placeholder="密码" @keyup.enter="doLogin" />
        <p v-if="loginError" class="fe-err">{{ loginError }}</p>
        <div class="fe-btns">
          <button class="fe-btn" @click="doLogin" :disabled="busy">{{ busy ? "登录中..." : "登录" }}</button>
          <button class="fe-btn fe-btn-ghost" @click="closeLogin">取消</button>
        </div>
        <p v-if="session" class="fe-dim">已登录: {{ session.username }}</p>
      </div>
    </div>

    <!-- 编辑弹窗 -->
    <div v-if="showEditor" class="fe-overlay" @click.self="closeEditor">
      <div class="fe-modal fe-editor">
        <h3>✏️ 编辑: {{ mdPath }}</h3>
        <label class="fe-label">标题</label>
        <input v-model="editorTitle" class="fe-title" placeholder="文章标题" />
        <div class="fe-panes">
          <textarea v-model="editorContent" @input="updatePreview" placeholder="Markdown 内容..." spellcheck="false"></textarea>
          <div class="fe-preview" v-html="editorPreview"></div>
        </div>
        <div class="fe-status">{{ loadingContent ? "加载中..." : editorStatus }}</div>
        <div class="fe-btns">
          <button class="fe-btn" @click="saveContent">💾 保存</button>
          <button class="fe-btn fe-btn-ghost" @click="closeEditor">关闭</button>
          <button v-if="session" class="fe-btn fe-btn-red" @click="logout">退出登录</button>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.front-editor { --fe-accent: #6366f1; }
.fe-fab {
  position: fixed; right: 1.2rem; bottom: 1.2rem; z-index: 200;
  width: 48px; height: 48px; border-radius: 50%; border: none;
  background: var(--fe-accent); color: #fff; font-size: 1.3rem;
  cursor: pointer; box-shadow: 0 4px 16px rgba(99,102,241,.4);
  transition: transform .2s;
}
.fe-fab:hover { transform: scale(1.1); }
.fe-overlay {
  position: fixed; inset: 0; background: rgba(0,0,0,.6); z-index: 300;
  display: flex; align-items: center; justify-content: center; padding: 16px;
}
.fe-modal {
  background: var(--c-bg-lighter, #1a1a28); color: var(--c-text, #c8c8d0);
  border: 1px solid var(--c-border, #333); border-radius: 12px;
  padding: 20px; max-width: 900px; width: 100%;
}
.fe-login { max-width: 360px; }
.fe-modal h3 { margin: 0 0 12px; color: var(--fe-accent); }
.fe-modal input {
  width: 100%; box-sizing: border-box; padding: 8px 10px; margin: 6px 0;
  border: 1px solid var(--c-border, #333); border-radius: 8px;
  background: var(--c-bg, #111); color: var(--c-text, #fff);
}
.fe-title { font-size: 15px; }
.fe-panes { display: flex; gap: 10px; margin-top: 8px; }
.fe-panes textarea, .fe-preview {
  width: 50%; min-height: 50vh; box-sizing: border-box; padding: 10px;
  border: 1px solid var(--c-border, #333); border-radius: 8px;
  background: var(--c-bg, #111); color: var(--c-text, #c8c8d0);
  font: 13px/1.6 monospace;
}
.fe-preview { overflow-y: auto; }
.fe-preview h1, .fe-preview h2, .fe-preview h3 { color: var(--fe-accent); }
.fe-preview a { color: #1e90ff; }
.fe-preview code { background: var(--c-bg-light, #1a1a28); padding: 2px 5px; border-radius: 3px; }
.fe-status { font-size: 12px; opacity: .7; min-height: 18px; padding: 6px 0; }
.fe-btns { display: flex; gap: 8px; margin-top: 6px; }
.fe-btn {
  padding: 8px 18px; border: 1px solid var(--fe-accent); border-radius: 8px;
  background: var(--fe-accent); color: #fff; cursor: pointer; font-weight: 600;
}
.fe-btn:hover { opacity: .88; }
.fe-btn-ghost { background: transparent; color: var(--c-text, #c8c8d0); }
.fe-btn-red { background: #e74c3c; border-color: #e74c3c; }
.fe-err { color: #e74c3c; font-size: 13px; }
.fe-label { display: block; font-size: 12px; opacity: .7; margin: 8px 0 2px; }
.fe-dim { font-size: 12px; opacity: .6; margin-top: 8px; }
@media (max-width: 700px) {
  .fe-panes { flex-direction: column; }
  .fe-panes textarea, .fe-preview { width: 100%; min-height: 30vh; }
}
</style>

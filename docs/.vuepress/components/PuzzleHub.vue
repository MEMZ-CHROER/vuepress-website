<script setup>
import { computed, onMounted, ref } from "vue";

/**
 * PROJECT 666 — 谜题中枢
 *
 * 答案只以 FNV-1a 哈希形式存在，避免 view-source 直接看穿。
 * 进度存 localStorage，十关顺序解锁。
 */
const SALT = "lxy:666:";

const LEVELS = [
  {
    id: 1,
    name: "CREDITS",
    title: "第一片 · 致谢页的低语",
    hint: "去 /credits/。把那句副标题里的 1 和 0 按顺序抠出来，按位取反，再转成十进制。",
    hashes: ["5b9db524"],
    reward: "第一片记忆回收。它是一个两位数，来自一次按位取反。",
  },
  {
    id: 2,
    name: "ABOUT",
    title: "第二片 · 被光吞掉的字",
    hint: "去 /about/。页面上有一行字和背景融为一体；选中它，或直接看源码。它是 Base64。",
    hashes: ["cea2e75b"],
    reward: "第二片记忆回收。它同样是一个两位数。两片放在一起会发生什么？",
  },
  {
    id: 3,
    name: "404",
    title: "第三片 · 迷路坦克的留言",
    hint: "随便打开一个不存在的地址，看 404 页的 HTML 源码。那里有一段摩斯密码。",
    hashes: ["6334c37b"],
    reward: "第三片记忆回收。回声会带你去 /webSearch/。",
  },
  {
    id: 4,
    name: "FOURIER",
    title: "第四片 · 频率里的字",
    hint: "去 /webSearch/。播放那段信号，看频谱图——或者下载 WAV 用 Audacity 的频谱视图打开。",
    hashes: ["568a1ce2", "813e4061", "e623a1ef", "f66d81d5"],
    reward: "第四片记忆回收。图片也会说话，去看看 /puzzle/echo.png 的最低有效位。",
  },
  {
    id: 5,
    name: "LSB",
    title: "第五片 · 图片的最低处",
    hint: "下载 /puzzle/echo.png，用 LSB 隐写工具（zsteg / StegSolve / Python PIL）读 RGB 最低位。",
    hashes: ["5c950f55"],
    reward: "第五片记忆回收。机器人也在看这份地图：/robots.txt。",
  },
  {
    id: 6,
    name: "ROBOTS",
    title: "第六片 · 给爬虫的密信",
    hint: "打开 /robots.txt。注释里有一段 Base64。",
    hashes: ["cb9de5de"],
    reward: "第六片记忆回收。工具页的 <head> 里还有一句话。",
  },
  {
    id: 7,
    name: "META",
    title: "第七片 · 页头的自我介绍",
    hint: "打开 /tools/ 的网页源码，看 <head> 里的 <meta name=\"x-pzl\">。",
    hashes: ["0eaf2e07"],
    reward: "第七片记忆回收。友情链接页藏着一首只能选中才能读的诗。",
  },
  {
    id: 8,
    name: "ACROSTIC",
    title: "第八片 · 藏头的诗",
    hint: "去 /friends/，选中页面下方那片空白。把每行第一个字母连起来。",
    hashes: ["30cde5b6", "b920e77f"],
    reward: "第八片记忆回收。诗的最后一行提到了第九扇门。",
  },
  {
    id: 9,
    name: "URL DOOR",
    title: "第九片 · 参数里的门",
    hint: "在首页地址后面接上 ?pzl=door9，回车。",
    hashes: ["ffcb4806"],
    reward: "第九片记忆回收。最后一扇门在时间里。",
  },
  {
    id: 10,
    name: "COMMIT",
    title: "第十片 · 时间里的签名",
    hint: "去 GitHub 找到标题含 PROJECT 666 的那次提交，把正文每一行第一个字母连起来。",
    hashes: ["dd69bac7"],
    reward: "十片记忆全部回收。现在，把第一片和第二片相乘。",
  },
];

const FINAL_HASH = "91f0afb0"; // 666

const solved = ref([]);
const input = ref("");
const error = ref("");
const flash = ref("");
const finalInput = ref("");
const finalError = ref("");
const vaultOpen = ref(false);

const activeIndex = computed(() => {
  for (let i = 0; i < LEVELS.length; i++) if (!solved.value.includes(i)) return i;
  return LEVELS.length;
});

const allSolved = computed(() => solved.value.length === LEVELS.length);
const progress = computed(() => Math.round((solved.value.length / LEVELS.length) * 100));

function fnv1a(str) {
  let x = 0x811c9dc5;
  for (let i = 0; i < str.length; i++) {
    x ^= str.charCodeAt(i);
    x = Math.imul(x, 0x01000193) >>> 0;
  }
  return x.toString(16).padStart(8, "0");
}

function normalize(str) {
  return String(str || "")
    .trim()
    .toUpperCase()
    .replace(/[\s\-_.,，。:：;；'"`]/g, "");
}

function save() {
  try {
    localStorage.setItem("lxy_pzl_solved", JSON.stringify(solved.value));
  } catch {
    /* ignore */
  }
}

function submit() {
  const idx = activeIndex.value;
  if (idx >= LEVELS.length) return;
  error.value = "";
  const value = normalize(input.value);
  if (!value) {
    error.value = "信号为空。";
    return;
  }
  const level = LEVELS[idx];
  if (level.hashes.includes(fnv1a(SALT + value))) {
    solved.value = [...solved.value, idx];
    input.value = "";
    flash.value = `✓ ${level.name} 已解锁`;
    setTimeout(() => (flash.value = ""), 2200);
    save();
  } else {
    error.value = "校验失败。再听听那段信号。";
  }
}

function submitFinal() {
  finalError.value = "";
  const value = normalize(finalInput.value);
  if (fnv1a(SALT + value) === FINAL_HASH) {
    vaultOpen.value = true;
    try {
      localStorage.setItem("lxy_vault_unlocked", "1");
    } catch {
      /* ignore */
    }
    finalInput.value = "";
  } else {
    finalError.value = "主密钥错误。十片记忆里应该藏着它的算法。";
  }
}

function reset() {
  if (!window.confirm("重置全部谜题进度？")) return;
  solved.value = [];
  input.value = "";
  error.value = "";
  finalInput.value = "";
  finalError.value = "";
  vaultOpen.value = false;
  try {
    localStorage.removeItem("lxy_pzl_solved");
    localStorage.removeItem("lxy_vault_unlocked");
  } catch {
    /* ignore */
  }
}

onMounted(() => {
  try {
    const raw = localStorage.getItem("lxy_pzl_solved");
    if (raw) {
      const parsed = JSON.parse(raw);
      if (Array.isArray(parsed)) {
        solved.value = parsed.filter(
          (n) => Number.isInteger(n) && n >= 0 && n < LEVELS.length,
        );
      }
    }
    vaultOpen.value = localStorage.getItem("lxy_vault_unlocked") === "1";
  } catch {
    /* ignore */
  }
});
</script>

<template>
  <div class="pzl-wrap">
    <div class="pzl-grid"></div>

    <header class="pzl-head">
      <div class="pzl-kicker">// ARCHIVE 0x666</div>
      <h1>PROJECT <span>666</span></h1>
      <p class="pzl-sub">
        十片记忆散落在站点各处。每一片都是一种不同的藏法。<br />
        全部回收之后，用第一片和第二片算出主密钥。
      </p>
      <div class="pzl-bar">
        <div class="pzl-bar-fill" :style="{ width: progress + '%' }"></div>
      </div>
      <div class="pzl-meta">
        <span>RECOVERED {{ solved.length }} / {{ LEVELS.length }}</span>
        <span>{{ progress }}%</span>
      </div>
    </header>

    <Transition name="flash">
      <div v-if="flash" class="pzl-flash">{{ flash }}</div>
    </Transition>

    <ol class="pzl-list">
      <li
        v-for="(level, i) in LEVELS"
        :key="level.id"
        class="pzl-item"
        :class="{
          done: solved.includes(i),
          active: i === activeIndex,
          locked: i > activeIndex,
        }"
      >
        <div class="pzl-num">{{ String(level.id).padStart(2, "0") }}</div>

        <div class="pzl-body">
          <div class="pzl-row">
            <span class="pzl-name">{{ solved.includes(i) || i <= activeIndex ? level.name : "???" }}</span>
            <span class="pzl-state">
              <template v-if="solved.includes(i)">✓ RECOVERED</template>
              <template v-else-if="i === activeIndex">▶ AWAITING SIGNAL</template>
              <template v-else>🔒 LOCKED</template>
            </span>
          </div>

          <div v-if="solved.includes(i)" class="pzl-reward">
            {{ level.reward }}
          </div>

          <div v-if="i === activeIndex" class="pzl-active">
            <p class="pzl-hint">{{ level.hint }}</p>
            <form class="pzl-form" @submit.prevent="submit">
              <input
                v-model="input"
                type="text"
                autocomplete="off"
                spellcheck="false"
                placeholder="输入这一片的答案…"
                aria-label="谜题答案"
              />
              <button type="submit">校验</button>
            </form>
            <p v-if="error" class="pzl-error">{{ error }}</p>
          </div>
        </div>
      </li>
    </ol>

    <section v-if="allSolved" class="pzl-final">
      <div class="pzl-final-title">// MASTER KEY</div>
      <p>
        十片记忆已经归位。第一片是 <b class="pzl-gold">37</b>，第二片是
        <b class="pzl-gold">18</b>。<br />
        它们之间只差一个运算符。
      </p>
      <form class="pzl-form pzl-form-final" @submit.prevent="submitFinal">
        <input
          v-model="finalInput"
          type="text"
          inputmode="numeric"
          autocomplete="off"
          spellcheck="false"
          placeholder="主密钥"
          aria-label="主密钥"
        />
        <button type="submit">打开金库</button>
      </form>
      <p v-if="finalError" class="pzl-error">{{ finalError }}</p>
      <div v-if="vaultOpen" class="pzl-open">
        <span>ACCESS GRANTED</span>
        <a href="/vault/">进入金库 →</a>
      </div>
    </section>

    <footer class="pzl-foot">
      <button class="pzl-reset" @click="reset">重置进度</button>
      <span>// 答案不区分大小写，忽略空格与标点</span>
    </footer>
  </div>
</template>

<style scoped>
.pzl-wrap {
  position: relative;
  max-width: 860px;
  margin: 0 auto;
  padding: 2.5rem 1.2rem 4rem;
  color: #c7d2fe;
  font-family: "JetBrains Mono", "Fira Code", ui-monospace, monospace;
}
.pzl-grid {
  position: fixed;
  inset: 0;
  z-index: -1;
  pointer-events: none;
  background:
    radial-gradient(circle at 20% 0%, rgba(99, 102, 241, 0.16), transparent 45%),
    radial-gradient(circle at 80% 100%, rgba(168, 85, 247, 0.12), transparent 45%),
    linear-gradient(135deg, #070713 0%, #0c0c22 55%, #080814 100%);
}
.pzl-grid::after {
  content: "";
  position: absolute;
  inset: 0;
  background-image:
    linear-gradient(rgba(99, 102, 241, 0.06) 1px, transparent 1px),
    linear-gradient(90deg, rgba(99, 102, 241, 0.06) 1px, transparent 1px);
  background-size: 42px 42px;
  mask-image: radial-gradient(circle at 50% 30%, #000 10%, transparent 75%);
}
.pzl-head {
  text-align: left;
  border: 1px solid rgba(99, 102, 241, 0.3);
  border-radius: 16px;
  padding: 1.6rem 1.6rem 1.2rem;
  background: rgba(10, 10, 28, 0.72);
  box-shadow:
    0 0 0 1px rgba(0, 0, 0, 0.4) inset,
    0 20px 60px rgba(0, 0, 0, 0.35);
  backdrop-filter: blur(8px);
}
.pzl-kicker {
  font-size: 0.72rem;
  letter-spacing: 0.28em;
  color: #6366f1;
  text-transform: uppercase;
}
.pzl-head h1 {
  margin: 0.35rem 0 0.5rem;
  font-size: clamp(2rem, 7vw, 3.4rem);
  font-weight: 900;
  letter-spacing: 0.04em;
  color: #eef2ff;
  line-height: 1.05;
}
.pzl-head h1 span {
  background: linear-gradient(135deg, #6366f1, #a855f7, #ec4899);
  -webkit-background-clip: text;
  background-clip: text;
  -webkit-text-fill-color: transparent;
}
.pzl-sub {
  margin: 0 0 1.1rem;
  font-size: 0.86rem;
  line-height: 1.75;
  color: #94a3b8;
}
.pzl-bar {
  height: 6px;
  border-radius: 999px;
  background: rgba(99, 102, 241, 0.15);
  overflow: hidden;
}
.pzl-bar-fill {
  height: 100%;
  border-radius: 999px;
  background: linear-gradient(90deg, #6366f1, #a855f7, #ec4899);
  box-shadow: 0 0 14px rgba(99, 102, 241, 0.8);
  transition: width 0.5s cubic-bezier(0.4, 0, 0.2, 1);
}
.pzl-meta {
  display: flex;
  justify-content: space-between;
  margin-top: 0.5rem;
  font-size: 0.7rem;
  letter-spacing: 0.14em;
  color: #64748b;
}
.pzl-list {
  list-style: none;
  margin: 1.6rem 0 0;
  padding: 0;
  display: grid;
  gap: 0.7rem;
}
.pzl-item {
  display: flex;
  gap: 1rem;
  padding: 1rem 1.1rem;
  border: 1px solid rgba(99, 102, 241, 0.16);
  border-radius: 14px;
  background: rgba(12, 12, 32, 0.66);
  transition: all 0.3s ease;
}
.pzl-item.locked {
  opacity: 0.4;
  filter: grayscale(0.5);
}
.pzl-item.active {
  border-color: rgba(99, 102, 241, 0.75);
  box-shadow:
    0 0 0 1px rgba(99, 102, 241, 0.25),
    0 0 34px rgba(99, 102, 241, 0.18);
  background: rgba(20, 20, 52, 0.8);
}
.pzl-item.done {
  border-color: rgba(34, 197, 94, 0.35);
  background: rgba(8, 24, 20, 0.55);
}
.pzl-num {
  flex: 0 0 auto;
  width: 2.2rem;
  height: 2.2rem;
  display: grid;
  place-items: center;
  border-radius: 9px;
  border: 1px solid rgba(99, 102, 241, 0.35);
  font-size: 0.78rem;
  font-weight: 700;
  color: #a5b4fc;
  background: rgba(99, 102, 241, 0.1);
}
.pzl-item.done .pzl-num {
  border-color: rgba(34, 197, 94, 0.5);
  color: #4ade80;
  background: rgba(34, 197, 94, 0.1);
}
.pzl-body {
  flex: 1;
  min-width: 0;
}
.pzl-row {
  display: flex;
  align-items: baseline;
  justify-content: space-between;
  gap: 0.8rem;
  flex-wrap: wrap;
}
.pzl-name {
  font-size: 0.92rem;
  font-weight: 700;
  letter-spacing: 0.08em;
  color: #e0e7ff;
}
.pzl-state {
  font-size: 0.64rem;
  letter-spacing: 0.16em;
  color: #64748b;
}
.pzl-item.active .pzl-state {
  color: #818cf8;
}
.pzl-item.done .pzl-state {
  color: #4ade80;
}
.pzl-reward {
  margin-top: 0.45rem;
  font-size: 0.82rem;
  line-height: 1.7;
  color: #86efac;
}
.pzl-active {
  margin-top: 0.7rem;
}
.pzl-hint {
  margin: 0 0 0.6rem;
  font-size: 0.82rem;
  line-height: 1.75;
  color: #cbd5e1;
}
.pzl-form {
  display: flex;
  gap: 0.5rem;
}
.pzl-form input {
  flex: 1;
  min-width: 0;
  padding: 0.6rem 0.8rem;
  border-radius: 9px;
  border: 1px solid rgba(99, 102, 241, 0.35);
  background: rgba(5, 5, 18, 0.85);
  color: #e0e7ff;
  font-family: inherit;
  font-size: 0.86rem;
  outline: none;
  transition: border-color 0.2s ease, box-shadow 0.2s ease;
}
.pzl-form input:focus {
  border-color: #6366f1;
  box-shadow: 0 0 0 3px rgba(99, 102, 241, 0.16);
}
.pzl-form button {
  flex: 0 0 auto;
  padding: 0.6rem 1.1rem;
  border-radius: 9px;
  border: 1px solid rgba(99, 102, 241, 0.5);
  background: linear-gradient(135deg, rgba(99, 102, 241, 0.28), rgba(168, 85, 247, 0.24));
  color: #e0e7ff;
  font-family: inherit;
  font-size: 0.82rem;
  font-weight: 700;
  letter-spacing: 0.08em;
  cursor: pointer;
  transition: all 0.2s ease;
}
.pzl-form button:hover {
  border-color: #818cf8;
  box-shadow: 0 0 16px rgba(99, 102, 241, 0.35);
  transform: translateY(-1px);
}
.pzl-error {
  margin: 0.5rem 0 0;
  font-size: 0.78rem;
  color: #f87171;
}
.pzl-final {
  margin-top: 1.8rem;
  padding: 1.6rem;
  border-radius: 16px;
  border: 1px solid rgba(212, 175, 55, 0.45);
  background:
    radial-gradient(circle at 50% 0%, rgba(212, 175, 55, 0.12), transparent 65%),
    rgba(14, 12, 4, 0.75);
  box-shadow: 0 0 40px rgba(212, 175, 55, 0.12);
}
.pzl-final-title {
  font-size: 0.72rem;
  letter-spacing: 0.28em;
  color: #d4af37;
}
.pzl-final p {
  font-size: 0.86rem;
  line-height: 1.8;
  color: #e7e5e4;
}
.pzl-gold {
  color: #f0d060;
}
.pzl-form-final {
  margin-top: 0.8rem;
}
.pzl-open {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 1rem;
  flex-wrap: wrap;
  margin-top: 1rem;
  padding: 0.8rem 1rem;
  border-radius: 10px;
  border: 1px solid rgba(34, 197, 94, 0.4);
  background: rgba(34, 197, 94, 0.08);
  font-size: 0.8rem;
  letter-spacing: 0.14em;
  color: #4ade80;
}
.pzl-open a {
  color: #f0d060;
  font-weight: 700;
  text-decoration: none;
}
.pzl-open a:hover {
  text-decoration: underline;
}
.pzl-foot {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 1rem;
  flex-wrap: wrap;
  margin-top: 1.6rem;
  font-size: 0.68rem;
  letter-spacing: 0.08em;
  color: #475569;
}
.pzl-reset {
  padding: 0.35rem 0.8rem;
  border-radius: 999px;
  border: 1px solid rgba(100, 116, 139, 0.35);
  background: transparent;
  color: #64748b;
  font-family: inherit;
  font-size: 0.68rem;
  cursor: pointer;
}
.pzl-reset:hover {
  color: #f87171;
  border-color: rgba(248, 113, 113, 0.5);
}
.pzl-flash {
  position: fixed;
  top: 84px;
  left: 50%;
  transform: translateX(-50%);
  z-index: 80;
  padding: 0.55rem 1.1rem;
  border-radius: 999px;
  border: 1px solid rgba(34, 197, 94, 0.5);
  background: rgba(6, 24, 16, 0.92);
  color: #4ade80;
  font-size: 0.78rem;
  letter-spacing: 0.1em;
  box-shadow: 0 0 26px rgba(34, 197, 94, 0.3);
}
.flash-enter-active,
.flash-leave-active {
  transition: all 0.25s ease;
}
.flash-enter-from,
.flash-leave-to {
  opacity: 0;
  transform: translate(-50%, -8px);
}
@media (max-width: 640px) {
  .pzl-wrap {
    padding: 1.6rem 0.7rem 3rem;
  }
  .pzl-head {
    padding: 1.2rem 1rem;
  }
  .pzl-item {
    padding: 0.85rem 0.8rem;
    gap: 0.7rem;
  }
  .pzl-row {
    flex-direction: column;
    gap: 0.2rem;
  }
}
</style>

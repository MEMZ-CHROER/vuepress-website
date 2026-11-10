<script setup>
import { computed, onMounted, ref } from "vue";

/**
 * 金库 / 终末档案 的双重锁。
 *
 * stage="vault"   → 主密钥 666，解锁 SECRET_01~03
 * stage="ending"  → 第二道锁 MEMZ，解锁 TRUE ENDING
 */
const props = defineProps({
  stage: { type: String, default: "vault" },
});

const SALT = "lxy:666:";
const isEnding = computed(() => props.stage === "ending");

const CONFIG = computed(() =>
  isEnding.value
    ? {
        storageKey: "lxy_ending_unlocked",
        hashes: ["f99dc3d7", "e581a434"], // MEMZ / MEMZ-CHROER
        kicker: "// TERMINAL ARCHIVE",
        title: "ENDING",
        subtitle: "第二道锁。答案是 Credits 页 Founder 那一栏里的 GitHub ID。",
        placeholder: "第二道锁的答案",
        errorText: "名字不正确。金库里提到过它。",
      }
    : {
        storageKey: "lxy_vault_unlocked",
        hashes: ["91f0afb0"], // 666
        kicker: "// VAULT 0x666",
        title: "VAULT",
        subtitle: "主密钥。十片记忆的乘积。",
        placeholder: "主密钥",
        errorText: "主密钥错误。37 × 18 = ?",
      },
);

const unlocked = ref(false);
const input = ref("");
const error = ref("");
const gold = ref(false);

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

function submit() {
  error.value = "";
  const value = normalize(input.value);
  if (!value) {
    error.value = "信号为空。";
    return;
  }
  if (CONFIG.value.hashes.includes(fnv1a(SALT + value))) {
    unlocked.value = true;
    input.value = "";
    try {
      localStorage.setItem(CONFIG.value.storageKey, "1");
    } catch {
      /* ignore */
    }
  } else {
    error.value = CONFIG.value.errorText;
  }
}

function toggleGold() {
  gold.value = !gold.value;
  document.documentElement.classList.toggle("pzl-gold", gold.value);
  try {
    localStorage.setItem("lxy_gold", gold.value ? "1" : "0");
  } catch {
    /* ignore */
  }
  window.dispatchEvent(new Event("pzl-gold-change"));
}

onMounted(() => {
  try {
    unlocked.value = localStorage.getItem(CONFIG.value.storageKey) === "1";
    gold.value = localStorage.getItem("lxy_gold") === "1";
  } catch {
    /* ignore */
  }
});
</script>

<template>
  <div class="vault-wrap">
    <div class="vault-grid"></div>

    <!-- 未解锁 -->
    <section v-if="!unlocked" class="vault-lock">
      <div class="vault-kicker">{{ CONFIG.kicker }}</div>
      <h1>{{ CONFIG.title }}</h1>
      <p class="vault-sub">{{ CONFIG.subtitle }}</p>
      <form class="vault-form" @submit.prevent="submit">
        <input
          v-model="input"
          type="text"
          autocomplete="off"
          spellcheck="false"
          :placeholder="CONFIG.placeholder"
          aria-label="密钥"
        />
        <button type="submit">验证</button>
      </form>
      <p v-if="error" class="vault-error">{{ error }}</p>
      <p class="vault-back">
        <a href="/puzzle/">← 回到 PROJECT 666 中枢</a>
      </p>
    </section>

    <!-- 金库已解锁 -->
    <section v-else-if="!isEnding" class="vault-open">
      <div class="vault-kicker">// ACCESS GRANTED</div>
      <h1>金库已开启</h1>
      <p class="vault-sub">
        37 × 18 = 666。它不是魔鬼的数字，<br />
        是 111011010 按位取反后的 37，和 9 位长度两倍的乘积。
      </p>

      <div class="vault-letter">
        <p>给走到这里的你：</p>
        <p>
          这十道门没有奖励你什么实质的东西。它们只是让你在网站的角落里多待了一会儿——
          看源码、听一段频率、读一行 transparent 的字、翻一次提交记录。
        </p>
        <p>
          一个网站最容易被忽略的地方，往往藏着搭站的人最想说的话。
          谢谢你愿意找。
        </p>
        <p class="vault-sign">— Lxy</p>
      </div>

      <div class="vault-cards">
        <article class="vault-card">
          <div class="card-id">SECRET_01</div>
          <h3>黄金模式</h3>
          <p>把全站主色换成金色。666 解锁的第一个后果。</p>
          <button class="card-btn" :class="{ on: gold }" @click="toggleGold">
            {{ gold ? "关闭黄金模式" : "开启黄金模式" }}
          </button>
        </article>

        <article class="vault-card">
          <div class="card-id">SECRET_02</div>
          <h3>终末档案</h3>
          <p>
            还有一道门。答案是本站仓库主人的 GitHub ID——
            Credits 页 Founder 那一栏里出现过的名字（大小写不限，连字符可省略）。
          </p>
          <a class="card-btn" href="/vault/ending/">进入 /vault/ending/ →</a>
        </article>

        <article class="vault-card">
          <div class="card-id">SECRET_03</div>
          <h3>遗留文件</h3>
          <p>一封没有放进任何页面的短信。只有 666 之后才有人告诉你它在哪。</p>
          <a class="card-btn" href="/puzzle/secret.txt" target="_blank" rel="noopener">
            读取 secret.txt →
          </a>
        </article>
      </div>

      <p class="vault-note">
        提示：开启黄金模式后，回到任意页面，左下角的 🪲 会变成 👑。
      </p>
    </section>

    <!-- TRUE ENDING 已解锁 -->
    <section v-else class="vault-open vault-ending">
      <div class="vault-kicker">// TRUE ENDING</div>
      <h1>十片记忆</h1>
      <p class="vault-sub">你找齐了所有答案。这里是完整的花名册。</p>

      <div class="ending-list">
        <div class="ending-row"><span>01</span><b>CREDITS</b><em>37</em></div>
        <div class="ending-row"><span>02</span><b>ABOUT</b><em>18</em></div>
        <div class="ending-row"><span>03</span><b>404</b><em>ECHO</em></div>
        <div class="ending-row"><span>04</span><b>FOURIER</b><em>FOURIER</em></div>
        <div class="ending-row"><span>05</span><b>LSB</b><em>NORTH</em></div>
        <div class="ending-row"><span>06</span><b>ROBOTS</b><em>ROBOT</em></div>
        <div class="ending-row"><span>07</span><b>META</b><em>META</em></div>
        <div class="ending-row"><span>08</span><b>ACROSTIC</b><em>FRIEND</em></div>
        <div class="ending-row"><span>09</span><b>URL DOOR</b><em>DOOR</em></div>
        <div class="ending-row"><span>10</span><b>COMMIT</b><em>COMMIT</em></div>
      </div>

      <div class="ending-formula">
        <span>37</span>
        <span class="op">×</span>
        <span>18</span>
        <span class="op">=</span>
        <span class="result">666</span>
      </div>

      <div class="vault-letter">
        <p>最后一段话：</p>
        <p>
          二进制取反、Base64、摩斯、频谱、最低位、robots、meta、藏头、URL 参数、提交历史——
          十种藏法，十种“你居然真的会看这里”的瞬间。
        </p>
        <p>
          如果你也想在自己的网站里藏点什么，别等一个完美的理由。
          先写下第一行看不见的字。
        </p>
        <p class="vault-sign">— Lxy &amp; Hejuxiao</p>
      </div>

      <div class="ending-actions">
        <a href="/puzzle/">← 回到中枢</a>
        <a href="/credits/">去 Credits</a>
        <a href="/">回首页</a>
      </div>
    </section>
  </div>
</template>

<style scoped>
.vault-wrap {
  position: relative;
  min-height: 70vh;
  max-width: 900px;
  margin: 0 auto;
  padding: 2.5rem 1.2rem 4rem;
  color: #cbd5e1;
  font-family: "JetBrains Mono", "Fira Code", ui-monospace, monospace;
}
.vault-grid {
  position: fixed;
  inset: 0;
  z-index: -1;
  pointer-events: none;
  background:
    radial-gradient(circle at 50% -10%, rgba(212, 175, 55, 0.12), transparent 50%),
    radial-gradient(circle at 20% 100%, rgba(99, 102, 241, 0.1), transparent 50%),
    linear-gradient(135deg, #07070f 0%, #0b0a18 55%, #05050c 100%);
}
.vault-grid::after {
  content: "";
  position: absolute;
  inset: 0;
  background-image:
    linear-gradient(rgba(212, 175, 55, 0.05) 1px, transparent 1px),
    linear-gradient(90deg, rgba(212, 175, 55, 0.05) 1px, transparent 1px);
  background-size: 46px 46px;
  mask-image: radial-gradient(circle at 50% 35%, #000 10%, transparent 75%);
}
.vault-kicker {
  font-size: 0.7rem;
  letter-spacing: 0.3em;
  color: #d4af37;
}
.vault-lock {
  max-width: 560px;
  margin: 6vh auto 0;
  padding: 2.4rem 1.8rem;
  text-align: center;
  border: 1px solid rgba(212, 175, 55, 0.35);
  border-radius: 18px;
  background: rgba(12, 10, 4, 0.7);
  box-shadow: 0 0 60px rgba(212, 175, 55, 0.1);
  backdrop-filter: blur(8px);
}
.vault-lock h1 {
  margin: 0.4rem 0 0.3rem;
  font-size: clamp(2rem, 8vw, 3.2rem);
  font-weight: 900;
  letter-spacing: 0.1em;
  color: #f0d060;
  text-shadow: 0 0 30px rgba(212, 175, 55, 0.35);
}
.vault-sub {
  font-size: 0.86rem;
  line-height: 1.8;
  color: #94a3b8;
}
.vault-form {
  display: flex;
  gap: 0.5rem;
  margin: 1.4rem 0 0.4rem;
}
.vault-form input {
  flex: 1;
  min-width: 0;
  padding: 0.7rem 0.9rem;
  border-radius: 10px;
  border: 1px solid rgba(212, 175, 55, 0.35);
  background: rgba(4, 4, 10, 0.9);
  color: #f0d060;
  font-family: inherit;
  font-size: 0.9rem;
  text-align: center;
  letter-spacing: 0.2em;
  outline: none;
}
.vault-form input:focus {
  border-color: #d4af37;
  box-shadow: 0 0 0 3px rgba(212, 175, 55, 0.14);
}
.vault-form button {
  flex: 0 0 auto;
  padding: 0.7rem 1.2rem;
  border-radius: 10px;
  border: 1px solid rgba(212, 175, 55, 0.5);
  background: linear-gradient(135deg, rgba(212, 175, 55, 0.28), rgba(184, 134, 11, 0.2));
  color: #f0d060;
  font-family: inherit;
  font-weight: 700;
  letter-spacing: 0.1em;
  cursor: pointer;
}
.vault-form button:hover {
  box-shadow: 0 0 18px rgba(212, 175, 55, 0.35);
}
.vault-error {
  font-size: 0.78rem;
  color: #f87171;
}
.vault-back {
  margin-top: 1.2rem;
  font-size: 0.72rem;
}
.vault-back a {
  color: #64748b;
  text-decoration: none;
}
.vault-back a:hover {
  color: #a5b4fc;
}
.vault-open h1 {
  margin: 0.4rem 0 0.2rem;
  font-size: clamp(2rem, 7vw, 3rem);
  font-weight: 900;
  letter-spacing: 0.04em;
  color: #f0d060;
  text-shadow: 0 0 34px rgba(212, 175, 55, 0.3);
}
.vault-letter {
  margin: 1.8rem 0;
  padding: 1.4rem 1.5rem;
  border-left: 3px solid rgba(212, 175, 55, 0.6);
  border-radius: 0 12px 12px 0;
  background: rgba(212, 175, 55, 0.05);
}
.vault-letter p {
  margin: 0.55rem 0;
  font-size: 0.88rem;
  line-height: 1.95;
  color: #d6d3d1;
}
.vault-sign {
  text-align: right;
  color: #d4af37 !important;
  font-weight: 700;
}
.vault-cards {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(240px, 1fr));
  gap: 0.9rem;
  margin: 1.4rem 0;
}
.vault-card {
  padding: 1.2rem;
  border: 1px solid rgba(212, 175, 55, 0.22);
  border-radius: 14px;
  background: rgba(14, 12, 4, 0.62);
  transition: all 0.3s ease;
}
.vault-card:hover {
  border-color: rgba(212, 175, 55, 0.55);
  transform: translateY(-3px);
  box-shadow: 0 14px 34px rgba(0, 0, 0, 0.3);
}
.card-id {
  font-size: 0.64rem;
  letter-spacing: 0.24em;
  color: #a16207;
}
.vault-card h3 {
  margin: 0.35rem 0 0.5rem;
  font-size: 1.05rem;
  color: #f0d060;
}
.vault-card p {
  font-size: 0.78rem;
  line-height: 1.75;
  color: #a8a29e;
}
.card-btn {
  display: inline-block;
  margin-top: 0.7rem;
  padding: 0.5rem 0.9rem;
  border-radius: 8px;
  border: 1px solid rgba(212, 175, 55, 0.4);
  background: rgba(212, 175, 55, 0.1);
  color: #f0d060;
  font-family: inherit;
  font-size: 0.74rem;
  font-weight: 700;
  text-decoration: none;
  cursor: pointer;
  transition: all 0.2s ease;
}
.card-btn:hover {
  background: rgba(212, 175, 55, 0.2);
  border-color: #d4af37;
}
.card-btn.on {
  background: rgba(212, 175, 55, 0.85);
  color: #1c1917;
}
.vault-note {
  font-size: 0.74rem;
  color: #78716c;
}
.ending-list {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(260px, 1fr));
  gap: 0.45rem;
  margin: 1.4rem 0;
}
.ending-row {
  display: flex;
  align-items: center;
  gap: 0.8rem;
  padding: 0.55rem 0.8rem;
  border-radius: 8px;
  border: 1px solid rgba(212, 175, 55, 0.16);
  background: rgba(14, 12, 4, 0.45);
  font-size: 0.8rem;
}
.ending-row span {
  color: #78716c;
}
.ending-row b {
  flex: 1;
  color: #e7e5e4;
  letter-spacing: 0.06em;
}
.ending-row em {
  color: #f0d060;
  font-style: normal;
  font-weight: 700;
}
.ending-formula {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 0.9rem;
  margin: 1.6rem 0;
  padding: 1.2rem;
  border-radius: 14px;
  border: 1px solid rgba(212, 175, 55, 0.3);
  background: rgba(212, 175, 55, 0.06);
  font-size: clamp(1.2rem, 4vw, 1.8rem);
  font-weight: 800;
  color: #e7e5e4;
  letter-spacing: 0.08em;
}
.ending-formula .op {
  color: #78716c;
  font-weight: 400;
}
.ending-formula .result {
  color: #f0d060;
  text-shadow: 0 0 24px rgba(212, 175, 55, 0.5);
}
.ending-actions {
  display: flex;
  flex-wrap: wrap;
  gap: 0.8rem;
  margin-top: 1.6rem;
}
.ending-actions a {
  padding: 0.55rem 1rem;
  border-radius: 999px;
  border: 1px solid rgba(212, 175, 55, 0.3);
  color: #d4af37;
  font-size: 0.76rem;
  text-decoration: none;
}
.ending-actions a:hover {
  background: rgba(212, 175, 55, 0.12);
  border-color: #d4af37;
}
@media (max-width: 640px) {
  .vault-wrap {
    padding: 1.6rem 0.7rem 3rem;
  }
  .vault-lock {
    padding: 1.8rem 1.1rem;
  }
}
</style>

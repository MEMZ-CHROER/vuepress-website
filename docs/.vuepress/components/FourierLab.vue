<script setup>
import { onBeforeUnmount, onMounted, ref } from "vue";
import { withBase } from "vuepress/client";

/**
 * 频谱实验室
 *
 * 1. 拉取 /puzzle/signal.wav
 * 2. 在浏览器里做一次离线 STFT（FFT + Hann 窗）
 * 3. 把「时间 × 频率」画成频谱图
 *
 * 信号里的字母会直接显形——不需要实时播放也能看。
 */
const W = 900;
const H = 280;
const FFT_SIZE = 2048;
const HOP = 256;
const FREQ_LOW = 420;
const FREQ_HIGH = 2900;

const audio = ref(null);
const canvas = ref(null);
const playing = ref(false);
const status = ref("loading"); // loading | ready | error
const errorText = ref("");
const src = withBase("/puzzle/signal.wav");

let raf = 0;
let audioCtx = null;

/* ---------- 轻量 FFT（radix-2，原地） ---------- */
function fft(re, im) {
  const n = re.length;
  for (let i = 1, j = 0; i < n; i++) {
    let bit = n >> 1;
    for (; j & bit; bit >>= 1) j ^= bit;
    j ^= bit;
    if (i < j) {
      [re[i], re[j]] = [re[j], re[i]];
      [im[i], im[j]] = [im[j], im[i]];
    }
  }
  for (let len = 2; len <= n; len <<= 1) {
    const ang = (-2 * Math.PI) / len;
    const wRe = Math.cos(ang);
    const wIm = Math.sin(ang);
    for (let i = 0; i < n; i += len) {
      let curRe = 1;
      let curIm = 0;
      const half = len >> 1;
      for (let j = 0; j < half; j++) {
        const a = i + j;
        const b = a + half;
        const vRe = re[b] * curRe - im[b] * curIm;
        const vIm = re[b] * curIm + im[b] * curRe;
        const uRe = re[a];
        const uIm = im[a];
        re[a] = uRe + vRe;
        im[a] = uIm + vIm;
        re[b] = uRe - vRe;
        im[b] = uIm - vIm;
        const nextRe = curRe * wRe - curIm * wIm;
        curIm = curRe * wIm + curIm * wRe;
        curRe = nextRe;
      }
    }
  }
}

/* ---------- 颜色映射 ---------- */
function colorFor(v) {
  if (v < 0.07) return [5, 7, 18];
  if (v < 0.22) return [18, 36, 104];
  if (v < 0.42) return [38, 108, 198];
  if (v < 0.62) return [86, 196, 255];
  if (v < 0.8) return [196, 238, 255];
  return [255, 250, 220];
}

/* ---------- 画频谱 ---------- */
function drawSpectrogram(samples, sampleRate) {
  const cv = canvas.value;
  if (!cv) return;
  const ctx = cv.getContext("2d");
  if (!ctx) return;

  cv.width = W;
  cv.height = H;

  const frameCount = Math.max(1, Math.floor((samples.length - FFT_SIZE) / HOP) + 1);
  const img = ctx.createImageData(W, H);
  const data = img.data;

  const window = new Float64Array(FFT_SIZE);
  for (let i = 0; i < FFT_SIZE; i++) {
    window[i] = 0.5 - 0.5 * Math.cos((2 * Math.PI * i) / (FFT_SIZE - 1));
  }

  const binHz = sampleRate / FFT_SIZE;
  const lowBin = Math.max(1, Math.floor(FREQ_LOW / binHz));
  const highBin = Math.min(FFT_SIZE / 2 - 1, Math.ceil(FREQ_HIGH / binHz));

  const re = new Float64Array(FFT_SIZE);
  const im = new Float64Array(FFT_SIZE);
  const mag = new Float64Array(FFT_SIZE / 2);

  for (let x = 0; x < W; x++) {
    const frame = Math.floor((x / W) * frameCount);
    const start = frame * HOP;

    for (let i = 0; i < FFT_SIZE; i++) {
      const idx = start + i;
      re[i] = idx < samples.length ? samples[idx] * window[i] : 0;
      im[i] = 0;
    }
    fft(re, im);

    for (let b = 0; b < FFT_SIZE / 2; b++) {
      mag[b] = Math.hypot(re[b], im[b]) / (FFT_SIZE / 2);
    }

    for (let y = 0; y < H; y++) {
      // y=0 顶部是高频
      const t = 1 - y / (H - 1);
      const bin = Math.round(lowBin + t * (highBin - lowBin));
      const m = mag[bin] || 0;
      const db = 20 * Math.log10(m + 1e-9);
      const v = Math.max(0, Math.min(1, (db + 58) / 58));
      const [r, g, b] = colorFor(v);
      const p = (y * W + x) * 4;
      data[p] = r;
      data[p + 1] = g;
      data[p + 2] = b;
      data[p + 3] = 255;
    }
  }

  ctx.putImageData(img, 0, 0);
}

async function build() {
  try {
    const res = await fetch(src);
    if (!res.ok) throw new Error(`HTTP ${res.status}`);
    const buf = await res.arrayBuffer();
    const AC = window.AudioContext || window.webkitAudioContext;
    if (!AC) throw new Error("当前浏览器不支持 Web Audio");
    audioCtx = new AC();
    const decoded = await audioCtx.decodeAudioData(buf.slice(0));
    drawSpectrogram(decoded.getChannelData(0), decoded.sampleRate);
    status.value = "ready";
    audioCtx.close().catch(() => {});
    audioCtx = null;
  } catch (e) {
    status.value = "error";
    errorText.value = String(e?.message || e);
  }
}

async function toggle() {
  const el = audio.value;
  if (!el) return;
  if (el.paused) {
    el.currentTime = 0;
    try {
      await el.play();
      playing.value = true;
    } catch {
      playing.value = false;
    }
  } else {
    el.pause();
    playing.value = false;
  }
}

function onEnded() {
  playing.value = false;
  cancelAnimationFrame(raf);
}

onMounted(() => {
  void build();
});

onBeforeUnmount(() => {
  cancelAnimationFrame(raf);
  if (audio.value && !audio.value.paused) audio.value.pause();
  if (audioCtx) audioCtx.close().catch(() => {});
});
</script>

<template>
  <div class="fl">
    <div class="fl-head">
      <span class="fl-dot" :class="{ live: status === 'ready' }"></span>
      <span class="fl-title">SIGNAL ANALYZER</span>
      <span class="fl-hz">44.1 kHz · FFT {{ FFT_SIZE }} · OFFLINE STFT</span>
    </div>

    <div class="fl-screen">
      <canvas ref="canvas" class="fl-canvas" aria-label="信号的频谱图"></canvas>
      <div v-if="status === 'loading'" class="fl-cover">
        <p>解析信号中…</p>
      </div>
      <div v-else-if="status === 'error'" class="fl-cover">
        <p>频谱解析失败：{{ errorText }}</p>
        <a class="fl-btn" :href="src" download>直接下载 WAV</a>
      </div>
      <div class="fl-axis">
        <span>high freq</span>
        <span>low freq</span>
      </div>
    </div>

    <div class="fl-controls">
      <button class="fl-btn" @click="toggle">
        {{ playing ? "❚❚ 暂停" : "▶ 播放" }}
      </button>
      <a class="fl-btn ghost" :href="src" download>下载 WAV</a>
      <span class="fl-tip">横轴时间，纵轴频率。字已经显形了。</span>
    </div>

    <audio ref="audio" :src="src" preload="auto" @ended="onEnded"></audio>
  </div>
</template>

<style scoped>
.fl {
  margin: 1.5rem 0;
  border: 1px solid rgba(99, 102, 241, 0.3);
  border-radius: 14px;
  background: rgba(8, 8, 22, 0.78);
  overflow: hidden;
  box-shadow: 0 18px 50px rgba(0, 0, 0, 0.35);
}
.fl-head {
  display: flex;
  align-items: center;
  gap: 0.6rem;
  padding: 0.6rem 0.9rem;
  border-bottom: 1px solid rgba(99, 102, 241, 0.18);
  font-family: "JetBrains Mono", ui-monospace, monospace;
  font-size: 0.68rem;
  letter-spacing: 0.14em;
  color: #818cf8;
}
.fl-dot {
  width: 7px;
  height: 7px;
  border-radius: 50%;
  background: #475569;
}
.fl-dot.live {
  background: #4ade80;
  box-shadow: 0 0 10px rgba(74, 222, 128, 0.8);
}
.fl-title {
  flex: 1;
}
.fl-hz {
  color: #475569;
  letter-spacing: 0.06em;
}
.fl-screen {
  position: relative;
  background: #04040c;
}
.fl-canvas {
  display: block;
  width: 100%;
  height: 260px;
  image-rendering: auto;
}
.fl-cover {
  position: absolute;
  inset: 0;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 0.8rem;
  background: rgba(4, 4, 12, 0.82);
  text-align: center;
}
.fl-cover p {
  margin: 0;
  font-size: 0.78rem;
  line-height: 1.7;
  color: #94a3b8;
  font-family: "JetBrains Mono", ui-monospace, monospace;
}
.fl-axis {
  position: absolute;
  top: 0.4rem;
  right: 0.5rem;
  bottom: 0.4rem;
  display: flex;
  flex-direction: column;
  justify-content: space-between;
  font-family: "JetBrains Mono", ui-monospace, monospace;
  font-size: 0.55rem;
  letter-spacing: 0.1em;
  color: rgba(148, 163, 184, 0.4);
  pointer-events: none;
}
.fl-controls {
  display: flex;
  align-items: center;
  gap: 0.5rem;
  flex-wrap: wrap;
  padding: 0.8rem 0.9rem;
  border-top: 1px solid rgba(99, 102, 241, 0.18);
}
.fl-btn {
  padding: 0.45rem 0.9rem;
  border-radius: 8px;
  border: 1px solid rgba(99, 102, 241, 0.45);
  background: linear-gradient(135deg, rgba(99, 102, 241, 0.24), rgba(168, 85, 247, 0.18));
  color: #c7d2fe;
  font-family: "JetBrains Mono", ui-monospace, monospace;
  font-size: 0.72rem;
  font-weight: 700;
  letter-spacing: 0.06em;
  text-decoration: none;
  cursor: pointer;
  transition: all 0.2s ease;
}
.fl-btn:hover {
  border-color: #818cf8;
  box-shadow: 0 0 14px rgba(99, 102, 241, 0.3);
}
.fl-btn.ghost {
  background: transparent;
  border-color: rgba(100, 116, 139, 0.35);
  color: #94a3b8;
}
.fl-tip {
  margin-left: auto;
  font-size: 0.65rem;
  color: #475569;
  font-family: "JetBrains Mono", ui-monospace, monospace;
}
@media (max-width: 640px) {
  .fl-canvas {
    height: 190px;
  }
  .fl-tip {
    margin-left: 0;
    width: 100%;
  }
}
</style>

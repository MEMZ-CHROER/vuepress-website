<script setup>
import { ref, onMounted, onUnmounted } from "vue";

const patterns = [
  { id: 0, name: "乱序（宽范围）", desc: "均匀随机，0..1e9" },
  { id: 1, name: "升序", desc: "已经从小到大排好" },
  { id: 2, name: "降序", desc: "从大到小" },
  { id: 3, name: "小范围", desc: "随机 0..999" },
  { id: 4, name: "近有序", desc: "基本有序，少量交换" },
  { id: 5, name: "重复值", desc: "0..9 重复" },
];
const scales = [1000, 10000, 50000, 100000, 500000, 1000000];

const status = ref("加载中...");
const loaded = ref(false);
const current = ref(0);
const results = ref(null);
const running = ref(false);
const bestAlgo = ref("");

let mod = null;
let scriptEl = null;

function loadWasm() {
  return new Promise((resolve, reject) => {
    if (window.LxyBenchFactory) { resolve(window.LxyBenchFactory); return; }
    scriptEl = document.createElement("script");
    scriptEl.src = "/wasm/lxybench.js";
    scriptEl.onload = () => resolve(window.LxyBenchFactory);
    scriptEl.onerror = () => reject(new Error("WASM 脚本加载失败"));
    document.head.appendChild(scriptEl);
  });
}

onMounted(async () => {
  try {
    const factory = await loadWasm();
    mod = await factory();
    loaded.value = true;
    status.value = "就绪";
    run();
  } catch (e) {
    status.value = "❌ " + e.message;
  }
});
onUnmounted(() => { if (scriptEl) scriptEl.remove(); });

async function run() {
  if (!loaded.value || running.value) return;
  running.value = true;
  status.value = "运行 C++ 基准测试中...";
  try {
    // 直接用原始导出函数（避免 cwrap 可能的参数转换差异）
    const fn = mod._run_bench;
    const ptr = fn(current.value);
    const s = mod.UTF8ToString(ptr);
    results.value = JSON.parse(s);
    // 计算最佳算法（取 n=100000 的）
    const mid = results.value["n100000"];
    bestAlgo.value = mid ? mid.algo : "?";
    status.value = "完成";
  } catch (e) {
    status.value = "❌ 运行失败: " + e.message;
  } finally {
    running.value = false;
  }
}

function pick(i) { current.value = i; run(); }
</script>

<template>
  <div class="lxy-bench">
    <h3 class="bench-title">⚡ 真实 C++ 基准：lxySort vs std::sort</h3>
    <p class="bench-sub">
      通过 WebAssembly 在浏览器里跑 <b>真实的 C++ 代码</b>（<code>lxy_sort.hpp</code> 编译）。
      选一种数据模式，看 lxySort 和 std::sort 在不同规模下的实测耗时（各规模跑 5 次取最优）。
    </p>

    <div class="bench-patterns">
      <button v-for="p in patterns" :key="p.id" class="pattern"
        :class="{ active: current === p.id }" @click="pick(p.id)"
        :title="p.desc">
        {{ p.name }}
      </button>
    </div>

    <div class="bench-status" :class="{ running }">
      <span v-if="!loaded">{{ status }}</span>
      <span v-else-if="running">⏳ {{ status }}…</span>
      <span v-else-if="results">✅ {{ status }}（最佳算法：<b>{{ bestAlgo }}</b>）</span>
    </div>

    <div v-if="results" class="bench-table-wrap">
      <table class="bench-table">
        <thead>
          <tr>
            <th>数据规模</th>
            <th>lxySort</th>
            <th>std::sort</th>
            <th>加速比</th>
            <th>lxySort 分支</th>
          </tr>
        </thead>
        <tbody>
          <tr v-for="n in scales" :key="n">
            <td>{{ n >= 1000 ? (n / 1000).toFixed(0) + "k" : n }}</td>
            <td>{{ results["n" + n].lxy.toFixed(4) }} ms</td>
            <td>{{ results["n" + n].std.toFixed(4) }} ms</td>
            <td :class="results['n'+n].lxy <= results['n'+n].std ? 'win' : 'lose'">
              {{ (results["n"+n].std / results["n"+n].lxy).toFixed(2) }}x
              <span v-if="results['n'+n].lxy <= results['n'+n].std">🟢</span>
              <span v-else>🔴</span>
            </td>
            <td class="algo">{{ results["n"+n].algo }}</td>
          </tr>
        </tbody>
      </table>
      <p class="bench-note">
        数值越小越快。🟢 表示 lxySort 更快；🔴 表示 std::sort 更快。不同机器性能不同，这里是在你的浏览器里实时跑的。
      </p>
    </div>

    <p class="bench-source">
      📥 源码：<a href="/files/lxy_sort.hpp" target="_blank">lxy_sort.hpp</a> ·
      <a href="/files/bench.cpp" target="_blank">bench.cpp</a>（本组件是它的 WASM 版）
    </p>
  </div>
</template>

<style scoped>
.lxy-bench {
  border: 1px solid var(--c-border);
  border-radius: 12px;
  padding: 1.2rem;
  margin: 1rem 0;
  background: var(--c-bg-lighter);
}
.bench-title { margin: 0 0 0.3rem; font-size: 1.05rem; }
.bench-sub { font-size: 0.85rem; color: var(--c-text-light); margin: 0 0 0.8rem; line-height: 1.6; }
.bench-sub code { background: var(--c-bg-light); padding: 1px 5px; border-radius: 3px; }
.bench-patterns { display: flex; flex-wrap: wrap; gap: 0.4rem; margin-bottom: 0.6rem; }
.pattern {
  border: 1px solid var(--c-border); border-radius: 6px;
  background: var(--c-bg); color: var(--c-text);
  padding: 0.3rem 0.7rem; font-size: 0.78rem; cursor: pointer;
  transition: all 0.2s;
}
.pattern:hover { border-color: #6366f1; color: #6366f1; }
.pattern.active { background: #6366f1; color: #fff; border-color: #6366f1; }
.bench-status { font-size: 0.85rem; margin-bottom: 0.8rem; min-height: 1.3em; }
.bench-status.running { color: #6366f1; }
.bench-table-wrap { overflow-x: auto; }
.bench-table { width: 100%; border-collapse: collapse; font-size: 0.82rem; }
.bench-table th, .bench-table td { padding: 0.4rem 0.6rem; border: 1px solid var(--c-border); text-align: center; white-space: nowrap; }
.bench-table th { background: var(--c-bg); font-weight: 600; }
.bench-table td.win { color: #22c55e; font-weight: 700; }
.bench-table td.lose { color: #ef4444; }
.bench-table td.algo { color: #6366f1; }
.bench-note { font-size: 0.75rem; color: var(--c-text-lighter); margin: 0.5rem 0; }
.bench-source { font-size: 0.8rem; color: var(--c-text-light); margin: 0.5rem 0 0; }
.bench-source a { color: var(--c-brand, #6366f1); }
</style>

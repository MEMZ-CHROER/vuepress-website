<script setup>
import { ref } from "vue";

// ============ JS 版 lxySort（与 C++ 版逻辑一致）============
// 用字段标记实际用了哪个算法
let used = "";

function isUnorderedEx(a, l, r) {
  if (l > r) return { unordered: false, mx: 0, mn: 0 };
  let mx = a[l], mn = a[l], unordered = false;
  for (let i = l + 1; i <= r; i++) {
    if (a[i] < a[i - 1]) unordered = true;
    if (a[i] > mx) mx = a[i];
    if (a[i] < mn) mn = a[i];
  }
  return { unordered, mx, mn };
}

function mergeSortRange(a, l, r) {
  if (l >= r) return;
  const mid = (l + r) >> 1;
  mergeSortRange(a, l, mid);
  mergeSortRange(a, mid + 1, r);
  // 用临时数组存归并结果，最后拷回 a（对齐 C++ 版逻辑）
  const temp = a.slice(l, r + 1);
  let i = 0, j = mid - l + 1, k = l;
  while (i < mid - l + 1 && j < r - l + 1)
    a[k++] = temp[i] <= temp[j] ? temp[i++] : temp[j++];
  while (i < mid - l + 1) a[k++] = temp[i++];
  while (j < r - l + 1) a[k++] = temp[j++];
}
function mergeSort(a) {
  if (!a.length) return;
  mergeSortRange(a, 0, a.length - 1);
}

function jwjSort(a, l, r) {
  while (l < r) {
    let ok = true;
    for (let i = l; i < r; i++) if (a[i] > a[i + 1]) { [a[i], a[i + 1]] = [a[i + 1], a[i]]; ok = false; }
    r--;
    for (let i = r; i > l; i--) if (a[i] < a[i - 1]) { [a[i], a[i - 1]] = [a[i - 1], a[i]]; ok = false; }
    l++;
    if (ok) break;
  }
}

function quickSortInner(a, l, r, depth, limit) {
  if (depth > limit) { used = used + " → quickSort 深度超限, mergeSort 兜底"; mergeSortRange(a, l, r); return; }
  while (l < r) {
    const pv = l + Math.floor(Math.random() * (r - l + 1));
    [a[r], a[pv]] = [a[pv], a[r]];
    const pivot = a[r];
    let store = l;
    for (let i = l; i < r; i++) if (a[i] <= pivot) { [a[store], a[i]] = [a[i], a[store]]; store++; }
    [a[store], a[r]] = [a[r], a[store]];
    const pos = store;
    if (pos - l < r - pos) { quickSortInner(a, l, pos - 1, depth + 1, limit); l = pos + 1; }
    else { quickSortInner(a, pos + 1, r, depth + 1, limit); r = pos - 1; }
  }
}
function countingSort(a, l, r) {
  if (l >= r) return;
  let mx = a[l], mn = a[l];
  for (let i = l + 1; i <= r; i++) { mx = Math.max(mx, a[i]); mn = Math.min(mn, a[i]); }
  const cnt = new Array(mx - mn + 1).fill(0);
  for (let i = l; i <= r; i++) cnt[a[i] - mn]++;
  let p = l;
  cnt.forEach((c, i) => { while (c--) a[p++] = i + mn; });
}

function radixSort(a, l, r) {
  if (l >= r) return;
  let mx = a[l];
  for (let i = l + 1; i <= r; i++) mx = Math.max(mx, a[i]);
  const len = r - l + 1;
  const temp = new Array(len);
  for (let exp = 1; Math.floor(mx / exp) > 0; exp *= 10) {
    const cnt = new Array(10).fill(0);
    for (let i = l; i <= r; i++) cnt[Math.floor(a[i] / exp) % 10]++;
    for (let i = 1; i < 10; i++) cnt[i] += cnt[i - 1];
    for (let i = r; i >= l; i--) temp[--cnt[Math.floor(a[i] / exp) % 10]] = a[i];
    for (let i = l; i <= r; i++) a[i] = temp[i - l];
  }
}

// 区间智能排序（供隔离后中间段使用，与 C++ 版 lxySortRange 一致）
function lxySortRange(a, l, r) {
  if (l >= r) return;
  const { unordered, mx, mn } = isUnorderedEx(a, l, r);
  if (!unordered) { used += " → 中间段已有序"; return; }
  if (mx - mn < 20) { used += " → countingSort 计数"; countingSort(a, l, r); return; }
  if (a[r] < a[l] && (r - l + 1) <= 64) { used += " → jwjSort 鸡尾酒"; [a[l], a[r]] = [a[r], a[l]]; jwjSort(a, l, r); return; }
  if (mn > 0 && mx <= 99999) { used += " → radixSort 基数"; radixSort(a, l, r); return; }
  used += " → quickSort 快排";
  const limit = 2 * Math.log2(r - l + 1) + 1;
  quickSortInner(a, l, r, 0, limit);
}

// 主角：默认把 min 隔离到最左、max 隔离到最右，再对中间段智能排序
function lxySort(a, m) {
  if (!a.length) { used = "（空数组）"; return; }
  const n = a.length;
  const whole = isUnorderedEx(a, 0, n - 1);
  if (!whole.unordered) { used = "已有序，直接返回"; return; }
  if (m) { used = "mergeSort（稳定排序）"; mergeSort(a); return; }
  if (n <= 2) { if (a[0] > a[1]) { [a[0], a[1]] = [a[1], a[0]]; } used = "小数组直接交换"; return; }

  // 隔离 min 到最左、max 到最右
  let posMax = 0, posMin = 0;
  for (let i = 1; i < n; i++) {
    if (a[i] > a[posMax]) posMax = i;
    if (a[i] < a[posMin]) posMin = i;
  }
  [a[posMax], a[n - 1]] = [a[n - 1], a[posMax]];
  if (posMin === n - 1) posMin = posMax;
  [a[posMin], a[0]] = [a[0], a[posMin]];

  used = "隔离 min→最左, max→最右，中间段";
  lxySortRange(a, 1, n - 2);   // 中间段 [1, n-2]
}

// ============ 组件状态 ============
const input = ref("5 2 8 1 9 3 7 6 4");
const stable = ref(false);
const result = ref("");
const algo = ref("");
const error = ref("");
const elapsed = ref("");

function run() {
  error.value = ""; result.value = ""; algo.value = ""; elapsed.value = "";
  const parts = input.value.split(/[\s,，;；]+/).filter(Boolean);
  if (!parts.length) { error.value = "请输入至少一个数字"; return; }
  const arr = [];
  for (const p of parts) {
    const v = Number(p);
    if (!Number.isInteger(v)) { error.value = `"${p}" 不是整数`; return; }
    arr.push(v);
  }
  const copy = [...arr];
  used = "";
  const t0 = performance.now();
  lxySort(copy, stable.value);
  const t1 = performance.now();
  result.value = copy.join(" ");
  algo.value = used || "（空数组）";
  elapsed.value = ((t1 - t0).toFixed(3)) + " ms";
}

// 预置几个示例
const presets = [
  { label: "乱序", value: "5 2 8 1 9 3 7 6 4" },
  { label: "已有序", value: "1 2 3 4 5 6 7 8 9" },
  { label: "递减小数组", value: "9 8 7 6 5 4 3 2 1" },
  { label: "小范围→计数", value: "3 1 4 1 5 9 2 6 5 3 5 8 9 7 9" },
  { label: "大范围→快排", value: "9999 123 456 7890 1 55555 333 777 4444 66" },
];

function loadPreset(v) { input.value = v; }

run(); // 初始跑一次展示效果
</script>

<template>
  <div class="lxy-demo">
    <h3 class="demo-title">🚀 lxySort 在线演示</h3>

    <div class="demo-input">
      <label>输入整数（空格 / 逗号分隔）</label>
      <input v-model="input" @keyup.enter="run" placeholder="例如：5 2 8 1 9 3" />
    </div>

    <div class="demo-presets">
      <button v-for="p in presets" :key="p.label" class="preset" @click="loadPreset(p.value)">{{ p.label }}</button>
    </div>

    <div class="demo-controls">
      <label class="stable-toggle">
        <input type="checkbox" v-model="stable" />
        稳定排序（m=true → 归并）
      </label>
      <button class="run-btn" @click="run">▶ 运行 lxySort</button>
    </div>

    <div v-if="error" class="demo-error">⚠️ {{ error }}</div>

    <div v-if="result" class="demo-result">
      <div class="row"><span class="k">排序结果：</span><span class="v">{{ result }}</span></div>
      <div class="row"><span class="k">使用算法：</span><span class="v algo">{{ algo }}</span></div>
      <div class="row"><span class="k">耗时：</span><span class="v">{{ elapsed }}</span></div>
    </div>
  </div>
</template>

<style scoped>
.lxy-demo {
  border: 1px solid var(--c-border);
  border-radius: 12px;
  padding: 1.2rem;
  margin: 1rem 0;
  background: var(--c-bg-lighter);
}
.demo-title { margin: 0 0 0.8rem; font-size: 1.05rem; }
.demo-input { margin-bottom: 0.6rem; }
.demo-input label { display: block; font-size: 0.85rem; margin-bottom: 0.3rem; color: var(--c-text-light); }
.demo-input input {
  width: 100%; box-sizing: border-box;
  padding: 0.5rem 0.7rem;
  border: 1px solid var(--c-border); border-radius: 8px;
  background: var(--c-bg);
  color: var(--c-text);
  font-family: monospace;
}
.demo-presets { display: flex; flex-wrap: wrap; gap: 0.4rem; margin-bottom: 0.8rem; }
.preset {
  border: 1px solid var(--c-border); border-radius: 6px;
  background: var(--c-bg); color: var(--c-text);
  padding: 0.25rem 0.6rem; font-size: 0.75rem; cursor: pointer;
  transition: all 0.2s;
}
.preset:hover { border-color: #6366f1; color: #6366f1; }
.demo-controls { display: flex; align-items: center; gap: 1rem; margin-bottom: 0.8rem; flex-wrap: wrap; }
.stable-toggle { font-size: 0.85rem; display: flex; align-items: center; gap: 0.3rem; }
.run-btn {
  background: #6366f1; color: #fff; border: none; border-radius: 8px;
  padding: 0.45rem 1rem; font-size: 0.85rem; cursor: pointer; font-weight: 600;
}
.run-btn:hover { opacity: 0.88; }
.demo-error { color: #dc2626; font-size: 0.85rem; margin-bottom: 0.5rem; }
.demo-result {
  border-top: 1px dashed var(--c-border); padding-top: 0.8rem;
  font-size: 0.9rem; line-height: 1.7;
}
.row .k { color: var(--c-text-light); }
.row .v { font-family: monospace; word-break: break-all; }
.row .algo { color: #6366f1; font-weight: 600; }
</style>

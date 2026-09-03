<script setup>
import { ref } from "vue";

// ============ JS 版 lxySort v2（对齐 C++ lxy_sort.hpp 新版调度逻辑）============
// 与 C++ 版 lxySortImpl 保持一致：tiny→插入 / 排序检测 / counting / natural-merge / radix / introsort
let used = "";

// ---------- 基础工具 ----------
function insertionSort(a, l, r) {
  for (let i = l + 1; i <= r; i++) {
    const v = a[i];
    let j = i - 1;
    while (j >= l && a[j] > v) {
      a[j + 1] = a[j];
      j--;
    }
    a[j + 1] = v;
  }
}
function reverse(a, l, r) {
  while (l < r) {
    [a[l], a[r]] = [a[r], a[l]];
    l++;
    r--;
  }
}

// ---------- 归并排序（稳定） ----------
function mergeSortRange(a, l, r, tmp) {
  if (l >= r) return;
  const mid = (l + r) >> 1;
  mergeSortRange(a, l, mid, tmp);
  mergeSortRange(a, mid + 1, r, tmp);
  let i = l,
    j = mid + 1,
    o = l;
  while (i <= mid && j <= r) tmp[o++] = a[i] <= a[j] ? a[i++] : a[j++];
  while (i <= mid) tmp[o++] = a[i++];
  while (j <= r) tmp[o++] = a[j++];
  for (let k = l; k <= r; k++) a[k] = tmp[k];
}
function mergeSort(a) {
  if (a.length < 2) return;
  const tmp = new Array(a.length);
  mergeSortRange(a, 0, a.length - 1, tmp);
}

// ---------- Natural Merge（近有序分段归并，稳定） ----------
function naturalMerge(a, descPos, descCount) {
  const n = a.length;
  let runs = [0];
  for (let i = 0; i < descCount; i++) runs.push(descPos[i]);
  runs.push(n);
  const tmp = new Array(n);
  while (runs.length - 1 > 1) {
    const nr = [];
    for (let i = 0; i < runs.length - 1; i += 2) {
      const l = runs[i];
      if (i + 1 >= runs.length - 1) {
        nr.push(l);
        continue;
      }
      const m = runs[i + 1];
      const r = i + 2 < runs.length - 1 ? runs[i + 2] : n;
      let p = l,
        q = m,
        o = l;
      while (p < m && q < r) tmp[o++] = a[q] < a[p] ? a[q++] : a[p++];
      while (p < m) tmp[o++] = a[p++];
      while (q < r) tmp[o++] = a[q++];
      for (let s = l; s < r; s++) a[s] = tmp[s];
      nr.push(l);
    }
    nr.push(n);
    runs = nr;
  }
}

// ---------- 计数排序（稳定，整数小范围） ----------
function countingSort(a, minv) {
  const n = a.length;
  let mx = a[0],
    mn = a[0];
  for (let i = 1; i < n; i++) {
    mx = Math.max(mx, a[i]);
    mn = Math.min(mn, a[i]);
  }
  const range = mx - mn + 1;
  const cnt = new Array(range).fill(0);
  for (let i = 0; i < n; i++) cnt[a[i] - mn]++;
  let sum = 0;
  for (let i = 0; i < range; i++) {
    const c = cnt[i];
    cnt[i] = sum;
    sum += c;
  }
  const out = new Array(n);
  for (let i = 0; i < n; i++) out[cnt[a[i] - mn]++] = a[i]; // stable
  for (let i = 0; i < n; i++) a[i] = out[i];
}

// ---------- LSD 基数排序（base 256，稳定，处理负数） ----------
function radixSort(a) {
  const n = a.length;
  if (n < 2) return;
  // 把负数偏移到非负：先加一个足够大的偏移（用最小值的绝对值）
  let mn = a[0],
    mx = a[0];
  for (let i = 1; i < n; i++) {
    mn = Math.min(mn, a[i]);
    mx = Math.max(mx, a[i]);
  }
  const offset = mn < 0 ? -mn : 0;
  const src = new Array(n),
    dst = new Array(n);
  let maxKey = 0;
  for (let i = 0; i < n; i++) {
    const k = a[i] + offset;
    src[i] = k;
    if (k > maxKey) maxKey = k;
  }
  let bytes = 4;
  while (bytes > 1 && maxKey >>> ((bytes - 1) * 8) === 0) bytes--;
  for (let b = 0; b < bytes; b++) {
    const shift = b * 8;
    const cnt = new Array(256).fill(0);
    for (let i = 0; i < n; i++) cnt[(src[i] >>> shift) & 0xff]++;
    let s = 0;
    for (let i = 0; i < 256; i++) {
      const c = cnt[i];
      cnt[i] = s;
      s += c;
    }
    for (let i = 0; i < n; i++) {
      const v = src[i];
      dst[cnt[(v >>> shift) & 0xff]++] = v;
    }
    for (let i = 0; i < n; i++) src[i] = dst[i];
  }
  for (let i = 0; i < n; i++) a[i] = src[i] - offset;
}

// ---------- 堆排序（introsort 回退） ----------
function heapify(a, start, end, i) {
  for (;;) {
    let largest = i;
    const l = 2 * i - start + 1,
      r = 2 * i - start + 2;
    if (l < end && a[largest] < a[l]) largest = l;
    if (r < end && a[largest] < a[r]) largest = r;
    if (largest === i) break;
    [a[largest], a[i]] = [a[i], a[largest]];
    i = largest;
  }
}
function heapSortRange(a, l, r) {
  const len = r - l + 1;
  for (let i = l + Math.floor(len / 2) - 1; i >= l; i--) heapify(a, l, r + 1, i);
  for (let i = r; i > l; i--) {
    [a[l], a[i]] = [a[i], a[l]];
    heapify(a, l, i, l);
  }
}

// ---------- Introsort（快排 + 堆回退 + 插入基） ----------
function introRec(a, l, r, depth) {
  if (r - l <= 16) {
    insertionSort(a, l, r);
    return;
  }
  if (depth <= 0) {
    heapSortRange(a, l, r);
    return;
  }
  // 三分取中
  const mid = (l + r) >> 1;
  if (a[l] > a[mid]) [a[l], a[mid]] = [a[mid], a[l]];
  if (a[l] > a[r]) [a[l], a[r]] = [a[r], a[l]];
  if (a[mid] > a[r]) [a[mid], a[r]] = [a[r], a[mid]];
  const pivot = a[mid];
  [a[mid], a[r]] = [a[r], a[mid]];
  let i = l,
    j = r - 1;
  while (true) {
    while (i <= j && a[i] <= pivot) i++;
    while (i <= j && a[j] > pivot) j--;
    if (i >= j) break;
    [a[i], a[j]] = [a[j], a[i]];
    i++;
    j--;
  }
  [a[i], a[r]] = [a[r], a[i]];
  introRec(a, l, i - 1, depth - 1);
  introRec(a, i + 1, r, depth - 1);
}
function introQuickSort(a) {
  if (a.length < 2) return;
  introRec(a, 0, a.length - 1, 2 * Math.floor(Math.log2(a.length)));
}

// ---------- 主调度器（对齐 C++ lxySortImpl） ----------
function lxySortImpl(a, stable) {
  const n = a.length;
  if (n <= 1) {
    used = "O(1)";
    return;
  }

  // tiny 数组：n<=16 直接插入；17..128 检测排序/反转
  if (n <= 128) {
    if (n <= 16) {
      used = "Insertion";
      insertionSort(a, 0, n - 1);
      return;
    }
    let asc = true,
      desc = true;
    for (let i = 1; i < n; i++) {
      if (a[i] < a[i - 1]) asc = false;
      if (a[i - 1] < a[i]) desc = false;
      if (!asc && !desc) break;
    }
    if (asc) {
      used = "O(n) already-sorted";
      return;
    }
    if (desc) {
      used = "O(n) reverse";
      reverse(a, 0, n - 1);
      return;
    }
    if (stable) {
      used = "MergeSort";
      mergeSort(a);
    } else {
      used = "Introsort";
      introQuickSort(a);
    }
    return;
  }

  // Phase A：排序/反转检测
  {
    let asc = true,
      desc = true;
    for (let i = 1; i < n; i++) {
      if (a[i] < a[i - 1]) asc = false;
      if (a[i - 1] < a[i]) desc = false;
      if (!asc && !desc) break;
    }
    if (asc) {
      used = "O(n) already-sorted";
      return;
    }
    if (desc) {
      used = "O(n) reverse";
      reverse(a, 0, n - 1);
      return;
    }
  }

  // Phase B：扫描 max/min/descCount
  let maxv = a[0],
    minv = a[0];
  let descCount = 0;
  const descPos = [];
  const descLimit = n / 16 > 16 ? n / 16 : 16;
  for (let i = 1; i < n; i++) {
    if (a[i] < a[i - 1]) {
      if (descCount < 32) descPos[descCount] = i;
      descCount++;
    }
    if (maxv < a[i]) maxv = a[i];
    if (a[i] < minv) minv = a[i];
    // 整数宽范围：范围>n 且 descents 超阈值 → 必为 radix，提前结束扫描
    if (maxv - minv > n && descCount > descLimit) break;
  }

  // Counting（整数小范围）
  const dataRange = maxv - minv;
  if (dataRange <= n) {
    used = "Counting";
    countingSort(a, minv);
    return;
  }

  // NaturalMerge（近有序）
  if (descCount <= descLimit && descCount <= 32) {
    used = "NaturalMerge";
    naturalMerge(a, descPos, descCount);
    return;
  }

  // Radix（宽范围，大数组）
  if (n >= 512) {
    used = "Radix";
    radixSort(a);
    return;
  }

  // 比较路径
  if (stable) {
    used = "MergeSort";
    mergeSort(a);
  } else {
    used = "Introsort";
    introQuickSort(a);
  }
}

function lxySort(a, stable) {
  lxySortImpl(a, stable);
}
function lxyStableSort(a) {
  lxySortImpl(a, true);
}

// 浮点数专用调度：counting/radix 位运算不适用浮点，走比较路径（introsort/merge）
function lxySortFloat(a, stable) {
  const n = a.length;
  if (n <= 1) {
    used = "O(1)";
    return;
  }
  if (n <= 16) {
    used = "Insertion";
    insertionSort(a, 0, n - 1);
    return;
  }
  // 排序/反转检测
  let asc = true,
    desc = true;
  for (let i = 1; i < n; i++) {
    if (a[i] < a[i - 1]) asc = false;
    if (a[i - 1] < a[i]) desc = false;
    if (!asc && !desc) break;
  }
  if (asc) {
    used = "O(n) already-sorted";
    return;
  }
  if (desc) {
    used = "O(n) reverse";
    reverse(a, 0, n - 1);
    return;
  }
  // 比较路径
  if (stable) {
    used = "MergeSort";
    mergeSort(a);
  } else {
    used = "Introsort";
    introQuickSort(a);
  }
}

// ============ 组件状态 ============
const input = ref("5 2 8 1 9 3 7 6 4");
const stable = ref(false);
const result = ref("");
const algo = ref("");
const error = ref("");
const elapsed = ref("");

function run() {
  error.value = "";
  result.value = "";
  algo.value = "";
  elapsed.value = "";
  const parts = input.value.split(/[\s,，;；]+/).filter(Boolean);
  if (!parts.length) {
    error.value = "请输入至少一个数字";
    return;
  }
  const arr = [];
  let hasFloat = false;
  for (const p of parts) {
    const v = Number(p);
    if (isNaN(v)) {
      error.value = `"${p}" 不是数字`;
      return;
    }
    if (!Number.isInteger(v)) hasFloat = true;
    arr.push(v);
  }
  const copy = [...arr];
  used = "";
  const t0 = performance.now();
  // 含浮点数时走比较排序路径（counting/radix 位运算不适用浮点）
  if (hasFloat) {
    lxySortFloat(copy, stable.value);
  } else {
    lxySort(copy, stable.value);
  }
  const t1 = performance.now();
  result.value = copy.join(" ");
  algo.value = used || "（空数组）";
  elapsed.value = (t1 - t0).toFixed(3) + " ms";
}

// 预置几个示例（覆盖不同算法分支）
const presets = [
  { label: "乱序", value: "5 2 8 1 9 3 7 6 4" },
  { label: "已有序", value: "1 2 3 4 5 6 7 8 9" },
  { label: "递减", value: "9 8 7 6 5 4 3 2 1" },
  { label: "小范围→计数", value: "3 1 4 1 5 9 2 6 5 3 5 8 9 7 9" },
  { label: "近有序→自然归并", value: "1 2 3 5 4 6 7 8 10 9 11 12" },
  { label: "大范围→基数", value: "9999 123 456 7890 1 55555 333 777 4444 66" },
  { label: "长数组→大数组", value: "7 3 9 1 4 8 6 2 5 10 13 11 15 12 14 18 16 20 17 19" },
  {
    label: "🧪 大数组·乱序(计数)",
    value:
      "46 8 31 15 22 3 47 12 9 28 5 40 19 33 11 44 6 25 17 50 2 38 21 14 29 7 42 30 16 23 10 49 4 35 27 18 41 13 36 24 1 48 20 37 26 39 45 32 43 34",
  },
  {
    label: "🧪 大数组·宽范围(基数)",
    value:
      "45872 913 61234 87 90341 4567 1024 78901 23456 671 8901 34567 10987 54321 234 98765 3210 45678 9012 87654 2345 67890 12345 543 89012 3456 7890 1234 5678 90123 4567 89012 34567 890 12345 67890 1234 5678 9012 34567 8901 23456 7890 123 45678 90123 4567 89012 345",
  },
  {
    label: "🧪 大数组·近有序(自然归并)",
    value:
      "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 22 21 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 44 43 45 46 47 48 49 50",
  },
];

function loadPreset(v) {
  input.value = v;
}

// 生成随机数组（触发不同算法分支）
const genSizes = [50, 200, 500, 1000];
function genRandom() {
  const n = parseInt(randSize.value, 10) || 200;
  const range = parseInt(randRange.value, 10) || 1000000000;
  const arr = [];
  for (let i = 0; i < n; i++) arr.push(Math.floor(Math.random() * range));
  input.value = arr.join(" ");
  run();
}
function genNearSorted() {
  const n = parseInt(randSize.value, 10) || 200;
  const arr = [];
  for (let i = 0; i < n; i++) arr.push(i);
  // 少量交换
  for (let k = 0; k < Math.max(3, n / 20); k++) {
    const x = Math.floor(Math.random() * n),
      y = Math.floor(Math.random() * n);
    [arr[x], arr[y]] = [arr[y], arr[x]];
  }
  input.value = arr.join(" ");
  run();
}
const randSize = ref("200");
const randRange = ref("1000000000");

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
      <button v-for="p in presets" :key="p.label" class="preset" @click="loadPreset(p.value)">
        {{ p.label }}
      </button>
    </div>

    <div class="demo-generate">
      <label>🎲 随机生成</label>
      <input
        v-model="randSize"
        type="number"
        min="1"
        max="5000"
        title="数组长度"
        placeholder="200"
      />
      <span class="gen-sep">个元素，范围</span>
      <input
        v-model="randRange"
        type="number"
        min="1"
        max="1000000000"
        title="数值范围"
        placeholder="1000000000"
      />
      <button class="run-btn" @click="genRandom">🎲 乱序</button>
      <button class="run-btn ghost" @click="genNearSorted">📈 近有序</button>
    </div>

    <div class="demo-controls">
      <label class="stable-toggle">
        <input type="checkbox" v-model="stable" />
        稳定排序（stable=true）
      </label>
      <button class="run-btn" @click="run">▶ 运行 lxySort</button>
    </div>

    <div v-if="error" class="demo-error">⚠️ {{ error }}</div>

    <div v-if="result" class="demo-result">
      <div class="row">
        <span class="k">排序结果：</span><span class="v">{{ result }}</span>
      </div>
      <div class="row">
        <span class="k">使用算法：</span><span class="v algo">{{ algo }}</span>
      </div>
      <div class="row">
        <span class="k">耗时：</span><span class="v">{{ elapsed }}</span>
      </div>
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
.demo-title {
  margin: 0 0 0.8rem;
  font-size: 1.05rem;
}
.demo-input {
  margin-bottom: 0.6rem;
}
.demo-input label {
  display: block;
  font-size: 0.85rem;
  margin-bottom: 0.3rem;
  color: var(--c-text-light);
}
.demo-input input {
  width: 100%;
  box-sizing: border-box;
  padding: 0.5rem 0.7rem;
  border: 1px solid var(--c-border);
  border-radius: 8px;
  background: var(--c-bg);
  color: var(--c-text);
  font-family: monospace;
}
.demo-presets {
  display: flex;
  flex-wrap: wrap;
  gap: 0.4rem;
  margin-bottom: 0.8rem;
}
.preset {
  border: 1px solid var(--c-border);
  border-radius: 6px;
  background: var(--c-bg);
  color: var(--c-text);
  padding: 0.25rem 0.6rem;
  font-size: 0.75rem;
  cursor: pointer;
  transition: all 0.2s;
}
.preset:hover {
  border-color: #6366f1;
  color: #6366f1;
}
.demo-controls {
  display: flex;
  align-items: center;
  gap: 1rem;
  margin-bottom: 0.8rem;
  flex-wrap: wrap;
}
.demo-generate {
  display: flex;
  align-items: center;
  gap: 0.4rem;
  margin-bottom: 0.8rem;
  flex-wrap: wrap;
  font-size: 0.85rem;
}
.demo-generate label {
  color: var(--c-text-light);
}
.demo-generate input {
  width: 80px;
  padding: 0.35rem 0.5rem;
  border: 1px solid var(--c-border);
  border-radius: 6px;
  background: var(--c-bg);
  color: var(--c-text);
  font-family: monospace;
}
.gen-sep {
  color: var(--c-text-lighter);
  font-size: 0.75rem;
}
.run-btn.ghost {
  background: transparent;
  color: var(--c-brand, #6366f1);
  border: 1px solid var(--c-brand, #6366f1);
}
.stable-toggle {
  font-size: 0.85rem;
  display: flex;
  align-items: center;
  gap: 0.3rem;
}
.run-btn {
  background: #6366f1;
  color: #fff;
  border: none;
  border-radius: 8px;
  padding: 0.45rem 1rem;
  font-size: 0.85rem;
  cursor: pointer;
  font-weight: 600;
}
.run-btn:hover {
  opacity: 0.88;
}
.demo-error {
  color: #dc2626;
  font-size: 0.85rem;
  margin-bottom: 0.5rem;
}
.demo-result {
  border-top: 1px dashed var(--c-border);
  padding-top: 0.8rem;
  font-size: 0.9rem;
  line-height: 1.7;
}
.row .k {
  color: var(--c-text-light);
}
.row .v {
  font-family: monospace;
  word-break: break-all;
}
.row .algo {
  color: #6366f1;
  font-weight: 600;
}
</style>

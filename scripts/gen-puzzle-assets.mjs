#!/usr/bin/env node
/**
 * PROJECT 666 — 谜题资产生成器
 *
 * 生成：
 *   docs/.vuepress/public/puzzle/signal.wav   —— 频谱图里藏着 FOURIER
 *   docs/.vuepress/public/puzzle/echo.png     —— RGB 最低位藏着 PZL5:NORTH
 *
 * 用法：node scripts/gen-puzzle-assets.mjs
 */
import fs from "node:fs";
import path from "node:path";
import zlib from "node:zlib";

const root = path.resolve(process.cwd());
const outDir = path.join(root, "docs/.vuepress/public/puzzle");
fs.mkdirSync(outDir, { recursive: true });

/* ------------------------------------------------------------------ */
/* 5x7 点阵字体（只包含 FOURIER 需要的字母）                            */
/* ------------------------------------------------------------------ */
const FONT = {
  F: ["11111", "10000", "10000", "11110", "10000", "10000", "10000"],
  O: ["01110", "10001", "10001", "10001", "10001", "10001", "01110"],
  U: ["10001", "10001", "10001", "10001", "10001", "10001", "01110"],
  R: ["11110", "10001", "10001", "11110", "10100", "10010", "10001"],
  I: ["11111", "00100", "00100", "00100", "00100", "00100", "11111"],
  E: ["11111", "10000", "10000", "11110", "10000", "10000", "11111"],
  L: ["10000", "10000", "10000", "10000", "10000", "10000", "11111"],
  S: ["01111", "10000", "10000", "01110", "00001", "00001", "11110"],
  B: ["11110", "10001", "10001", "11110", "10001", "10001", "11110"],
};

/* ------------------------------------------------------------------ */
/* 1. 生成频谱图 WAV                                                   */
/* ------------------------------------------------------------------ */
function buildSpectrogram(letters) {
  const sampleRate = 44100;
  const colDur = 0.062; // 每个点阵列 62ms
  const perCol = Math.round(sampleRate * colDur);
  const fade = Math.round(sampleRate * 0.008); // 8ms 淡入淡出，避免爆音

  // 把文字展开成一列一列的 7 行像素
  const columns = [];
  for (let i = 0; i < 6; i++) columns.push([0, 0, 0, 0, 0, 0, 0]); // 前导静音
  for (let li = 0; li < letters.length; li++) {
    const glyph = FONT[letters[li]];
    for (let c = 0; c < 5; c++) {
      const col = [];
      for (let r = 0; r < 7; r++) col.push(glyph[r][c] === "1" ? 1 : 0);
      columns.push(col);
    }
    if (li !== letters.length - 1) columns.push([0, 0, 0, 0, 0, 0, 0]);
  }
  for (let i = 0; i < 6; i++) columns.push([0, 0, 0, 0, 0, 0, 0]);

  // 行 -> 频率：点阵顶部 = 高频（频谱图上是上方）
  const rowFreq = [];
  for (let r = 0; r < 7; r++) rowFreq.push(2260 - r * 260);

  const total = columns.length * perCol;
  const pcm = new Float64Array(total);

  for (let ci = 0; ci < columns.length; ci++) {
    const col = columns[ci];
    const base = ci * perCol;
    const active = [];
    for (let r = 0; r < 7; r++) if (col[r]) active.push(rowFreq[r]);

    for (let s = 0; s < perCol; s++) {
      // 每列两端做淡入淡出
      let env = 1;
      if (s < fade) env = s / fade;
      else if (s > perCol - fade) env = (perCol - s) / fade;

      let v = 0;
      for (const f of active) {
        v += Math.sin((2 * Math.PI * f * (base + s)) / sampleRate);
      }
      pcm[base + s] = (v / 7) * 0.8 * env;
    }
  }

  // 转 16-bit PCM
  const data = Buffer.alloc(total * 2);
  for (let i = 0; i < total; i++) {
    let v = Math.max(-1, Math.min(1, pcm[i]));
    data.writeInt16LE(Math.round(v * 32767), i * 2);
  }

  const header = Buffer.alloc(44);
  header.write("RIFF", 0);
  header.writeUInt32LE(36 + data.length, 4);
  header.write("WAVE", 8);
  header.write("fmt ", 12);
  header.writeUInt32LE(16, 16); // fmt chunk size
  header.writeUInt16LE(1, 20); // PCM
  header.writeUInt16LE(1, 22); // mono
  header.writeUInt32LE(sampleRate, 24);
  header.writeUInt32LE(sampleRate * 2, 28); // byte rate
  header.writeUInt16LE(2, 32); // block align
  header.writeUInt16LE(16, 34); // bits
  header.write("data", 36);
  header.writeUInt32LE(data.length, 40);

  return Buffer.concat([header, data]);
}

/* ------------------------------------------------------------------ */
/* 2. 生成 LSB 隐写 PNG                                                */
/* ------------------------------------------------------------------ */
const CRC_TABLE = (() => {
  const table = new Uint32Array(256);
  for (let n = 0; n < 256; n++) {
    let c = n;
    for (let k = 0; k < 8; k++) c = c & 1 ? 0xedb88320 ^ (c >>> 1) : c >>> 1;
    table[n] = c >>> 0;
  }
  return table;
})();

function crc32(buf) {
  let c = 0xffffffff;
  for (let i = 0; i < buf.length; i++) c = CRC_TABLE[(c ^ buf[i]) & 0xff] ^ (c >>> 8);
  return (c ^ 0xffffffff) >>> 0;
}

function chunk(type, payload) {
  const typeBuf = Buffer.from(type, "ascii");
  const body = Buffer.concat([typeBuf, payload]);
  const out = Buffer.alloc(8 + payload.length + 4);
  out.writeUInt32BE(payload.length, 0);
  body.copy(out, 4);
  out.writeUInt32BE(crc32(body), 8 + payload.length);
  return out;
}

function buildPng() {
  const W = 480;
  const H = 480;
  const px = Buffer.alloc(W * H * 3);

  // 深空渐变 + 噪点
  for (let y = 0; y < H; y++) {
    for (let x = 0; x < W; x++) {
      const i = (y * W + x) * 3;
      const t = y / H;
      const glow = Math.max(0, 1 - Math.hypot(x - W * 0.5, y - H * 0.46) / (W * 0.72));
      const r = Math.round(8 + 46 * glow + 8 * t + Math.random() * 6);
      const g = Math.round(10 + 24 * glow + 10 * t + Math.random() * 6);
      const b = Math.round(26 + 78 * glow + 14 * t + Math.random() * 8);
      px[i] = Math.min(255, r);
      px[i + 1] = Math.min(255, g);
      px[i + 2] = Math.min(255, b);
    }
  }

  // 画一个极淡的 "LSB" 提示（点阵放大 6 倍）
  const drawText = (text, ox, oy, scale, alpha) => {
    let cx = ox;
    for (const ch of text) {
      const glyph = FONT[ch];
      for (let r = 0; r < 7; r++) {
        for (let c = 0; c < 5; c++) {
          if (glyph[r][c] !== "1") continue;
          for (let dy = 0; dy < scale; dy++) {
            for (let dx = 0; dx < scale; dx++) {
              const x = cx + c * scale + dx;
              const y = oy + r * scale + dy;
              if (x < 0 || y < 0 || x >= W || y >= H) continue;
              const i = (y * W + x) * 3;
              px[i] = Math.round(px[i] * (1 - alpha) + 190 * alpha);
              px[i + 1] = Math.round(px[i + 1] * (1 - alpha) + 205 * alpha);
              px[i + 2] = Math.round(px[i + 2] * (1 - alpha) + 255 * alpha);
            }
          }
        }
      }
      cx += 6 * scale;
    }
  };
  drawText("LSB", 20, H - 90, 6, 0.07);

  // 把消息写入 RGB 最低位（MSB first）
  const message = Buffer.from("PZL5:NORTH", "ascii");
  const bits = [];
  for (const byte of message) {
    for (let b = 7; b >= 0; b--) bits.push((byte >> b) & 1);
  }
  let bitIndex = 0;
  outer: for (let i = 0; i < px.length; i++) {
    if (bitIndex >= bits.length) break outer;
    px[i] = (px[i] & 0xfe) | bits[bitIndex++];
  }

  // 原始扫描线（每行前缀 filter=0）
  const raw = Buffer.alloc(H * (1 + W * 3));
  for (let y = 0; y < H; y++) {
    raw[y * (1 + W * 3)] = 0;
    px.copy(raw, y * (1 + W * 3) + 1, y * W * 3, (y + 1) * W * 3);
  }

  const ihdr = Buffer.alloc(13);
  ihdr.writeUInt32BE(W, 0);
  ihdr.writeUInt32BE(H, 4);
  ihdr[8] = 8; // bit depth
  ihdr[9] = 2; // RGB
  ihdr[10] = 0;
  ihdr[11] = 0;
  ihdr[12] = 0;

  const text = Buffer.from(
    "Comment\0PZL5 // least significant bit (RGB) // read the lowest bit of every channel",
    "latin1",
  );

  return Buffer.concat([
    Buffer.from([137, 80, 78, 71, 13, 10, 26, 10]),
    chunk("IHDR", ihdr),
    chunk("tEXt", text),
    chunk("IDAT", zlib.deflateSync(raw, { level: 9 })),
    chunk("IEND", Buffer.alloc(0)),
  ]);
}

/* ------------------------------------------------------------------ */
/* 3. 写出                                                             */
/* ------------------------------------------------------------------ */
const wav = buildSpectrogram("FOURIER");
fs.writeFileSync(path.join(outDir, "signal.wav"), wav);
console.log(`✓ signal.wav  ${(wav.length / 1024).toFixed(1)} KiB`);

const png = buildPng();
fs.writeFileSync(path.join(outDir, "echo.png"), png);
console.log(`✓ echo.png    ${(png.length / 1024).toFixed(1)} KiB`);

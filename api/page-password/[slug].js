import crypto from "node:crypto";

const GITHUB_TOKEN = process.env.GITHUB_TOKEN || "";
const GH = "https://api.github.com/repos/MEMZ-CHROER/vuepress-website";

async function readPasswords() {
  const res = await fetch(`${GH}/contents/_passwords.json`, {
    headers: { Authorization: `Bearer ${GITHUB_TOKEN}`, Accept: "application/vnd.github.v3.raw" },
  });
  if (!res.ok) return {};
  const text = await res.text();
  try {
    return JSON.parse(text);
  } catch {
    return {};
  }
}

async function sha256(input) {
  const d = new TextEncoder().encode(input);
  const h = await crypto.subtle.digest("SHA-256", d);
  return Array.from(new Uint8Array(h)).map((b) => b.toString(16).padStart(2, "0")).join("");
}

export default async function handler(req, res) {
  const { pathname } = new URL(req.url, "http://localhost");
  const pagePath = pathname.replace(/\/$/, "").replace(/\.html$/, "");
  const passKey = "pw_" + pagePath.replace(/\//g, "_");

  // GET /api/page-password/:path  → 查是否有密码；GET ?cookie=1 同时返回 token 验证状态
  if (req.method === "GET") {
    const passwords = await readPasswords();
    const needsPw = !!passwords[pagePath];
    const cookie = req.headers.cookie || "";
    const validSession = cookie.includes(passKey + "=1");
    return res.status(200).json({ needsPw, validSession });
  }

  // POST /api/page-password/:path  → 提交密码验证，成功后写 cookie 并重定向
  if (req.method === "POST") {
    const body = await req.json();
    const submitted = typeof body.password === "string" ? body.password : "";
    const passwords = await readPasswords();
    if (!passwords[pagePath]) return res.status(404).json({ error: "no password set" });
    if (passwords[pagePath] !== submitted) return res.status(401).json({ error: "wrong password" });
    res.setHeader("Set-Cookie", `${passKey}=1; Path=/; Max-Age=86400; SameSite=Lax`);
    return res.status(200).json({ ok: true });
  }

  res.setHeader("Allow", ["GET", "POST"]);
  res.status(405).json({ error: "method not allowed" });
}

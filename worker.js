// Cloudflare Worker — API proxy + OAuth + Page Password Protection + Multi-User Auth
// Copyright (c) 2026 MEMZ-CHROER — All Rights Reserved. See LICENSE.WORKER.

const GH_API = "https://api.github.com";
const SITE_ORIGIN = "https://www.liuxiyu.cn";
const RATE_LIMIT_WINDOW = 60000; // 1 minute
const MAX_LOGIN_ATTEMPTS = 5;
const MAX_USERNAME_LEN = 32;
const MAX_PASSWORD_LEN = 128;

// In-memory rate limiter (per Worker instance, resets on cold start)
const loginAttempts = new Map();

function json(data, status = 200) {
  return new Response(JSON.stringify(data), {
    status,
    headers: { "Content-Type": "application/json", "Access-Control-Allow-Origin": "*" },
  });
}

async function getSession(token, db) {
  if (!token || typeof token !== "string") return null;
  const { results } = await db
    .prepare(
      "SELECT u.id, u.username, u.role, u.permissions, u.write_paths FROM sessions s JOIN users u ON s.user_id = u.id WHERE s.token = ?",
    )
    .bind(token)
    .all();
  if (!results || results.length === 0) return null;
  return results[0];
}

async function requirePerm(token, db, perm) {
  const user = await getSession(token, db);
  if (!user) return false;
  return user.permissions === "all" || (user.permissions && user.permissions.indexOf(perm) > -1);
}

async function isAdmin(token, db) {
  const user = await getSession(token, db);
  return user && user.role === "admin";
}

// custom 角色：允许写入指定路径（pwd/文件名），读不受限
// write_paths 存逗号分隔的路径/目录前缀；支持目录（结尾 /）与精确文件名
// custom 角色可以有 permissions 节点（如 pages），但写操作一律受 write_paths 白名单约束
async function canWrite(token, db, targetPath) {
  const user = await getSession(token, db);
  if (!user) return false;
  // admin 或 all 全权放行（不受白名单限制）
  if (user.permissions === "all" || user.role === "admin") return true;
  // 非 custom 角色：走原有 requirePerm 逻辑（由调用方决定用哪个节点）
  if (user.role !== "custom") return null; // null = 不拦截，交给原逻辑
  // custom 角色：无论有什么权限节点，写操作都受白名单约束
  const allowed = (user.write_paths || "")
    .split(",")
    .map((s) => s.trim())
    .filter(Boolean);
  if (allowed.length === 0) return false;
  let t = targetPath.replace(/^\/(api|raw)\//, "/");
  // GitHub API 路径形如 /repos/OWNER/REPO/contents/docs/csharp/a.md
  const m = t.match(/\/contents\/(.+)$/);
  if (!m) {
    throw new Error("cw no contents: " + targetPath);
  }
  const filePath = m[1];
  // 兼容 docs/ 前缀差异：文件可能在 docs/ 下，但 URL 里可能带或不带 docs/ 前缀
  const variants = [];
  if (filePath.indexOf("docs/") === 0) variants.push(filePath, filePath.slice(5));
  else variants.push(filePath, "docs/" + filePath);
  for (const rule of allowed) {
    if (!rule) continue;
    // 目录规则（结尾 /）：前缀匹配
    if (rule.endsWith("/")) {
      for (const v of variants) {
        if (v === rule.slice(0, -1) || v.startsWith(rule) || v.startsWith("docs/" + rule))
          return true;
      }
    }
    // 精确文件名规则
    else {
      for (const v of variants) {
        if (v === rule) return true;
      }
    }
  }
  return false;
}

async function sha256(input) {
  const encoder = new TextEncoder();
  const data = encoder.encode(input);
  const hash = await crypto.subtle.digest("SHA-256", data);
  return Array.from(new Uint8Array(hash))
    .map((b) => b.toString(16).padStart(2, "0"))
    .join("");
}

function sanitize(input, maxLen = MAX_USERNAME_LEN) {
  if (typeof input !== "string") return "";
  return input
    .replace(/[<>&"'\\;]/g, "")
    .trim()
    .substring(0, maxLen);
}

function checkRateLimit(ip) {
  const now = Date.now();
  const entry = loginAttempts.get(ip);
  if (entry) {
    if (entry.count >= MAX_LOGIN_ATTEMPTS && now - entry.windowStart < RATE_LIMIT_WINDOW) {
      return false;
    }
    if (now - entry.windowStart > RATE_LIMIT_WINDOW) {
      loginAttempts.set(ip, { count: 1, windowStart: now });
    } else {
      entry.count++;
    }
  } else {
    loginAttempts.set(ip, { count: 1, windowStart: now });
  }
  return true;
}

export default {
  async fetch(request, env, ctx) {
    const url = new URL(request.url);
    const path = url.pathname;

    // ── CORS preflight ──
    if (request.method === "OPTIONS") {
      return new Response(null, {
        headers: {
          "Access-Control-Allow-Origin": "*",
          "Access-Control-Allow-Methods": "GET,POST,PUT,DELETE",
          "Access-Control-Allow-Headers": "Content-Type,Authorization",
        },
      });
    }

    // ── MD 内容接口：客户端填充用，从 raw.githubusercontent 拉 md（免 API 限流）──
    // /md/docs/hacknet/commands/README.md → raw 拉取
    if (path.startsWith("/md/")) {
      var mdPath = path.replace(/^\/md\//, "");
      var mdUrl =
        "https://raw.githubusercontent.com/MEMZ-CHROER/vuepress-website/master/docs/" + mdPath;
      var mdRes = await fetch(mdUrl, {
        headers: { "User-Agent": "CSEL-Worker" },
        // cf: { cacheEverything: true, cacheTtl: 60 } // 生产启用，本地 Miniflare 不支持
      });
      if (!mdRes.ok) {
        return new Response("md not found", {
          status: 404,
          headers: { "Content-Type": "text/plain" },
        });
      }
      return new Response(mdRes.body, {
        status: 200,
        headers: {
          "Content-Type": "text/plain; charset=utf-8",
          "Cache-Control": "public, max-age=60",
          "Access-Control-Allow-Origin": "*",
        },
      });
    }

    // ═══════════════════════════════════════════════
    //  MULTI-USER AUTH API
    // ═══════════════════════════════════════════════

    // Login
    if (path === "/api/auth/login" && request.method === "POST") {
      const clientIp = request.headers.get("CF-Connecting-IP") || "unknown";
      try {
        if (!checkRateLimit(clientIp)) {
          return json({ error: "too many attempts, try later" }, 429);
        }
        const body = await request.json();
        const username = sanitize(body.username, MAX_USERNAME_LEN);
        const password =
          typeof body.password === "string" ? body.password.substring(0, MAX_PASSWORD_LEN) : "";
        if (!username || username.length < 2) return json({ error: "invalid username" }, 400);
        if (!password) return json({ error: "invalid password" }, 400);
        const { results } = await env.DB.prepare("SELECT * FROM users WHERE username = ?")
          .bind(username)
          .all();
        if (!results || results.length === 0) return json({ error: "invalid credentials" }, 401);
        const user = results[0];
        const hash = await sha256(password);
        if (hash !== user.password_hash) return json({ error: "invalid credentials" }, 401);
        const token = await sha256(
          username + ":" + hash + ":" + Math.random().toString(36).slice(2),
        );
        await env.DB.prepare("INSERT INTO sessions (user_id, token) VALUES (?, ?)")
          .bind(user.id, token)
          .run();
        loginAttempts.delete(clientIp);
        return json({
          token,
          username: user.username,
          role: user.role,
          permissions: user.permissions || "all",
        });
      } catch (e) {
        return json({ error: "login error: " + e.message }, 500);
      }
    }

    // Verify session
    if (path === "/api/auth/check" && request.method === "POST") {
      const body = await request.json();
      if (!body.token || typeof body.token !== "string") return json({ valid: false });
      const user = await getSession(body.token, env.DB);
      if (!user) return json({ valid: false });
      return json({
        valid: true,
        username: user.username,
        role: user.role,
        permissions: user.permissions || "all",
      });
    }

    // Logout
    if (path === "/api/auth/logout" && request.method === "POST") {
      const body = await request.json();
      if (body.token)
        await env.DB.prepare("DELETE FROM sessions WHERE token = ?").bind(body.token).run();
      return json({ ok: true });
    }

    // List users (admin only)
    if (path === "/api/auth/users" && request.method === "GET") {
      const token = url.searchParams.get("token");
      if (!(await isAdmin(token, env.DB))) return json({ error: "unauthorized" }, 403);
      const { results } = await env.DB.prepare(
        "SELECT id, username, role, permissions, write_paths, created_at FROM users ORDER BY id",
      ).all();
      return json(results || []);
    }

    // Create user (admin only)
    if (path === "/api/auth/users" && request.method === "POST") {
      const body = await request.json();
      if (!(await isAdmin(body.token, env.DB))) return json({ error: "unauthorized" }, 403);
      const username = sanitize(body.username, MAX_USERNAME_LEN);
      if (!username || username.length < 2) return json({ error: "invalid username" }, 400);
      const password = typeof body.password === "string" ? body.password : "";
      if (password.length < 6) return json({ error: "password too short (min 6)" }, 400);
      // role 支持 admin / custom / editor（custom = 写操作受 write_paths 白名单约束）
      const role = body.role === "admin" ? "admin" : body.role === "custom" ? "custom" : "editor";
      const perms =
        body.permissions === "all"
          ? "all"
          : body.permissions || "media,navbar,pages,passwords,sidebar";
      // custom 权限：write_paths 存允许写入的路径/文件名（逗号分隔）
      const writePaths = (typeof body.write_paths === "string" ? body.write_paths : "").trim();
      const hash = await sha256(password);
      try {
        await env.DB.prepare(
          "INSERT INTO users (username, password_hash, role, permissions, write_paths) VALUES (?, ?, ?, ?, ?)",
        )
          .bind(username, hash, role, perms, writePaths)
          .run();
        return json({ ok: true });
      } catch (e) {
        if (e.message && e.message.includes("UNIQUE"))
          return json({ error: "username already exists" }, 409);
        return json({ error: "create failed" }, 500);
      }
    }

    // Delete user (admin only)
    if (path === "/api/auth/users/delete" && request.method === "POST") {
      const body = await request.json();
      if (!(await isAdmin(body.token, env.DB))) return json({ error: "unauthorized" }, 403);
      const id = parseInt(body.id);
      if (!id || id <= 1) return json({ error: "cannot delete root admin" }, 400);
      await env.DB.prepare("DELETE FROM users WHERE id = ?").bind(id).run();
      await env.DB.prepare("DELETE FROM sessions WHERE user_id = ?").bind(id).run();
      return json({ ok: true });
    }

    // Change password
    if (path === "/api/auth/password" && request.method === "POST") {
      const body = await request.json();
      if (!body.token || typeof body.token !== "string")
        return json({ error: "unauthorized" }, 401);
      const { results } = await env.DB.prepare(
        "SELECT u.id, u.password_hash FROM sessions s JOIN users u ON s.user_id = u.id WHERE s.token = ?",
      )
        .bind(body.token)
        .all();
      if (!results || results.length === 0) return json({ error: "unauthorized" }, 401);
      const user = results[0];
      const oldHash = await sha256(body.oldPassword || "");
      if (oldHash !== user.password_hash) return json({ error: "wrong password" }, 401);
      const newPassword = typeof body.newPassword === "string" ? body.newPassword : "";
      if (newPassword.length < 6) return json({ error: "password too short" }, 400);
      const newHash = await sha256(newPassword);
      await env.DB.prepare("UPDATE users SET password_hash = ? WHERE id = ?")
        .bind(newHash, user.id)
        .run();
      return json({ ok: true });
    }

    // ═══════════════════════════════════════════════
    //  GITHUB API PROXY
    // ═══════════════════════════════════════════════
    if (path.startsWith("/api/") || path.startsWith("/raw/")) {
      const isRaw = path.startsWith("/raw/");
      const target = GH_API + path.replace(/^\/(api|raw)/, "") + url.search;
      const targetPath = path.replace(/^\/(api|raw)\//, "/");
      const cache = caches.default;

      // GET — no permission check needed (read-only)
      if (request.method === "GET") {
        const cacheKey = new Request(target, request);
        let cached = await cache.match(cacheKey);
        if (cached) return cached;
        const headers = {
          Authorization: "Bearer " + env.GITHUB_TOKEN,
          "Content-Type": "application/json",
          "User-Agent": "CSEL-Worker",
        };
        headers.Accept = isRaw ? "application/vnd.github.v3.raw" : "application/vnd.github.v3+json";
        let res = await fetch(target, { headers });
        res = new Response(res.body, res);
        res.headers.set("Cache-Control", "public, max-age=10");
        ctx.waitUntil(cache.put(cacheKey, res.clone()));
        return res;
      }

      // PUT/DELETE — require session token and check permissions
      const clone = request.clone();
      let bodyData;
      try {
        bodyData = await clone.json();
      } catch (e) {
        bodyData = {};
      }
      const token = bodyData._token || "";

      // Permission matrix based on target file path (VuePress 迁移后路径)
      const isPasswords = targetPath.includes("/contents/_passwords.json");
      const isConfig = targetPath.includes("/contents/.vuepress/config.json");
      const isStyle =
        targetPath.includes("/contents/.vuepress/theme.json") ||
        targetPath.includes("/contents/.vuepress/styles/index.scss");
      const isMedia = targetPath.includes("/contents/.vuepress/public/assets/img/");
      const isNavbar = targetPath.includes("/contents/.vuepress/navbar.json");
      const isSidebar = targetPath.includes("/contents/.vuepress/sidebar.json");
      const isPage = /\/contents\/docs\/.*\.md$/.test(targetPath);
      const isPagesBuild = targetPath.includes("/pages/builds");

      // custom 权限模型优先：按 write_paths 白名单校验（仅限文件写操作，读不拦截）
      const cw = await canWrite(token, env.DB, targetPath);
      if (cw === true) {
        // custom 放行，直接继续
      } else if (cw === false) {
        return json({ error: "not allowed to write this path" }, 403);
      } else {
        // 非 custom 用户，走原有权限矩阵
        if (isPasswords) {
          if (!(await requirePerm(token, env.DB, "passwords")))
            return json({ error: "need 'passwords' permission" }, 403);
        } else if (isConfig || isStyle) {
          if (!(await isAdmin(token, env.DB))) return json({ error: "admin only" }, 403);
        } else if (isMedia) {
          if (!(await requirePerm(token, env.DB, "media")))
            return json({ error: "need 'media' permission" }, 403);
        } else if (isNavbar) {
          if (!(await requirePerm(token, env.DB, "navbar")))
            return json({ error: "need 'navbar' permission" }, 403);
        } else if (isSidebar) {
          if (!(await requirePerm(token, env.DB, "sidebar")))
            return json({ error: "need 'sidebar' permission" }, 403);
        } else if (isPage || isPagesBuild) {
          if (!(await requirePerm(token, env.DB, "pages")))
            return json({ error: "need 'pages' permission" }, 403);
        } else {
          // Unknown path — admin only for safety
          if (!(await isAdmin(token, env.DB))) return json({ error: "unauthorized" }, 403);
        }
      }

      // Strip _token before forwarding, rebuild body
      delete bodyData._token;
      const headers = {
        Authorization: "Bearer " + env.GITHUB_TOKEN,
        "Content-Type": "application/json",
        "User-Agent": "CSEL-Worker",
      };
      headers.Accept = "application/vnd.github.v3+json";
      return fetch(target, { method: request.method, headers, body: JSON.stringify(bodyData) });
    }

    // ═══════════════════════════════════════════════
    //  SITEMAP GENERATOR
    // ═══════════════════════════════════════════════
    if (path === "/sitemap.xml" && request.method === "GET") {
      const cache = caches.default;
      const cacheKey = new Request("https://sitemap-cache/site-sitemap", request);
      let cached = await cache.match(cacheKey);
      if (cached) return cached;
      const treeUrl = GH_API + "/repos/MEMZ-CHROER/vuepress-website/git/trees/master?recursive=1";
      const treeRes = await fetch(treeUrl, {
        headers: { Authorization: "Bearer " + env.GITHUB_TOKEN, "User-Agent": "CSEL-Worker" },
      });
      if (!treeRes.ok) return new Response("Failed to fetch repo tree", { status: 500 });
      const treeData = await treeRes.json();
      // VuePress 迁移后：内容在 docs/ 下，URL 映射回无后缀路径（docs/hacknet/foo.md → /hacknet/foo）
      const pages = (treeData.tree || []).filter(
        (f) =>
          f.path.startsWith("docs/") &&
          f.path.endsWith(".md") &&
          f.path !== "docs/README.md" &&
          f.path !== "docs/test.md",
      );
      const siteUrl = "https://www.liuxiyu.cn";
      const now = new Date().toISOString().split("T")[0];
      let xml =
        '<?xml version="1.0" encoding="UTF-8"?>\n<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">\n';
      xml += `  <url><loc>${siteUrl}/</loc><priority>1.0</priority><lastmod>${now}</lastmod></url>\n`;
      pages.forEach((f) => {
        // 目录式：docs/hacknet/commands/README.md → /hacknet/commands
        let urlPath =
          "/" +
          f.path
            .replace(/^docs\//, "")
            .replace(/\.md$/, "")
            .replace(/\/README$/, "");
        if (urlPath === "/") urlPath = "/";
        const priority = f.path.startsWith("docs/hacknet/") ? "0.7" : "0.8";
        xml += `  <url><loc>${siteUrl}${urlPath}</loc><priority>${priority}</priority><lastmod>${now}</lastmod></url>\n`;
      });
      xml += "</urlset>\n";
      const res = new Response(xml, {
        headers: {
          "Content-Type": "application/xml; charset=utf-8",
          "Cache-Control": "public, max-age=3600",
        },
      });
      ctx.waitUntil(cache.put(cacheKey, res.clone()));
      return res;
    }

    // ═══════════════════════════════════════════════
    //  PAGE PASSWORD PROTECTION
    // ═══════════════════════════════════════════════
    // 归一化：剥离尾斜杠与 .html，使无后缀与 .html 直连指向同一密码键
    var pagePath = path.replace(/\/$/, "").replace(/\.html$/, "");
    var cookie = request.headers.get("Cookie") || "";
    var passKey = "pw_" + pagePath.replace(/\//g, "_");
    var passed = cookie.includes(passKey + "=1");

    if (
      request.method === "POST" &&
      pagePath !== "/admin" &&
      !pagePath.startsWith("/admin") &&
      !pagePath.startsWith("/assets")
    ) {
      var body = await request.text();
      var formData = new URLSearchParams(body);
      var submittedPw = formData.get("password") || "";
      var cfgUrl = GH_API + "/repos/MEMZ-CHROER/vuepress-website/contents/_passwords.json";
      var cfgRes = await fetch(cfgUrl, {
        headers: {
          Authorization: "Bearer " + env.GITHUB_TOKEN,
          Accept: "application/vnd.github.v3.raw",
          "User-Agent": "CSEL-Worker",
        },
      });
      if (cfgRes.ok) {
        var cfgText = await cfgRes.text();
        try {
          var cfg = JSON.parse(cfgText);
          if (cfg[pagePath] && cfg[pagePath] === submittedPw) {
            return new Response("", {
              status: 302,
              headers: {
                "Set-Cookie": passKey + "=1; Path=/; Max-Age=86400; SameSite=Lax",
                Location: pagePath || "/",
              },
            });
          }
        } catch (e) {}
      }
      return new Response("密码错误", { status: 403 });
    }

    if (
      !passed &&
      pagePath !== "/admin" &&
      !pagePath.startsWith("/admin") &&
      !pagePath.startsWith("/assets")
    ) {
      var cfgUrl2 = GH_API + "/repos/MEMZ-CHROER/vuepress-website/contents/_passwords.json";
      var cfgRes2 = await fetch(cfgUrl2, {
        headers: {
          Authorization: "Bearer " + env.GITHUB_TOKEN,
          Accept: "application/vnd.github.v3.raw",
          "User-Agent": "CSEL-Worker",
        },
      });
      var needsPw = false;
      if (cfgRes2.ok) {
        var cfgText2 = await cfgRes2.text();
        try {
          var cfg2 = JSON.parse(cfgText2);
          if (cfg2[pagePath]) needsPw = true;
        } catch (e) {}
      }
      if (needsPw) {
        return new Response(
          '<!DOCTYPE html><html><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1.0">' +
            "<title>页面已加密</title><style>body{margin:0;background:#0c0c0c;color:#cce8cc;font:16px/1.7 monospace;display:flex;min-height:100vh;align-items:center;justify-content:center}" +
            ".box{max-width:380px;padding:32px;background:#0f1a0f;border:1px solid #1a3a1a;text-align:center}" +
            "h1{font-size:20px;color:#00ff41;margin:0 0 16px}input{width:100%;padding:10px;background:#0c0c0c;border:1px solid #1a3a3a;color:#cce8cc;font:15px monospace;box-sizing:border-box}" +
            "button{width:100%;padding:12px;margin-top:12px;background:#00ff41;color:#000;border:none;font:15px monospace;font-weight:bold;cursor:pointer}" +
            ".err{color:#e74c3c;font-size:14px}</style></head><body>" +
            '<div class="box"><h1>🔒 页面已加密</h1>' +
            (url.searchParams.get("wrong") ? '<p class="err">密码错误</p>' : "") +
            '<form method="POST"><input type="password" name="password" placeholder="输入访问密码" /><button type="submit">验证</button></form></div></body></html>',
          { headers: { "Content-Type": "text/html; charset=utf-8" } },
        );
      }
    }

    // ── URL 规范化：无后缀目录页 301 到带斜杠 ──
    // 让访问 URL 始终以 / 结尾，避免 VuePress 客户端（inferRoutePath）把无后缀 URL 推断成 .html 而 404
    var lastSeg = path.split("/").pop() || "";
    if (path !== "/" && !path.endsWith("/") && !/\.[a-zA-Z0-9]{1,8}$/.test(lastSeg)) {
      return Response.redirect(new URL(path + "/", request.url).toString(), 301);
    }

    // ── URL 重写：VuePress 产物是 .html 文件，保持无后缀 URL 访问 ──
    // 候选顺序：原样（Pages 对目录自动 301 到 index.html）→ 补 .html → 目录 index.html
    function resolveOriginPaths(p) {
      if (p === "/" || p === "") return ["/index.html"];
      if (p.endsWith("/")) return [p + "index.html"];
      // .html 结尾：先原样，404 时回退到目录 index.html（防旧链接/缓存跳 .html 后 404）
      if (/\.html$/i.test(p)) {
        return [p, p.replace(/\.html$/i, "") + "/index.html"];
      }
      var lastSeg = p.split("/").pop() || "";
      if (/\.[a-zA-Z0-9]{1,8}$/.test(lastSeg)) return [p]; // 已有扩展名：原样透传
      return [p, p + ".html", p + "/index.html"];
    }

    var cache = caches.default;
    var originUrl = SITE_ORIGIN + path;
    var originRes = null;
    var cacheKey = null;
    var candidates = resolveOriginPaths(path);
    for (var i = 0; i < candidates.length; i++) {
      var oUrl = SITE_ORIGIN + candidates[i];
      // 缓存 key 包含 query，方便用 ?t=xxx 绕过缓存验证；页面更新后 60 秒内自动生效
      var ck = new Request(oUrl + url.search, request);
      var cached = await cache.match(ck);
      if (cached) return cached;
      var r = await fetch(oUrl, { headers: { "User-Agent": "CSEL-Worker" } });
      if (r.ok) {
        originUrl = oUrl;
        originRes = r;
        cacheKey = ck;
        break;
      }
    }
    if (!originRes) {
      // 全部候选 404：返回第一个候选的响应（GitHub Pages 默认 404 页 / VuePress 404.html）
      var firstUrl = SITE_ORIGIN + candidates[0];
      originRes = await fetch(firstUrl, { headers: { "User-Agent": "CSEL-Worker" } });
      originUrl = firstUrl;
      cacheKey = new Request(firstUrl + url.search, request);
    }

    // ── 客户端内容填充：SSR 骨架保留 + vp-content 内容区替换（热改核心）──
    // 只对 HTML 页面做；admin/密码页已由前置逻辑拦截，不会走到这里
    var contentType = originRes.headers.get("Content-Type") || "";
    if (contentType.indexOf("text/html") > -1) {
      var rawHtml = await originRes.text();
      var vpStart = rawHtml.indexOf("<div vp-content");
      if (vpStart > -1) {
        // 定位 vp-content 内容区闭合（数嵌套 div）
        var depth = 0,
          j = vpStart;
        while (j < rawHtml.length) {
          if (rawHtml.startsWith("<div", j)) depth++;
          if (rawHtml.startsWith("</div>", j)) {
            depth--;
            if (depth === 0) break;
          }
          j++;
        }
        if (depth === 0) {
          var contentStart = rawHtml.indexOf(">", vpStart) + 1;
          var openTag = rawHtml.slice(vpStart, contentStart);
          // 加 data-md 指向内容源（页面路径 → GitHub md 路径）
          // 根路径 / → docs/README.md；其他 /hacknet/story/ → docs/hacknet/story/README.md
          var mdKey = path === "/" ? "README.md" : path.replace(/^\/|\/$/g, "") + "/README.md";
          var openTagWithMd =
            openTag.indexOf("data-md") > -1
              ? openTag
              : openTag.replace(">", ' data-md="' + mdKey + '">');
          var rest = rawHtml.slice(j + 6); // 跳过 </div>
          // 注入客户端 JS：拉 /md/ 渲染填充
          var clientScript =
            '<script type="module">' +
            "const el=document.querySelector('[data-md]');" +
            "if(el){const mdKey=el.getAttribute('data-md');" +
            "fetch('/md/'+mdKey).then(r=>r.text()).then(md=>{" +
            "el.innerHTML=md.split('\\n').map(l=>{" +
            "if(l.startsWith('# '))return '<h1>'+esc(l.slice(2))+'</h1>';" +
            "if(l.startsWith('## '))return '<h2>'+esc(l.slice(3))+'</h2>';" +
            "if(l.startsWith('- '))return '<li>'+esc(l.slice(2))+'</li>';" +
            "if(l.trim()==='')return '';" +
            "return '<p>'+esc(l)+'</p>';" +
            "}).join('\\n')})}" +
            "function esc(s){return s.replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;')}" +
            "</script>";
          rawHtml =
            rawHtml.slice(0, vpStart) +
            openTagWithMd +
            "<!-- 内容由客户端填充 --></div>" +
            rest.replace("</body>", clientScript + "</body>");
        }
      }
      // 重建响应（含替换后的 HTML）
      originRes = new Response(rawHtml, {
        status: originRes.status,
        headers: {
          "Content-Type": "text/html; charset=utf-8",
          "Cache-Control": "public, max-age=60",
        },
      });
    }

    var res = new Response(originRes.body, originRes);
    if (originRes.ok) {
      res.headers.set("Cache-Control", "public, max-age=60");
      ctx.waitUntil(cache.put(cacheKey, res.clone()));
    }
    return res;
  },
};

// Vercel Serverless Function: 文章摘要（AI）
// 通过 OpenAI Chat Completions（自定义 endpoint）生成摘要
// 环境变量 `OPENAI_API_KEY` 必须在 `.env.local`（已在 .gitignore）中提供，且在 Vercel 项目设置里同步。

import { promises as fs } from "fs";
import path from "path";
import fetch from "node-fetch";

/**
 * 辅助函数：读取 markdown 文件内容
 */
async function readArticle(slug) {
  // slug 可能以 .md 结尾或不带，统一处理
  const normalized = slug.replace(/\.md$/, "");
  // 文章可能位于多个目录（posts、cpp、csharp、Learning、maths 等），这里先尝试相对根目录的路径
  const possibleDirs = ["posts", "cpp", "csharp", "Learning", "maths", "planting", "tanks", "friends", "tools", "FAQ", "InformationAI", "GeoGraphy", "imgs"];
  for (const dir of possibleDirs) {
    const filePath = path.join(process.cwd(), "docs", dir, `${normalized}.md`);
    try {
      return await fs.readFile(filePath, "utf-8");
    } catch (e) {
      // ignore, try next dir
    }
  }
  // fallback: 直接在 docs 根下查找
  try {
    const filePath = path.join(process.cwd(), "docs", `${normalized}.md`);
    return await fs.readFile(filePath, "utf-8");
  } catch (e) {
    return null;
  }
}

export default async function handler(req, res) {
  const { slug } = req.query; // Vercel 通过路径参数提供 query
  if (!slug) {
    res.status(400).json({ error: "Missing slug parameter" });
    return;
  }
  const article = await readArticle(slug);
  if (!article) {
    res.status(404).json({ error: "Article not found" });
    return;
  }
  const apiKey = process.env.OPENAI_API_KEY;
  if (!apiKey) {
    res.status(500).json({ error: "OPENAI_API_KEY not configured" });
    return;
  }
  try {
    const response = await fetch("https://openapi.imikufans.cn/v1/chat/completions", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
        Authorization: `Bearer ${apiKey}`,
      },
      body: JSON.stringify({
        model: "GLM-4-Flash-250414",
        messages: [
          { role: "system", content: "You are an assistant that summarizes Chinese technical blog posts. Produce a concise 2‑3 sentence summary in Chinese without extra formatting." },
          { role: "user", content: article },
        ],
        temperature: 0.5,
        max_tokens: 200,
      }),
    });
    if (!response.ok) {
      const err = await response.text();
      throw new Error(`OpenAI API error ${response.status}: ${err}`);
    }
    const data = await response.json();
    const summary = data.choices?.[0]?.message?.content?.trim() ?? "";
    res.status(200).json({ summary });
  } catch (err) {
    console.error("AI summary error:", err);
    res.status(500).json({ error: err.message || "AI summary failed" });
  }
}

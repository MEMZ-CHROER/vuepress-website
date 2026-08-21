import { put } from "@vercel/blob";
import crypto from "node:crypto";

const md5 = (s) => crypto.createHash("md5").update(s).digest("hex");

export const config = { api: { bodyParser: false } };

export default async function handler(req, res) {
  const { filename, username, password } = req.query;
  if (!username || !password) {
    return res.status(400).send("No username or password.");
  }
  if (username !== md5("chaosmile") || password !== md5("chaosmilepassword")) {
    return res.status(400).send("username or password error.");
  }
  if (!filename) {
    return res.status(400).send("No filename.");
  }
  try {
    // bodyParser: false 时 req 是原始流，直接收集
    const chunks = [];
    for await (const chunk of req) chunks.push(chunk);
    const buffer = Buffer.concat(chunks);
    if (buffer.length === 0) {
      return res.status(400).send("No file content.");
    }
    await put(`files/${filename}`, buffer, {
      access: "private",
      addRandomSuffix: false,
      allowOverwrite: true,
    });
    res.status(200).json({ code: 200, message: "upload success." });
  } catch (e) {
    res.status(500).json({ code: 500, message: e.message ?? "upload error." });
  }
}

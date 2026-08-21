import { list } from "@vercel/blob";

export default async function handler(req, res) {
  const { filename } = req.query;
  if (!filename) {
    return res.status(400).json({ code: 400, message: "No filename." });
  }
  try {
    const { blobs } = await list({ prefix: `files/${filename}` });
    const exists = blobs.some((b) => b.pathname === `files/${filename}`);
    res.status(200).json({ code: 200, data: exists, message: "search success." });
  } catch (e) {
    res.status(500).json({ code: 500, message: e.message ?? "search error." });
  }
}

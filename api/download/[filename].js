import { get } from '@vercel/blob'

export default async function handler(req, res) {
  const { filename } = req.query
  if (!filename) {
    return res.status(400).json({ code: 400, message: 'No filename.' })
  }
  try {
    const result = await get(`files/${filename}`, { access: 'private' })
    if (!result) {
      return res.status(404).json({ code: 404, message: 'file not found.' })
    }
    const chunks = []
    for await (const chunk of result.stream) chunks.push(Buffer.from(chunk))
    const content = Buffer.concat(chunks).toString('utf-8')
    res.status(200).json({ code: 200, data: content, message: 'download success.' })
  } catch (e) {
    res.status(500).json({ code: 500, message: e.message ?? 'download error.' })
  }
}

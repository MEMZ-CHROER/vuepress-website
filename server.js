const express = require("express");
const fileUpload = require("express-fileupload");
const OSS = require("ali-oss");
const md5 = require("md5");
const app = express();
const port = 3008;

const client = new OSS({
  region: "oss-cn-beijing", // 例如：'oss-cn-hangzhou'
  accessKeyId: "LTAI5tEyUFs6DzbLUyNjqqWh",
  accessKeySecret: "JZMfGCRZJXgpPCxLRsBoBRBNZBJXnc",
  bucket: "teaching-materials-bucket",
  secure: true, // 使用HTTPS
  cname: true, // 启用自有域名配置
  endpoint: "download.chaosmile.com", // 你的自有域名
});

app.use(fileUpload());

app.get("/api/search/:filename", async (req, res) => {
  const { filename } = req.params;
  if (!filename) {
    return res.status(400).send("No filename.");
  }
  try {
    const result = await client.list({ prefix: `files/${filename}` });
    if (
      result?.objects?.length > 0 &&
      result?.objects?.some((item) => item.name === `files/${filename}`)
    ) {
      return res.status(200).json({
        code: 200,
        data: true,
        message: "search success.",
      });
    }
    return res.status(200).json({
      code: 200,
      data: false,
      message: "search success.",
    });
  } catch (error) {
    return res.status(500).json({ code: 500, message: error.message ?? "search error." });
  }
});

app.get("/api/download/:filename", async (req, res) => {
  const { filename } = req.params;
  if (!filename) {
    return res.status(400).send("No filename.");
  }
  try {
    const result = await client.get(`files/${filename}`);
    const contentType = result.res.headers["content-type"];
    if (contentType.startsWith("image/")) {
      res.set("Content-Type", contentType);
      res.status(200).send(result.content);
    } else {
      res.status(200).json({
        code: 200,
        data: result.content.toString("utf-8"),
        message: "download success.",
      });
    }
  } catch (error) {
    return res.status(500).json({ code: 500, message: error.message ?? "download error." });
  }
});

app.post("/api/upload", async (req, res) => {
  const username = req.body.username;
  const password = req.body.password;
  if (!username || !password) {
    return res.status(400).send("No username or password.");
  }
  if (username !== md5("chaosmile") || password !== md5("chaosmilepassword")) {
    return res.status(400).send("username or password error.");
  }
  if (!req.files || Object.keys(req.files).length === 0) {
    return res.status(400).send("No files were uploaded.");
  }
  const filename = req.body.filename;
  const file = req.files.file;
  if (!filename) {
    return res.status(400).send("No filename.");
  }
  if (!file) {
    return res.status(400).send("No file.");
  }

  const options = {
    meta: { temp: "noi" },
    mime: "cpp",
    headers: {
      "Content-Type": filename.includes(".cpp") ? "text/plain" : "image/*",
      "x-oss-object-acl": "private",
    },
  };
  try {
    await client.put(`files/${filename}`, file.data, options);
    res.status(200).json({ code: 200, message: "upload success." });
  } catch (error) {
    return res.status(500).json({ code: 500, message: error.message ?? "upload error." });
  }
});

app.listen(port, () => {
  console.log(`Server is running on port ${port}`);
});

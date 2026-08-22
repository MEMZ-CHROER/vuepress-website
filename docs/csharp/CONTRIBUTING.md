# 🤝 投稿指南（C# 社区）

欢迎为 C# 社区专区贡献内容！这里是参与方式。

## 📌 仓库地址

**GitHub**: `https://github.com/MEMZ-CHROER/vuepress-website`

本仓库是公开的，任何人都可以参与贡献。

## 🚀 投稿流程（PR 方式）

1. **Fork 仓库**
   打开 [vuepress-website](https://github.com/MEMZ-CHROER/vuepress-website)，点击右上角 **Fork**，把仓库复制到你自己的账号。

2. **克隆你的 Fork**
   ```bash
   git clone https://github.com/<你的用户名>/vuepress-website.git
   cd vuepress-website
   ```

3. **创建分支**
   ```bash
   git checkout -b add-csharp-article
   ```

4. **添加文章**
   在 `docs/csharp/` 目录下新建一个 `.md` 文件，例如 `docs/csharp/a.md`。
   **请使用下面的模板**。

5. **提交并推送**
   ```bash
   git add docs/csharp/
   git commit -m "docs: 添加 C# 文章 xxx"
   git push origin add-csharp-article
   ```

6. **发起 Pull Request**
   回到 GitHub 你的 Fork 仓库，点击 **Compare & pull request**，把改动提交到原仓库的 `master` 分支。
   等管理员审核合并。

## 📝 文章格式规范

每篇文章开头需要一段 **frontmatter**（YAML 元数据），格式如下：

```yaml
---
title: 文章标题
date: 2026-08-22
category:
  - C#
tag:
  - C#
  - 你想要的标签
---
```

- `title`：文章标题（必填）
- `date`：日期，格式 `YYYY-MM-DD`（不要加引号）
- `category`：分类，建议用 `C#`
- `tag`：标签，可以多个（如 `C#`、`语法`、`教程` 等）

## ✅ 注意事项

- **文件名**：用有意义的英文名，如 `docs/csharp/linq-basics.md`（比 `a.md` 好）
- **内容**：Markdown 格式，支持代码块、标题、表格等
- **图片**：如果文章需要图片，放到 `docs/csharp/static/` 目录，用 `./static/xxx.png` 引用
- **不要改其他目录的文件**：只添加 `docs/csharp/` 下的内容
- **不要提交敏感信息**：如密码、token、密钥等

## ❓ 常见问题

- **不知道怎么 Fork？** 看 [GitHub Fork 帮助](https://docs.github.com/zh/pull-requests/collaborating-with-pull-requests/working-with-forks)
- **不会写 Markdown？** 参考 [Markdown 指南](https://www.markdownguide.org/zh-hans/)
- **投稿后多久生效？** 合并后会自动构建部署，约 1-2 分钟生效

---

感谢你的贡献！🎉

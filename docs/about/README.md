---
title: 关于本站
sidebar: false
tag:
  - 关于
---

# 👋 关于本站

<div class="about-page">

## 🚀 这是啥

一个关于**手绘坦克**、**编程学习**和**奇思妙想**的个人网站。

用 [VuePress 2](https://v2.vuepress.vuejs.org/) 搭建，托管在 GitHub Pages，评论用 Twikoo + Railway。

## 🧑 关于我

<blockquote class="about-quote">
正在努力成为一个会写代码、会画坦克、会种枇杷树的斜杠少年 🎯
</blockquote>

**技能树：**

| 技能                         | 进度            |
| ---------------------------- | --------------- |
| 🎨 手绘坦克                  | ██████████ 100% |
| 💻 编程（C++ / HTML / 前端） | ████████░░ 80%  |
| 🌱 种枇杷                    | ██████░░░░ 60%  |
| 🔢 数学                      | █████░░░░░ 50%  |

## 📜 建站历程

| 时间    | 事记                                 |
| ------- | ------------------------------------ |
| 🏁 2024 | 网站诞生，最初只是个坦克展示页       |
| 🚀 2025 | 迁移到 VuePress，加入博客、AI 等内容 |
| ✨ 2026 | 全面 UI 美化，评论区上线，内容大扩充 |

## 🛠️ 技术栈

```
┌─────────────────────────────┐
│     🌐 网站架构              │
│                             │
│  📦 VuePress 2               │
│  🎨 Vite 打包                │
│  🌙 暗色模式支持              │
│  💬 Twikoo 评论               │
│  🗄️ MongoDB（Railway）       │
│  🚀 Vercel / GitHub Pages    │
│  📝 KaTeX 数学公式            │
│  📊 Mermaid 图表              │
└─────────────────────────────┘
```

### 🧩 插件生态

本站基于 **VuePress 2 + vuepress-theme-hope**，当前启用的插件：

```text
@vuepress/bundler-vite@2.0.0-rc.30
@vuepress/plugin-active-header-links@2.0.0-rc.130
@vuepress/plugin-back-to-top@2.0.0-rc.130
@vuepress/plugin-blog@2.0.0-rc.130
@vuepress/plugin-catalog@2.0.0-rc.130
@vuepress/plugin-comment@2.0.0-rc.130
@vuepress/plugin-copy-code@2.0.0-rc.130
@vuepress/plugin-copyright@2.0.0-rc.130
@vuepress/plugin-feed@2.0.0-rc.130
@vuepress/plugin-git@2.0.0-rc.130
@vuepress/plugin-icon@2.0.0-rc.130
@vuepress/plugin-links-check@2.0.0-rc.130
@vuepress/plugin-markdown-chart@2.0.0-rc.130
@vuepress/plugin-markdown-ext@2.0.0-rc.130
@vuepress/plugin-markdown-hint@2.0.0-rc.130
@vuepress/plugin-markdown-image@2.0.0-rc.130
@vuepress/plugin-markdown-include@2.0.0-rc.130
@vuepress/plugin-markdown-math@2.0.0-rc.130
@vuepress/plugin-markdown-preview@2.0.0-rc.130
@vuepress/plugin-markdown-stylize@2.0.0-rc.130
@vuepress/plugin-markdown-tab@2.0.0-rc.130
@vuepress/plugin-notice@2.0.0-rc.130
@vuepress/plugin-nprogress@2.0.0-rc.130
@vuepress/plugin-photo-swipe@2.0.0-rc.130
@vuepress/plugin-pwa@2.0.0-rc.130
@vuepress/plugin-reading-time@2.0.0-rc.130
@vuepress/plugin-redirect@2.0.0-rc.130
@vuepress/plugin-rtl@2.0.0-rc.130
@vuepress/plugin-sass-palette@2.0.0-rc.130
@vuepress/plugin-seo@2.0.0-rc.130
@vuepress/plugin-shiki@2.0.0-rc.130
@vuepress/plugin-sitemap@2.0.0-rc.130
@vuepress/plugin-slimsearch@2.0.0-rc.130
@vuepress/plugin-theme-data@2.0.0-rc.130
vuepress-plugin-components@2.0.0-rc.107
vuepress-plugin-md-enhance@2.0.0-rc.107
vuepress-theme-hope@2.0.0-rc.107
vuepress@2.0.0-rc.30
```

## 🎯 未来的想法

- [x] 首页美化 + 按钮系统
- [x] 每日一句随机语录
- [x] 评论区
- [x] 友情链接
- [ ] 更多坦克设计文章 🎨
- [ ] 编程教程系列 💻
- [ ] 更多好玩的交互 🪲

## 📬 联系我

- **GitHub**: [MEMZ-CHROER](https://github.com/MEMZ-CHROER)
- **主站**: [www.liuxiyu.cn](https://www.liuxiyu.cn)

---

_感谢你看到这里，一起加油吧 🚀_

</div>

<style scoped>
.about-page {
  max-width: 700px;
  margin: 0 auto;
  line-height: 1.8;
}

.about-quote {
  font-size: 1.05rem;
  border-left: 4px solid var(--c-brand);
  background: rgba(99, 102, 241, 0.04);
  border-radius: 0 var(--radius-sm, 6px) var(--radius-sm, 6px) 0;
  padding: 0.8rem 1.2rem;
  margin: 1rem 0;
  color: var(--c-text-light);
  font-style: normal;
}

.about-page table {
  font-size: 0.9rem;
}

.about-page pre {
  background: var(--c-bg-light);
  border: 1px solid var(--c-border);
  border-radius: var(--radius-md, 10px);
  padding: 0.8rem 1rem;
  font-size: 0.8rem;
  line-height: 1.6;
}

.about-page input[type="checkbox"] {
  margin-right: 0.3rem;
}
</style>

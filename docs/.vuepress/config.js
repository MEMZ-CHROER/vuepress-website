import { defineUserConfig } from "vuepress";
import { viteBundler } from "@vuepress/bundler-vite";
import { hopeTheme } from "vuepress-theme-hope";

export default defineUserConfig({
  lang: "zh-CN",

  title: "Lxy的博客",
  description: "Lxy Powered by VuePress@2.0.0-rc.30",
  head: [
    ["link", { rel: "icon", href: "https://z.wiki/u/MMPFZO" }],
    // KaTeX 样式（theme-hope 的 markdown.math 也会注入，重复无害）
    [
      "link",
      { rel: "stylesheet", href: "https://cdn.jsdelivr.net/npm/katex@0.16.11/dist/katex.min.css" },
    ],
  ],

  theme: hopeTheme({
    hostname: "https://www.liuxiyu.cn",
    author: "lxy",
    logo: "https://z.wiki/u/UVdSlT",

    navbar: [
      "/",
      {
        text: "📋 附件表",
        link: "/fujian/",
      },
      {
        text: "🛠️ 工具下载",
        link: "/tools/",
      },
      {
        text: "🤖 信息AI",
        link: "/InformationAI/",
      },
      {
        text: "🎨 坦克们",
        children: [
          { text: "🎨 坦克图鉴", link: "/tanks/" },
          { text: "📖 坦克设计图鉴", link: "/tanks/design.html" },
        ],
      },
      {
        text: "🌱 种花",
        link: "/planting/",
      },
      {
        text: "🔢 Maths",
        link: "/maths/",
      },
      {
        text: "🤝 友链",
        link: "/friends/",
      },
      {
        text: "👤 关于",
        link: "/about/",
      },
      {
        text: "🐙 GitHub",
        link: "https://www.github.com/MEMZ-CHROER/vuepress-test",
      },
    ],

    // Markdown 增强（内置，替代原 markdown-ext / stylize / chart / katex 插件）
    markdown: {
      // markdown-ext
      gfm: true,
      footnote: true,
      tasklist: true,
      // markdown-stylize
      align: true,
      sub: true,
      sup: true,
      mark: true,
      // markdown-chart
      mermaid: true,
      // markdown-math（katex）
      math: true,
      // markdown-tab（选项卡）
      tabs: true,
      codeTabs: true,
    },

    // 插件（内置，替代原独立插件：blog / sitemap2 / feed2 / search / pwa / copyright）
    plugins: {
      // 博客（只把 posts/ 下当文章，沿用原 blogPlugin 的行为）
      blog: {
        filter: ({ filePathRelative }) =>
          filePathRelative?.startsWith("posts/") ?? false,
        excerpt: true,
      },
      // 本地全文搜索（替代 @vuepress/plugin-search）
      slimsearch: true,
      // 站点地图
      sitemap: true,
      // RSS/Atom 订阅
      feed: {
        atom: true,
        rss: true,
        json: false,
        count: 20,
      },
      // 版权信息
      copyright: {
        author: "lxy",
        license: "MIT",
      },
      // 离线 PWA
      pwa: {
        themeColor: "#6366f1",
        manifest: {
          icons: [
            { src: "/icons/icon-192.png", sizes: "192x192", type: "image/png" },
            { src: "/icons/icon-512.png", sizes: "512x512", type: "image/png" },
          ],
        },
      },
      // 图片点击放大预览
      photoSwipe: true,
      // 代码块复制按钮
      copyCode: true,
      // 文章阅读时间
      readingTime: true,
      // 文章自动目录
      catalog: true,
      // 最后更新时间 / 贡献者
      git: true,
      // Font Awesome 图标（markdown 里用 :icon: 语法）
      icon: true,
      // 常用组件（Badge / BiliBili 视频 / PDF / 代码演示 / 分享）
      components: {
        components: ["Badge", "BiliBili", "CodePen", "PDF", "Share"],
      },
      // 公告弹窗
      notice: [
        {
          path: "/",
          title: "🚀 欢迎",
          content: "欢迎来到 Lxy 的博客！本站已升级 theme-hope 主题。",
          showOnce: true,
        },
      ],
    },
  }),

  bundler: viteBundler(),
});

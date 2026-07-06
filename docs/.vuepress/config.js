import { blogPlugin } from '@vuepress/plugin-blog'
import { defaultTheme } from '@vuepress/theme-default'
import { defineUserConfig } from 'vuepress'
import { viteBundler } from '@vuepress/bundler-vite'
import { copyrightPlugin } from '@vuepress/plugin-copyright'
import { markdownChartPlugin } from '@vuepress/plugin-markdown-chart'
import { sitemapPlugin } from 'vuepress-plugin-sitemap2'
import { feedPlugin } from 'vuepress-plugin-feed2'
import markdownItKatex from 'markdown-it-katex'

export default defineUserConfig({
  lang: 'zh-CN',

  title: 'Lxy的博客',
  description: 'Lxy Powered by VuePress@2.0.0-rc.25',
  head: [
    ['link', { rel: 'icon', href: 'https://z.wiki/u/MMPFZO' }],
    // 启用 KaTeX 样式
    ['link', { rel: 'stylesheet', href: 'https://cdn.jsdelivr.net/npm/katex@0.16.9/dist/katex.min.css' }],
  ],

  theme: defaultTheme({
    logo: 'https://z.wiki/u/UVdSlT',


    navbar: [
      '/',
      {
        text: '📋 附件表',
        link: '/fujian/',
      },
      {
        text: '🛠️ 工具下载',
        link: '/tools/',
      },
      {
        text: '🤖 信息AI',
        link: '/InformationAI/',
      },
      {
        text: '🎨 坦克们',
        children: [
          { text: '🎨 坦克图鉴', link: '/tanks/' },
          { text: '📖 坦克设计图鉴', link: '/tanks/design.html' },
        ],
      },
      {
        text: '🌱 种花',
        link: '/planting/',
      },
      {
        text: '🔢 Maths',
        link: '/maths/',
      },
      {
        text: '🤝 友链',
        link: '/friends/',
      },
      {
        text: '👤 关于',
        link: '/about/',
      },
      {
        text: '🐙 GitHub',
        link: 'https://www.github.com/MEMZ-CHROER/vuepress-test'
      },
    ],
  }),

  // 启用 KaTeX 渲染（支持 $...$ 与 $$...$$）
  extendsMarkdown: (md) => {
    md.use(markdownItKatex)
  },

  plugins: [
    // Sitemap 生成（帮助百度/Google 收录）
    sitemapPlugin({
      hostname: 'https://www.liuxiyu.cn',
      excludeUrls: ['/404.html'],
    }),

    // RSS/Atom/JSON 订阅
    feedPlugin({
      hostname: 'https://www.liuxiyu.cn',
      atom: true,
      rss: true,
      json: false,
      count: 20,
      filter: ({ frontmatter, filePathRelative }) =>
        !frontmatter.home && !!filePathRelative,
    }),

    copyrightPlugin({
      author: "lxy",
      license: "MIT"
    }),
    // 启用 Mermaid 渲染
    markdownChartPlugin({
      mermaid: true,
    }),
    blogPlugin({
      // Only files under posts are articles
      filter: ({ filePathRelative }) =>
        filePathRelative ? filePathRelative.startsWith('posts/') : false,

      // Getting article info
      getInfo: ({ frontmatter, title, data }) => ({
        title,
        author: frontmatter.author || '',
        date: frontmatter.date || null,
        category: frontmatter.category || [],
        tag: frontmatter.tag || [],
        excerpt:
          // Support manually set excerpt through frontmatter
          typeof frontmatter.excerpt === 'string'
            ? frontmatter.excerpt
            : data?.excerpt || '',
      }),

      // Generate excerpt for all pages excerpt those users choose to disable
      excerptFilter: ({ frontmatter }) =>
        !frontmatter.home &&
        frontmatter.excerpt !== false &&
        typeof frontmatter.excerpt !== 'string',

      category: [
        {
          key: 'category',
          getter: (page) => page.frontmatter.category || [],
          layout: 'Category',
          itemLayout: 'Category',
          frontmatter: () => ({
            title: 'Categories',
            sidebar: false,
          }),
          itemFrontmatter: (name) => ({
            title: `Category ${name}`,
            sidebar: false,
          }),
        },
        {
          key: 'tag',
          getter: (page) => page.frontmatter.tag || [],
          layout: 'Tag',
          itemLayout: 'Tag',
          frontmatter: () => ({
            title: 'Tags',
            sidebar: false,
          }),
          itemFrontmatter: (name) => ({
            title: `Tag ${name}`,
            sidebar: false,
          }),
        },
      ],

      type: [
        {
          key: 'article',
          // Remove archive articles
          filter: (page) => !page.frontmatter.archive,
          layout: 'Article',
          frontmatter: () => ({
            title: 'Articles',
            sidebar: false,
          }),
          // Sort pages with time and sticky
          sorter: (pageA, pageB) => {
            if (pageA.frontmatter.sticky && pageB.frontmatter.sticky)
              return pageB.frontmatter.sticky - pageA.frontmatter.sticky

            if (pageA.frontmatter.sticky && !pageB.frontmatter.sticky) return -1

            if (!pageA.frontmatter.sticky && pageB.frontmatter.sticky) return 1

            if (!pageB.frontmatter.date) return 1
            if (!pageA.frontmatter.date) return -1

            return (
              new Date(pageB.frontmatter.date).getTime() -
              new Date(pageA.frontmatter.date).getTime()
            )
          },
        },
        {
          key: 'timeline',
          // Only article with date should be added to timeline
          filter: (page) => page.frontmatter.date instanceof Date,
          // Sort pages with time
          sorter: (pageA, pageB) =>
            new Date(pageB.frontmatter.date).getTime() -
            new Date(pageA.frontmatter.date).getTime(),
          layout: 'Timeline',
          frontmatter: () => ({
            title: 'Timeline',
            sidebar: false,
          }),
        },
      ],
      hotReload: true,
    }),
  ],

  bundler: viteBundler(),
})
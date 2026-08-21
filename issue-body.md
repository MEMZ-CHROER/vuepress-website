## 问题描述 / Bug description

### 环境 / Environment

- VuePress `2.0.0-rc.26` + `@vuepress/bundler-vite`（rc.31 的 `packages/bundler-vite/src/build/build.ts` 结构相同，应同样受影响）

### 现象 / What happens

`vuepress build` 生成的静态 HTML 中，**每个页面的 `<main>` body 都是渲染循环里第一个页面的内容**。

目录式站点（`about/README.md` → `/about/`）生产实测：

- `about/index.html` 是 `<main class="vp-home">`，包含**首页 hero 的内容**（应为 `vp-page` 文档布局）
- 所有页面的 `<title>` 也复用了第一个页面的 head
- 客户端水合 / SPA 导航正常（浏览器会用正确的数据 chunk 重渲染），所以**只有 SSR 产物是错的**

### 影响 / Impact

- SEO：所有页面重复首屏内容 + 重复 title
- URL 直达（深链、爬虫、禁用 JS）显示错页
- 第一个渲染的页面成为"污染源"——把 `404.md` 移出首位，只会改变所有页面串成谁，无法根治

### 根因 / Root cause

`packages/bundler-vite/src/build/build.ts`：

```ts
const { vueApp, vueRouter } = await createVueServerApp(...) // 只创建一次
for (const page of app.pages) {
  await renderPage({ page, vueApp, vueRouter, ... })       // 复用同一 app
}
```

`renderPageToString` 对每个页面调用 `vueRouter.push(page.path)`，但 **`vueApp` 的渲染状态在页面间从未重置**，导致第一个页面之后的每个页面都复用了第一个页面的渲染结果。

### 修复方案（生产已验证）/ Proposed fix (verified in production)

每页重建 Vue app：

```ts
const ssrTemplate = await getSsrTemplate(app)
for (const page of app.pages) {
  const { vueApp, vueRouter } = await createVueServerApp(
    app.dir.temp('.server', serverEntryChunk.fileName)
  )
  await renderPage({ page, vueApp, vueRouter, ... })
}
```

对 `@vuepress/bundler-vite@2.0.0-rc.26` 打了该补丁（patch-package 固化），在线上博客验证：

- `about/index.html`：`vp-home`（首页泄漏）→ `vp-page`（正确的文档布局）
- 重建后所有页面 title 正确
- build 耗时无明显变化（~20s / 94 页）

@meteorlxy 感谢查看。关联：#1719 / #1720（clean-URL 目录回退）。

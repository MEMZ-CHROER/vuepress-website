---
home: true
title: 主页面
heroImage: ""
heroText: Lxy和核巨啸的坦克网站
tagline: 🎨 一个关于手绘坦克、编程学习和奇思妙想的网站

actions:
  - text: 🎨 核巨啸的坦克们
    link: /tanks/
    type: primary
  - text: 🆕 坦克设计图鉴
    link: /tanks/design.html
    type: secondary
  - text: 🤖 信息相关
    link: /InformationAI/
    type: thirdary
  - text: 📦 附件下载区
    link: /tools/
    type: thirdary
  - text: 🌐 HTML学习
    link: /Learning/HTML/
    type: thirdary
  - text: ⚡ C++学习
    link: /Learning/C++/
    type: thirdary
  - text: 🔢 数学三阶幻方
    link: /maths/
    type: thirdary

features:
  - title: 🌱 种花（劳技课展示）
    details: 奶茶杯再次利用，水培和土培枇杷核养成的幼苗
  - title: 🎨 核巨啸的坦克和飞机们
    details: 这是一个关于核巨啸的坦克们的展示区，30+辆原创手绘坦克
  - title: 📦 下载区（夸克网盘）
    details: 这是一个关于Lxy的电脑小工具的下载区
  - title: 🆕 坦克设计图鉴
    details: 从想象力到纸面，手把手教你设计属于自己的坦克
---

<ClientOnly>
  <QuoteOfTheDay />
  <StatsPanel />
  <RelatedPosts />
</ClientOnly>

<div class="subscribe-bar">
  <span>📡 订阅更新：</span>
  <a href="/rss.xml" target="_blank" rel="noopener noreferrer">📄 RSS</a>
  <a href="/atom.xml" target="_blank" rel="noopener noreferrer">⚛️ Atom</a>
</div>
ICP备案号：<a href="https://beian.miit.gov.cn/">津ICP备2026006589号-1</a>

ICP主体备案号：<a href="https://beian.miit.gov.cn/">津ICP备2026006589号</a>

若要查看许可协议，请点击[LICENSE][Lxy_MIT_License]查看协议
<a href="/admin/">🔐 管理后台</a>
<a href="https://cdn.liuxiyu.cn">进入CDN节点（GitHub Pages 备份）</a>
<a href="https://www.liuxiyu.cn">回到主站</a>

[Lxy_MIT_License]: ./LICENSE/README.md

<style>
.subscribe-bar {
  max-width: 800px;
  margin: 1rem auto 0;
  padding: 0.6rem 1rem;
  text-align: center;
  font-size: 0.9rem;
  color: var(--c-text-light);
}
.subscribe-bar a {
  margin: 0 0.4rem;
  text-decoration: none;
  color: var(--c-brand, #6366f1);
  font-weight: 600;
}
.subscribe-bar a:hover {
  text-decoration: underline;
}
</style>

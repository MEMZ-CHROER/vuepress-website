---
title: 友情链接
sidebar: false
---

# 🤝 友情链接

> *“一辆坦克太孤单，一群坦克才热闹。”*

<div class="friends-page">

## 🏠 主站

<div class="friend-card">
  <a href="https://liuxiyu.dpdns.org" target="_blank" rel="noopener noreferrer">
    <div class="friend-avatar">🏠</div>
    <div class="friend-info">
      <div class="friend-name">Lxy 的主站</div>
      <div class="friend-desc">个人网站主站，CDN 分发</div>
    </div>
  </a>
</div>

<div class="friend-card">
  <a href="https://www.github.com/MEMZ-CHROER" target="_blank" rel="noopener noreferrer">
    <div class="friend-avatar">🐙</div>
    <div class="friend-info">
      <div class="friend-name">MEMZ-CHROER</div>
      <div class="friend-desc">GitHub 主页，代码仓库都在这里</div>
    </div>
  </a>
</div>

## 👥 小伙伴们

<div class="friend-card">
  <a href="https://github.com/chao-smile" target="_blank" rel="noopener noreferrer">
    <div class="friend-avatar">😄</div>
    <div class="friend-info">
      <div class="friend-name">chao-smile</div>
      <div class="friend-desc">合作小伙伴，共同维护中</div>
    </div>
  </a>
</div>

## 🔗 申请友链

如果你想交换友情链接，可以到评论区留言或者提交 Issue：

```
站点名称：你的网站名
站点链接：https://example.com
站点简介：一句话介绍
```

---

*友情链接不分大小，有趣就行。*

</div>

<style scoped>
.friends-page {
  max-width: 700px;
  margin: 0 auto;
}

.friend-card {
  margin: 0.8rem 0;
  border: 1px solid var(--c-border);
  border-radius: var(--radius-md, 10px);
  background: var(--c-bg-lighter);
  transition: all 0.3s ease;
  overflow: hidden;
}

.friend-card:hover {
  transform: translateY(-2px);
  box-shadow: var(--shadow-card-hover, 0 10px 25px rgba(0, 0, 0, 0.08));
  border-color: var(--c-brand-light);
}

.friend-card a {
  display: flex;
  align-items: center;
  gap: 1rem;
  padding: 1rem 1.2rem;
  text-decoration: none !important;
  color: inherit !important;
  border: none !important;
}

.friend-avatar {
  font-size: 2.2rem;
  flex-shrink: 0;
  width: 3rem;
  text-align: center;
}

.friend-info {
  flex: 1;
}

.friend-name {
  font-size: 1.05rem;
  font-weight: 700;
  color: var(--c-text-dark);
  margin-bottom: 0.2rem;
}

.friend-desc {
  font-size: 0.85rem;
  color: var(--c-text-lighter);
}

.friends-page h2 {
  font-size: 1.2rem;
  margin: 1.2rem 0 0.5rem;
  padding-bottom: 0.3rem;
  border-bottom: 1px solid var(--c-border-light);
}
</style>

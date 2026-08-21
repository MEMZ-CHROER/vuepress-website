<script setup>
import { computed } from "vue";

const quotes = [
  {
    text: "炮管要长，装甲要厚，速度要快，造型要帅——小孩子才做选择，我全都要。",
    author: "核巨啸",
  },
  {
    text: "一辆好坦克，从名字开始就要有气势。",
    author: "核巨啸设计语录",
  },
  {
    text: "你以为我在画坦克？不，我在创造朋友。",
    author: "匿名坦克设计师",
  },
  {
    text: "编程和设计坦克有一个共同点：都是从空白开始，用想象力填满它。",
    author: "Lxy",
  },
  {
    text: "坦克的履带碾过的地方，就是路。代码跑通的地方，就是桥。",
    author: "网络",
  },
  {
    text: "没有最强的坦克，只有最懂它的人。",
    author: "坦克手语录",
  },
  {
    text: "Bug 就像坦克的薄弱装甲——你不找它，敌人也会找到。",
    author: "程序员谚语",
  },
  {
    text: "每一个像素，都是坦克的一颗螺丝。",
    author: "核巨啸",
  },
  {
    text: "最酷的坦克，永远是下一辆。",
    author: "设计哲学",
  },
  {
    text: "学无止境，就像坦克的炮管——永远指向更远的地方。",
    author: "Lxy的学习笔记",
  },
  {
    text: "如果一辆坦克不够解决问题，那就画两辆。",
    author: "核巨啸战术思想",
  },
  {
    text: "设计不是一种职业，而是一种看待世界的方式。",
    author: "坦克设计手稿扉页",
  },
];

// 随机选一条
const todayQuote = computed(() => {
  return quotes[Math.floor(Math.random() * quotes.length)];
});
</script>

<template>
  <div class="quote-card">
    <div class="quote-icon">💬</div>
    <blockquote class="quote-text">
      <p class="quote-content">「{{ todayQuote.text }}」</p>
      <footer class="quote-author">—— {{ todayQuote.author }}</footer>
    </blockquote>
  </div>
</template>

<style scoped>
.quote-card {
  max-width: 650px;
  margin: 1.5rem auto 0.5rem;
  padding: 1.2rem 1.5rem;
  background: linear-gradient(135deg, rgba(99, 102, 241, 0.05), rgba(139, 92, 246, 0.08));
  border: 1px solid rgba(99, 102, 241, 0.15);
  border-radius: var(--radius-md, 12px);
  display: flex;
  align-items: flex-start;
  gap: 0.8rem;
  transition: all 0.3s ease;
  animation: quoteFadeIn 0.6s ease;
}

.quote-card:hover {
  border-color: rgba(99, 102, 241, 0.3);
  box-shadow: 0 4px 20px rgba(99, 102, 241, 0.1);
  transform: translateY(-2px);
}

@keyframes quoteFadeIn {
  from {
    opacity: 0;
    transform: translateY(8px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}

.quote-icon {
  font-size: 1.5rem;
  line-height: 1;
  flex-shrink: 0;
  padding-top: 0.1rem;
}

.quote-text {
  margin: 0;
  padding: 0;
  border: none !important;
  background: none !important;
}

.quote-content {
  font-size: 0.95rem;
  font-weight: 500;
  color: var(--c-text);
  line-height: 1.7;
  margin: 0 0 0.3rem 0 !important;
  font-style: normal;
}

.quote-author {
  font-size: 0.8rem;
  color: var(--c-text-lighter);
  text-align: right;
  margin-top: 0.2rem;
}

@media (max-width: 719px) {
  .quote-card {
    padding: 1rem;
    margin: 1rem 0.5rem;
  }
  .quote-content {
    font-size: 0.88rem;
  }
}
</style>

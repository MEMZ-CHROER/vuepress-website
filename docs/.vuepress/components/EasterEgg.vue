<script setup>
import { ref } from 'vue'

const count = ref(0)
const showSurprise = ref(false)
const msg = ref('')

function clickTank() {
  count.value++
  if (count.value >= 5) {
    showSurprise.value = true
    const messages = [
      '🎉 你发现了一只野生坦克！',
      '💥 坦克开炮了！BOOM！',
      '🛡️ 装甲加厚 +100',
      '⚡ 速度提升，引擎轰鸣！',
      '🎨 获得限定涂装：幻彩星辰',
      '🏆 成就解锁：坦克收藏家',
      '🔫 主炮已就绪，目标锁定！',
    ]
    msg.value = messages[Math.floor(Math.random() * messages.length)]
    setTimeout(() => {
      showSurprise.value = false
      count.value = 0
    }, 2000)
  }
}
</script>

<template>
  <div class="easter-egg">
    <span class="egg-trigger" @click="clickTank" title="戳我试试">🪲</span>
    <Transition name="pop">
      <div v-if="showSurprise" class="egg-msg">{{ msg }}</div>
    </Transition>
  </div>
</template>

<style scoped>
.easter-egg {
  position: fixed;
  bottom: 1.2rem;
  left: 1.2rem;
  z-index: 100;
  font-size: 1.2rem;
  opacity: 0.3;
  transition: opacity 0.3s ease;
  cursor: pointer;
}

.easter-egg:hover {
  opacity: 0.8;
}

.egg-trigger {
  display: inline-block;
  user-select: none;
  transition: transform 0.2s ease;
}

.egg-trigger:active {
  transform: scale(1.3);
}

.egg-msg {
  position: absolute;
  bottom: 2rem;
  left: 0;
  background: var(--c-bg-lighter);
  border: 1px solid var(--c-border);
  border-radius: 8px;
  padding: 0.4rem 0.8rem;
  font-size: 0.8rem;
  white-space: nowrap;
  box-shadow: var(--shadow-md, 0 4px 16px rgba(0, 0, 0, 0.06));
  color: var(--c-text);
  font-weight: 500;
}

.pop-enter-active {
  animation: popIn 0.3s cubic-bezier(0.34, 1.56, 0.64, 1);
}

.pop-leave-active {
  animation: popIn 0.2s ease reverse;
}

@keyframes popIn {
  from {
    opacity: 0;
    transform: translateY(8px) scale(0.8);
  }
  to {
    opacity: 1;
    transform: translateY(0) scale(1);
  }
}
</style>

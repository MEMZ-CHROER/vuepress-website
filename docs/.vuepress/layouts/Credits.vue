<script setup lang="ts">
import { onMounted, ref } from 'vue'

const particleContainer = ref<HTMLElement | null>(null)

onMounted(() => {
  const container = particleContainer.value
  if (!container) return
  const colors = ['#6366f1','#a855f7','#ec4899','#22c55e','#f59e0b']
  for (let i = 0; i < 30; i++) {
    const p = document.createElement('div')
    p.className = 'particle'
    const size = Math.random() * 6 + 2
    p.style.cssText =
      `width:${size}px;height:${size}px;` +
      `left:${Math.random()*100}%;bottom:-${Math.random()*20}px;` +
      `background:${colors[Math.floor(Math.random()*colors.length)]};` +
      `opacity:${Math.random()*0.5+0.2};` +
      `animation-duration:${Math.random()*15+10}s;` +
      `animation-delay:${Math.random()*10}s;`
    container.appendChild(p)
  }
  setTimeout(() => {
    const hero = document.getElementById('hero')
    if (hero) hero.classList.add('loaded')
  }, 100)
})
</script>

<template>
  <div class="credits-bg" ref="particleContainer"></div>

  <div class="credits-page">
    <div class="hero-section" id="hero">
      <span class="hero-icon">💜</span>
      <h1 class="hero-title">Credits</h1>
      <p class="hero-subtitle">
        <span class="typewriter-text">每一个帮助过这个项目的名字，都值得被记住。</span>
      </p>
    </div>

    <div class="section-divider"></div>

    <div class="founders-section">
      <div class="section-label"><span>👑</span> Founders / Creator</div>
      <div class="founders-grid">
        <div class="founder-card">
          <div class="founder-avatar">🧑‍💻</div>
          <div class="founder-name">Lxy</div>
          <div class="founder-role">Founder &amp; Lead Developer</div>
          <div class="founder-desc">写代码、画坦克、种枇杷。<br>这个项目的一切始于一个想法。</div>
          <div class="founder-tags">
            <span class="founder-tag">全栈开发</span>
            <span class="founder-tag">手绘坦克</span>
            <span class="founder-tag">C++ 讲师</span>
          </div>
        </div>
        <div class="founder-card">
          <div class="founder-avatar">🎨</div>
          <div class="founder-name">CoreAmigo</div>
          <div class="founder-role">Co-Founder &amp; Designer</div>
          <div class="founder-desc">负责美术与创意方向。<br>核巨啸的坦克们背后的灵魂画手。</div>
          <div class="founder-tags">
            <span class="founder-tag">视觉设计</span>
            <span class="founder-tag">坦克图鉴</span>
            <span class="founder-tag">艺术指导</span>
          </div>
        </div>
      </div>
    </div>

    <div class="section-divider"></div>

    <div class="thanks-section">
      <div class="section-label"><span>🙏</span> Special Thanks</div>
      <div class="thanks-grid">
        <div class="thank-item">
          <div class="thank-icon">⭐</div>
          <div class="thank-info">
            <div class="thank-name">Someone's Name</div>
            <div class="thank-contribution">在 XX 方面给予了重要帮助</div>
          </div>
        </div>
        <div class="thank-item">
          <div class="thank-icon">💡</div>
          <div class="thank-info">
            <div class="thank-name">Another Person</div>
            <div class="thank-contribution">提供了关键的想法和建议</div>
          </div>
        </div>
        <div class="thank-item">
          <div class="thank-icon">🛠️</div>
          <div class="thank-info">
            <div class="thank-name">Helper Name</div>
            <div class="thank-contribution">修复了多个重要 Bug</div>
          </div>
        </div>
        <div class="thank-item">
          <div class="thank-icon">🌟</div>
          <div class="thank-info">
            <div class="thank-name">Name Here</div>
            <div class="thank-contribution">长期以来的支持与陪伴</div>
          </div>
        </div>
      </div>
    </div>

    <div class="section-divider"></div>

    <div class="tech-section">
      <div class="section-label"><span>⚡</span> Tech Stack</div>
      <div class="tech-grid">
        <span class="tech-badge"><span class="tech-dot" style="background:#42b883"></span>VuePress 2</span>
        <span class="tech-badge"><span class="tech-dot" style="background:#ff6b6b"></span>theme-hope</span>
        <span class="tech-badge"><span class="tech-dot" style="background:#3178c6"></span>TypeScript</span>
        <span class="tech-badge"><span class="tech-dot" style="background:#f7df1e"></span>JavaScript</span>
        <span class="tech-badge"><span class="tech-dot" style="background:#00d4aa"></span>Cloudflare Worker</span>
        <span class="tech-badge"><span class="tech-dot" style="background:#336791"></span>D1 Database</span>
        <span class="tech-badge"><span class="tech-dot" style="background:#7289da"></span>GitHub API</span>
        <span class="tech-badge"><span class="tech-dot" style="background:#06b6d4"></span>Vercel</span>
        <span class="tech-badge"><span class="tech-dot" style="background:#a855f7"></span>WeAuth</span>
        <span class="tech-badge"><span class="tech-dot" style="background:#22c55e"></span>Mermaid</span>
        <span class="tech-badge"><span class="tech-dot" style="background:#eab308"></span>KaTeX</span>
        <span class="tech-badge"><span class="tech-dot" style="background:#f97316"></span>PhotoSwipe</span>
      </div>
    </div>

    <div class="section-divider"></div>

    <div class="credits-footer">
      <p>Made with 💜 by Lxy &amp; CoreAmigo · © 2026</p>
      <a href="https://github.com/MEMZ-CHROER/vuepress-website"
         class="gitHub-btn" target="_blank" rel="noopener">
        ⭐ Star on GitHub
      </a>
    </div>
  </div>
</template>

<style scoped>
@keyframes float { 0%,100%{transform:translateY(0)} 50%{transform:translateY(-12px)} }
@keyframes glow-pulse { 0%,100%{box-shadow:0 0 20px rgba(99,102,241,.3)} 50%{box-shadow:0 0 40px rgba(99,102,241,.7),0 0 80px rgba(99,102,241,.3)} }
@keyframes gradient-shift { 0%{background-position:0% 50%} 50%{background-position:100% 50%} 100%{background-position:0% 50%} }
@keyframes fade-in-up { from{opacity:0;transform:translateY(30px)} to{opacity:1;transform:translateY(0)} }
@keyframes typewriter { from{width:0} to{width:100%} }
@keyframes blink-caret { from,to{border-color:transparent} 50%{border-color:#6366f1} }
@keyframes particle-float { 0%{transform:translateY(0) rotate(0deg);opacity:0} 10%{opacity:1} 90%{opacity:1} 100%{transform:translateY(-80vh) rotate(720deg);opacity:0} }
@keyframes shimmer { 0%{background-position:-200% center} 100%{background-position:200% center} }

.credits-bg { position:fixed;inset:0;z-index:-1;overflow:hidden;background:linear-gradient(135deg,#0a0a1a 0%,#1a1a3e 50%,#0d0d20 100%) }
.particle { position:absolute;border-radius:50%;pointer-events:none;animation:particle-float linear infinite }
.credits-page { max-width:900px;margin:0 auto;padding:60px 24px 80px }
.hero-section { text-align:center;padding:40px 0 60px;animation:fade-in-up 0.8s ease-out }
.hero-icon { font-size:64px;display:block;margin-bottom:16px;animation,float:3s ease-in-out infinite }
.hero-title { font-size:clamp(36px,8vw,64px);font-weight:900;background:linear-gradient(135deg,#6366f1,#a855f7,#ec4899,#f59e0b);background-size:300% 300%;-webkit-background-clip:text;-webkit-text-fill-color:transparent;background-clip:text;animation:gradient-shift 4s ease infinite;letter-spacing:-1px;line-height:1.2 }
.hero-subtitle { color:#94a3b8;font-size:18px;margin-top:12px;font-family:'JetBrains Mono',monospace }
.typewriter-text { display:inline-block;overflow:hidden;white-space:nowrap;border-right:3px solid #6366f1;animation:typewriter 2s steps(30) forwards,blink-caret 0.75s step-end infinite;width:0 }
.hero-section.loaded .typewriter-text { width:100% }
.section-divider { height:2px;margin:48px 0;background:linear-gradient(90deg,transparent,#6366f1,#a855f7,#ec4899,transparent);background-size:200% 100%;animation:shimmer 3s linear infinite;border-radius:2px }
.section-label { display:inline-flex;align-items:center;gap:8px;font-size:12px;font-weight:700;letter-spacing:3px;text-transform:uppercase;color:#6366f1;margin-bottom:32px;padding:6px 16px;background:rgba(99,102,241,.1);border:1px solid rgba(99,102,241,.3);border-radius:100px }
.founders-section { animation:fade-in-up 0.8s ease-out 0.2s both }
.founders-grid { display:grid;grid-template-columns:repeat(auto-fit,minmax(280px,1fr));gap:24px }
.founder-card { position:relative;background:linear-gradient(145deg,rgba(30,30,60,.8),rgba(20,20,40,.9));border:1px solid rgba(99,102,241,.3);border-radius:20px;padding:40px 32px;text-align:center;transition:all 0.4s cubic-bezier(.4,0,.2,1);overflow:hidden }
.founder-card::before { content:'';position:absolute;inset:0;background:radial-gradient(circle at 50% 0%,rgba(99,102,241,.15),transparent 60%);opacity:0;transition:opacity 0.4s ease }
.founder-card:hover { transform:translateY(-8px);border-color:rgba(99,102,241,.6);animation:glow-pulse 2s ease-in-out infinite }
.founder-card:hover::before { opacity:1 }
.founder-avatar { width:88px;height:88px;border-radius:50%;margin:0 auto 20px;background:linear-gradient(135deg,#6366f1,#a855f7);display:flex;align-items:center;justify-content:center;font-size:36px;position:relative;z-index:1;border:3px solid rgba(255,255,255,.1) }
.founder-name { font-size:22px;font-weight:800;color:#f1f5f9;margin-bottom:6px;position:relative;z-index:1 }
.founder-role { font-size:13px;color:#a855f7;font-weight:600;letter-spacing:1px;text-transform:uppercase;margin-bottom:16px;position:relative;z-index:1 }
.founder-desc { font-size:14px;color:#94a3b8;line-height:1.7;position:relative;z-index:1 }
.founder-tags { display:flex;flex-wrap:wrap;gap:6px;justify-content:center;margin-top:16px;position:relative;z-index:1 }
.founder-tag { font-size:11px;padding:3px 10px;background:rgba(99,102,241,.15);border:1px solid rgba(99,102,241,.3);border-radius:100px;color:#a5b4fc;font-weight:500 }
.thanks-section { animation:fade-in-up 0.8s ease-out 0.4s both }
.thanks-grid { display:grid;grid-template-columns:repeat(auto-fit,minmax(240px,1fr));gap:16px }
.thank-item { display:flex;align-items:center;gap:14px;padding:16px 20px;background:rgba(30,30,60,.5);border:1px solid rgba(99,102,241,.15);border-radius:12px;transition:all 0.3s ease }
.thank-item:hover { background:rgba(99,102,241,.1);border-color:rgba(99,102,241,.4);transform:translateX(6px) }
.thank-icon { font-size:24px;width:44px;height:44px;display:flex;align-items:center;justify-content:center;background:rgba(99,102,241,.15);border-radius:10px;flex-shrink:0 }
.thank-info { flex:1;min-width:0 }
.thank-name { font-size:15px;font-weight:700;color:#e2e8f0;margin-bottom:2px }
.thank-contribution { font-size:12px;color:#64748b;white-space:nowrap;overflow:hidden;text-overflow:ellipsis }
.tech-section { animation:fade-in-up 0.8s ease-out 0.6s both }
.tech-grid { display:flex;flex-wrap:wrap;gap:10px }
.tech-badge { display:inline-flex;align-items:center;gap:8px;padding:8px 16px;background:rgba(30,30,60,.6);border:1px solid rgba(99,102,241,.2);border-radius:8px;font-size:13px;font-weight:600;color:#cbd5e1;transition:all 0.25s ease;cursor:default }
.tech-badge:hover { background:rgba(99,102,241,.2);border-color:#6366f1;color:#fff;transform:scale(1.05) }
.tech-dot { width:8px;height:8px;border-radius:50%;flex-shrink:0 }
.credits-footer { text-align:center;padding:40px 0 20px;color:#475569;font-size:13px;animation:fade-in-up 0.8s ease-out 0.8s both }
.credits-footer a { color:#6366f1;text-decoration:none }
.credits-footer a:hover { text-decoration:underline }
.gitHub-btn { display:inline-flex;align-items:center;gap:8px;padding:10px 24px;background:linear-gradient(135deg,#6366f1,#a855f7);color:#fff !important;border-radius:100px;font-weight:700;font-size:14px;text-decoration:none !important;transition:all 0.3s ease;margin-top:16px }
.gitHub-btn:hover { transform:scale(1.05);box-shadow:0 8px 24px rgba(99,102,241,.4);text-decoration:none !important }
@media (max-width:640px) { .founders-grid,.thanks-grid { grid-template-columns:1fr } .hero-title { font-size:36px } }
</style>

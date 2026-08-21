import { defineClientConfig, resolveRoute } from "vuepress/client";
import MyLayout from "./layouts/Layout.vue";
import QuoteOfTheDay from "./components/QuoteOfTheDay.vue";
import Comment from "./components/Comment.vue";
import EasterEgg from "./components/EasterEgg.vue";
import StatsPanel from "./components/StatsPanel.vue";
import Upload from "./components/upload.vue";
import LxySortDemo from "./components/LxySortDemo.vue";

export default defineClientConfig({
  layouts: {
    Layout: MyLayout,
  },
  enhance: ({ app, router }) => {
    app.component("QuoteOfTheDay", QuoteOfTheDay);
    app.component("Comment", Comment);
    app.component("EasterEgg", EasterEgg);
    app.component("StatsPanel", StatsPanel);
    app.component("Upload", Upload);
    app.component("LxySortDemo", LxySortDemo);

    // ==== clean URL 目录兜底（PR #1720 项目侧方案）====
    // 访问无后缀路径（如 /about）时：
    // 1. /about.html（about.md）存在 → 放行（同名文件优先）
    // 2. 仅 /about/（目录 index）存在 → 重定向到 /about/
    // 3. 都没有 → 保持 404
    router.beforeEach((to) => {
      const { path } = to;
      if (path !== "/" && !path.endsWith("/") && !/^\/.+\.[a-zA-Z0-9]+$/.test(path)) {
        const fileRoute = resolveRoute(`${path}.html`);
        const dirRoute = resolveRoute(`${path}/`);
        if (fileRoute.notFound && !dirRoute.notFound) {
          return `${path}/`;
        }
      }
      return true;
    });
  },
});

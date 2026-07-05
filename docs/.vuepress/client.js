import { defineClientConfig } from 'vuepress/client'
import Article from './layouts/Article.vue'
import Category from './layouts/Category.vue'
import Tag from './layouts/Tag.vue'
import Timeline from './layouts/Timeline.vue'
import MyLayout from './layouts/Layout.vue'
import QuoteOfTheDay from './components/QuoteOfTheDay.vue'
import Comment from './components/Comment.vue'
import EasterEgg from './components/EasterEgg.vue'
import StatsPanel from './components/StatsPanel.vue'

export default defineClientConfig({
  // we provide some blog layouts
  layouts: {
    Layout: MyLayout,
    Article,
    Category,
    Tag,
    Timeline,
  },
  enhance: ({ app }) => {
    app.component('QuoteOfTheDay', QuoteOfTheDay)
    app.component('Comment', Comment)
    app.component('EasterEgg', EasterEgg)
    app.component('StatsPanel', StatsPanel)
  },
})

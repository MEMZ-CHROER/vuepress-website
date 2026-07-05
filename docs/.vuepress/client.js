import { defineClientConfig } from 'vuepress/client'
import Article from './layouts/Article.vue'
import Category from './layouts/Category.vue'
import Tag from './layouts/Tag.vue'
import Timeline from './layouts/Timeline.vue'
import QuoteOfTheDay from './components/QuoteOfTheDay.vue'
import Comment from './components/Comment.vue'
import Post from './layouts/Post.vue'

export default defineClientConfig({
  // we provide some blog layouts
  layouts: {
    Article,
    Category,
    Tag,
    Timeline,
    Post,
  },
  enhance: ({ app }) => {
    app.component('QuoteOfTheDay', QuoteOfTheDay)
    app.component('Comment', Comment)
  },
})

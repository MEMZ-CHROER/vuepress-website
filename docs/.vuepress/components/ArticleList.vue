<script setup>
defineProps({
  /** Article items */
  items: {
    type: Array,
    required: true,
  },
  /** Whether is timeline or not */
  isTimeline: Boolean,
})
</script>

<template>
  <div class="article-wrapper">
    <div v-if="!items.length" class="empty-state">
      <div class="empty-icon">📝</div>
      <p>暂无内容</p>
    </div>

    <article
      v-for="{ info, path } in items"
      :key="path"
      class="article"
      @click="$router.push(path)"
    >
      <div class="article-meta">
        <span v-if="info.sticky" class="sticky-badge">📌 置顶</span>
        <span v-if="info.date" class="date-badge">
          {{ new Date(info.date).toLocaleDateString('zh-CN', { year: 'numeric', month: 'long', day: 'numeric' }) }}
        </span>
      </div>

      <header class="title">
        {{
          (isTimeline ? `${new Date(info.date).toLocaleDateString()}: ` : '') +
          info.title
        }}
      </header>

      <hr />

      <div v-if="info.author || info.date || info.category || info.tag" class="article-info">
        <span v-if="info.author" class="author">
          <span class="info-icon">👤</span> {{ info.author }}
        </span>

        <span v-if="info.date && !isTimeline" class="date">
          <span class="info-icon">📅</span> {{ new Date(info.date).toLocaleDateString() }}
        </span>

        <span v-if="info.category" class="category">
          <span class="info-icon">📂</span> {{ info.category.join(', ') }}
        </span>

        <span v-if="info.tag" class="tag">
          <span class="info-icon">🏷️</span> {{ info.tag.join(', ') }}
        </span>
      </div>

      <div v-if="info.excerpt" class="excerpt" v-html="info.excerpt" />
    </article>
  </div>
</template>

<style lang="scss" scoped>
.article-wrapper {
  max-width: 800px;
  margin: 0 auto;
  padding: 1.5rem 0;
}

.empty-state {
  text-align: center;
  padding: 3rem 1rem;
  color: var(--c-text-lighter);

  .empty-icon {
    font-size: 3rem;
    margin-bottom: 0.5rem;
  }

  p {
    font-size: 1rem;
  }
}

.article {
  position: relative;
  box-sizing: border-box;
  width: 100%;
  margin: 0 auto 1.5rem;
  padding: 1.5rem 1.8rem;
  border: 1px solid var(--c-border);
  border-radius: var(--radius-md);
  background: var(--c-bg-lighter);
  color: var(--c-text);
  text-align: start;
  cursor: pointer;
  transition: all var(--transition-normal, 0.3s cubic-bezier(0.4, 0, 0.2, 1));
  box-shadow: var(--shadow-card, 0 1px 3px rgba(0, 0, 0, 0.06));
  overflow: hidden;

  &::before {
    content: '';
    position: absolute;
    top: 0;
    left: 0;
    width: 4px;
    height: 0;
    background: linear-gradient(180deg, var(--c-brand), var(--c-brand-light));
    transition: height 0.3s cubic-bezier(0.4, 0, 0.2, 1);
    border-radius: 0 0 2px 2px;
  }

  &:hover {
    transform: translateY(-3px);
    box-shadow: var(--shadow-card-hover, 0 10px 25px rgba(0, 0, 0, 0.08));
    border-color: rgba(99, 102, 241, 0.25);

    &::before {
      height: 100%;
    }

    .title {
      color: var(--c-brand);
    }
  }

  .article-meta {
    display: flex;
    align-items: center;
    gap: 0.5rem;
    margin-bottom: 0.3rem;
  }

  .sticky-badge {
    display: inline-flex;
    align-items: center;
    font-size: 0.75rem;
    font-weight: 600;
    color: #d97706;
    background: rgba(217, 119, 6, 0.1);
    padding: 0.1rem 0.5rem;
    border-radius: 20px;
  }

  .date-badge {
    font-size: 0.75rem;
    color: var(--c-text-lighter);
  }

  .title {
    position: relative;
    display: inline-block;
    font-size: 1.3rem;
    font-weight: 700;
    line-height: 2rem;
    color: var(--c-text-dark);
    transition: color 0.2s cubic-bezier(0.4, 0, 0.2, 1);

    &::after {
      content: '';
      position: absolute;
      bottom: 0;
      inset-inline-start: 0;
      width: 100%;
      height: 2px;
      background: var(--c-brand);
      visibility: hidden;
      transition: transform 0.3s ease-in-out;
      transform: scaleX(0);
    }

    &:hover::after {
      visibility: visible;
      transform: scaleX(1);
    }
  }

  hr {
    border: none;
    height: 1px;
    background: linear-gradient(90deg, var(--c-border), transparent);
    margin: 0.6rem 0 0.8rem;
  }

  .article-info {
    display: flex;
    flex-wrap: wrap;
    gap: 0.4rem 0.8rem;
    margin-top: 0.3rem;

    > span {
      display: inline-flex;
      align-items: center;
      gap: 2px;
      font-size: 0.8rem;
      color: var(--c-text-lighter);
      line-height: 1.8;
      background: var(--c-bg-light);
      padding: 0.1rem 0.6rem;
      border-radius: 20px;
      transition: background 0.2s cubic-bezier(0.4, 0, 0.2, 1);

      &:hover {
        background: rgba(99, 102, 241, 0.1);
        color: var(--c-brand);
      }

      .info-icon {
        font-size: 0.75rem;
      }
    }
  }

  .excerpt {
    margin-top: 0.8rem;
    font-size: 0.92rem;
    line-height: 1.7;
    color: var(--c-text-light);

    :deep(h1) { display: none; }
    :deep(h2) { font-size: 1.2em; margin: 0.6rem 0; }
    :deep(h3) { font-size: 1.1em; margin: 0.5rem 0; }
    :deep(p) { margin: 0.4rem 0; }
  }
}

@media (max-width: 719px) {
  .article {
    padding: 1.2rem;
    border-radius: var(--radius-sm, 6px);

    .title {
      font-size: 1.1rem;
    }
  }
}
</style>

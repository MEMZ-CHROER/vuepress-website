<script setup lang="ts">
import { onMounted } from 'vue'
import CreditsPage from './CreditsContent.vue'

onMounted(() => {
  // 粒子在子组件内部创建，这里只用来保证生命周期挂载
})
</script>

<CreditsPage />

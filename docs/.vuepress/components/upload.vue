<template>
  <div v-if="!!filename">
    <input
      type="file"
      :ref="filename"
      class="fileInput"
      accept=".*"
      @change="uploadFile"
    />
    <div class="pulse-box" :style="style">
      <div v-if="loading" class="pulse-full" :style="getBG"></div>
      <div v-if="loading" class="pulse-container">
        <div class="pulse-bubble pulse-bubble-1"></div>
        <div class="pulse-bubble pulse-bubble-2"></div>
        <div class="pulse-bubble pulse-bubble-3"></div>
      </div>
      <pre
        v-if="!!content"
        class="language-cpp"
      ><code v-html="highlightedCode"></code></pre>
    </div>
    <div class="buttonLine" v-if="button">
      <div class="fileButton" @click="upload">{{ buttonText }}</div>
    </div>
  </div>
</template>
<script>
import hljs from "highlight.js";
import "highlight.js/styles/atom-one-dark.css";
import md5 from "md5";
export default {
  props: {
    filename: {
      type: String,
      default: "1.cpp",
    },
    button: {
      type: Boolean,
      default: false,
    },
  },
  computed: {
    highlightedCode() {
      return hljs.highlightAuto(this.content ?? "").value;
    },
    style() {
      if (this.loading) {
        return {
          "min-height": "120px",
        };
      }
      return {};
    },
    getBG() {
      if (this.content) {
        return {
          "background-color": "white",
        };
      }
      return {
        "background-color": "transparent",
      };
    },
  },
  data() {
    return {
      buttonText: "加载中...",
      content: undefined,
      prefix: "",
      loading: false,
    };
  },
  mounted() {
    if (process.env.NODE_ENV !== "development") {
      this.prefix = "https://api.liuxiyu.cn";
    }
    this.getFile();
  },
  methods: {
    inWindow() {
      return typeof window !== "undefined";
    },
    getFile() {
      this.loading = true;
      fetch(`${this.prefix}/api/search/${this.filename}`, {
        method: "GET",
      })
        .then((response) => response.json())
        .then(async (data) => {
          this.loading = false;
          if (data?.code?.toString() !== "200") {
            if (this.inWindow()) {
              window.alert("文件查询失败，请联系管理员");
            }
            console.error("文件查询失败:", data);
            return;
          }
          if (data?.data) {
            this.loading = true;
            this.buttonText = "更新答案";
            const response = await fetch(
              `${this.prefix}/api/download/${this.filename}`
            );
            if (!response.ok) {
              this.loading = false;
              if (this.inWindow()) {
                window.alert("文件下载失败，请联系管理员");
              }
              console.error("文件下载失败:", response);
              return;
            }
            const cppContent = await response.json();
            this.content = cppContent?.data;
            this.loading = false;
            return;
          }
          this.buttonText = "上传答案";
          return;
        })
        .catch((error) => {
          this.loading = true;
          this.buttonText = "重新查询";
          if (this.inWindow()) {
            window.alert("文件查询失败，请联系管理员");
          }
          console.error("文件查询失败:", error);
        });
    },
    upload() {
      if (this.loading) {
        return;
      }
      if (this.buttonText === "重新查询") {
        this.getFile();
        return;
      }
      this.$refs[this.filename].click();
    },
    uploadFile(e) {
      const file = e.target.files[0];
      if (!file) {
        if (this.inWindow()) {
          window.alert("请选择文件");
        }
        return;
      }

      if (file.size === 0) {
        if (this.inWindow()) {
          window.alert("文件内容不能为空");
        }
        return;
      }

      if (file.size > 1024 * 1024 * 1) {
        if (this.inWindow()) {
          window.alert("文件大小不能超过1M");
        }
        return;
      }

      this.buttonText = "上传中...";
      this.loading = true;
      // 清空input缓存
      this.$refs[this.filename].value = "";

      const uploadUrl = `${this.prefix}/api/upload?filename=${encodeURIComponent(this.filename)}&username=${md5("chaosmile")}&password=${md5("chaosmilepassword")}`;
      file
        .text()
        .then((text) =>
          fetch(uploadUrl, {
            method: "POST",
            headers: { "Content-Type": "text/plain" },
            body: text,
          })
        )
        .then((response) => response.json())
        .then((data) => {
          this.loading = false;
          if (data?.code?.toString() !== "200") {
            if (this.inWindow()) {
              window.alert("上传失败，请重新上传");
            }
            this.buttonText = "上传失败，请重新上传";
            return;
          }
          this.getFile();
          return;
        })
        .catch((error) => {
          console.error("上传文件时出现错误:", error);
          this.loading = false;
          this.buttonText = "重新上传";
          if (this.inWindow()) {
            window.alert("上传失败，请重新上传");
          }
          this.buttonText = "上传失败，请重新上传";
        });
    },
  },
};
</script>
<style scoped>
.fileInput {
  display: none;
}
.buttonLine {
  text-align: right;
  padding-right: 1em;
}
.fileButton {
  display: inline-block;
  cursor: pointer;
  color: white;
  background-color: #3eaf7c;
  padding: 6px 18px;
  border-radius: 6px;
  box-shadow: 0 2px 0 #3eaf7c;
  transition: all 0.1s ease-in-out;
}
.fileButton:hover {
  background-color: #3eaf7cdd;
  box-shadow: 0 2px 0 #3eaf7c;
}

.fileContent {
  font-size: 1.2em;
}

.pulse-box {
  width: 100%;
  height: 100%;
  position: relative;
}

.pulse-full {
  width: 100%;
  height: 100%;
  position: absolute;
  top: 0;
  left: 0;
  opacity: 0.3;
  z-index: 2;
}

.pulse-container {
  position: absolute;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  width: 120px;
  display: flex;
  justify-content: space-between;
  align-items: center;
  z-index: 3;
  height: 120px;
}

.pulse-bubble {
  width: 20px;
  height: 20px;
  border-radius: 50%;
  background-color: #3eaf7c;
}

.pulse-bubble-1 {
  animation: pulse 0.4s ease 0s infinite alternate;
}
.pulse-bubble-2 {
  animation: pulse 0.4s ease 0.2s infinite alternate;
}
.pulse-bubble-3 {
  animation: pulse 0.4s ease 0.4s infinite alternate;
}
@keyframes pulse {
  from {
    opacity: 1;
    transform: scale(1);
  }
  to {
    opacity: 0.25;
    transform: scale(0.75);
  }
}
</style>

---
title: 频率里的字
sidebar: false
comment: false
description: 一段关于傅里叶变换的笔记，以及一段只肯在频谱里显形的信号。
---

# 频率里的字

> 有些东西在时域里看不见，只有在频率里才肯开口。

这是一个不太像教程的页面。它关于**傅里叶变换**，也关于一段被藏起来的信号。

## 先听一段信号

下面是一个 3 秒左右的 WAV。它听起来只是几组忽高忽低的电子音，没有旋律，也没有歌词。

但如果你把它拆成频率，它会说话。

<FourierLab />

如果你在浏览器里看不清，可以把 `signal.wav` 下载下来，用 **Audacity**（切到「频谱图」视图）、**Sonic Visualiser**（Spectrogram）或任何能画 STFT 频谱的工具打开。把频率轴放在 0–3 kHz，时间轴拉满。

::: tip 提示
横轴是时间，纵轴是频率。亮度表示该频率在该时刻的能量。
:::

## 傅里叶变换到底在做什么

一句话版本：**傅里叶变换把“随时间变化的信号”重新描述成“由哪些频率组成”。**

它像一个棱镜。白光穿过棱镜，被拆成不同颜色的光；一段声音穿过傅里叶变换，被拆成不同频率的正弦波。人耳其实一直在做类似的事——听到一个和弦时，你能分辨出里面有哪些音高；傅里叶变换只是把这件事写成了数学。

更严格地说，傅里叶变换是一组**基变换**：它把信号从「时间」这组基，投影到「复指数函数 $e^{i\omega t}$」这组基上。正变换是与基函数做内积（所以有负号），逆变换是把基函数按系数重新加起来。这也是为什么正变换和逆变换长得几乎一样，以及为什么它保持能量不变（帕塞瓦尔定理）。

对离散信号，计算机算的是 **DFT**（离散傅里叶变换）；**FFT** 只是把 DFT 从 $O(N^2)$ 加速到 $O(N\log N)$ 的算法，不改变结果。N = 1024 时，运算量大约从三千万次降到三万次。

## 三个常见的误解

**1. 傅里叶变换能告诉你“某个频率在什么时候出现”。**

不能。严格的傅里叶变换是对整段信号积分，得到的是全局频谱。要知道频率随时间怎么变，需要**短时傅里叶变换（STFT）**、**小波变换**或**频谱图**。本页的频谱图就是一种 STFT 的可视化。

**2. “带限信号”是物理世界里真实存在的东西。**

Slepian 等人都指出过：严格带限、严格时限都是数学模型的构造。现实信号不可能被完全带限，验证它需要无限高的频率和无限长的观测时间。把模型假设当成物理实在，是信号处理里很常见的一类误解。

**3. 负频率是“不存在的频率”。**

负频率是复指数表示的自然结果。对实信号，频谱满足共轭对称，负半轴是正半轴的镜像；工程上经常把负半轴折叠掉，变成“单边频谱”，但那是表示习惯，不是物理上多出了一个反向旋转的频率。

## DFT / FFT / 小波，怎么选

| 工具 | 分辨率 | 适合 | 代价 |
| --- | --- | --- | --- |
| DFT / FFT | 频率分辨率固定 | 平稳信号、频谱分析、卷积加速 | 丢失时间定位 |
| STFT | 时频都有，窗口固定 | 语音、音乐、频谱图 | 时间/频率分辨率权衡 |
| 小波变换 | 低频频率分辨率高、高频时间分辨率高 | 瞬态、去噪、压缩、非平稳信号 | 实现与解释更复杂 |

一句话：**FFT 回答“有哪些频率”，频谱图回答“什么时候有哪些频率”，小波回答“哪个尺度上出现了什么”。**

## 回到这段信号

现在你大概知道该看哪里了。

那段 WAV 里的 7 个字母，是用 7 个固定频率的短音拼出来的：每个时间片里，哪些频率亮着，就对应点阵里的哪些像素。把频谱图横过来读，就是答案。

如果你已经读出来了，去 [PROJECT 666](/puzzle/) 把它交掉。

## 参考资料

- [Fourier transform — Wikipedia](https://en.wikipedia.org/wiki/Fourier_transform)
- [An Interactive Guide to the Fourier Transform — BetterExplained](https://betterexplained.com/articles/an-interactive-guide-to-the-fourier-transform/)
- [傅里叶变换：将时光凝固成音符 — 知乎](https://zhuanlan.zhihu.com/p/480251944)
- [频率空间：信号与系统的隐藏语言 — Bohrium](https://whu.bohrium.com/sciencepedia/feynman/keyword/frequency_space)
- [time–frequency representation — ScienceDirect](https://www.sciencedirect.com/topics/computer-science/time-frequency-representation)
- [On the Concept of Frequency in Signal Processing — arXiv:2105.13177](https://arxiv.org/abs/2105.13177)
- [Principles of Digital Communications I, Lecture S15 — MIT OCW](https://ocw.mit.edu/courses/6-450-principles-of-digital-communications-i-fall-2006/)
- [Instantaneous Frequency and Time-Frequency Analysis: Myths, Misconceptions and Mysteries — IEEE](https://site.ieee.org/nvwb-sit/event/comsoc-signal-processing-instantaneous-frequency-and-time-frequency-analysis-myths-misconceptions-and-mysteries/)
- [Wavelets: “Beyond Comparison” — D. L. Fugal](http://www.aticourses.com/Wavelets%20-%20Beyond%20Comparison%20by%20Lee%20Fugal.pdf)

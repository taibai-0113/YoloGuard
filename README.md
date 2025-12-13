# YoloGuard - 智能驾驶疲劳检测系统（v1.0）

> 基于 C++ Qt 6.5 与 OpenCV 4.10 开发的高性能疲劳驾驶检测应用。

## 功能特性

- **高性能推理**: 针对无独显笔记本优化，采用 320x320 输入与抽帧策略。
- **实时监测**: 支持 USB 摄像头热插拔，低延迟显示。
- **多级报警**: 支持视觉红框闪烁与系统蜂鸣器声音报警。
- **数据持久化**: 自动保存报警历史记录至本地文件。
- **动态配置**: 支持实时调节检测灵敏度 (Confidence Threshold)。

## 环境要求

- **IDE**: Visual Studio 2022
- **Framework**: Qt 6.5.3 (MSVC 2019 64-bit)
- **Library**: OpenCV 4.10 (DNN Module)

## 使用说明

1.  **下载源码**: Clone 本仓库到本地。
2.  **获取模型**: 请在右侧 **Releases** 页面下载 `best.onnx` 模型文件。
3.  **放置模型**: 将下载的 `best.onnx` 放入项目根目录（与 `.cpp` 文件同级）。
4.  **编译运行**: 使用 Visual Studio 打开 `.sln` 文件，切换至 **Release** 模式运行。

---
*Powered by YOLOv7 & Qt*

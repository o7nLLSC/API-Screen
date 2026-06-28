# 截图工具

基于 Windows API 实现的快速截图连发工具，通过模拟键盘按键实现批量截图功能。

## 功能特性

- ✅ 使用 Windows API `keybd_event` 模拟 PrintScreen 按键
- ✅ 鼠标右键触发批量截图
- ✅ HOME 键调整截图数量（1-99）
- ✅ 实时显示当前截图强度

## 技术原理

### 按键模拟

使用 Windows API `keybd_event` 模拟硬件按键动作：

```cpp
// 按下 PrintScreen
keybd_event(VK_SNAPSHOT, 0, 0, 0);
// 释放 PrintScreen
keybd_event(VK_SNAPSHOT, 0, KEYEVENTF_KEYUP, 0);
```

### 按键检测

使用 `GetAsyncKeyState` 检测按键状态（非阻塞方式）：

```cpp
// 检测鼠标右键是否按下
if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
```

## 编译方法

### 环境要求

- Visual Studio 2019/2022
- Windows 10/11


## 使用方法

1. **运行程序**：编译 双击 `你的.exe`
2. **触发截图**：按住鼠标右键，程序会自动连续截取指定数量的屏幕
3. **调整强度**：按 HOME 键，输入新的截图数量（默认 14）
4. **退出程序**：按 Ctrl+C 或关闭控制台窗口

### 默认配置

| 按键 | 功能 |
|------|------|
| 鼠标右键 | 触发批量截图 |
| HOME | 设置截图强度 |

## 代码结构

```
├── keybd_event.cpp    # 主程序
│   ├── g_jt_active       # 触发状态标志（防止重复触发）
│   ├── g_QiangDu         # 截图强度（原子变量）
│   ├── g_vKey_JT         # 触发按键（默认鼠标右键）
│   ├── ScreenshotBatch() # 批量截图函数
│   ├── ShowTipInfo()     # 显示提示信息
│   └── main()            # 主循环
```


## 许可证

MIT License

## 作者

o7nll

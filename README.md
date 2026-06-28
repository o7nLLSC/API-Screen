# 罗技动态链接库

基于对原版 `ghub_device.dll` 的逆向分析，重新实现的罗技驱动通信动态链接库。

## 功能特性

- ✅ 通过 NT API 直接与罗技内核驱动通信
- ✅ 支持鼠标移动、点击、滚轮操作
- ✅ 支持键盘按键模拟（包含完整的 74 个键位映射）
- ✅ 自动设备路径探测（9个索引 × 2个GUID组合）

## 编译方法

### 环境要求

- Visual Studio 2019/2022 (x64)
- Windows 10/11

### 编译命令

```bash
# 使用 Developer Command Prompt
cl /EHsc /Fe:ghub_device.dll /LD ghub_device.cpp
```

### CMakeLists.txt（可选）

```cmake
cmake_minimum_required(VERSION 3.14)
project(ghub_device)

set(CMAKE_CXX_STANDARD 17)
add_library(ghub_device SHARED ghub_device.cpp)
target_compile_options(ghub_device PRIVATE /EHsc)
```

## 导出函数

### 设备控制

| 函数名 | 返回类型 | 参数 | 说明 |
|--------|----------|------|------|
| `device_open` | `int` | 无 | 打开设备，返回 1 成功，0 失败 |
| `device_close` | `NTSTATUS` | 无 | 关闭设备 |
| `delay` | `int` | 无 | 返回延迟值（固定返回 2） |

### 鼠标操作

| 函数名 | 返回类型 | 参数 | 说明 |
|--------|----------|------|------|
| `moveR` | `char` | `int x`, `int y` | 相对移动鼠标 |
| `mouse_down` | `char` | `char button` | 鼠标按下（1=左键，2=右键） |
| `mouse_up` | `char` | 无 | 鼠标释放 |
| `scroll` | `char` | `int amount` | 滚轮滚动（正值向上） |

### 键盘操作

| 函数名 | 返回类型 | 参数 | 说明 |
|--------|----------|------|------|
| `key_down` | `char` | `const char* key` | 按下指定按键 |
| `key_up` | `char` | 无 | 释放所有按键 |

## 按键名称映射

支持以下按键名称（传入 `key_down` 函数）：

### 字母
`a`, `b`, `c`, `d`, `e`, `f`, `g`, `h`, `i`, `j`, `k`, `l`, `m`, `n`, `o`, `p`, `q`, `r`, `s`, `t`, `u`, `v`, `w`, `x`, `y`, `z`

### 数字
`1`, `2`, `3`, `4`, `5`, `6`, `7`, `8`, `9`, `0`

### 功能键
`f1` ~ `f24`, `printscreen`, `scroll_lock`, `pause`

### 方向键
`up`, `down`, `left`, `right`

### 控制键
`lctrl`, `rctrl`, `lshift`, `rshift`, `lalt`, `ralt`, `lwin`, `rwin`

### 其他
`enter`, `esc`, `back_space`, `tab`, `space`, `cap`, `minus`, `equal`, `square_bracket_left`, `square_bracket_right`, `back_slash`, `column`, `quote`, `back_tick`, `comma`, `period`, `slash`, `insert`, `home`, `page_up`, `del`, `end`, `page_down`, `numlock`, `apps`

### 小键盘
`numpad_div`, `numpad_mul`, `numpad_minus`, `numpad_plus`, `numpad_enter`, `numpad_0` ~ `numpad_9`, `numpad_dec`

## 使用示例

### C++ 调用示例

```cpp
#include <Windows.h>
#include <iostream>

typedef int(*fn_device_open)();
typedef void(*fn_key_down)(const char*);
typedef void(*fn_key_up)();
typedef void(*fn_moveR)(int, int);
typedef void(*fn_mouse_down)(int);
typedef void(*fn_mouse_up)();
typedef void(*fn_scroll)(int);
typedef void(*fn_device_close)();

int main() {
    HMODULE hDll = LoadLibraryA("ghub_device.dll");
    if (!hDll) {
        printf("加载失败\n");
        return -1;
    }

    fn_device_open device_open = (fn_device_open)GetProcAddress(hDll, "device_open");
    fn_key_down key_down = (fn_key_down)GetProcAddress(hDll, "key_down");
    fn_key_up key_up = (fn_key_up)GetProcAddress(hDll, "key_up");

    if (device_open() == 1) {
        key_down("printscreen");  // 按下截图键
        key_up();                // 释放按键
        
        device_close();
    }

    FreeLibrary(hDll);
    return 0;
}
```

### Python 调用示例（使用 ctypes）

```python
import ctypes

dll = ctypes.CDLL("ghub_device.dll")

dll.device_open()
dll.key_down(b"printscreen")
dll.key_up()
dll.device_close()
```

## 技术说明

### 设备路径格式

程序会自动尝试以下设备路径格式：

```
\??\ROOT#SYSTEM#0000#{1abc05c0-c378-41b9-9cef-df1aba82b015}
\??\ROOT#SYSTEM#0001#{df31f106-d870-453d-8fa1-ec8ab43fa1d2}
\??\ROOT#SYSTEM#0002#{1abc05c0-c378-41b9-9cef-df1aba82b015}
...
```

### IO 控制码

| 控制码 | 用途 |
|--------|------|
| `0x2A2010` | 鼠标操作 |
| `0x2A200C` | 键盘操作 |

### 输入缓冲区格式

**鼠标移动**（5 字节）：
```
[0] = 0
[1] = X 偏移量 (-127 ~ 127)
[2] = Y 偏移量 (-127 ~ 127)
[3-4] = 0
```

**键盘操作**（8 字节 HID 格式）：
```
[0] = 修饰键位标志 (Ctrl/Shift/Alt/Win)
[1] = 保留
[2] = 按键码
[3-7] = 保留
```

## 注意事项

1. 必须安装罗技 G Hub 驱动
2. 仅支持 x64 架构
3. 建议以管理员权限运行
4. `key_up` 无参数，会释放所有按键
5. 鼠标移动偏移量限制在 ±127，超过时自动分步移动

## 许可证

MIT License

## 作者

o7nll

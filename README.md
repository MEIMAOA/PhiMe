# PhiMe · Phigros 自动打歌 Mod（源码）

《Phigros》安卓版（Unity + IL2CPP）注入式原生 Mod，C++17 freestanding（无 STL、无异常/RTTI）。

> 本项目基于 / 引用了开源仓库 [melodycheese/PhiMe](https://github.com/melodycheese/PhiMe)。

这是**从零重建的源码**：原工程文件曾丢失，这里按反汇编 + 记忆重建了核心功能。注入入口 `libmain.so` 加载器沿用现有二进制，本仓库只放 Mod 本体 `libPhiMeMod.so` 的源码。

## 已完成的功能（v1.6.0 重建版）

- **一键解锁全部线索 / 曲绘 / 头像 / Igallta**
  - 头像 / 曲绘 / 线索（收藏品）走游戏钥匙系统：存档值是 `SaveManagement` 的 int，键名 `<kind>key<名字>`（3 头像 / 2 曲绘 / 1 线索·收藏品 / 0 歌曲）；写 1 + `GameInformation.CheckKeys()` 即解锁，重启不丢。
  - Igallta 走 `SaveManagement.SaveBool` 写隐藏曲解锁标记。
- **自动游玩**：hook `ClickControl/DragControl/FlickControl/HoldControl.Judge()`，在游戏自己判定前把音符标成 Perfect（`ChartNote.isJudged = true` + `realTime = nowTime`）。
- **自动打歌评级**：分数到达所选评级（V 960000 / S 920000 / A 880000 / B 820000 / C 700000）后停止判定，余下音符按 Miss 结算。

## 目录

| 路径 | 说明 |
|---|---|
| `src/il2cpp.cpp` | IL2CPP API 绑定（dlsym）+ 类/方法/字段查找 + `hookMethod` |
| `src/mod.h` | 类型、配置、功能声明 |
| `src/main.cpp` | 入口（constructor → 线程 → 延迟初始化） |
| `src/keys.cpp` | 线索/曲绘/头像/Igallta 一键解锁 |
| `src/autoplay.cpp` | 自动游玩 + 自动打歌评级 |
| `src/prefs.cpp` | PlayerPrefs 读写（`prefGetInt`/`prefSetInt`） |
| `src/log.cpp` | 日志（`__android_log_print`，tag `PhiMeMod`） |
| `build_android.ps1` | 构建脚本（LLVM clang++，aarch64） |
| `libs/` | 空桩 `.so`，只为让链接器记录 DT_NEEDED |

## 构建

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\build_android.ps1
# 产物: out\android\libPhiMeMod.so
```

需要 `C:\Program Files\LLVM\bin`（LLVM 23.1.5）。

## 使用（触发方式）

目前用 PlayerPrefs 触发（面板 UI 待补）：

| PlayerPrefs 键 | 值 | 作用 |
|---|---|---|
| `PhiMeMod.autoplay` | `1` | 开启自动游玩 |
| `PhiMeMod.autoRank` | `0`~`5` | 评级上限（0 不限，1 V / 2 S / 3 A / 4 B / 5 C） |
| `PhiMeMod.unlockIgallta` | `1` | 解锁 Igallta（第六章隐藏曲） |
| `PhiMeMod.unlockClues` | `1` | 解锁全部线索（收藏品，kind 1） |
| `PhiMeMod.unlockIllustrations` | `1` | 解锁全部曲绘（kind 2） |
| `PhiMeMod.unlockAvatars` | `1` | 解锁全部头像（kind 3） |

注入：把编译出的 `libPhiMeMod.so` 放到
`/sdcard/Android/media/com.PigeonGames.Phigros/PhiMeMod/libPhiMeMod.so`（或
`/sdcard/Android/data/com.PigeonGames.Phigros/files/PhiMeMod/`），重启游戏即可加载（加载器会优先读推送的 .so）。

## 声明

- 第三方 Mod，与 Pigeon Games 无关，仅供学习交流；本仓库不含任何游戏资源。
- 原仓库 [melodycheese/PhiMe](https://github.com/melodycheese/PhiMe) 现已无法访问，此处仍按原项目说明保留引用。

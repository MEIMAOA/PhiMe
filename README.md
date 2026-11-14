# PhiMe · Phigros 自动打歌 Mod

PhiMe 是一个注入到《Phigros》安卓版（Unity + IL2CPP）里的原生 Mod（C++17 freestanding，无 STL）。它在你自己的存档上工作：自动打歌、按评级自动收分、一键解锁隐藏曲/头像，并把成绩上传到本地与云端。

> 本项目基于 / 引用了开源仓库 [melodycheese/PhiMe](https://github.com/melodycheese/PhiMe)。

---

## 功能

- **自动游玩**：所有音符走游戏自己的 Perfect 判定，打击音、特效、连击都是原生的；自动游玩时屏蔽手动判定。
- **自动打歌评级**：可选 V / S / A / B / C，分数到达该评级所需分数后停止判定，结算就是所选评级。
- **Igallta 一键解锁**：写入游戏自己的解锁存档标记，第六章隐藏曲 EZ / HD / IN 直接可打（AT 仍需 IN 拿 S）。
- **全部头像解锁**：写入存档里的 Head portrait 钥匙（`3key<头像名>` = 1），账号页 84 个头像全部可选，重启不丢。
- **课题模式修复**：修好游戏自带的课题模式打第二首时崩溃/不开始的问题。
- **保存与云同步**：成绩照常保存到本地；登录并打开游戏的自动同步后，和正常游玩一样在结算时上传云存档。
- 练习模式（选段/循环/变速）、选歌界面镜像/练习/自动开关、皮肤、判定详情、B30 页、上隐条等。

## 版本

当前版本 **v1.5.8**。完整变更记录见 [CHANGELOG.md](CHANGELOG.md)。

## 使用

两种方式：

1. **直接安装签名好的完整 APK**（整个游戏 + 已注入的 Mod）。
   - 由于完整 APK 体积约 2.4 GB（超过 GitHub 单个文件 100 MB 上限），且包含《Phigros》原游戏本体，本仓库不放完整 APK。
   - 完整 APK 请向发布者索取（或从已安装的设备上 `adb pull` 拉取 `base.apk`）。

2. **手动注入**（已有游戏本体，只想替换 Mod）：
   - 用 APK 重打包工具，把本仓库 `android/arm64-v8a/libPhiMeMod.so` 替换进 APK 的 `lib/arm64-v8a/libPhiMeMod.so`；
   - 把原 `lib/arm64-v8a/libmain.so` 改名为 `libmain_orig.so`，再放入本仓库的 `android/arm64-v8a/libmain.so`（32 位同理放 `android/armeabi-v7a/libmain.so`）；
   - 重新签名安装即可。
   - 或者：不重打包，直接把 `libPhiMeMod.so` 推到设备 `/sdcard/Android/data/com.PigeonGames.Phigros/files/PhiMeMod/libPhiMeMod.so`，`libmain.so` 加载器会优先读取它。

> `libmain.so` 是注入用入口 shim：`JNI_OnLoad` 时 `dlopen` 加载 `libPhiMeMod.so`，再链回游戏原来的 `libmain.so`（重打包时被改名为 `libmain_orig.so`）。

## 仓库文件

| 路径 | 说明 |
|---|---|
| `android/arm64-v8a/libPhiMeMod.so` | Mod 本体（arm64） |
| `android/arm64-v8a/libmain.so` | 注入入口 shim（arm64） |
| `android/armeabi-v7a/libmain.so` | 注入入口 shim（32 位） |
| `README.md` | 本文件 |
| `CHANGELOG.md` | 版本变更 |

## 声明

- 本项目是《Phigros》的第三方 Mod，与 Pigeon Games 无关，仅供学习与交流。
- 本仓库不含任何《Phigros》游戏资源（曲包、音频、图片、游戏本体等），只放 Mod 自己的二进制与文档。
- 原版 PhiMe 仓库地址（[melodycheese/PhiMe](https://github.com/melodycheese/PhiMe)）现已无法访问（账号/仓库已不可见），此处仍按原项目说明保留引用。

## 注意

本仓库当前**只有编译后的二进制，源码暂缺**（原工程文件已丢失）。如需源码，请联系发布者。

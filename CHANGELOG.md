# Changelog

## v1.5.8
- 新增「解锁全部头像」：写入存档里的 Head portrait 钥匙（`3key<头像名>` = 1），账号页 84 个头像全部可选，重启不丢。

## v1.5.7
- 修复游戏自带「课题模式」打完第一首后第二首谱面出来但不开始 / 崩溃的问题。
  - 原因：Mod 的课题镜像开关遍历 `List<T>` 时把值类型列表当对象指针读，第二首数据被填上后 SIGSEGV。
  - 修复：遍历前校验列表元素类型（`LevelStartInfo[]`），不认识的列表跳过。

## v1.5.6
- 新增「解锁 Igallta」：用游戏自己的 `SaveManagement.SaveBool` 写入 `unlockFlagOfIgalltaEZ/HD/IN`、`VRUnlockFlagOfIgallta`、`重生KeyCompleted`，第六章隐藏曲 EZ / HD / IN 直接可打（AT 仍需 IN 拿 S）。

## v1.5.4
- 新增「自动打歌评级」：可选 V / S / A / B / C，分数到达该评级所需分数后停止判定。

## v1.5.3
- 保存与云同步：成绩照常保存到本地；登录并打开游戏自动同步后，结算时上传云存档；Mod 自己的数据（判定详情、课题成绩等）随云存档一起走。

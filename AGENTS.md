# GKI HWID 副本规则

接着上个窗口继续时，先读 `docs/当前会话交接.md`。

- `hwid/allowlist.txt` 是允许启动此内核的设备 HWID 白名单，一行一个。
- 修改白名单后必须重新构建内核；不要直接复用旧的 Image。
- 未明确要求时不要 push、不要 cancel 远程 workflow、不要覆盖原仓库。
- HWID 校验在内核 early boot 阶段执行，缺失或不在白名单会 panic 并在 1 秒后重启。

# MIMO 构建配置

`profiles/` 下的 12 个 `.mk` 文件是冻结实现的编译 profile：四个均衡 kernel 与三个后端的笛卡尔积各一份。它们只包含构建宏，不包含 cycle、instret、仿真输出或质量报告。

Makefile 会根据 `MIMO_VALIDATE_KERNEL`/`MIMO_PERF_KERNEL` 和 `MIMO_IMPL` 自动选择 profile，并将候选开关锁定为冻结值。不要手工把 profile 指向实验目录，也不要在同一 configuration 下增加第二个实现入口。

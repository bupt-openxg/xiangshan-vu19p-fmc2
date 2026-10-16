# 官方 Linux 内存镜像（昆明湖 kmh-v2）

## 获取

```bash
./sw/linux/fetch_image.sh        # 或由 flows/07_run_linux.sh 自动调用
```

- **官方直链**（随 XiangShan-doc 仓库公开托管）：
  `https://raw.githubusercontent.com/OpenXiangShan/XiangShan-doc/main/docs/integration/resources/data_kmh.zip`
- **出处文档**：[docs/integration/fpga/fpga.md](https://github.com/OpenXiangShan/XiangShan-doc/blob/master/docs/integration/fpga/fpga.md)
  （渲染页 <https://docs.xiangshan.cc/zh-cn/dev/integration/fpga/>，"实例昆明湖内存镜像txt"一行）
- **校验**：5,178,920 字节，`sha256 = 483e077e0ef8d4e8afc8c896273fa239f5f871ed5b38459de3fc7cab3671876c`
- 解压得到 `data.txt`（26,708,402 字节文本，即 13.3MB 内存映像，OpenSBI v1.5.1 + Linux 6.10.7 buildroot）
- ⚠️ 同目录的 `data.zip` 是**南湖(Nanhu)** 镜像、`data_dual_rc.zip` 是双核版，均不适用于本工程

## 为什么必须先生成"修正版镜像"

镜像内嵌 DTB 中 `serial@310b0000` 的 `clock-frequency = 50MHz`，
而本设计（env-scripts fpga_diff + 本板 AEConsole 时钟）**实际时钟为 25MHz**。
软件按 50MHz 计算分频 → 实际波特率整体偏离（OpenSBI 横幅落到 57.6k、内核阶段乱码）。

```bash
python3 tools/patch_linux_dtb_clock.py sw/linux/data.txt sw/linux/data_patched.txt
# 把 DTB clock-frequency 从 50MHz 改为 25MHz (唯一一处)
```

修正后 **OpenSBI、Linux 内核、自制裸机三者统一为分频 14 ≈ 111.6 kbps**，
串口终端按 **115200 8N1** 打开即可完整阅读（内核日志中 `base_baud = 1562500` = 25MHz/16，
即分频 14 的来源）。

## 启动流程与预期现象

`flows/07_run_linux.sh` 会依次：halt → 写入 13.3MB 镜像(约 55s) → 开串口抓包 → reset_cpu。
约 4 秒后可见：

```
OpenSBI v1.5.1-3-g0dfdb94 ... Platform Name : bosc,kmh-v2-1core
[    0.000000] Linux version 6.10.7-... riscv64 GNU/Linux
[    1.019208] 310b0000.serial: ttyS0 at MMIO 0x310b0000 (irq = 0, base_baud = 1562500) is a 16550A
Starting syslogd: OK / Starting klogd: OK
Linux buildroot 6.10.7-... riscv64 GNU/Linux
#                     ← buildroot root shell（无登录密码），可直接输入命令
```

实测交互（完整日志见 `docs/evidence/serial_linux_full_boot_success.log`）：

```bash
printf 'uname -a\r\n' > /dev/ttyUSB0     # → Linux buildroot 6.10.7-... riscv64 GNU/Linux
printf 'ls /\r\n'    > /dev/ttyUSB0      # → bin dev etc init lib ...
```

> 提示：控制台无流控，内核日志滚动时连续输入会触发 `input overrun`（字符丢失）；
> 等 `#` 提示符稳定后再逐条输入。

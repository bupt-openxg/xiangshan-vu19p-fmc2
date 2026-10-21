# 扩展指令随机激励

本目录包含两种实现：

1. `ext_random.c` + `run.sh`：**FPGA 核级随机测试**（本仓库实际执行）。
   - 7 条扩展指令，e16/m1/vl=8，vxrm 0–3 全遍历；
   - 随机值 + 周期性边界值（0/±1/±32767/±32768/±16384 等）；
   - 每条指令逐 lane 与 C 标量参考模型比较，同时校验 vxsat；
   - 默认 1,000,000 条指令请求，`RANDOM_CASES=N tests/random/run.sh` 可调。
   - 标量参考的语义取自 YunSuan golden model：
     `vector_integer_mac.cpp`（vscmul/vscmulcj/vscmacc/vscmacccj）与
     `vector_integer_aluF.cpp`（vscaddconj/vscaddrq/vscaddrtq）。

2. `run_random_tests.py`：官方 `XiangShan/yunsuan/scripts/run_random_tests.py`
   的移植副本（commit `7c844094`）。用于在具备 Verilator 和 YunSuan emu 的
   环境按文档原样运行 VIMAC/VIALUF 各 10⁹ 条：
   ```bash
   make -C <xs-env>/XiangShan/yunsuan emu
   python3 run_random_tests.py --fu-type 10 --total 1000000000 --jobs 10 \
       --emu <xs-env>/XiangShan/yunsuan/build/emu --output-dir <log>/vimac
   python3 run_random_tests.py --fu-type 5 --total 1000000000 --jobs 10 \
       --emu <xs-env>/XiangShan/yunsuan/build/emu --output-dir <log>/vialuf
   ```
   板卡无法直接运行该 emu；`run.sh` 是它在 FPGA 上的可执行子集。

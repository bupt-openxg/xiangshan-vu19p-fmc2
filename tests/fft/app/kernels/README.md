# FFT 冻结 kernel 映射

每个 backend/点数目录只保留一个现行 bundle：

- scalar：`dft_scalar.c` 和配套的 scalar helper；
- RVV：`dft_rvv.c`；
- EXT：`dft_ext.c` 和同目录的 RVV companion。

`mainline.mk` 是唯一选择入口，Makefile 强制 `FFT_MAINLINE=1`。bundle 中的重复 helper 是该实现的编译依赖，不是第二个算法候选。源码必须保持 bit-exact，不能在构建时替换为实验文件。

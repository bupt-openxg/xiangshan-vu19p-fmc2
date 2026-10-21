# MIMO 冻结 kernel

每个均衡任务和后端只有一个现行入口：

```text
eq_zf/scalar/eq_zf_scalar.c
eq_zf/rvv/eq_zf_rvv.c
eq_zf/ext/eq_zf_ext.c
eq_mmse/scalar/eq_mmse_scalar.c
eq_mmse/rvv/eq_mmse_rvv.c
eq_mmse/ext/eq_mmse_ext.c
```

`common/` 只存放这些入口所需的共享定点、布局和算术 helper。2x2 的后端专用路径分别由对应入口调用，4x4 的共享求解器也只能通过上述入口进入。冻结实现的宏组合由 `../config/profiles/` 管理；本目录不包含候选、历史快照或测量产物。

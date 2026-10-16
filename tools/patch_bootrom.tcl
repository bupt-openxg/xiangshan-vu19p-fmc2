# 向 ROM (bootrom BRAM) 写入版本兼容的 3 指令启动代码 (双入口)
# 背景: 2026-10-13 比特流的 coe 含 mnstatus@0x744 指令, 在香山 4b9ddb8 上
#       首条指令即非法指令陷阱 (4b9ddb8 的 mnstatus 在 0x353), CPU 无法启动。
# 本脚本通过 flash JTAG-AXI 直接改写 ROM (BRAM), 使现有比特流可正常启动。
# ⚠️ FPGA 每次重新配置 (重烧 bit / 断电重载) 后 ROM 会恢复原样, 需重跑本脚本;
#    重新综合的比特流 (blk_mem_gen_0.tcl 已修复) 则无需。
open_hw_manager
connect_hw_server
open_hw_target
current_hw_device [lindex [get_hw_devices] 0]
refresh_hw_device [current_hw_device]
set flash [get_hw_axis hw_axi_1]
# addiw x5,x0,1; slli x5,x5,31; jalr x0,0(x5) -> 0x8000_0000
# 数据串最右字对应最低地址 (hw_axi 约定)
set boot "0002806701f292930010029b"
foreach {addr tag} {10000000 入口A-0x00 10000088 入口B-0x88} {
  create_hw_axi_txn w $flash -type write -address $addr -len 3 -data $boot
  run_hw_axi [get_hw_axi_txns w]
  puts "写入 $tag @0x$addr -> BRESP=[get_property STATUS.BRESP $flash]"
  delete_hw_axi_txn [get_hw_axi_txns w]
}
catch {close_hw_target}
catch {disconnect_hw_server}
catch {close_hw_manager}
puts "=== BOOTROM PATCH DONE ==="

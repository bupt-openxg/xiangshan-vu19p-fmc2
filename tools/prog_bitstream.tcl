# 烧录 build/ 下的比特流与探针文件到 FPGA（JTAG）
#   用法: vivado -mode tcl -source tools/prog_bitstream.tcl
#         （bit 目录默认 build/；可用环境变量 FPGA_BIT_HOME 覆盖）
#   注意: Tcl 读取环境变量必须经 $::env(...)，不能用 bash 的 ${VAR:-default}
set root [file dirname [file dirname [file normalize [info script]]]]
set bitdir "$root/build"
if {[info exists ::env(FPGA_BIT_HOME)] && $::env(FPGA_BIT_HOME) ne ""} {
    set bitdir $::env(FPGA_BIT_HOME)
}
set bit [lindex [glob -nocomplain $bitdir/*.bit] 0]
set ltx [lindex [glob -nocomplain $bitdir/*.ltx] 0]

if {$bit eq ""} { puts "ERROR: 未找到比特流: $bitdir/*.bit"; exit 1 }
puts "BIT: $bit"
puts "LTX: $ltx"

open_hw_manager
connect_hw_server
open_hw_target
current_hw_device [get_hw_devices *]
refresh_hw_device [current_hw_device]
set_property PROGRAM.FILE $bit [current_hw_device]
if {$ltx ne ""} { set_property PROBES.FILE $ltx [current_hw_device] }
puts "烧录中（约 3 分钟）..."
program_hw_devices [current_hw_device]
puts "=== 比特流烧录完成 ==="
catch {close_hw_target}
catch {disconnect_hw_server}
catch {close_hw_manager}
exit

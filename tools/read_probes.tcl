# 上板状态读取: 烧写比特流 -> 打印全部 VIO 探针 -> halt/复位序列 -> 再读
#   （比对计数器变化即可判断 CPU 是否在取指/访存、复位链与调试模块状态）
# 用法: source tools/env.sh && vivado -mode tcl -source tools/read_probes.tcl
#       （可用 -tclargs <bit目录> 覆盖, 默认 build/ 或 $FPGA_BIT_HOME）
set root [file dirname [file dirname [file normalize [info script]]]]
set bitdir "${FPGA_BIT_HOME}"
if {$bitdir eq ""} { set bitdir "$root/build" }
if {[llength $::argv] > 0 && [lindex $::argv 0] ne ""} { set bitdir [lindex $::argv 0] }

set bit [lindex [glob -nocomplain $bitdir/*.bit] 0]
set ltx [lindex [glob -nocomplain $bitdir/*.ltx] 0]
if {$bit eq "" || $ltx eq ""} { puts "ERROR: bit/ltx not found in $bitdir"; exit 1 }
puts "BIT: $bit"
puts "LTX: $ltx"

open_hw_manager
connect_hw_server
open_hw_target
current_hw_device [get_hw_devices *]
refresh_hw_device [current_hw_device]
set_property PROGRAM.FILE $bit [current_hw_device]
set_property PROBES.FILE  $ltx [current_hw_device]
program_hw_devices [current_hw_device]
puts "=== FPGA PROGRAMMED ==="
refresh_hw_device -update_hw_probes true [current_hw_device]

# ---- 探针解析助手 (规避修改后模式查询回退导致的 44-152 错误) ----
proc vio_probe_out {vio name} {
    foreach p [get_hw_probes -of_objects $vio] {
        if {[get_property NAME $p] eq $name && ![catch {get_property OUTPUT_VALUE $p}]} { return $p }
    }
    foreach p [get_hw_probes -of_objects $vio] {
        if {[get_property NAME $p] eq $name} { return $p }
    }
    return ""
}
set VIO [get_hw_vios hw_vio_1]

proc dump_probes {tag} {
   set vio $VIO
   refresh_hw_vio $vio
   puts "=== $tag ==="
   foreach p [get_hw_probes -of_objects $vio] {
      set n [get_property NAME $p]
      if {![catch {set v [get_property INPUT_VALUE $p]}]} {
         puts "  in  $n = 0x$v"
      } elseif {![catch {set v [get_property OUTPUT_VALUE $p]}]} {
         puts "  out $n = $v"
      }
   }
}

proc wr {n v} {
   set p [vio_probe_out $VIO $n]
   if {$p eq ""} { puts "ERROR: 未找到输出探针 $n"; return }
   set_property OUTPUT_VALUE $v $p
   commit_hw_vio $p
}

dump_probes "S0 上电初值 (VIO INIT 全 1, CPU 应在运行)"

after 3000
dump_probes "S1 3 秒后 (计数器走动 = CPU 取指/访存活动)"

puts "--- halt (sw6=0) ---"
wr vio_sw6 0
after 500
dump_probes "S2 halt 后"

puts "--- reset 序列 sw6=1 -> sw5=0 -> sw5=1 ---"
wr vio_sw6 1
after 500
wr vio_sw5 0
after 500
wr vio_sw5 1
after 3000
dump_probes "S3 复位后 3 秒 (CPU 应重新启动, 计数器应增长)"

catch {close_hw_target}
catch {disconnect_hw_server}
catch {close_hw_manager}
puts "=== READ PROBES DONE ==="
exit

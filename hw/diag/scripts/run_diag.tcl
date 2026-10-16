# 诊断比特流上板: 编程 -> 读 VIO 输入探针 -> 可控 UART 发送 -> 复位序列验证
# 全程 TCL, 不触碰板上任何按键/开关
set root [file dirname [file dirname [file normalize [info script]]]]
set prjdir $root/proj
set bit [lindex [glob -nocomplain $prjdir/board_diag.runs/impl_1/*.bit] 0]
set ltx [lindex [glob -nocomplain $prjdir/board_diag.runs/impl_1/*.ltx] 0]

if {$bit eq "" || $ltx eq ""} { puts "ERROR: bit/ltx not found"; exit 1 }

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

set vio [get_hw_vios hw_vio_1]
puts "probes: [get_hw_probes -of_objects $vio]"

proc step {} {
   set vio [get_hw_vios hw_vio_1]
   refresh_hw_vio $vio
   puts "in0 cnt5_25M  = [get_property INPUT_VALUE [get_hw_probes probe_in0 -of_objects $vio]]"
   puts "in1 cnt6_200M = [get_property INPUT_VALUE [get_hw_probes probe_in1 -of_objects $vio]]"
   puts "in2 cnt7_80M  = [get_property INPUT_VALUE [get_hw_probes probe_in2 -of_objects $vio]]"
   puts "in3 cnt8_1M   = [get_property INPUT_VALUE [get_hw_probes probe_in3 -of_objects $vio]]"
   set v [get_property INPUT_VALUE [get_hw_probes probe_in4 -of_objects $vio]]
   puts "in4 = 0x$v (b0 sin,[1]ubusy,[2]rflag,[3]cpu_rstn,[4]sw5d,[5]p4,[6]p5,[7]p6,[8]sw6,[9]sw5,[10]sout)"
}


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
proc vio_commit {name val} {
   set vio [get_hw_vios hw_vio_1]
   set p [vio_probe_out $vio $name]
   if {$p eq ""} { puts "ERROR: 未找到输出探针 $name"; return }
   set_property OUTPUT_VALUE $val $p
   commit_hw_vio $p
}

puts "=== STEP 0: 上电初值 ==="
step
after 3000
puts "=== STEP 1: 3 秒后再读 (计数器应有走动) ==="
step

puts "=== STEP 2: UART 发送 'H'(0x48) ==="
vio_commit probe_out2 0x48
vio_commit probe_out3 0
after 300
vio_commit probe_out3 1
after 300
vio_commit probe_out3 0
after 500
puts "=== STEP 3: 发送后 (ubusy 应先 1 后回 0) ==="
step

puts "=== STEP 4: 复刻复位序列 sw6=0 -> sw6=1 -> sw5=0 -> sw5=1 ==="
vio_commit probe_out0 0
after 400
vio_commit probe_out0 1
after 400
vio_commit probe_out1 0
after 400
vio_commit probe_out1 1
after 500
puts "=== STEP 5: 序列后 (期望 cpu_rstn=1) ==="
step
after 1000
step

puts "=== STEP 6: 再发 'U'(0x55) ==="
vio_commit probe_out2 0x55
vio_commit probe_out3 1
after 300
vio_commit probe_out3 0
after 500
step

catch {close_hw_target}
catch {disconnect_hw_server}
catch {close_hw_manager}
puts "=== DIAG DONE ==="
exit

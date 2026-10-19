# =============================================================================
# 从 phys_opt 检查点续跑 布线 -> 比特流（跳过 report_power/report_methodology）
#   背景: 2026-10-19 00:40 systemd-oomd 在 report_power 的 Vector-less Activity
#         Propagation 阶段杀掉整个会话；当时 route_design 已完成（WNS +0.709ns，
#         0 路由错误），但 routed.dcp 在报告之后才写，未能落盘。
#   用法: vivado -mode batch -source tools/resume_impl_route.tcl -tclargs \
#           <impl_1/fpga_top_debug_physopt.dcp> <build 输出目录> <impl_1 运行目录>
#   说明: 布线指令与工程一致: route_design -directive Explore
# =============================================================================
set physopt [lindex $argv 0]
set outdir  [lindex $argv 1]
set rundir  [lindex $argv 2]
foreach p [list $physopt] {
    if {![file exists $p]} { puts "ERROR: 缺少检查点 $p"; exit 1 }
}

puts "=== open physopt: $physopt ==="
open_checkpoint $physopt

puts "=== route_design -directive Explore ==="
route_design -directive Explore

puts "=== write routed checkpoint ==="
write_checkpoint -force $rundir/fpga_top_debug_routed.dcp

puts "=== reports (无 power/methodology) ==="
report_timing_summary -max_paths 10 -file $rundir/fpga_top_debug_timing_summary_routed.rpt -warn_on_violation
report_route_status -file $rundir/fpga_top_debug_route_status.rpt
report_drc -file $rundir/fpga_top_debug_drc_routed.rpt
report_utilization -file $rundir/fpga_top_debug_utilization_routed.rpt

puts "=== write_bitstream ==="
write_bitstream -force $outdir/fpga_top_debug.bit
write_debug_probes -force $outdir/fpga_top_debug.ltx
puts "=== RESUME DONE ==="

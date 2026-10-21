# =============================================================================
# 服务器端：synth -> impl -> bitstream，只补写必要报告（report_power 由 impl 前置步骤执行）
# 用法:
#   vivado -mode batch -source build_server.tcl -tclargs <xpr> <outdir> [jobs]
# =============================================================================
set prj    [lindex $argv 0]
set outdir [lindex $argv 1]
set jobs   [lindex $argv 2]
if {$jobs eq ""} { set jobs 16 }
if {![file exists $prj]} {
    puts "ERROR: project not found: $prj"
    exit 1
}
file mkdir $outdir

puts "=== open_project $prj ==="
open_project $prj

puts "=== synth_1 (-jobs $jobs) ==="
catch {reset_run synth_1}
catch {reset_run impl_1}
launch_runs synth_1 -jobs $jobs
wait_on_run synth_1
if {[get_property PROGRESS [get_runs synth_1]] ne "100%"} {
    puts "ERROR: synth_1 failed: [get_property STATUS [get_runs synth_1]]"
    exit 1
}
puts "SYNTH: [get_property STATUS [get_runs synth_1]]"

puts "=== impl_1 -> write_bitstream (-jobs $jobs) ==="
launch_runs impl_1 -to_step write_bitstream -jobs $jobs
wait_on_run impl_1
if {[get_property PROGRESS [get_runs impl_1]] ne "100%"} {
    puts "ERROR: impl_1 failed: [get_property STATUS [get_runs impl_1]]"
    exit 1
}
puts "IMPL: [get_property STATUS [get_runs impl_1]]"

open_run impl_1
report_timing_summary -max_paths 10 -file "$outdir/fpga_top_debug_timing_summary_routed.rpt" -warn_on_violation
report_route_status -file "$outdir/fpga_top_debug_route_status.rpt"
report_drc -file "$outdir/fpga_top_debug_drc_routed.rpt"
report_utilization -file "$outdir/fpga_top_debug_utilization_routed.rpt"

puts "=== write_bitstream ==="
write_bitstream -force "$outdir/fpga_top_debug.bit"
write_debug_probes -force "$outdir/fpga_top_debug.ltx"
puts "=== SERVER BUILD DONE ==="
exit

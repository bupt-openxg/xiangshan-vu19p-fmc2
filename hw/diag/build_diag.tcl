# 构建板卡诊断比特流: 工程 -> 综合 -> 实现 -> 比特流 (小设计, 分钟级)
set root [file dirname [file normalize [info script]]]
set prjdir $root/proj

create_project -force board_diag $prjdir -part xcvu19p-fsva3824-1-e

add_files -norecurse [list \
   $root/rtl/fpga_diag_top.sv \
   $root/rtl/button_debounce.v ]
set_property top fpga_diag_top [current_fileset]

add_files -fileset constrs_1 -norecurse $root/constr/diag.xdc

create_ip -name vio -vendor xilinx.com -library ip -version 3.0 -module_name vio_diag
set_property -dict [list \
   CONFIG.C_NUM_PROBE_IN      {5} \
   CONFIG.C_PROBE_IN0_WIDTH   {32} \
   CONFIG.C_PROBE_IN1_WIDTH   {32} \
   CONFIG.C_PROBE_IN2_WIDTH   {32} \
   CONFIG.C_PROBE_IN3_WIDTH   {32} \
   CONFIG.C_PROBE_IN4_WIDTH   {16} \
   CONFIG.C_NUM_PROBE_OUT     {4} \
   CONFIG.C_PROBE_OUT0_WIDTH  {1} \
   CONFIG.C_PROBE_OUT1_WIDTH  {1} \
   CONFIG.C_PROBE_OUT2_WIDTH  {8} \
   CONFIG.C_PROBE_OUT3_WIDTH  {1} \
   CONFIG.C_PROBE_OUT0_INIT_VAL {0x1} \
   CONFIG.C_PROBE_OUT1_INIT_VAL {0x1} \
   CONFIG.C_EN_PROBE_IN_ACTIVITY {false} \
] [get_ips vio_diag]
generate_target all [get_ips vio_diag]

launch_runs synth_1 -jobs 8
wait_on_run synth_1
puts "SYNTH_STATUS=[get_property STATUS [get_runs synth_1]]"

launch_runs impl_1 -to_step write_bitstream -jobs 8
wait_on_run impl_1
puts "IMPL_STATUS=[get_property STATUS [get_runs impl_1]]"

puts "BIT=[glob -nocomplain $prjdir/board_diag.runs/impl_1/*.bit]"
exit

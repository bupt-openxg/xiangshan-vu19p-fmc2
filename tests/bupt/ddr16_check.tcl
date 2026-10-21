# =============================================================================
# 16GB DDR 容量/读写检查（JTAG-AXI，不依赖 CPU 缓存）
# 用法: vivado -mode batch -source ddr16_check.tcl -tclargs <fpga_top_debug.ltx>
# 检查 2/6/10/14GB 与 16GB 末尾 8 字节；不同地址写入不同 pattern，
# 只有 16GB 器件才能在所有窗口同时读回，且低位不会被高位别名覆盖。
# 通过时打印 DDR16G_PASS。
# =============================================================================
set ltx_file [lindex $argv 0]
if {![file exists $ltx_file]} { puts "ERROR: missing ltx $ltx_file"; exit 1 }

proc vio_probe_out {vio name} {
    foreach p [get_hw_probes -of_objects $vio] {
        if {[get_property NAME $p] eq $name && ![catch {get_property OUTPUT_VALUE $p}]} { return $p }
    }
    foreach p [get_hw_probes -of_objects $vio] {
        if {[get_property NAME $p] eq $name} { return $p }
    }
    return ""
}
proc wr {p v} { set_property OUTPUT_VALUE $v $p; commit_hw_vio $p; after 200 }

open_hw_manager
connect_hw_server
open_hw_target
current_hw_device [get_hw_devices *]
set dev [current_hw_device]
set_property PROBES.FILE $ltx_file $dev
refresh_hw_device -update_hw_probes true $dev
refresh_hw_vio hw_vio_1
set vio [get_hw_vios hw_vio_1]

set p_sw4 [vio_probe_out $vio vio_sw4]
if {$p_sw4 eq ""} { set p_sw4 [vio_probe_out $vio ddr_rstn_to_core] }
set p_sw5 [vio_probe_out $vio vio_sw5]
set p_sw6 [vio_probe_out $vio vio_sw6]
if {$p_sw4 eq ""} { puts "ERROR: DDR reset probe not found"; exit 1 }
# 先停 CPU，避免测试期间 CPU 访问 DDR 干扰 JTAG 读写。
if {$p_sw5 ne ""} { wr $p_sw5 0 }
if {$p_sw6 ne ""} { wr $p_sw6 0 }
wr $p_sw4 1
after 500
puts "DDR16G probes: sw4=$p_sw4 sw5=$p_sw5 sw6=$p_sw6"

set hw_axi ""
foreach a [get_hw_axis] {
    if {![catch {get_property AXI_DATA_WIDTH $a} w] && $w == 64} { set hw_axi $a; break }
}
if {$hw_axi eq ""} { set hw_axi [lindex [get_hw_axis] end] }
puts "DDR_AXI=$hw_axi width=[get_property AXI_DATA_WIDTH $hw_axi]"

proc axi_write {axi addr data} {
    create_hw_axi_txn wr_txn $axi -address [format "%llx" $addr] \
        -data $data -len 1 -burst INCR -type write
    run_hw_axi wr_txn
    delete_hw_axi_txn wr_txn
}
proc axi_read {axi addr} {
    create_hw_axi_txn rd_txn $axi -address [format "%llx" $addr] \
        -len 1 -burst INCR -type read
    run_hw_axi rd_txn
    set txn [get_hw_axi_txns rd_txn]
    set data [get_property DATA $txn]
    delete_hw_axi_txn $txn
    return $data
}

# AXI 地址 34 位，合法范围 0x0..0x3FFFFFFFF（16GB）。
set cases [list \
    [list 0x80000000        0x1122334455667788] \
    [list 0x180000000       0x99AABBCCDDEEFF00] \
    [list 0x280000000       0x0123456789ABCDEF] \
    [list 0x380000000       0x55AA55AA55AA55AA] \
    [list 0x3FFFFFFF8       0xFEDCBA9876543210] \
]
set fail 0
foreach c $cases {
    lassign $c addr value
    puts "WRITE [format 0x%llx $addr] = [format 0x%016llx $value]"
    axi_write $hw_axi $addr [format "%016llx" $value]
}
foreach c $cases {
    lassign $c addr value
    set got [axi_read $hw_axi $addr]
    set exp [format "%016llx" $value]
    set ok [expr {[string tolower $got] eq [string tolower $exp]}]
    puts "READ  [format 0x%llx $addr] = 0x$got expected=0x$exp [expr {$ok ? "PASS" : "FAIL"}]"
    if {!$ok} { incr fail }
}
if {$fail == 0} { puts "DDR16G_PASS" } else { puts "DDR16G_FAIL count=$fail" }
catch {close_hw_target}
catch {disconnect_hw_server}
catch {close_hw_manager}
exit [expr {$fail == 0 ? 0 : 1}]

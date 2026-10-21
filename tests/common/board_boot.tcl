# =============================================================================
# 单次上板启动：halt -> JTAG 写 DDR -> VIO 复位释放 CPU，一次 Vivado 会话完成
#   用法: vivado -mode batch -source board_boot.tcl -tclargs <ltx> <workload.txt>
#   说明: 合并 third_party/env-scripts/fpga_diff/vivado/scripts 下
#         halt_soc.tcl / jtag_write_ddr.tcl / reset_cpu.tcl 的等价逻辑，
#         省掉每次运行 3 次 Vivado 启动的开销（约 20s/次）。
# =============================================================================
set ltx_file  [lindex $argv 0]
set workload  [lindex $argv 1]

if {![file exists $ltx_file]} {
    puts "ERROR: .ltx not found: $ltx_file"
    exit 1
}
if {![file exists $workload]} {
    puts "ERROR: workload not found: $workload"
    exit 1
}

proc vio_probe_out {vio name} {
    foreach p [get_hw_probes -of_objects $vio] {
        if {[get_property NAME $p] eq $name && ![catch {get_property OUTPUT_VALUE $p}]} {
            return $p
        }
    }
    foreach p [get_hw_probes -of_objects $vio] {
        if {[get_property NAME $p] eq $name} { return $p }
    }
    return ""
}

proc write_to_ddr {fn hw_axi hw_axi_data_width} {
    puts "Write to DDR: $fn ..."
    set fdata [open $fn r]
    set idx 0
    set line 0
    while {[gets $fdata dummy] >= 0} { incr line }
    if {$line % 2 != 0} {
        close $fdata
        error "input file has odd number of lines, expected address/data pairs"
    }
    set total [expr {$line / 2}]
    seek $fdata 0 start
    set prev_percent -1
    set beat_bytes [expr {$hw_axi_data_width / 8}]
    set beat_hex_chars [expr {$beat_bytes * 2}]
    set max_len 256

    while {1} {
        if {[gets $fdata aline] < 0} { break }
        if {[gets $fdata dline] < 0} {
            close $fdata
            error "missing data line for address line: $aline"
        }
        set AddrString [string trim $aline]
        set DataString [string trim $dline]
        if {$AddrString eq "" || $DataString eq ""} { break }
        if {[expr {[string length $DataString] % $beat_hex_chars}] != 0} {
            close $fdata
            error "DataString length is not aligned to ${hw_axi_data_width}-bit beats"
        }
        if {[scan $AddrString "%llx" addr_val] != 1} {
            close $fdata
            error "Invalid address format: $AddrString"
        }
        if {[expr {$addr_val % $beat_bytes}] != 0} {
            close $fdata
            error "Address is not ${beat_bytes}-byte aligned: $AddrString"
        }
        set len [expr {[string length $DataString] / $beat_hex_chars}]
        if {$len < 1} { close $fdata; error "DataString contains no beats" }

        set start_addr $addr_val
        set remaining_len $len
        while {$remaining_len > 0} {
            set chunk_len [expr {$remaining_len > $max_len ? $max_len : $remaining_len}]
            set chunk_hex_chars [expr {$chunk_len * $beat_hex_chars}]
            set DataChunk [string range $DataString 0 [expr {$chunk_hex_chars - 1}]]
            create_hw_axi_txn wr_txn $hw_axi -address [format "%llx" $start_addr] \
                -data $DataChunk -len $chunk_len -burst INCR -type write
            run_hw_axi wr_txn
            delete_hw_axi_txn wr_txn
            set start_addr [expr {$start_addr + $chunk_len * $beat_bytes}]
            set DataString [string range $DataString $chunk_hex_chars end]
            set remaining_len [expr {$remaining_len - $chunk_len}]
        }
        incr idx
        set percent [expr {int(($idx * 100) / $total)}]
        if {$percent > $prev_percent || $idx == $total} {
            puts "  DDR ${percent}% ($idx/$total)"
            flush stdout
            set prev_percent $percent
        }
    }
    close $fdata
}

open_hw_manager
connect_hw_server
open_hw_target
current_hw_device [get_hw_devices *]
set dev [current_hw_device]

set_property PROBES.FILE $ltx_file $dev
refresh_hw_device -update_hw_probes true $dev
refresh_hw_vio hw_vio_1
set vio [get_hw_vios hw_vio_1]

set p_sw5 [vio_probe_out $vio vio_sw5]
set p_sw6 [vio_probe_out $vio vio_sw6]
set p_sw4 [vio_probe_out $vio vio_sw4]
if {$p_sw4 eq ""} { set p_sw4 [vio_probe_out $vio ddr_rstn_to_core] }
if {$p_sw5 eq "" || $p_sw6 eq "" || $p_sw4 eq ""} {
    puts "ERROR: VIO probe resolution failed (sw4=$p_sw4 sw5=$p_sw5 sw6=$p_sw6)"
    exit 1
}

proc wr {p v} { set_property OUTPUT_VALUE $v $p; commit_hw_vio $p; after 300 }

# ---- halt + release DDR reset before JTAG writes ----
wr $p_sw4 1
wr $p_sw5 0
wr $p_sw6 0
refresh_hw_vio $vio
puts "BOARD_HALT_DONE"

# ---- DDR write through the 64-bit JTAG-AXI master ----
set hw_axi_list [get_hw_axis]
set hw_axi ""
if {[info exists ::env(HW_AXI_NAME)] && [string trim $::env(HW_AXI_NAME)] ne ""} {
    set hw_axi [get_hw_axis [string trim $::env(HW_AXI_NAME)]]
}
if {$hw_axi eq ""} {
    foreach _a $hw_axi_list {
        if {![catch {get_property AXI_DATA_WIDTH $_a} _w] && $_w == 64} {
            set hw_axi $_a
            break
        }
    }
}
if {$hw_axi eq ""} {
    set hw_axi [lindex $hw_axi_list end]
    puts "WARNING: no 64-bit AXI core, fallback to $hw_axi"
}
set hw_axi_data_width 64
if {![catch {get_property AXI_DATA_WIDTH $hw_axi} _hw_w] && $_hw_w ne ""} {
    set hw_axi_data_width $_hw_w
}
puts "DDR_AXI=$hw_axi width=$hw_axi_data_width"
write_to_ddr $workload $hw_axi $hw_axi_data_width
puts "BOARD_DDR_DONE"

# ---- reset: sw4=1, sw5=0, sw6=0 -> sw6=1 -> sw5=1 ----
wr $p_sw4 1
wr $p_sw5 0
wr $p_sw6 0
wr $p_sw6 1
wr $p_sw5 1
puts "BOARD_RESET_DONE"

catch {close_hw_target}
catch {disconnect_hw_server}
catch {close_hw_manager}
exit

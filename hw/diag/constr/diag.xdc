# 板卡诊断设计约束 (PD-VU19P / PHINE), 引脚与 fpga.xdc 一致
# 注意: 注释必须独占一行 (Tcl 会把行尾 # 之后当作参数)

####### 时钟
set_property PACKAGE_PIN BA51 [get_ports clk5_p]
set_property PACKAGE_PIN BB51 [get_ports clk5_n]
set_property PACKAGE_PIN BW37 [get_ports clk6_p]
set_property PACKAGE_PIN BY37 [get_ports clk6_n]
set_property PACKAGE_PIN BM44 [get_ports clk7_p]
set_property PACKAGE_PIN BN44 [get_ports clk7_n]
set_property PACKAGE_PIN E57  [get_ports clk8_p]
set_property PACKAGE_PIN E58  [get_ports clk8_n]

####### 复位引脚
set_property PACKAGE_PIN BG18 [get_ports rstn_sw6]
set_property PACKAGE_PIN BG17 [get_ports rstn_sw5]
set_property PACKAGE_PIN W16  [get_ports rstn_sw4]

####### UART
set_property PACKAGE_PIN BG14 [get_ports uart0_sout]
set_property PACKAGE_PIN BH14 [get_ports uart0_sin]

####### LED
set_property PACKAGE_PIN AA16 [get_ports led0]
set_property PACKAGE_PIN AA14 [get_ports led2]
set_property PACKAGE_PIN AB13 [get_ports led3]

####### IOSTANDARD / 电气属性
set_property IOSTANDARD LVDS [get_ports clk5_p]
set_property IOSTANDARD LVDS [get_ports clk5_n]
set_property IOSTANDARD LVDS [get_ports clk6_p]
set_property IOSTANDARD LVDS [get_ports clk6_n]
set_property IOSTANDARD DIFF_SSTL12 [get_ports clk7_p]
set_property IOSTANDARD DIFF_SSTL12 [get_ports clk7_n]
set_property IOSTANDARD LVDS [get_ports clk8_p]
set_property IOSTANDARD LVDS [get_ports clk8_n]
set_property DIFF_TERM TRUE [get_ports clk5_p]
set_property DIFF_TERM_ADV TERM_100 [get_ports clk5_p]
set_property DIFF_TERM TRUE [get_ports clk6_p]
set_property DIFF_TERM_ADV TERM_100 [get_ports clk6_p]
set_property DIFF_TERM TRUE [get_ports clk7_p]
set_property DIFF_TERM TRUE [get_ports clk8_p]
set_property DIFF_TERM_ADV TERM_100 [get_ports clk8_p]
set_property DIFF_TERM_ADV {} [get_ports clk7_p]
set_property DIFF_TERM_ADV {} [get_ports clk7_n]

set_property IOSTANDARD LVCMOS33 [get_ports rstn_sw6]
set_property IOSTANDARD LVCMOS33 [get_ports rstn_sw5]
set_property IOSTANDARD LVCMOS33 [get_ports rstn_sw4]
set_property PULLUP true [get_ports rstn_sw6]
set_property PULLUP true [get_ports rstn_sw5]
set_property PULLUP true [get_ports rstn_sw4]

set_property IOSTANDARD LVCMOS33 [get_ports uart0_sout]
set_property IOSTANDARD LVCMOS33 [get_ports uart0_sin]
set_property IOSTANDARD LVCMOS33 [get_ports led0]
set_property IOSTANDARD LVCMOS33 [get_ports led2]
set_property IOSTANDARD LVCMOS33 [get_ports led3]

####### 时钟定义
create_clock -period 40.000   -name clk5_25m  [get_ports clk5_p]
create_clock -period 5.000    -name clk6_200m [get_ports clk6_p]
create_clock -period 12.500   -name clk7_80m  [get_ports clk7_p]
create_clock -period 1000.000 -name clk8_1m   [get_ports clk8_p]

set_clock_groups -asynchronous \
   -group [get_clocks clk5_25m] \
   -group [get_clocks clk6_200m] \
   -group [get_clocks clk7_80m] \
   -group [get_clocks clk8_1m]

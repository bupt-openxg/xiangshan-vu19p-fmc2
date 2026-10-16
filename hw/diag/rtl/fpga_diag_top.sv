// 板卡诊断顶层 (PD-VU19P / PHINE): 用最小设计给出板上地面真相
//   - 四路时钟计数器 (clk5 25M / clk6 200M / clk7 80M / clk8 1M) -> 验证时钟是否存在
//   - 引脚电平 + 复刻的 XiangShan cpu_rstn 逻辑 -> 验证复位释放路径与引脚上下拉
//   - VIO 驱动的 115200 UART 发送器 -> 端到端验证 串口引脚/线缆/适配器/上位机
// 全部状态经 VIO 输入探针在 JTAG 上可读, 输出全由 VIO 控制, 不依赖任何物理按键。
module fpga_diag_top(
   input  clk5_p, clk5_n,   // 25MHz  AEConsole PLL GC04 -> CPU 系统时钟
   input  clk6_p, clk6_n,   // 200MHz 板载 OSC
   input  clk7_p, clk7_n,   // 80MHz  AEConsole PLL GC02 -> DDR 参考
   input  clk8_p, clk8_n,   // 1MHz   AEConsole PLL GC06 -> RTC
   input  rstn_sw6,         // PUSHSW1 (BG18)
   input  rstn_sw5,         // PUSHSW2 (BG17)
   input  rstn_sw4,         // GPIO_6  (W16)
   input  uart0_sin,        // 串口适配器 TXD -> FPGA RXD (BH14)
   output uart0_sout,       // FPGA TXD -> 适配器 RXD (BG14)
   output led0, led2, led3
);

// ---------------- 时钟 ----------------
wire clk5_i, clk6_i, clk8_i, clk7_i;
wire clk5, clk6, clk7, clk8;

IBUFGDS ibufgds5 (.I(clk5_p), .IB(clk5_n), .O(clk5_i));
BUFG    bufg5    (.I(clk5_i), .O(clk5));
IBUFGDS ibufgds6 (.I(clk6_p), .IB(clk6_n), .O(clk6_i));
BUFG    bufg6    (.I(clk6_i), .O(clk6));
IBUFGDS ibufgds8 (.I(clk8_p), .IB(clk8_n), .O(clk8_i));
BUFG    bufg8    (.I(clk8_i), .O(clk8));
IBUFDS  ibufds7  (.I(clk7_p), .IB(clk7_n), .O(clk7_i));
BUFG    bufg7    (.I(clk7_i), .O(clk7));

// ---------------- 引脚电平 ----------------
wire p6, p5, p4;
IBUF ib6 (.I(rstn_sw6), .O(p6));
IBUF ib5 (.I(rstn_sw5), .O(p5));
IBUF ib4 (.I(rstn_sw4), .O(p4));

// ---------------- 时钟计数器 ----------------
reg [31:0] cnt5, cnt6, cnt7, cnt8;
always @(posedge clk5) cnt5 <= cnt5 + 1'b1;
always @(posedge clk6) cnt6 <= cnt6 + 1'b1;
always @(posedge clk7) cnt7 <= cnt7 + 1'b1;
always @(posedge clk8) cnt8 <= cnt8 + 1'b1;

// ---------------- VIO ----------------
wire        vio_sw6, vio_sw5;
wire [7:0]  uart_data;
wire        uart_start;

// ---------------- 复刻 XiangShan 的 cpu_rstn 逻辑 ----------------
reg  cpu_rstn;
reg  vio_sw5_d;
wire cpu_setn_rflag;

always @(posedge clk5)
   vio_sw5_d <= vio_sw5;

button_debounce u_fsm(
   .clk          (clk5),
   .rstn         (p6 && vio_sw6),
   .button_i     (p5 && vio_sw5),
   .button_rflag (cpu_setn_rflag)
);

always @(posedge clk5) begin
   if ((p6 == 1'b0) || (vio_sw6 == 1'b0))
      cpu_rstn <= 1'b0;
   else if (cpu_setn_rflag || (vio_sw5 && !vio_sw5_d))
      cpu_rstn <= 1'b1;
   else
      cpu_rstn <= cpu_rstn;
end

// ---------------- VIO 控制的 UART TX (115200 @ 25MHz) ----------------
localparam integer DIV = 217;   // 25e6 / 115200
reg [8:0]  ucnt;
reg [3:0]  ubit;
reg [9:0]  ush;
reg        ubusy;
reg        txd;
reg        ustart_d;

always @(posedge clk5)
   ustart_d <= uart_start;

wire ustart_p = uart_start && !ustart_d;

always @(posedge clk5) begin
   if (!ubusy) begin
      txd <= 1'b1;
      if (ustart_p) begin
         ush   <= {1'b1, uart_data, 1'b0};   // [0]=start, [8:1]=data, [9]=stop
         ubusy <= 1'b1;
         ubit  <= 4'd0;
         ucnt  <= 9'd0;
         txd   <= 1'b0;                      // 起始位
      end
   end else begin
      if (ucnt == DIV - 1) begin
         ucnt <= 9'd0;
         if (ubit == 4'd9) begin
            ubusy <= 1'b0;
            txd   <= 1'b1;
         end else begin
            ubit <= ubit + 1'b1;
            ush  <= {1'b1, ush[9:1]};
            txd  <= ush[1];
         end
      end else begin
         ucnt <= ucnt + 1'b1;
      end
   end
end

assign uart0_sout = txd;

// ---------------- LED ----------------
assign led0 = cpu_rstn;         // 亮 = CPU 复位已释放
assign led2 = ubusy;            // 亮 = UART 正在发送
assign led3 = cnt5[22];         // 心跳(~3Hz)

// ---------------- VIO 实例 ----------------
vio_diag u_vio(
   .clk        (clk6),
   .probe_in0  (cnt5),
   .probe_in1  (cnt6),
   .probe_in2  (cnt7),
   .probe_in3  (cnt8),
   .probe_in4  ({5'b0, uart0_sout, vio_sw5, vio_sw6, p6, p5, p4, vio_sw5_d, cpu_rstn, cpu_setn_rflag, ubusy, uart0_sin}),
   .probe_out0 (vio_sw6),
   .probe_out1 (vio_sw5),
   .probe_out2 (uart_data),
   .probe_out3 (uart_start)
);

endmodule

// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2020.2 (lin64) Build 3064766 Wed Nov 18 09:12:47 MST 2020
// Date        : Fri Oct 20 18:37:17 2023
// Host        : gold1 running 64-bit Ubuntu 18.04.6 LTS
// Command     : write_verilog -force -mode synth_stub -rename_top decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix -prefix
//               decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_ pfm_dynamic_dpa_hub_0_stub.v
// Design      : pfm_dynamic_dpa_hub_0
// Purpose     : Stub declaration of top-level module interface
// Device      : xcu280-fsvh2892-2L-e
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* X_CORE_INFO = "trace_integrator,Vivado 2020.2" *)
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix(mon_clk, mon_resetn, trace_clk, trace_resetn, 
  trace_rst, out_valid, out_ready, out_data, s_axi_awaddr, s_axi_awprot, s_axi_awvalid, 
  s_axi_awready, s_axi_wdata, s_axi_wstrb, s_axi_wvalid, s_axi_wready, s_axi_bvalid, 
  s_axi_bready, s_axi_bresp, s_axi_araddr, s_axi_arprot, s_axi_arvalid, s_axi_arready, 
  s_axi_rdata, s_axi_rresp, s_axi_rvalid, s_axi_rready, trace0_counter, trace0_read, 
  trace0_event, trace0_data, trace0_counter_overflow, trace0_valid, trace1_counter, 
  trace1_read, trace1_event, trace1_data, trace1_counter_overflow, trace1_valid, 
  trace2_counter, trace2_read, trace2_event, trace2_data, trace2_counter_overflow, 
  trace2_valid, trace3_counter, trace3_read, trace3_event, trace3_data, 
  trace3_counter_overflow, trace3_valid, trace4_counter, trace4_read, trace4_event, 
  trace4_data, trace4_counter_overflow, trace4_valid, trace5_counter, trace5_read, 
  trace5_event, trace5_data, trace5_counter_overflow, trace5_valid, trace6_counter, 
  trace6_read, trace6_event, trace6_data, trace6_counter_overflow, trace6_valid, 
  trace7_counter, trace7_read, trace7_event, trace7_data, trace7_counter_overflow, 
  trace7_valid, trace8_counter, trace8_read, trace8_event, trace8_data, 
  trace8_counter_overflow, trace8_valid, trace9_counter, trace9_read, trace9_event, 
  trace9_data, trace9_counter_overflow, trace9_valid, trace10_counter, trace10_read, 
  trace10_event, trace10_data, trace10_counter_overflow, trace10_valid, trace11_counter, 
  trace11_read, trace11_event, trace11_data, trace11_counter_overflow, trace11_valid, 
  trace12_counter, trace12_read, trace12_event, trace12_data, trace12_counter_overflow, 
  trace12_valid, trace13_counter, trace13_read, trace13_event, trace13_data, 
  trace13_counter_overflow, trace13_valid, trace14_counter, trace14_read, trace14_event, 
  trace14_data, trace14_counter_overflow, trace14_valid, trace15_counter, trace15_read, 
  trace15_event, trace15_data, trace15_counter_overflow, trace15_valid, trace16_counter, 
  trace16_read, trace16_event, trace16_data, trace16_counter_overflow, trace16_valid, 
  trace17_counter, trace17_read, trace17_event, trace17_data, trace17_counter_overflow, 
  trace17_valid, trace18_counter, trace18_read, trace18_event, trace18_data, 
  trace18_counter_overflow, trace18_valid, trace19_counter, trace19_read, trace19_event, 
  trace19_data, trace19_counter_overflow, trace19_valid, trace20_counter, trace20_read, 
  trace20_event, trace20_data, trace20_counter_overflow, trace20_valid, trace21_counter, 
  trace21_read, trace21_event, trace21_data, trace21_counter_overflow, trace21_valid, 
  trace22_counter, trace22_read, trace22_event, trace22_data, trace22_counter_overflow, 
  trace22_valid, trace23_counter, trace23_read, trace23_event, trace23_data, 
  trace23_counter_overflow, trace23_valid, trace24_counter, trace24_read, trace24_event, 
  trace24_data, trace24_counter_overflow, trace24_valid, trace25_counter, trace25_read, 
  trace25_event, trace25_data, trace25_counter_overflow, trace25_valid, trace26_counter, 
  trace26_read, trace26_event, trace26_data, trace26_counter_overflow, trace26_valid, 
  trace27_counter, trace27_read, trace27_event, trace27_data, trace27_counter_overflow, 
  trace27_valid, trace28_counter, trace28_read, trace28_event, trace28_data, 
  trace28_counter_overflow, trace28_valid, trace29_counter, trace29_read, trace29_event, 
  trace29_data, trace29_counter_overflow, trace29_valid, trace30_counter, trace30_read, 
  trace30_event, trace30_data, trace30_counter_overflow, trace30_valid, trace31_counter, 
  trace31_read, trace31_event, trace31_data, trace31_counter_overflow, trace31_valid, 
  trace32_counter, trace32_read, trace32_event, trace32_data, trace32_counter_overflow, 
  trace32_valid, trace33_counter, trace33_read, trace33_event, trace33_data, 
  trace33_counter_overflow, trace33_valid, trace34_counter, trace34_read, trace34_event, 
  trace34_data, trace34_counter_overflow, trace34_valid, trace35_counter, trace35_read, 
  trace35_event, trace35_data, trace35_counter_overflow, trace35_valid, trace36_counter, 
  trace36_read, trace36_event, trace36_data, trace36_counter_overflow, trace36_valid, 
  trace37_counter, trace37_read, trace37_event, trace37_data, trace37_counter_overflow, 
  trace37_valid, trace38_counter, trace38_read, trace38_event, trace38_data, 
  trace38_counter_overflow, trace38_valid, trace39_counter, trace39_read, trace39_event, 
  trace39_data, trace39_counter_overflow, trace39_valid, trace40_counter, trace40_read, 
  trace40_event, trace40_data, trace40_counter_overflow, trace40_valid, trace41_counter, 
  trace41_read, trace41_event, trace41_data, trace41_counter_overflow, trace41_valid, 
  trace42_counter, trace42_read, trace42_event, trace42_data, trace42_counter_overflow, 
  trace42_valid, trace43_counter, trace43_read, trace43_event, trace43_data, 
  trace43_counter_overflow, trace43_valid, trace44_counter, trace44_read, trace44_event, 
  trace44_data, trace44_counter_overflow, trace44_valid, trace45_counter, trace45_read, 
  trace45_event, trace45_data, trace45_counter_overflow, trace45_valid, trace46_counter, 
  trace46_read, trace46_event, trace46_data, trace46_counter_overflow, trace46_valid, 
  trace47_counter, trace47_read, trace47_event, trace47_data, trace47_counter_overflow, 
  trace47_valid, trace48_counter, trace48_read, trace48_event, trace48_data, 
  trace48_counter_overflow, trace48_valid, trace49_counter, trace49_read, trace49_event, 
  trace49_data, trace49_counter_overflow, trace49_valid, trace50_counter, trace50_read, 
  trace50_event, trace50_data, trace50_counter_overflow, trace50_valid, trace51_counter, 
  trace51_read, trace51_event, trace51_data, trace51_counter_overflow, trace51_valid, 
  trace52_counter, trace52_read, trace52_event, trace52_data, trace52_counter_overflow, 
  trace52_valid, trace53_counter, trace53_read, trace53_event, trace53_data, 
  trace53_counter_overflow, trace53_valid, trace54_counter, trace54_read, trace54_event, 
  trace54_data, trace54_counter_overflow, trace54_valid, trace55_counter, trace55_read, 
  trace55_event, trace55_data, trace55_counter_overflow, trace55_valid)
/* synthesis syn_black_box black_box_pad_pin="mon_clk,mon_resetn,trace_clk,trace_resetn,trace_rst,out_valid,out_ready,out_data[63:0],s_axi_awaddr[3:0],s_axi_awprot[2:0],s_axi_awvalid,s_axi_awready,s_axi_wdata[31:0],s_axi_wstrb[3:0],s_axi_wvalid,s_axi_wready,s_axi_bvalid,s_axi_bready,s_axi_bresp[1:0],s_axi_araddr[3:0],s_axi_arprot[2:0],s_axi_arvalid,s_axi_arready,s_axi_rdata[31:0],s_axi_rresp[1:0],s_axi_rvalid,s_axi_rready,trace0_counter[44:0],trace0_read,trace0_event,trace0_data[63:0],trace0_counter_overflow,trace0_valid,trace1_counter[44:0],trace1_read,trace1_event,trace1_data[63:0],trace1_counter_overflow,trace1_valid,trace2_counter[44:0],trace2_read,trace2_event,trace2_data[63:0],trace2_counter_overflow,trace2_valid,trace3_counter[44:0],trace3_read,trace3_event,trace3_data[63:0],trace3_counter_overflow,trace3_valid,trace4_counter[44:0],trace4_read,trace4_event,trace4_data[63:0],trace4_counter_overflow,trace4_valid,trace5_counter[44:0],trace5_read,trace5_event,trace5_data[63:0],trace5_counter_overflow,trace5_valid,trace6_counter[44:0],trace6_read,trace6_event,trace6_data[63:0],trace6_counter_overflow,trace6_valid,trace7_counter[44:0],trace7_read,trace7_event,trace7_data[63:0],trace7_counter_overflow,trace7_valid,trace8_counter[44:0],trace8_read,trace8_event,trace8_data[63:0],trace8_counter_overflow,trace8_valid,trace9_counter[44:0],trace9_read,trace9_event,trace9_data[63:0],trace9_counter_overflow,trace9_valid,trace10_counter[44:0],trace10_read,trace10_event,trace10_data[63:0],trace10_counter_overflow,trace10_valid,trace11_counter[44:0],trace11_read,trace11_event,trace11_data[63:0],trace11_counter_overflow,trace11_valid,trace12_counter[44:0],trace12_read,trace12_event,trace12_data[63:0],trace12_counter_overflow,trace12_valid,trace13_counter[44:0],trace13_read,trace13_event,trace13_data[63:0],trace13_counter_overflow,trace13_valid,trace14_counter[44:0],trace14_read,trace14_event,trace14_data[63:0],trace14_counter_overflow,trace14_valid,trace15_counter[44:0],trace15_read,trace15_event,trace15_data[63:0],trace15_counter_overflow,trace15_valid,trace16_counter[44:0],trace16_read,trace16_event,trace16_data[63:0],trace16_counter_overflow,trace16_valid,trace17_counter[44:0],trace17_read,trace17_event,trace17_data[63:0],trace17_counter_overflow,trace17_valid,trace18_counter[44:0],trace18_read,trace18_event,trace18_data[63:0],trace18_counter_overflow,trace18_valid,trace19_counter[44:0],trace19_read,trace19_event,trace19_data[63:0],trace19_counter_overflow,trace19_valid,trace20_counter[44:0],trace20_read,trace20_event,trace20_data[63:0],trace20_counter_overflow,trace20_valid,trace21_counter[44:0],trace21_read,trace21_event,trace21_data[63:0],trace21_counter_overflow,trace21_valid,trace22_counter[44:0],trace22_read,trace22_event,trace22_data[63:0],trace22_counter_overflow,trace22_valid,trace23_counter[44:0],trace23_read,trace23_event,trace23_data[63:0],trace23_counter_overflow,trace23_valid,trace24_counter[44:0],trace24_read,trace24_event,trace24_data[63:0],trace24_counter_overflow,trace24_valid,trace25_counter[44:0],trace25_read,trace25_event,trace25_data[63:0],trace25_counter_overflow,trace25_valid,trace26_counter[44:0],trace26_read,trace26_event,trace26_data[63:0],trace26_counter_overflow,trace26_valid,trace27_counter[44:0],trace27_read,trace27_event,trace27_data[63:0],trace27_counter_overflow,trace27_valid,trace28_counter[44:0],trace28_read,trace28_event,trace28_data[63:0],trace28_counter_overflow,trace28_valid,trace29_counter[44:0],trace29_read,trace29_event,trace29_data[63:0],trace29_counter_overflow,trace29_valid,trace30_counter[44:0],trace30_read,trace30_event,trace30_data[63:0],trace30_counter_overflow,trace30_valid,trace31_counter[44:0],trace31_read,trace31_event,trace31_data[63:0],trace31_counter_overflow,trace31_valid,trace32_counter[44:0],trace32_read,trace32_event,trace32_data[63:0],trace32_counter_overflow,trace32_valid,trace33_counter[44:0],trace33_read,trace33_event,trace33_data[63:0],trace33_counter_overflow,trace33_valid,trace34_counter[44:0],trace34_read,trace34_event,trace34_data[63:0],trace34_counter_overflow,trace34_valid,trace35_counter[44:0],trace35_read,trace35_event,trace35_data[63:0],trace35_counter_overflow,trace35_valid,trace36_counter[44:0],trace36_read,trace36_event,trace36_data[63:0],trace36_counter_overflow,trace36_valid,trace37_counter[44:0],trace37_read,trace37_event,trace37_data[63:0],trace37_counter_overflow,trace37_valid,trace38_counter[44:0],trace38_read,trace38_event,trace38_data[63:0],trace38_counter_overflow,trace38_valid,trace39_counter[44:0],trace39_read,trace39_event,trace39_data[63:0],trace39_counter_overflow,trace39_valid,trace40_counter[44:0],trace40_read,trace40_event,trace40_data[63:0],trace40_counter_overflow,trace40_valid,trace41_counter[44:0],trace41_read,trace41_event,trace41_data[63:0],trace41_counter_overflow,trace41_valid,trace42_counter[44:0],trace42_read,trace42_event,trace42_data[63:0],trace42_counter_overflow,trace42_valid,trace43_counter[44:0],trace43_read,trace43_event,trace43_data[63:0],trace43_counter_overflow,trace43_valid,trace44_counter[44:0],trace44_read,trace44_event,trace44_data[63:0],trace44_counter_overflow,trace44_valid,trace45_counter[44:0],trace45_read,trace45_event,trace45_data[63:0],trace45_counter_overflow,trace45_valid,trace46_counter[44:0],trace46_read,trace46_event,trace46_data[63:0],trace46_counter_overflow,trace46_valid,trace47_counter[44:0],trace47_read,trace47_event,trace47_data[63:0],trace47_counter_overflow,trace47_valid,trace48_counter[44:0],trace48_read,trace48_event,trace48_data[63:0],trace48_counter_overflow,trace48_valid,trace49_counter[44:0],trace49_read,trace49_event,trace49_data[63:0],trace49_counter_overflow,trace49_valid,trace50_counter[44:0],trace50_read,trace50_event,trace50_data[63:0],trace50_counter_overflow,trace50_valid,trace51_counter[44:0],trace51_read,trace51_event,trace51_data[63:0],trace51_counter_overflow,trace51_valid,trace52_counter[44:0],trace52_read,trace52_event,trace52_data[63:0],trace52_counter_overflow,trace52_valid,trace53_counter[44:0],trace53_read,trace53_event,trace53_data[63:0],trace53_counter_overflow,trace53_valid,trace54_counter[44:0],trace54_read,trace54_event,trace54_data[63:0],trace54_counter_overflow,trace54_valid,trace55_counter[44:0],trace55_read,trace55_event,trace55_data[63:0],trace55_counter_overflow,trace55_valid" */;
  input mon_clk;
  input mon_resetn;
  input trace_clk;
  input trace_resetn;
  output trace_rst;
  output out_valid;
  input out_ready;
  output [63:0]out_data;
  input [3:0]s_axi_awaddr;
  input [2:0]s_axi_awprot;
  input s_axi_awvalid;
  output s_axi_awready;
  input [31:0]s_axi_wdata;
  input [3:0]s_axi_wstrb;
  input s_axi_wvalid;
  output s_axi_wready;
  output s_axi_bvalid;
  input s_axi_bready;
  output [1:0]s_axi_bresp;
  input [3:0]s_axi_araddr;
  input [2:0]s_axi_arprot;
  input s_axi_arvalid;
  output s_axi_arready;
  output [31:0]s_axi_rdata;
  output [1:0]s_axi_rresp;
  output s_axi_rvalid;
  input s_axi_rready;
  output [44:0]trace0_counter;
  output trace0_read;
  input trace0_event;
  input [63:0]trace0_data;
  output trace0_counter_overflow;
  input trace0_valid;
  output [44:0]trace1_counter;
  output trace1_read;
  input trace1_event;
  input [63:0]trace1_data;
  output trace1_counter_overflow;
  input trace1_valid;
  output [44:0]trace2_counter;
  output trace2_read;
  input trace2_event;
  input [63:0]trace2_data;
  output trace2_counter_overflow;
  input trace2_valid;
  output [44:0]trace3_counter;
  output trace3_read;
  input trace3_event;
  input [63:0]trace3_data;
  output trace3_counter_overflow;
  input trace3_valid;
  output [44:0]trace4_counter;
  output trace4_read;
  input trace4_event;
  input [63:0]trace4_data;
  output trace4_counter_overflow;
  input trace4_valid;
  output [44:0]trace5_counter;
  output trace5_read;
  input trace5_event;
  input [63:0]trace5_data;
  output trace5_counter_overflow;
  input trace5_valid;
  output [44:0]trace6_counter;
  output trace6_read;
  input trace6_event;
  input [63:0]trace6_data;
  output trace6_counter_overflow;
  input trace6_valid;
  output [44:0]trace7_counter;
  output trace7_read;
  input trace7_event;
  input [63:0]trace7_data;
  output trace7_counter_overflow;
  input trace7_valid;
  output [44:0]trace8_counter;
  output trace8_read;
  input trace8_event;
  input [63:0]trace8_data;
  output trace8_counter_overflow;
  input trace8_valid;
  output [44:0]trace9_counter;
  output trace9_read;
  input trace9_event;
  input [63:0]trace9_data;
  output trace9_counter_overflow;
  input trace9_valid;
  output [44:0]trace10_counter;
  output trace10_read;
  input trace10_event;
  input [63:0]trace10_data;
  output trace10_counter_overflow;
  input trace10_valid;
  output [44:0]trace11_counter;
  output trace11_read;
  input trace11_event;
  input [63:0]trace11_data;
  output trace11_counter_overflow;
  input trace11_valid;
  output [44:0]trace12_counter;
  output trace12_read;
  input trace12_event;
  input [63:0]trace12_data;
  output trace12_counter_overflow;
  input trace12_valid;
  output [44:0]trace13_counter;
  output trace13_read;
  input trace13_event;
  input [63:0]trace13_data;
  output trace13_counter_overflow;
  input trace13_valid;
  output [44:0]trace14_counter;
  output trace14_read;
  input trace14_event;
  input [63:0]trace14_data;
  output trace14_counter_overflow;
  input trace14_valid;
  output [44:0]trace15_counter;
  output trace15_read;
  input trace15_event;
  input [63:0]trace15_data;
  output trace15_counter_overflow;
  input trace15_valid;
  output [44:0]trace16_counter;
  output trace16_read;
  input trace16_event;
  input [63:0]trace16_data;
  output trace16_counter_overflow;
  input trace16_valid;
  output [44:0]trace17_counter;
  output trace17_read;
  input trace17_event;
  input [63:0]trace17_data;
  output trace17_counter_overflow;
  input trace17_valid;
  output [44:0]trace18_counter;
  output trace18_read;
  input trace18_event;
  input [63:0]trace18_data;
  output trace18_counter_overflow;
  input trace18_valid;
  output [44:0]trace19_counter;
  output trace19_read;
  input trace19_event;
  input [63:0]trace19_data;
  output trace19_counter_overflow;
  input trace19_valid;
  output [44:0]trace20_counter;
  output trace20_read;
  input trace20_event;
  input [63:0]trace20_data;
  output trace20_counter_overflow;
  input trace20_valid;
  output [44:0]trace21_counter;
  output trace21_read;
  input trace21_event;
  input [63:0]trace21_data;
  output trace21_counter_overflow;
  input trace21_valid;
  output [44:0]trace22_counter;
  output trace22_read;
  input trace22_event;
  input [63:0]trace22_data;
  output trace22_counter_overflow;
  input trace22_valid;
  output [44:0]trace23_counter;
  output trace23_read;
  input trace23_event;
  input [63:0]trace23_data;
  output trace23_counter_overflow;
  input trace23_valid;
  output [44:0]trace24_counter;
  output trace24_read;
  input trace24_event;
  input [63:0]trace24_data;
  output trace24_counter_overflow;
  input trace24_valid;
  output [44:0]trace25_counter;
  output trace25_read;
  input trace25_event;
  input [63:0]trace25_data;
  output trace25_counter_overflow;
  input trace25_valid;
  output [44:0]trace26_counter;
  output trace26_read;
  input trace26_event;
  input [63:0]trace26_data;
  output trace26_counter_overflow;
  input trace26_valid;
  output [44:0]trace27_counter;
  output trace27_read;
  input trace27_event;
  input [63:0]trace27_data;
  output trace27_counter_overflow;
  input trace27_valid;
  output [44:0]trace28_counter;
  output trace28_read;
  input trace28_event;
  input [63:0]trace28_data;
  output trace28_counter_overflow;
  input trace28_valid;
  output [44:0]trace29_counter;
  output trace29_read;
  input trace29_event;
  input [63:0]trace29_data;
  output trace29_counter_overflow;
  input trace29_valid;
  output [44:0]trace30_counter;
  output trace30_read;
  input trace30_event;
  input [63:0]trace30_data;
  output trace30_counter_overflow;
  input trace30_valid;
  output [44:0]trace31_counter;
  output trace31_read;
  input trace31_event;
  input [63:0]trace31_data;
  output trace31_counter_overflow;
  input trace31_valid;
  output [44:0]trace32_counter;
  output trace32_read;
  input trace32_event;
  input [63:0]trace32_data;
  output trace32_counter_overflow;
  input trace32_valid;
  output [44:0]trace33_counter;
  output trace33_read;
  input trace33_event;
  input [63:0]trace33_data;
  output trace33_counter_overflow;
  input trace33_valid;
  output [44:0]trace34_counter;
  output trace34_read;
  input trace34_event;
  input [63:0]trace34_data;
  output trace34_counter_overflow;
  input trace34_valid;
  output [44:0]trace35_counter;
  output trace35_read;
  input trace35_event;
  input [63:0]trace35_data;
  output trace35_counter_overflow;
  input trace35_valid;
  output [44:0]trace36_counter;
  output trace36_read;
  input trace36_event;
  input [63:0]trace36_data;
  output trace36_counter_overflow;
  input trace36_valid;
  output [44:0]trace37_counter;
  output trace37_read;
  input trace37_event;
  input [63:0]trace37_data;
  output trace37_counter_overflow;
  input trace37_valid;
  output [44:0]trace38_counter;
  output trace38_read;
  input trace38_event;
  input [63:0]trace38_data;
  output trace38_counter_overflow;
  input trace38_valid;
  output [44:0]trace39_counter;
  output trace39_read;
  input trace39_event;
  input [63:0]trace39_data;
  output trace39_counter_overflow;
  input trace39_valid;
  output [44:0]trace40_counter;
  output trace40_read;
  input trace40_event;
  input [63:0]trace40_data;
  output trace40_counter_overflow;
  input trace40_valid;
  output [44:0]trace41_counter;
  output trace41_read;
  input trace41_event;
  input [63:0]trace41_data;
  output trace41_counter_overflow;
  input trace41_valid;
  output [44:0]trace42_counter;
  output trace42_read;
  input trace42_event;
  input [63:0]trace42_data;
  output trace42_counter_overflow;
  input trace42_valid;
  output [44:0]trace43_counter;
  output trace43_read;
  input trace43_event;
  input [63:0]trace43_data;
  output trace43_counter_overflow;
  input trace43_valid;
  output [44:0]trace44_counter;
  output trace44_read;
  input trace44_event;
  input [63:0]trace44_data;
  output trace44_counter_overflow;
  input trace44_valid;
  output [44:0]trace45_counter;
  output trace45_read;
  input trace45_event;
  input [63:0]trace45_data;
  output trace45_counter_overflow;
  input trace45_valid;
  output [44:0]trace46_counter;
  output trace46_read;
  input trace46_event;
  input [63:0]trace46_data;
  output trace46_counter_overflow;
  input trace46_valid;
  output [44:0]trace47_counter;
  output trace47_read;
  input trace47_event;
  input [63:0]trace47_data;
  output trace47_counter_overflow;
  input trace47_valid;
  output [44:0]trace48_counter;
  output trace48_read;
  input trace48_event;
  input [63:0]trace48_data;
  output trace48_counter_overflow;
  input trace48_valid;
  output [44:0]trace49_counter;
  output trace49_read;
  input trace49_event;
  input [63:0]trace49_data;
  output trace49_counter_overflow;
  input trace49_valid;
  output [44:0]trace50_counter;
  output trace50_read;
  input trace50_event;
  input [63:0]trace50_data;
  output trace50_counter_overflow;
  input trace50_valid;
  output [44:0]trace51_counter;
  output trace51_read;
  input trace51_event;
  input [63:0]trace51_data;
  output trace51_counter_overflow;
  input trace51_valid;
  output [44:0]trace52_counter;
  output trace52_read;
  input trace52_event;
  input [63:0]trace52_data;
  output trace52_counter_overflow;
  input trace52_valid;
  output [44:0]trace53_counter;
  output trace53_read;
  input trace53_event;
  input [63:0]trace53_data;
  output trace53_counter_overflow;
  input trace53_valid;
  output [44:0]trace54_counter;
  output trace54_read;
  input trace54_event;
  input [63:0]trace54_data;
  output trace54_counter_overflow;
  input trace54_valid;
  output [44:0]trace55_counter;
  output trace55_read;
  input trace55_event;
  input [63:0]trace55_data;
  output trace55_counter_overflow;
  input trace55_valid;
endmodule

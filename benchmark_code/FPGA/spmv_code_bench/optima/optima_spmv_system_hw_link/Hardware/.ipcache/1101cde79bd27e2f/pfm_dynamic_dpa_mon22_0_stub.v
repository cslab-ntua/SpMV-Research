// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2020.2 (lin64) Build 3064766 Wed Nov 18 09:12:47 MST 2020
// Date        : Fri Oct 20 19:07:54 2023
// Host        : gold1 running 64-bit Ubuntu 18.04.6 LTS
// Command     : write_verilog -force -mode synth_stub -rename_top decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix -prefix
//               decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_ pfm_dynamic_dpa_mon22_0_stub.v
// Design      : pfm_dynamic_dpa_mon22_0
// Purpose     : Stub declaration of top-level module interface
// Device      : xcu280-fsvh2892-2L-e
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* X_CORE_INFO = "axi_interface_monitor,Vivado 2020.2" *)
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix(mon_clk, mon_resetn, trace_clk, trace_rst, 
  rtrace_counter_overflow, rtrace_counter, rtrace_event, rtrace_data, rtrace_valid, 
  rtrace_read, wtrace_counter_overflow, wtrace_counter, wtrace_event, wtrace_data, 
  wtrace_valid, wtrace_read, s_axi_awaddr, s_axi_awprot, s_axi_awvalid, s_axi_awready, 
  s_axi_wdata, s_axi_wstrb, s_axi_wvalid, s_axi_wready, s_axi_bvalid, s_axi_bready, 
  s_axi_bresp, s_axi_araddr, s_axi_arprot, s_axi_arvalid, s_axi_arready, s_axi_rdata, 
  s_axi_rresp, s_axi_rvalid, s_axi_rready, s_axi_awaddr_mon, s_axi_awprot_mon, 
  s_axi_awvalid_mon, s_axi_awready_mon, s_axi_wdata_mon, s_axi_wstrb_mon, s_axi_wvalid_mon, 
  s_axi_wready_mon, s_axi_bresp_mon, s_axi_bvalid_mon, s_axi_bready_mon, s_axi_araddr_mon, 
  s_axi_arprot_mon, s_axi_arvalid_mon, s_axi_arready_mon, s_axi_rdata_mon, s_axi_rresp_mon, 
  s_axi_rvalid_mon, s_axi_rready_mon, m_axi_AWVALID, m_axi_AWREADY, m_axi_AWADDR, m_axi_AWID, 
  m_axi_AWLEN, m_axi_AWSIZE, m_axi_AWBURST, m_axi_AWLOCK, m_axi_AWCACHE, m_axi_AWPROT, 
  m_axi_AWQOS, m_axi_AWREGION, m_axi_AWUSER, m_axi_WVALID, m_axi_WREADY, m_axi_WDATA, 
  m_axi_WSTRB, m_axi_WLAST, m_axi_WID, m_axi_WUSER, m_axi_ARVALID, m_axi_ARREADY, m_axi_ARADDR, 
  m_axi_ARID, m_axi_ARLEN, m_axi_ARSIZE, m_axi_ARBURST, m_axi_ARLOCK, m_axi_ARCACHE, 
  m_axi_ARPROT, m_axi_ARQOS, m_axi_ARREGION, m_axi_ARUSER, m_axi_RVALID, m_axi_RREADY, 
  m_axi_RDATA, m_axi_RLAST, m_axi_RID, m_axi_RUSER, m_axi_RRESP, m_axi_BVALID, m_axi_BREADY, 
  m_axi_BRESP, m_axi_BID, m_axi_BUSER)
/* synthesis syn_black_box black_box_pad_pin="mon_clk,mon_resetn,trace_clk,trace_rst,rtrace_counter_overflow,rtrace_counter[44:0],rtrace_event,rtrace_data[63:0],rtrace_valid,rtrace_read,wtrace_counter_overflow,wtrace_counter[44:0],wtrace_event,wtrace_data[63:0],wtrace_valid,wtrace_read,s_axi_awaddr[7:0],s_axi_awprot[2:0],s_axi_awvalid,s_axi_awready,s_axi_wdata[31:0],s_axi_wstrb[3:0],s_axi_wvalid,s_axi_wready,s_axi_bvalid,s_axi_bready,s_axi_bresp[1:0],s_axi_araddr[7:0],s_axi_arprot[2:0],s_axi_arvalid,s_axi_arready,s_axi_rdata[31:0],s_axi_rresp[1:0],s_axi_rvalid,s_axi_rready,s_axi_awaddr_mon[7:0],s_axi_awprot_mon[2:0],s_axi_awvalid_mon,s_axi_awready_mon,s_axi_wdata_mon[31:0],s_axi_wstrb_mon[3:0],s_axi_wvalid_mon,s_axi_wready_mon,s_axi_bresp_mon[1:0],s_axi_bvalid_mon,s_axi_bready_mon,s_axi_araddr_mon[7:0],s_axi_arprot_mon[2:0],s_axi_arvalid_mon,s_axi_arready_mon,s_axi_rdata_mon[31:0],s_axi_rresp_mon[1:0],s_axi_rvalid_mon,s_axi_rready_mon,m_axi_AWVALID,m_axi_AWREADY,m_axi_AWADDR[63:0],m_axi_AWID[0:0],m_axi_AWLEN[7:0],m_axi_AWSIZE[2:0],m_axi_AWBURST[1:0],m_axi_AWLOCK[1:0],m_axi_AWCACHE[3:0],m_axi_AWPROT[2:0],m_axi_AWQOS[3:0],m_axi_AWREGION[3:0],m_axi_AWUSER[0:0],m_axi_WVALID,m_axi_WREADY,m_axi_WDATA[511:0],m_axi_WSTRB[63:0],m_axi_WLAST,m_axi_WID[0:0],m_axi_WUSER[0:0],m_axi_ARVALID,m_axi_ARREADY,m_axi_ARADDR[63:0],m_axi_ARID[0:0],m_axi_ARLEN[7:0],m_axi_ARSIZE[2:0],m_axi_ARBURST[1:0],m_axi_ARLOCK[1:0],m_axi_ARCACHE[3:0],m_axi_ARPROT[2:0],m_axi_ARQOS[3:0],m_axi_ARREGION[3:0],m_axi_ARUSER[0:0],m_axi_RVALID,m_axi_RREADY,m_axi_RDATA[511:0],m_axi_RLAST,m_axi_RID[0:0],m_axi_RUSER[0:0],m_axi_RRESP,m_axi_BVALID,m_axi_BREADY,m_axi_BRESP,m_axi_BID[0:0],m_axi_BUSER[0:0]" */;
  input mon_clk;
  input mon_resetn;
  input trace_clk;
  input trace_rst;
  input rtrace_counter_overflow;
  input [44:0]rtrace_counter;
  output rtrace_event;
  output [63:0]rtrace_data;
  output rtrace_valid;
  input rtrace_read;
  input wtrace_counter_overflow;
  input [44:0]wtrace_counter;
  output wtrace_event;
  output [63:0]wtrace_data;
  output wtrace_valid;
  input wtrace_read;
  input [7:0]s_axi_awaddr;
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
  input [7:0]s_axi_araddr;
  input [2:0]s_axi_arprot;
  input s_axi_arvalid;
  output s_axi_arready;
  output [31:0]s_axi_rdata;
  output [1:0]s_axi_rresp;
  output s_axi_rvalid;
  input s_axi_rready;
  input [7:0]s_axi_awaddr_mon;
  input [2:0]s_axi_awprot_mon;
  input s_axi_awvalid_mon;
  input s_axi_awready_mon;
  input [31:0]s_axi_wdata_mon;
  input [3:0]s_axi_wstrb_mon;
  input s_axi_wvalid_mon;
  input s_axi_wready_mon;
  input [1:0]s_axi_bresp_mon;
  input s_axi_bvalid_mon;
  input s_axi_bready_mon;
  input [7:0]s_axi_araddr_mon;
  input [2:0]s_axi_arprot_mon;
  input s_axi_arvalid_mon;
  input s_axi_arready_mon;
  input [31:0]s_axi_rdata_mon;
  input [1:0]s_axi_rresp_mon;
  input s_axi_rvalid_mon;
  input s_axi_rready_mon;
  input m_axi_AWVALID;
  input m_axi_AWREADY;
  input [63:0]m_axi_AWADDR;
  input [0:0]m_axi_AWID;
  input [7:0]m_axi_AWLEN;
  input [2:0]m_axi_AWSIZE;
  input [1:0]m_axi_AWBURST;
  input [1:0]m_axi_AWLOCK;
  input [3:0]m_axi_AWCACHE;
  input [2:0]m_axi_AWPROT;
  input [3:0]m_axi_AWQOS;
  input [3:0]m_axi_AWREGION;
  input [0:0]m_axi_AWUSER;
  input m_axi_WVALID;
  input m_axi_WREADY;
  input [511:0]m_axi_WDATA;
  input [63:0]m_axi_WSTRB;
  input m_axi_WLAST;
  input [0:0]m_axi_WID;
  input [0:0]m_axi_WUSER;
  input m_axi_ARVALID;
  input m_axi_ARREADY;
  input [63:0]m_axi_ARADDR;
  input [0:0]m_axi_ARID;
  input [7:0]m_axi_ARLEN;
  input [2:0]m_axi_ARSIZE;
  input [1:0]m_axi_ARBURST;
  input [1:0]m_axi_ARLOCK;
  input [3:0]m_axi_ARCACHE;
  input [2:0]m_axi_ARPROT;
  input [3:0]m_axi_ARQOS;
  input [3:0]m_axi_ARREGION;
  input [0:0]m_axi_ARUSER;
  input m_axi_RVALID;
  input m_axi_RREADY;
  input [511:0]m_axi_RDATA;
  input m_axi_RLAST;
  input [0:0]m_axi_RID;
  input [0:0]m_axi_RUSER;
  input m_axi_RRESP;
  input m_axi_BVALID;
  input m_axi_BREADY;
  input m_axi_BRESP;
  input [0:0]m_axi_BID;
  input [0:0]m_axi_BUSER;
endmodule

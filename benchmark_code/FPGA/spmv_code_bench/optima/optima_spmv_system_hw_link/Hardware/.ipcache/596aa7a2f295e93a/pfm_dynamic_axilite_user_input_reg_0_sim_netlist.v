// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2020.2 (lin64) Build 3064766 Wed Nov 18 09:12:47 MST 2020
// Date        : Fri Oct 20 18:48:34 2023
// Host        : gold1 running 64-bit Ubuntu 18.04.6 LTS
// Command     : write_verilog -force -mode funcsim -rename_top decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix -prefix
//               decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_ pfm_dynamic_axilite_user_input_reg_0_sim_netlist.v
// Design      : pfm_dynamic_axilite_user_input_reg_0
// Purpose     : This verilog netlist is a functional simulation representation of the design and should not be modified
//               or synthesized. This netlist cannot be used for SDF annotated simulation.
// Device      : xcu280-fsvh2892-2L-e
// --------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

(* C_AXI_ADDR_WIDTH = "32" *) (* C_AXI_ARUSER_WIDTH = "1" *) (* C_AXI_AWUSER_WIDTH = "1" *) 
(* C_AXI_BUSER_WIDTH = "1" *) (* C_AXI_DATA_WIDTH = "32" *) (* C_AXI_ID_WIDTH = "1" *) 
(* C_AXI_PROTOCOL = "2" *) (* C_AXI_RUSER_WIDTH = "1" *) (* C_AXI_SUPPORTS_REGION_SIGNALS = "0" *) 
(* C_AXI_SUPPORTS_USER_SIGNALS = "0" *) (* C_AXI_WUSER_WIDTH = "1" *) (* C_FAMILY = "virtexuplusHBM" *) 
(* C_NUM_SLR_CROSSINGS = "0" *) (* C_PIPELINES_MASTER_AR = "0" *) (* C_PIPELINES_MASTER_AW = "0" *) 
(* C_PIPELINES_MASTER_B = "0" *) (* C_PIPELINES_MASTER_R = "0" *) (* C_PIPELINES_MASTER_W = "0" *) 
(* C_PIPELINES_MIDDLE_AR = "0" *) (* C_PIPELINES_MIDDLE_AW = "0" *) (* C_PIPELINES_MIDDLE_B = "0" *) 
(* C_PIPELINES_MIDDLE_R = "0" *) (* C_PIPELINES_MIDDLE_W = "0" *) (* C_PIPELINES_SLAVE_AR = "0" *) 
(* C_PIPELINES_SLAVE_AW = "0" *) (* C_PIPELINES_SLAVE_B = "0" *) (* C_PIPELINES_SLAVE_R = "0" *) 
(* C_PIPELINES_SLAVE_W = "0" *) (* C_REG_CONFIG_AR = "9" *) (* C_REG_CONFIG_AW = "9" *) 
(* C_REG_CONFIG_B = "9" *) (* C_REG_CONFIG_R = "9" *) (* C_REG_CONFIG_W = "9" *) 
(* C_RESERVE_MODE = "0" *) (* DowngradeIPIdentifiedWarnings = "yes" *) (* G_AXI_ARADDR_INDEX = "0" *) 
(* G_AXI_ARADDR_WIDTH = "32" *) (* G_AXI_ARBURST_INDEX = "35" *) (* G_AXI_ARBURST_WIDTH = "0" *) 
(* G_AXI_ARCACHE_INDEX = "35" *) (* G_AXI_ARCACHE_WIDTH = "0" *) (* G_AXI_ARID_INDEX = "35" *) 
(* G_AXI_ARID_WIDTH = "0" *) (* G_AXI_ARLEN_INDEX = "35" *) (* G_AXI_ARLEN_WIDTH = "0" *) 
(* G_AXI_ARLOCK_INDEX = "35" *) (* G_AXI_ARLOCK_WIDTH = "0" *) (* G_AXI_ARPAYLOAD_WIDTH = "35" *) 
(* G_AXI_ARPROT_INDEX = "32" *) (* G_AXI_ARPROT_WIDTH = "3" *) (* G_AXI_ARQOS_INDEX = "35" *) 
(* G_AXI_ARQOS_WIDTH = "0" *) (* G_AXI_ARREGION_INDEX = "35" *) (* G_AXI_ARREGION_WIDTH = "0" *) 
(* G_AXI_ARSIZE_INDEX = "35" *) (* G_AXI_ARSIZE_WIDTH = "0" *) (* G_AXI_ARUSER_INDEX = "35" *) 
(* G_AXI_ARUSER_WIDTH = "0" *) (* G_AXI_AWADDR_INDEX = "0" *) (* G_AXI_AWADDR_WIDTH = "32" *) 
(* G_AXI_AWBURST_INDEX = "35" *) (* G_AXI_AWBURST_WIDTH = "0" *) (* G_AXI_AWCACHE_INDEX = "35" *) 
(* G_AXI_AWCACHE_WIDTH = "0" *) (* G_AXI_AWID_INDEX = "35" *) (* G_AXI_AWID_WIDTH = "0" *) 
(* G_AXI_AWLEN_INDEX = "35" *) (* G_AXI_AWLEN_WIDTH = "0" *) (* G_AXI_AWLOCK_INDEX = "35" *) 
(* G_AXI_AWLOCK_WIDTH = "0" *) (* G_AXI_AWPAYLOAD_WIDTH = "35" *) (* G_AXI_AWPROT_INDEX = "32" *) 
(* G_AXI_AWPROT_WIDTH = "3" *) (* G_AXI_AWQOS_INDEX = "35" *) (* G_AXI_AWQOS_WIDTH = "0" *) 
(* G_AXI_AWREGION_INDEX = "35" *) (* G_AXI_AWREGION_WIDTH = "0" *) (* G_AXI_AWSIZE_INDEX = "35" *) 
(* G_AXI_AWSIZE_WIDTH = "0" *) (* G_AXI_AWUSER_INDEX = "35" *) (* G_AXI_AWUSER_WIDTH = "0" *) 
(* G_AXI_BID_INDEX = "2" *) (* G_AXI_BID_WIDTH = "0" *) (* G_AXI_BPAYLOAD_WIDTH = "2" *) 
(* G_AXI_BRESP_INDEX = "0" *) (* G_AXI_BRESP_WIDTH = "2" *) (* G_AXI_BUSER_INDEX = "2" *) 
(* G_AXI_BUSER_WIDTH = "0" *) (* G_AXI_RDATA_INDEX = "0" *) (* G_AXI_RDATA_WIDTH = "32" *) 
(* G_AXI_RID_INDEX = "34" *) (* G_AXI_RID_WIDTH = "0" *) (* G_AXI_RLAST_INDEX = "34" *) 
(* G_AXI_RLAST_WIDTH = "0" *) (* G_AXI_RPAYLOAD_WIDTH = "34" *) (* G_AXI_RRESP_INDEX = "32" *) 
(* G_AXI_RRESP_WIDTH = "2" *) (* G_AXI_RUSER_INDEX = "34" *) (* G_AXI_RUSER_WIDTH = "0" *) 
(* G_AXI_WDATA_INDEX = "0" *) (* G_AXI_WDATA_WIDTH = "32" *) (* G_AXI_WID_INDEX = "36" *) 
(* G_AXI_WID_WIDTH = "0" *) (* G_AXI_WLAST_INDEX = "36" *) (* G_AXI_WLAST_WIDTH = "0" *) 
(* G_AXI_WPAYLOAD_WIDTH = "36" *) (* G_AXI_WSTRB_INDEX = "32" *) (* G_AXI_WSTRB_WIDTH = "4" *) 
(* G_AXI_WUSER_INDEX = "36" *) (* G_AXI_WUSER_WIDTH = "0" *) (* P_FORWARD = "0" *) 
(* P_RESPONSE = "1" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_axi_register_slice
   (aclk,
    aclk2x,
    aresetn,
    s_axi_awid,
    s_axi_awaddr,
    s_axi_awlen,
    s_axi_awsize,
    s_axi_awburst,
    s_axi_awlock,
    s_axi_awcache,
    s_axi_awprot,
    s_axi_awregion,
    s_axi_awqos,
    s_axi_awuser,
    s_axi_awvalid,
    s_axi_awready,
    s_axi_wid,
    s_axi_wdata,
    s_axi_wstrb,
    s_axi_wlast,
    s_axi_wuser,
    s_axi_wvalid,
    s_axi_wready,
    s_axi_bid,
    s_axi_bresp,
    s_axi_buser,
    s_axi_bvalid,
    s_axi_bready,
    s_axi_arid,
    s_axi_araddr,
    s_axi_arlen,
    s_axi_arsize,
    s_axi_arburst,
    s_axi_arlock,
    s_axi_arcache,
    s_axi_arprot,
    s_axi_arregion,
    s_axi_arqos,
    s_axi_aruser,
    s_axi_arvalid,
    s_axi_arready,
    s_axi_rid,
    s_axi_rdata,
    s_axi_rresp,
    s_axi_rlast,
    s_axi_ruser,
    s_axi_rvalid,
    s_axi_rready,
    m_axi_awid,
    m_axi_awaddr,
    m_axi_awlen,
    m_axi_awsize,
    m_axi_awburst,
    m_axi_awlock,
    m_axi_awcache,
    m_axi_awprot,
    m_axi_awregion,
    m_axi_awqos,
    m_axi_awuser,
    m_axi_awvalid,
    m_axi_awready,
    m_axi_wid,
    m_axi_wdata,
    m_axi_wstrb,
    m_axi_wlast,
    m_axi_wuser,
    m_axi_wvalid,
    m_axi_wready,
    m_axi_bid,
    m_axi_bresp,
    m_axi_buser,
    m_axi_bvalid,
    m_axi_bready,
    m_axi_arid,
    m_axi_araddr,
    m_axi_arlen,
    m_axi_arsize,
    m_axi_arburst,
    m_axi_arlock,
    m_axi_arcache,
    m_axi_arprot,
    m_axi_arregion,
    m_axi_arqos,
    m_axi_aruser,
    m_axi_arvalid,
    m_axi_arready,
    m_axi_rid,
    m_axi_rdata,
    m_axi_rresp,
    m_axi_rlast,
    m_axi_ruser,
    m_axi_rvalid,
    m_axi_rready);
  input aclk;
  input aclk2x;
  input aresetn;
  input [0:0]s_axi_awid;
  input [31:0]s_axi_awaddr;
  input [7:0]s_axi_awlen;
  input [2:0]s_axi_awsize;
  input [1:0]s_axi_awburst;
  input [0:0]s_axi_awlock;
  input [3:0]s_axi_awcache;
  input [2:0]s_axi_awprot;
  input [3:0]s_axi_awregion;
  input [3:0]s_axi_awqos;
  input [0:0]s_axi_awuser;
  input s_axi_awvalid;
  output s_axi_awready;
  input [0:0]s_axi_wid;
  input [31:0]s_axi_wdata;
  input [3:0]s_axi_wstrb;
  input s_axi_wlast;
  input [0:0]s_axi_wuser;
  input s_axi_wvalid;
  output s_axi_wready;
  output [0:0]s_axi_bid;
  output [1:0]s_axi_bresp;
  output [0:0]s_axi_buser;
  output s_axi_bvalid;
  input s_axi_bready;
  input [0:0]s_axi_arid;
  input [31:0]s_axi_araddr;
  input [7:0]s_axi_arlen;
  input [2:0]s_axi_arsize;
  input [1:0]s_axi_arburst;
  input [0:0]s_axi_arlock;
  input [3:0]s_axi_arcache;
  input [2:0]s_axi_arprot;
  input [3:0]s_axi_arregion;
  input [3:0]s_axi_arqos;
  input [0:0]s_axi_aruser;
  input s_axi_arvalid;
  output s_axi_arready;
  output [0:0]s_axi_rid;
  output [31:0]s_axi_rdata;
  output [1:0]s_axi_rresp;
  output s_axi_rlast;
  output [0:0]s_axi_ruser;
  output s_axi_rvalid;
  input s_axi_rready;
  output [0:0]m_axi_awid;
  output [31:0]m_axi_awaddr;
  output [7:0]m_axi_awlen;
  output [2:0]m_axi_awsize;
  output [1:0]m_axi_awburst;
  output [0:0]m_axi_awlock;
  output [3:0]m_axi_awcache;
  output [2:0]m_axi_awprot;
  output [3:0]m_axi_awregion;
  output [3:0]m_axi_awqos;
  output [0:0]m_axi_awuser;
  output m_axi_awvalid;
  input m_axi_awready;
  output [0:0]m_axi_wid;
  output [31:0]m_axi_wdata;
  output [3:0]m_axi_wstrb;
  output m_axi_wlast;
  output [0:0]m_axi_wuser;
  output m_axi_wvalid;
  input m_axi_wready;
  input [0:0]m_axi_bid;
  input [1:0]m_axi_bresp;
  input [0:0]m_axi_buser;
  input m_axi_bvalid;
  output m_axi_bready;
  output [0:0]m_axi_arid;
  output [31:0]m_axi_araddr;
  output [7:0]m_axi_arlen;
  output [2:0]m_axi_arsize;
  output [1:0]m_axi_arburst;
  output [0:0]m_axi_arlock;
  output [3:0]m_axi_arcache;
  output [2:0]m_axi_arprot;
  output [3:0]m_axi_arregion;
  output [3:0]m_axi_arqos;
  output [0:0]m_axi_aruser;
  output m_axi_arvalid;
  input m_axi_arready;
  input [0:0]m_axi_rid;
  input [31:0]m_axi_rdata;
  input [1:0]m_axi_rresp;
  input m_axi_rlast;
  input [0:0]m_axi_ruser;
  input m_axi_rvalid;
  output m_axi_rready;

  wire \<const0> ;
  wire aclk;
  wire areset_d;
  wire aresetn;
  wire [31:0]m_axi_araddr;
  wire [2:0]m_axi_arprot;
  wire m_axi_arready;
  wire m_axi_arvalid;
  wire [31:0]m_axi_awaddr;
  wire [2:0]m_axi_awprot;
  wire m_axi_awready;
  wire m_axi_awvalid;
  wire m_axi_bready;
  wire [1:0]m_axi_bresp;
  wire m_axi_bvalid;
  wire [31:0]m_axi_rdata;
  wire m_axi_rready;
  wire [1:0]m_axi_rresp;
  wire m_axi_rvalid;
  wire [31:0]m_axi_wdata;
  wire m_axi_wready;
  wire [3:0]m_axi_wstrb;
  wire m_axi_wvalid;
  wire [31:0]s_axi_araddr;
  wire [2:0]s_axi_arprot;
  wire s_axi_arready;
  wire s_axi_arvalid;
  wire [31:0]s_axi_awaddr;
  wire [2:0]s_axi_awprot;
  wire s_axi_awready;
  wire s_axi_awvalid;
  wire s_axi_bready;
  wire [1:0]s_axi_bresp;
  wire s_axi_bvalid;
  wire [31:0]s_axi_rdata;
  wire s_axi_rready;
  wire [1:0]s_axi_rresp;
  wire s_axi_rvalid;
  wire [31:0]s_axi_wdata;
  wire s_axi_wready;
  wire [3:0]s_axi_wstrb;
  wire s_axi_wvalid;

  assign m_axi_arburst[1] = \<const0> ;
  assign m_axi_arburst[0] = \<const0> ;
  assign m_axi_arcache[3] = \<const0> ;
  assign m_axi_arcache[2] = \<const0> ;
  assign m_axi_arcache[1] = \<const0> ;
  assign m_axi_arcache[0] = \<const0> ;
  assign m_axi_arid[0] = \<const0> ;
  assign m_axi_arlen[7] = \<const0> ;
  assign m_axi_arlen[6] = \<const0> ;
  assign m_axi_arlen[5] = \<const0> ;
  assign m_axi_arlen[4] = \<const0> ;
  assign m_axi_arlen[3] = \<const0> ;
  assign m_axi_arlen[2] = \<const0> ;
  assign m_axi_arlen[1] = \<const0> ;
  assign m_axi_arlen[0] = \<const0> ;
  assign m_axi_arlock[0] = \<const0> ;
  assign m_axi_arqos[3] = \<const0> ;
  assign m_axi_arqos[2] = \<const0> ;
  assign m_axi_arqos[1] = \<const0> ;
  assign m_axi_arqos[0] = \<const0> ;
  assign m_axi_arregion[3] = \<const0> ;
  assign m_axi_arregion[2] = \<const0> ;
  assign m_axi_arregion[1] = \<const0> ;
  assign m_axi_arregion[0] = \<const0> ;
  assign m_axi_arsize[2] = \<const0> ;
  assign m_axi_arsize[1] = \<const0> ;
  assign m_axi_arsize[0] = \<const0> ;
  assign m_axi_aruser[0] = \<const0> ;
  assign m_axi_awburst[1] = \<const0> ;
  assign m_axi_awburst[0] = \<const0> ;
  assign m_axi_awcache[3] = \<const0> ;
  assign m_axi_awcache[2] = \<const0> ;
  assign m_axi_awcache[1] = \<const0> ;
  assign m_axi_awcache[0] = \<const0> ;
  assign m_axi_awid[0] = \<const0> ;
  assign m_axi_awlen[7] = \<const0> ;
  assign m_axi_awlen[6] = \<const0> ;
  assign m_axi_awlen[5] = \<const0> ;
  assign m_axi_awlen[4] = \<const0> ;
  assign m_axi_awlen[3] = \<const0> ;
  assign m_axi_awlen[2] = \<const0> ;
  assign m_axi_awlen[1] = \<const0> ;
  assign m_axi_awlen[0] = \<const0> ;
  assign m_axi_awlock[0] = \<const0> ;
  assign m_axi_awqos[3] = \<const0> ;
  assign m_axi_awqos[2] = \<const0> ;
  assign m_axi_awqos[1] = \<const0> ;
  assign m_axi_awqos[0] = \<const0> ;
  assign m_axi_awregion[3] = \<const0> ;
  assign m_axi_awregion[2] = \<const0> ;
  assign m_axi_awregion[1] = \<const0> ;
  assign m_axi_awregion[0] = \<const0> ;
  assign m_axi_awsize[2] = \<const0> ;
  assign m_axi_awsize[1] = \<const0> ;
  assign m_axi_awsize[0] = \<const0> ;
  assign m_axi_awuser[0] = \<const0> ;
  assign m_axi_wid[0] = \<const0> ;
  assign m_axi_wlast = \<const0> ;
  assign m_axi_wuser[0] = \<const0> ;
  assign s_axi_bid[0] = \<const0> ;
  assign s_axi_buser[0] = \<const0> ;
  assign s_axi_rid[0] = \<const0> ;
  assign s_axi_rlast = \<const0> ;
  assign s_axi_ruser[0] = \<const0> ;
  GND GND
       (.G(\<const0> ));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_axic_register_slice \ar.ar_pipe 
       (.D({s_axi_arprot,s_axi_araddr}),
        .SR(areset_d),
        .aclk(aclk),
        .m_axi_araddr(m_axi_araddr),
        .m_axi_arprot(m_axi_arprot),
        .m_axi_arready(m_axi_arready),
        .m_axi_arvalid(m_axi_arvalid),
        .s_axi_arready(s_axi_arready),
        .s_axi_arvalid(s_axi_arvalid));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_axic_register_slice_0 \aw.aw_pipe 
       (.D({s_axi_awprot,s_axi_awaddr}),
        .SR(areset_d),
        .aclk(aclk),
        .aresetn(aresetn),
        .m_axi_awaddr(m_axi_awaddr),
        .m_axi_awprot(m_axi_awprot),
        .m_axi_awready(m_axi_awready),
        .m_axi_awvalid(m_axi_awvalid),
        .s_axi_awready(s_axi_awready),
        .s_axi_awvalid(s_axi_awvalid));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_axic_register_slice__parameterized1 \b.b_pipe 
       (.SR(areset_d),
        .aclk(aclk),
        .m_axi_bready(m_axi_bready),
        .m_axi_bresp(m_axi_bresp),
        .m_axi_bvalid(m_axi_bvalid),
        .s_axi_bready(s_axi_bready),
        .s_axi_bresp(s_axi_bresp),
        .s_axi_bvalid(s_axi_bvalid));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_axic_register_slice__parameterized2 \r.r_pipe 
       (.D({m_axi_rresp,m_axi_rdata}),
        .SR(areset_d),
        .aclk(aclk),
        .m_axi_rready(m_axi_rready),
        .m_axi_rvalid(m_axi_rvalid),
        .s_axi_rdata(s_axi_rdata),
        .s_axi_rready(s_axi_rready),
        .s_axi_rresp(s_axi_rresp),
        .s_axi_rvalid(s_axi_rvalid));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_axic_register_slice__parameterized0 \w.w_pipe 
       (.D({s_axi_wstrb,s_axi_wdata}),
        .SR(areset_d),
        .aclk(aclk),
        .m_axi_wdata(m_axi_wdata),
        .m_axi_wready(m_axi_wready),
        .m_axi_wstrb(m_axi_wstrb),
        .m_axi_wvalid(m_axi_wvalid),
        .s_axi_wready(s_axi_wready),
        .s_axi_wvalid(s_axi_wvalid));
endmodule

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_axic_register_slice
   (s_axi_arready,
    m_axi_arvalid,
    m_axi_araddr,
    m_axi_arprot,
    aclk,
    SR,
    s_axi_arvalid,
    D,
    m_axi_arready);
  output s_axi_arready;
  output m_axi_arvalid;
  output [31:0]m_axi_araddr;
  output [2:0]m_axi_arprot;
  input aclk;
  input [0:0]SR;
  input s_axi_arvalid;
  input [34:0]D;
  input m_axi_arready;

  wire [34:0]D;
  wire [0:0]SR;
  wire aclk;
  wire [2:0]fifoaddr;
  wire \fifoaddr[0]_i_1__2_n_0 ;
  wire \fifoaddr[1]_i_1__2_n_0 ;
  wire \fifoaddr[2]_i_1__2_n_0 ;
  wire \fifoaddr[2]_i_2__2_n_0 ;
  wire [31:0]m_axi_araddr;
  wire [2:0]m_axi_arprot;
  wire m_axi_arready;
  wire m_axi_arvalid;
  wire push;
  wire s_axi_arready;
  wire s_axi_arvalid;
  wire [34:0]s_payload_d;
  wire s_ready_d;
  wire s_ready_i;
  wire s_valid_d;
  wire [34:0]srl_out;

  (* SOFT_HLUTNM = "soft_lutpair0" *) 
  LUT1 #(
    .INIT(2'h1)) 
    \fifoaddr[0]_i_1__2 
       (.I0(fifoaddr[0]),
        .O(\fifoaddr[0]_i_1__2_n_0 ));
  LUT6 #(
    .INIT(64'hFF00AA557F40AA15)) 
    \fifoaddr[1]_i_1__2 
       (.I0(fifoaddr[1]),
        .I1(s_valid_d),
        .I2(s_ready_d),
        .I3(fifoaddr[2]),
        .I4(fifoaddr[0]),
        .I5(m_axi_arready),
        .O(\fifoaddr[1]_i_1__2_n_0 ));
  LUT6 #(
    .INIT(64'h7FE53FEA3FEA3FEA)) 
    \fifoaddr[2]_i_1__2 
       (.I0(m_axi_arready),
        .I1(fifoaddr[0]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[2]),
        .I4(s_ready_d),
        .I5(s_valid_d),
        .O(\fifoaddr[2]_i_1__2_n_0 ));
  LUT6 #(
    .INIT(64'hFFAAAAFF7FAAAABF)) 
    \fifoaddr[2]_i_2__2 
       (.I0(fifoaddr[2]),
        .I1(s_valid_d),
        .I2(s_ready_d),
        .I3(fifoaddr[0]),
        .I4(fifoaddr[1]),
        .I5(m_axi_arready),
        .O(\fifoaddr[2]_i_2__2_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \fifoaddr_reg[0] 
       (.C(aclk),
        .CE(\fifoaddr[2]_i_1__2_n_0 ),
        .D(\fifoaddr[0]_i_1__2_n_0 ),
        .Q(fifoaddr[0]),
        .R(SR));
  FDSE #(
    .INIT(1'b1)) 
    \fifoaddr_reg[1] 
       (.C(aclk),
        .CE(\fifoaddr[2]_i_1__2_n_0 ),
        .D(\fifoaddr[1]_i_1__2_n_0 ),
        .Q(fifoaddr[1]),
        .S(SR));
  FDSE #(
    .INIT(1'b1)) 
    \fifoaddr_reg[2] 
       (.C(aclk),
        .CE(\fifoaddr[2]_i_1__2_n_0 ),
        .D(\fifoaddr[2]_i_2__2_n_0 ),
        .Q(fifoaddr[2]),
        .S(SR));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_107 \gen_srls[0].srl_nx1 
       (.Q(s_payload_d[0]),
        .aclk(aclk),
        .\m_axi_araddr[0] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[0]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_108 \gen_srls[10].srl_nx1 
       (.Q(s_payload_d[10]),
        .aclk(aclk),
        .\m_axi_araddr[10] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[10]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_109 \gen_srls[11].srl_nx1 
       (.Q(s_payload_d[11]),
        .aclk(aclk),
        .\m_axi_araddr[11] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[11]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_110 \gen_srls[12].srl_nx1 
       (.Q(s_payload_d[12]),
        .aclk(aclk),
        .\m_axi_araddr[12] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[12]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_111 \gen_srls[13].srl_nx1 
       (.Q(s_payload_d[13]),
        .aclk(aclk),
        .\m_axi_araddr[13] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[13]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_112 \gen_srls[14].srl_nx1 
       (.Q(s_payload_d[14]),
        .aclk(aclk),
        .\m_axi_araddr[14] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[14]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_113 \gen_srls[15].srl_nx1 
       (.Q(s_payload_d[15]),
        .aclk(aclk),
        .\m_axi_araddr[15] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[15]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_114 \gen_srls[16].srl_nx1 
       (.Q(s_payload_d[16]),
        .aclk(aclk),
        .\m_axi_araddr[16] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[16]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_115 \gen_srls[17].srl_nx1 
       (.Q(s_payload_d[17]),
        .aclk(aclk),
        .\m_axi_araddr[17] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[17]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_116 \gen_srls[18].srl_nx1 
       (.Q(s_payload_d[18]),
        .aclk(aclk),
        .\m_axi_araddr[18] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[18]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_117 \gen_srls[19].srl_nx1 
       (.Q(s_payload_d[19]),
        .aclk(aclk),
        .\m_axi_araddr[19] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[19]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_118 \gen_srls[1].srl_nx1 
       (.Q(s_payload_d[1]),
        .aclk(aclk),
        .\m_axi_araddr[1] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[1]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_119 \gen_srls[20].srl_nx1 
       (.Q(s_payload_d[20]),
        .aclk(aclk),
        .\m_axi_araddr[20] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[20]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_120 \gen_srls[21].srl_nx1 
       (.Q(s_payload_d[21]),
        .aclk(aclk),
        .\m_axi_araddr[21] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[21]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_121 \gen_srls[22].srl_nx1 
       (.Q(s_payload_d[22]),
        .aclk(aclk),
        .\m_axi_araddr[22] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[22]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_122 \gen_srls[23].srl_nx1 
       (.Q(s_payload_d[23]),
        .aclk(aclk),
        .\m_axi_araddr[23] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[23]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_123 \gen_srls[24].srl_nx1 
       (.Q(s_payload_d[24]),
        .aclk(aclk),
        .\m_axi_araddr[24] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[24]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_124 \gen_srls[25].srl_nx1 
       (.Q(s_payload_d[25]),
        .aclk(aclk),
        .\m_axi_araddr[25] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[25]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_125 \gen_srls[26].srl_nx1 
       (.Q(s_payload_d[26]),
        .aclk(aclk),
        .\m_axi_araddr[26] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[26]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_126 \gen_srls[27].srl_nx1 
       (.Q(s_payload_d[27]),
        .aclk(aclk),
        .\m_axi_araddr[27] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[27]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_127 \gen_srls[28].srl_nx1 
       (.Q(s_payload_d[28]),
        .aclk(aclk),
        .\m_axi_araddr[28] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[28]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_128 \gen_srls[29].srl_nx1 
       (.Q(s_payload_d[29]),
        .aclk(aclk),
        .\m_axi_araddr[29] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[29]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_129 \gen_srls[2].srl_nx1 
       (.Q(s_payload_d[2]),
        .aclk(aclk),
        .\m_axi_araddr[2] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[2]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_130 \gen_srls[30].srl_nx1 
       (.Q(s_payload_d[30]),
        .aclk(aclk),
        .\m_axi_araddr[30] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[30]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_131 \gen_srls[31].srl_nx1 
       (.Q(s_payload_d[31]),
        .aclk(aclk),
        .\m_axi_araddr[31] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[31]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_132 \gen_srls[32].srl_nx1 
       (.Q(s_payload_d[32]),
        .aclk(aclk),
        .\m_axi_arprot[0] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[32]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_133 \gen_srls[33].srl_nx1 
       (.Q(s_payload_d[33]),
        .aclk(aclk),
        .\m_axi_arprot[1] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[33]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_134 \gen_srls[34].srl_nx1 
       (.Q(s_payload_d[34]),
        .aclk(aclk),
        .\m_axi_arprot[2] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[34]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_135 \gen_srls[3].srl_nx1 
       (.Q(s_payload_d[3]),
        .aclk(aclk),
        .\m_axi_araddr[3] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[3]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_136 \gen_srls[4].srl_nx1 
       (.Q(s_payload_d[4]),
        .aclk(aclk),
        .\m_axi_araddr[4] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[4]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_137 \gen_srls[5].srl_nx1 
       (.Q(s_payload_d[5]),
        .aclk(aclk),
        .\m_axi_araddr[5] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[5]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_138 \gen_srls[6].srl_nx1 
       (.Q(s_payload_d[6]),
        .aclk(aclk),
        .\m_axi_araddr[6] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[6]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_139 \gen_srls[7].srl_nx1 
       (.Q(s_payload_d[7]),
        .aclk(aclk),
        .\m_axi_araddr[7] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[7]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_140 \gen_srls[8].srl_nx1 
       (.Q(s_payload_d[8]),
        .aclk(aclk),
        .\m_axi_araddr[8] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[8]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_141 \gen_srls[9].srl_nx1 
       (.Q(s_payload_d[9]),
        .aclk(aclk),
        .\m_axi_araddr[9] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[9]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[0]_INST_0 
       (.I0(srl_out[0]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[0]),
        .O(m_axi_araddr[0]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[10]_INST_0 
       (.I0(srl_out[10]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[10]),
        .O(m_axi_araddr[10]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[11]_INST_0 
       (.I0(srl_out[11]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[11]),
        .O(m_axi_araddr[11]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[12]_INST_0 
       (.I0(srl_out[12]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[12]),
        .O(m_axi_araddr[12]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[13]_INST_0 
       (.I0(srl_out[13]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[13]),
        .O(m_axi_araddr[13]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[14]_INST_0 
       (.I0(srl_out[14]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[14]),
        .O(m_axi_araddr[14]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[15]_INST_0 
       (.I0(srl_out[15]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[15]),
        .O(m_axi_araddr[15]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[16]_INST_0 
       (.I0(srl_out[16]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[16]),
        .O(m_axi_araddr[16]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[17]_INST_0 
       (.I0(srl_out[17]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[17]),
        .O(m_axi_araddr[17]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[18]_INST_0 
       (.I0(srl_out[18]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[18]),
        .O(m_axi_araddr[18]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[19]_INST_0 
       (.I0(srl_out[19]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[19]),
        .O(m_axi_araddr[19]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[1]_INST_0 
       (.I0(srl_out[1]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[1]),
        .O(m_axi_araddr[1]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[20]_INST_0 
       (.I0(srl_out[20]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[20]),
        .O(m_axi_araddr[20]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[21]_INST_0 
       (.I0(srl_out[21]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[21]),
        .O(m_axi_araddr[21]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[22]_INST_0 
       (.I0(srl_out[22]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[22]),
        .O(m_axi_araddr[22]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[23]_INST_0 
       (.I0(srl_out[23]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[23]),
        .O(m_axi_araddr[23]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[24]_INST_0 
       (.I0(srl_out[24]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[24]),
        .O(m_axi_araddr[24]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[25]_INST_0 
       (.I0(srl_out[25]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[25]),
        .O(m_axi_araddr[25]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[26]_INST_0 
       (.I0(srl_out[26]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[26]),
        .O(m_axi_araddr[26]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[27]_INST_0 
       (.I0(srl_out[27]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[27]),
        .O(m_axi_araddr[27]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[28]_INST_0 
       (.I0(srl_out[28]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[28]),
        .O(m_axi_araddr[28]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[29]_INST_0 
       (.I0(srl_out[29]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[29]),
        .O(m_axi_araddr[29]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[2]_INST_0 
       (.I0(srl_out[2]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[2]),
        .O(m_axi_araddr[2]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[30]_INST_0 
       (.I0(srl_out[30]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[30]),
        .O(m_axi_araddr[30]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[31]_INST_0 
       (.I0(srl_out[31]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[31]),
        .O(m_axi_araddr[31]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[3]_INST_0 
       (.I0(srl_out[3]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[3]),
        .O(m_axi_araddr[3]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[4]_INST_0 
       (.I0(srl_out[4]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[4]),
        .O(m_axi_araddr[4]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[5]_INST_0 
       (.I0(srl_out[5]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[5]),
        .O(m_axi_araddr[5]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[6]_INST_0 
       (.I0(srl_out[6]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[6]),
        .O(m_axi_araddr[6]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[7]_INST_0 
       (.I0(srl_out[7]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[7]),
        .O(m_axi_araddr[7]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[8]_INST_0 
       (.I0(srl_out[8]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[8]),
        .O(m_axi_araddr[8]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_araddr[9]_INST_0 
       (.I0(srl_out[9]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[9]),
        .O(m_axi_araddr[9]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_arprot[0]_INST_0 
       (.I0(srl_out[32]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[32]),
        .O(m_axi_arprot[0]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_arprot[1]_INST_0 
       (.I0(srl_out[33]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[33]),
        .O(m_axi_arprot[1]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_arprot[2]_INST_0 
       (.I0(srl_out[34]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[34]),
        .O(m_axi_arprot[2]));
  (* SOFT_HLUTNM = "soft_lutpair0" *) 
  LUT5 #(
    .INIT(32'h80000FFF)) 
    m_axi_arvalid_INST_0
       (.I0(s_ready_d),
        .I1(s_valid_d),
        .I2(fifoaddr[0]),
        .I3(fifoaddr[1]),
        .I4(fifoaddr[2]),
        .O(m_axi_arvalid));
  FDRE \s_payload_d_reg[0] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[0]),
        .Q(s_payload_d[0]),
        .R(1'b0));
  FDRE \s_payload_d_reg[10] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[10]),
        .Q(s_payload_d[10]),
        .R(1'b0));
  FDRE \s_payload_d_reg[11] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[11]),
        .Q(s_payload_d[11]),
        .R(1'b0));
  FDRE \s_payload_d_reg[12] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[12]),
        .Q(s_payload_d[12]),
        .R(1'b0));
  FDRE \s_payload_d_reg[13] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[13]),
        .Q(s_payload_d[13]),
        .R(1'b0));
  FDRE \s_payload_d_reg[14] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[14]),
        .Q(s_payload_d[14]),
        .R(1'b0));
  FDRE \s_payload_d_reg[15] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[15]),
        .Q(s_payload_d[15]),
        .R(1'b0));
  FDRE \s_payload_d_reg[16] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[16]),
        .Q(s_payload_d[16]),
        .R(1'b0));
  FDRE \s_payload_d_reg[17] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[17]),
        .Q(s_payload_d[17]),
        .R(1'b0));
  FDRE \s_payload_d_reg[18] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[18]),
        .Q(s_payload_d[18]),
        .R(1'b0));
  FDRE \s_payload_d_reg[19] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[19]),
        .Q(s_payload_d[19]),
        .R(1'b0));
  FDRE \s_payload_d_reg[1] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[1]),
        .Q(s_payload_d[1]),
        .R(1'b0));
  FDRE \s_payload_d_reg[20] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[20]),
        .Q(s_payload_d[20]),
        .R(1'b0));
  FDRE \s_payload_d_reg[21] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[21]),
        .Q(s_payload_d[21]),
        .R(1'b0));
  FDRE \s_payload_d_reg[22] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[22]),
        .Q(s_payload_d[22]),
        .R(1'b0));
  FDRE \s_payload_d_reg[23] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[23]),
        .Q(s_payload_d[23]),
        .R(1'b0));
  FDRE \s_payload_d_reg[24] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[24]),
        .Q(s_payload_d[24]),
        .R(1'b0));
  FDRE \s_payload_d_reg[25] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[25]),
        .Q(s_payload_d[25]),
        .R(1'b0));
  FDRE \s_payload_d_reg[26] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[26]),
        .Q(s_payload_d[26]),
        .R(1'b0));
  FDRE \s_payload_d_reg[27] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[27]),
        .Q(s_payload_d[27]),
        .R(1'b0));
  FDRE \s_payload_d_reg[28] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[28]),
        .Q(s_payload_d[28]),
        .R(1'b0));
  FDRE \s_payload_d_reg[29] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[29]),
        .Q(s_payload_d[29]),
        .R(1'b0));
  FDRE \s_payload_d_reg[2] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[2]),
        .Q(s_payload_d[2]),
        .R(1'b0));
  FDRE \s_payload_d_reg[30] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[30]),
        .Q(s_payload_d[30]),
        .R(1'b0));
  FDRE \s_payload_d_reg[31] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[31]),
        .Q(s_payload_d[31]),
        .R(1'b0));
  FDRE \s_payload_d_reg[32] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[32]),
        .Q(s_payload_d[32]),
        .R(1'b0));
  FDRE \s_payload_d_reg[33] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[33]),
        .Q(s_payload_d[33]),
        .R(1'b0));
  FDRE \s_payload_d_reg[34] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[34]),
        .Q(s_payload_d[34]),
        .R(1'b0));
  FDRE \s_payload_d_reg[3] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[3]),
        .Q(s_payload_d[3]),
        .R(1'b0));
  FDRE \s_payload_d_reg[4] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[4]),
        .Q(s_payload_d[4]),
        .R(1'b0));
  FDRE \s_payload_d_reg[5] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[5]),
        .Q(s_payload_d[5]),
        .R(1'b0));
  FDRE \s_payload_d_reg[6] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[6]),
        .Q(s_payload_d[6]),
        .R(1'b0));
  FDRE \s_payload_d_reg[7] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[7]),
        .Q(s_payload_d[7]),
        .R(1'b0));
  FDRE \s_payload_d_reg[8] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[8]),
        .Q(s_payload_d[8]),
        .R(1'b0));
  FDRE \s_payload_d_reg[9] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[9]),
        .Q(s_payload_d[9]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    s_ready_d_reg
       (.C(aclk),
        .CE(1'b1),
        .D(s_axi_arready),
        .Q(s_ready_d),
        .R(SR));
  LUT6 #(
    .INIT(64'h8810981198119811)) 
    s_ready_reg_i_1
       (.I0(fifoaddr[1]),
        .I1(fifoaddr[2]),
        .I2(m_axi_arready),
        .I3(fifoaddr[0]),
        .I4(s_valid_d),
        .I5(s_ready_d),
        .O(s_ready_i));
  FDRE #(
    .INIT(1'b0)) 
    s_ready_reg_reg
       (.C(aclk),
        .CE(1'b1),
        .D(s_ready_i),
        .Q(s_axi_arready),
        .R(SR));
  FDRE s_valid_d_reg
       (.C(aclk),
        .CE(1'b1),
        .D(s_axi_arvalid),
        .Q(s_valid_d),
        .R(1'b0));
  LUT6 #(
    .INIT(64'h7FCF3FC03FC03FC0)) 
    \shift_reg_reg[0]_srl4_i_1 
       (.I0(m_axi_arready),
        .I1(fifoaddr[0]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[2]),
        .I4(s_ready_d),
        .I5(s_valid_d),
        .O(push));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_axic_register_slice" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_axic_register_slice_0
   (SR,
    s_axi_awready,
    m_axi_awvalid,
    m_axi_awaddr,
    m_axi_awprot,
    aclk,
    s_axi_awvalid,
    aresetn,
    D,
    m_axi_awready);
  output [0:0]SR;
  output s_axi_awready;
  output m_axi_awvalid;
  output [31:0]m_axi_awaddr;
  output [2:0]m_axi_awprot;
  input aclk;
  input s_axi_awvalid;
  input aresetn;
  input [34:0]D;
  input m_axi_awready;

  wire [34:0]D;
  wire [0:0]SR;
  wire aclk;
  wire aresetn;
  wire [2:0]fifoaddr;
  wire \fifoaddr[0]_i_1_n_0 ;
  wire \fifoaddr[1]_i_1_n_0 ;
  wire \fifoaddr[2]_i_1_n_0 ;
  wire \fifoaddr[2]_i_2_n_0 ;
  wire [31:0]m_axi_awaddr;
  wire [2:0]m_axi_awprot;
  wire m_axi_awready;
  wire m_axi_awvalid;
  wire push;
  wire reset;
  wire s_axi_awready;
  wire s_axi_awvalid;
  wire [34:0]s_payload_d;
  wire s_ready_d;
  wire s_ready_i;
  wire s_valid_d;
  wire [34:0]srl_out;

  LUT1 #(
    .INIT(2'h1)) 
    areset_d_i_1
       (.I0(aresetn),
        .O(reset));
  FDRE #(
    .INIT(1'b0)) 
    areset_d_reg
       (.C(aclk),
        .CE(1'b1),
        .D(reset),
        .Q(SR),
        .R(1'b0));
  (* SOFT_HLUTNM = "soft_lutpair1" *) 
  LUT1 #(
    .INIT(2'h1)) 
    \fifoaddr[0]_i_1 
       (.I0(fifoaddr[0]),
        .O(\fifoaddr[0]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'hFF00AA557F40AA15)) 
    \fifoaddr[1]_i_1 
       (.I0(fifoaddr[1]),
        .I1(s_valid_d),
        .I2(s_ready_d),
        .I3(fifoaddr[2]),
        .I4(fifoaddr[0]),
        .I5(m_axi_awready),
        .O(\fifoaddr[1]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'h7FE53FEA3FEA3FEA)) 
    \fifoaddr[2]_i_1 
       (.I0(m_axi_awready),
        .I1(fifoaddr[0]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[2]),
        .I4(s_ready_d),
        .I5(s_valid_d),
        .O(\fifoaddr[2]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'hFFAAAAFF7FAAAABF)) 
    \fifoaddr[2]_i_2 
       (.I0(fifoaddr[2]),
        .I1(s_valid_d),
        .I2(s_ready_d),
        .I3(fifoaddr[0]),
        .I4(fifoaddr[1]),
        .I5(m_axi_awready),
        .O(\fifoaddr[2]_i_2_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \fifoaddr_reg[0] 
       (.C(aclk),
        .CE(\fifoaddr[2]_i_1_n_0 ),
        .D(\fifoaddr[0]_i_1_n_0 ),
        .Q(fifoaddr[0]),
        .R(SR));
  FDSE #(
    .INIT(1'b1)) 
    \fifoaddr_reg[1] 
       (.C(aclk),
        .CE(\fifoaddr[2]_i_1_n_0 ),
        .D(\fifoaddr[1]_i_1_n_0 ),
        .Q(fifoaddr[1]),
        .S(SR));
  FDSE #(
    .INIT(1'b1)) 
    \fifoaddr_reg[2] 
       (.C(aclk),
        .CE(\fifoaddr[2]_i_1_n_0 ),
        .D(\fifoaddr[2]_i_2_n_0 ),
        .Q(fifoaddr[2]),
        .S(SR));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_72 \gen_srls[0].srl_nx1 
       (.Q(s_payload_d[0]),
        .aclk(aclk),
        .\m_axi_awaddr[0] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[0]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_73 \gen_srls[10].srl_nx1 
       (.Q(s_payload_d[10]),
        .aclk(aclk),
        .\m_axi_awaddr[10] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[10]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_74 \gen_srls[11].srl_nx1 
       (.Q(s_payload_d[11]),
        .aclk(aclk),
        .\m_axi_awaddr[11] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[11]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_75 \gen_srls[12].srl_nx1 
       (.Q(s_payload_d[12]),
        .aclk(aclk),
        .\m_axi_awaddr[12] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[12]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_76 \gen_srls[13].srl_nx1 
       (.Q(s_payload_d[13]),
        .aclk(aclk),
        .\m_axi_awaddr[13] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[13]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_77 \gen_srls[14].srl_nx1 
       (.Q(s_payload_d[14]),
        .aclk(aclk),
        .\m_axi_awaddr[14] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[14]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_78 \gen_srls[15].srl_nx1 
       (.Q(s_payload_d[15]),
        .aclk(aclk),
        .\m_axi_awaddr[15] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[15]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_79 \gen_srls[16].srl_nx1 
       (.Q(s_payload_d[16]),
        .aclk(aclk),
        .\m_axi_awaddr[16] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[16]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_80 \gen_srls[17].srl_nx1 
       (.Q(s_payload_d[17]),
        .aclk(aclk),
        .\m_axi_awaddr[17] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[17]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_81 \gen_srls[18].srl_nx1 
       (.Q(s_payload_d[18]),
        .aclk(aclk),
        .\m_axi_awaddr[18] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[18]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_82 \gen_srls[19].srl_nx1 
       (.Q(s_payload_d[19]),
        .aclk(aclk),
        .\m_axi_awaddr[19] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[19]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_83 \gen_srls[1].srl_nx1 
       (.Q(s_payload_d[1]),
        .aclk(aclk),
        .\m_axi_awaddr[1] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[1]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_84 \gen_srls[20].srl_nx1 
       (.Q(s_payload_d[20]),
        .aclk(aclk),
        .\m_axi_awaddr[20] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[20]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_85 \gen_srls[21].srl_nx1 
       (.Q(s_payload_d[21]),
        .aclk(aclk),
        .\m_axi_awaddr[21] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[21]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_86 \gen_srls[22].srl_nx1 
       (.Q(s_payload_d[22]),
        .aclk(aclk),
        .\m_axi_awaddr[22] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[22]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_87 \gen_srls[23].srl_nx1 
       (.Q(s_payload_d[23]),
        .aclk(aclk),
        .\m_axi_awaddr[23] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[23]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_88 \gen_srls[24].srl_nx1 
       (.Q(s_payload_d[24]),
        .aclk(aclk),
        .\m_axi_awaddr[24] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[24]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_89 \gen_srls[25].srl_nx1 
       (.Q(s_payload_d[25]),
        .aclk(aclk),
        .\m_axi_awaddr[25] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[25]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_90 \gen_srls[26].srl_nx1 
       (.Q(s_payload_d[26]),
        .aclk(aclk),
        .\m_axi_awaddr[26] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[26]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_91 \gen_srls[27].srl_nx1 
       (.Q(s_payload_d[27]),
        .aclk(aclk),
        .\m_axi_awaddr[27] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[27]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_92 \gen_srls[28].srl_nx1 
       (.Q(s_payload_d[28]),
        .aclk(aclk),
        .\m_axi_awaddr[28] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[28]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_93 \gen_srls[29].srl_nx1 
       (.Q(s_payload_d[29]),
        .aclk(aclk),
        .\m_axi_awaddr[29] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[29]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_94 \gen_srls[2].srl_nx1 
       (.Q(s_payload_d[2]),
        .aclk(aclk),
        .\m_axi_awaddr[2] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[2]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_95 \gen_srls[30].srl_nx1 
       (.Q(s_payload_d[30]),
        .aclk(aclk),
        .\m_axi_awaddr[30] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[30]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_96 \gen_srls[31].srl_nx1 
       (.Q(s_payload_d[31]),
        .aclk(aclk),
        .\m_axi_awaddr[31] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[31]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_97 \gen_srls[32].srl_nx1 
       (.Q(s_payload_d[32]),
        .aclk(aclk),
        .\m_axi_awprot[0] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[32]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_98 \gen_srls[33].srl_nx1 
       (.Q(s_payload_d[33]),
        .aclk(aclk),
        .\m_axi_awprot[1] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[33]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_99 \gen_srls[34].srl_nx1 
       (.Q(s_payload_d[34]),
        .aclk(aclk),
        .\m_axi_awprot[2] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[34]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_100 \gen_srls[3].srl_nx1 
       (.Q(s_payload_d[3]),
        .aclk(aclk),
        .\m_axi_awaddr[3] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[3]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_101 \gen_srls[4].srl_nx1 
       (.Q(s_payload_d[4]),
        .aclk(aclk),
        .\m_axi_awaddr[4] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[4]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_102 \gen_srls[5].srl_nx1 
       (.Q(s_payload_d[5]),
        .aclk(aclk),
        .\m_axi_awaddr[5] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[5]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_103 \gen_srls[6].srl_nx1 
       (.Q(s_payload_d[6]),
        .aclk(aclk),
        .\m_axi_awaddr[6] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[6]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_104 \gen_srls[7].srl_nx1 
       (.Q(s_payload_d[7]),
        .aclk(aclk),
        .\m_axi_awaddr[7] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[7]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_105 \gen_srls[8].srl_nx1 
       (.Q(s_payload_d[8]),
        .aclk(aclk),
        .\m_axi_awaddr[8] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[8]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_106 \gen_srls[9].srl_nx1 
       (.Q(s_payload_d[9]),
        .aclk(aclk),
        .\m_axi_awaddr[9] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[9]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[0]_INST_0 
       (.I0(srl_out[0]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[0]),
        .O(m_axi_awaddr[0]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[10]_INST_0 
       (.I0(srl_out[10]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[10]),
        .O(m_axi_awaddr[10]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[11]_INST_0 
       (.I0(srl_out[11]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[11]),
        .O(m_axi_awaddr[11]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[12]_INST_0 
       (.I0(srl_out[12]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[12]),
        .O(m_axi_awaddr[12]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[13]_INST_0 
       (.I0(srl_out[13]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[13]),
        .O(m_axi_awaddr[13]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[14]_INST_0 
       (.I0(srl_out[14]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[14]),
        .O(m_axi_awaddr[14]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[15]_INST_0 
       (.I0(srl_out[15]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[15]),
        .O(m_axi_awaddr[15]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[16]_INST_0 
       (.I0(srl_out[16]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[16]),
        .O(m_axi_awaddr[16]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[17]_INST_0 
       (.I0(srl_out[17]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[17]),
        .O(m_axi_awaddr[17]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[18]_INST_0 
       (.I0(srl_out[18]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[18]),
        .O(m_axi_awaddr[18]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[19]_INST_0 
       (.I0(srl_out[19]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[19]),
        .O(m_axi_awaddr[19]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[1]_INST_0 
       (.I0(srl_out[1]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[1]),
        .O(m_axi_awaddr[1]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[20]_INST_0 
       (.I0(srl_out[20]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[20]),
        .O(m_axi_awaddr[20]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[21]_INST_0 
       (.I0(srl_out[21]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[21]),
        .O(m_axi_awaddr[21]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[22]_INST_0 
       (.I0(srl_out[22]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[22]),
        .O(m_axi_awaddr[22]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[23]_INST_0 
       (.I0(srl_out[23]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[23]),
        .O(m_axi_awaddr[23]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[24]_INST_0 
       (.I0(srl_out[24]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[24]),
        .O(m_axi_awaddr[24]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[25]_INST_0 
       (.I0(srl_out[25]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[25]),
        .O(m_axi_awaddr[25]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[26]_INST_0 
       (.I0(srl_out[26]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[26]),
        .O(m_axi_awaddr[26]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[27]_INST_0 
       (.I0(srl_out[27]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[27]),
        .O(m_axi_awaddr[27]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[28]_INST_0 
       (.I0(srl_out[28]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[28]),
        .O(m_axi_awaddr[28]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[29]_INST_0 
       (.I0(srl_out[29]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[29]),
        .O(m_axi_awaddr[29]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[2]_INST_0 
       (.I0(srl_out[2]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[2]),
        .O(m_axi_awaddr[2]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[30]_INST_0 
       (.I0(srl_out[30]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[30]),
        .O(m_axi_awaddr[30]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[31]_INST_0 
       (.I0(srl_out[31]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[31]),
        .O(m_axi_awaddr[31]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[3]_INST_0 
       (.I0(srl_out[3]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[3]),
        .O(m_axi_awaddr[3]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[4]_INST_0 
       (.I0(srl_out[4]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[4]),
        .O(m_axi_awaddr[4]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[5]_INST_0 
       (.I0(srl_out[5]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[5]),
        .O(m_axi_awaddr[5]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[6]_INST_0 
       (.I0(srl_out[6]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[6]),
        .O(m_axi_awaddr[6]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[7]_INST_0 
       (.I0(srl_out[7]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[7]),
        .O(m_axi_awaddr[7]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[8]_INST_0 
       (.I0(srl_out[8]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[8]),
        .O(m_axi_awaddr[8]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awaddr[9]_INST_0 
       (.I0(srl_out[9]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[9]),
        .O(m_axi_awaddr[9]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awprot[0]_INST_0 
       (.I0(srl_out[32]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[32]),
        .O(m_axi_awprot[0]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awprot[1]_INST_0 
       (.I0(srl_out[33]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[33]),
        .O(m_axi_awprot[1]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_awprot[2]_INST_0 
       (.I0(srl_out[34]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[34]),
        .O(m_axi_awprot[2]));
  (* SOFT_HLUTNM = "soft_lutpair1" *) 
  LUT5 #(
    .INIT(32'h80000FFF)) 
    m_axi_awvalid_INST_0
       (.I0(s_ready_d),
        .I1(s_valid_d),
        .I2(fifoaddr[0]),
        .I3(fifoaddr[1]),
        .I4(fifoaddr[2]),
        .O(m_axi_awvalid));
  FDRE \s_payload_d_reg[0] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[0]),
        .Q(s_payload_d[0]),
        .R(1'b0));
  FDRE \s_payload_d_reg[10] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[10]),
        .Q(s_payload_d[10]),
        .R(1'b0));
  FDRE \s_payload_d_reg[11] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[11]),
        .Q(s_payload_d[11]),
        .R(1'b0));
  FDRE \s_payload_d_reg[12] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[12]),
        .Q(s_payload_d[12]),
        .R(1'b0));
  FDRE \s_payload_d_reg[13] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[13]),
        .Q(s_payload_d[13]),
        .R(1'b0));
  FDRE \s_payload_d_reg[14] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[14]),
        .Q(s_payload_d[14]),
        .R(1'b0));
  FDRE \s_payload_d_reg[15] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[15]),
        .Q(s_payload_d[15]),
        .R(1'b0));
  FDRE \s_payload_d_reg[16] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[16]),
        .Q(s_payload_d[16]),
        .R(1'b0));
  FDRE \s_payload_d_reg[17] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[17]),
        .Q(s_payload_d[17]),
        .R(1'b0));
  FDRE \s_payload_d_reg[18] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[18]),
        .Q(s_payload_d[18]),
        .R(1'b0));
  FDRE \s_payload_d_reg[19] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[19]),
        .Q(s_payload_d[19]),
        .R(1'b0));
  FDRE \s_payload_d_reg[1] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[1]),
        .Q(s_payload_d[1]),
        .R(1'b0));
  FDRE \s_payload_d_reg[20] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[20]),
        .Q(s_payload_d[20]),
        .R(1'b0));
  FDRE \s_payload_d_reg[21] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[21]),
        .Q(s_payload_d[21]),
        .R(1'b0));
  FDRE \s_payload_d_reg[22] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[22]),
        .Q(s_payload_d[22]),
        .R(1'b0));
  FDRE \s_payload_d_reg[23] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[23]),
        .Q(s_payload_d[23]),
        .R(1'b0));
  FDRE \s_payload_d_reg[24] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[24]),
        .Q(s_payload_d[24]),
        .R(1'b0));
  FDRE \s_payload_d_reg[25] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[25]),
        .Q(s_payload_d[25]),
        .R(1'b0));
  FDRE \s_payload_d_reg[26] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[26]),
        .Q(s_payload_d[26]),
        .R(1'b0));
  FDRE \s_payload_d_reg[27] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[27]),
        .Q(s_payload_d[27]),
        .R(1'b0));
  FDRE \s_payload_d_reg[28] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[28]),
        .Q(s_payload_d[28]),
        .R(1'b0));
  FDRE \s_payload_d_reg[29] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[29]),
        .Q(s_payload_d[29]),
        .R(1'b0));
  FDRE \s_payload_d_reg[2] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[2]),
        .Q(s_payload_d[2]),
        .R(1'b0));
  FDRE \s_payload_d_reg[30] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[30]),
        .Q(s_payload_d[30]),
        .R(1'b0));
  FDRE \s_payload_d_reg[31] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[31]),
        .Q(s_payload_d[31]),
        .R(1'b0));
  FDRE \s_payload_d_reg[32] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[32]),
        .Q(s_payload_d[32]),
        .R(1'b0));
  FDRE \s_payload_d_reg[33] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[33]),
        .Q(s_payload_d[33]),
        .R(1'b0));
  FDRE \s_payload_d_reg[34] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[34]),
        .Q(s_payload_d[34]),
        .R(1'b0));
  FDRE \s_payload_d_reg[3] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[3]),
        .Q(s_payload_d[3]),
        .R(1'b0));
  FDRE \s_payload_d_reg[4] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[4]),
        .Q(s_payload_d[4]),
        .R(1'b0));
  FDRE \s_payload_d_reg[5] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[5]),
        .Q(s_payload_d[5]),
        .R(1'b0));
  FDRE \s_payload_d_reg[6] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[6]),
        .Q(s_payload_d[6]),
        .R(1'b0));
  FDRE \s_payload_d_reg[7] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[7]),
        .Q(s_payload_d[7]),
        .R(1'b0));
  FDRE \s_payload_d_reg[8] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[8]),
        .Q(s_payload_d[8]),
        .R(1'b0));
  FDRE \s_payload_d_reg[9] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[9]),
        .Q(s_payload_d[9]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    s_ready_d_reg
       (.C(aclk),
        .CE(1'b1),
        .D(s_axi_awready),
        .Q(s_ready_d),
        .R(SR));
  LUT6 #(
    .INIT(64'h8810981198119811)) 
    s_ready_reg_i_1
       (.I0(fifoaddr[1]),
        .I1(fifoaddr[2]),
        .I2(m_axi_awready),
        .I3(fifoaddr[0]),
        .I4(s_valid_d),
        .I5(s_ready_d),
        .O(s_ready_i));
  FDRE #(
    .INIT(1'b0)) 
    s_ready_reg_reg
       (.C(aclk),
        .CE(1'b1),
        .D(s_ready_i),
        .Q(s_axi_awready),
        .R(SR));
  FDRE s_valid_d_reg
       (.C(aclk),
        .CE(1'b1),
        .D(s_axi_awvalid),
        .Q(s_valid_d),
        .R(1'b0));
  LUT6 #(
    .INIT(64'h7FCF3FC03FC03FC0)) 
    \shift_reg_reg[0]_srl4_i_1 
       (.I0(m_axi_awready),
        .I1(fifoaddr[0]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[2]),
        .I4(s_ready_d),
        .I5(s_valid_d),
        .O(push));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_axic_register_slice" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_axic_register_slice__parameterized0
   (s_axi_wready,
    m_axi_wvalid,
    m_axi_wdata,
    m_axi_wstrb,
    aclk,
    SR,
    s_axi_wvalid,
    D,
    m_axi_wready);
  output s_axi_wready;
  output m_axi_wvalid;
  output [31:0]m_axi_wdata;
  output [3:0]m_axi_wstrb;
  input aclk;
  input [0:0]SR;
  input s_axi_wvalid;
  input [35:0]D;
  input m_axi_wready;

  wire [35:0]D;
  wire [0:0]SR;
  wire aclk;
  wire [2:0]fifoaddr;
  wire \fifoaddr[0]_i_1__0_n_0 ;
  wire \fifoaddr[1]_i_1__0_n_0 ;
  wire \fifoaddr[2]_i_1__0_n_0 ;
  wire \fifoaddr[2]_i_2__0_n_0 ;
  wire [31:0]m_axi_wdata;
  wire m_axi_wready;
  wire [3:0]m_axi_wstrb;
  wire m_axi_wvalid;
  wire push;
  wire s_axi_wready;
  wire s_axi_wvalid;
  wire [35:0]s_payload_d;
  wire s_ready_d;
  wire s_ready_i;
  wire s_valid_d;
  wire [35:0]srl_out;

  (* SOFT_HLUTNM = "soft_lutpair4" *) 
  LUT1 #(
    .INIT(2'h1)) 
    \fifoaddr[0]_i_1__0 
       (.I0(fifoaddr[0]),
        .O(\fifoaddr[0]_i_1__0_n_0 ));
  LUT6 #(
    .INIT(64'hFF00AA557F40AA15)) 
    \fifoaddr[1]_i_1__0 
       (.I0(fifoaddr[1]),
        .I1(s_valid_d),
        .I2(s_ready_d),
        .I3(fifoaddr[2]),
        .I4(fifoaddr[0]),
        .I5(m_axi_wready),
        .O(\fifoaddr[1]_i_1__0_n_0 ));
  LUT6 #(
    .INIT(64'h7FE53FEA3FEA3FEA)) 
    \fifoaddr[2]_i_1__0 
       (.I0(m_axi_wready),
        .I1(fifoaddr[0]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[2]),
        .I4(s_ready_d),
        .I5(s_valid_d),
        .O(\fifoaddr[2]_i_1__0_n_0 ));
  LUT6 #(
    .INIT(64'hFFAAAAFF7FAAAABF)) 
    \fifoaddr[2]_i_2__0 
       (.I0(fifoaddr[2]),
        .I1(s_valid_d),
        .I2(s_ready_d),
        .I3(fifoaddr[0]),
        .I4(fifoaddr[1]),
        .I5(m_axi_wready),
        .O(\fifoaddr[2]_i_2__0_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \fifoaddr_reg[0] 
       (.C(aclk),
        .CE(\fifoaddr[2]_i_1__0_n_0 ),
        .D(\fifoaddr[0]_i_1__0_n_0 ),
        .Q(fifoaddr[0]),
        .R(SR));
  FDSE #(
    .INIT(1'b1)) 
    \fifoaddr_reg[1] 
       (.C(aclk),
        .CE(\fifoaddr[2]_i_1__0_n_0 ),
        .D(\fifoaddr[1]_i_1__0_n_0 ),
        .Q(fifoaddr[1]),
        .S(SR));
  FDSE #(
    .INIT(1'b1)) 
    \fifoaddr_reg[2] 
       (.C(aclk),
        .CE(\fifoaddr[2]_i_1__0_n_0 ),
        .D(\fifoaddr[2]_i_2__0_n_0 ),
        .Q(fifoaddr[2]),
        .S(SR));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl \gen_srls[0].srl_nx1 
       (.Q(s_payload_d[0]),
        .aclk(aclk),
        .\m_axi_wdata[0] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[0]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_1 \gen_srls[10].srl_nx1 
       (.Q(s_payload_d[10]),
        .aclk(aclk),
        .\m_axi_wdata[10] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[10]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_2 \gen_srls[11].srl_nx1 
       (.Q(s_payload_d[11]),
        .aclk(aclk),
        .\m_axi_wdata[11] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[11]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_3 \gen_srls[12].srl_nx1 
       (.Q(s_payload_d[12]),
        .aclk(aclk),
        .\m_axi_wdata[12] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[12]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_4 \gen_srls[13].srl_nx1 
       (.Q(s_payload_d[13]),
        .aclk(aclk),
        .\m_axi_wdata[13] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[13]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_5 \gen_srls[14].srl_nx1 
       (.Q(s_payload_d[14]),
        .aclk(aclk),
        .\m_axi_wdata[14] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[14]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_6 \gen_srls[15].srl_nx1 
       (.Q(s_payload_d[15]),
        .aclk(aclk),
        .\m_axi_wdata[15] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[15]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_7 \gen_srls[16].srl_nx1 
       (.Q(s_payload_d[16]),
        .aclk(aclk),
        .\m_axi_wdata[16] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[16]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_8 \gen_srls[17].srl_nx1 
       (.Q(s_payload_d[17]),
        .aclk(aclk),
        .\m_axi_wdata[17] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[17]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_9 \gen_srls[18].srl_nx1 
       (.Q(s_payload_d[18]),
        .aclk(aclk),
        .\m_axi_wdata[18] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[18]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_10 \gen_srls[19].srl_nx1 
       (.Q(s_payload_d[19]),
        .aclk(aclk),
        .\m_axi_wdata[19] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[19]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_11 \gen_srls[1].srl_nx1 
       (.Q(s_payload_d[1]),
        .aclk(aclk),
        .\m_axi_wdata[1] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[1]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_12 \gen_srls[20].srl_nx1 
       (.Q(s_payload_d[20]),
        .aclk(aclk),
        .\m_axi_wdata[20] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[20]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_13 \gen_srls[21].srl_nx1 
       (.Q(s_payload_d[21]),
        .aclk(aclk),
        .\m_axi_wdata[21] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[21]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_14 \gen_srls[22].srl_nx1 
       (.Q(s_payload_d[22]),
        .aclk(aclk),
        .\m_axi_wdata[22] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[22]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_15 \gen_srls[23].srl_nx1 
       (.Q(s_payload_d[23]),
        .aclk(aclk),
        .\m_axi_wdata[23] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[23]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_16 \gen_srls[24].srl_nx1 
       (.Q(s_payload_d[24]),
        .aclk(aclk),
        .\m_axi_wdata[24] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[24]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_17 \gen_srls[25].srl_nx1 
       (.Q(s_payload_d[25]),
        .aclk(aclk),
        .\m_axi_wdata[25] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[25]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_18 \gen_srls[26].srl_nx1 
       (.Q(s_payload_d[26]),
        .aclk(aclk),
        .\m_axi_wdata[26] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[26]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_19 \gen_srls[27].srl_nx1 
       (.Q(s_payload_d[27]),
        .aclk(aclk),
        .\m_axi_wdata[27] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[27]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_20 \gen_srls[28].srl_nx1 
       (.Q(s_payload_d[28]),
        .aclk(aclk),
        .\m_axi_wdata[28] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[28]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_21 \gen_srls[29].srl_nx1 
       (.Q(s_payload_d[29]),
        .aclk(aclk),
        .\m_axi_wdata[29] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[29]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_22 \gen_srls[2].srl_nx1 
       (.Q(s_payload_d[2]),
        .aclk(aclk),
        .\m_axi_wdata[2] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[2]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_23 \gen_srls[30].srl_nx1 
       (.Q(s_payload_d[30]),
        .aclk(aclk),
        .\m_axi_wdata[30] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[30]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_24 \gen_srls[31].srl_nx1 
       (.Q(s_payload_d[31]),
        .aclk(aclk),
        .\m_axi_wdata[31] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[31]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_25 \gen_srls[32].srl_nx1 
       (.Q(s_payload_d[32]),
        .aclk(aclk),
        .\m_axi_wstrb[0] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[32]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_26 \gen_srls[33].srl_nx1 
       (.Q(s_payload_d[33]),
        .aclk(aclk),
        .\m_axi_wstrb[1] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[33]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_27 \gen_srls[34].srl_nx1 
       (.Q(s_payload_d[34]),
        .aclk(aclk),
        .\m_axi_wstrb[2] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[34]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_28 \gen_srls[35].srl_nx1 
       (.Q(s_payload_d[35]),
        .aclk(aclk),
        .\m_axi_wstrb[3] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[35]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_29 \gen_srls[3].srl_nx1 
       (.Q(s_payload_d[3]),
        .aclk(aclk),
        .\m_axi_wdata[3] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[3]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_30 \gen_srls[4].srl_nx1 
       (.Q(s_payload_d[4]),
        .aclk(aclk),
        .\m_axi_wdata[4] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[4]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_31 \gen_srls[5].srl_nx1 
       (.Q(s_payload_d[5]),
        .aclk(aclk),
        .\m_axi_wdata[5] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[5]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_32 \gen_srls[6].srl_nx1 
       (.Q(s_payload_d[6]),
        .aclk(aclk),
        .\m_axi_wdata[6] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[6]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_33 \gen_srls[7].srl_nx1 
       (.Q(s_payload_d[7]),
        .aclk(aclk),
        .\m_axi_wdata[7] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[7]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_34 \gen_srls[8].srl_nx1 
       (.Q(s_payload_d[8]),
        .aclk(aclk),
        .\m_axi_wdata[8] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[8]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_35 \gen_srls[9].srl_nx1 
       (.Q(s_payload_d[9]),
        .aclk(aclk),
        .\m_axi_wdata[9] (fifoaddr[1:0]),
        .push(push),
        .srl_out(srl_out[9]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[0]_INST_0 
       (.I0(srl_out[0]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[0]),
        .O(m_axi_wdata[0]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[10]_INST_0 
       (.I0(srl_out[10]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[10]),
        .O(m_axi_wdata[10]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[11]_INST_0 
       (.I0(srl_out[11]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[11]),
        .O(m_axi_wdata[11]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[12]_INST_0 
       (.I0(srl_out[12]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[12]),
        .O(m_axi_wdata[12]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[13]_INST_0 
       (.I0(srl_out[13]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[13]),
        .O(m_axi_wdata[13]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[14]_INST_0 
       (.I0(srl_out[14]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[14]),
        .O(m_axi_wdata[14]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[15]_INST_0 
       (.I0(srl_out[15]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[15]),
        .O(m_axi_wdata[15]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[16]_INST_0 
       (.I0(srl_out[16]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[16]),
        .O(m_axi_wdata[16]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[17]_INST_0 
       (.I0(srl_out[17]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[17]),
        .O(m_axi_wdata[17]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[18]_INST_0 
       (.I0(srl_out[18]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[18]),
        .O(m_axi_wdata[18]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[19]_INST_0 
       (.I0(srl_out[19]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[19]),
        .O(m_axi_wdata[19]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[1]_INST_0 
       (.I0(srl_out[1]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[1]),
        .O(m_axi_wdata[1]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[20]_INST_0 
       (.I0(srl_out[20]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[20]),
        .O(m_axi_wdata[20]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[21]_INST_0 
       (.I0(srl_out[21]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[21]),
        .O(m_axi_wdata[21]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[22]_INST_0 
       (.I0(srl_out[22]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[22]),
        .O(m_axi_wdata[22]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[23]_INST_0 
       (.I0(srl_out[23]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[23]),
        .O(m_axi_wdata[23]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[24]_INST_0 
       (.I0(srl_out[24]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[24]),
        .O(m_axi_wdata[24]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[25]_INST_0 
       (.I0(srl_out[25]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[25]),
        .O(m_axi_wdata[25]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[26]_INST_0 
       (.I0(srl_out[26]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[26]),
        .O(m_axi_wdata[26]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[27]_INST_0 
       (.I0(srl_out[27]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[27]),
        .O(m_axi_wdata[27]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[28]_INST_0 
       (.I0(srl_out[28]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[28]),
        .O(m_axi_wdata[28]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[29]_INST_0 
       (.I0(srl_out[29]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[29]),
        .O(m_axi_wdata[29]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[2]_INST_0 
       (.I0(srl_out[2]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[2]),
        .O(m_axi_wdata[2]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[30]_INST_0 
       (.I0(srl_out[30]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[30]),
        .O(m_axi_wdata[30]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[31]_INST_0 
       (.I0(srl_out[31]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[31]),
        .O(m_axi_wdata[31]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[3]_INST_0 
       (.I0(srl_out[3]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[3]),
        .O(m_axi_wdata[3]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[4]_INST_0 
       (.I0(srl_out[4]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[4]),
        .O(m_axi_wdata[4]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[5]_INST_0 
       (.I0(srl_out[5]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[5]),
        .O(m_axi_wdata[5]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[6]_INST_0 
       (.I0(srl_out[6]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[6]),
        .O(m_axi_wdata[6]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[7]_INST_0 
       (.I0(srl_out[7]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[7]),
        .O(m_axi_wdata[7]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[8]_INST_0 
       (.I0(srl_out[8]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[8]),
        .O(m_axi_wdata[8]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wdata[9]_INST_0 
       (.I0(srl_out[9]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[9]),
        .O(m_axi_wdata[9]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wstrb[0]_INST_0 
       (.I0(srl_out[32]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[32]),
        .O(m_axi_wstrb[0]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wstrb[1]_INST_0 
       (.I0(srl_out[33]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[33]),
        .O(m_axi_wstrb[1]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wstrb[2]_INST_0 
       (.I0(srl_out[34]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[34]),
        .O(m_axi_wstrb[2]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \m_axi_wstrb[3]_INST_0 
       (.I0(srl_out[35]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[35]),
        .O(m_axi_wstrb[3]));
  (* SOFT_HLUTNM = "soft_lutpair4" *) 
  LUT5 #(
    .INIT(32'h80000FFF)) 
    m_axi_wvalid_INST_0
       (.I0(s_ready_d),
        .I1(s_valid_d),
        .I2(fifoaddr[0]),
        .I3(fifoaddr[1]),
        .I4(fifoaddr[2]),
        .O(m_axi_wvalid));
  FDRE \s_payload_d_reg[0] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[0]),
        .Q(s_payload_d[0]),
        .R(1'b0));
  FDRE \s_payload_d_reg[10] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[10]),
        .Q(s_payload_d[10]),
        .R(1'b0));
  FDRE \s_payload_d_reg[11] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[11]),
        .Q(s_payload_d[11]),
        .R(1'b0));
  FDRE \s_payload_d_reg[12] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[12]),
        .Q(s_payload_d[12]),
        .R(1'b0));
  FDRE \s_payload_d_reg[13] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[13]),
        .Q(s_payload_d[13]),
        .R(1'b0));
  FDRE \s_payload_d_reg[14] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[14]),
        .Q(s_payload_d[14]),
        .R(1'b0));
  FDRE \s_payload_d_reg[15] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[15]),
        .Q(s_payload_d[15]),
        .R(1'b0));
  FDRE \s_payload_d_reg[16] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[16]),
        .Q(s_payload_d[16]),
        .R(1'b0));
  FDRE \s_payload_d_reg[17] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[17]),
        .Q(s_payload_d[17]),
        .R(1'b0));
  FDRE \s_payload_d_reg[18] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[18]),
        .Q(s_payload_d[18]),
        .R(1'b0));
  FDRE \s_payload_d_reg[19] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[19]),
        .Q(s_payload_d[19]),
        .R(1'b0));
  FDRE \s_payload_d_reg[1] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[1]),
        .Q(s_payload_d[1]),
        .R(1'b0));
  FDRE \s_payload_d_reg[20] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[20]),
        .Q(s_payload_d[20]),
        .R(1'b0));
  FDRE \s_payload_d_reg[21] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[21]),
        .Q(s_payload_d[21]),
        .R(1'b0));
  FDRE \s_payload_d_reg[22] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[22]),
        .Q(s_payload_d[22]),
        .R(1'b0));
  FDRE \s_payload_d_reg[23] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[23]),
        .Q(s_payload_d[23]),
        .R(1'b0));
  FDRE \s_payload_d_reg[24] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[24]),
        .Q(s_payload_d[24]),
        .R(1'b0));
  FDRE \s_payload_d_reg[25] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[25]),
        .Q(s_payload_d[25]),
        .R(1'b0));
  FDRE \s_payload_d_reg[26] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[26]),
        .Q(s_payload_d[26]),
        .R(1'b0));
  FDRE \s_payload_d_reg[27] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[27]),
        .Q(s_payload_d[27]),
        .R(1'b0));
  FDRE \s_payload_d_reg[28] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[28]),
        .Q(s_payload_d[28]),
        .R(1'b0));
  FDRE \s_payload_d_reg[29] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[29]),
        .Q(s_payload_d[29]),
        .R(1'b0));
  FDRE \s_payload_d_reg[2] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[2]),
        .Q(s_payload_d[2]),
        .R(1'b0));
  FDRE \s_payload_d_reg[30] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[30]),
        .Q(s_payload_d[30]),
        .R(1'b0));
  FDRE \s_payload_d_reg[31] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[31]),
        .Q(s_payload_d[31]),
        .R(1'b0));
  FDRE \s_payload_d_reg[32] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[32]),
        .Q(s_payload_d[32]),
        .R(1'b0));
  FDRE \s_payload_d_reg[33] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[33]),
        .Q(s_payload_d[33]),
        .R(1'b0));
  FDRE \s_payload_d_reg[34] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[34]),
        .Q(s_payload_d[34]),
        .R(1'b0));
  FDRE \s_payload_d_reg[35] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[35]),
        .Q(s_payload_d[35]),
        .R(1'b0));
  FDRE \s_payload_d_reg[3] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[3]),
        .Q(s_payload_d[3]),
        .R(1'b0));
  FDRE \s_payload_d_reg[4] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[4]),
        .Q(s_payload_d[4]),
        .R(1'b0));
  FDRE \s_payload_d_reg[5] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[5]),
        .Q(s_payload_d[5]),
        .R(1'b0));
  FDRE \s_payload_d_reg[6] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[6]),
        .Q(s_payload_d[6]),
        .R(1'b0));
  FDRE \s_payload_d_reg[7] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[7]),
        .Q(s_payload_d[7]),
        .R(1'b0));
  FDRE \s_payload_d_reg[8] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[8]),
        .Q(s_payload_d[8]),
        .R(1'b0));
  FDRE \s_payload_d_reg[9] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[9]),
        .Q(s_payload_d[9]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    s_ready_d_reg
       (.C(aclk),
        .CE(1'b1),
        .D(s_axi_wready),
        .Q(s_ready_d),
        .R(SR));
  LUT6 #(
    .INIT(64'h8810981198119811)) 
    s_ready_reg_i_1
       (.I0(fifoaddr[1]),
        .I1(fifoaddr[2]),
        .I2(m_axi_wready),
        .I3(fifoaddr[0]),
        .I4(s_valid_d),
        .I5(s_ready_d),
        .O(s_ready_i));
  FDRE #(
    .INIT(1'b0)) 
    s_ready_reg_reg
       (.C(aclk),
        .CE(1'b1),
        .D(s_ready_i),
        .Q(s_axi_wready),
        .R(SR));
  FDRE s_valid_d_reg
       (.C(aclk),
        .CE(1'b1),
        .D(s_axi_wvalid),
        .Q(s_valid_d),
        .R(1'b0));
  LUT6 #(
    .INIT(64'h7FCF3FC03FC03FC0)) 
    \shift_reg_reg[0]_srl4_i_1 
       (.I0(m_axi_wready),
        .I1(fifoaddr[0]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[2]),
        .I4(s_ready_d),
        .I5(s_valid_d),
        .O(push));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_axic_register_slice" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_axic_register_slice__parameterized1
   (m_axi_bready,
    s_axi_bresp,
    s_axi_bvalid,
    aclk,
    SR,
    m_axi_bvalid,
    m_axi_bresp,
    s_axi_bready);
  output m_axi_bready;
  output [1:0]s_axi_bresp;
  output s_axi_bvalid;
  input aclk;
  input [0:0]SR;
  input m_axi_bvalid;
  input [1:0]m_axi_bresp;
  input s_axi_bready;

  wire [0:0]SR;
  wire aclk;
  wire [2:0]fifoaddr;
  wire \fifoaddr[0]_i_1__1_n_0 ;
  wire \fifoaddr[1]_i_1__1_n_0 ;
  wire \fifoaddr[2]_i_1__1_n_0 ;
  wire \fifoaddr[2]_i_2__1_n_0 ;
  wire m_axi_bready;
  wire [1:0]m_axi_bresp;
  wire m_axi_bvalid;
  wire push;
  wire s_axi_bready;
  wire [1:0]s_axi_bresp;
  wire s_axi_bvalid;
  wire [1:0]s_payload_d;
  wire s_ready_d;
  wire s_ready_i;
  wire s_valid_d;
  wire [1:0]srl_out;

  (* SOFT_HLUTNM = "soft_lutpair2" *) 
  LUT1 #(
    .INIT(2'h1)) 
    \fifoaddr[0]_i_1__1 
       (.I0(fifoaddr[0]),
        .O(\fifoaddr[0]_i_1__1_n_0 ));
  LUT6 #(
    .INIT(64'hFF00AA557F40AA15)) 
    \fifoaddr[1]_i_1__1 
       (.I0(fifoaddr[1]),
        .I1(s_valid_d),
        .I2(s_ready_d),
        .I3(fifoaddr[2]),
        .I4(fifoaddr[0]),
        .I5(s_axi_bready),
        .O(\fifoaddr[1]_i_1__1_n_0 ));
  LUT6 #(
    .INIT(64'h7FE53FEA3FEA3FEA)) 
    \fifoaddr[2]_i_1__1 
       (.I0(s_axi_bready),
        .I1(fifoaddr[0]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[2]),
        .I4(s_ready_d),
        .I5(s_valid_d),
        .O(\fifoaddr[2]_i_1__1_n_0 ));
  LUT6 #(
    .INIT(64'hFFAAAAFF7FAAAABF)) 
    \fifoaddr[2]_i_2__1 
       (.I0(fifoaddr[2]),
        .I1(s_valid_d),
        .I2(s_ready_d),
        .I3(fifoaddr[0]),
        .I4(fifoaddr[1]),
        .I5(s_axi_bready),
        .O(\fifoaddr[2]_i_2__1_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \fifoaddr_reg[0] 
       (.C(aclk),
        .CE(\fifoaddr[2]_i_1__1_n_0 ),
        .D(\fifoaddr[0]_i_1__1_n_0 ),
        .Q(fifoaddr[0]),
        .R(SR));
  FDSE #(
    .INIT(1'b1)) 
    \fifoaddr_reg[1] 
       (.C(aclk),
        .CE(\fifoaddr[2]_i_1__1_n_0 ),
        .D(\fifoaddr[1]_i_1__1_n_0 ),
        .Q(fifoaddr[1]),
        .S(SR));
  FDSE #(
    .INIT(1'b1)) 
    \fifoaddr_reg[2] 
       (.C(aclk),
        .CE(\fifoaddr[2]_i_1__1_n_0 ),
        .D(\fifoaddr[2]_i_2__1_n_0 ),
        .Q(fifoaddr[2]),
        .S(SR));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_70 \gen_srls[0].srl_nx1 
       (.Q(s_payload_d[0]),
        .aclk(aclk),
        .push(push),
        .\s_axi_bresp[0] (fifoaddr[1:0]),
        .srl_out(srl_out[0]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_71 \gen_srls[1].srl_nx1 
       (.Q(s_payload_d[1]),
        .aclk(aclk),
        .push(push),
        .\s_axi_bresp[1] (fifoaddr[1:0]),
        .srl_out(srl_out[1]));
  (* SOFT_HLUTNM = "soft_lutpair2" *) 
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_bresp[0]_INST_0 
       (.I0(srl_out[0]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[0]),
        .O(s_axi_bresp[0]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_bresp[1]_INST_0 
       (.I0(srl_out[1]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[1]),
        .O(s_axi_bresp[1]));
  LUT5 #(
    .INIT(32'h80000FFF)) 
    s_axi_bvalid_INST_0
       (.I0(s_ready_d),
        .I1(s_valid_d),
        .I2(fifoaddr[0]),
        .I3(fifoaddr[1]),
        .I4(fifoaddr[2]),
        .O(s_axi_bvalid));
  FDRE \s_payload_d_reg[0] 
       (.C(aclk),
        .CE(1'b1),
        .D(m_axi_bresp[0]),
        .Q(s_payload_d[0]),
        .R(1'b0));
  FDRE \s_payload_d_reg[1] 
       (.C(aclk),
        .CE(1'b1),
        .D(m_axi_bresp[1]),
        .Q(s_payload_d[1]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    s_ready_d_reg
       (.C(aclk),
        .CE(1'b1),
        .D(m_axi_bready),
        .Q(s_ready_d),
        .R(SR));
  LUT6 #(
    .INIT(64'h8810981198119811)) 
    s_ready_reg_i_1
       (.I0(fifoaddr[1]),
        .I1(fifoaddr[2]),
        .I2(s_axi_bready),
        .I3(fifoaddr[0]),
        .I4(s_valid_d),
        .I5(s_ready_d),
        .O(s_ready_i));
  FDRE #(
    .INIT(1'b0)) 
    s_ready_reg_reg
       (.C(aclk),
        .CE(1'b1),
        .D(s_ready_i),
        .Q(m_axi_bready),
        .R(SR));
  FDRE s_valid_d_reg
       (.C(aclk),
        .CE(1'b1),
        .D(m_axi_bvalid),
        .Q(s_valid_d),
        .R(1'b0));
  LUT6 #(
    .INIT(64'h7FCF3FC03FC03FC0)) 
    \shift_reg_reg[0]_srl4_i_1 
       (.I0(s_axi_bready),
        .I1(fifoaddr[0]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[2]),
        .I4(s_ready_d),
        .I5(s_valid_d),
        .O(push));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_axic_register_slice" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_axic_register_slice__parameterized2
   (m_axi_rready,
    s_axi_rvalid,
    s_axi_rdata,
    s_axi_rresp,
    aclk,
    SR,
    m_axi_rvalid,
    D,
    s_axi_rready);
  output m_axi_rready;
  output s_axi_rvalid;
  output [31:0]s_axi_rdata;
  output [1:0]s_axi_rresp;
  input aclk;
  input [0:0]SR;
  input m_axi_rvalid;
  input [33:0]D;
  input s_axi_rready;

  wire [33:0]D;
  wire [0:0]SR;
  wire aclk;
  wire [2:0]fifoaddr;
  wire \fifoaddr[0]_i_1__3_n_0 ;
  wire \fifoaddr[1]_i_1__3_n_0 ;
  wire \fifoaddr[2]_i_1__3_n_0 ;
  wire \fifoaddr[2]_i_2__3_n_0 ;
  wire m_axi_rready;
  wire m_axi_rvalid;
  wire push;
  wire [31:0]s_axi_rdata;
  wire s_axi_rready;
  wire [1:0]s_axi_rresp;
  wire s_axi_rvalid;
  wire [33:0]s_payload_d;
  wire s_ready_d;
  wire s_ready_i;
  wire s_valid_d;
  wire [33:0]srl_out;

  (* SOFT_HLUTNM = "soft_lutpair3" *) 
  LUT1 #(
    .INIT(2'h1)) 
    \fifoaddr[0]_i_1__3 
       (.I0(fifoaddr[0]),
        .O(\fifoaddr[0]_i_1__3_n_0 ));
  LUT6 #(
    .INIT(64'hFF00AA557F40AA15)) 
    \fifoaddr[1]_i_1__3 
       (.I0(fifoaddr[1]),
        .I1(s_valid_d),
        .I2(s_ready_d),
        .I3(fifoaddr[2]),
        .I4(fifoaddr[0]),
        .I5(s_axi_rready),
        .O(\fifoaddr[1]_i_1__3_n_0 ));
  LUT6 #(
    .INIT(64'h7FE53FEA3FEA3FEA)) 
    \fifoaddr[2]_i_1__3 
       (.I0(s_axi_rready),
        .I1(fifoaddr[0]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[2]),
        .I4(s_ready_d),
        .I5(s_valid_d),
        .O(\fifoaddr[2]_i_1__3_n_0 ));
  LUT6 #(
    .INIT(64'hFFAAAAFF7FAAAABF)) 
    \fifoaddr[2]_i_2__3 
       (.I0(fifoaddr[2]),
        .I1(s_valid_d),
        .I2(s_ready_d),
        .I3(fifoaddr[0]),
        .I4(fifoaddr[1]),
        .I5(s_axi_rready),
        .O(\fifoaddr[2]_i_2__3_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \fifoaddr_reg[0] 
       (.C(aclk),
        .CE(\fifoaddr[2]_i_1__3_n_0 ),
        .D(\fifoaddr[0]_i_1__3_n_0 ),
        .Q(fifoaddr[0]),
        .R(SR));
  FDSE #(
    .INIT(1'b1)) 
    \fifoaddr_reg[1] 
       (.C(aclk),
        .CE(\fifoaddr[2]_i_1__3_n_0 ),
        .D(\fifoaddr[1]_i_1__3_n_0 ),
        .Q(fifoaddr[1]),
        .S(SR));
  FDSE #(
    .INIT(1'b1)) 
    \fifoaddr_reg[2] 
       (.C(aclk),
        .CE(\fifoaddr[2]_i_1__3_n_0 ),
        .D(\fifoaddr[2]_i_2__3_n_0 ),
        .Q(fifoaddr[2]),
        .S(SR));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_36 \gen_srls[0].srl_nx1 
       (.Q(s_payload_d[0]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[0] (fifoaddr[1:0]),
        .srl_out(srl_out[0]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_37 \gen_srls[10].srl_nx1 
       (.Q(s_payload_d[10]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[10] (fifoaddr[1:0]),
        .srl_out(srl_out[10]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_38 \gen_srls[11].srl_nx1 
       (.Q(s_payload_d[11]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[11] (fifoaddr[1:0]),
        .srl_out(srl_out[11]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_39 \gen_srls[12].srl_nx1 
       (.Q(s_payload_d[12]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[12] (fifoaddr[1:0]),
        .srl_out(srl_out[12]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_40 \gen_srls[13].srl_nx1 
       (.Q(s_payload_d[13]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[13] (fifoaddr[1:0]),
        .srl_out(srl_out[13]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_41 \gen_srls[14].srl_nx1 
       (.Q(s_payload_d[14]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[14] (fifoaddr[1:0]),
        .srl_out(srl_out[14]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_42 \gen_srls[15].srl_nx1 
       (.Q(s_payload_d[15]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[15] (fifoaddr[1:0]),
        .srl_out(srl_out[15]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_43 \gen_srls[16].srl_nx1 
       (.Q(s_payload_d[16]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[16] (fifoaddr[1:0]),
        .srl_out(srl_out[16]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_44 \gen_srls[17].srl_nx1 
       (.Q(s_payload_d[17]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[17] (fifoaddr[1:0]),
        .srl_out(srl_out[17]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_45 \gen_srls[18].srl_nx1 
       (.Q(s_payload_d[18]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[18] (fifoaddr[1:0]),
        .srl_out(srl_out[18]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_46 \gen_srls[19].srl_nx1 
       (.Q(s_payload_d[19]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[19] (fifoaddr[1:0]),
        .srl_out(srl_out[19]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_47 \gen_srls[1].srl_nx1 
       (.Q(s_payload_d[1]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[1] (fifoaddr[1:0]),
        .srl_out(srl_out[1]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_48 \gen_srls[20].srl_nx1 
       (.Q(s_payload_d[20]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[20] (fifoaddr[1:0]),
        .srl_out(srl_out[20]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_49 \gen_srls[21].srl_nx1 
       (.Q(s_payload_d[21]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[21] (fifoaddr[1:0]),
        .srl_out(srl_out[21]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_50 \gen_srls[22].srl_nx1 
       (.Q(s_payload_d[22]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[22] (fifoaddr[1:0]),
        .srl_out(srl_out[22]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_51 \gen_srls[23].srl_nx1 
       (.Q(s_payload_d[23]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[23] (fifoaddr[1:0]),
        .srl_out(srl_out[23]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_52 \gen_srls[24].srl_nx1 
       (.Q(s_payload_d[24]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[24] (fifoaddr[1:0]),
        .srl_out(srl_out[24]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_53 \gen_srls[25].srl_nx1 
       (.Q(s_payload_d[25]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[25] (fifoaddr[1:0]),
        .srl_out(srl_out[25]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_54 \gen_srls[26].srl_nx1 
       (.Q(s_payload_d[26]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[26] (fifoaddr[1:0]),
        .srl_out(srl_out[26]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_55 \gen_srls[27].srl_nx1 
       (.Q(s_payload_d[27]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[27] (fifoaddr[1:0]),
        .srl_out(srl_out[27]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_56 \gen_srls[28].srl_nx1 
       (.Q(s_payload_d[28]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[28] (fifoaddr[1:0]),
        .srl_out(srl_out[28]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_57 \gen_srls[29].srl_nx1 
       (.Q(s_payload_d[29]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[29] (fifoaddr[1:0]),
        .srl_out(srl_out[29]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_58 \gen_srls[2].srl_nx1 
       (.Q(s_payload_d[2]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[2] (fifoaddr[1:0]),
        .srl_out(srl_out[2]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_59 \gen_srls[30].srl_nx1 
       (.Q(s_payload_d[30]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[30] (fifoaddr[1:0]),
        .srl_out(srl_out[30]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_60 \gen_srls[31].srl_nx1 
       (.Q(s_payload_d[31]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[31] (fifoaddr[1:0]),
        .srl_out(srl_out[31]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_61 \gen_srls[32].srl_nx1 
       (.Q(s_payload_d[32]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rresp[0] (fifoaddr[1:0]),
        .srl_out(srl_out[32]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_62 \gen_srls[33].srl_nx1 
       (.Q(s_payload_d[33]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rresp[1] (fifoaddr[1:0]),
        .srl_out(srl_out[33]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_63 \gen_srls[3].srl_nx1 
       (.Q(s_payload_d[3]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[3] (fifoaddr[1:0]),
        .srl_out(srl_out[3]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_64 \gen_srls[4].srl_nx1 
       (.Q(s_payload_d[4]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[4] (fifoaddr[1:0]),
        .srl_out(srl_out[4]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_65 \gen_srls[5].srl_nx1 
       (.Q(s_payload_d[5]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[5] (fifoaddr[1:0]),
        .srl_out(srl_out[5]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_66 \gen_srls[6].srl_nx1 
       (.Q(s_payload_d[6]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[6] (fifoaddr[1:0]),
        .srl_out(srl_out[6]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_67 \gen_srls[7].srl_nx1 
       (.Q(s_payload_d[7]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[7] (fifoaddr[1:0]),
        .srl_out(srl_out[7]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_68 \gen_srls[8].srl_nx1 
       (.Q(s_payload_d[8]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[8] (fifoaddr[1:0]),
        .srl_out(srl_out[8]));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_69 \gen_srls[9].srl_nx1 
       (.Q(s_payload_d[9]),
        .aclk(aclk),
        .push(push),
        .\s_axi_rdata[9] (fifoaddr[1:0]),
        .srl_out(srl_out[9]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[0]_INST_0 
       (.I0(srl_out[0]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[0]),
        .O(s_axi_rdata[0]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[10]_INST_0 
       (.I0(srl_out[10]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[10]),
        .O(s_axi_rdata[10]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[11]_INST_0 
       (.I0(srl_out[11]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[11]),
        .O(s_axi_rdata[11]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[12]_INST_0 
       (.I0(srl_out[12]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[12]),
        .O(s_axi_rdata[12]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[13]_INST_0 
       (.I0(srl_out[13]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[13]),
        .O(s_axi_rdata[13]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[14]_INST_0 
       (.I0(srl_out[14]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[14]),
        .O(s_axi_rdata[14]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[15]_INST_0 
       (.I0(srl_out[15]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[15]),
        .O(s_axi_rdata[15]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[16]_INST_0 
       (.I0(srl_out[16]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[16]),
        .O(s_axi_rdata[16]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[17]_INST_0 
       (.I0(srl_out[17]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[17]),
        .O(s_axi_rdata[17]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[18]_INST_0 
       (.I0(srl_out[18]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[18]),
        .O(s_axi_rdata[18]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[19]_INST_0 
       (.I0(srl_out[19]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[19]),
        .O(s_axi_rdata[19]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[1]_INST_0 
       (.I0(srl_out[1]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[1]),
        .O(s_axi_rdata[1]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[20]_INST_0 
       (.I0(srl_out[20]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[20]),
        .O(s_axi_rdata[20]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[21]_INST_0 
       (.I0(srl_out[21]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[21]),
        .O(s_axi_rdata[21]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[22]_INST_0 
       (.I0(srl_out[22]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[22]),
        .O(s_axi_rdata[22]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[23]_INST_0 
       (.I0(srl_out[23]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[23]),
        .O(s_axi_rdata[23]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[24]_INST_0 
       (.I0(srl_out[24]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[24]),
        .O(s_axi_rdata[24]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[25]_INST_0 
       (.I0(srl_out[25]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[25]),
        .O(s_axi_rdata[25]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[26]_INST_0 
       (.I0(srl_out[26]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[26]),
        .O(s_axi_rdata[26]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[27]_INST_0 
       (.I0(srl_out[27]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[27]),
        .O(s_axi_rdata[27]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[28]_INST_0 
       (.I0(srl_out[28]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[28]),
        .O(s_axi_rdata[28]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[29]_INST_0 
       (.I0(srl_out[29]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[29]),
        .O(s_axi_rdata[29]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[2]_INST_0 
       (.I0(srl_out[2]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[2]),
        .O(s_axi_rdata[2]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[30]_INST_0 
       (.I0(srl_out[30]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[30]),
        .O(s_axi_rdata[30]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[31]_INST_0 
       (.I0(srl_out[31]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[31]),
        .O(s_axi_rdata[31]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[3]_INST_0 
       (.I0(srl_out[3]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[3]),
        .O(s_axi_rdata[3]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[4]_INST_0 
       (.I0(srl_out[4]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[4]),
        .O(s_axi_rdata[4]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[5]_INST_0 
       (.I0(srl_out[5]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[5]),
        .O(s_axi_rdata[5]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[6]_INST_0 
       (.I0(srl_out[6]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[6]),
        .O(s_axi_rdata[6]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[7]_INST_0 
       (.I0(srl_out[7]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[7]),
        .O(s_axi_rdata[7]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[8]_INST_0 
       (.I0(srl_out[8]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[8]),
        .O(s_axi_rdata[8]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rdata[9]_INST_0 
       (.I0(srl_out[9]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[9]),
        .O(s_axi_rdata[9]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rresp[0]_INST_0 
       (.I0(srl_out[32]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[32]),
        .O(s_axi_rresp[0]));
  LUT5 #(
    .INIT(32'hFEEE0222)) 
    \s_axi_rresp[1]_INST_0 
       (.I0(srl_out[33]),
        .I1(fifoaddr[2]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[0]),
        .I4(s_payload_d[33]),
        .O(s_axi_rresp[1]));
  (* SOFT_HLUTNM = "soft_lutpair3" *) 
  LUT5 #(
    .INIT(32'h80000FFF)) 
    s_axi_rvalid_INST_0
       (.I0(s_ready_d),
        .I1(s_valid_d),
        .I2(fifoaddr[0]),
        .I3(fifoaddr[1]),
        .I4(fifoaddr[2]),
        .O(s_axi_rvalid));
  FDRE \s_payload_d_reg[0] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[0]),
        .Q(s_payload_d[0]),
        .R(1'b0));
  FDRE \s_payload_d_reg[10] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[10]),
        .Q(s_payload_d[10]),
        .R(1'b0));
  FDRE \s_payload_d_reg[11] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[11]),
        .Q(s_payload_d[11]),
        .R(1'b0));
  FDRE \s_payload_d_reg[12] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[12]),
        .Q(s_payload_d[12]),
        .R(1'b0));
  FDRE \s_payload_d_reg[13] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[13]),
        .Q(s_payload_d[13]),
        .R(1'b0));
  FDRE \s_payload_d_reg[14] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[14]),
        .Q(s_payload_d[14]),
        .R(1'b0));
  FDRE \s_payload_d_reg[15] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[15]),
        .Q(s_payload_d[15]),
        .R(1'b0));
  FDRE \s_payload_d_reg[16] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[16]),
        .Q(s_payload_d[16]),
        .R(1'b0));
  FDRE \s_payload_d_reg[17] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[17]),
        .Q(s_payload_d[17]),
        .R(1'b0));
  FDRE \s_payload_d_reg[18] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[18]),
        .Q(s_payload_d[18]),
        .R(1'b0));
  FDRE \s_payload_d_reg[19] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[19]),
        .Q(s_payload_d[19]),
        .R(1'b0));
  FDRE \s_payload_d_reg[1] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[1]),
        .Q(s_payload_d[1]),
        .R(1'b0));
  FDRE \s_payload_d_reg[20] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[20]),
        .Q(s_payload_d[20]),
        .R(1'b0));
  FDRE \s_payload_d_reg[21] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[21]),
        .Q(s_payload_d[21]),
        .R(1'b0));
  FDRE \s_payload_d_reg[22] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[22]),
        .Q(s_payload_d[22]),
        .R(1'b0));
  FDRE \s_payload_d_reg[23] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[23]),
        .Q(s_payload_d[23]),
        .R(1'b0));
  FDRE \s_payload_d_reg[24] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[24]),
        .Q(s_payload_d[24]),
        .R(1'b0));
  FDRE \s_payload_d_reg[25] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[25]),
        .Q(s_payload_d[25]),
        .R(1'b0));
  FDRE \s_payload_d_reg[26] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[26]),
        .Q(s_payload_d[26]),
        .R(1'b0));
  FDRE \s_payload_d_reg[27] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[27]),
        .Q(s_payload_d[27]),
        .R(1'b0));
  FDRE \s_payload_d_reg[28] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[28]),
        .Q(s_payload_d[28]),
        .R(1'b0));
  FDRE \s_payload_d_reg[29] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[29]),
        .Q(s_payload_d[29]),
        .R(1'b0));
  FDRE \s_payload_d_reg[2] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[2]),
        .Q(s_payload_d[2]),
        .R(1'b0));
  FDRE \s_payload_d_reg[30] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[30]),
        .Q(s_payload_d[30]),
        .R(1'b0));
  FDRE \s_payload_d_reg[31] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[31]),
        .Q(s_payload_d[31]),
        .R(1'b0));
  FDRE \s_payload_d_reg[32] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[32]),
        .Q(s_payload_d[32]),
        .R(1'b0));
  FDRE \s_payload_d_reg[33] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[33]),
        .Q(s_payload_d[33]),
        .R(1'b0));
  FDRE \s_payload_d_reg[3] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[3]),
        .Q(s_payload_d[3]),
        .R(1'b0));
  FDRE \s_payload_d_reg[4] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[4]),
        .Q(s_payload_d[4]),
        .R(1'b0));
  FDRE \s_payload_d_reg[5] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[5]),
        .Q(s_payload_d[5]),
        .R(1'b0));
  FDRE \s_payload_d_reg[6] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[6]),
        .Q(s_payload_d[6]),
        .R(1'b0));
  FDRE \s_payload_d_reg[7] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[7]),
        .Q(s_payload_d[7]),
        .R(1'b0));
  FDRE \s_payload_d_reg[8] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[8]),
        .Q(s_payload_d[8]),
        .R(1'b0));
  FDRE \s_payload_d_reg[9] 
       (.C(aclk),
        .CE(1'b1),
        .D(D[9]),
        .Q(s_payload_d[9]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    s_ready_d_reg
       (.C(aclk),
        .CE(1'b1),
        .D(m_axi_rready),
        .Q(s_ready_d),
        .R(SR));
  LUT6 #(
    .INIT(64'h8810981198119811)) 
    s_ready_reg_i_1
       (.I0(fifoaddr[1]),
        .I1(fifoaddr[2]),
        .I2(s_axi_rready),
        .I3(fifoaddr[0]),
        .I4(s_valid_d),
        .I5(s_ready_d),
        .O(s_ready_i));
  FDRE #(
    .INIT(1'b0)) 
    s_ready_reg_reg
       (.C(aclk),
        .CE(1'b1),
        .D(s_ready_i),
        .Q(m_axi_rready),
        .R(SR));
  FDRE s_valid_d_reg
       (.C(aclk),
        .CE(1'b1),
        .D(m_axi_rvalid),
        .Q(s_valid_d),
        .R(1'b0));
  LUT6 #(
    .INIT(64'h7FCF3FC03FC03FC0)) 
    \shift_reg_reg[0]_srl4_i_1 
       (.I0(s_axi_rready),
        .I1(fifoaddr[0]),
        .I2(fifoaddr[1]),
        .I3(fifoaddr[2]),
        .I4(s_ready_d),
        .I5(s_valid_d),
        .O(push));
endmodule

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl
   (srl_out,
    push,
    Q,
    \m_axi_wdata[0] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[0] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[0] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[0].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[0].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[0] [0]),
        .A1(\m_axi_wdata[0] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_1
   (srl_out,
    push,
    Q,
    \m_axi_wdata[10] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[10] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[10] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[10].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[10].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[10] [0]),
        .A1(\m_axi_wdata[10] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_10
   (srl_out,
    push,
    Q,
    \m_axi_wdata[19] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[19] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[19] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[19].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[19].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[19] [0]),
        .A1(\m_axi_wdata[19] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_100
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[3] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[3] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[3] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[3].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[3].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[3] [0]),
        .A1(\m_axi_awaddr[3] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_101
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[4] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[4] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[4] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[4].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[4].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[4] [0]),
        .A1(\m_axi_awaddr[4] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_102
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[5] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[5] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[5] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[5].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[5].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[5] [0]),
        .A1(\m_axi_awaddr[5] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_103
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[6] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[6] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[6] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[6].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[6].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[6] [0]),
        .A1(\m_axi_awaddr[6] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_104
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[7] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[7] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[7] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[7].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[7].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[7] [0]),
        .A1(\m_axi_awaddr[7] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_105
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[8] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[8] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[8] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[8].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[8].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[8] [0]),
        .A1(\m_axi_awaddr[8] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_106
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[9] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[9] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[9] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[9].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[9].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[9] [0]),
        .A1(\m_axi_awaddr[9] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_107
   (srl_out,
    push,
    Q,
    \m_axi_araddr[0] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[0] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[0] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[0].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[0].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[0] [0]),
        .A1(\m_axi_araddr[0] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_108
   (srl_out,
    push,
    Q,
    \m_axi_araddr[10] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[10] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[10] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[10].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[10].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[10] [0]),
        .A1(\m_axi_araddr[10] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_109
   (srl_out,
    push,
    Q,
    \m_axi_araddr[11] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[11] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[11] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[11].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[11].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[11] [0]),
        .A1(\m_axi_araddr[11] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_11
   (srl_out,
    push,
    Q,
    \m_axi_wdata[1] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[1] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[1] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[1].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[1].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[1] [0]),
        .A1(\m_axi_wdata[1] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_110
   (srl_out,
    push,
    Q,
    \m_axi_araddr[12] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[12] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[12] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[12].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[12].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[12] [0]),
        .A1(\m_axi_araddr[12] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_111
   (srl_out,
    push,
    Q,
    \m_axi_araddr[13] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[13] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[13] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[13].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[13].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[13] [0]),
        .A1(\m_axi_araddr[13] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_112
   (srl_out,
    push,
    Q,
    \m_axi_araddr[14] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[14] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[14] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[14].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[14].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[14] [0]),
        .A1(\m_axi_araddr[14] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_113
   (srl_out,
    push,
    Q,
    \m_axi_araddr[15] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[15] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[15] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[15].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[15].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[15] [0]),
        .A1(\m_axi_araddr[15] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_114
   (srl_out,
    push,
    Q,
    \m_axi_araddr[16] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[16] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[16] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[16].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[16].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[16] [0]),
        .A1(\m_axi_araddr[16] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_115
   (srl_out,
    push,
    Q,
    \m_axi_araddr[17] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[17] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[17] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[17].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[17].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[17] [0]),
        .A1(\m_axi_araddr[17] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_116
   (srl_out,
    push,
    Q,
    \m_axi_araddr[18] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[18] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[18] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[18].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[18].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[18] [0]),
        .A1(\m_axi_araddr[18] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_117
   (srl_out,
    push,
    Q,
    \m_axi_araddr[19] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[19] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[19] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[19].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[19].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[19] [0]),
        .A1(\m_axi_araddr[19] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_118
   (srl_out,
    push,
    Q,
    \m_axi_araddr[1] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[1] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[1] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[1].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[1].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[1] [0]),
        .A1(\m_axi_araddr[1] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_119
   (srl_out,
    push,
    Q,
    \m_axi_araddr[20] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[20] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[20] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[20].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[20].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[20] [0]),
        .A1(\m_axi_araddr[20] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_12
   (srl_out,
    push,
    Q,
    \m_axi_wdata[20] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[20] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[20] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[20].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[20].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[20] [0]),
        .A1(\m_axi_wdata[20] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_120
   (srl_out,
    push,
    Q,
    \m_axi_araddr[21] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[21] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[21] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[21].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[21].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[21] [0]),
        .A1(\m_axi_araddr[21] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_121
   (srl_out,
    push,
    Q,
    \m_axi_araddr[22] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[22] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[22] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[22].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[22].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[22] [0]),
        .A1(\m_axi_araddr[22] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_122
   (srl_out,
    push,
    Q,
    \m_axi_araddr[23] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[23] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[23] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[23].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[23].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[23] [0]),
        .A1(\m_axi_araddr[23] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_123
   (srl_out,
    push,
    Q,
    \m_axi_araddr[24] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[24] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[24] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[24].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[24].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[24] [0]),
        .A1(\m_axi_araddr[24] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_124
   (srl_out,
    push,
    Q,
    \m_axi_araddr[25] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[25] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[25] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[25].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[25].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[25] [0]),
        .A1(\m_axi_araddr[25] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_125
   (srl_out,
    push,
    Q,
    \m_axi_araddr[26] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[26] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[26] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[26].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[26].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[26] [0]),
        .A1(\m_axi_araddr[26] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_126
   (srl_out,
    push,
    Q,
    \m_axi_araddr[27] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[27] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[27] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[27].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[27].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[27] [0]),
        .A1(\m_axi_araddr[27] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_127
   (srl_out,
    push,
    Q,
    \m_axi_araddr[28] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[28] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[28] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[28].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[28].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[28] [0]),
        .A1(\m_axi_araddr[28] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_128
   (srl_out,
    push,
    Q,
    \m_axi_araddr[29] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[29] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[29] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[29].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[29].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[29] [0]),
        .A1(\m_axi_araddr[29] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_129
   (srl_out,
    push,
    Q,
    \m_axi_araddr[2] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[2] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[2] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[2].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[2].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[2] [0]),
        .A1(\m_axi_araddr[2] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_13
   (srl_out,
    push,
    Q,
    \m_axi_wdata[21] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[21] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[21] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[21].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[21].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[21] [0]),
        .A1(\m_axi_wdata[21] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_130
   (srl_out,
    push,
    Q,
    \m_axi_araddr[30] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[30] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[30] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[30].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[30].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[30] [0]),
        .A1(\m_axi_araddr[30] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_131
   (srl_out,
    push,
    Q,
    \m_axi_araddr[31] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[31] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[31] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[31].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[31].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[31] [0]),
        .A1(\m_axi_araddr[31] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_132
   (srl_out,
    push,
    Q,
    \m_axi_arprot[0] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_arprot[0] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_arprot[0] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[32].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[32].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_arprot[0] [0]),
        .A1(\m_axi_arprot[0] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_133
   (srl_out,
    push,
    Q,
    \m_axi_arprot[1] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_arprot[1] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_arprot[1] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[33].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[33].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_arprot[1] [0]),
        .A1(\m_axi_arprot[1] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_134
   (srl_out,
    push,
    Q,
    \m_axi_arprot[2] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_arprot[2] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_arprot[2] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[34].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[34].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_arprot[2] [0]),
        .A1(\m_axi_arprot[2] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_135
   (srl_out,
    push,
    Q,
    \m_axi_araddr[3] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[3] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[3] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[3].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[3].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[3] [0]),
        .A1(\m_axi_araddr[3] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_136
   (srl_out,
    push,
    Q,
    \m_axi_araddr[4] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[4] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[4] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[4].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[4].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[4] [0]),
        .A1(\m_axi_araddr[4] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_137
   (srl_out,
    push,
    Q,
    \m_axi_araddr[5] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[5] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[5] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[5].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[5].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[5] [0]),
        .A1(\m_axi_araddr[5] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_138
   (srl_out,
    push,
    Q,
    \m_axi_araddr[6] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[6] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[6] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[6].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[6].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[6] [0]),
        .A1(\m_axi_araddr[6] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_139
   (srl_out,
    push,
    Q,
    \m_axi_araddr[7] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[7] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[7] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[7].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[7].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[7] [0]),
        .A1(\m_axi_araddr[7] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_14
   (srl_out,
    push,
    Q,
    \m_axi_wdata[22] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[22] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[22] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[22].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[22].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[22] [0]),
        .A1(\m_axi_wdata[22] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_140
   (srl_out,
    push,
    Q,
    \m_axi_araddr[8] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[8] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[8] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[8].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[8].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[8] [0]),
        .A1(\m_axi_araddr[8] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_141
   (srl_out,
    push,
    Q,
    \m_axi_araddr[9] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_araddr[9] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_araddr[9] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\ar.ar_pipe/gen_srls[9].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\ar.ar_pipe/gen_srls[9].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_araddr[9] [0]),
        .A1(\m_axi_araddr[9] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_15
   (srl_out,
    push,
    Q,
    \m_axi_wdata[23] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[23] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[23] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[23].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[23].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[23] [0]),
        .A1(\m_axi_wdata[23] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_16
   (srl_out,
    push,
    Q,
    \m_axi_wdata[24] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[24] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[24] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[24].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[24].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[24] [0]),
        .A1(\m_axi_wdata[24] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_17
   (srl_out,
    push,
    Q,
    \m_axi_wdata[25] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[25] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[25] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[25].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[25].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[25] [0]),
        .A1(\m_axi_wdata[25] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_18
   (srl_out,
    push,
    Q,
    \m_axi_wdata[26] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[26] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[26] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[26].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[26].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[26] [0]),
        .A1(\m_axi_wdata[26] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_19
   (srl_out,
    push,
    Q,
    \m_axi_wdata[27] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[27] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[27] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[27].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[27].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[27] [0]),
        .A1(\m_axi_wdata[27] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_2
   (srl_out,
    push,
    Q,
    \m_axi_wdata[11] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[11] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[11] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[11].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[11].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[11] [0]),
        .A1(\m_axi_wdata[11] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_20
   (srl_out,
    push,
    Q,
    \m_axi_wdata[28] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[28] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[28] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[28].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[28].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[28] [0]),
        .A1(\m_axi_wdata[28] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_21
   (srl_out,
    push,
    Q,
    \m_axi_wdata[29] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[29] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[29] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[29].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[29].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[29] [0]),
        .A1(\m_axi_wdata[29] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_22
   (srl_out,
    push,
    Q,
    \m_axi_wdata[2] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[2] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[2] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[2].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[2].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[2] [0]),
        .A1(\m_axi_wdata[2] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_23
   (srl_out,
    push,
    Q,
    \m_axi_wdata[30] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[30] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[30] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[30].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[30].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[30] [0]),
        .A1(\m_axi_wdata[30] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_24
   (srl_out,
    push,
    Q,
    \m_axi_wdata[31] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[31] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[31] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[31].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[31].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[31] [0]),
        .A1(\m_axi_wdata[31] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_25
   (srl_out,
    push,
    Q,
    \m_axi_wstrb[0] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wstrb[0] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wstrb[0] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[32].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[32].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wstrb[0] [0]),
        .A1(\m_axi_wstrb[0] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_26
   (srl_out,
    push,
    Q,
    \m_axi_wstrb[1] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wstrb[1] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wstrb[1] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[33].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[33].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wstrb[1] [0]),
        .A1(\m_axi_wstrb[1] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_27
   (srl_out,
    push,
    Q,
    \m_axi_wstrb[2] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wstrb[2] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wstrb[2] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[34].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[34].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wstrb[2] [0]),
        .A1(\m_axi_wstrb[2] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_28
   (srl_out,
    push,
    Q,
    \m_axi_wstrb[3] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wstrb[3] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wstrb[3] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[35].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[35].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wstrb[3] [0]),
        .A1(\m_axi_wstrb[3] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_29
   (srl_out,
    push,
    Q,
    \m_axi_wdata[3] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[3] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[3] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[3].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[3].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[3] [0]),
        .A1(\m_axi_wdata[3] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_3
   (srl_out,
    push,
    Q,
    \m_axi_wdata[12] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[12] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[12] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[12].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[12].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[12] [0]),
        .A1(\m_axi_wdata[12] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_30
   (srl_out,
    push,
    Q,
    \m_axi_wdata[4] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[4] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[4] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[4].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[4].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[4] [0]),
        .A1(\m_axi_wdata[4] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_31
   (srl_out,
    push,
    Q,
    \m_axi_wdata[5] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[5] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[5] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[5].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[5].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[5] [0]),
        .A1(\m_axi_wdata[5] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_32
   (srl_out,
    push,
    Q,
    \m_axi_wdata[6] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[6] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[6] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[6].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[6].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[6] [0]),
        .A1(\m_axi_wdata[6] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_33
   (srl_out,
    push,
    Q,
    \m_axi_wdata[7] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[7] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[7] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[7].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[7].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[7] [0]),
        .A1(\m_axi_wdata[7] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_34
   (srl_out,
    push,
    Q,
    \m_axi_wdata[8] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[8] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[8] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[8].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[8].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[8] [0]),
        .A1(\m_axi_wdata[8] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_35
   (srl_out,
    push,
    Q,
    \m_axi_wdata[9] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[9] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[9] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[9].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[9].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[9] [0]),
        .A1(\m_axi_wdata[9] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_36
   (srl_out,
    push,
    Q,
    \s_axi_rdata[0] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[0] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[0] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[0].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[0].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[0] [0]),
        .A1(\s_axi_rdata[0] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_37
   (srl_out,
    push,
    Q,
    \s_axi_rdata[10] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[10] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[10] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[10].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[10].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[10] [0]),
        .A1(\s_axi_rdata[10] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_38
   (srl_out,
    push,
    Q,
    \s_axi_rdata[11] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[11] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[11] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[11].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[11].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[11] [0]),
        .A1(\s_axi_rdata[11] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_39
   (srl_out,
    push,
    Q,
    \s_axi_rdata[12] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[12] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[12] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[12].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[12].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[12] [0]),
        .A1(\s_axi_rdata[12] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_4
   (srl_out,
    push,
    Q,
    \m_axi_wdata[13] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[13] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[13] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[13].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[13].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[13] [0]),
        .A1(\m_axi_wdata[13] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_40
   (srl_out,
    push,
    Q,
    \s_axi_rdata[13] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[13] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[13] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[13].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[13].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[13] [0]),
        .A1(\s_axi_rdata[13] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_41
   (srl_out,
    push,
    Q,
    \s_axi_rdata[14] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[14] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[14] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[14].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[14].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[14] [0]),
        .A1(\s_axi_rdata[14] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_42
   (srl_out,
    push,
    Q,
    \s_axi_rdata[15] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[15] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[15] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[15].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[15].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[15] [0]),
        .A1(\s_axi_rdata[15] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_43
   (srl_out,
    push,
    Q,
    \s_axi_rdata[16] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[16] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[16] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[16].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[16].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[16] [0]),
        .A1(\s_axi_rdata[16] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_44
   (srl_out,
    push,
    Q,
    \s_axi_rdata[17] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[17] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[17] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[17].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[17].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[17] [0]),
        .A1(\s_axi_rdata[17] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_45
   (srl_out,
    push,
    Q,
    \s_axi_rdata[18] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[18] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[18] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[18].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[18].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[18] [0]),
        .A1(\s_axi_rdata[18] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_46
   (srl_out,
    push,
    Q,
    \s_axi_rdata[19] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[19] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[19] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[19].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[19].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[19] [0]),
        .A1(\s_axi_rdata[19] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_47
   (srl_out,
    push,
    Q,
    \s_axi_rdata[1] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[1] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[1] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[1].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[1].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[1] [0]),
        .A1(\s_axi_rdata[1] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_48
   (srl_out,
    push,
    Q,
    \s_axi_rdata[20] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[20] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[20] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[20].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[20].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[20] [0]),
        .A1(\s_axi_rdata[20] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_49
   (srl_out,
    push,
    Q,
    \s_axi_rdata[21] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[21] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[21] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[21].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[21].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[21] [0]),
        .A1(\s_axi_rdata[21] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_5
   (srl_out,
    push,
    Q,
    \m_axi_wdata[14] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[14] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[14] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[14].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[14].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[14] [0]),
        .A1(\m_axi_wdata[14] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_50
   (srl_out,
    push,
    Q,
    \s_axi_rdata[22] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[22] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[22] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[22].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[22].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[22] [0]),
        .A1(\s_axi_rdata[22] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_51
   (srl_out,
    push,
    Q,
    \s_axi_rdata[23] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[23] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[23] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[23].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[23].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[23] [0]),
        .A1(\s_axi_rdata[23] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_52
   (srl_out,
    push,
    Q,
    \s_axi_rdata[24] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[24] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[24] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[24].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[24].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[24] [0]),
        .A1(\s_axi_rdata[24] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_53
   (srl_out,
    push,
    Q,
    \s_axi_rdata[25] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[25] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[25] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[25].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[25].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[25] [0]),
        .A1(\s_axi_rdata[25] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_54
   (srl_out,
    push,
    Q,
    \s_axi_rdata[26] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[26] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[26] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[26].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[26].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[26] [0]),
        .A1(\s_axi_rdata[26] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_55
   (srl_out,
    push,
    Q,
    \s_axi_rdata[27] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[27] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[27] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[27].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[27].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[27] [0]),
        .A1(\s_axi_rdata[27] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_56
   (srl_out,
    push,
    Q,
    \s_axi_rdata[28] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[28] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[28] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[28].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[28].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[28] [0]),
        .A1(\s_axi_rdata[28] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_57
   (srl_out,
    push,
    Q,
    \s_axi_rdata[29] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[29] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[29] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[29].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[29].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[29] [0]),
        .A1(\s_axi_rdata[29] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_58
   (srl_out,
    push,
    Q,
    \s_axi_rdata[2] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[2] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[2] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[2].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[2].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[2] [0]),
        .A1(\s_axi_rdata[2] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_59
   (srl_out,
    push,
    Q,
    \s_axi_rdata[30] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[30] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[30] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[30].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[30].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[30] [0]),
        .A1(\s_axi_rdata[30] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_6
   (srl_out,
    push,
    Q,
    \m_axi_wdata[15] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[15] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[15] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[15].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[15].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[15] [0]),
        .A1(\m_axi_wdata[15] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_60
   (srl_out,
    push,
    Q,
    \s_axi_rdata[31] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[31] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[31] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[31].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[31].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[31] [0]),
        .A1(\s_axi_rdata[31] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_61
   (srl_out,
    push,
    Q,
    \s_axi_rresp[0] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rresp[0] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rresp[0] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[32].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[32].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rresp[0] [0]),
        .A1(\s_axi_rresp[0] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_62
   (srl_out,
    push,
    Q,
    \s_axi_rresp[1] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rresp[1] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rresp[1] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[33].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[33].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rresp[1] [0]),
        .A1(\s_axi_rresp[1] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_63
   (srl_out,
    push,
    Q,
    \s_axi_rdata[3] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[3] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[3] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[3].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[3].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[3] [0]),
        .A1(\s_axi_rdata[3] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_64
   (srl_out,
    push,
    Q,
    \s_axi_rdata[4] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[4] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[4] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[4].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[4].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[4] [0]),
        .A1(\s_axi_rdata[4] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_65
   (srl_out,
    push,
    Q,
    \s_axi_rdata[5] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[5] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[5] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[5].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[5].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[5] [0]),
        .A1(\s_axi_rdata[5] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_66
   (srl_out,
    push,
    Q,
    \s_axi_rdata[6] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[6] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[6] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[6].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[6].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[6] [0]),
        .A1(\s_axi_rdata[6] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_67
   (srl_out,
    push,
    Q,
    \s_axi_rdata[7] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[7] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[7] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[7].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[7].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[7] [0]),
        .A1(\s_axi_rdata[7] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_68
   (srl_out,
    push,
    Q,
    \s_axi_rdata[8] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[8] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[8] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[8].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[8].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[8] [0]),
        .A1(\s_axi_rdata[8] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_69
   (srl_out,
    push,
    Q,
    \s_axi_rdata[9] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_rdata[9] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_rdata[9] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\r.r_pipe/gen_srls[9].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\r.r_pipe/gen_srls[9].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_rdata[9] [0]),
        .A1(\s_axi_rdata[9] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_7
   (srl_out,
    push,
    Q,
    \m_axi_wdata[16] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[16] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[16] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[16].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[16].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[16] [0]),
        .A1(\m_axi_wdata[16] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_70
   (srl_out,
    push,
    Q,
    \s_axi_bresp[0] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_bresp[0] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_bresp[0] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\b.b_pipe/gen_srls[0].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\b.b_pipe/gen_srls[0].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_bresp[0] [0]),
        .A1(\s_axi_bresp[0] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_71
   (srl_out,
    push,
    Q,
    \s_axi_bresp[1] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\s_axi_bresp[1] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire push;
  wire [1:0]\s_axi_bresp[1] ;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\b.b_pipe/gen_srls[1].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\b.b_pipe/gen_srls[1].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\s_axi_bresp[1] [0]),
        .A1(\s_axi_bresp[1] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_72
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[0] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[0] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[0] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[0].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[0].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[0] [0]),
        .A1(\m_axi_awaddr[0] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_73
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[10] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[10] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[10] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[10].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[10].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[10] [0]),
        .A1(\m_axi_awaddr[10] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_74
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[11] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[11] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[11] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[11].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[11].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[11] [0]),
        .A1(\m_axi_awaddr[11] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_75
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[12] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[12] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[12] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[12].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[12].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[12] [0]),
        .A1(\m_axi_awaddr[12] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_76
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[13] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[13] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[13] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[13].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[13].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[13] [0]),
        .A1(\m_axi_awaddr[13] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_77
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[14] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[14] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[14] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[14].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[14].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[14] [0]),
        .A1(\m_axi_awaddr[14] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_78
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[15] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[15] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[15] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[15].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[15].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[15] [0]),
        .A1(\m_axi_awaddr[15] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_79
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[16] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[16] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[16] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[16].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[16].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[16] [0]),
        .A1(\m_axi_awaddr[16] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_8
   (srl_out,
    push,
    Q,
    \m_axi_wdata[17] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[17] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[17] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[17].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[17].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[17] [0]),
        .A1(\m_axi_wdata[17] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_80
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[17] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[17] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[17] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[17].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[17].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[17] [0]),
        .A1(\m_axi_awaddr[17] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_81
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[18] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[18] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[18] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[18].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[18].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[18] [0]),
        .A1(\m_axi_awaddr[18] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_82
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[19] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[19] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[19] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[19].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[19].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[19] [0]),
        .A1(\m_axi_awaddr[19] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_83
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[1] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[1] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[1] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[1].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[1].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[1] [0]),
        .A1(\m_axi_awaddr[1] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_84
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[20] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[20] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[20] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[20].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[20].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[20] [0]),
        .A1(\m_axi_awaddr[20] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_85
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[21] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[21] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[21] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[21].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[21].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[21] [0]),
        .A1(\m_axi_awaddr[21] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_86
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[22] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[22] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[22] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[22].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[22].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[22] [0]),
        .A1(\m_axi_awaddr[22] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_87
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[23] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[23] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[23] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[23].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[23].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[23] [0]),
        .A1(\m_axi_awaddr[23] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_88
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[24] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[24] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[24] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[24].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[24].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[24] [0]),
        .A1(\m_axi_awaddr[24] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_89
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[25] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[25] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[25] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[25].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[25].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[25] [0]),
        .A1(\m_axi_awaddr[25] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_9
   (srl_out,
    push,
    Q,
    \m_axi_wdata[18] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_wdata[18] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_wdata[18] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\w.w_pipe/gen_srls[18].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\w.w_pipe/gen_srls[18].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_wdata[18] [0]),
        .A1(\m_axi_wdata[18] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_90
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[26] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[26] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[26] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[26].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[26].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[26] [0]),
        .A1(\m_axi_awaddr[26] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_91
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[27] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[27] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[27] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[27].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[27].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[27] [0]),
        .A1(\m_axi_awaddr[27] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_92
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[28] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[28] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[28] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[28].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[28].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[28] [0]),
        .A1(\m_axi_awaddr[28] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_93
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[29] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[29] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[29] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[29].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[29].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[29] [0]),
        .A1(\m_axi_awaddr[29] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_94
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[2] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[2] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[2] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[2].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[2].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[2] [0]),
        .A1(\m_axi_awaddr[2] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_95
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[30] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[30] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[30] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[30].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[30].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[30] [0]),
        .A1(\m_axi_awaddr[30] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_96
   (srl_out,
    push,
    Q,
    \m_axi_awaddr[31] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awaddr[31] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awaddr[31] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[31].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[31].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awaddr[31] [0]),
        .A1(\m_axi_awaddr[31] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_97
   (srl_out,
    push,
    Q,
    \m_axi_awprot[0] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awprot[0] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awprot[0] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[32].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[32].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awprot[0] [0]),
        .A1(\m_axi_awprot[0] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_98
   (srl_out,
    push,
    Q,
    \m_axi_awprot[1] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awprot[1] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awprot[1] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[33].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[33].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awprot[1] [0]),
        .A1(\m_axi_awprot[1] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* ORIG_REF_NAME = "axi_register_slice_v2_1_22_srl_rtl" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_srl_rtl_99
   (srl_out,
    push,
    Q,
    \m_axi_awprot[2] ,
    aclk);
  output [0:0]srl_out;
  input push;
  input [0:0]Q;
  input [1:0]\m_axi_awprot[2] ;
  input aclk;

  wire [0:0]Q;
  wire aclk;
  wire [1:0]\m_axi_awprot[2] ;
  wire push;
  wire [0:0]srl_out;

  (* srl_bus_name = "inst/\aw.aw_pipe/gen_srls[34].srl_nx1/shift_reg_reg " *) 
  (* srl_name = "inst/\aw.aw_pipe/gen_srls[34].srl_nx1/shift_reg_reg[0]_srl4 " *) 
  SRL16E #(
    .INIT(16'h0000)) 
    \shift_reg_reg[0]_srl4 
       (.A0(\m_axi_awprot[2] [0]),
        .A1(\m_axi_awprot[2] [1]),
        .A2(1'b0),
        .A3(1'b0),
        .CE(push),
        .CLK(aclk),
        .D(Q),
        .Q(srl_out));
endmodule

(* CHECK_LICENSE_TYPE = "pfm_dynamic_axilite_user_input_reg_0,axi_register_slice_v2_1_22_axi_register_slice,{}" *) (* DowngradeIPIdentifiedWarnings = "yes" *) (* X_CORE_INFO = "axi_register_slice_v2_1_22_axi_register_slice,Vivado 2020.2" *) 
(* NotValidForBitStream *)
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix
   (aclk,
    aresetn,
    s_axi_awaddr,
    s_axi_awprot,
    s_axi_awvalid,
    s_axi_awready,
    s_axi_wdata,
    s_axi_wstrb,
    s_axi_wvalid,
    s_axi_wready,
    s_axi_bresp,
    s_axi_bvalid,
    s_axi_bready,
    s_axi_araddr,
    s_axi_arprot,
    s_axi_arvalid,
    s_axi_arready,
    s_axi_rdata,
    s_axi_rresp,
    s_axi_rvalid,
    s_axi_rready,
    m_axi_awaddr,
    m_axi_awprot,
    m_axi_awvalid,
    m_axi_awready,
    m_axi_wdata,
    m_axi_wstrb,
    m_axi_wvalid,
    m_axi_wready,
    m_axi_bresp,
    m_axi_bvalid,
    m_axi_bready,
    m_axi_araddr,
    m_axi_arprot,
    m_axi_arvalid,
    m_axi_arready,
    m_axi_rdata,
    m_axi_rresp,
    m_axi_rvalid,
    m_axi_rready);
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 CLK CLK" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME CLK, ASSOCIATED_BUSIF S_AXI:M_AXI, ASSOCIATED_RESET ARESETN, FREQ_HZ 50000000, FREQ_TOLERANCE_HZ 0, PHASE 0.000, CLK_DOMAIN pfm_dynamic_s_axi_aclk, INSERT_VIP 0" *) input aclk;
  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 RST RST" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME RST, POLARITY ACTIVE_LOW, INSERT_VIP 0, TYPE INTERCONNECT" *) input aresetn;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWADDR" *) input [31:0]s_axi_awaddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWPROT" *) input [2:0]s_axi_awprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWVALID" *) input s_axi_awvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWREADY" *) output s_axi_awready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI WDATA" *) input [31:0]s_axi_wdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI WSTRB" *) input [3:0]s_axi_wstrb;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI WVALID" *) input s_axi_wvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI WREADY" *) output s_axi_wready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI BRESP" *) output [1:0]s_axi_bresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI BVALID" *) output s_axi_bvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI BREADY" *) input s_axi_bready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI ARADDR" *) input [31:0]s_axi_araddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI ARPROT" *) input [2:0]s_axi_arprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI ARVALID" *) input s_axi_arvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI ARREADY" *) output s_axi_arready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI RDATA" *) output [31:0]s_axi_rdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI RRESP" *) output [1:0]s_axi_rresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI RVALID" *) output s_axi_rvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI RREADY" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME S_AXI, DATA_WIDTH 32, PROTOCOL AXI4LITE, FREQ_HZ 50000000, ID_WIDTH 0, ADDR_WIDTH 32, AWUSER_WIDTH 0, ARUSER_WIDTH 0, WUSER_WIDTH 0, RUSER_WIDTH 0, BUSER_WIDTH 0, READ_WRITE_MODE READ_WRITE, HAS_BURST 0, HAS_LOCK 0, HAS_PROT 1, HAS_CACHE 0, HAS_QOS 0, HAS_REGION 0, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 1, SUPPORTS_NARROW_BURST 0, NUM_READ_OUTSTANDING 2, NUM_WRITE_OUTSTANDING 2, MAX_BURST_LENGTH 1, PHASE 0.000, CLK_DOMAIN pfm_dynamic_s_axi_aclk, NUM_READ_THREADS 1, NUM_WRITE_THREADS 1, RUSER_BITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0" *) input s_axi_rready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI AWADDR" *) output [31:0]m_axi_awaddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI AWPROT" *) output [2:0]m_axi_awprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI AWVALID" *) output m_axi_awvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI AWREADY" *) input m_axi_awready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI WDATA" *) output [31:0]m_axi_wdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI WSTRB" *) output [3:0]m_axi_wstrb;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI WVALID" *) output m_axi_wvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI WREADY" *) input m_axi_wready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI BRESP" *) input [1:0]m_axi_bresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI BVALID" *) input m_axi_bvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI BREADY" *) output m_axi_bready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI ARADDR" *) output [31:0]m_axi_araddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI ARPROT" *) output [2:0]m_axi_arprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI ARVALID" *) output m_axi_arvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI ARREADY" *) input m_axi_arready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI RDATA" *) input [31:0]m_axi_rdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI RRESP" *) input [1:0]m_axi_rresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI RVALID" *) input m_axi_rvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI RREADY" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME M_AXI, DATA_WIDTH 32, PROTOCOL AXI4LITE, FREQ_HZ 50000000, ID_WIDTH 0, ADDR_WIDTH 32, AWUSER_WIDTH 0, ARUSER_WIDTH 0, WUSER_WIDTH 0, RUSER_WIDTH 0, BUSER_WIDTH 0, READ_WRITE_MODE READ_WRITE, HAS_BURST 0, HAS_LOCK 0, HAS_PROT 1, HAS_CACHE 0, HAS_QOS 0, HAS_REGION 0, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 1, SUPPORTS_NARROW_BURST 0, NUM_READ_OUTSTANDING 2, NUM_WRITE_OUTSTANDING 2, MAX_BURST_LENGTH 1, PHASE 0.000, CLK_DOMAIN pfm_dynamic_s_axi_aclk, NUM_READ_THREADS 1, NUM_WRITE_THREADS 1, RUSER_BITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0" *) output m_axi_rready;

  wire aclk;
  wire aresetn;
  wire [31:0]m_axi_araddr;
  wire [2:0]m_axi_arprot;
  wire m_axi_arready;
  wire m_axi_arvalid;
  wire [31:0]m_axi_awaddr;
  wire [2:0]m_axi_awprot;
  wire m_axi_awready;
  wire m_axi_awvalid;
  wire m_axi_bready;
  wire [1:0]m_axi_bresp;
  wire m_axi_bvalid;
  wire [31:0]m_axi_rdata;
  wire m_axi_rready;
  wire [1:0]m_axi_rresp;
  wire m_axi_rvalid;
  wire [31:0]m_axi_wdata;
  wire m_axi_wready;
  wire [3:0]m_axi_wstrb;
  wire m_axi_wvalid;
  wire [31:0]s_axi_araddr;
  wire [2:0]s_axi_arprot;
  wire s_axi_arready;
  wire s_axi_arvalid;
  wire [31:0]s_axi_awaddr;
  wire [2:0]s_axi_awprot;
  wire s_axi_awready;
  wire s_axi_awvalid;
  wire s_axi_bready;
  wire [1:0]s_axi_bresp;
  wire s_axi_bvalid;
  wire [31:0]s_axi_rdata;
  wire s_axi_rready;
  wire [1:0]s_axi_rresp;
  wire s_axi_rvalid;
  wire [31:0]s_axi_wdata;
  wire s_axi_wready;
  wire [3:0]s_axi_wstrb;
  wire s_axi_wvalid;
  wire NLW_inst_m_axi_wlast_UNCONNECTED;
  wire NLW_inst_s_axi_rlast_UNCONNECTED;
  wire [1:0]NLW_inst_m_axi_arburst_UNCONNECTED;
  wire [3:0]NLW_inst_m_axi_arcache_UNCONNECTED;
  wire [0:0]NLW_inst_m_axi_arid_UNCONNECTED;
  wire [7:0]NLW_inst_m_axi_arlen_UNCONNECTED;
  wire [0:0]NLW_inst_m_axi_arlock_UNCONNECTED;
  wire [3:0]NLW_inst_m_axi_arqos_UNCONNECTED;
  wire [3:0]NLW_inst_m_axi_arregion_UNCONNECTED;
  wire [2:0]NLW_inst_m_axi_arsize_UNCONNECTED;
  wire [0:0]NLW_inst_m_axi_aruser_UNCONNECTED;
  wire [1:0]NLW_inst_m_axi_awburst_UNCONNECTED;
  wire [3:0]NLW_inst_m_axi_awcache_UNCONNECTED;
  wire [0:0]NLW_inst_m_axi_awid_UNCONNECTED;
  wire [7:0]NLW_inst_m_axi_awlen_UNCONNECTED;
  wire [0:0]NLW_inst_m_axi_awlock_UNCONNECTED;
  wire [3:0]NLW_inst_m_axi_awqos_UNCONNECTED;
  wire [3:0]NLW_inst_m_axi_awregion_UNCONNECTED;
  wire [2:0]NLW_inst_m_axi_awsize_UNCONNECTED;
  wire [0:0]NLW_inst_m_axi_awuser_UNCONNECTED;
  wire [0:0]NLW_inst_m_axi_wid_UNCONNECTED;
  wire [0:0]NLW_inst_m_axi_wuser_UNCONNECTED;
  wire [0:0]NLW_inst_s_axi_bid_UNCONNECTED;
  wire [0:0]NLW_inst_s_axi_buser_UNCONNECTED;
  wire [0:0]NLW_inst_s_axi_rid_UNCONNECTED;
  wire [0:0]NLW_inst_s_axi_ruser_UNCONNECTED;

  (* C_AXI_ADDR_WIDTH = "32" *) 
  (* C_AXI_ARUSER_WIDTH = "1" *) 
  (* C_AXI_AWUSER_WIDTH = "1" *) 
  (* C_AXI_BUSER_WIDTH = "1" *) 
  (* C_AXI_DATA_WIDTH = "32" *) 
  (* C_AXI_ID_WIDTH = "1" *) 
  (* C_AXI_PROTOCOL = "2" *) 
  (* C_AXI_RUSER_WIDTH = "1" *) 
  (* C_AXI_SUPPORTS_REGION_SIGNALS = "0" *) 
  (* C_AXI_SUPPORTS_USER_SIGNALS = "0" *) 
  (* C_AXI_WUSER_WIDTH = "1" *) 
  (* C_FAMILY = "virtexuplusHBM" *) 
  (* C_NUM_SLR_CROSSINGS = "0" *) 
  (* C_PIPELINES_MASTER_AR = "0" *) 
  (* C_PIPELINES_MASTER_AW = "0" *) 
  (* C_PIPELINES_MASTER_B = "0" *) 
  (* C_PIPELINES_MASTER_R = "0" *) 
  (* C_PIPELINES_MASTER_W = "0" *) 
  (* C_PIPELINES_MIDDLE_AR = "0" *) 
  (* C_PIPELINES_MIDDLE_AW = "0" *) 
  (* C_PIPELINES_MIDDLE_B = "0" *) 
  (* C_PIPELINES_MIDDLE_R = "0" *) 
  (* C_PIPELINES_MIDDLE_W = "0" *) 
  (* C_PIPELINES_SLAVE_AR = "0" *) 
  (* C_PIPELINES_SLAVE_AW = "0" *) 
  (* C_PIPELINES_SLAVE_B = "0" *) 
  (* C_PIPELINES_SLAVE_R = "0" *) 
  (* C_PIPELINES_SLAVE_W = "0" *) 
  (* C_REG_CONFIG_AR = "9" *) 
  (* C_REG_CONFIG_AW = "9" *) 
  (* C_REG_CONFIG_B = "9" *) 
  (* C_REG_CONFIG_R = "9" *) 
  (* C_REG_CONFIG_W = "9" *) 
  (* C_RESERVE_MODE = "0" *) 
  (* DowngradeIPIdentifiedWarnings = "yes" *) 
  (* G_AXI_ARADDR_INDEX = "0" *) 
  (* G_AXI_ARADDR_WIDTH = "32" *) 
  (* G_AXI_ARBURST_INDEX = "35" *) 
  (* G_AXI_ARBURST_WIDTH = "0" *) 
  (* G_AXI_ARCACHE_INDEX = "35" *) 
  (* G_AXI_ARCACHE_WIDTH = "0" *) 
  (* G_AXI_ARID_INDEX = "35" *) 
  (* G_AXI_ARID_WIDTH = "0" *) 
  (* G_AXI_ARLEN_INDEX = "35" *) 
  (* G_AXI_ARLEN_WIDTH = "0" *) 
  (* G_AXI_ARLOCK_INDEX = "35" *) 
  (* G_AXI_ARLOCK_WIDTH = "0" *) 
  (* G_AXI_ARPAYLOAD_WIDTH = "35" *) 
  (* G_AXI_ARPROT_INDEX = "32" *) 
  (* G_AXI_ARPROT_WIDTH = "3" *) 
  (* G_AXI_ARQOS_INDEX = "35" *) 
  (* G_AXI_ARQOS_WIDTH = "0" *) 
  (* G_AXI_ARREGION_INDEX = "35" *) 
  (* G_AXI_ARREGION_WIDTH = "0" *) 
  (* G_AXI_ARSIZE_INDEX = "35" *) 
  (* G_AXI_ARSIZE_WIDTH = "0" *) 
  (* G_AXI_ARUSER_INDEX = "35" *) 
  (* G_AXI_ARUSER_WIDTH = "0" *) 
  (* G_AXI_AWADDR_INDEX = "0" *) 
  (* G_AXI_AWADDR_WIDTH = "32" *) 
  (* G_AXI_AWBURST_INDEX = "35" *) 
  (* G_AXI_AWBURST_WIDTH = "0" *) 
  (* G_AXI_AWCACHE_INDEX = "35" *) 
  (* G_AXI_AWCACHE_WIDTH = "0" *) 
  (* G_AXI_AWID_INDEX = "35" *) 
  (* G_AXI_AWID_WIDTH = "0" *) 
  (* G_AXI_AWLEN_INDEX = "35" *) 
  (* G_AXI_AWLEN_WIDTH = "0" *) 
  (* G_AXI_AWLOCK_INDEX = "35" *) 
  (* G_AXI_AWLOCK_WIDTH = "0" *) 
  (* G_AXI_AWPAYLOAD_WIDTH = "35" *) 
  (* G_AXI_AWPROT_INDEX = "32" *) 
  (* G_AXI_AWPROT_WIDTH = "3" *) 
  (* G_AXI_AWQOS_INDEX = "35" *) 
  (* G_AXI_AWQOS_WIDTH = "0" *) 
  (* G_AXI_AWREGION_INDEX = "35" *) 
  (* G_AXI_AWREGION_WIDTH = "0" *) 
  (* G_AXI_AWSIZE_INDEX = "35" *) 
  (* G_AXI_AWSIZE_WIDTH = "0" *) 
  (* G_AXI_AWUSER_INDEX = "35" *) 
  (* G_AXI_AWUSER_WIDTH = "0" *) 
  (* G_AXI_BID_INDEX = "2" *) 
  (* G_AXI_BID_WIDTH = "0" *) 
  (* G_AXI_BPAYLOAD_WIDTH = "2" *) 
  (* G_AXI_BRESP_INDEX = "0" *) 
  (* G_AXI_BRESP_WIDTH = "2" *) 
  (* G_AXI_BUSER_INDEX = "2" *) 
  (* G_AXI_BUSER_WIDTH = "0" *) 
  (* G_AXI_RDATA_INDEX = "0" *) 
  (* G_AXI_RDATA_WIDTH = "32" *) 
  (* G_AXI_RID_INDEX = "34" *) 
  (* G_AXI_RID_WIDTH = "0" *) 
  (* G_AXI_RLAST_INDEX = "34" *) 
  (* G_AXI_RLAST_WIDTH = "0" *) 
  (* G_AXI_RPAYLOAD_WIDTH = "34" *) 
  (* G_AXI_RRESP_INDEX = "32" *) 
  (* G_AXI_RRESP_WIDTH = "2" *) 
  (* G_AXI_RUSER_INDEX = "34" *) 
  (* G_AXI_RUSER_WIDTH = "0" *) 
  (* G_AXI_WDATA_INDEX = "0" *) 
  (* G_AXI_WDATA_WIDTH = "32" *) 
  (* G_AXI_WID_INDEX = "36" *) 
  (* G_AXI_WID_WIDTH = "0" *) 
  (* G_AXI_WLAST_INDEX = "36" *) 
  (* G_AXI_WLAST_WIDTH = "0" *) 
  (* G_AXI_WPAYLOAD_WIDTH = "36" *) 
  (* G_AXI_WSTRB_INDEX = "32" *) 
  (* G_AXI_WSTRB_WIDTH = "4" *) 
  (* G_AXI_WUSER_INDEX = "36" *) 
  (* G_AXI_WUSER_WIDTH = "0" *) 
  (* P_FORWARD = "0" *) 
  (* P_RESPONSE = "1" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_register_slice_v2_1_22_axi_register_slice inst
       (.aclk(aclk),
        .aclk2x(1'b0),
        .aresetn(aresetn),
        .m_axi_araddr(m_axi_araddr),
        .m_axi_arburst(NLW_inst_m_axi_arburst_UNCONNECTED[1:0]),
        .m_axi_arcache(NLW_inst_m_axi_arcache_UNCONNECTED[3:0]),
        .m_axi_arid(NLW_inst_m_axi_arid_UNCONNECTED[0]),
        .m_axi_arlen(NLW_inst_m_axi_arlen_UNCONNECTED[7:0]),
        .m_axi_arlock(NLW_inst_m_axi_arlock_UNCONNECTED[0]),
        .m_axi_arprot(m_axi_arprot),
        .m_axi_arqos(NLW_inst_m_axi_arqos_UNCONNECTED[3:0]),
        .m_axi_arready(m_axi_arready),
        .m_axi_arregion(NLW_inst_m_axi_arregion_UNCONNECTED[3:0]),
        .m_axi_arsize(NLW_inst_m_axi_arsize_UNCONNECTED[2:0]),
        .m_axi_aruser(NLW_inst_m_axi_aruser_UNCONNECTED[0]),
        .m_axi_arvalid(m_axi_arvalid),
        .m_axi_awaddr(m_axi_awaddr),
        .m_axi_awburst(NLW_inst_m_axi_awburst_UNCONNECTED[1:0]),
        .m_axi_awcache(NLW_inst_m_axi_awcache_UNCONNECTED[3:0]),
        .m_axi_awid(NLW_inst_m_axi_awid_UNCONNECTED[0]),
        .m_axi_awlen(NLW_inst_m_axi_awlen_UNCONNECTED[7:0]),
        .m_axi_awlock(NLW_inst_m_axi_awlock_UNCONNECTED[0]),
        .m_axi_awprot(m_axi_awprot),
        .m_axi_awqos(NLW_inst_m_axi_awqos_UNCONNECTED[3:0]),
        .m_axi_awready(m_axi_awready),
        .m_axi_awregion(NLW_inst_m_axi_awregion_UNCONNECTED[3:0]),
        .m_axi_awsize(NLW_inst_m_axi_awsize_UNCONNECTED[2:0]),
        .m_axi_awuser(NLW_inst_m_axi_awuser_UNCONNECTED[0]),
        .m_axi_awvalid(m_axi_awvalid),
        .m_axi_bid(1'b0),
        .m_axi_bready(m_axi_bready),
        .m_axi_bresp(m_axi_bresp),
        .m_axi_buser(1'b0),
        .m_axi_bvalid(m_axi_bvalid),
        .m_axi_rdata(m_axi_rdata),
        .m_axi_rid(1'b0),
        .m_axi_rlast(1'b0),
        .m_axi_rready(m_axi_rready),
        .m_axi_rresp(m_axi_rresp),
        .m_axi_ruser(1'b0),
        .m_axi_rvalid(m_axi_rvalid),
        .m_axi_wdata(m_axi_wdata),
        .m_axi_wid(NLW_inst_m_axi_wid_UNCONNECTED[0]),
        .m_axi_wlast(NLW_inst_m_axi_wlast_UNCONNECTED),
        .m_axi_wready(m_axi_wready),
        .m_axi_wstrb(m_axi_wstrb),
        .m_axi_wuser(NLW_inst_m_axi_wuser_UNCONNECTED[0]),
        .m_axi_wvalid(m_axi_wvalid),
        .s_axi_araddr(s_axi_araddr),
        .s_axi_arburst({1'b0,1'b1}),
        .s_axi_arcache({1'b0,1'b0,1'b0,1'b0}),
        .s_axi_arid(1'b0),
        .s_axi_arlen({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .s_axi_arlock(1'b0),
        .s_axi_arprot(s_axi_arprot),
        .s_axi_arqos({1'b0,1'b0,1'b0,1'b0}),
        .s_axi_arready(s_axi_arready),
        .s_axi_arregion({1'b0,1'b0,1'b0,1'b0}),
        .s_axi_arsize({1'b0,1'b1,1'b0}),
        .s_axi_aruser(1'b0),
        .s_axi_arvalid(s_axi_arvalid),
        .s_axi_awaddr(s_axi_awaddr),
        .s_axi_awburst({1'b0,1'b1}),
        .s_axi_awcache({1'b0,1'b0,1'b0,1'b0}),
        .s_axi_awid(1'b0),
        .s_axi_awlen({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .s_axi_awlock(1'b0),
        .s_axi_awprot(s_axi_awprot),
        .s_axi_awqos({1'b0,1'b0,1'b0,1'b0}),
        .s_axi_awready(s_axi_awready),
        .s_axi_awregion({1'b0,1'b0,1'b0,1'b0}),
        .s_axi_awsize({1'b0,1'b1,1'b0}),
        .s_axi_awuser(1'b0),
        .s_axi_awvalid(s_axi_awvalid),
        .s_axi_bid(NLW_inst_s_axi_bid_UNCONNECTED[0]),
        .s_axi_bready(s_axi_bready),
        .s_axi_bresp(s_axi_bresp),
        .s_axi_buser(NLW_inst_s_axi_buser_UNCONNECTED[0]),
        .s_axi_bvalid(s_axi_bvalid),
        .s_axi_rdata(s_axi_rdata),
        .s_axi_rid(NLW_inst_s_axi_rid_UNCONNECTED[0]),
        .s_axi_rlast(NLW_inst_s_axi_rlast_UNCONNECTED),
        .s_axi_rready(s_axi_rready),
        .s_axi_rresp(s_axi_rresp),
        .s_axi_ruser(NLW_inst_s_axi_ruser_UNCONNECTED[0]),
        .s_axi_rvalid(s_axi_rvalid),
        .s_axi_wdata(s_axi_wdata),
        .s_axi_wid(1'b0),
        .s_axi_wlast(1'b0),
        .s_axi_wready(s_axi_wready),
        .s_axi_wstrb(s_axi_wstrb),
        .s_axi_wuser(1'b0),
        .s_axi_wvalid(s_axi_wvalid));
endmodule
`ifndef GLBL
`define GLBL
`timescale  1 ps / 1 ps

module glbl ();

    parameter ROC_WIDTH = 100000;
    parameter TOC_WIDTH = 0;
    parameter GRES_WIDTH = 10000;
    parameter GRES_START = 10000;

//--------   STARTUP Globals --------------
    wire GSR;
    wire GTS;
    wire GWE;
    wire PRLD;
    wire GRESTORE;
    tri1 p_up_tmp;
    tri (weak1, strong0) PLL_LOCKG = p_up_tmp;

    wire PROGB_GLBL;
    wire CCLKO_GLBL;
    wire FCSBO_GLBL;
    wire [3:0] DO_GLBL;
    wire [3:0] DI_GLBL;
   
    reg GSR_int;
    reg GTS_int;
    reg PRLD_int;
    reg GRESTORE_int;

//--------   JTAG Globals --------------
    wire JTAG_TDO_GLBL;
    wire JTAG_TCK_GLBL;
    wire JTAG_TDI_GLBL;
    wire JTAG_TMS_GLBL;
    wire JTAG_TRST_GLBL;

    reg JTAG_CAPTURE_GLBL;
    reg JTAG_RESET_GLBL;
    reg JTAG_SHIFT_GLBL;
    reg JTAG_UPDATE_GLBL;
    reg JTAG_RUNTEST_GLBL;

    reg JTAG_SEL1_GLBL = 0;
    reg JTAG_SEL2_GLBL = 0 ;
    reg JTAG_SEL3_GLBL = 0;
    reg JTAG_SEL4_GLBL = 0;

    reg JTAG_USER_TDO1_GLBL = 1'bz;
    reg JTAG_USER_TDO2_GLBL = 1'bz;
    reg JTAG_USER_TDO3_GLBL = 1'bz;
    reg JTAG_USER_TDO4_GLBL = 1'bz;

    assign (strong1, weak0) GSR = GSR_int;
    assign (strong1, weak0) GTS = GTS_int;
    assign (weak1, weak0) PRLD = PRLD_int;
    assign (strong1, weak0) GRESTORE = GRESTORE_int;

    initial begin
	GSR_int = 1'b1;
	PRLD_int = 1'b1;
	#(ROC_WIDTH)
	GSR_int = 1'b0;
	PRLD_int = 1'b0;
    end

    initial begin
	GTS_int = 1'b1;
	#(TOC_WIDTH)
	GTS_int = 1'b0;
    end

    initial begin 
	GRESTORE_int = 1'b0;
	#(GRES_START);
	GRESTORE_int = 1'b1;
	#(GRES_WIDTH);
	GRESTORE_int = 1'b0;
    end

endmodule
`endif

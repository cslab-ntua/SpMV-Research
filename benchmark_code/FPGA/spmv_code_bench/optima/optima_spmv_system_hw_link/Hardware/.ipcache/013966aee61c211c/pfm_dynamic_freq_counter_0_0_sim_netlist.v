// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2020.2 (lin64) Build 3064766 Wed Nov 18 09:12:47 MST 2020
// Date        : Fri Oct 20 19:40:59 2023
// Host        : gold1 running 64-bit Ubuntu 18.04.6 LTS
// Command     : write_verilog -force -mode funcsim -rename_top decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix -prefix
//               decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_ pfm_dynamic_freq_counter_0_0_sim_netlist.v
// Design      : pfm_dynamic_freq_counter_0_0
// Purpose     : This verilog netlist is a functional simulation representation of the design and should not be modified
//               or synthesized. This netlist cannot be used for SDF annotated simulation.
// Device      : xcu280-fsvh2892-2L-e
// --------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_freq_counter
   (axil_rdata,
    axil_awready,
    axil_wready,
    axil_bvalid,
    axil_arready,
    axil_rvalid,
    test_clk0,
    test_clk1,
    clk,
    axil_arvalid,
    axil_araddr,
    axil_bready,
    axil_wvalid,
    axil_awvalid,
    reset_n,
    axil_rready,
    axil_wdata,
    axil_awaddr);
  output [31:0]axil_rdata;
  output axil_awready;
  output axil_wready;
  output axil_bvalid;
  output axil_arready;
  output axil_rvalid;
  input test_clk0;
  input test_clk1;
  input clk;
  input axil_arvalid;
  input [3:0]axil_araddr;
  input axil_bready;
  input axil_wvalid;
  input axil_awvalid;
  input reset_n;
  input axil_rready;
  input [30:0]axil_wdata;
  input [3:0]axil_awaddr;

  wire [3:0]axil_araddr;
  wire axil_arready;
  wire axil_arready_i_1_n_0;
  wire axil_arvalid;
  wire [3:0]axil_awaddr;
  wire axil_awready;
  wire axil_awready_i_1_n_0;
  wire axil_awvalid;
  wire axil_bready;
  wire axil_bvalid;
  wire axil_bvalid_i_1_n_0;
  wire [31:0]axil_rdata;
  wire \axil_rdata[0]_i_2_n_0 ;
  wire \axil_rdata[0]_i_3_n_0 ;
  wire \axil_rdata[10]_i_2_n_0 ;
  wire \axil_rdata[11]_i_2_n_0 ;
  wire \axil_rdata[12]_i_2_n_0 ;
  wire \axil_rdata[13]_i_2_n_0 ;
  wire \axil_rdata[14]_i_1_n_0 ;
  wire \axil_rdata[14]_i_2_n_0 ;
  wire \axil_rdata[15]_i_2_n_0 ;
  wire \axil_rdata[16]_i_2_n_0 ;
  wire \axil_rdata[17]_i_1_n_0 ;
  wire \axil_rdata[17]_i_2_n_0 ;
  wire \axil_rdata[18]_i_2_n_0 ;
  wire \axil_rdata[19]_i_2_n_0 ;
  wire \axil_rdata[1]_i_10_n_0 ;
  wire \axil_rdata[1]_i_11_n_0 ;
  wire \axil_rdata[1]_i_12_n_0 ;
  wire \axil_rdata[1]_i_2_n_0 ;
  wire \axil_rdata[1]_i_3_n_0 ;
  wire \axil_rdata[1]_i_4_n_0 ;
  wire \axil_rdata[1]_i_5_n_0 ;
  wire \axil_rdata[1]_i_6_n_0 ;
  wire \axil_rdata[1]_i_7_n_0 ;
  wire \axil_rdata[1]_i_8_n_0 ;
  wire \axil_rdata[1]_i_9_n_0 ;
  wire \axil_rdata[20]_i_1_n_0 ;
  wire \axil_rdata[20]_i_2_n_0 ;
  wire \axil_rdata[21]_i_2_n_0 ;
  wire \axil_rdata[22]_i_1_n_0 ;
  wire \axil_rdata[22]_i_2_n_0 ;
  wire \axil_rdata[23]_i_2_n_0 ;
  wire \axil_rdata[24]_i_1_n_0 ;
  wire \axil_rdata[24]_i_2_n_0 ;
  wire \axil_rdata[25]_i_2_n_0 ;
  wire \axil_rdata[26]_i_2_n_0 ;
  wire \axil_rdata[27]_i_2_n_0 ;
  wire \axil_rdata[28]_i_2_n_0 ;
  wire \axil_rdata[29]_i_1_n_0 ;
  wire \axil_rdata[29]_i_2_n_0 ;
  wire \axil_rdata[29]_i_3_n_0 ;
  wire \axil_rdata[2]_i_2_n_0 ;
  wire \axil_rdata[30]_i_2_n_0 ;
  wire \axil_rdata[31]_i_2_n_0 ;
  wire \axil_rdata[31]_i_3_n_0 ;
  wire \axil_rdata[3]_i_2_n_0 ;
  wire \axil_rdata[4]_i_1_n_0 ;
  wire \axil_rdata[4]_i_2_n_0 ;
  wire \axil_rdata[5]_i_2_n_0 ;
  wire \axil_rdata[6]_i_2_n_0 ;
  wire \axil_rdata[7]_i_2_n_0 ;
  wire \axil_rdata[8]_i_1_n_0 ;
  wire \axil_rdata[8]_i_2_n_0 ;
  wire \axil_rdata[9]_i_2_n_0 ;
  wire [31:0]axil_rdata_0;
  wire axil_rready;
  wire axil_rvalid;
  wire axil_rvalid_i_1_n_0;
  wire [30:0]axil_wdata;
  wire axil_wready;
  wire axil_wready4_out;
  wire axil_wready_i_1_n_0;
  wire axil_wvalid;
  wire \clear_user_rst_reg_n_0_[0] ;
  wire clk;
  wire done;
  wire done0_synced;
  wire done1_synced;
  wire p_1_in;
  wire [1:1]p_1_in__0;
  wire [1:1]p_1_in__1;
  wire [0:0]p_1_out;
  wire \ref_clk_cntr[0]_i_3_n_0 ;
  wire [31:0]ref_clk_cntr_reg;
  wire \ref_clk_cntr_reg[0]_i_2_n_0 ;
  wire \ref_clk_cntr_reg[0]_i_2_n_1 ;
  wire \ref_clk_cntr_reg[0]_i_2_n_10 ;
  wire \ref_clk_cntr_reg[0]_i_2_n_11 ;
  wire \ref_clk_cntr_reg[0]_i_2_n_12 ;
  wire \ref_clk_cntr_reg[0]_i_2_n_13 ;
  wire \ref_clk_cntr_reg[0]_i_2_n_14 ;
  wire \ref_clk_cntr_reg[0]_i_2_n_15 ;
  wire \ref_clk_cntr_reg[0]_i_2_n_2 ;
  wire \ref_clk_cntr_reg[0]_i_2_n_3 ;
  wire \ref_clk_cntr_reg[0]_i_2_n_4 ;
  wire \ref_clk_cntr_reg[0]_i_2_n_5 ;
  wire \ref_clk_cntr_reg[0]_i_2_n_6 ;
  wire \ref_clk_cntr_reg[0]_i_2_n_7 ;
  wire \ref_clk_cntr_reg[0]_i_2_n_8 ;
  wire \ref_clk_cntr_reg[0]_i_2_n_9 ;
  wire \ref_clk_cntr_reg[16]_i_1_n_0 ;
  wire \ref_clk_cntr_reg[16]_i_1_n_1 ;
  wire \ref_clk_cntr_reg[16]_i_1_n_10 ;
  wire \ref_clk_cntr_reg[16]_i_1_n_11 ;
  wire \ref_clk_cntr_reg[16]_i_1_n_12 ;
  wire \ref_clk_cntr_reg[16]_i_1_n_13 ;
  wire \ref_clk_cntr_reg[16]_i_1_n_14 ;
  wire \ref_clk_cntr_reg[16]_i_1_n_15 ;
  wire \ref_clk_cntr_reg[16]_i_1_n_2 ;
  wire \ref_clk_cntr_reg[16]_i_1_n_3 ;
  wire \ref_clk_cntr_reg[16]_i_1_n_4 ;
  wire \ref_clk_cntr_reg[16]_i_1_n_5 ;
  wire \ref_clk_cntr_reg[16]_i_1_n_6 ;
  wire \ref_clk_cntr_reg[16]_i_1_n_7 ;
  wire \ref_clk_cntr_reg[16]_i_1_n_8 ;
  wire \ref_clk_cntr_reg[16]_i_1_n_9 ;
  wire \ref_clk_cntr_reg[24]_i_1_n_1 ;
  wire \ref_clk_cntr_reg[24]_i_1_n_10 ;
  wire \ref_clk_cntr_reg[24]_i_1_n_11 ;
  wire \ref_clk_cntr_reg[24]_i_1_n_12 ;
  wire \ref_clk_cntr_reg[24]_i_1_n_13 ;
  wire \ref_clk_cntr_reg[24]_i_1_n_14 ;
  wire \ref_clk_cntr_reg[24]_i_1_n_15 ;
  wire \ref_clk_cntr_reg[24]_i_1_n_2 ;
  wire \ref_clk_cntr_reg[24]_i_1_n_3 ;
  wire \ref_clk_cntr_reg[24]_i_1_n_4 ;
  wire \ref_clk_cntr_reg[24]_i_1_n_5 ;
  wire \ref_clk_cntr_reg[24]_i_1_n_6 ;
  wire \ref_clk_cntr_reg[24]_i_1_n_7 ;
  wire \ref_clk_cntr_reg[24]_i_1_n_8 ;
  wire \ref_clk_cntr_reg[24]_i_1_n_9 ;
  wire \ref_clk_cntr_reg[8]_i_1_n_0 ;
  wire \ref_clk_cntr_reg[8]_i_1_n_1 ;
  wire \ref_clk_cntr_reg[8]_i_1_n_10 ;
  wire \ref_clk_cntr_reg[8]_i_1_n_11 ;
  wire \ref_clk_cntr_reg[8]_i_1_n_12 ;
  wire \ref_clk_cntr_reg[8]_i_1_n_13 ;
  wire \ref_clk_cntr_reg[8]_i_1_n_14 ;
  wire \ref_clk_cntr_reg[8]_i_1_n_15 ;
  wire \ref_clk_cntr_reg[8]_i_1_n_2 ;
  wire \ref_clk_cntr_reg[8]_i_1_n_3 ;
  wire \ref_clk_cntr_reg[8]_i_1_n_4 ;
  wire \ref_clk_cntr_reg[8]_i_1_n_5 ;
  wire \ref_clk_cntr_reg[8]_i_1_n_6 ;
  wire \ref_clk_cntr_reg[8]_i_1_n_7 ;
  wire \ref_clk_cntr_reg[8]_i_1_n_8 ;
  wire \ref_clk_cntr_reg[8]_i_1_n_9 ;
  wire reset_n;
  wire rst0_synced;
  wire rst1_synced;
  wire sel;
  wire src_in0;
  wire src_in1;
  wire [1:0]state_read;
  wire \state_read[1]_i_1_n_0 ;
  wire [2:0]state_write;
  wire \state_write[2]_i_2_n_0 ;
  wire test_clk0;
  wire \test_clk0_cntr[0]_i_1_n_0 ;
  wire \test_clk0_cntr[0]_i_3_n_0 ;
  wire [31:0]test_clk0_cntr_reg;
  wire \test_clk0_cntr_reg[0]_i_2_n_0 ;
  wire \test_clk0_cntr_reg[0]_i_2_n_1 ;
  wire \test_clk0_cntr_reg[0]_i_2_n_10 ;
  wire \test_clk0_cntr_reg[0]_i_2_n_11 ;
  wire \test_clk0_cntr_reg[0]_i_2_n_12 ;
  wire \test_clk0_cntr_reg[0]_i_2_n_13 ;
  wire \test_clk0_cntr_reg[0]_i_2_n_14 ;
  wire \test_clk0_cntr_reg[0]_i_2_n_15 ;
  wire \test_clk0_cntr_reg[0]_i_2_n_2 ;
  wire \test_clk0_cntr_reg[0]_i_2_n_3 ;
  wire \test_clk0_cntr_reg[0]_i_2_n_4 ;
  wire \test_clk0_cntr_reg[0]_i_2_n_5 ;
  wire \test_clk0_cntr_reg[0]_i_2_n_6 ;
  wire \test_clk0_cntr_reg[0]_i_2_n_7 ;
  wire \test_clk0_cntr_reg[0]_i_2_n_8 ;
  wire \test_clk0_cntr_reg[0]_i_2_n_9 ;
  wire \test_clk0_cntr_reg[16]_i_1_n_0 ;
  wire \test_clk0_cntr_reg[16]_i_1_n_1 ;
  wire \test_clk0_cntr_reg[16]_i_1_n_10 ;
  wire \test_clk0_cntr_reg[16]_i_1_n_11 ;
  wire \test_clk0_cntr_reg[16]_i_1_n_12 ;
  wire \test_clk0_cntr_reg[16]_i_1_n_13 ;
  wire \test_clk0_cntr_reg[16]_i_1_n_14 ;
  wire \test_clk0_cntr_reg[16]_i_1_n_15 ;
  wire \test_clk0_cntr_reg[16]_i_1_n_2 ;
  wire \test_clk0_cntr_reg[16]_i_1_n_3 ;
  wire \test_clk0_cntr_reg[16]_i_1_n_4 ;
  wire \test_clk0_cntr_reg[16]_i_1_n_5 ;
  wire \test_clk0_cntr_reg[16]_i_1_n_6 ;
  wire \test_clk0_cntr_reg[16]_i_1_n_7 ;
  wire \test_clk0_cntr_reg[16]_i_1_n_8 ;
  wire \test_clk0_cntr_reg[16]_i_1_n_9 ;
  wire \test_clk0_cntr_reg[24]_i_1_n_1 ;
  wire \test_clk0_cntr_reg[24]_i_1_n_10 ;
  wire \test_clk0_cntr_reg[24]_i_1_n_11 ;
  wire \test_clk0_cntr_reg[24]_i_1_n_12 ;
  wire \test_clk0_cntr_reg[24]_i_1_n_13 ;
  wire \test_clk0_cntr_reg[24]_i_1_n_14 ;
  wire \test_clk0_cntr_reg[24]_i_1_n_15 ;
  wire \test_clk0_cntr_reg[24]_i_1_n_2 ;
  wire \test_clk0_cntr_reg[24]_i_1_n_3 ;
  wire \test_clk0_cntr_reg[24]_i_1_n_4 ;
  wire \test_clk0_cntr_reg[24]_i_1_n_5 ;
  wire \test_clk0_cntr_reg[24]_i_1_n_6 ;
  wire \test_clk0_cntr_reg[24]_i_1_n_7 ;
  wire \test_clk0_cntr_reg[24]_i_1_n_8 ;
  wire \test_clk0_cntr_reg[24]_i_1_n_9 ;
  wire \test_clk0_cntr_reg[8]_i_1_n_0 ;
  wire \test_clk0_cntr_reg[8]_i_1_n_1 ;
  wire \test_clk0_cntr_reg[8]_i_1_n_10 ;
  wire \test_clk0_cntr_reg[8]_i_1_n_11 ;
  wire \test_clk0_cntr_reg[8]_i_1_n_12 ;
  wire \test_clk0_cntr_reg[8]_i_1_n_13 ;
  wire \test_clk0_cntr_reg[8]_i_1_n_14 ;
  wire \test_clk0_cntr_reg[8]_i_1_n_15 ;
  wire \test_clk0_cntr_reg[8]_i_1_n_2 ;
  wire \test_clk0_cntr_reg[8]_i_1_n_3 ;
  wire \test_clk0_cntr_reg[8]_i_1_n_4 ;
  wire \test_clk0_cntr_reg[8]_i_1_n_5 ;
  wire \test_clk0_cntr_reg[8]_i_1_n_6 ;
  wire \test_clk0_cntr_reg[8]_i_1_n_7 ;
  wire \test_clk0_cntr_reg[8]_i_1_n_8 ;
  wire \test_clk0_cntr_reg[8]_i_1_n_9 ;
  wire [31:0]test_clk0_cntr_synced;
  wire test_clk1;
  wire \test_clk1_cntr[0]_i_1_n_0 ;
  wire \test_clk1_cntr[0]_i_3_n_0 ;
  wire [31:0]test_clk1_cntr_reg;
  wire \test_clk1_cntr_reg[0]_i_2_n_0 ;
  wire \test_clk1_cntr_reg[0]_i_2_n_1 ;
  wire \test_clk1_cntr_reg[0]_i_2_n_10 ;
  wire \test_clk1_cntr_reg[0]_i_2_n_11 ;
  wire \test_clk1_cntr_reg[0]_i_2_n_12 ;
  wire \test_clk1_cntr_reg[0]_i_2_n_13 ;
  wire \test_clk1_cntr_reg[0]_i_2_n_14 ;
  wire \test_clk1_cntr_reg[0]_i_2_n_15 ;
  wire \test_clk1_cntr_reg[0]_i_2_n_2 ;
  wire \test_clk1_cntr_reg[0]_i_2_n_3 ;
  wire \test_clk1_cntr_reg[0]_i_2_n_4 ;
  wire \test_clk1_cntr_reg[0]_i_2_n_5 ;
  wire \test_clk1_cntr_reg[0]_i_2_n_6 ;
  wire \test_clk1_cntr_reg[0]_i_2_n_7 ;
  wire \test_clk1_cntr_reg[0]_i_2_n_8 ;
  wire \test_clk1_cntr_reg[0]_i_2_n_9 ;
  wire \test_clk1_cntr_reg[16]_i_1_n_0 ;
  wire \test_clk1_cntr_reg[16]_i_1_n_1 ;
  wire \test_clk1_cntr_reg[16]_i_1_n_10 ;
  wire \test_clk1_cntr_reg[16]_i_1_n_11 ;
  wire \test_clk1_cntr_reg[16]_i_1_n_12 ;
  wire \test_clk1_cntr_reg[16]_i_1_n_13 ;
  wire \test_clk1_cntr_reg[16]_i_1_n_14 ;
  wire \test_clk1_cntr_reg[16]_i_1_n_15 ;
  wire \test_clk1_cntr_reg[16]_i_1_n_2 ;
  wire \test_clk1_cntr_reg[16]_i_1_n_3 ;
  wire \test_clk1_cntr_reg[16]_i_1_n_4 ;
  wire \test_clk1_cntr_reg[16]_i_1_n_5 ;
  wire \test_clk1_cntr_reg[16]_i_1_n_6 ;
  wire \test_clk1_cntr_reg[16]_i_1_n_7 ;
  wire \test_clk1_cntr_reg[16]_i_1_n_8 ;
  wire \test_clk1_cntr_reg[16]_i_1_n_9 ;
  wire \test_clk1_cntr_reg[24]_i_1_n_1 ;
  wire \test_clk1_cntr_reg[24]_i_1_n_10 ;
  wire \test_clk1_cntr_reg[24]_i_1_n_11 ;
  wire \test_clk1_cntr_reg[24]_i_1_n_12 ;
  wire \test_clk1_cntr_reg[24]_i_1_n_13 ;
  wire \test_clk1_cntr_reg[24]_i_1_n_14 ;
  wire \test_clk1_cntr_reg[24]_i_1_n_15 ;
  wire \test_clk1_cntr_reg[24]_i_1_n_2 ;
  wire \test_clk1_cntr_reg[24]_i_1_n_3 ;
  wire \test_clk1_cntr_reg[24]_i_1_n_4 ;
  wire \test_clk1_cntr_reg[24]_i_1_n_5 ;
  wire \test_clk1_cntr_reg[24]_i_1_n_6 ;
  wire \test_clk1_cntr_reg[24]_i_1_n_7 ;
  wire \test_clk1_cntr_reg[24]_i_1_n_8 ;
  wire \test_clk1_cntr_reg[24]_i_1_n_9 ;
  wire \test_clk1_cntr_reg[8]_i_1_n_0 ;
  wire \test_clk1_cntr_reg[8]_i_1_n_1 ;
  wire \test_clk1_cntr_reg[8]_i_1_n_10 ;
  wire \test_clk1_cntr_reg[8]_i_1_n_11 ;
  wire \test_clk1_cntr_reg[8]_i_1_n_12 ;
  wire \test_clk1_cntr_reg[8]_i_1_n_13 ;
  wire \test_clk1_cntr_reg[8]_i_1_n_14 ;
  wire \test_clk1_cntr_reg[8]_i_1_n_15 ;
  wire \test_clk1_cntr_reg[8]_i_1_n_2 ;
  wire \test_clk1_cntr_reg[8]_i_1_n_3 ;
  wire \test_clk1_cntr_reg[8]_i_1_n_4 ;
  wire \test_clk1_cntr_reg[8]_i_1_n_5 ;
  wire \test_clk1_cntr_reg[8]_i_1_n_6 ;
  wire \test_clk1_cntr_reg[8]_i_1_n_7 ;
  wire \test_clk1_cntr_reg[8]_i_1_n_8 ;
  wire \test_clk1_cntr_reg[8]_i_1_n_9 ;
  wire [31:0]test_clk1_cntr_synced;
  wire user_rst0_ack;
  wire user_rst1_ack;
  wire user_rst_i_1_n_0;
  wire user_rst_reg_n_0;
  wire xpm_cdc_array_single_inst2_i_2_n_0;
  wire xpm_cdc_array_single_inst2_i_3_n_0;
  wire [7:7]\NLW_ref_clk_cntr_reg[24]_i_1_CO_UNCONNECTED ;
  wire [7:7]\NLW_test_clk0_cntr_reg[24]_i_1_CO_UNCONNECTED ;
  wire [7:7]\NLW_test_clk1_cntr_reg[24]_i_1_CO_UNCONNECTED ;

  LUT5 #(
    .INIT(32'hF7F72000)) 
    axil_arready_i_1
       (.I0(reset_n),
        .I1(state_read[1]),
        .I2(state_read[0]),
        .I3(axil_arvalid),
        .I4(axil_arready),
        .O(axil_arready_i_1_n_0));
  FDRE #(
    .INIT(1'b0)) 
    axil_arready_reg
       (.C(clk),
        .CE(1'b1),
        .D(axil_arready_i_1_n_0),
        .Q(axil_arready),
        .R(1'b0));
  LUT6 #(
    .INIT(64'hFFFFDDFF00004000)) 
    axil_awready_i_1
       (.I0(state_write[1]),
        .I1(state_write[0]),
        .I2(axil_awvalid),
        .I3(reset_n),
        .I4(state_write[2]),
        .I5(axil_awready),
        .O(axil_awready_i_1_n_0));
  FDRE #(
    .INIT(1'b0)) 
    axil_awready_reg
       (.C(clk),
        .CE(1'b1),
        .D(axil_awready_i_1_n_0),
        .Q(axil_awready),
        .R(1'b0));
  LUT6 #(
    .INIT(64'h2F2F2FFF20202000)) 
    axil_bvalid_i_1
       (.I0(axil_wvalid),
        .I1(state_write[2]),
        .I2(axil_wready4_out),
        .I3(axil_bready),
        .I4(state_write[1]),
        .I5(axil_bvalid),
        .O(axil_bvalid_i_1_n_0));
  (* SOFT_HLUTNM = "soft_lutpair0" *) 
  LUT5 #(
    .INIT(32'h00002088)) 
    axil_bvalid_i_2
       (.I0(reset_n),
        .I1(state_write[2]),
        .I2(axil_wvalid),
        .I3(state_write[1]),
        .I4(state_write[0]),
        .O(axil_wready4_out));
  FDRE #(
    .INIT(1'b0)) 
    axil_bvalid_reg
       (.C(clk),
        .CE(1'b1),
        .D(axil_bvalid_i_1_n_0),
        .Q(axil_bvalid),
        .R(1'b0));
  LUT6 #(
    .INIT(64'hFFFFFFFFFEAEAAAA)) 
    \axil_rdata[0]_i_1 
       (.I0(\axil_rdata[0]_i_2_n_0 ),
        .I1(test_clk0_cntr_synced[0]),
        .I2(axil_araddr[2]),
        .I3(test_clk1_cntr_synced[0]),
        .I4(axil_araddr[3]),
        .I5(\axil_rdata[0]_i_3_n_0 ),
        .O(axil_rdata_0[0]));
  LUT6 #(
    .INIT(64'h00000000FFE200E2)) 
    \axil_rdata[0]_i_2 
       (.I0(axil_wdata[0]),
        .I1(\axil_rdata[31]_i_2_n_0 ),
        .I2(user_rst_reg_n_0),
        .I3(axil_araddr[2]),
        .I4(ref_clk_cntr_reg[0]),
        .I5(axil_araddr[3]),
        .O(\axil_rdata[0]_i_2_n_0 ));
  LUT2 #(
    .INIT(4'hE)) 
    \axil_rdata[0]_i_3 
       (.I0(axil_araddr[0]),
        .I1(axil_araddr[1]),
        .O(\axil_rdata[0]_i_3_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF40404540)) 
    \axil_rdata[10]_i_1 
       (.I0(axil_araddr[3]),
        .I1(ref_clk_cntr_reg[10]),
        .I2(axil_araddr[2]),
        .I3(axil_wdata[9]),
        .I4(\axil_rdata[31]_i_2_n_0 ),
        .I5(\axil_rdata[10]_i_2_n_0 ),
        .O(axil_rdata_0[10]));
  LUT6 #(
    .INIT(64'hFEEEFEFEFEEEEEEE)) 
    \axil_rdata[10]_i_2 
       (.I0(axil_araddr[1]),
        .I1(axil_araddr[0]),
        .I2(axil_araddr[3]),
        .I3(test_clk1_cntr_synced[10]),
        .I4(axil_araddr[2]),
        .I5(test_clk0_cntr_synced[10]),
        .O(\axil_rdata[10]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF40404540)) 
    \axil_rdata[11]_i_1 
       (.I0(axil_araddr[3]),
        .I1(ref_clk_cntr_reg[11]),
        .I2(axil_araddr[2]),
        .I3(axil_wdata[10]),
        .I4(\axil_rdata[31]_i_2_n_0 ),
        .I5(\axil_rdata[11]_i_2_n_0 ),
        .O(axil_rdata_0[11]));
  LUT6 #(
    .INIT(64'hFEEEFEFEFEEEEEEE)) 
    \axil_rdata[11]_i_2 
       (.I0(axil_araddr[1]),
        .I1(axil_araddr[0]),
        .I2(axil_araddr[3]),
        .I3(test_clk1_cntr_synced[11]),
        .I4(axil_araddr[2]),
        .I5(test_clk0_cntr_synced[11]),
        .O(\axil_rdata[11]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF40404540)) 
    \axil_rdata[12]_i_1 
       (.I0(axil_araddr[3]),
        .I1(ref_clk_cntr_reg[12]),
        .I2(axil_araddr[2]),
        .I3(axil_wdata[11]),
        .I4(\axil_rdata[31]_i_2_n_0 ),
        .I5(\axil_rdata[12]_i_2_n_0 ),
        .O(axil_rdata_0[12]));
  LUT6 #(
    .INIT(64'hFEEEFEFEFEEEEEEE)) 
    \axil_rdata[12]_i_2 
       (.I0(axil_araddr[1]),
        .I1(axil_araddr[0]),
        .I2(axil_araddr[3]),
        .I3(test_clk1_cntr_synced[12]),
        .I4(axil_araddr[2]),
        .I5(test_clk0_cntr_synced[12]),
        .O(\axil_rdata[12]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF40404540)) 
    \axil_rdata[13]_i_1 
       (.I0(axil_araddr[3]),
        .I1(ref_clk_cntr_reg[13]),
        .I2(axil_araddr[2]),
        .I3(axil_wdata[12]),
        .I4(\axil_rdata[31]_i_2_n_0 ),
        .I5(\axil_rdata[13]_i_2_n_0 ),
        .O(axil_rdata_0[13]));
  LUT6 #(
    .INIT(64'hFEEEFEFEFEEEEEEE)) 
    \axil_rdata[13]_i_2 
       (.I0(axil_araddr[1]),
        .I1(axil_araddr[0]),
        .I2(axil_araddr[3]),
        .I3(test_clk1_cntr_synced[13]),
        .I4(axil_araddr[2]),
        .I5(test_clk0_cntr_synced[13]),
        .O(\axil_rdata[13]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \axil_rdata[14]_i_1 
       (.I0(test_clk1_cntr_synced[14]),
        .I1(test_clk0_cntr_synced[14]),
        .I2(axil_araddr[3]),
        .I3(ref_clk_cntr_reg[14]),
        .I4(axil_araddr[2]),
        .I5(\axil_rdata[14]_i_2_n_0 ),
        .O(\axil_rdata[14]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'h0000000000000008)) 
    \axil_rdata[14]_i_2 
       (.I0(axil_wdata[13]),
        .I1(axil_wvalid),
        .I2(axil_awaddr[0]),
        .I3(axil_awaddr[1]),
        .I4(axil_awaddr[2]),
        .I5(axil_awaddr[3]),
        .O(\axil_rdata[14]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF40404540)) 
    \axil_rdata[15]_i_1 
       (.I0(axil_araddr[3]),
        .I1(ref_clk_cntr_reg[15]),
        .I2(axil_araddr[2]),
        .I3(axil_wdata[14]),
        .I4(\axil_rdata[31]_i_2_n_0 ),
        .I5(\axil_rdata[15]_i_2_n_0 ),
        .O(axil_rdata_0[15]));
  LUT6 #(
    .INIT(64'hFEEEFEFEFEEEEEEE)) 
    \axil_rdata[15]_i_2 
       (.I0(axil_araddr[1]),
        .I1(axil_araddr[0]),
        .I2(axil_araddr[3]),
        .I3(test_clk1_cntr_synced[15]),
        .I4(axil_araddr[2]),
        .I5(test_clk0_cntr_synced[15]),
        .O(\axil_rdata[15]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF40404540)) 
    \axil_rdata[16]_i_1 
       (.I0(axil_araddr[3]),
        .I1(ref_clk_cntr_reg[16]),
        .I2(axil_araddr[2]),
        .I3(axil_wdata[15]),
        .I4(\axil_rdata[31]_i_2_n_0 ),
        .I5(\axil_rdata[16]_i_2_n_0 ),
        .O(axil_rdata_0[16]));
  LUT6 #(
    .INIT(64'hFEEEFEFEFEEEEEEE)) 
    \axil_rdata[16]_i_2 
       (.I0(axil_araddr[1]),
        .I1(axil_araddr[0]),
        .I2(axil_araddr[3]),
        .I3(test_clk1_cntr_synced[16]),
        .I4(axil_araddr[2]),
        .I5(test_clk0_cntr_synced[16]),
        .O(\axil_rdata[16]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \axil_rdata[17]_i_1 
       (.I0(test_clk1_cntr_synced[17]),
        .I1(test_clk0_cntr_synced[17]),
        .I2(axil_araddr[3]),
        .I3(ref_clk_cntr_reg[17]),
        .I4(axil_araddr[2]),
        .I5(\axil_rdata[17]_i_2_n_0 ),
        .O(\axil_rdata[17]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'h0000000000000008)) 
    \axil_rdata[17]_i_2 
       (.I0(axil_wdata[16]),
        .I1(axil_wvalid),
        .I2(axil_awaddr[0]),
        .I3(axil_awaddr[1]),
        .I4(axil_awaddr[2]),
        .I5(axil_awaddr[3]),
        .O(\axil_rdata[17]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF40404540)) 
    \axil_rdata[18]_i_1 
       (.I0(axil_araddr[3]),
        .I1(ref_clk_cntr_reg[18]),
        .I2(axil_araddr[2]),
        .I3(axil_wdata[17]),
        .I4(\axil_rdata[31]_i_2_n_0 ),
        .I5(\axil_rdata[18]_i_2_n_0 ),
        .O(axil_rdata_0[18]));
  LUT6 #(
    .INIT(64'hFEEEFEFEFEEEEEEE)) 
    \axil_rdata[18]_i_2 
       (.I0(axil_araddr[1]),
        .I1(axil_araddr[0]),
        .I2(axil_araddr[3]),
        .I3(test_clk1_cntr_synced[18]),
        .I4(axil_araddr[2]),
        .I5(test_clk0_cntr_synced[18]),
        .O(\axil_rdata[18]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF40404540)) 
    \axil_rdata[19]_i_1 
       (.I0(axil_araddr[3]),
        .I1(ref_clk_cntr_reg[19]),
        .I2(axil_araddr[2]),
        .I3(axil_wdata[18]),
        .I4(\axil_rdata[31]_i_2_n_0 ),
        .I5(\axil_rdata[19]_i_2_n_0 ),
        .O(axil_rdata_0[19]));
  LUT6 #(
    .INIT(64'hFEEEFEFEFEEEEEEE)) 
    \axil_rdata[19]_i_2 
       (.I0(axil_araddr[1]),
        .I1(axil_araddr[0]),
        .I2(axil_araddr[3]),
        .I3(test_clk1_cntr_synced[19]),
        .I4(axil_araddr[2]),
        .I5(test_clk0_cntr_synced[19]),
        .O(\axil_rdata[19]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF44444544)) 
    \axil_rdata[1]_i_1 
       (.I0(\axil_rdata[1]_i_2_n_0 ),
        .I1(axil_araddr[2]),
        .I2(\axil_rdata[1]_i_3_n_0 ),
        .I3(\axil_rdata[1]_i_4_n_0 ),
        .I4(\axil_rdata[1]_i_5_n_0 ),
        .I5(\axil_rdata[1]_i_6_n_0 ),
        .O(axil_rdata_0[1]));
  (* SOFT_HLUTNM = "soft_lutpair1" *) 
  LUT4 #(
    .INIT(16'hFFFE)) 
    \axil_rdata[1]_i_10 
       (.I0(ref_clk_cntr_reg[12]),
        .I1(ref_clk_cntr_reg[11]),
        .I2(ref_clk_cntr_reg[13]),
        .I3(ref_clk_cntr_reg[10]),
        .O(\axil_rdata[1]_i_10_n_0 ));
  LUT4 #(
    .INIT(16'hFFFE)) 
    \axil_rdata[1]_i_11 
       (.I0(ref_clk_cntr_reg[24]),
        .I1(ref_clk_cntr_reg[27]),
        .I2(ref_clk_cntr_reg[25]),
        .I3(ref_clk_cntr_reg[26]),
        .O(\axil_rdata[1]_i_11_n_0 ));
  LUT4 #(
    .INIT(16'hFFDF)) 
    \axil_rdata[1]_i_12 
       (.I0(ref_clk_cntr_reg[14]),
        .I1(ref_clk_cntr_reg[1]),
        .I2(ref_clk_cntr_reg[15]),
        .I3(ref_clk_cntr_reg[0]),
        .O(\axil_rdata[1]_i_12_n_0 ));
  LUT3 #(
    .INIT(8'hBA)) 
    \axil_rdata[1]_i_2 
       (.I0(axil_araddr[3]),
        .I1(ref_clk_cntr_reg[1]),
        .I2(axil_araddr[2]),
        .O(\axil_rdata[1]_i_2_n_0 ));
  LUT4 #(
    .INIT(16'hFFFE)) 
    \axil_rdata[1]_i_3 
       (.I0(\axil_rdata[1]_i_7_n_0 ),
        .I1(\axil_rdata[1]_i_8_n_0 ),
        .I2(\axil_rdata[1]_i_9_n_0 ),
        .I3(\axil_rdata[1]_i_10_n_0 ),
        .O(\axil_rdata[1]_i_3_n_0 ));
  LUT5 #(
    .INIT(32'h00008000)) 
    \axil_rdata[1]_i_4 
       (.I0(ref_clk_cntr_reg[8]),
        .I1(ref_clk_cntr_reg[6]),
        .I2(ref_clk_cntr_reg[9]),
        .I3(ref_clk_cntr_reg[4]),
        .I4(\axil_rdata[1]_i_11_n_0 ),
        .O(\axil_rdata[1]_i_4_n_0 ));
  LUT5 #(
    .INIT(32'hFFFFFFFE)) 
    \axil_rdata[1]_i_5 
       (.I0(ref_clk_cntr_reg[28]),
        .I1(ref_clk_cntr_reg[30]),
        .I2(ref_clk_cntr_reg[29]),
        .I3(ref_clk_cntr_reg[31]),
        .I4(\axil_rdata[1]_i_12_n_0 ),
        .O(\axil_rdata[1]_i_5_n_0 ));
  LUT6 #(
    .INIT(64'hFEEEFEFEFEEEEEEE)) 
    \axil_rdata[1]_i_6 
       (.I0(axil_araddr[1]),
        .I1(axil_araddr[0]),
        .I2(axil_araddr[3]),
        .I3(test_clk1_cntr_synced[1]),
        .I4(axil_araddr[2]),
        .I5(test_clk0_cntr_synced[1]),
        .O(\axil_rdata[1]_i_6_n_0 ));
  LUT4 #(
    .INIT(16'hFFFE)) 
    \axil_rdata[1]_i_7 
       (.I0(ref_clk_cntr_reg[2]),
        .I1(ref_clk_cntr_reg[7]),
        .I2(ref_clk_cntr_reg[3]),
        .I3(ref_clk_cntr_reg[5]),
        .O(\axil_rdata[1]_i_7_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair2" *) 
  LUT4 #(
    .INIT(16'hFFFE)) 
    \axil_rdata[1]_i_8 
       (.I0(ref_clk_cntr_reg[22]),
        .I1(ref_clk_cntr_reg[21]),
        .I2(ref_clk_cntr_reg[23]),
        .I3(ref_clk_cntr_reg[20]),
        .O(\axil_rdata[1]_i_8_n_0 ));
  LUT4 #(
    .INIT(16'hFFFE)) 
    \axil_rdata[1]_i_9 
       (.I0(ref_clk_cntr_reg[18]),
        .I1(ref_clk_cntr_reg[17]),
        .I2(ref_clk_cntr_reg[19]),
        .I3(ref_clk_cntr_reg[16]),
        .O(\axil_rdata[1]_i_9_n_0 ));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \axil_rdata[20]_i_1 
       (.I0(test_clk1_cntr_synced[20]),
        .I1(test_clk0_cntr_synced[20]),
        .I2(axil_araddr[3]),
        .I3(ref_clk_cntr_reg[20]),
        .I4(axil_araddr[2]),
        .I5(\axil_rdata[20]_i_2_n_0 ),
        .O(\axil_rdata[20]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'h0000000000000008)) 
    \axil_rdata[20]_i_2 
       (.I0(axil_wdata[19]),
        .I1(axil_wvalid),
        .I2(axil_awaddr[0]),
        .I3(axil_awaddr[1]),
        .I4(axil_awaddr[2]),
        .I5(axil_awaddr[3]),
        .O(\axil_rdata[20]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF40404540)) 
    \axil_rdata[21]_i_1 
       (.I0(axil_araddr[3]),
        .I1(ref_clk_cntr_reg[21]),
        .I2(axil_araddr[2]),
        .I3(axil_wdata[20]),
        .I4(\axil_rdata[31]_i_2_n_0 ),
        .I5(\axil_rdata[21]_i_2_n_0 ),
        .O(axil_rdata_0[21]));
  LUT6 #(
    .INIT(64'hFEEEFEFEFEEEEEEE)) 
    \axil_rdata[21]_i_2 
       (.I0(axil_araddr[1]),
        .I1(axil_araddr[0]),
        .I2(axil_araddr[3]),
        .I3(test_clk1_cntr_synced[21]),
        .I4(axil_araddr[2]),
        .I5(test_clk0_cntr_synced[21]),
        .O(\axil_rdata[21]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \axil_rdata[22]_i_1 
       (.I0(test_clk1_cntr_synced[22]),
        .I1(test_clk0_cntr_synced[22]),
        .I2(axil_araddr[3]),
        .I3(ref_clk_cntr_reg[22]),
        .I4(axil_araddr[2]),
        .I5(\axil_rdata[22]_i_2_n_0 ),
        .O(\axil_rdata[22]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'h0000000000000008)) 
    \axil_rdata[22]_i_2 
       (.I0(axil_wdata[21]),
        .I1(axil_wvalid),
        .I2(axil_awaddr[0]),
        .I3(axil_awaddr[1]),
        .I4(axil_awaddr[2]),
        .I5(axil_awaddr[3]),
        .O(\axil_rdata[22]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF40404540)) 
    \axil_rdata[23]_i_1 
       (.I0(axil_araddr[3]),
        .I1(ref_clk_cntr_reg[23]),
        .I2(axil_araddr[2]),
        .I3(axil_wdata[22]),
        .I4(\axil_rdata[31]_i_2_n_0 ),
        .I5(\axil_rdata[23]_i_2_n_0 ),
        .O(axil_rdata_0[23]));
  LUT6 #(
    .INIT(64'hFEEEFEFEFEEEEEEE)) 
    \axil_rdata[23]_i_2 
       (.I0(axil_araddr[1]),
        .I1(axil_araddr[0]),
        .I2(axil_araddr[3]),
        .I3(test_clk1_cntr_synced[23]),
        .I4(axil_araddr[2]),
        .I5(test_clk0_cntr_synced[23]),
        .O(\axil_rdata[23]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \axil_rdata[24]_i_1 
       (.I0(test_clk1_cntr_synced[24]),
        .I1(test_clk0_cntr_synced[24]),
        .I2(axil_araddr[3]),
        .I3(ref_clk_cntr_reg[24]),
        .I4(axil_araddr[2]),
        .I5(\axil_rdata[24]_i_2_n_0 ),
        .O(\axil_rdata[24]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'h0000000000000008)) 
    \axil_rdata[24]_i_2 
       (.I0(axil_wdata[23]),
        .I1(axil_wvalid),
        .I2(axil_awaddr[0]),
        .I3(axil_awaddr[1]),
        .I4(axil_awaddr[2]),
        .I5(axil_awaddr[3]),
        .O(\axil_rdata[24]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF40404540)) 
    \axil_rdata[25]_i_1 
       (.I0(axil_araddr[3]),
        .I1(ref_clk_cntr_reg[25]),
        .I2(axil_araddr[2]),
        .I3(axil_wdata[24]),
        .I4(\axil_rdata[31]_i_2_n_0 ),
        .I5(\axil_rdata[25]_i_2_n_0 ),
        .O(axil_rdata_0[25]));
  LUT6 #(
    .INIT(64'hFEEEFEFEFEEEEEEE)) 
    \axil_rdata[25]_i_2 
       (.I0(axil_araddr[1]),
        .I1(axil_araddr[0]),
        .I2(axil_araddr[3]),
        .I3(test_clk1_cntr_synced[25]),
        .I4(axil_araddr[2]),
        .I5(test_clk0_cntr_synced[25]),
        .O(\axil_rdata[25]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF40404540)) 
    \axil_rdata[26]_i_1 
       (.I0(axil_araddr[3]),
        .I1(ref_clk_cntr_reg[26]),
        .I2(axil_araddr[2]),
        .I3(axil_wdata[25]),
        .I4(\axil_rdata[31]_i_2_n_0 ),
        .I5(\axil_rdata[26]_i_2_n_0 ),
        .O(axil_rdata_0[26]));
  LUT6 #(
    .INIT(64'hFEEEFEFEFEEEEEEE)) 
    \axil_rdata[26]_i_2 
       (.I0(axil_araddr[1]),
        .I1(axil_araddr[0]),
        .I2(axil_araddr[3]),
        .I3(test_clk1_cntr_synced[26]),
        .I4(axil_araddr[2]),
        .I5(test_clk0_cntr_synced[26]),
        .O(\axil_rdata[26]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF40404540)) 
    \axil_rdata[27]_i_1 
       (.I0(axil_araddr[3]),
        .I1(ref_clk_cntr_reg[27]),
        .I2(axil_araddr[2]),
        .I3(axil_wdata[26]),
        .I4(\axil_rdata[31]_i_2_n_0 ),
        .I5(\axil_rdata[27]_i_2_n_0 ),
        .O(axil_rdata_0[27]));
  LUT6 #(
    .INIT(64'hFEEEFEFEFEEEEEEE)) 
    \axil_rdata[27]_i_2 
       (.I0(axil_araddr[1]),
        .I1(axil_araddr[0]),
        .I2(axil_araddr[3]),
        .I3(test_clk1_cntr_synced[27]),
        .I4(axil_araddr[2]),
        .I5(test_clk0_cntr_synced[27]),
        .O(\axil_rdata[27]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF40404540)) 
    \axil_rdata[28]_i_1 
       (.I0(axil_araddr[3]),
        .I1(ref_clk_cntr_reg[28]),
        .I2(axil_araddr[2]),
        .I3(axil_wdata[27]),
        .I4(\axil_rdata[31]_i_2_n_0 ),
        .I5(\axil_rdata[28]_i_2_n_0 ),
        .O(axil_rdata_0[28]));
  LUT6 #(
    .INIT(64'hFEEEFEFEFEEEEEEE)) 
    \axil_rdata[28]_i_2 
       (.I0(axil_araddr[1]),
        .I1(axil_araddr[0]),
        .I2(axil_araddr[3]),
        .I3(test_clk1_cntr_synced[28]),
        .I4(axil_araddr[2]),
        .I5(test_clk0_cntr_synced[28]),
        .O(\axil_rdata[28]_i_2_n_0 ));
  LUT3 #(
    .INIT(8'hE0)) 
    \axil_rdata[29]_i_1 
       (.I0(axil_araddr[1]),
        .I1(axil_araddr[0]),
        .I2(axil_arvalid),
        .O(\axil_rdata[29]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \axil_rdata[29]_i_2 
       (.I0(test_clk1_cntr_synced[29]),
        .I1(test_clk0_cntr_synced[29]),
        .I2(axil_araddr[3]),
        .I3(ref_clk_cntr_reg[29]),
        .I4(axil_araddr[2]),
        .I5(\axil_rdata[29]_i_3_n_0 ),
        .O(\axil_rdata[29]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'h0000000000000008)) 
    \axil_rdata[29]_i_3 
       (.I0(axil_wdata[28]),
        .I1(axil_wvalid),
        .I2(axil_awaddr[0]),
        .I3(axil_awaddr[1]),
        .I4(axil_awaddr[2]),
        .I5(axil_awaddr[3]),
        .O(\axil_rdata[29]_i_3_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF40404540)) 
    \axil_rdata[2]_i_1 
       (.I0(axil_araddr[3]),
        .I1(ref_clk_cntr_reg[2]),
        .I2(axil_araddr[2]),
        .I3(axil_wdata[1]),
        .I4(\axil_rdata[31]_i_2_n_0 ),
        .I5(\axil_rdata[2]_i_2_n_0 ),
        .O(axil_rdata_0[2]));
  LUT6 #(
    .INIT(64'hFEEEFEFEFEEEEEEE)) 
    \axil_rdata[2]_i_2 
       (.I0(axil_araddr[1]),
        .I1(axil_araddr[0]),
        .I2(axil_araddr[3]),
        .I3(test_clk1_cntr_synced[2]),
        .I4(axil_araddr[2]),
        .I5(test_clk0_cntr_synced[2]),
        .O(\axil_rdata[2]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF40404540)) 
    \axil_rdata[30]_i_1 
       (.I0(axil_araddr[3]),
        .I1(ref_clk_cntr_reg[30]),
        .I2(axil_araddr[2]),
        .I3(axil_wdata[29]),
        .I4(\axil_rdata[31]_i_2_n_0 ),
        .I5(\axil_rdata[30]_i_2_n_0 ),
        .O(axil_rdata_0[30]));
  LUT6 #(
    .INIT(64'hFEEEFEFEFEEEEEEE)) 
    \axil_rdata[30]_i_2 
       (.I0(axil_araddr[1]),
        .I1(axil_araddr[0]),
        .I2(axil_araddr[3]),
        .I3(test_clk1_cntr_synced[30]),
        .I4(axil_araddr[2]),
        .I5(test_clk0_cntr_synced[30]),
        .O(\axil_rdata[30]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF40404540)) 
    \axil_rdata[31]_i_1 
       (.I0(axil_araddr[3]),
        .I1(ref_clk_cntr_reg[31]),
        .I2(axil_araddr[2]),
        .I3(axil_wdata[30]),
        .I4(\axil_rdata[31]_i_2_n_0 ),
        .I5(\axil_rdata[31]_i_3_n_0 ),
        .O(axil_rdata_0[31]));
  LUT5 #(
    .INIT(32'hFFFEFFFF)) 
    \axil_rdata[31]_i_2 
       (.I0(axil_awaddr[3]),
        .I1(axil_awaddr[2]),
        .I2(axil_awaddr[1]),
        .I3(axil_awaddr[0]),
        .I4(axil_wvalid),
        .O(\axil_rdata[31]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFFFF8A80)) 
    \axil_rdata[31]_i_3 
       (.I0(axil_araddr[3]),
        .I1(test_clk1_cntr_synced[31]),
        .I2(axil_araddr[2]),
        .I3(test_clk0_cntr_synced[31]),
        .I4(axil_araddr[1]),
        .I5(axil_araddr[0]),
        .O(\axil_rdata[31]_i_3_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF40404540)) 
    \axil_rdata[3]_i_1 
       (.I0(axil_araddr[3]),
        .I1(ref_clk_cntr_reg[3]),
        .I2(axil_araddr[2]),
        .I3(axil_wdata[2]),
        .I4(\axil_rdata[31]_i_2_n_0 ),
        .I5(\axil_rdata[3]_i_2_n_0 ),
        .O(axil_rdata_0[3]));
  LUT6 #(
    .INIT(64'hFEEEFEFEFEEEEEEE)) 
    \axil_rdata[3]_i_2 
       (.I0(axil_araddr[1]),
        .I1(axil_araddr[0]),
        .I2(axil_araddr[3]),
        .I3(test_clk1_cntr_synced[3]),
        .I4(axil_araddr[2]),
        .I5(test_clk0_cntr_synced[3]),
        .O(\axil_rdata[3]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \axil_rdata[4]_i_1 
       (.I0(test_clk1_cntr_synced[4]),
        .I1(test_clk0_cntr_synced[4]),
        .I2(axil_araddr[3]),
        .I3(ref_clk_cntr_reg[4]),
        .I4(axil_araddr[2]),
        .I5(\axil_rdata[4]_i_2_n_0 ),
        .O(\axil_rdata[4]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'h0000000000000008)) 
    \axil_rdata[4]_i_2 
       (.I0(axil_wdata[3]),
        .I1(axil_wvalid),
        .I2(axil_awaddr[0]),
        .I3(axil_awaddr[1]),
        .I4(axil_awaddr[2]),
        .I5(axil_awaddr[3]),
        .O(\axil_rdata[4]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF40404540)) 
    \axil_rdata[5]_i_1 
       (.I0(axil_araddr[3]),
        .I1(ref_clk_cntr_reg[5]),
        .I2(axil_araddr[2]),
        .I3(axil_wdata[4]),
        .I4(\axil_rdata[31]_i_2_n_0 ),
        .I5(\axil_rdata[5]_i_2_n_0 ),
        .O(axil_rdata_0[5]));
  LUT6 #(
    .INIT(64'hFEEEFEFEFEEEEEEE)) 
    \axil_rdata[5]_i_2 
       (.I0(axil_araddr[1]),
        .I1(axil_araddr[0]),
        .I2(axil_araddr[3]),
        .I3(test_clk1_cntr_synced[5]),
        .I4(axil_araddr[2]),
        .I5(test_clk0_cntr_synced[5]),
        .O(\axil_rdata[5]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF40404540)) 
    \axil_rdata[6]_i_1 
       (.I0(axil_araddr[3]),
        .I1(ref_clk_cntr_reg[6]),
        .I2(axil_araddr[2]),
        .I3(axil_wdata[5]),
        .I4(\axil_rdata[31]_i_2_n_0 ),
        .I5(\axil_rdata[6]_i_2_n_0 ),
        .O(axil_rdata_0[6]));
  LUT6 #(
    .INIT(64'hFEEEFEFEFEEEEEEE)) 
    \axil_rdata[6]_i_2 
       (.I0(axil_araddr[1]),
        .I1(axil_araddr[0]),
        .I2(axil_araddr[3]),
        .I3(test_clk1_cntr_synced[6]),
        .I4(axil_araddr[2]),
        .I5(test_clk0_cntr_synced[6]),
        .O(\axil_rdata[6]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF40404540)) 
    \axil_rdata[7]_i_1 
       (.I0(axil_araddr[3]),
        .I1(ref_clk_cntr_reg[7]),
        .I2(axil_araddr[2]),
        .I3(axil_wdata[6]),
        .I4(\axil_rdata[31]_i_2_n_0 ),
        .I5(\axil_rdata[7]_i_2_n_0 ),
        .O(axil_rdata_0[7]));
  LUT6 #(
    .INIT(64'hFEEEFEFEFEEEEEEE)) 
    \axil_rdata[7]_i_2 
       (.I0(axil_araddr[1]),
        .I1(axil_araddr[0]),
        .I2(axil_araddr[3]),
        .I3(test_clk1_cntr_synced[7]),
        .I4(axil_araddr[2]),
        .I5(test_clk0_cntr_synced[7]),
        .O(\axil_rdata[7]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAFA0CFCFAFA0C0C0)) 
    \axil_rdata[8]_i_1 
       (.I0(test_clk1_cntr_synced[8]),
        .I1(test_clk0_cntr_synced[8]),
        .I2(axil_araddr[3]),
        .I3(ref_clk_cntr_reg[8]),
        .I4(axil_araddr[2]),
        .I5(\axil_rdata[8]_i_2_n_0 ),
        .O(\axil_rdata[8]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'h0000000000000008)) 
    \axil_rdata[8]_i_2 
       (.I0(axil_wdata[7]),
        .I1(axil_wvalid),
        .I2(axil_awaddr[0]),
        .I3(axil_awaddr[1]),
        .I4(axil_awaddr[2]),
        .I5(axil_awaddr[3]),
        .O(\axil_rdata[8]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF40404540)) 
    \axil_rdata[9]_i_1 
       (.I0(axil_araddr[3]),
        .I1(ref_clk_cntr_reg[9]),
        .I2(axil_araddr[2]),
        .I3(axil_wdata[8]),
        .I4(\axil_rdata[31]_i_2_n_0 ),
        .I5(\axil_rdata[9]_i_2_n_0 ),
        .O(axil_rdata_0[9]));
  LUT6 #(
    .INIT(64'hFEEEFEFEFEEEEEEE)) 
    \axil_rdata[9]_i_2 
       (.I0(axil_araddr[1]),
        .I1(axil_araddr[0]),
        .I2(axil_araddr[3]),
        .I3(test_clk1_cntr_synced[9]),
        .I4(axil_araddr[2]),
        .I5(test_clk0_cntr_synced[9]),
        .O(\axil_rdata[9]_i_2_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[0] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(axil_rdata_0[0]),
        .Q(axil_rdata[0]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[10] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(axil_rdata_0[10]),
        .Q(axil_rdata[10]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[11] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(axil_rdata_0[11]),
        .Q(axil_rdata[11]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[12] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(axil_rdata_0[12]),
        .Q(axil_rdata[12]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[13] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(axil_rdata_0[13]),
        .Q(axil_rdata[13]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[14] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(\axil_rdata[14]_i_1_n_0 ),
        .Q(axil_rdata[14]),
        .R(\axil_rdata[29]_i_1_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[15] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(axil_rdata_0[15]),
        .Q(axil_rdata[15]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[16] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(axil_rdata_0[16]),
        .Q(axil_rdata[16]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[17] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(\axil_rdata[17]_i_1_n_0 ),
        .Q(axil_rdata[17]),
        .R(\axil_rdata[29]_i_1_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[18] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(axil_rdata_0[18]),
        .Q(axil_rdata[18]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[19] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(axil_rdata_0[19]),
        .Q(axil_rdata[19]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[1] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(axil_rdata_0[1]),
        .Q(axil_rdata[1]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[20] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(\axil_rdata[20]_i_1_n_0 ),
        .Q(axil_rdata[20]),
        .R(\axil_rdata[29]_i_1_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[21] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(axil_rdata_0[21]),
        .Q(axil_rdata[21]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[22] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(\axil_rdata[22]_i_1_n_0 ),
        .Q(axil_rdata[22]),
        .R(\axil_rdata[29]_i_1_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[23] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(axil_rdata_0[23]),
        .Q(axil_rdata[23]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[24] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(\axil_rdata[24]_i_1_n_0 ),
        .Q(axil_rdata[24]),
        .R(\axil_rdata[29]_i_1_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[25] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(axil_rdata_0[25]),
        .Q(axil_rdata[25]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[26] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(axil_rdata_0[26]),
        .Q(axil_rdata[26]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[27] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(axil_rdata_0[27]),
        .Q(axil_rdata[27]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[28] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(axil_rdata_0[28]),
        .Q(axil_rdata[28]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[29] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(\axil_rdata[29]_i_2_n_0 ),
        .Q(axil_rdata[29]),
        .R(\axil_rdata[29]_i_1_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[2] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(axil_rdata_0[2]),
        .Q(axil_rdata[2]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[30] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(axil_rdata_0[30]),
        .Q(axil_rdata[30]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[31] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(axil_rdata_0[31]),
        .Q(axil_rdata[31]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[3] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(axil_rdata_0[3]),
        .Q(axil_rdata[3]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[4] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(\axil_rdata[4]_i_1_n_0 ),
        .Q(axil_rdata[4]),
        .R(\axil_rdata[29]_i_1_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[5] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(axil_rdata_0[5]),
        .Q(axil_rdata[5]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[6] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(axil_rdata_0[6]),
        .Q(axil_rdata[6]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[7] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(axil_rdata_0[7]),
        .Q(axil_rdata[7]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[8] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(\axil_rdata[8]_i_1_n_0 ),
        .Q(axil_rdata[8]),
        .R(\axil_rdata[29]_i_1_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axil_rdata_reg[9] 
       (.C(clk),
        .CE(axil_arvalid),
        .D(axil_rdata_0[9]),
        .Q(axil_rdata[9]),
        .R(1'b0));
  LUT6 #(
    .INIT(64'hDDFFFFFF00800080)) 
    axil_rvalid_i_1
       (.I0(reset_n),
        .I1(state_read[0]),
        .I2(axil_arvalid),
        .I3(state_read[1]),
        .I4(axil_rready),
        .I5(axil_rvalid),
        .O(axil_rvalid_i_1_n_0));
  FDRE #(
    .INIT(1'b0)) 
    axil_rvalid_reg
       (.C(clk),
        .CE(1'b1),
        .D(axil_rvalid_i_1_n_0),
        .Q(axil_rvalid),
        .R(1'b0));
  LUT6 #(
    .INIT(64'hFFFFFF7700002000)) 
    axil_wready_i_1
       (.I0(reset_n),
        .I1(state_write[2]),
        .I2(axil_wvalid),
        .I3(state_write[1]),
        .I4(state_write[0]),
        .I5(axil_wready),
        .O(axil_wready_i_1_n_0));
  FDRE #(
    .INIT(1'b0)) 
    axil_wready_reg
       (.C(clk),
        .CE(1'b1),
        .D(axil_wready_i_1_n_0),
        .Q(axil_wready),
        .R(1'b0));
  LUT2 #(
    .INIT(4'h8)) 
    \clear_user_rst[0]_i_1 
       (.I0(user_rst1_ack),
        .I1(user_rst0_ack),
        .O(p_1_out));
  FDRE \clear_user_rst_reg[0] 
       (.C(clk),
        .CE(1'b1),
        .D(p_1_out),
        .Q(\clear_user_rst_reg_n_0_[0] ),
        .R(1'b0));
  FDRE \clear_user_rst_reg[1] 
       (.C(clk),
        .CE(1'b1),
        .D(\clear_user_rst_reg_n_0_[0] ),
        .Q(p_1_in),
        .R(1'b0));
  LUT4 #(
    .INIT(16'hFFEF)) 
    \ref_clk_cntr[0]_i_1 
       (.I0(xpm_cdc_array_single_inst2_i_3_n_0),
        .I1(xpm_cdc_array_single_inst2_i_2_n_0),
        .I2(\axil_rdata[1]_i_4_n_0 ),
        .I3(\axil_rdata[1]_i_5_n_0 ),
        .O(sel));
  LUT1 #(
    .INIT(2'h1)) 
    \ref_clk_cntr[0]_i_3 
       (.I0(ref_clk_cntr_reg[0]),
        .O(\ref_clk_cntr[0]_i_3_n_0 ));
  FDRE \ref_clk_cntr_reg[0] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[0]_i_2_n_15 ),
        .Q(ref_clk_cntr_reg[0]),
        .R(src_in0));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ref_clk_cntr_reg[0]_i_2 
       (.CI(1'b0),
        .CI_TOP(1'b0),
        .CO({\ref_clk_cntr_reg[0]_i_2_n_0 ,\ref_clk_cntr_reg[0]_i_2_n_1 ,\ref_clk_cntr_reg[0]_i_2_n_2 ,\ref_clk_cntr_reg[0]_i_2_n_3 ,\ref_clk_cntr_reg[0]_i_2_n_4 ,\ref_clk_cntr_reg[0]_i_2_n_5 ,\ref_clk_cntr_reg[0]_i_2_n_6 ,\ref_clk_cntr_reg[0]_i_2_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b1}),
        .O({\ref_clk_cntr_reg[0]_i_2_n_8 ,\ref_clk_cntr_reg[0]_i_2_n_9 ,\ref_clk_cntr_reg[0]_i_2_n_10 ,\ref_clk_cntr_reg[0]_i_2_n_11 ,\ref_clk_cntr_reg[0]_i_2_n_12 ,\ref_clk_cntr_reg[0]_i_2_n_13 ,\ref_clk_cntr_reg[0]_i_2_n_14 ,\ref_clk_cntr_reg[0]_i_2_n_15 }),
        .S({ref_clk_cntr_reg[7:1],\ref_clk_cntr[0]_i_3_n_0 }));
  FDRE \ref_clk_cntr_reg[10] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[8]_i_1_n_13 ),
        .Q(ref_clk_cntr_reg[10]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[11] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[8]_i_1_n_12 ),
        .Q(ref_clk_cntr_reg[11]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[12] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[8]_i_1_n_11 ),
        .Q(ref_clk_cntr_reg[12]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[13] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[8]_i_1_n_10 ),
        .Q(ref_clk_cntr_reg[13]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[14] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[8]_i_1_n_9 ),
        .Q(ref_clk_cntr_reg[14]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[15] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[8]_i_1_n_8 ),
        .Q(ref_clk_cntr_reg[15]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[16] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[16]_i_1_n_15 ),
        .Q(ref_clk_cntr_reg[16]),
        .R(src_in0));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ref_clk_cntr_reg[16]_i_1 
       (.CI(\ref_clk_cntr_reg[8]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ref_clk_cntr_reg[16]_i_1_n_0 ,\ref_clk_cntr_reg[16]_i_1_n_1 ,\ref_clk_cntr_reg[16]_i_1_n_2 ,\ref_clk_cntr_reg[16]_i_1_n_3 ,\ref_clk_cntr_reg[16]_i_1_n_4 ,\ref_clk_cntr_reg[16]_i_1_n_5 ,\ref_clk_cntr_reg[16]_i_1_n_6 ,\ref_clk_cntr_reg[16]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\ref_clk_cntr_reg[16]_i_1_n_8 ,\ref_clk_cntr_reg[16]_i_1_n_9 ,\ref_clk_cntr_reg[16]_i_1_n_10 ,\ref_clk_cntr_reg[16]_i_1_n_11 ,\ref_clk_cntr_reg[16]_i_1_n_12 ,\ref_clk_cntr_reg[16]_i_1_n_13 ,\ref_clk_cntr_reg[16]_i_1_n_14 ,\ref_clk_cntr_reg[16]_i_1_n_15 }),
        .S(ref_clk_cntr_reg[23:16]));
  FDRE \ref_clk_cntr_reg[17] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[16]_i_1_n_14 ),
        .Q(ref_clk_cntr_reg[17]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[18] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[16]_i_1_n_13 ),
        .Q(ref_clk_cntr_reg[18]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[19] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[16]_i_1_n_12 ),
        .Q(ref_clk_cntr_reg[19]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[1] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[0]_i_2_n_14 ),
        .Q(ref_clk_cntr_reg[1]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[20] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[16]_i_1_n_11 ),
        .Q(ref_clk_cntr_reg[20]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[21] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[16]_i_1_n_10 ),
        .Q(ref_clk_cntr_reg[21]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[22] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[16]_i_1_n_9 ),
        .Q(ref_clk_cntr_reg[22]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[23] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[16]_i_1_n_8 ),
        .Q(ref_clk_cntr_reg[23]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[24] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[24]_i_1_n_15 ),
        .Q(ref_clk_cntr_reg[24]),
        .R(src_in0));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ref_clk_cntr_reg[24]_i_1 
       (.CI(\ref_clk_cntr_reg[16]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\NLW_ref_clk_cntr_reg[24]_i_1_CO_UNCONNECTED [7],\ref_clk_cntr_reg[24]_i_1_n_1 ,\ref_clk_cntr_reg[24]_i_1_n_2 ,\ref_clk_cntr_reg[24]_i_1_n_3 ,\ref_clk_cntr_reg[24]_i_1_n_4 ,\ref_clk_cntr_reg[24]_i_1_n_5 ,\ref_clk_cntr_reg[24]_i_1_n_6 ,\ref_clk_cntr_reg[24]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\ref_clk_cntr_reg[24]_i_1_n_8 ,\ref_clk_cntr_reg[24]_i_1_n_9 ,\ref_clk_cntr_reg[24]_i_1_n_10 ,\ref_clk_cntr_reg[24]_i_1_n_11 ,\ref_clk_cntr_reg[24]_i_1_n_12 ,\ref_clk_cntr_reg[24]_i_1_n_13 ,\ref_clk_cntr_reg[24]_i_1_n_14 ,\ref_clk_cntr_reg[24]_i_1_n_15 }),
        .S(ref_clk_cntr_reg[31:24]));
  FDRE \ref_clk_cntr_reg[25] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[24]_i_1_n_14 ),
        .Q(ref_clk_cntr_reg[25]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[26] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[24]_i_1_n_13 ),
        .Q(ref_clk_cntr_reg[26]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[27] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[24]_i_1_n_12 ),
        .Q(ref_clk_cntr_reg[27]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[28] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[24]_i_1_n_11 ),
        .Q(ref_clk_cntr_reg[28]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[29] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[24]_i_1_n_10 ),
        .Q(ref_clk_cntr_reg[29]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[2] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[0]_i_2_n_13 ),
        .Q(ref_clk_cntr_reg[2]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[30] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[24]_i_1_n_9 ),
        .Q(ref_clk_cntr_reg[30]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[31] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[24]_i_1_n_8 ),
        .Q(ref_clk_cntr_reg[31]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[3] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[0]_i_2_n_12 ),
        .Q(ref_clk_cntr_reg[3]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[4] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[0]_i_2_n_11 ),
        .Q(ref_clk_cntr_reg[4]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[5] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[0]_i_2_n_10 ),
        .Q(ref_clk_cntr_reg[5]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[6] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[0]_i_2_n_9 ),
        .Q(ref_clk_cntr_reg[6]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[7] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[0]_i_2_n_8 ),
        .Q(ref_clk_cntr_reg[7]),
        .R(src_in0));
  FDRE \ref_clk_cntr_reg[8] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[8]_i_1_n_15 ),
        .Q(ref_clk_cntr_reg[8]),
        .R(src_in0));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ref_clk_cntr_reg[8]_i_1 
       (.CI(\ref_clk_cntr_reg[0]_i_2_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ref_clk_cntr_reg[8]_i_1_n_0 ,\ref_clk_cntr_reg[8]_i_1_n_1 ,\ref_clk_cntr_reg[8]_i_1_n_2 ,\ref_clk_cntr_reg[8]_i_1_n_3 ,\ref_clk_cntr_reg[8]_i_1_n_4 ,\ref_clk_cntr_reg[8]_i_1_n_5 ,\ref_clk_cntr_reg[8]_i_1_n_6 ,\ref_clk_cntr_reg[8]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\ref_clk_cntr_reg[8]_i_1_n_8 ,\ref_clk_cntr_reg[8]_i_1_n_9 ,\ref_clk_cntr_reg[8]_i_1_n_10 ,\ref_clk_cntr_reg[8]_i_1_n_11 ,\ref_clk_cntr_reg[8]_i_1_n_12 ,\ref_clk_cntr_reg[8]_i_1_n_13 ,\ref_clk_cntr_reg[8]_i_1_n_14 ,\ref_clk_cntr_reg[8]_i_1_n_15 }),
        .S(ref_clk_cntr_reg[15:8]));
  FDRE \ref_clk_cntr_reg[9] 
       (.C(clk),
        .CE(sel),
        .D(\ref_clk_cntr_reg[8]_i_1_n_14 ),
        .Q(ref_clk_cntr_reg[9]),
        .R(src_in0));
  LUT4 #(
    .INIT(16'h5808)) 
    \state_read[1]_i_1 
       (.I0(state_read[0]),
        .I1(axil_arvalid),
        .I2(state_read[1]),
        .I3(axil_rready),
        .O(\state_read[1]_i_1_n_0 ));
  LUT1 #(
    .INIT(2'h1)) 
    \state_read[1]_i_2 
       (.I0(state_read[1]),
        .O(p_1_in__1));
  (* FSM_ENCODED_STATES = "IDLE_READ:01,WAIT_FOR_RVALID_READ:10," *) 
  FDSE #(
    .INIT(1'b1)) 
    \state_read_reg[0] 
       (.C(clk),
        .CE(\state_read[1]_i_1_n_0 ),
        .D(state_read[1]),
        .Q(state_read[0]),
        .S(src_in1));
  (* FSM_ENCODED_STATES = "IDLE_READ:01,WAIT_FOR_RVALID_READ:10," *) 
  FDRE #(
    .INIT(1'b0)) 
    \state_read_reg[1] 
       (.C(clk),
        .CE(\state_read[1]_i_1_n_0 ),
        .D(p_1_in__1),
        .Q(state_read[1]),
        .R(src_in1));
  (* SOFT_HLUTNM = "soft_lutpair0" *) 
  LUT2 #(
    .INIT(4'h1)) 
    \state_write[1]_i_1 
       (.I0(state_write[2]),
        .I1(state_write[1]),
        .O(p_1_in__0));
  LUT1 #(
    .INIT(2'h1)) 
    \state_write[2]_i_1 
       (.I0(reset_n),
        .O(src_in1));
  LUT6 #(
    .INIT(64'h000A000A0CF00C00)) 
    \state_write[2]_i_2 
       (.I0(axil_bready),
        .I1(axil_wvalid),
        .I2(state_write[0]),
        .I3(state_write[1]),
        .I4(axil_awvalid),
        .I5(state_write[2]),
        .O(\state_write[2]_i_2_n_0 ));
  (* FSM_ENCODED_STATES = "IDLE_WRITE:001,WAIT_FOR_WVALID_WRITE:010,WAIT_FOR_BREADY_WRITE:100," *) 
  FDSE #(
    .INIT(1'b1)) 
    \state_write_reg[0] 
       (.C(clk),
        .CE(\state_write[2]_i_2_n_0 ),
        .D(state_write[2]),
        .Q(state_write[0]),
        .S(src_in1));
  (* FSM_ENCODED_STATES = "IDLE_WRITE:001,WAIT_FOR_WVALID_WRITE:010,WAIT_FOR_BREADY_WRITE:100," *) 
  FDRE #(
    .INIT(1'b0)) 
    \state_write_reg[1] 
       (.C(clk),
        .CE(\state_write[2]_i_2_n_0 ),
        .D(p_1_in__0),
        .Q(state_write[1]),
        .R(src_in1));
  (* FSM_ENCODED_STATES = "IDLE_WRITE:001,WAIT_FOR_WVALID_WRITE:010,WAIT_FOR_BREADY_WRITE:100," *) 
  FDRE #(
    .INIT(1'b0)) 
    \state_write_reg[2] 
       (.C(clk),
        .CE(\state_write[2]_i_2_n_0 ),
        .D(state_write[1]),
        .Q(state_write[2]),
        .R(src_in1));
  LUT1 #(
    .INIT(2'h1)) 
    \test_clk0_cntr[0]_i_1 
       (.I0(done0_synced),
        .O(\test_clk0_cntr[0]_i_1_n_0 ));
  LUT1 #(
    .INIT(2'h1)) 
    \test_clk0_cntr[0]_i_3 
       (.I0(test_clk0_cntr_reg[0]),
        .O(\test_clk0_cntr[0]_i_3_n_0 ));
  FDRE \test_clk0_cntr_reg[0] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[0]_i_2_n_15 ),
        .Q(test_clk0_cntr_reg[0]),
        .R(rst0_synced));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \test_clk0_cntr_reg[0]_i_2 
       (.CI(1'b0),
        .CI_TOP(1'b0),
        .CO({\test_clk0_cntr_reg[0]_i_2_n_0 ,\test_clk0_cntr_reg[0]_i_2_n_1 ,\test_clk0_cntr_reg[0]_i_2_n_2 ,\test_clk0_cntr_reg[0]_i_2_n_3 ,\test_clk0_cntr_reg[0]_i_2_n_4 ,\test_clk0_cntr_reg[0]_i_2_n_5 ,\test_clk0_cntr_reg[0]_i_2_n_6 ,\test_clk0_cntr_reg[0]_i_2_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b1}),
        .O({\test_clk0_cntr_reg[0]_i_2_n_8 ,\test_clk0_cntr_reg[0]_i_2_n_9 ,\test_clk0_cntr_reg[0]_i_2_n_10 ,\test_clk0_cntr_reg[0]_i_2_n_11 ,\test_clk0_cntr_reg[0]_i_2_n_12 ,\test_clk0_cntr_reg[0]_i_2_n_13 ,\test_clk0_cntr_reg[0]_i_2_n_14 ,\test_clk0_cntr_reg[0]_i_2_n_15 }),
        .S({test_clk0_cntr_reg[7:1],\test_clk0_cntr[0]_i_3_n_0 }));
  FDRE \test_clk0_cntr_reg[10] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[8]_i_1_n_13 ),
        .Q(test_clk0_cntr_reg[10]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[11] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[8]_i_1_n_12 ),
        .Q(test_clk0_cntr_reg[11]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[12] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[8]_i_1_n_11 ),
        .Q(test_clk0_cntr_reg[12]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[13] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[8]_i_1_n_10 ),
        .Q(test_clk0_cntr_reg[13]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[14] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[8]_i_1_n_9 ),
        .Q(test_clk0_cntr_reg[14]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[15] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[8]_i_1_n_8 ),
        .Q(test_clk0_cntr_reg[15]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[16] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[16]_i_1_n_15 ),
        .Q(test_clk0_cntr_reg[16]),
        .R(rst0_synced));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \test_clk0_cntr_reg[16]_i_1 
       (.CI(\test_clk0_cntr_reg[8]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\test_clk0_cntr_reg[16]_i_1_n_0 ,\test_clk0_cntr_reg[16]_i_1_n_1 ,\test_clk0_cntr_reg[16]_i_1_n_2 ,\test_clk0_cntr_reg[16]_i_1_n_3 ,\test_clk0_cntr_reg[16]_i_1_n_4 ,\test_clk0_cntr_reg[16]_i_1_n_5 ,\test_clk0_cntr_reg[16]_i_1_n_6 ,\test_clk0_cntr_reg[16]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\test_clk0_cntr_reg[16]_i_1_n_8 ,\test_clk0_cntr_reg[16]_i_1_n_9 ,\test_clk0_cntr_reg[16]_i_1_n_10 ,\test_clk0_cntr_reg[16]_i_1_n_11 ,\test_clk0_cntr_reg[16]_i_1_n_12 ,\test_clk0_cntr_reg[16]_i_1_n_13 ,\test_clk0_cntr_reg[16]_i_1_n_14 ,\test_clk0_cntr_reg[16]_i_1_n_15 }),
        .S(test_clk0_cntr_reg[23:16]));
  FDRE \test_clk0_cntr_reg[17] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[16]_i_1_n_14 ),
        .Q(test_clk0_cntr_reg[17]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[18] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[16]_i_1_n_13 ),
        .Q(test_clk0_cntr_reg[18]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[19] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[16]_i_1_n_12 ),
        .Q(test_clk0_cntr_reg[19]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[1] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[0]_i_2_n_14 ),
        .Q(test_clk0_cntr_reg[1]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[20] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[16]_i_1_n_11 ),
        .Q(test_clk0_cntr_reg[20]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[21] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[16]_i_1_n_10 ),
        .Q(test_clk0_cntr_reg[21]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[22] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[16]_i_1_n_9 ),
        .Q(test_clk0_cntr_reg[22]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[23] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[16]_i_1_n_8 ),
        .Q(test_clk0_cntr_reg[23]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[24] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[24]_i_1_n_15 ),
        .Q(test_clk0_cntr_reg[24]),
        .R(rst0_synced));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \test_clk0_cntr_reg[24]_i_1 
       (.CI(\test_clk0_cntr_reg[16]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\NLW_test_clk0_cntr_reg[24]_i_1_CO_UNCONNECTED [7],\test_clk0_cntr_reg[24]_i_1_n_1 ,\test_clk0_cntr_reg[24]_i_1_n_2 ,\test_clk0_cntr_reg[24]_i_1_n_3 ,\test_clk0_cntr_reg[24]_i_1_n_4 ,\test_clk0_cntr_reg[24]_i_1_n_5 ,\test_clk0_cntr_reg[24]_i_1_n_6 ,\test_clk0_cntr_reg[24]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\test_clk0_cntr_reg[24]_i_1_n_8 ,\test_clk0_cntr_reg[24]_i_1_n_9 ,\test_clk0_cntr_reg[24]_i_1_n_10 ,\test_clk0_cntr_reg[24]_i_1_n_11 ,\test_clk0_cntr_reg[24]_i_1_n_12 ,\test_clk0_cntr_reg[24]_i_1_n_13 ,\test_clk0_cntr_reg[24]_i_1_n_14 ,\test_clk0_cntr_reg[24]_i_1_n_15 }),
        .S(test_clk0_cntr_reg[31:24]));
  FDRE \test_clk0_cntr_reg[25] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[24]_i_1_n_14 ),
        .Q(test_clk0_cntr_reg[25]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[26] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[24]_i_1_n_13 ),
        .Q(test_clk0_cntr_reg[26]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[27] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[24]_i_1_n_12 ),
        .Q(test_clk0_cntr_reg[27]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[28] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[24]_i_1_n_11 ),
        .Q(test_clk0_cntr_reg[28]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[29] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[24]_i_1_n_10 ),
        .Q(test_clk0_cntr_reg[29]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[2] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[0]_i_2_n_13 ),
        .Q(test_clk0_cntr_reg[2]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[30] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[24]_i_1_n_9 ),
        .Q(test_clk0_cntr_reg[30]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[31] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[24]_i_1_n_8 ),
        .Q(test_clk0_cntr_reg[31]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[3] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[0]_i_2_n_12 ),
        .Q(test_clk0_cntr_reg[3]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[4] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[0]_i_2_n_11 ),
        .Q(test_clk0_cntr_reg[4]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[5] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[0]_i_2_n_10 ),
        .Q(test_clk0_cntr_reg[5]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[6] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[0]_i_2_n_9 ),
        .Q(test_clk0_cntr_reg[6]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[7] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[0]_i_2_n_8 ),
        .Q(test_clk0_cntr_reg[7]),
        .R(rst0_synced));
  FDRE \test_clk0_cntr_reg[8] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[8]_i_1_n_15 ),
        .Q(test_clk0_cntr_reg[8]),
        .R(rst0_synced));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \test_clk0_cntr_reg[8]_i_1 
       (.CI(\test_clk0_cntr_reg[0]_i_2_n_0 ),
        .CI_TOP(1'b0),
        .CO({\test_clk0_cntr_reg[8]_i_1_n_0 ,\test_clk0_cntr_reg[8]_i_1_n_1 ,\test_clk0_cntr_reg[8]_i_1_n_2 ,\test_clk0_cntr_reg[8]_i_1_n_3 ,\test_clk0_cntr_reg[8]_i_1_n_4 ,\test_clk0_cntr_reg[8]_i_1_n_5 ,\test_clk0_cntr_reg[8]_i_1_n_6 ,\test_clk0_cntr_reg[8]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\test_clk0_cntr_reg[8]_i_1_n_8 ,\test_clk0_cntr_reg[8]_i_1_n_9 ,\test_clk0_cntr_reg[8]_i_1_n_10 ,\test_clk0_cntr_reg[8]_i_1_n_11 ,\test_clk0_cntr_reg[8]_i_1_n_12 ,\test_clk0_cntr_reg[8]_i_1_n_13 ,\test_clk0_cntr_reg[8]_i_1_n_14 ,\test_clk0_cntr_reg[8]_i_1_n_15 }),
        .S(test_clk0_cntr_reg[15:8]));
  FDRE \test_clk0_cntr_reg[9] 
       (.C(test_clk0),
        .CE(\test_clk0_cntr[0]_i_1_n_0 ),
        .D(\test_clk0_cntr_reg[8]_i_1_n_14 ),
        .Q(test_clk0_cntr_reg[9]),
        .R(rst0_synced));
  LUT1 #(
    .INIT(2'h1)) 
    \test_clk1_cntr[0]_i_1 
       (.I0(done1_synced),
        .O(\test_clk1_cntr[0]_i_1_n_0 ));
  LUT1 #(
    .INIT(2'h1)) 
    \test_clk1_cntr[0]_i_3 
       (.I0(test_clk1_cntr_reg[0]),
        .O(\test_clk1_cntr[0]_i_3_n_0 ));
  FDRE \test_clk1_cntr_reg[0] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[0]_i_2_n_15 ),
        .Q(test_clk1_cntr_reg[0]),
        .R(rst1_synced));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \test_clk1_cntr_reg[0]_i_2 
       (.CI(1'b0),
        .CI_TOP(1'b0),
        .CO({\test_clk1_cntr_reg[0]_i_2_n_0 ,\test_clk1_cntr_reg[0]_i_2_n_1 ,\test_clk1_cntr_reg[0]_i_2_n_2 ,\test_clk1_cntr_reg[0]_i_2_n_3 ,\test_clk1_cntr_reg[0]_i_2_n_4 ,\test_clk1_cntr_reg[0]_i_2_n_5 ,\test_clk1_cntr_reg[0]_i_2_n_6 ,\test_clk1_cntr_reg[0]_i_2_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b1}),
        .O({\test_clk1_cntr_reg[0]_i_2_n_8 ,\test_clk1_cntr_reg[0]_i_2_n_9 ,\test_clk1_cntr_reg[0]_i_2_n_10 ,\test_clk1_cntr_reg[0]_i_2_n_11 ,\test_clk1_cntr_reg[0]_i_2_n_12 ,\test_clk1_cntr_reg[0]_i_2_n_13 ,\test_clk1_cntr_reg[0]_i_2_n_14 ,\test_clk1_cntr_reg[0]_i_2_n_15 }),
        .S({test_clk1_cntr_reg[7:1],\test_clk1_cntr[0]_i_3_n_0 }));
  FDRE \test_clk1_cntr_reg[10] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[8]_i_1_n_13 ),
        .Q(test_clk1_cntr_reg[10]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[11] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[8]_i_1_n_12 ),
        .Q(test_clk1_cntr_reg[11]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[12] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[8]_i_1_n_11 ),
        .Q(test_clk1_cntr_reg[12]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[13] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[8]_i_1_n_10 ),
        .Q(test_clk1_cntr_reg[13]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[14] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[8]_i_1_n_9 ),
        .Q(test_clk1_cntr_reg[14]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[15] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[8]_i_1_n_8 ),
        .Q(test_clk1_cntr_reg[15]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[16] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[16]_i_1_n_15 ),
        .Q(test_clk1_cntr_reg[16]),
        .R(rst1_synced));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \test_clk1_cntr_reg[16]_i_1 
       (.CI(\test_clk1_cntr_reg[8]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\test_clk1_cntr_reg[16]_i_1_n_0 ,\test_clk1_cntr_reg[16]_i_1_n_1 ,\test_clk1_cntr_reg[16]_i_1_n_2 ,\test_clk1_cntr_reg[16]_i_1_n_3 ,\test_clk1_cntr_reg[16]_i_1_n_4 ,\test_clk1_cntr_reg[16]_i_1_n_5 ,\test_clk1_cntr_reg[16]_i_1_n_6 ,\test_clk1_cntr_reg[16]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\test_clk1_cntr_reg[16]_i_1_n_8 ,\test_clk1_cntr_reg[16]_i_1_n_9 ,\test_clk1_cntr_reg[16]_i_1_n_10 ,\test_clk1_cntr_reg[16]_i_1_n_11 ,\test_clk1_cntr_reg[16]_i_1_n_12 ,\test_clk1_cntr_reg[16]_i_1_n_13 ,\test_clk1_cntr_reg[16]_i_1_n_14 ,\test_clk1_cntr_reg[16]_i_1_n_15 }),
        .S(test_clk1_cntr_reg[23:16]));
  FDRE \test_clk1_cntr_reg[17] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[16]_i_1_n_14 ),
        .Q(test_clk1_cntr_reg[17]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[18] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[16]_i_1_n_13 ),
        .Q(test_clk1_cntr_reg[18]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[19] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[16]_i_1_n_12 ),
        .Q(test_clk1_cntr_reg[19]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[1] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[0]_i_2_n_14 ),
        .Q(test_clk1_cntr_reg[1]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[20] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[16]_i_1_n_11 ),
        .Q(test_clk1_cntr_reg[20]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[21] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[16]_i_1_n_10 ),
        .Q(test_clk1_cntr_reg[21]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[22] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[16]_i_1_n_9 ),
        .Q(test_clk1_cntr_reg[22]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[23] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[16]_i_1_n_8 ),
        .Q(test_clk1_cntr_reg[23]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[24] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[24]_i_1_n_15 ),
        .Q(test_clk1_cntr_reg[24]),
        .R(rst1_synced));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \test_clk1_cntr_reg[24]_i_1 
       (.CI(\test_clk1_cntr_reg[16]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\NLW_test_clk1_cntr_reg[24]_i_1_CO_UNCONNECTED [7],\test_clk1_cntr_reg[24]_i_1_n_1 ,\test_clk1_cntr_reg[24]_i_1_n_2 ,\test_clk1_cntr_reg[24]_i_1_n_3 ,\test_clk1_cntr_reg[24]_i_1_n_4 ,\test_clk1_cntr_reg[24]_i_1_n_5 ,\test_clk1_cntr_reg[24]_i_1_n_6 ,\test_clk1_cntr_reg[24]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\test_clk1_cntr_reg[24]_i_1_n_8 ,\test_clk1_cntr_reg[24]_i_1_n_9 ,\test_clk1_cntr_reg[24]_i_1_n_10 ,\test_clk1_cntr_reg[24]_i_1_n_11 ,\test_clk1_cntr_reg[24]_i_1_n_12 ,\test_clk1_cntr_reg[24]_i_1_n_13 ,\test_clk1_cntr_reg[24]_i_1_n_14 ,\test_clk1_cntr_reg[24]_i_1_n_15 }),
        .S(test_clk1_cntr_reg[31:24]));
  FDRE \test_clk1_cntr_reg[25] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[24]_i_1_n_14 ),
        .Q(test_clk1_cntr_reg[25]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[26] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[24]_i_1_n_13 ),
        .Q(test_clk1_cntr_reg[26]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[27] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[24]_i_1_n_12 ),
        .Q(test_clk1_cntr_reg[27]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[28] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[24]_i_1_n_11 ),
        .Q(test_clk1_cntr_reg[28]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[29] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[24]_i_1_n_10 ),
        .Q(test_clk1_cntr_reg[29]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[2] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[0]_i_2_n_13 ),
        .Q(test_clk1_cntr_reg[2]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[30] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[24]_i_1_n_9 ),
        .Q(test_clk1_cntr_reg[30]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[31] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[24]_i_1_n_8 ),
        .Q(test_clk1_cntr_reg[31]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[3] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[0]_i_2_n_12 ),
        .Q(test_clk1_cntr_reg[3]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[4] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[0]_i_2_n_11 ),
        .Q(test_clk1_cntr_reg[4]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[5] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[0]_i_2_n_10 ),
        .Q(test_clk1_cntr_reg[5]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[6] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[0]_i_2_n_9 ),
        .Q(test_clk1_cntr_reg[6]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[7] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[0]_i_2_n_8 ),
        .Q(test_clk1_cntr_reg[7]),
        .R(rst1_synced));
  FDRE \test_clk1_cntr_reg[8] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[8]_i_1_n_15 ),
        .Q(test_clk1_cntr_reg[8]),
        .R(rst1_synced));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \test_clk1_cntr_reg[8]_i_1 
       (.CI(\test_clk1_cntr_reg[0]_i_2_n_0 ),
        .CI_TOP(1'b0),
        .CO({\test_clk1_cntr_reg[8]_i_1_n_0 ,\test_clk1_cntr_reg[8]_i_1_n_1 ,\test_clk1_cntr_reg[8]_i_1_n_2 ,\test_clk1_cntr_reg[8]_i_1_n_3 ,\test_clk1_cntr_reg[8]_i_1_n_4 ,\test_clk1_cntr_reg[8]_i_1_n_5 ,\test_clk1_cntr_reg[8]_i_1_n_6 ,\test_clk1_cntr_reg[8]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\test_clk1_cntr_reg[8]_i_1_n_8 ,\test_clk1_cntr_reg[8]_i_1_n_9 ,\test_clk1_cntr_reg[8]_i_1_n_10 ,\test_clk1_cntr_reg[8]_i_1_n_11 ,\test_clk1_cntr_reg[8]_i_1_n_12 ,\test_clk1_cntr_reg[8]_i_1_n_13 ,\test_clk1_cntr_reg[8]_i_1_n_14 ,\test_clk1_cntr_reg[8]_i_1_n_15 }),
        .S(test_clk1_cntr_reg[15:8]));
  FDRE \test_clk1_cntr_reg[9] 
       (.C(test_clk1),
        .CE(\test_clk1_cntr[0]_i_1_n_0 ),
        .D(\test_clk1_cntr_reg[8]_i_1_n_14 ),
        .Q(test_clk1_cntr_reg[9]),
        .R(rst1_synced));
  LUT5 #(
    .INIT(32'h3A330A00)) 
    user_rst_i_1
       (.I0(axil_wdata[0]),
        .I1(p_1_in),
        .I2(\axil_rdata[31]_i_2_n_0 ),
        .I3(axil_wready),
        .I4(user_rst_reg_n_0),
        .O(user_rst_i_1_n_0));
  FDRE user_rst_reg
       (.C(clk),
        .CE(1'b1),
        .D(user_rst_i_1_n_0),
        .Q(user_rst_reg_n_0),
        .R(1'b0));
  (* DEST_SYNC_FF = "2" *) 
  (* INIT_SYNC_FF = "0" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* SRC_INPUT_REG = "0" *) 
  (* VERSION = "0" *) 
  (* WIDTH = "1" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  (* XPM_MODULE = "TRUE" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__6 xpm_cdc_array_single_inst
       (.dest_clk(test_clk0),
        .dest_out(rst0_synced),
        .src_clk(1'b0),
        .src_in(src_in0));
  (* DEST_SYNC_FF = "2" *) 
  (* INIT_SYNC_FF = "0" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* SRC_INPUT_REG = "0" *) 
  (* VERSION = "0" *) 
  (* WIDTH = "1" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  (* XPM_MODULE = "TRUE" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__8 xpm_cdc_array_single_inst1
       (.dest_clk(clk),
        .dest_out(user_rst0_ack),
        .src_clk(1'b0),
        .src_in(rst0_synced));
  (* DEST_SYNC_FF = "2" *) 
  (* INIT_SYNC_FF = "0" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* SRC_INPUT_REG = "0" *) 
  (* VERSION = "0" *) 
  (* WIDTH = "1" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  (* XPM_MODULE = "TRUE" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__10 xpm_cdc_array_single_inst2
       (.dest_clk(test_clk0),
        .dest_out(done0_synced),
        .src_clk(1'b0),
        .src_in(done));
  LUT4 #(
    .INIT(16'h0004)) 
    xpm_cdc_array_single_inst2_i_1
       (.I0(\axil_rdata[1]_i_5_n_0 ),
        .I1(\axil_rdata[1]_i_4_n_0 ),
        .I2(xpm_cdc_array_single_inst2_i_2_n_0),
        .I3(xpm_cdc_array_single_inst2_i_3_n_0),
        .O(done));
  (* SOFT_HLUTNM = "soft_lutpair1" *) 
  LUT5 #(
    .INIT(32'hFFFFFFFE)) 
    xpm_cdc_array_single_inst2_i_2
       (.I0(ref_clk_cntr_reg[10]),
        .I1(ref_clk_cntr_reg[13]),
        .I2(ref_clk_cntr_reg[11]),
        .I3(ref_clk_cntr_reg[12]),
        .I4(\axil_rdata[1]_i_9_n_0 ),
        .O(xpm_cdc_array_single_inst2_i_2_n_0));
  (* SOFT_HLUTNM = "soft_lutpair2" *) 
  LUT5 #(
    .INIT(32'hFFFFFFFE)) 
    xpm_cdc_array_single_inst2_i_3
       (.I0(ref_clk_cntr_reg[20]),
        .I1(ref_clk_cntr_reg[23]),
        .I2(ref_clk_cntr_reg[21]),
        .I3(ref_clk_cntr_reg[22]),
        .I4(\axil_rdata[1]_i_7_n_0 ),
        .O(xpm_cdc_array_single_inst2_i_3_n_0));
  (* DEST_SYNC_FF = "2" *) 
  (* INIT_SYNC_FF = "0" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* SRC_INPUT_REG = "0" *) 
  (* VERSION = "0" *) 
  (* WIDTH = "32" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  (* XPM_MODULE = "TRUE" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0__2 xpm_cdc_array_single_inst3
       (.dest_clk(clk),
        .dest_out(test_clk0_cntr_synced),
        .src_clk(1'b0),
        .src_in(test_clk0_cntr_reg));
  (* DEST_SYNC_FF = "2" *) 
  (* INIT_SYNC_FF = "0" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* SRC_INPUT_REG = "0" *) 
  (* VERSION = "0" *) 
  (* WIDTH = "32" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  (* XPM_MODULE = "TRUE" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0 xpm_cdc_array_single_inst4
       (.dest_clk(clk),
        .dest_out(test_clk1_cntr_synced),
        .src_clk(1'b0),
        .src_in(test_clk1_cntr_reg));
  (* DEST_SYNC_FF = "2" *) 
  (* INIT_SYNC_FF = "0" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* SRC_INPUT_REG = "0" *) 
  (* VERSION = "0" *) 
  (* WIDTH = "1" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  (* XPM_MODULE = "TRUE" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__7 xpm_cdc_array_single_inst5
       (.dest_clk(test_clk1),
        .dest_out(rst1_synced),
        .src_clk(1'b0),
        .src_in(src_in0));
  (* DEST_SYNC_FF = "2" *) 
  (* INIT_SYNC_FF = "0" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* SRC_INPUT_REG = "0" *) 
  (* VERSION = "0" *) 
  (* WIDTH = "1" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  (* XPM_MODULE = "TRUE" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__9 xpm_cdc_array_single_inst6
       (.dest_clk(clk),
        .dest_out(user_rst1_ack),
        .src_clk(1'b0),
        .src_in(rst1_synced));
  (* DEST_SYNC_FF = "2" *) 
  (* INIT_SYNC_FF = "0" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* SRC_INPUT_REG = "0" *) 
  (* VERSION = "0" *) 
  (* WIDTH = "1" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  (* XPM_MODULE = "TRUE" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single xpm_cdc_array_single_inst7
       (.dest_clk(test_clk1),
        .dest_out(done1_synced),
        .src_clk(1'b0),
        .src_in(done));
  LUT2 #(
    .INIT(4'hB)) 
    xpm_cdc_array_single_inst_i_1
       (.I0(user_rst_reg_n_0),
        .I1(reset_n),
        .O(src_in0));
endmodule

(* CHECK_LICENSE_TYPE = "pfm_dynamic_freq_counter_0_0,freq_counter,{}" *) (* DowngradeIPIdentifiedWarnings = "yes" *) (* IP_DEFINITION_SOURCE = "package_project" *) 
(* X_CORE_INFO = "freq_counter,Vivado 2020.2" *) 
(* NotValidForBitStream *)
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix
   (clk,
    reset_n,
    axil_awaddr,
    axil_awprot,
    axil_awvalid,
    axil_awready,
    axil_wdata,
    axil_wstrb,
    axil_wvalid,
    axil_wready,
    axil_bvalid,
    axil_bresp,
    axil_bready,
    axil_araddr,
    axil_arprot,
    axil_arvalid,
    axil_arready,
    axil_rdata,
    axil_rresp,
    axil_rvalid,
    axil_rready,
    test_clk0,
    test_clk1);
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 clk CLK" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME clk, ASSOCIATED_BUSIF axil, FREQ_HZ 50000000, PHASE 0.000, CLK_DOMAIN pfm_dynamic_s_axi_aclk, INSERT_VIP 0" *) input clk;
  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 reset_n RST" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME reset_n, POLARITY ACTIVE_LOW, INSERT_VIP 0" *) input reset_n;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 axil AWADDR" *) input [31:0]axil_awaddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 axil AWPROT" *) input [2:0]axil_awprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 axil AWVALID" *) input axil_awvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 axil AWREADY" *) output axil_awready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 axil WDATA" *) input [31:0]axil_wdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 axil WSTRB" *) input [3:0]axil_wstrb;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 axil WVALID" *) input axil_wvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 axil WREADY" *) output axil_wready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 axil BVALID" *) output axil_bvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 axil BRESP" *) output [1:0]axil_bresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 axil BREADY" *) input axil_bready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 axil ARADDR" *) input [31:0]axil_araddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 axil ARPROT" *) input [2:0]axil_arprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 axil ARVALID" *) input axil_arvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 axil ARREADY" *) output axil_arready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 axil RDATA" *) output [31:0]axil_rdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 axil RRESP" *) output [1:0]axil_rresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 axil RVALID" *) output axil_rvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 axil RREADY" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME axil, DATA_WIDTH 32, PROTOCOL AXI4LITE, FREQ_HZ 50000000, ID_WIDTH 0, ADDR_WIDTH 32, AWUSER_WIDTH 0, ARUSER_WIDTH 0, WUSER_WIDTH 0, RUSER_WIDTH 0, BUSER_WIDTH 0, READ_WRITE_MODE READ_WRITE, HAS_BURST 0, HAS_LOCK 0, HAS_PROT 1, HAS_CACHE 0, HAS_QOS 0, HAS_REGION 0, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 1, SUPPORTS_NARROW_BURST 0, NUM_READ_OUTSTANDING 1, NUM_WRITE_OUTSTANDING 1, MAX_BURST_LENGTH 1, PHASE 0.000, CLK_DOMAIN pfm_dynamic_s_axi_aclk, NUM_READ_THREADS 1, NUM_WRITE_THREADS 1, RUSER_BITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0" *) input axil_rready;
  input test_clk0;
  input test_clk1;

  wire \<const0> ;
  wire [31:0]axil_araddr;
  wire axil_arready;
  wire axil_arvalid;
  wire [31:0]axil_awaddr;
  wire axil_awready;
  wire axil_awvalid;
  wire axil_bready;
  wire axil_bvalid;
  wire [31:0]axil_rdata;
  wire axil_rready;
  wire axil_rvalid;
  wire [31:0]axil_wdata;
  wire axil_wready;
  wire axil_wvalid;
  wire clk;
  wire reset_n;
  wire test_clk0;
  wire test_clk1;

  assign axil_bresp[1] = \<const0> ;
  assign axil_bresp[0] = \<const0> ;
  assign axil_rresp[1] = \<const0> ;
  assign axil_rresp[0] = \<const0> ;
  GND GND
       (.G(\<const0> ));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_freq_counter inst
       (.axil_araddr(axil_araddr[3:0]),
        .axil_arready(axil_arready),
        .axil_arvalid(axil_arvalid),
        .axil_awaddr(axil_awaddr[3:0]),
        .axil_awready(axil_awready),
        .axil_awvalid(axil_awvalid),
        .axil_bready(axil_bready),
        .axil_bvalid(axil_bvalid),
        .axil_rdata(axil_rdata),
        .axil_rready(axil_rready),
        .axil_rvalid(axil_rvalid),
        .axil_wdata({axil_wdata[31:2],axil_wdata[0]}),
        .axil_wready(axil_wready),
        .axil_wvalid(axil_wvalid),
        .clk(clk),
        .reset_n(reset_n),
        .test_clk0(test_clk0),
        .test_clk1(test_clk1));
endmodule

(* DEST_SYNC_FF = "2" *) (* INIT_SYNC_FF = "0" *) (* SIM_ASSERT_CHK = "0" *) 
(* SRC_INPUT_REG = "0" *) (* VERSION = "0" *) (* WIDTH = "1" *) 
(* XPM_MODULE = "TRUE" *) (* keep_hierarchy = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single
   (src_clk,
    src_in,
    dest_clk,
    dest_out);
  input src_clk;
  input [0:0]src_in;
  input dest_clk;
  output [0:0]dest_out;

  wire async_path_bit;
  wire dest_clk;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) wire \syncstages_ff[0] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) wire \syncstages_ff[1] ;

  assign async_path_bit = src_in[0];
  assign dest_out[0] = \syncstages_ff[1] ;
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit),
        .Q(\syncstages_ff[0] ),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] ),
        .Q(\syncstages_ff[1] ),
        .R(1'b0));
endmodule

(* DEST_SYNC_FF = "2" *) (* INIT_SYNC_FF = "0" *) (* ORIG_REF_NAME = "xpm_cdc_array_single" *) 
(* SIM_ASSERT_CHK = "0" *) (* SRC_INPUT_REG = "0" *) (* VERSION = "0" *) 
(* WIDTH = "1" *) (* XPM_MODULE = "TRUE" *) (* keep_hierarchy = "true" *) 
(* xpm_cdc = "ARRAY_SINGLE" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__10
   (src_clk,
    src_in,
    dest_clk,
    dest_out);
  input src_clk;
  input [0:0]src_in;
  input dest_clk;
  output [0:0]dest_out;

  wire async_path_bit;
  wire dest_clk;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) wire \syncstages_ff[0] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) wire \syncstages_ff[1] ;

  assign async_path_bit = src_in[0];
  assign dest_out[0] = \syncstages_ff[1] ;
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit),
        .Q(\syncstages_ff[0] ),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] ),
        .Q(\syncstages_ff[1] ),
        .R(1'b0));
endmodule

(* DEST_SYNC_FF = "2" *) (* INIT_SYNC_FF = "0" *) (* ORIG_REF_NAME = "xpm_cdc_array_single" *) 
(* SIM_ASSERT_CHK = "0" *) (* SRC_INPUT_REG = "0" *) (* VERSION = "0" *) 
(* WIDTH = "1" *) (* XPM_MODULE = "TRUE" *) (* keep_hierarchy = "true" *) 
(* xpm_cdc = "ARRAY_SINGLE" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__6
   (src_clk,
    src_in,
    dest_clk,
    dest_out);
  input src_clk;
  input [0:0]src_in;
  input dest_clk;
  output [0:0]dest_out;

  wire async_path_bit;
  wire dest_clk;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) wire \syncstages_ff[0] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) wire \syncstages_ff[1] ;

  assign async_path_bit = src_in[0];
  assign dest_out[0] = \syncstages_ff[1] ;
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit),
        .Q(\syncstages_ff[0] ),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] ),
        .Q(\syncstages_ff[1] ),
        .R(1'b0));
endmodule

(* DEST_SYNC_FF = "2" *) (* INIT_SYNC_FF = "0" *) (* ORIG_REF_NAME = "xpm_cdc_array_single" *) 
(* SIM_ASSERT_CHK = "0" *) (* SRC_INPUT_REG = "0" *) (* VERSION = "0" *) 
(* WIDTH = "1" *) (* XPM_MODULE = "TRUE" *) (* keep_hierarchy = "true" *) 
(* xpm_cdc = "ARRAY_SINGLE" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__7
   (src_clk,
    src_in,
    dest_clk,
    dest_out);
  input src_clk;
  input [0:0]src_in;
  input dest_clk;
  output [0:0]dest_out;

  wire async_path_bit;
  wire dest_clk;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) wire \syncstages_ff[0] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) wire \syncstages_ff[1] ;

  assign async_path_bit = src_in[0];
  assign dest_out[0] = \syncstages_ff[1] ;
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit),
        .Q(\syncstages_ff[0] ),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] ),
        .Q(\syncstages_ff[1] ),
        .R(1'b0));
endmodule

(* DEST_SYNC_FF = "2" *) (* INIT_SYNC_FF = "0" *) (* ORIG_REF_NAME = "xpm_cdc_array_single" *) 
(* SIM_ASSERT_CHK = "0" *) (* SRC_INPUT_REG = "0" *) (* VERSION = "0" *) 
(* WIDTH = "1" *) (* XPM_MODULE = "TRUE" *) (* keep_hierarchy = "true" *) 
(* xpm_cdc = "ARRAY_SINGLE" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__8
   (src_clk,
    src_in,
    dest_clk,
    dest_out);
  input src_clk;
  input [0:0]src_in;
  input dest_clk;
  output [0:0]dest_out;

  wire async_path_bit;
  wire dest_clk;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) wire \syncstages_ff[0] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) wire \syncstages_ff[1] ;

  assign async_path_bit = src_in[0];
  assign dest_out[0] = \syncstages_ff[1] ;
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit),
        .Q(\syncstages_ff[0] ),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] ),
        .Q(\syncstages_ff[1] ),
        .R(1'b0));
endmodule

(* DEST_SYNC_FF = "2" *) (* INIT_SYNC_FF = "0" *) (* ORIG_REF_NAME = "xpm_cdc_array_single" *) 
(* SIM_ASSERT_CHK = "0" *) (* SRC_INPUT_REG = "0" *) (* VERSION = "0" *) 
(* WIDTH = "1" *) (* XPM_MODULE = "TRUE" *) (* keep_hierarchy = "true" *) 
(* xpm_cdc = "ARRAY_SINGLE" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__9
   (src_clk,
    src_in,
    dest_clk,
    dest_out);
  input src_clk;
  input [0:0]src_in;
  input dest_clk;
  output [0:0]dest_out;

  wire async_path_bit;
  wire dest_clk;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) wire \syncstages_ff[0] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) wire \syncstages_ff[1] ;

  assign async_path_bit = src_in[0];
  assign dest_out[0] = \syncstages_ff[1] ;
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit),
        .Q(\syncstages_ff[0] ),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] ),
        .Q(\syncstages_ff[1] ),
        .R(1'b0));
endmodule

(* DEST_SYNC_FF = "2" *) (* INIT_SYNC_FF = "0" *) (* ORIG_REF_NAME = "xpm_cdc_array_single" *) 
(* SIM_ASSERT_CHK = "0" *) (* SRC_INPUT_REG = "0" *) (* VERSION = "0" *) 
(* WIDTH = "32" *) (* XPM_MODULE = "TRUE" *) (* keep_hierarchy = "true" *) 
(* xpm_cdc = "ARRAY_SINGLE" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0
   (src_clk,
    src_in,
    dest_clk,
    dest_out);
  input src_clk;
  input [31:0]src_in;
  input dest_clk;
  output [31:0]dest_out;

  wire [31:0]async_path_bit;
  wire dest_clk;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) wire [31:0]\syncstages_ff[0] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) wire [31:0]\syncstages_ff[1] ;

  assign async_path_bit = src_in[31:0];
  assign dest_out[31:0] = \syncstages_ff[1] ;
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[0]),
        .Q(\syncstages_ff[0] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][10] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[10]),
        .Q(\syncstages_ff[0] [10]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][11] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[11]),
        .Q(\syncstages_ff[0] [11]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][12] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[12]),
        .Q(\syncstages_ff[0] [12]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][13] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[13]),
        .Q(\syncstages_ff[0] [13]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][14] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[14]),
        .Q(\syncstages_ff[0] [14]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][15] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[15]),
        .Q(\syncstages_ff[0] [15]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][16] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[16]),
        .Q(\syncstages_ff[0] [16]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][17] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[17]),
        .Q(\syncstages_ff[0] [17]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][18] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[18]),
        .Q(\syncstages_ff[0] [18]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][19] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[19]),
        .Q(\syncstages_ff[0] [19]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[1]),
        .Q(\syncstages_ff[0] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][20] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[20]),
        .Q(\syncstages_ff[0] [20]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][21] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[21]),
        .Q(\syncstages_ff[0] [21]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][22] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[22]),
        .Q(\syncstages_ff[0] [22]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][23] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[23]),
        .Q(\syncstages_ff[0] [23]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][24] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[24]),
        .Q(\syncstages_ff[0] [24]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][25] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[25]),
        .Q(\syncstages_ff[0] [25]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][26] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[26]),
        .Q(\syncstages_ff[0] [26]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][27] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[27]),
        .Q(\syncstages_ff[0] [27]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][28] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[28]),
        .Q(\syncstages_ff[0] [28]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][29] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[29]),
        .Q(\syncstages_ff[0] [29]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[2]),
        .Q(\syncstages_ff[0] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][30] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[30]),
        .Q(\syncstages_ff[0] [30]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][31] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[31]),
        .Q(\syncstages_ff[0] [31]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[3]),
        .Q(\syncstages_ff[0] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][4] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[4]),
        .Q(\syncstages_ff[0] [4]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][5] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[5]),
        .Q(\syncstages_ff[0] [5]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][6] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[6]),
        .Q(\syncstages_ff[0] [6]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][7] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[7]),
        .Q(\syncstages_ff[0] [7]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][8] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[8]),
        .Q(\syncstages_ff[0] [8]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][9] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[9]),
        .Q(\syncstages_ff[0] [9]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [0]),
        .Q(\syncstages_ff[1] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][10] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [10]),
        .Q(\syncstages_ff[1] [10]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][11] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [11]),
        .Q(\syncstages_ff[1] [11]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][12] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [12]),
        .Q(\syncstages_ff[1] [12]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][13] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [13]),
        .Q(\syncstages_ff[1] [13]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][14] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [14]),
        .Q(\syncstages_ff[1] [14]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][15] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [15]),
        .Q(\syncstages_ff[1] [15]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][16] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [16]),
        .Q(\syncstages_ff[1] [16]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][17] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [17]),
        .Q(\syncstages_ff[1] [17]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][18] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [18]),
        .Q(\syncstages_ff[1] [18]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][19] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [19]),
        .Q(\syncstages_ff[1] [19]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [1]),
        .Q(\syncstages_ff[1] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][20] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [20]),
        .Q(\syncstages_ff[1] [20]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][21] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [21]),
        .Q(\syncstages_ff[1] [21]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][22] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [22]),
        .Q(\syncstages_ff[1] [22]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][23] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [23]),
        .Q(\syncstages_ff[1] [23]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][24] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [24]),
        .Q(\syncstages_ff[1] [24]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][25] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [25]),
        .Q(\syncstages_ff[1] [25]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][26] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [26]),
        .Q(\syncstages_ff[1] [26]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][27] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [27]),
        .Q(\syncstages_ff[1] [27]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][28] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [28]),
        .Q(\syncstages_ff[1] [28]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][29] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [29]),
        .Q(\syncstages_ff[1] [29]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [2]),
        .Q(\syncstages_ff[1] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][30] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [30]),
        .Q(\syncstages_ff[1] [30]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][31] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [31]),
        .Q(\syncstages_ff[1] [31]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [3]),
        .Q(\syncstages_ff[1] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][4] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [4]),
        .Q(\syncstages_ff[1] [4]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][5] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [5]),
        .Q(\syncstages_ff[1] [5]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][6] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [6]),
        .Q(\syncstages_ff[1] [6]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][7] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [7]),
        .Q(\syncstages_ff[1] [7]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][8] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [8]),
        .Q(\syncstages_ff[1] [8]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][9] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [9]),
        .Q(\syncstages_ff[1] [9]),
        .R(1'b0));
endmodule

(* DEST_SYNC_FF = "2" *) (* INIT_SYNC_FF = "0" *) (* ORIG_REF_NAME = "xpm_cdc_array_single" *) 
(* SIM_ASSERT_CHK = "0" *) (* SRC_INPUT_REG = "0" *) (* VERSION = "0" *) 
(* WIDTH = "32" *) (* XPM_MODULE = "TRUE" *) (* keep_hierarchy = "true" *) 
(* xpm_cdc = "ARRAY_SINGLE" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0__2
   (src_clk,
    src_in,
    dest_clk,
    dest_out);
  input src_clk;
  input [31:0]src_in;
  input dest_clk;
  output [31:0]dest_out;

  wire [31:0]async_path_bit;
  wire dest_clk;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) wire [31:0]\syncstages_ff[0] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) wire [31:0]\syncstages_ff[1] ;

  assign async_path_bit = src_in[31:0];
  assign dest_out[31:0] = \syncstages_ff[1] ;
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[0]),
        .Q(\syncstages_ff[0] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][10] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[10]),
        .Q(\syncstages_ff[0] [10]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][11] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[11]),
        .Q(\syncstages_ff[0] [11]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][12] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[12]),
        .Q(\syncstages_ff[0] [12]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][13] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[13]),
        .Q(\syncstages_ff[0] [13]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][14] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[14]),
        .Q(\syncstages_ff[0] [14]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][15] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[15]),
        .Q(\syncstages_ff[0] [15]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][16] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[16]),
        .Q(\syncstages_ff[0] [16]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][17] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[17]),
        .Q(\syncstages_ff[0] [17]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][18] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[18]),
        .Q(\syncstages_ff[0] [18]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][19] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[19]),
        .Q(\syncstages_ff[0] [19]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[1]),
        .Q(\syncstages_ff[0] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][20] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[20]),
        .Q(\syncstages_ff[0] [20]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][21] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[21]),
        .Q(\syncstages_ff[0] [21]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][22] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[22]),
        .Q(\syncstages_ff[0] [22]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][23] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[23]),
        .Q(\syncstages_ff[0] [23]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][24] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[24]),
        .Q(\syncstages_ff[0] [24]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][25] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[25]),
        .Q(\syncstages_ff[0] [25]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][26] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[26]),
        .Q(\syncstages_ff[0] [26]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][27] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[27]),
        .Q(\syncstages_ff[0] [27]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][28] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[28]),
        .Q(\syncstages_ff[0] [28]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][29] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[29]),
        .Q(\syncstages_ff[0] [29]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[2]),
        .Q(\syncstages_ff[0] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][30] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[30]),
        .Q(\syncstages_ff[0] [30]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][31] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[31]),
        .Q(\syncstages_ff[0] [31]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[3]),
        .Q(\syncstages_ff[0] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][4] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[4]),
        .Q(\syncstages_ff[0] [4]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][5] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[5]),
        .Q(\syncstages_ff[0] [5]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][6] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[6]),
        .Q(\syncstages_ff[0] [6]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][7] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[7]),
        .Q(\syncstages_ff[0] [7]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][8] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[8]),
        .Q(\syncstages_ff[0] [8]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[0][9] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[9]),
        .Q(\syncstages_ff[0] [9]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [0]),
        .Q(\syncstages_ff[1] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][10] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [10]),
        .Q(\syncstages_ff[1] [10]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][11] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [11]),
        .Q(\syncstages_ff[1] [11]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][12] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [12]),
        .Q(\syncstages_ff[1] [12]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][13] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [13]),
        .Q(\syncstages_ff[1] [13]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][14] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [14]),
        .Q(\syncstages_ff[1] [14]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][15] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [15]),
        .Q(\syncstages_ff[1] [15]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][16] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [16]),
        .Q(\syncstages_ff[1] [16]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][17] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [17]),
        .Q(\syncstages_ff[1] [17]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][18] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [18]),
        .Q(\syncstages_ff[1] [18]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][19] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [19]),
        .Q(\syncstages_ff[1] [19]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [1]),
        .Q(\syncstages_ff[1] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][20] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [20]),
        .Q(\syncstages_ff[1] [20]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][21] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [21]),
        .Q(\syncstages_ff[1] [21]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][22] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [22]),
        .Q(\syncstages_ff[1] [22]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][23] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [23]),
        .Q(\syncstages_ff[1] [23]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][24] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [24]),
        .Q(\syncstages_ff[1] [24]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][25] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [25]),
        .Q(\syncstages_ff[1] [25]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][26] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [26]),
        .Q(\syncstages_ff[1] [26]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][27] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [27]),
        .Q(\syncstages_ff[1] [27]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][28] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [28]),
        .Q(\syncstages_ff[1] [28]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][29] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [29]),
        .Q(\syncstages_ff[1] [29]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [2]),
        .Q(\syncstages_ff[1] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][30] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [30]),
        .Q(\syncstages_ff[1] [30]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][31] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [31]),
        .Q(\syncstages_ff[1] [31]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [3]),
        .Q(\syncstages_ff[1] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][4] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [4]),
        .Q(\syncstages_ff[1] [4]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][5] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [5]),
        .Q(\syncstages_ff[1] [5]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][6] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [6]),
        .Q(\syncstages_ff[1] [6]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][7] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [7]),
        .Q(\syncstages_ff[1] [7]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][8] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [8]),
        .Q(\syncstages_ff[1] [8]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[1][9] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [9]),
        .Q(\syncstages_ff[1] [9]),
        .R(1'b0));
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

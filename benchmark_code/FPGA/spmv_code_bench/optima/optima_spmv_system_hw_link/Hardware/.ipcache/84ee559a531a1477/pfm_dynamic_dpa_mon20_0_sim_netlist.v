// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2020.2 (lin64) Build 3064766 Wed Nov 18 09:12:47 MST 2020
// Date        : Fri Oct 20 19:52:24 2023
// Host        : gold1 running 64-bit Ubuntu 18.04.6 LTS
// Command     : write_verilog -force -mode funcsim -rename_top decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix -prefix
//               decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_ pfm_dynamic_dpa_mon20_0_sim_netlist.v
// Design      : pfm_dynamic_dpa_mon20_0
// Purpose     : This verilog netlist is a functional simulation representation of the design and should not be modified
//               or synthesized. This netlist cannot be used for SDF annotated simulation.
// Device      : xcu280-fsvh2892-2L-e
// --------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AXI_LITE_IF
   (axi_arready_reg_0,
    axi_wready_reg_0,
    axi_awready_reg_0,
    s_axi_bvalid,
    s_axi_rvalid,
    D,
    \s_axi_awaddr[4] ,
    \s_axi_awaddr[7] ,
    \Count_Out_i_reg[0] ,
    reset_sample_reg__0,
    E,
    slv_reg_addr_vld,
    mon_resetn_0,
    p_1_in,
    \register_select_reg[1] ,
    control_wr_en,
    \s_axi_awaddr[3] ,
    slv_reg_out_vld,
    sample_reg_rd_first_reg,
    s_axi_rdata,
    \axi_rdata_reg[0]_0 ,
    mon_clk,
    axi_rvalid_reg_0,
    \sample_data_reg[31] ,
    \sample_data_reg[30] ,
    \sample_data_reg[29] ,
    \sample_data_reg[28] ,
    \sample_data_reg[27] ,
    \sample_data_reg[26] ,
    \sample_data_reg[25] ,
    \sample_data_reg[24] ,
    \sample_data_reg[23] ,
    \sample_data_reg[22] ,
    \sample_data_reg[21] ,
    \sample_data_reg[20] ,
    \sample_data_reg[19] ,
    \sample_data_reg[18] ,
    \sample_data_reg[17] ,
    \sample_data_reg[16] ,
    \sample_data_reg[15] ,
    \sample_data_reg[14] ,
    \sample_data_reg[13] ,
    \sample_data_reg[12] ,
    \sample_data_reg[11] ,
    \sample_data_reg[10] ,
    \sample_data_reg[9] ,
    \sample_data_reg[8] ,
    \sample_data_reg[7] ,
    \sample_data_reg[6] ,
    \sample_data_reg[5] ,
    \sample_data_reg[4] ,
    \sample_data_reg[3] ,
    \sample_data_reg[2] ,
    \sample_data_reg[1] ,
    \sample_data_reg[0] ,
    s_axi_awaddr,
    s_axi_araddr,
    s_axi_arvalid,
    Q,
    sample_reg_rd_first,
    \Count_Out_i_reg[0]_0 ,
    mon_resetn,
    register_select,
    s_axi_awvalid,
    s_axi_wvalid,
    \sample_data_reg[31]_0 ,
    \sample_data_reg[31]_1 ,
    \sample_data_reg[0]_0 ,
    \sample_data_reg[0]_1 ,
    \sample_data_reg[1]_0 ,
    \sample_data_reg[1]_1 ,
    \sample_data_reg[2]_0 ,
    \sample_data_reg[2]_1 ,
    \sample_data_reg[3]_0 ,
    \sample_data_reg[3]_1 ,
    \sample_data_reg[4]_0 ,
    \sample_data_reg[4]_1 ,
    \sample_data_reg[5]_0 ,
    \sample_data_reg[5]_1 ,
    \sample_data_reg[6]_0 ,
    \sample_data_reg[6]_1 ,
    \sample_data_reg[7]_0 ,
    \sample_data_reg[7]_1 ,
    \sample_data_reg[8]_0 ,
    \sample_data_reg[8]_1 ,
    \sample_data_reg[9]_0 ,
    \sample_data_reg[9]_1 ,
    \sample_data_reg[10]_0 ,
    \sample_data_reg[10]_1 ,
    \sample_data_reg[11]_0 ,
    \sample_data_reg[11]_1 ,
    \sample_data_reg[12]_0 ,
    \sample_data_reg[12]_1 ,
    \sample_data_reg[13]_0 ,
    \sample_data_reg[13]_1 ,
    \sample_data_reg[14]_0 ,
    \sample_data_reg[14]_1 ,
    \sample_data_reg[15]_0 ,
    \sample_data_reg[15]_1 ,
    \sample_data_reg[16]_0 ,
    \sample_data_reg[16]_1 ,
    \sample_data_reg[17]_0 ,
    \sample_data_reg[17]_1 ,
    \sample_data_reg[18]_0 ,
    \sample_data_reg[18]_1 ,
    \sample_data_reg[19]_0 ,
    \sample_data_reg[19]_1 ,
    \sample_data_reg[20]_0 ,
    \sample_data_reg[20]_1 ,
    \sample_data_reg[21]_0 ,
    \sample_data_reg[21]_1 ,
    \sample_data_reg[22]_0 ,
    \sample_data_reg[22]_1 ,
    \sample_data_reg[23]_0 ,
    \sample_data_reg[23]_1 ,
    \sample_data_reg[24]_0 ,
    \sample_data_reg[24]_1 ,
    \sample_data_reg[25]_0 ,
    \sample_data_reg[25]_1 ,
    \sample_data_reg[26]_0 ,
    \sample_data_reg[26]_1 ,
    \sample_data_reg[27]_0 ,
    \sample_data_reg[27]_1 ,
    \sample_data_reg[28]_0 ,
    \sample_data_reg[28]_1 ,
    \sample_data_reg[29]_0 ,
    \sample_data_reg[29]_1 ,
    \sample_data_reg[30]_0 ,
    \sample_data_reg[30]_1 ,
    \sample_data_reg[31]_2 ,
    \sample_data_reg[31]_3 ,
    s_axi_bready,
    \axi_rdata_reg[31]_0 ,
    \axi_rdata_reg[31]_1 );
  output axi_arready_reg_0;
  output axi_wready_reg_0;
  output axi_awready_reg_0;
  output s_axi_bvalid;
  output s_axi_rvalid;
  output [31:0]D;
  output [2:0]\s_axi_awaddr[4] ;
  output [3:0]\s_axi_awaddr[7] ;
  output [0:0]\Count_Out_i_reg[0] ;
  output reset_sample_reg__0;
  output [0:0]E;
  output slv_reg_addr_vld;
  output [0:0]mon_resetn_0;
  output p_1_in;
  output [0:0]\register_select_reg[1] ;
  output control_wr_en;
  output [0:0]\s_axi_awaddr[3] ;
  output slv_reg_out_vld;
  output sample_reg_rd_first_reg;
  output [31:0]s_axi_rdata;
  input \axi_rdata_reg[0]_0 ;
  input mon_clk;
  input axi_rvalid_reg_0;
  input \sample_data_reg[31] ;
  input \sample_data_reg[30] ;
  input \sample_data_reg[29] ;
  input \sample_data_reg[28] ;
  input \sample_data_reg[27] ;
  input \sample_data_reg[26] ;
  input \sample_data_reg[25] ;
  input \sample_data_reg[24] ;
  input \sample_data_reg[23] ;
  input \sample_data_reg[22] ;
  input \sample_data_reg[21] ;
  input \sample_data_reg[20] ;
  input \sample_data_reg[19] ;
  input \sample_data_reg[18] ;
  input \sample_data_reg[17] ;
  input \sample_data_reg[16] ;
  input \sample_data_reg[15] ;
  input \sample_data_reg[14] ;
  input \sample_data_reg[13] ;
  input \sample_data_reg[12] ;
  input \sample_data_reg[11] ;
  input \sample_data_reg[10] ;
  input \sample_data_reg[9] ;
  input \sample_data_reg[8] ;
  input \sample_data_reg[7] ;
  input \sample_data_reg[6] ;
  input \sample_data_reg[5] ;
  input \sample_data_reg[4] ;
  input \sample_data_reg[3] ;
  input \sample_data_reg[2] ;
  input \sample_data_reg[1] ;
  input \sample_data_reg[0] ;
  input [7:0]s_axi_awaddr;
  input [7:0]s_axi_araddr;
  input s_axi_arvalid;
  input [0:0]Q;
  input sample_reg_rd_first;
  input \Count_Out_i_reg[0]_0 ;
  input mon_resetn;
  input [0:0]register_select;
  input s_axi_awvalid;
  input s_axi_wvalid;
  input [63:0]\sample_data_reg[31]_0 ;
  input [63:0]\sample_data_reg[31]_1 ;
  input \sample_data_reg[0]_0 ;
  input \sample_data_reg[0]_1 ;
  input \sample_data_reg[1]_0 ;
  input \sample_data_reg[1]_1 ;
  input \sample_data_reg[2]_0 ;
  input \sample_data_reg[2]_1 ;
  input \sample_data_reg[3]_0 ;
  input \sample_data_reg[3]_1 ;
  input \sample_data_reg[4]_0 ;
  input \sample_data_reg[4]_1 ;
  input \sample_data_reg[5]_0 ;
  input \sample_data_reg[5]_1 ;
  input \sample_data_reg[6]_0 ;
  input \sample_data_reg[6]_1 ;
  input \sample_data_reg[7]_0 ;
  input \sample_data_reg[7]_1 ;
  input \sample_data_reg[8]_0 ;
  input \sample_data_reg[8]_1 ;
  input \sample_data_reg[9]_0 ;
  input \sample_data_reg[9]_1 ;
  input \sample_data_reg[10]_0 ;
  input \sample_data_reg[10]_1 ;
  input \sample_data_reg[11]_0 ;
  input \sample_data_reg[11]_1 ;
  input \sample_data_reg[12]_0 ;
  input \sample_data_reg[12]_1 ;
  input \sample_data_reg[13]_0 ;
  input \sample_data_reg[13]_1 ;
  input \sample_data_reg[14]_0 ;
  input \sample_data_reg[14]_1 ;
  input \sample_data_reg[15]_0 ;
  input \sample_data_reg[15]_1 ;
  input \sample_data_reg[16]_0 ;
  input \sample_data_reg[16]_1 ;
  input \sample_data_reg[17]_0 ;
  input \sample_data_reg[17]_1 ;
  input \sample_data_reg[18]_0 ;
  input \sample_data_reg[18]_1 ;
  input \sample_data_reg[19]_0 ;
  input \sample_data_reg[19]_1 ;
  input \sample_data_reg[20]_0 ;
  input \sample_data_reg[20]_1 ;
  input \sample_data_reg[21]_0 ;
  input \sample_data_reg[21]_1 ;
  input \sample_data_reg[22]_0 ;
  input \sample_data_reg[22]_1 ;
  input \sample_data_reg[23]_0 ;
  input \sample_data_reg[23]_1 ;
  input \sample_data_reg[24]_0 ;
  input \sample_data_reg[24]_1 ;
  input \sample_data_reg[25]_0 ;
  input \sample_data_reg[25]_1 ;
  input \sample_data_reg[26]_0 ;
  input \sample_data_reg[26]_1 ;
  input \sample_data_reg[27]_0 ;
  input \sample_data_reg[27]_1 ;
  input \sample_data_reg[28]_0 ;
  input \sample_data_reg[28]_1 ;
  input \sample_data_reg[29]_0 ;
  input \sample_data_reg[29]_1 ;
  input \sample_data_reg[30]_0 ;
  input \sample_data_reg[30]_1 ;
  input \sample_data_reg[31]_2 ;
  input \sample_data_reg[31]_3 ;
  input s_axi_bready;
  input [0:0]\axi_rdata_reg[31]_0 ;
  input [31:0]\axi_rdata_reg[31]_1 ;

  wire \Count_Out_i[31]_i_4_n_0 ;
  wire [0:0]\Count_Out_i_reg[0] ;
  wire \Count_Out_i_reg[0]_0 ;
  wire [31:0]D;
  wire [0:0]E;
  wire [0:0]Q;
  wire axi_arready0;
  wire axi_arready_reg_0;
  wire axi_awready0;
  wire axi_awready_reg_0;
  wire axi_bvalid_i_1_n_0;
  wire \axi_rdata_reg[0]_0 ;
  wire [0:0]\axi_rdata_reg[31]_0 ;
  wire [31:0]\axi_rdata_reg[31]_1 ;
  wire axi_rvalid_reg_0;
  wire axi_wready0;
  wire axi_wready_reg_0;
  wire control_wr_en;
  wire metrics_cnt_en_i_2_n_0;
  wire metrics_cnt_en_i_4_n_0;
  wire mon_clk;
  wire mon_resetn;
  wire [0:0]mon_resetn_0;
  wire p_1_in;
  wire [0:0]register_select;
  wire \register_select[1]_i_3_n_0 ;
  wire [0:0]\register_select_reg[1] ;
  wire reset_sample_reg__0;
  wire [7:0]s_axi_araddr;
  wire s_axi_arvalid;
  wire [7:0]s_axi_awaddr;
  wire [0:0]\s_axi_awaddr[3] ;
  wire [2:0]\s_axi_awaddr[4] ;
  wire [3:0]\s_axi_awaddr[7] ;
  wire s_axi_awvalid;
  wire s_axi_bready;
  wire s_axi_bvalid;
  wire [31:0]s_axi_rdata;
  wire s_axi_rvalid;
  wire s_axi_wvalid;
  wire \sample_data[0]_i_2_n_0 ;
  wire \sample_data[0]_i_4_n_0 ;
  wire \sample_data[10]_i_2_n_0 ;
  wire \sample_data[10]_i_4_n_0 ;
  wire \sample_data[11]_i_2_n_0 ;
  wire \sample_data[11]_i_4_n_0 ;
  wire \sample_data[12]_i_2_n_0 ;
  wire \sample_data[12]_i_4_n_0 ;
  wire \sample_data[13]_i_2_n_0 ;
  wire \sample_data[13]_i_4_n_0 ;
  wire \sample_data[14]_i_2_n_0 ;
  wire \sample_data[14]_i_4_n_0 ;
  wire \sample_data[15]_i_2_n_0 ;
  wire \sample_data[15]_i_4_n_0 ;
  wire \sample_data[16]_i_2_n_0 ;
  wire \sample_data[16]_i_4_n_0 ;
  wire \sample_data[17]_i_2_n_0 ;
  wire \sample_data[17]_i_4_n_0 ;
  wire \sample_data[18]_i_2_n_0 ;
  wire \sample_data[18]_i_4_n_0 ;
  wire \sample_data[19]_i_2_n_0 ;
  wire \sample_data[19]_i_4_n_0 ;
  wire \sample_data[1]_i_2_n_0 ;
  wire \sample_data[1]_i_4_n_0 ;
  wire \sample_data[20]_i_2_n_0 ;
  wire \sample_data[20]_i_4_n_0 ;
  wire \sample_data[21]_i_2_n_0 ;
  wire \sample_data[21]_i_4_n_0 ;
  wire \sample_data[22]_i_2_n_0 ;
  wire \sample_data[22]_i_4_n_0 ;
  wire \sample_data[23]_i_2_n_0 ;
  wire \sample_data[23]_i_4_n_0 ;
  wire \sample_data[24]_i_2_n_0 ;
  wire \sample_data[24]_i_4_n_0 ;
  wire \sample_data[25]_i_2_n_0 ;
  wire \sample_data[25]_i_4_n_0 ;
  wire \sample_data[26]_i_2_n_0 ;
  wire \sample_data[26]_i_4_n_0 ;
  wire \sample_data[27]_i_2_n_0 ;
  wire \sample_data[27]_i_4_n_0 ;
  wire \sample_data[28]_i_2_n_0 ;
  wire \sample_data[28]_i_4_n_0 ;
  wire \sample_data[29]_i_2_n_0 ;
  wire \sample_data[29]_i_4_n_0 ;
  wire \sample_data[2]_i_2_n_0 ;
  wire \sample_data[2]_i_4_n_0 ;
  wire \sample_data[30]_i_2_n_0 ;
  wire \sample_data[30]_i_4_n_0 ;
  wire \sample_data[31]_i_10_n_0 ;
  wire \sample_data[31]_i_11_n_0 ;
  wire \sample_data[31]_i_12_n_0 ;
  wire \sample_data[31]_i_16_n_0 ;
  wire \sample_data[31]_i_20_n_0 ;
  wire \sample_data[31]_i_4_n_0 ;
  wire \sample_data[31]_i_5_n_0 ;
  wire \sample_data[31]_i_7_n_0 ;
  wire \sample_data[31]_i_8_n_0 ;
  wire \sample_data[31]_i_9_n_0 ;
  wire \sample_data[3]_i_2_n_0 ;
  wire \sample_data[3]_i_4_n_0 ;
  wire \sample_data[4]_i_2_n_0 ;
  wire \sample_data[4]_i_4_n_0 ;
  wire \sample_data[5]_i_2_n_0 ;
  wire \sample_data[5]_i_4_n_0 ;
  wire \sample_data[6]_i_2_n_0 ;
  wire \sample_data[6]_i_4_n_0 ;
  wire \sample_data[7]_i_2_n_0 ;
  wire \sample_data[7]_i_4_n_0 ;
  wire \sample_data[8]_i_2_n_0 ;
  wire \sample_data[8]_i_4_n_0 ;
  wire \sample_data[9]_i_2_n_0 ;
  wire \sample_data[9]_i_4_n_0 ;
  wire \sample_data_reg[0] ;
  wire \sample_data_reg[0]_0 ;
  wire \sample_data_reg[0]_1 ;
  wire \sample_data_reg[10] ;
  wire \sample_data_reg[10]_0 ;
  wire \sample_data_reg[10]_1 ;
  wire \sample_data_reg[11] ;
  wire \sample_data_reg[11]_0 ;
  wire \sample_data_reg[11]_1 ;
  wire \sample_data_reg[12] ;
  wire \sample_data_reg[12]_0 ;
  wire \sample_data_reg[12]_1 ;
  wire \sample_data_reg[13] ;
  wire \sample_data_reg[13]_0 ;
  wire \sample_data_reg[13]_1 ;
  wire \sample_data_reg[14] ;
  wire \sample_data_reg[14]_0 ;
  wire \sample_data_reg[14]_1 ;
  wire \sample_data_reg[15] ;
  wire \sample_data_reg[15]_0 ;
  wire \sample_data_reg[15]_1 ;
  wire \sample_data_reg[16] ;
  wire \sample_data_reg[16]_0 ;
  wire \sample_data_reg[16]_1 ;
  wire \sample_data_reg[17] ;
  wire \sample_data_reg[17]_0 ;
  wire \sample_data_reg[17]_1 ;
  wire \sample_data_reg[18] ;
  wire \sample_data_reg[18]_0 ;
  wire \sample_data_reg[18]_1 ;
  wire \sample_data_reg[19] ;
  wire \sample_data_reg[19]_0 ;
  wire \sample_data_reg[19]_1 ;
  wire \sample_data_reg[1] ;
  wire \sample_data_reg[1]_0 ;
  wire \sample_data_reg[1]_1 ;
  wire \sample_data_reg[20] ;
  wire \sample_data_reg[20]_0 ;
  wire \sample_data_reg[20]_1 ;
  wire \sample_data_reg[21] ;
  wire \sample_data_reg[21]_0 ;
  wire \sample_data_reg[21]_1 ;
  wire \sample_data_reg[22] ;
  wire \sample_data_reg[22]_0 ;
  wire \sample_data_reg[22]_1 ;
  wire \sample_data_reg[23] ;
  wire \sample_data_reg[23]_0 ;
  wire \sample_data_reg[23]_1 ;
  wire \sample_data_reg[24] ;
  wire \sample_data_reg[24]_0 ;
  wire \sample_data_reg[24]_1 ;
  wire \sample_data_reg[25] ;
  wire \sample_data_reg[25]_0 ;
  wire \sample_data_reg[25]_1 ;
  wire \sample_data_reg[26] ;
  wire \sample_data_reg[26]_0 ;
  wire \sample_data_reg[26]_1 ;
  wire \sample_data_reg[27] ;
  wire \sample_data_reg[27]_0 ;
  wire \sample_data_reg[27]_1 ;
  wire \sample_data_reg[28] ;
  wire \sample_data_reg[28]_0 ;
  wire \sample_data_reg[28]_1 ;
  wire \sample_data_reg[29] ;
  wire \sample_data_reg[29]_0 ;
  wire \sample_data_reg[29]_1 ;
  wire \sample_data_reg[2] ;
  wire \sample_data_reg[2]_0 ;
  wire \sample_data_reg[2]_1 ;
  wire \sample_data_reg[30] ;
  wire \sample_data_reg[30]_0 ;
  wire \sample_data_reg[30]_1 ;
  wire \sample_data_reg[31] ;
  wire [63:0]\sample_data_reg[31]_0 ;
  wire [63:0]\sample_data_reg[31]_1 ;
  wire \sample_data_reg[31]_2 ;
  wire \sample_data_reg[31]_3 ;
  wire \sample_data_reg[3] ;
  wire \sample_data_reg[3]_0 ;
  wire \sample_data_reg[3]_1 ;
  wire \sample_data_reg[4] ;
  wire \sample_data_reg[4]_0 ;
  wire \sample_data_reg[4]_1 ;
  wire \sample_data_reg[5] ;
  wire \sample_data_reg[5]_0 ;
  wire \sample_data_reg[5]_1 ;
  wire \sample_data_reg[6] ;
  wire \sample_data_reg[6]_0 ;
  wire \sample_data_reg[6]_1 ;
  wire \sample_data_reg[7] ;
  wire \sample_data_reg[7]_0 ;
  wire \sample_data_reg[7]_1 ;
  wire \sample_data_reg[8] ;
  wire \sample_data_reg[8]_0 ;
  wire \sample_data_reg[8]_1 ;
  wire \sample_data_reg[9] ;
  wire \sample_data_reg[9]_0 ;
  wire \sample_data_reg[9]_1 ;
  wire sample_reg_rd_first;
  wire sample_reg_rd_first_reg;
  wire [7:5]slv_reg_addr;
  wire slv_reg_addr_vld;
  wire slv_reg_out_vld;
  wire slv_reg_rden__0;
  wire slv_reg_wren__1;
  wire \trace_control[5]_i_2_n_0 ;

  (* SOFT_HLUTNM = "soft_lutpair27" *) 
  LUT2 #(
    .INIT(4'hB)) 
    \Count_Out_i[0]_i_1 
       (.I0(reset_sample_reg__0),
        .I1(Q),
        .O(\Count_Out_i_reg[0] ));
  (* SOFT_HLUTNM = "soft_lutpair27" *) 
  LUT2 #(
    .INIT(4'hE)) 
    \Count_Out_i[31]_i_1 
       (.I0(reset_sample_reg__0),
        .I1(sample_reg_rd_first),
        .O(E));
  LUT6 #(
    .INIT(64'h0080000000000000)) 
    \Count_Out_i[31]_i_3 
       (.I0(\register_select[1]_i_3_n_0 ),
        .I1(\trace_control[5]_i_2_n_0 ),
        .I2(slv_reg_addr_vld),
        .I3(\s_axi_awaddr[4] [2]),
        .I4(\Count_Out_i[31]_i_4_n_0 ),
        .I5(\Count_Out_i_reg[0]_0 ),
        .O(reset_sample_reg__0));
  LUT6 #(
    .INIT(64'hF00FFC0FF0AFFCAF)) 
    \Count_Out_i[31]_i_4 
       (.I0(s_axi_awaddr[5]),
        .I1(s_axi_araddr[5]),
        .I2(slv_reg_wren__1),
        .I3(slv_reg_rden__0),
        .I4(s_axi_araddr[6]),
        .I5(s_axi_awaddr[6]),
        .O(\Count_Out_i[31]_i_4_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair23" *) 
  LUT2 #(
    .INIT(4'h2)) 
    axi_arready_i_1
       (.I0(s_axi_arvalid),
        .I1(axi_arready_reg_0),
        .O(axi_arready0));
  FDRE #(
    .INIT(1'b0)) 
    axi_arready_reg
       (.C(mon_clk),
        .CE(1'b1),
        .D(axi_arready0),
        .Q(axi_arready_reg_0),
        .R(\axi_rdata_reg[0]_0 ));
  (* SOFT_HLUTNM = "soft_lutpair26" *) 
  LUT3 #(
    .INIT(8'h08)) 
    axi_awready_i_2
       (.I0(s_axi_wvalid),
        .I1(s_axi_awvalid),
        .I2(axi_awready_reg_0),
        .O(axi_awready0));
  FDRE #(
    .INIT(1'b0)) 
    axi_awready_reg
       (.C(mon_clk),
        .CE(1'b1),
        .D(axi_awready0),
        .Q(axi_awready_reg_0),
        .R(\axi_rdata_reg[0]_0 ));
  LUT6 #(
    .INIT(64'h7444444444444444)) 
    axi_bvalid_i_1
       (.I0(s_axi_bready),
        .I1(s_axi_bvalid),
        .I2(axi_wready_reg_0),
        .I3(axi_awready_reg_0),
        .I4(s_axi_wvalid),
        .I5(s_axi_awvalid),
        .O(axi_bvalid_i_1_n_0));
  FDRE #(
    .INIT(1'b0)) 
    axi_bvalid_reg
       (.C(mon_clk),
        .CE(1'b1),
        .D(axi_bvalid_i_1_n_0),
        .Q(s_axi_bvalid),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[0] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [0]),
        .Q(s_axi_rdata[0]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[10] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [10]),
        .Q(s_axi_rdata[10]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[11] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [11]),
        .Q(s_axi_rdata[11]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[12] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [12]),
        .Q(s_axi_rdata[12]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[13] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [13]),
        .Q(s_axi_rdata[13]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[14] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [14]),
        .Q(s_axi_rdata[14]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[15] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [15]),
        .Q(s_axi_rdata[15]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[16] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [16]),
        .Q(s_axi_rdata[16]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[17] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [17]),
        .Q(s_axi_rdata[17]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[18] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [18]),
        .Q(s_axi_rdata[18]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[19] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [19]),
        .Q(s_axi_rdata[19]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[1] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [1]),
        .Q(s_axi_rdata[1]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[20] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [20]),
        .Q(s_axi_rdata[20]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[21] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [21]),
        .Q(s_axi_rdata[21]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[22] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [22]),
        .Q(s_axi_rdata[22]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[23] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [23]),
        .Q(s_axi_rdata[23]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[24] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [24]),
        .Q(s_axi_rdata[24]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[25] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [25]),
        .Q(s_axi_rdata[25]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[26] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [26]),
        .Q(s_axi_rdata[26]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[27] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [27]),
        .Q(s_axi_rdata[27]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[28] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [28]),
        .Q(s_axi_rdata[28]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[29] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [29]),
        .Q(s_axi_rdata[29]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[2] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [2]),
        .Q(s_axi_rdata[2]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[30] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [30]),
        .Q(s_axi_rdata[30]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[31] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [31]),
        .Q(s_axi_rdata[31]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[3] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [3]),
        .Q(s_axi_rdata[3]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[4] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [4]),
        .Q(s_axi_rdata[4]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[5] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [5]),
        .Q(s_axi_rdata[5]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[6] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [6]),
        .Q(s_axi_rdata[6]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[7] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [7]),
        .Q(s_axi_rdata[7]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[8] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [8]),
        .Q(s_axi_rdata[8]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \axi_rdata_reg[9] 
       (.C(mon_clk),
        .CE(\axi_rdata_reg[31]_0 ),
        .D(\axi_rdata_reg[31]_1 [9]),
        .Q(s_axi_rdata[9]),
        .R(\axi_rdata_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    axi_rvalid_reg
       (.C(mon_clk),
        .CE(1'b1),
        .D(axi_rvalid_reg_0),
        .Q(s_axi_rvalid),
        .R(\axi_rdata_reg[0]_0 ));
  (* SOFT_HLUTNM = "soft_lutpair26" *) 
  LUT3 #(
    .INIT(8'h08)) 
    axi_wready_i_1
       (.I0(s_axi_wvalid),
        .I1(s_axi_awvalid),
        .I2(axi_wready_reg_0),
        .O(axi_wready0));
  FDRE #(
    .INIT(1'b0)) 
    axi_wready_reg
       (.C(mon_clk),
        .CE(1'b1),
        .D(axi_wready0),
        .Q(axi_wready_reg_0),
        .R(\axi_rdata_reg[0]_0 ));
  (* SOFT_HLUTNM = "soft_lutpair25" *) 
  LUT4 #(
    .INIT(16'h0800)) 
    metrics_cnt_en_i_1
       (.I0(metrics_cnt_en_i_2_n_0),
        .I1(slv_reg_wren__1),
        .I2(\s_axi_awaddr[4] [2]),
        .I3(metrics_cnt_en_i_4_n_0),
        .O(control_wr_en));
  LUT6 #(
    .INIT(64'h00000000FCBBBBBB)) 
    metrics_cnt_en_i_2
       (.I0(s_axi_awaddr[3]),
        .I1(slv_reg_wren__1),
        .I2(s_axi_araddr[3]),
        .I3(s_axi_arvalid),
        .I4(axi_arready_reg_0),
        .I5(\s_axi_awaddr[4] [0]),
        .O(metrics_cnt_en_i_2_n_0));
  (* SOFT_HLUTNM = "soft_lutpair24" *) 
  LUT4 #(
    .INIT(16'h8000)) 
    metrics_cnt_en_i_3
       (.I0(axi_wready_reg_0),
        .I1(axi_awready_reg_0),
        .I2(s_axi_wvalid),
        .I3(s_axi_awvalid),
        .O(slv_reg_wren__1));
  LUT6 #(
    .INIT(64'h0004440400000000)) 
    metrics_cnt_en_i_4
       (.I0(slv_reg_addr[6]),
        .I1(\sample_data[31]_i_8_n_0 ),
        .I2(s_axi_araddr[5]),
        .I3(slv_reg_wren__1),
        .I4(s_axi_awaddr[5]),
        .I5(\register_select[1]_i_3_n_0 ),
        .O(metrics_cnt_en_i_4_n_0));
  LUT6 #(
    .INIT(64'h0000AA8000000000)) 
    \register_select[0]_i_1 
       (.I0(\trace_control[5]_i_2_n_0 ),
        .I1(axi_arready_reg_0),
        .I2(s_axi_arvalid),
        .I3(slv_reg_wren__1),
        .I4(\s_axi_awaddr[4] [2]),
        .I5(metrics_cnt_en_i_4_n_0),
        .O(\s_axi_awaddr[7] [0]));
  LUT6 #(
    .INIT(64'h0200000000000000)) 
    \register_select[1]_i_1 
       (.I0(slv_reg_addr[5]),
        .I1(slv_reg_addr[6]),
        .I2(\s_axi_awaddr[4] [2]),
        .I3(slv_reg_addr_vld),
        .I4(\trace_control[5]_i_2_n_0 ),
        .I5(\register_select[1]_i_3_n_0 ),
        .O(p_1_in));
  LUT5 #(
    .INIT(32'hFFF788F7)) 
    \register_select[1]_i_2 
       (.I0(axi_arready_reg_0),
        .I1(s_axi_arvalid),
        .I2(s_axi_araddr[5]),
        .I3(slv_reg_wren__1),
        .I4(s_axi_awaddr[5]),
        .O(slv_reg_addr[5]));
  LUT6 #(
    .INIT(64'h000A202020202020)) 
    \register_select[1]_i_3 
       (.I0(\sample_data[31]_i_9_n_0 ),
        .I1(s_axi_awaddr[7]),
        .I2(slv_reg_wren__1),
        .I3(s_axi_araddr[7]),
        .I4(s_axi_arvalid),
        .I5(axi_arready_reg_0),
        .O(\register_select[1]_i_3_n_0 ));
  LUT6 #(
    .INIT(64'h8888800000000000)) 
    \register_select[3]_i_1 
       (.I0(\trace_control[5]_i_2_n_0 ),
        .I1(\s_axi_awaddr[4] [2]),
        .I2(axi_arready_reg_0),
        .I3(s_axi_arvalid),
        .I4(slv_reg_wren__1),
        .I5(metrics_cnt_en_i_4_n_0),
        .O(\s_axi_awaddr[7] [1]));
  LUT6 #(
    .INIT(64'h0000AA8000000000)) 
    \register_select[4]_i_1 
       (.I0(metrics_cnt_en_i_2_n_0),
        .I1(axi_arready_reg_0),
        .I2(s_axi_arvalid),
        .I3(slv_reg_wren__1),
        .I4(\s_axi_awaddr[4] [2]),
        .I5(metrics_cnt_en_i_4_n_0),
        .O(\s_axi_awaddr[7] [2]));
  (* SOFT_HLUTNM = "soft_lutpair21" *) 
  LUT5 #(
    .INIT(32'hFC888888)) 
    \register_select[5]_i_1 
       (.I0(s_axi_awaddr[7]),
        .I1(slv_reg_wren__1),
        .I2(s_axi_araddr[7]),
        .I3(s_axi_arvalid),
        .I4(axi_arready_reg_0),
        .O(\s_axi_awaddr[7] [3]));
  (* SOFT_HLUTNM = "soft_lutpair28" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \sample_ctr_val[63]_i_1 
       (.I0(p_1_in),
        .I1(register_select),
        .O(\register_select_reg[1] ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[0]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[0]_i_2_n_0 ),
        .I4(\sample_data_reg[0] ),
        .I5(\sample_data[0]_i_4_n_0 ),
        .O(D[0]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[0]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [0]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [0]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[0]_0 ),
        .O(\sample_data[0]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[0]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [32]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [32]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[0]_1 ),
        .O(\sample_data[0]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[10]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[10]_i_2_n_0 ),
        .I4(\sample_data_reg[10] ),
        .I5(\sample_data[10]_i_4_n_0 ),
        .O(D[10]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[10]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [10]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [10]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[10]_0 ),
        .O(\sample_data[10]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[10]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [42]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [42]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[10]_1 ),
        .O(\sample_data[10]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[11]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[11]_i_2_n_0 ),
        .I4(\sample_data_reg[11] ),
        .I5(\sample_data[11]_i_4_n_0 ),
        .O(D[11]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[11]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [11]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [11]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[11]_0 ),
        .O(\sample_data[11]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[11]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [43]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [43]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[11]_1 ),
        .O(\sample_data[11]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[12]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[12]_i_2_n_0 ),
        .I4(\sample_data_reg[12] ),
        .I5(\sample_data[12]_i_4_n_0 ),
        .O(D[12]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[12]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [12]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [12]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[12]_0 ),
        .O(\sample_data[12]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[12]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [44]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [44]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[12]_1 ),
        .O(\sample_data[12]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[13]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[13]_i_2_n_0 ),
        .I4(\sample_data_reg[13] ),
        .I5(\sample_data[13]_i_4_n_0 ),
        .O(D[13]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[13]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [13]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [13]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[13]_0 ),
        .O(\sample_data[13]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[13]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [45]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [45]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[13]_1 ),
        .O(\sample_data[13]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[14]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[14]_i_2_n_0 ),
        .I4(\sample_data_reg[14] ),
        .I5(\sample_data[14]_i_4_n_0 ),
        .O(D[14]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[14]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [14]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [14]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[14]_0 ),
        .O(\sample_data[14]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[14]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [46]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [46]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[14]_1 ),
        .O(\sample_data[14]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[15]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[15]_i_2_n_0 ),
        .I4(\sample_data_reg[15] ),
        .I5(\sample_data[15]_i_4_n_0 ),
        .O(D[15]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[15]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [15]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [15]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[15]_0 ),
        .O(\sample_data[15]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[15]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [47]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [47]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[15]_1 ),
        .O(\sample_data[15]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[16]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[16]_i_2_n_0 ),
        .I4(\sample_data_reg[16] ),
        .I5(\sample_data[16]_i_4_n_0 ),
        .O(D[16]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[16]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [16]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [16]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[16]_0 ),
        .O(\sample_data[16]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[16]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [48]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [48]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[16]_1 ),
        .O(\sample_data[16]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[17]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[17]_i_2_n_0 ),
        .I4(\sample_data_reg[17] ),
        .I5(\sample_data[17]_i_4_n_0 ),
        .O(D[17]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[17]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [17]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [17]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[17]_0 ),
        .O(\sample_data[17]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[17]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [49]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [49]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[17]_1 ),
        .O(\sample_data[17]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[18]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[18]_i_2_n_0 ),
        .I4(\sample_data_reg[18] ),
        .I5(\sample_data[18]_i_4_n_0 ),
        .O(D[18]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[18]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [18]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [18]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[18]_0 ),
        .O(\sample_data[18]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[18]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [50]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [50]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[18]_1 ),
        .O(\sample_data[18]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[19]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[19]_i_2_n_0 ),
        .I4(\sample_data_reg[19] ),
        .I5(\sample_data[19]_i_4_n_0 ),
        .O(D[19]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[19]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [19]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [19]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[19]_0 ),
        .O(\sample_data[19]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[19]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [51]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [51]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[19]_1 ),
        .O(\sample_data[19]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[1]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[1]_i_2_n_0 ),
        .I4(\sample_data_reg[1] ),
        .I5(\sample_data[1]_i_4_n_0 ),
        .O(D[1]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[1]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [1]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [1]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[1]_0 ),
        .O(\sample_data[1]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[1]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [33]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [33]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[1]_1 ),
        .O(\sample_data[1]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[20]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[20]_i_2_n_0 ),
        .I4(\sample_data_reg[20] ),
        .I5(\sample_data[20]_i_4_n_0 ),
        .O(D[20]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[20]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [20]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [20]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[20]_0 ),
        .O(\sample_data[20]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[20]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [52]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [52]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[20]_1 ),
        .O(\sample_data[20]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[21]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[21]_i_2_n_0 ),
        .I4(\sample_data_reg[21] ),
        .I5(\sample_data[21]_i_4_n_0 ),
        .O(D[21]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[21]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [21]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [21]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[21]_0 ),
        .O(\sample_data[21]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[21]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [53]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [53]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[21]_1 ),
        .O(\sample_data[21]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[22]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[22]_i_2_n_0 ),
        .I4(\sample_data_reg[22] ),
        .I5(\sample_data[22]_i_4_n_0 ),
        .O(D[22]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[22]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [22]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [22]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[22]_0 ),
        .O(\sample_data[22]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[22]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [54]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [54]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[22]_1 ),
        .O(\sample_data[22]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[23]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[23]_i_2_n_0 ),
        .I4(\sample_data_reg[23] ),
        .I5(\sample_data[23]_i_4_n_0 ),
        .O(D[23]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[23]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [23]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [23]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[23]_0 ),
        .O(\sample_data[23]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[23]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [55]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [55]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[23]_1 ),
        .O(\sample_data[23]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[24]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[24]_i_2_n_0 ),
        .I4(\sample_data_reg[24] ),
        .I5(\sample_data[24]_i_4_n_0 ),
        .O(D[24]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[24]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [24]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [24]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[24]_0 ),
        .O(\sample_data[24]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[24]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [56]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [56]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[24]_1 ),
        .O(\sample_data[24]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[25]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[25]_i_2_n_0 ),
        .I4(\sample_data_reg[25] ),
        .I5(\sample_data[25]_i_4_n_0 ),
        .O(D[25]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[25]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [25]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [25]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[25]_0 ),
        .O(\sample_data[25]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[25]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [57]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [57]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[25]_1 ),
        .O(\sample_data[25]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[26]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[26]_i_2_n_0 ),
        .I4(\sample_data_reg[26] ),
        .I5(\sample_data[26]_i_4_n_0 ),
        .O(D[26]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[26]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [26]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [26]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[26]_0 ),
        .O(\sample_data[26]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[26]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [58]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [58]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[26]_1 ),
        .O(\sample_data[26]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[27]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[27]_i_2_n_0 ),
        .I4(\sample_data_reg[27] ),
        .I5(\sample_data[27]_i_4_n_0 ),
        .O(D[27]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[27]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [27]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [27]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[27]_0 ),
        .O(\sample_data[27]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[27]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [59]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [59]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[27]_1 ),
        .O(\sample_data[27]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[28]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[28]_i_2_n_0 ),
        .I4(\sample_data_reg[28] ),
        .I5(\sample_data[28]_i_4_n_0 ),
        .O(D[28]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[28]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [28]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [28]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[28]_0 ),
        .O(\sample_data[28]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[28]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [60]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [60]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[28]_1 ),
        .O(\sample_data[28]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[29]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[29]_i_2_n_0 ),
        .I4(\sample_data_reg[29] ),
        .I5(\sample_data[29]_i_4_n_0 ),
        .O(D[29]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[29]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [29]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [29]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[29]_0 ),
        .O(\sample_data[29]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[29]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [61]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [61]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[29]_1 ),
        .O(\sample_data[29]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[2]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[2]_i_2_n_0 ),
        .I4(\sample_data_reg[2] ),
        .I5(\sample_data[2]_i_4_n_0 ),
        .O(D[2]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[2]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [2]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [2]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[2]_0 ),
        .O(\sample_data[2]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[2]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [34]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [34]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[2]_1 ),
        .O(\sample_data[2]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[30]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[30]_i_2_n_0 ),
        .I4(\sample_data_reg[30] ),
        .I5(\sample_data[30]_i_4_n_0 ),
        .O(D[30]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[30]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [30]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [30]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[30]_0 ),
        .O(\sample_data[30]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[30]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [62]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [62]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[30]_1 ),
        .O(\sample_data[30]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[31]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[31]_i_5_n_0 ),
        .I4(\sample_data_reg[31] ),
        .I5(\sample_data[31]_i_7_n_0 ),
        .O(D[31]));
  LUT4 #(
    .INIT(16'h0040)) 
    \sample_data[31]_i_10 
       (.I0(slv_reg_addr[5]),
        .I1(\sample_data[31]_i_9_n_0 ),
        .I2(slv_reg_addr[7]),
        .I3(slv_reg_addr[6]),
        .O(\sample_data[31]_i_10_n_0 ));
  LUT3 #(
    .INIT(8'hA8)) 
    \sample_data[31]_i_11 
       (.I0(\sample_data[31]_i_20_n_0 ),
        .I1(\s_axi_awaddr[4] [2]),
        .I2(\s_axi_awaddr[4] [1]),
        .O(\sample_data[31]_i_11_n_0 ));
  LUT3 #(
    .INIT(8'h08)) 
    \sample_data[31]_i_12 
       (.I0(\s_axi_awaddr[4] [1]),
        .I1(\s_axi_awaddr[4] [2]),
        .I2(\s_axi_awaddr[4] [0]),
        .O(\sample_data[31]_i_12_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair23" *) 
  LUT5 #(
    .INIT(32'hFFF788F7)) 
    \sample_data[31]_i_14 
       (.I0(axi_arready_reg_0),
        .I1(s_axi_arvalid),
        .I2(s_axi_araddr[2]),
        .I3(slv_reg_wren__1),
        .I4(s_axi_awaddr[2]),
        .O(\s_axi_awaddr[4] [0]));
  (* SOFT_HLUTNM = "soft_lutpair22" *) 
  LUT5 #(
    .INIT(32'hFFF788F7)) 
    \sample_data[31]_i_15 
       (.I0(axi_arready_reg_0),
        .I1(s_axi_arvalid),
        .I2(s_axi_araddr[3]),
        .I3(slv_reg_wren__1),
        .I4(s_axi_awaddr[3]),
        .O(\s_axi_awaddr[4] [1]));
  LUT6 #(
    .INIT(64'hAAA222A200000000)) 
    \sample_data[31]_i_16 
       (.I0(\Count_Out_i[31]_i_4_n_0 ),
        .I1(\sample_data[31]_i_8_n_0 ),
        .I2(s_axi_araddr[7]),
        .I3(slv_reg_wren__1),
        .I4(s_axi_awaddr[7]),
        .I5(\sample_data[31]_i_9_n_0 ),
        .O(\sample_data[31]_i_16_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair22" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \sample_data[31]_i_18 
       (.I0(axi_arready_reg_0),
        .I1(s_axi_arvalid),
        .O(slv_reg_rden__0));
  (* SOFT_HLUTNM = "soft_lutpair21" *) 
  LUT5 #(
    .INIT(32'hFFF788F7)) 
    \sample_data[31]_i_19 
       (.I0(axi_arready_reg_0),
        .I1(s_axi_arvalid),
        .I2(s_axi_araddr[7]),
        .I3(slv_reg_wren__1),
        .I4(s_axi_awaddr[7]),
        .O(slv_reg_addr[7]));
  LUT5 #(
    .INIT(32'h0AAAC000)) 
    \sample_data[31]_i_2 
       (.I0(s_axi_awaddr[4]),
        .I1(s_axi_araddr[4]),
        .I2(s_axi_arvalid),
        .I3(axi_arready_reg_0),
        .I4(slv_reg_wren__1),
        .O(\s_axi_awaddr[4] [2]));
  LUT6 #(
    .INIT(64'h0A000ACC00000000)) 
    \sample_data[31]_i_20 
       (.I0(s_axi_awaddr[2]),
        .I1(s_axi_araddr[2]),
        .I2(s_axi_awaddr[3]),
        .I3(slv_reg_wren__1),
        .I4(s_axi_araddr[3]),
        .I5(\sample_data[31]_i_8_n_0 ),
        .O(\sample_data[31]_i_20_n_0 ));
  LUT5 #(
    .INIT(32'h0AAAC000)) 
    \sample_data[31]_i_3 
       (.I0(s_axi_awaddr[6]),
        .I1(s_axi_araddr[6]),
        .I2(s_axi_arvalid),
        .I3(axi_arready_reg_0),
        .I4(slv_reg_wren__1),
        .O(slv_reg_addr[6]));
  LUT6 #(
    .INIT(64'h00000000FD5D0000)) 
    \sample_data[31]_i_4 
       (.I0(\sample_data[31]_i_8_n_0 ),
        .I1(s_axi_araddr[7]),
        .I2(slv_reg_wren__1),
        .I3(s_axi_awaddr[7]),
        .I4(\sample_data[31]_i_9_n_0 ),
        .I5(slv_reg_addr[5]),
        .O(\sample_data[31]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[31]_i_5 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [31]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [31]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[31]_2 ),
        .O(\sample_data[31]_i_5_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[31]_i_7 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [63]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [63]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[31]_3 ),
        .O(\sample_data[31]_i_7_n_0 ));
  LUT6 #(
    .INIT(64'h7888888888888888)) 
    \sample_data[31]_i_8 
       (.I0(axi_arready_reg_0),
        .I1(s_axi_arvalid),
        .I2(axi_wready_reg_0),
        .I3(axi_awready_reg_0),
        .I4(s_axi_wvalid),
        .I5(s_axi_awvalid),
        .O(\sample_data[31]_i_8_n_0 ));
  LUT6 #(
    .INIT(64'h0000005503000300)) 
    \sample_data[31]_i_9 
       (.I0(s_axi_araddr[1]),
        .I1(s_axi_awaddr[1]),
        .I2(s_axi_awaddr[0]),
        .I3(slv_reg_wren__1),
        .I4(s_axi_araddr[0]),
        .I5(slv_reg_rden__0),
        .O(\sample_data[31]_i_9_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[3]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[3]_i_2_n_0 ),
        .I4(\sample_data_reg[3] ),
        .I5(\sample_data[3]_i_4_n_0 ),
        .O(D[3]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[3]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [3]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [3]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[3]_0 ),
        .O(\sample_data[3]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[3]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [35]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [35]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[3]_1 ),
        .O(\sample_data[3]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[4]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[4]_i_2_n_0 ),
        .I4(\sample_data_reg[4] ),
        .I5(\sample_data[4]_i_4_n_0 ),
        .O(D[4]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[4]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [4]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [4]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[4]_0 ),
        .O(\sample_data[4]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[4]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [36]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [36]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[4]_1 ),
        .O(\sample_data[4]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[5]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[5]_i_2_n_0 ),
        .I4(\sample_data_reg[5] ),
        .I5(\sample_data[5]_i_4_n_0 ),
        .O(D[5]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[5]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [5]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [5]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[5]_0 ),
        .O(\sample_data[5]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[5]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [37]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [37]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[5]_1 ),
        .O(\sample_data[5]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[6]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[6]_i_2_n_0 ),
        .I4(\sample_data_reg[6] ),
        .I5(\sample_data[6]_i_4_n_0 ),
        .O(D[6]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[6]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [6]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [6]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[6]_0 ),
        .O(\sample_data[6]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[6]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [38]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [38]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[6]_1 ),
        .O(\sample_data[6]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[7]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[7]_i_2_n_0 ),
        .I4(\sample_data_reg[7] ),
        .I5(\sample_data[7]_i_4_n_0 ),
        .O(D[7]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[7]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [7]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [7]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[7]_0 ),
        .O(\sample_data[7]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[7]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [39]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [39]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[7]_1 ),
        .O(\sample_data[7]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[8]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[8]_i_2_n_0 ),
        .I4(\sample_data_reg[8] ),
        .I5(\sample_data[8]_i_4_n_0 ),
        .O(D[8]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[8]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [8]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [8]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[8]_0 ),
        .O(\sample_data[8]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[8]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [40]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [40]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[8]_1 ),
        .O(\sample_data[8]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFF40FF00)) 
    \sample_data[9]_i_1 
       (.I0(\s_axi_awaddr[4] [2]),
        .I1(slv_reg_addr[6]),
        .I2(\sample_data[31]_i_4_n_0 ),
        .I3(\sample_data[9]_i_2_n_0 ),
        .I4(\sample_data_reg[9] ),
        .I5(\sample_data[9]_i_4_n_0 ),
        .O(D[9]));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[9]_i_2 
       (.I0(\sample_data[31]_i_10_n_0 ),
        .I1(\sample_data_reg[31]_0 [9]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [9]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[9]_0 ),
        .O(\sample_data[9]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hAAAAAAAAAA808080)) 
    \sample_data[9]_i_4 
       (.I0(\sample_data[31]_i_16_n_0 ),
        .I1(\sample_data_reg[31]_0 [41]),
        .I2(\sample_data[31]_i_11_n_0 ),
        .I3(\sample_data_reg[31]_1 [41]),
        .I4(\sample_data[31]_i_12_n_0 ),
        .I5(\sample_data_reg[9]_1 ),
        .O(\sample_data[9]_i_4_n_0 ));
  LUT2 #(
    .INIT(4'hE)) 
    sample_reg_rd_first_i_1
       (.I0(p_1_in),
        .I1(sample_reg_rd_first),
        .O(sample_reg_rd_first_reg));
  (* SOFT_HLUTNM = "soft_lutpair28" *) 
  LUT2 #(
    .INIT(4'h8)) 
    \sample_time_diff_reg[31]_i_1 
       (.I0(mon_resetn),
        .I1(p_1_in),
        .O(mon_resetn_0));
  LUT6 #(
    .INIT(64'hFFFF800080008000)) 
    slv_reg_addr_vld_reg_i_1
       (.I0(s_axi_awvalid),
        .I1(s_axi_wvalid),
        .I2(axi_awready_reg_0),
        .I3(axi_wready_reg_0),
        .I4(s_axi_arvalid),
        .I5(axi_arready_reg_0),
        .O(slv_reg_addr_vld));
  (* SOFT_HLUTNM = "soft_lutpair24" *) 
  LUT4 #(
    .INIT(16'h8000)) 
    slv_reg_out_vld_reg_i_1
       (.I0(s_axi_awvalid),
        .I1(s_axi_wvalid),
        .I2(axi_awready_reg_0),
        .I3(axi_wready_reg_0),
        .O(slv_reg_out_vld));
  (* SOFT_HLUTNM = "soft_lutpair25" *) 
  LUT4 #(
    .INIT(16'h8000)) 
    \trace_control[5]_i_1 
       (.I0(\trace_control[5]_i_2_n_0 ),
        .I1(slv_reg_wren__1),
        .I2(\s_axi_awaddr[4] [2]),
        .I3(metrics_cnt_en_i_4_n_0),
        .O(\s_axi_awaddr[3] ));
  LUT6 #(
    .INIT(64'h0000000003444444)) 
    \trace_control[5]_i_2 
       (.I0(s_axi_awaddr[3]),
        .I1(slv_reg_wren__1),
        .I2(s_axi_araddr[3]),
        .I3(s_axi_arvalid),
        .I4(axi_arready_reg_0),
        .I5(\s_axi_awaddr[4] [0]),
        .O(\trace_control[5]_i_2_n_0 ));
endmodule

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_accel_counters
   (empty,
    ip_cur_tranx_reg,
    CO,
    Q,
    \max_ctr_reg[63] ,
    \sample_ctr_val_reg[32] ,
    \sample_ctr_val_reg[33] ,
    \sample_ctr_val_reg[34] ,
    \sample_ctr_val_reg[35] ,
    \sample_ctr_val_reg[36] ,
    \sample_ctr_val_reg[37] ,
    \sample_ctr_val_reg[38] ,
    \sample_ctr_val_reg[39] ,
    \sample_ctr_val_reg[40] ,
    \sample_ctr_val_reg[41] ,
    \sample_ctr_val_reg[42] ,
    \sample_ctr_val_reg[43] ,
    \sample_ctr_val_reg[44] ,
    \sample_ctr_val_reg[45] ,
    \sample_ctr_val_reg[46] ,
    \sample_ctr_val_reg[47] ,
    \sample_ctr_val_reg[48] ,
    \sample_ctr_val_reg[49] ,
    \sample_ctr_val_reg[50] ,
    \sample_ctr_val_reg[51] ,
    \sample_ctr_val_reg[52] ,
    \sample_ctr_val_reg[53] ,
    \sample_ctr_val_reg[54] ,
    \sample_ctr_val_reg[55] ,
    \sample_ctr_val_reg[56] ,
    \sample_ctr_val_reg[57] ,
    \sample_ctr_val_reg[58] ,
    \sample_ctr_val_reg[59] ,
    \sample_ctr_val_reg[60] ,
    \sample_ctr_val_reg[61] ,
    \sample_ctr_val_reg[62] ,
    \sample_ctr_val_reg[63] ,
    \ip_exec_count_reg[0]_0 ,
    \ip_exec_count_reg[32]_0 ,
    \ip_exec_count_reg[1]_0 ,
    \ip_exec_count_reg[33]_0 ,
    \ip_exec_count_reg[2]_0 ,
    \ip_exec_count_reg[34]_0 ,
    \ip_exec_count_reg[3]_0 ,
    \ip_exec_count_reg[35]_0 ,
    \ip_exec_count_reg[4]_0 ,
    \ip_exec_count_reg[36]_0 ,
    \ip_exec_count_reg[5]_0 ,
    \ip_exec_count_reg[37]_0 ,
    \ip_exec_count_reg[6]_0 ,
    \ip_exec_count_reg[38]_0 ,
    \ip_exec_count_reg[7]_0 ,
    \ip_exec_count_reg[39]_0 ,
    \ip_exec_count_reg[8]_0 ,
    \ip_exec_count_reg[40]_0 ,
    \ip_exec_count_reg[9]_0 ,
    \ip_exec_count_reg[41]_0 ,
    \ip_exec_count_reg[10]_0 ,
    \ip_exec_count_reg[42]_0 ,
    \ip_exec_count_reg[11]_0 ,
    \ip_exec_count_reg[43]_0 ,
    \ip_exec_count_reg[12]_0 ,
    \ip_exec_count_reg[44]_0 ,
    \ip_exec_count_reg[13]_0 ,
    \ip_exec_count_reg[45]_0 ,
    \ip_exec_count_reg[14]_0 ,
    \ip_exec_count_reg[46]_0 ,
    \ip_exec_count_reg[15]_0 ,
    \ip_exec_count_reg[47]_0 ,
    \ip_exec_count_reg[16]_0 ,
    \ip_exec_count_reg[48]_0 ,
    \ip_exec_count_reg[17]_0 ,
    \ip_exec_count_reg[49]_0 ,
    \ip_exec_count_reg[18]_0 ,
    \ip_exec_count_reg[50]_0 ,
    \ip_exec_count_reg[19]_0 ,
    \ip_exec_count_reg[51]_0 ,
    \ip_exec_count_reg[20]_0 ,
    \ip_exec_count_reg[52]_0 ,
    \ip_exec_count_reg[21]_0 ,
    \ip_exec_count_reg[53]_0 ,
    \ip_exec_count_reg[22]_0 ,
    \ip_exec_count_reg[54]_0 ,
    \ip_exec_count_reg[23]_0 ,
    \ip_exec_count_reg[55]_0 ,
    \ip_exec_count_reg[24]_0 ,
    \ip_exec_count_reg[56]_0 ,
    \ip_exec_count_reg[25]_0 ,
    \ip_exec_count_reg[57]_0 ,
    \ip_exec_count_reg[26]_0 ,
    \ip_exec_count_reg[58]_0 ,
    \ip_exec_count_reg[27]_0 ,
    \ip_exec_count_reg[59]_0 ,
    \ip_exec_count_reg[28]_0 ,
    \ip_exec_count_reg[60]_0 ,
    \ip_exec_count_reg[29]_0 ,
    \ip_exec_count_reg[61]_0 ,
    \ip_exec_count_reg[30]_0 ,
    \ip_exec_count_reg[62]_0 ,
    \ip_exec_count_reg[31]_0 ,
    \ip_exec_count_reg[63]_0 ,
    \sample_data_reg[31]_0 ,
    RST_ACTIVE,
    mon_clk,
    rd_en,
    O,
    \ip_cur_tranx_reg[15]_0 ,
    \ip_cur_tranx_reg[23]_0 ,
    \ip_cur_tranx_reg[31]_0 ,
    \ip_cur_tranx_reg[39]_0 ,
    \ip_cur_tranx_reg[47]_0 ,
    \ip_cur_tranx_reg[55]_0 ,
    \ip_cur_tranx_reg[63]_0 ,
    ip_exec_count0,
    ip_start_count0,
    cnt_enabled_reg,
    start_pulse,
    Metrics_Cnt_En,
    ap_done_reg,
    dataflow_en,
    ap_continue_reg,
    slv_reg_addr,
    \ip_max_parallel_tranx_reg[0]_0 ,
    E,
    D);
  output empty;
  output [63:0]ip_cur_tranx_reg;
  output [0:0]CO;
  output [63:0]Q;
  output [63:0]\max_ctr_reg[63] ;
  output \sample_ctr_val_reg[32] ;
  output \sample_ctr_val_reg[33] ;
  output \sample_ctr_val_reg[34] ;
  output \sample_ctr_val_reg[35] ;
  output \sample_ctr_val_reg[36] ;
  output \sample_ctr_val_reg[37] ;
  output \sample_ctr_val_reg[38] ;
  output \sample_ctr_val_reg[39] ;
  output \sample_ctr_val_reg[40] ;
  output \sample_ctr_val_reg[41] ;
  output \sample_ctr_val_reg[42] ;
  output \sample_ctr_val_reg[43] ;
  output \sample_ctr_val_reg[44] ;
  output \sample_ctr_val_reg[45] ;
  output \sample_ctr_val_reg[46] ;
  output \sample_ctr_val_reg[47] ;
  output \sample_ctr_val_reg[48] ;
  output \sample_ctr_val_reg[49] ;
  output \sample_ctr_val_reg[50] ;
  output \sample_ctr_val_reg[51] ;
  output \sample_ctr_val_reg[52] ;
  output \sample_ctr_val_reg[53] ;
  output \sample_ctr_val_reg[54] ;
  output \sample_ctr_val_reg[55] ;
  output \sample_ctr_val_reg[56] ;
  output \sample_ctr_val_reg[57] ;
  output \sample_ctr_val_reg[58] ;
  output \sample_ctr_val_reg[59] ;
  output \sample_ctr_val_reg[60] ;
  output \sample_ctr_val_reg[61] ;
  output \sample_ctr_val_reg[62] ;
  output \sample_ctr_val_reg[63] ;
  output \ip_exec_count_reg[0]_0 ;
  output \ip_exec_count_reg[32]_0 ;
  output \ip_exec_count_reg[1]_0 ;
  output \ip_exec_count_reg[33]_0 ;
  output \ip_exec_count_reg[2]_0 ;
  output \ip_exec_count_reg[34]_0 ;
  output \ip_exec_count_reg[3]_0 ;
  output \ip_exec_count_reg[35]_0 ;
  output \ip_exec_count_reg[4]_0 ;
  output \ip_exec_count_reg[36]_0 ;
  output \ip_exec_count_reg[5]_0 ;
  output \ip_exec_count_reg[37]_0 ;
  output \ip_exec_count_reg[6]_0 ;
  output \ip_exec_count_reg[38]_0 ;
  output \ip_exec_count_reg[7]_0 ;
  output \ip_exec_count_reg[39]_0 ;
  output \ip_exec_count_reg[8]_0 ;
  output \ip_exec_count_reg[40]_0 ;
  output \ip_exec_count_reg[9]_0 ;
  output \ip_exec_count_reg[41]_0 ;
  output \ip_exec_count_reg[10]_0 ;
  output \ip_exec_count_reg[42]_0 ;
  output \ip_exec_count_reg[11]_0 ;
  output \ip_exec_count_reg[43]_0 ;
  output \ip_exec_count_reg[12]_0 ;
  output \ip_exec_count_reg[44]_0 ;
  output \ip_exec_count_reg[13]_0 ;
  output \ip_exec_count_reg[45]_0 ;
  output \ip_exec_count_reg[14]_0 ;
  output \ip_exec_count_reg[46]_0 ;
  output \ip_exec_count_reg[15]_0 ;
  output \ip_exec_count_reg[47]_0 ;
  output \ip_exec_count_reg[16]_0 ;
  output \ip_exec_count_reg[48]_0 ;
  output \ip_exec_count_reg[17]_0 ;
  output \ip_exec_count_reg[49]_0 ;
  output \ip_exec_count_reg[18]_0 ;
  output \ip_exec_count_reg[50]_0 ;
  output \ip_exec_count_reg[19]_0 ;
  output \ip_exec_count_reg[51]_0 ;
  output \ip_exec_count_reg[20]_0 ;
  output \ip_exec_count_reg[52]_0 ;
  output \ip_exec_count_reg[21]_0 ;
  output \ip_exec_count_reg[53]_0 ;
  output \ip_exec_count_reg[22]_0 ;
  output \ip_exec_count_reg[54]_0 ;
  output \ip_exec_count_reg[23]_0 ;
  output \ip_exec_count_reg[55]_0 ;
  output \ip_exec_count_reg[24]_0 ;
  output \ip_exec_count_reg[56]_0 ;
  output \ip_exec_count_reg[25]_0 ;
  output \ip_exec_count_reg[57]_0 ;
  output \ip_exec_count_reg[26]_0 ;
  output \ip_exec_count_reg[58]_0 ;
  output \ip_exec_count_reg[27]_0 ;
  output \ip_exec_count_reg[59]_0 ;
  output \ip_exec_count_reg[28]_0 ;
  output \ip_exec_count_reg[60]_0 ;
  output \ip_exec_count_reg[29]_0 ;
  output \ip_exec_count_reg[61]_0 ;
  output \ip_exec_count_reg[30]_0 ;
  output \ip_exec_count_reg[62]_0 ;
  output \ip_exec_count_reg[31]_0 ;
  output \ip_exec_count_reg[63]_0 ;
  output [31:0]\sample_data_reg[31]_0 ;
  input RST_ACTIVE;
  input mon_clk;
  input rd_en;
  input [7:0]O;
  input [7:0]\ip_cur_tranx_reg[15]_0 ;
  input [7:0]\ip_cur_tranx_reg[23]_0 ;
  input [7:0]\ip_cur_tranx_reg[31]_0 ;
  input [7:0]\ip_cur_tranx_reg[39]_0 ;
  input [7:0]\ip_cur_tranx_reg[47]_0 ;
  input [7:0]\ip_cur_tranx_reg[55]_0 ;
  input [7:0]\ip_cur_tranx_reg[63]_0 ;
  input ip_exec_count0;
  input ip_start_count0;
  input cnt_enabled_reg;
  input start_pulse;
  input Metrics_Cnt_En;
  input ap_done_reg;
  input dataflow_en;
  input ap_continue_reg;
  input [2:0]slv_reg_addr;
  input \ip_max_parallel_tranx_reg[0]_0 ;
  input [0:0]E;
  input [31:0]D;

  wire [0:0]CO;
  wire [31:0]D;
  wire [0:0]E;
  wire Metrics_Cnt_En;
  wire [7:0]O;
  wire [63:0]Q;
  wire RST_ACTIVE;
  wire ap_continue_reg;
  wire ap_done_reg;
  wire cnt_enabled_reg;
  wire [31:0]data13;
  wire [31:0]data5;
  wire [31:0]data6;
  wire [31:0]data9;
  wire dataflow_en;
  wire empty;
  wire \ip_cur_tranx[0]_i_1_n_0 ;
  wire \ip_cur_tranx[0]_i_22_n_0 ;
  wire \ip_cur_tranx[0]_i_23_n_0 ;
  wire \ip_cur_tranx[0]_i_24_n_0 ;
  wire \ip_cur_tranx[0]_i_25_n_0 ;
  wire \ip_cur_tranx[0]_i_26_n_0 ;
  wire \ip_cur_tranx[0]_i_27_n_0 ;
  wire \ip_cur_tranx[0]_i_28_n_0 ;
  wire \ip_cur_tranx[0]_i_29_n_0 ;
  wire \ip_cur_tranx[0]_i_30_n_0 ;
  wire \ip_cur_tranx[0]_i_31_n_0 ;
  wire \ip_cur_tranx[0]_i_32_n_0 ;
  wire \ip_cur_tranx[0]_i_33_n_0 ;
  wire \ip_cur_tranx[0]_i_34_n_0 ;
  wire \ip_cur_tranx[0]_i_35_n_0 ;
  wire \ip_cur_tranx[0]_i_3_n_0 ;
  wire \ip_cur_tranx[0]_i_4_n_0 ;
  wire \ip_cur_tranx[0]_i_5_n_0 ;
  wire [63:0]ip_cur_tranx_reg;
  wire [7:0]\ip_cur_tranx_reg[15]_0 ;
  wire [7:0]\ip_cur_tranx_reg[23]_0 ;
  wire [7:0]\ip_cur_tranx_reg[31]_0 ;
  wire [7:0]\ip_cur_tranx_reg[39]_0 ;
  wire [7:0]\ip_cur_tranx_reg[47]_0 ;
  wire [7:0]\ip_cur_tranx_reg[55]_0 ;
  wire [7:0]\ip_cur_tranx_reg[63]_0 ;
  wire \ip_cycles_avg[0]_i_2_n_0 ;
  wire \ip_cycles_avg[0]_i_3_n_0 ;
  wire \ip_cycles_avg[0]_i_4_n_0 ;
  wire \ip_cycles_avg[0]_i_5_n_0 ;
  wire \ip_cycles_avg[0]_i_6_n_0 ;
  wire \ip_cycles_avg[0]_i_7_n_0 ;
  wire \ip_cycles_avg[0]_i_8_n_0 ;
  wire \ip_cycles_avg[0]_i_9_n_0 ;
  wire \ip_cycles_avg[16]_i_2_n_0 ;
  wire \ip_cycles_avg[16]_i_3_n_0 ;
  wire \ip_cycles_avg[16]_i_4_n_0 ;
  wire \ip_cycles_avg[16]_i_5_n_0 ;
  wire \ip_cycles_avg[16]_i_6_n_0 ;
  wire \ip_cycles_avg[16]_i_7_n_0 ;
  wire \ip_cycles_avg[16]_i_8_n_0 ;
  wire \ip_cycles_avg[16]_i_9_n_0 ;
  wire \ip_cycles_avg[24]_i_2_n_0 ;
  wire \ip_cycles_avg[24]_i_3_n_0 ;
  wire \ip_cycles_avg[24]_i_4_n_0 ;
  wire \ip_cycles_avg[24]_i_5_n_0 ;
  wire \ip_cycles_avg[24]_i_6_n_0 ;
  wire \ip_cycles_avg[24]_i_7_n_0 ;
  wire \ip_cycles_avg[24]_i_8_n_0 ;
  wire \ip_cycles_avg[24]_i_9_n_0 ;
  wire \ip_cycles_avg[32]_i_2_n_0 ;
  wire \ip_cycles_avg[32]_i_3_n_0 ;
  wire \ip_cycles_avg[32]_i_4_n_0 ;
  wire \ip_cycles_avg[32]_i_5_n_0 ;
  wire \ip_cycles_avg[32]_i_6_n_0 ;
  wire \ip_cycles_avg[32]_i_7_n_0 ;
  wire \ip_cycles_avg[32]_i_8_n_0 ;
  wire \ip_cycles_avg[32]_i_9_n_0 ;
  wire \ip_cycles_avg[40]_i_2_n_0 ;
  wire \ip_cycles_avg[40]_i_3_n_0 ;
  wire \ip_cycles_avg[40]_i_4_n_0 ;
  wire \ip_cycles_avg[40]_i_5_n_0 ;
  wire \ip_cycles_avg[40]_i_6_n_0 ;
  wire \ip_cycles_avg[40]_i_7_n_0 ;
  wire \ip_cycles_avg[40]_i_8_n_0 ;
  wire \ip_cycles_avg[40]_i_9_n_0 ;
  wire \ip_cycles_avg[48]_i_2_n_0 ;
  wire \ip_cycles_avg[48]_i_3_n_0 ;
  wire \ip_cycles_avg[48]_i_4_n_0 ;
  wire \ip_cycles_avg[48]_i_5_n_0 ;
  wire \ip_cycles_avg[48]_i_6_n_0 ;
  wire \ip_cycles_avg[48]_i_7_n_0 ;
  wire \ip_cycles_avg[48]_i_8_n_0 ;
  wire \ip_cycles_avg[48]_i_9_n_0 ;
  wire \ip_cycles_avg[56]_i_2_n_0 ;
  wire \ip_cycles_avg[56]_i_3_n_0 ;
  wire \ip_cycles_avg[56]_i_4_n_0 ;
  wire \ip_cycles_avg[56]_i_5_n_0 ;
  wire \ip_cycles_avg[56]_i_6_n_0 ;
  wire \ip_cycles_avg[56]_i_7_n_0 ;
  wire \ip_cycles_avg[56]_i_8_n_0 ;
  wire \ip_cycles_avg[56]_i_9_n_0 ;
  wire \ip_cycles_avg[8]_i_2_n_0 ;
  wire \ip_cycles_avg[8]_i_3_n_0 ;
  wire \ip_cycles_avg[8]_i_4_n_0 ;
  wire \ip_cycles_avg[8]_i_5_n_0 ;
  wire \ip_cycles_avg[8]_i_6_n_0 ;
  wire \ip_cycles_avg[8]_i_7_n_0 ;
  wire \ip_cycles_avg[8]_i_8_n_0 ;
  wire \ip_cycles_avg[8]_i_9_n_0 ;
  wire \ip_cycles_avg_reg[0]_i_1_n_0 ;
  wire \ip_cycles_avg_reg[0]_i_1_n_1 ;
  wire \ip_cycles_avg_reg[0]_i_1_n_10 ;
  wire \ip_cycles_avg_reg[0]_i_1_n_11 ;
  wire \ip_cycles_avg_reg[0]_i_1_n_12 ;
  wire \ip_cycles_avg_reg[0]_i_1_n_13 ;
  wire \ip_cycles_avg_reg[0]_i_1_n_14 ;
  wire \ip_cycles_avg_reg[0]_i_1_n_15 ;
  wire \ip_cycles_avg_reg[0]_i_1_n_2 ;
  wire \ip_cycles_avg_reg[0]_i_1_n_3 ;
  wire \ip_cycles_avg_reg[0]_i_1_n_4 ;
  wire \ip_cycles_avg_reg[0]_i_1_n_5 ;
  wire \ip_cycles_avg_reg[0]_i_1_n_6 ;
  wire \ip_cycles_avg_reg[0]_i_1_n_7 ;
  wire \ip_cycles_avg_reg[0]_i_1_n_8 ;
  wire \ip_cycles_avg_reg[0]_i_1_n_9 ;
  wire \ip_cycles_avg_reg[16]_i_1_n_0 ;
  wire \ip_cycles_avg_reg[16]_i_1_n_1 ;
  wire \ip_cycles_avg_reg[16]_i_1_n_10 ;
  wire \ip_cycles_avg_reg[16]_i_1_n_11 ;
  wire \ip_cycles_avg_reg[16]_i_1_n_12 ;
  wire \ip_cycles_avg_reg[16]_i_1_n_13 ;
  wire \ip_cycles_avg_reg[16]_i_1_n_14 ;
  wire \ip_cycles_avg_reg[16]_i_1_n_15 ;
  wire \ip_cycles_avg_reg[16]_i_1_n_2 ;
  wire \ip_cycles_avg_reg[16]_i_1_n_3 ;
  wire \ip_cycles_avg_reg[16]_i_1_n_4 ;
  wire \ip_cycles_avg_reg[16]_i_1_n_5 ;
  wire \ip_cycles_avg_reg[16]_i_1_n_6 ;
  wire \ip_cycles_avg_reg[16]_i_1_n_7 ;
  wire \ip_cycles_avg_reg[16]_i_1_n_8 ;
  wire \ip_cycles_avg_reg[16]_i_1_n_9 ;
  wire \ip_cycles_avg_reg[24]_i_1_n_0 ;
  wire \ip_cycles_avg_reg[24]_i_1_n_1 ;
  wire \ip_cycles_avg_reg[24]_i_1_n_10 ;
  wire \ip_cycles_avg_reg[24]_i_1_n_11 ;
  wire \ip_cycles_avg_reg[24]_i_1_n_12 ;
  wire \ip_cycles_avg_reg[24]_i_1_n_13 ;
  wire \ip_cycles_avg_reg[24]_i_1_n_14 ;
  wire \ip_cycles_avg_reg[24]_i_1_n_15 ;
  wire \ip_cycles_avg_reg[24]_i_1_n_2 ;
  wire \ip_cycles_avg_reg[24]_i_1_n_3 ;
  wire \ip_cycles_avg_reg[24]_i_1_n_4 ;
  wire \ip_cycles_avg_reg[24]_i_1_n_5 ;
  wire \ip_cycles_avg_reg[24]_i_1_n_6 ;
  wire \ip_cycles_avg_reg[24]_i_1_n_7 ;
  wire \ip_cycles_avg_reg[24]_i_1_n_8 ;
  wire \ip_cycles_avg_reg[24]_i_1_n_9 ;
  wire \ip_cycles_avg_reg[32]_i_1_n_0 ;
  wire \ip_cycles_avg_reg[32]_i_1_n_1 ;
  wire \ip_cycles_avg_reg[32]_i_1_n_10 ;
  wire \ip_cycles_avg_reg[32]_i_1_n_11 ;
  wire \ip_cycles_avg_reg[32]_i_1_n_12 ;
  wire \ip_cycles_avg_reg[32]_i_1_n_13 ;
  wire \ip_cycles_avg_reg[32]_i_1_n_14 ;
  wire \ip_cycles_avg_reg[32]_i_1_n_15 ;
  wire \ip_cycles_avg_reg[32]_i_1_n_2 ;
  wire \ip_cycles_avg_reg[32]_i_1_n_3 ;
  wire \ip_cycles_avg_reg[32]_i_1_n_4 ;
  wire \ip_cycles_avg_reg[32]_i_1_n_5 ;
  wire \ip_cycles_avg_reg[32]_i_1_n_6 ;
  wire \ip_cycles_avg_reg[32]_i_1_n_7 ;
  wire \ip_cycles_avg_reg[32]_i_1_n_8 ;
  wire \ip_cycles_avg_reg[32]_i_1_n_9 ;
  wire \ip_cycles_avg_reg[40]_i_1_n_0 ;
  wire \ip_cycles_avg_reg[40]_i_1_n_1 ;
  wire \ip_cycles_avg_reg[40]_i_1_n_10 ;
  wire \ip_cycles_avg_reg[40]_i_1_n_11 ;
  wire \ip_cycles_avg_reg[40]_i_1_n_12 ;
  wire \ip_cycles_avg_reg[40]_i_1_n_13 ;
  wire \ip_cycles_avg_reg[40]_i_1_n_14 ;
  wire \ip_cycles_avg_reg[40]_i_1_n_15 ;
  wire \ip_cycles_avg_reg[40]_i_1_n_2 ;
  wire \ip_cycles_avg_reg[40]_i_1_n_3 ;
  wire \ip_cycles_avg_reg[40]_i_1_n_4 ;
  wire \ip_cycles_avg_reg[40]_i_1_n_5 ;
  wire \ip_cycles_avg_reg[40]_i_1_n_6 ;
  wire \ip_cycles_avg_reg[40]_i_1_n_7 ;
  wire \ip_cycles_avg_reg[40]_i_1_n_8 ;
  wire \ip_cycles_avg_reg[40]_i_1_n_9 ;
  wire \ip_cycles_avg_reg[48]_i_1_n_0 ;
  wire \ip_cycles_avg_reg[48]_i_1_n_1 ;
  wire \ip_cycles_avg_reg[48]_i_1_n_10 ;
  wire \ip_cycles_avg_reg[48]_i_1_n_11 ;
  wire \ip_cycles_avg_reg[48]_i_1_n_12 ;
  wire \ip_cycles_avg_reg[48]_i_1_n_13 ;
  wire \ip_cycles_avg_reg[48]_i_1_n_14 ;
  wire \ip_cycles_avg_reg[48]_i_1_n_15 ;
  wire \ip_cycles_avg_reg[48]_i_1_n_2 ;
  wire \ip_cycles_avg_reg[48]_i_1_n_3 ;
  wire \ip_cycles_avg_reg[48]_i_1_n_4 ;
  wire \ip_cycles_avg_reg[48]_i_1_n_5 ;
  wire \ip_cycles_avg_reg[48]_i_1_n_6 ;
  wire \ip_cycles_avg_reg[48]_i_1_n_7 ;
  wire \ip_cycles_avg_reg[48]_i_1_n_8 ;
  wire \ip_cycles_avg_reg[48]_i_1_n_9 ;
  wire \ip_cycles_avg_reg[56]_i_1_n_1 ;
  wire \ip_cycles_avg_reg[56]_i_1_n_10 ;
  wire \ip_cycles_avg_reg[56]_i_1_n_11 ;
  wire \ip_cycles_avg_reg[56]_i_1_n_12 ;
  wire \ip_cycles_avg_reg[56]_i_1_n_13 ;
  wire \ip_cycles_avg_reg[56]_i_1_n_14 ;
  wire \ip_cycles_avg_reg[56]_i_1_n_15 ;
  wire \ip_cycles_avg_reg[56]_i_1_n_2 ;
  wire \ip_cycles_avg_reg[56]_i_1_n_3 ;
  wire \ip_cycles_avg_reg[56]_i_1_n_4 ;
  wire \ip_cycles_avg_reg[56]_i_1_n_5 ;
  wire \ip_cycles_avg_reg[56]_i_1_n_6 ;
  wire \ip_cycles_avg_reg[56]_i_1_n_7 ;
  wire \ip_cycles_avg_reg[56]_i_1_n_8 ;
  wire \ip_cycles_avg_reg[56]_i_1_n_9 ;
  wire \ip_cycles_avg_reg[8]_i_1_n_0 ;
  wire \ip_cycles_avg_reg[8]_i_1_n_1 ;
  wire \ip_cycles_avg_reg[8]_i_1_n_10 ;
  wire \ip_cycles_avg_reg[8]_i_1_n_11 ;
  wire \ip_cycles_avg_reg[8]_i_1_n_12 ;
  wire \ip_cycles_avg_reg[8]_i_1_n_13 ;
  wire \ip_cycles_avg_reg[8]_i_1_n_14 ;
  wire \ip_cycles_avg_reg[8]_i_1_n_15 ;
  wire \ip_cycles_avg_reg[8]_i_1_n_2 ;
  wire \ip_cycles_avg_reg[8]_i_1_n_3 ;
  wire \ip_cycles_avg_reg[8]_i_1_n_4 ;
  wire \ip_cycles_avg_reg[8]_i_1_n_5 ;
  wire \ip_cycles_avg_reg[8]_i_1_n_6 ;
  wire \ip_cycles_avg_reg[8]_i_1_n_7 ;
  wire \ip_cycles_avg_reg[8]_i_1_n_8 ;
  wire \ip_cycles_avg_reg[8]_i_1_n_9 ;
  wire \ip_cycles_avg_reg_n_0_[0] ;
  wire \ip_cycles_avg_reg_n_0_[10] ;
  wire \ip_cycles_avg_reg_n_0_[11] ;
  wire \ip_cycles_avg_reg_n_0_[12] ;
  wire \ip_cycles_avg_reg_n_0_[13] ;
  wire \ip_cycles_avg_reg_n_0_[14] ;
  wire \ip_cycles_avg_reg_n_0_[15] ;
  wire \ip_cycles_avg_reg_n_0_[16] ;
  wire \ip_cycles_avg_reg_n_0_[17] ;
  wire \ip_cycles_avg_reg_n_0_[18] ;
  wire \ip_cycles_avg_reg_n_0_[19] ;
  wire \ip_cycles_avg_reg_n_0_[1] ;
  wire \ip_cycles_avg_reg_n_0_[20] ;
  wire \ip_cycles_avg_reg_n_0_[21] ;
  wire \ip_cycles_avg_reg_n_0_[22] ;
  wire \ip_cycles_avg_reg_n_0_[23] ;
  wire \ip_cycles_avg_reg_n_0_[24] ;
  wire \ip_cycles_avg_reg_n_0_[25] ;
  wire \ip_cycles_avg_reg_n_0_[26] ;
  wire \ip_cycles_avg_reg_n_0_[27] ;
  wire \ip_cycles_avg_reg_n_0_[28] ;
  wire \ip_cycles_avg_reg_n_0_[29] ;
  wire \ip_cycles_avg_reg_n_0_[2] ;
  wire \ip_cycles_avg_reg_n_0_[30] ;
  wire \ip_cycles_avg_reg_n_0_[31] ;
  wire \ip_cycles_avg_reg_n_0_[3] ;
  wire \ip_cycles_avg_reg_n_0_[4] ;
  wire \ip_cycles_avg_reg_n_0_[5] ;
  wire \ip_cycles_avg_reg_n_0_[6] ;
  wire \ip_cycles_avg_reg_n_0_[7] ;
  wire \ip_cycles_avg_reg_n_0_[8] ;
  wire \ip_cycles_avg_reg_n_0_[9] ;
  wire ip_exec_count0;
  wire \ip_exec_count[0]_i_3_n_0 ;
  wire \ip_exec_count_reg[0]_0 ;
  wire \ip_exec_count_reg[0]_i_2_n_0 ;
  wire \ip_exec_count_reg[0]_i_2_n_1 ;
  wire \ip_exec_count_reg[0]_i_2_n_10 ;
  wire \ip_exec_count_reg[0]_i_2_n_11 ;
  wire \ip_exec_count_reg[0]_i_2_n_12 ;
  wire \ip_exec_count_reg[0]_i_2_n_13 ;
  wire \ip_exec_count_reg[0]_i_2_n_14 ;
  wire \ip_exec_count_reg[0]_i_2_n_15 ;
  wire \ip_exec_count_reg[0]_i_2_n_2 ;
  wire \ip_exec_count_reg[0]_i_2_n_3 ;
  wire \ip_exec_count_reg[0]_i_2_n_4 ;
  wire \ip_exec_count_reg[0]_i_2_n_5 ;
  wire \ip_exec_count_reg[0]_i_2_n_6 ;
  wire \ip_exec_count_reg[0]_i_2_n_7 ;
  wire \ip_exec_count_reg[0]_i_2_n_8 ;
  wire \ip_exec_count_reg[0]_i_2_n_9 ;
  wire \ip_exec_count_reg[10]_0 ;
  wire \ip_exec_count_reg[11]_0 ;
  wire \ip_exec_count_reg[12]_0 ;
  wire \ip_exec_count_reg[13]_0 ;
  wire \ip_exec_count_reg[14]_0 ;
  wire \ip_exec_count_reg[15]_0 ;
  wire \ip_exec_count_reg[16]_0 ;
  wire \ip_exec_count_reg[16]_i_1_n_0 ;
  wire \ip_exec_count_reg[16]_i_1_n_1 ;
  wire \ip_exec_count_reg[16]_i_1_n_10 ;
  wire \ip_exec_count_reg[16]_i_1_n_11 ;
  wire \ip_exec_count_reg[16]_i_1_n_12 ;
  wire \ip_exec_count_reg[16]_i_1_n_13 ;
  wire \ip_exec_count_reg[16]_i_1_n_14 ;
  wire \ip_exec_count_reg[16]_i_1_n_15 ;
  wire \ip_exec_count_reg[16]_i_1_n_2 ;
  wire \ip_exec_count_reg[16]_i_1_n_3 ;
  wire \ip_exec_count_reg[16]_i_1_n_4 ;
  wire \ip_exec_count_reg[16]_i_1_n_5 ;
  wire \ip_exec_count_reg[16]_i_1_n_6 ;
  wire \ip_exec_count_reg[16]_i_1_n_7 ;
  wire \ip_exec_count_reg[16]_i_1_n_8 ;
  wire \ip_exec_count_reg[16]_i_1_n_9 ;
  wire \ip_exec_count_reg[17]_0 ;
  wire \ip_exec_count_reg[18]_0 ;
  wire \ip_exec_count_reg[19]_0 ;
  wire \ip_exec_count_reg[1]_0 ;
  wire \ip_exec_count_reg[20]_0 ;
  wire \ip_exec_count_reg[21]_0 ;
  wire \ip_exec_count_reg[22]_0 ;
  wire \ip_exec_count_reg[23]_0 ;
  wire \ip_exec_count_reg[24]_0 ;
  wire \ip_exec_count_reg[24]_i_1_n_0 ;
  wire \ip_exec_count_reg[24]_i_1_n_1 ;
  wire \ip_exec_count_reg[24]_i_1_n_10 ;
  wire \ip_exec_count_reg[24]_i_1_n_11 ;
  wire \ip_exec_count_reg[24]_i_1_n_12 ;
  wire \ip_exec_count_reg[24]_i_1_n_13 ;
  wire \ip_exec_count_reg[24]_i_1_n_14 ;
  wire \ip_exec_count_reg[24]_i_1_n_15 ;
  wire \ip_exec_count_reg[24]_i_1_n_2 ;
  wire \ip_exec_count_reg[24]_i_1_n_3 ;
  wire \ip_exec_count_reg[24]_i_1_n_4 ;
  wire \ip_exec_count_reg[24]_i_1_n_5 ;
  wire \ip_exec_count_reg[24]_i_1_n_6 ;
  wire \ip_exec_count_reg[24]_i_1_n_7 ;
  wire \ip_exec_count_reg[24]_i_1_n_8 ;
  wire \ip_exec_count_reg[24]_i_1_n_9 ;
  wire \ip_exec_count_reg[25]_0 ;
  wire \ip_exec_count_reg[26]_0 ;
  wire \ip_exec_count_reg[27]_0 ;
  wire \ip_exec_count_reg[28]_0 ;
  wire \ip_exec_count_reg[29]_0 ;
  wire \ip_exec_count_reg[2]_0 ;
  wire \ip_exec_count_reg[30]_0 ;
  wire \ip_exec_count_reg[31]_0 ;
  wire \ip_exec_count_reg[32]_0 ;
  wire \ip_exec_count_reg[32]_i_1_n_0 ;
  wire \ip_exec_count_reg[32]_i_1_n_1 ;
  wire \ip_exec_count_reg[32]_i_1_n_10 ;
  wire \ip_exec_count_reg[32]_i_1_n_11 ;
  wire \ip_exec_count_reg[32]_i_1_n_12 ;
  wire \ip_exec_count_reg[32]_i_1_n_13 ;
  wire \ip_exec_count_reg[32]_i_1_n_14 ;
  wire \ip_exec_count_reg[32]_i_1_n_15 ;
  wire \ip_exec_count_reg[32]_i_1_n_2 ;
  wire \ip_exec_count_reg[32]_i_1_n_3 ;
  wire \ip_exec_count_reg[32]_i_1_n_4 ;
  wire \ip_exec_count_reg[32]_i_1_n_5 ;
  wire \ip_exec_count_reg[32]_i_1_n_6 ;
  wire \ip_exec_count_reg[32]_i_1_n_7 ;
  wire \ip_exec_count_reg[32]_i_1_n_8 ;
  wire \ip_exec_count_reg[32]_i_1_n_9 ;
  wire \ip_exec_count_reg[33]_0 ;
  wire \ip_exec_count_reg[34]_0 ;
  wire \ip_exec_count_reg[35]_0 ;
  wire \ip_exec_count_reg[36]_0 ;
  wire \ip_exec_count_reg[37]_0 ;
  wire \ip_exec_count_reg[38]_0 ;
  wire \ip_exec_count_reg[39]_0 ;
  wire \ip_exec_count_reg[3]_0 ;
  wire \ip_exec_count_reg[40]_0 ;
  wire \ip_exec_count_reg[40]_i_1_n_0 ;
  wire \ip_exec_count_reg[40]_i_1_n_1 ;
  wire \ip_exec_count_reg[40]_i_1_n_10 ;
  wire \ip_exec_count_reg[40]_i_1_n_11 ;
  wire \ip_exec_count_reg[40]_i_1_n_12 ;
  wire \ip_exec_count_reg[40]_i_1_n_13 ;
  wire \ip_exec_count_reg[40]_i_1_n_14 ;
  wire \ip_exec_count_reg[40]_i_1_n_15 ;
  wire \ip_exec_count_reg[40]_i_1_n_2 ;
  wire \ip_exec_count_reg[40]_i_1_n_3 ;
  wire \ip_exec_count_reg[40]_i_1_n_4 ;
  wire \ip_exec_count_reg[40]_i_1_n_5 ;
  wire \ip_exec_count_reg[40]_i_1_n_6 ;
  wire \ip_exec_count_reg[40]_i_1_n_7 ;
  wire \ip_exec_count_reg[40]_i_1_n_8 ;
  wire \ip_exec_count_reg[40]_i_1_n_9 ;
  wire \ip_exec_count_reg[41]_0 ;
  wire \ip_exec_count_reg[42]_0 ;
  wire \ip_exec_count_reg[43]_0 ;
  wire \ip_exec_count_reg[44]_0 ;
  wire \ip_exec_count_reg[45]_0 ;
  wire \ip_exec_count_reg[46]_0 ;
  wire \ip_exec_count_reg[47]_0 ;
  wire \ip_exec_count_reg[48]_0 ;
  wire \ip_exec_count_reg[48]_i_1_n_0 ;
  wire \ip_exec_count_reg[48]_i_1_n_1 ;
  wire \ip_exec_count_reg[48]_i_1_n_10 ;
  wire \ip_exec_count_reg[48]_i_1_n_11 ;
  wire \ip_exec_count_reg[48]_i_1_n_12 ;
  wire \ip_exec_count_reg[48]_i_1_n_13 ;
  wire \ip_exec_count_reg[48]_i_1_n_14 ;
  wire \ip_exec_count_reg[48]_i_1_n_15 ;
  wire \ip_exec_count_reg[48]_i_1_n_2 ;
  wire \ip_exec_count_reg[48]_i_1_n_3 ;
  wire \ip_exec_count_reg[48]_i_1_n_4 ;
  wire \ip_exec_count_reg[48]_i_1_n_5 ;
  wire \ip_exec_count_reg[48]_i_1_n_6 ;
  wire \ip_exec_count_reg[48]_i_1_n_7 ;
  wire \ip_exec_count_reg[48]_i_1_n_8 ;
  wire \ip_exec_count_reg[48]_i_1_n_9 ;
  wire \ip_exec_count_reg[49]_0 ;
  wire \ip_exec_count_reg[4]_0 ;
  wire \ip_exec_count_reg[50]_0 ;
  wire \ip_exec_count_reg[51]_0 ;
  wire \ip_exec_count_reg[52]_0 ;
  wire \ip_exec_count_reg[53]_0 ;
  wire \ip_exec_count_reg[54]_0 ;
  wire \ip_exec_count_reg[55]_0 ;
  wire \ip_exec_count_reg[56]_0 ;
  wire \ip_exec_count_reg[56]_i_1_n_1 ;
  wire \ip_exec_count_reg[56]_i_1_n_10 ;
  wire \ip_exec_count_reg[56]_i_1_n_11 ;
  wire \ip_exec_count_reg[56]_i_1_n_12 ;
  wire \ip_exec_count_reg[56]_i_1_n_13 ;
  wire \ip_exec_count_reg[56]_i_1_n_14 ;
  wire \ip_exec_count_reg[56]_i_1_n_15 ;
  wire \ip_exec_count_reg[56]_i_1_n_2 ;
  wire \ip_exec_count_reg[56]_i_1_n_3 ;
  wire \ip_exec_count_reg[56]_i_1_n_4 ;
  wire \ip_exec_count_reg[56]_i_1_n_5 ;
  wire \ip_exec_count_reg[56]_i_1_n_6 ;
  wire \ip_exec_count_reg[56]_i_1_n_7 ;
  wire \ip_exec_count_reg[56]_i_1_n_8 ;
  wire \ip_exec_count_reg[56]_i_1_n_9 ;
  wire \ip_exec_count_reg[57]_0 ;
  wire \ip_exec_count_reg[58]_0 ;
  wire \ip_exec_count_reg[59]_0 ;
  wire \ip_exec_count_reg[5]_0 ;
  wire \ip_exec_count_reg[60]_0 ;
  wire \ip_exec_count_reg[61]_0 ;
  wire \ip_exec_count_reg[62]_0 ;
  wire \ip_exec_count_reg[63]_0 ;
  wire \ip_exec_count_reg[6]_0 ;
  wire \ip_exec_count_reg[7]_0 ;
  wire \ip_exec_count_reg[8]_0 ;
  wire \ip_exec_count_reg[8]_i_1_n_0 ;
  wire \ip_exec_count_reg[8]_i_1_n_1 ;
  wire \ip_exec_count_reg[8]_i_1_n_10 ;
  wire \ip_exec_count_reg[8]_i_1_n_11 ;
  wire \ip_exec_count_reg[8]_i_1_n_12 ;
  wire \ip_exec_count_reg[8]_i_1_n_13 ;
  wire \ip_exec_count_reg[8]_i_1_n_14 ;
  wire \ip_exec_count_reg[8]_i_1_n_15 ;
  wire \ip_exec_count_reg[8]_i_1_n_2 ;
  wire \ip_exec_count_reg[8]_i_1_n_3 ;
  wire \ip_exec_count_reg[8]_i_1_n_4 ;
  wire \ip_exec_count_reg[8]_i_1_n_5 ;
  wire \ip_exec_count_reg[8]_i_1_n_6 ;
  wire \ip_exec_count_reg[8]_i_1_n_7 ;
  wire \ip_exec_count_reg[8]_i_1_n_8 ;
  wire \ip_exec_count_reg[8]_i_1_n_9 ;
  wire \ip_exec_count_reg[9]_0 ;
  wire \ip_exec_count_reg_n_0_[0] ;
  wire \ip_exec_count_reg_n_0_[10] ;
  wire \ip_exec_count_reg_n_0_[11] ;
  wire \ip_exec_count_reg_n_0_[12] ;
  wire \ip_exec_count_reg_n_0_[13] ;
  wire \ip_exec_count_reg_n_0_[14] ;
  wire \ip_exec_count_reg_n_0_[15] ;
  wire \ip_exec_count_reg_n_0_[16] ;
  wire \ip_exec_count_reg_n_0_[17] ;
  wire \ip_exec_count_reg_n_0_[18] ;
  wire \ip_exec_count_reg_n_0_[19] ;
  wire \ip_exec_count_reg_n_0_[1] ;
  wire \ip_exec_count_reg_n_0_[20] ;
  wire \ip_exec_count_reg_n_0_[21] ;
  wire \ip_exec_count_reg_n_0_[22] ;
  wire \ip_exec_count_reg_n_0_[23] ;
  wire \ip_exec_count_reg_n_0_[24] ;
  wire \ip_exec_count_reg_n_0_[25] ;
  wire \ip_exec_count_reg_n_0_[26] ;
  wire \ip_exec_count_reg_n_0_[27] ;
  wire \ip_exec_count_reg_n_0_[28] ;
  wire \ip_exec_count_reg_n_0_[29] ;
  wire \ip_exec_count_reg_n_0_[2] ;
  wire \ip_exec_count_reg_n_0_[30] ;
  wire \ip_exec_count_reg_n_0_[31] ;
  wire \ip_exec_count_reg_n_0_[3] ;
  wire \ip_exec_count_reg_n_0_[4] ;
  wire \ip_exec_count_reg_n_0_[5] ;
  wire \ip_exec_count_reg_n_0_[6] ;
  wire \ip_exec_count_reg_n_0_[7] ;
  wire \ip_exec_count_reg_n_0_[8] ;
  wire \ip_exec_count_reg_n_0_[9] ;
  wire ip_idle_carry__0_i_1_n_0;
  wire ip_idle_carry__0_i_2_n_0;
  wire ip_idle_carry__0_i_3_n_0;
  wire ip_idle_carry__0_i_4_n_0;
  wire ip_idle_carry__0_i_5_n_0;
  wire ip_idle_carry__0_i_6_n_0;
  wire ip_idle_carry__0_i_7_n_0;
  wire ip_idle_carry__0_i_8_n_0;
  wire ip_idle_carry__0_n_0;
  wire ip_idle_carry__0_n_1;
  wire ip_idle_carry__0_n_2;
  wire ip_idle_carry__0_n_3;
  wire ip_idle_carry__0_n_4;
  wire ip_idle_carry__0_n_5;
  wire ip_idle_carry__0_n_6;
  wire ip_idle_carry__0_n_7;
  wire ip_idle_carry__1_i_1_n_0;
  wire ip_idle_carry__1_i_2_n_0;
  wire ip_idle_carry__1_i_3_n_0;
  wire ip_idle_carry__1_i_4_n_0;
  wire ip_idle_carry__1_i_5_n_0;
  wire ip_idle_carry__1_i_6_n_0;
  wire ip_idle_carry__1_n_3;
  wire ip_idle_carry__1_n_4;
  wire ip_idle_carry__1_n_5;
  wire ip_idle_carry__1_n_6;
  wire ip_idle_carry__1_n_7;
  wire ip_idle_carry_i_1_n_0;
  wire ip_idle_carry_i_2_n_0;
  wire ip_idle_carry_i_3_n_0;
  wire ip_idle_carry_i_4_n_0;
  wire ip_idle_carry_i_5_n_0;
  wire ip_idle_carry_i_6_n_0;
  wire ip_idle_carry_i_7_n_0;
  wire ip_idle_carry_i_8_n_0;
  wire ip_idle_carry_n_0;
  wire ip_idle_carry_n_1;
  wire ip_idle_carry_n_2;
  wire ip_idle_carry_n_3;
  wire ip_idle_carry_n_4;
  wire ip_idle_carry_n_5;
  wire ip_idle_carry_n_6;
  wire ip_idle_carry_n_7;
  wire ip_max_parallel_tranx1_carry__0_i_10_n_0;
  wire ip_max_parallel_tranx1_carry__0_i_11_n_0;
  wire ip_max_parallel_tranx1_carry__0_i_12_n_0;
  wire ip_max_parallel_tranx1_carry__0_i_13_n_0;
  wire ip_max_parallel_tranx1_carry__0_i_14_n_0;
  wire ip_max_parallel_tranx1_carry__0_i_15_n_0;
  wire ip_max_parallel_tranx1_carry__0_i_16_n_0;
  wire ip_max_parallel_tranx1_carry__0_i_1_n_0;
  wire ip_max_parallel_tranx1_carry__0_i_2_n_0;
  wire ip_max_parallel_tranx1_carry__0_i_3_n_0;
  wire ip_max_parallel_tranx1_carry__0_i_4_n_0;
  wire ip_max_parallel_tranx1_carry__0_i_5_n_0;
  wire ip_max_parallel_tranx1_carry__0_i_6_n_0;
  wire ip_max_parallel_tranx1_carry__0_i_7_n_0;
  wire ip_max_parallel_tranx1_carry__0_i_8_n_0;
  wire ip_max_parallel_tranx1_carry__0_i_9_n_0;
  wire ip_max_parallel_tranx1_carry__0_n_0;
  wire ip_max_parallel_tranx1_carry__0_n_1;
  wire ip_max_parallel_tranx1_carry__0_n_2;
  wire ip_max_parallel_tranx1_carry__0_n_3;
  wire ip_max_parallel_tranx1_carry__0_n_4;
  wire ip_max_parallel_tranx1_carry__0_n_5;
  wire ip_max_parallel_tranx1_carry__0_n_6;
  wire ip_max_parallel_tranx1_carry__0_n_7;
  wire ip_max_parallel_tranx1_carry__1_i_10_n_0;
  wire ip_max_parallel_tranx1_carry__1_i_11_n_0;
  wire ip_max_parallel_tranx1_carry__1_i_12_n_0;
  wire ip_max_parallel_tranx1_carry__1_i_13_n_0;
  wire ip_max_parallel_tranx1_carry__1_i_14_n_0;
  wire ip_max_parallel_tranx1_carry__1_i_15_n_0;
  wire ip_max_parallel_tranx1_carry__1_i_16_n_0;
  wire ip_max_parallel_tranx1_carry__1_i_1_n_0;
  wire ip_max_parallel_tranx1_carry__1_i_2_n_0;
  wire ip_max_parallel_tranx1_carry__1_i_3_n_0;
  wire ip_max_parallel_tranx1_carry__1_i_4_n_0;
  wire ip_max_parallel_tranx1_carry__1_i_5_n_0;
  wire ip_max_parallel_tranx1_carry__1_i_6_n_0;
  wire ip_max_parallel_tranx1_carry__1_i_7_n_0;
  wire ip_max_parallel_tranx1_carry__1_i_8_n_0;
  wire ip_max_parallel_tranx1_carry__1_i_9_n_0;
  wire ip_max_parallel_tranx1_carry__1_n_0;
  wire ip_max_parallel_tranx1_carry__1_n_1;
  wire ip_max_parallel_tranx1_carry__1_n_2;
  wire ip_max_parallel_tranx1_carry__1_n_3;
  wire ip_max_parallel_tranx1_carry__1_n_4;
  wire ip_max_parallel_tranx1_carry__1_n_5;
  wire ip_max_parallel_tranx1_carry__1_n_6;
  wire ip_max_parallel_tranx1_carry__1_n_7;
  wire ip_max_parallel_tranx1_carry__2_i_10_n_0;
  wire ip_max_parallel_tranx1_carry__2_i_11_n_0;
  wire ip_max_parallel_tranx1_carry__2_i_12_n_0;
  wire ip_max_parallel_tranx1_carry__2_i_13_n_0;
  wire ip_max_parallel_tranx1_carry__2_i_14_n_0;
  wire ip_max_parallel_tranx1_carry__2_i_15_n_0;
  wire ip_max_parallel_tranx1_carry__2_i_16_n_0;
  wire ip_max_parallel_tranx1_carry__2_i_1_n_0;
  wire ip_max_parallel_tranx1_carry__2_i_2_n_0;
  wire ip_max_parallel_tranx1_carry__2_i_3_n_0;
  wire ip_max_parallel_tranx1_carry__2_i_4_n_0;
  wire ip_max_parallel_tranx1_carry__2_i_5_n_0;
  wire ip_max_parallel_tranx1_carry__2_i_6_n_0;
  wire ip_max_parallel_tranx1_carry__2_i_7_n_0;
  wire ip_max_parallel_tranx1_carry__2_i_8_n_0;
  wire ip_max_parallel_tranx1_carry__2_i_9_n_0;
  wire ip_max_parallel_tranx1_carry__2_n_0;
  wire ip_max_parallel_tranx1_carry__2_n_1;
  wire ip_max_parallel_tranx1_carry__2_n_2;
  wire ip_max_parallel_tranx1_carry__2_n_3;
  wire ip_max_parallel_tranx1_carry__2_n_4;
  wire ip_max_parallel_tranx1_carry__2_n_5;
  wire ip_max_parallel_tranx1_carry__2_n_6;
  wire ip_max_parallel_tranx1_carry__2_n_7;
  wire ip_max_parallel_tranx1_carry_i_10_n_0;
  wire ip_max_parallel_tranx1_carry_i_11_n_0;
  wire ip_max_parallel_tranx1_carry_i_12_n_0;
  wire ip_max_parallel_tranx1_carry_i_13_n_0;
  wire ip_max_parallel_tranx1_carry_i_14_n_0;
  wire ip_max_parallel_tranx1_carry_i_15_n_0;
  wire ip_max_parallel_tranx1_carry_i_16_n_0;
  wire ip_max_parallel_tranx1_carry_i_1_n_0;
  wire ip_max_parallel_tranx1_carry_i_2_n_0;
  wire ip_max_parallel_tranx1_carry_i_3_n_0;
  wire ip_max_parallel_tranx1_carry_i_4_n_0;
  wire ip_max_parallel_tranx1_carry_i_5_n_0;
  wire ip_max_parallel_tranx1_carry_i_6_n_0;
  wire ip_max_parallel_tranx1_carry_i_7_n_0;
  wire ip_max_parallel_tranx1_carry_i_8_n_0;
  wire ip_max_parallel_tranx1_carry_i_9_n_0;
  wire ip_max_parallel_tranx1_carry_n_0;
  wire ip_max_parallel_tranx1_carry_n_1;
  wire ip_max_parallel_tranx1_carry_n_2;
  wire ip_max_parallel_tranx1_carry_n_3;
  wire ip_max_parallel_tranx1_carry_n_4;
  wire ip_max_parallel_tranx1_carry_n_5;
  wire ip_max_parallel_tranx1_carry_n_6;
  wire ip_max_parallel_tranx1_carry_n_7;
  wire \ip_max_parallel_tranx_reg[0]_0 ;
  wire \ip_max_parallel_tranx_reg_n_0_[0] ;
  wire \ip_max_parallel_tranx_reg_n_0_[10] ;
  wire \ip_max_parallel_tranx_reg_n_0_[11] ;
  wire \ip_max_parallel_tranx_reg_n_0_[12] ;
  wire \ip_max_parallel_tranx_reg_n_0_[13] ;
  wire \ip_max_parallel_tranx_reg_n_0_[14] ;
  wire \ip_max_parallel_tranx_reg_n_0_[15] ;
  wire \ip_max_parallel_tranx_reg_n_0_[16] ;
  wire \ip_max_parallel_tranx_reg_n_0_[17] ;
  wire \ip_max_parallel_tranx_reg_n_0_[18] ;
  wire \ip_max_parallel_tranx_reg_n_0_[19] ;
  wire \ip_max_parallel_tranx_reg_n_0_[1] ;
  wire \ip_max_parallel_tranx_reg_n_0_[20] ;
  wire \ip_max_parallel_tranx_reg_n_0_[21] ;
  wire \ip_max_parallel_tranx_reg_n_0_[22] ;
  wire \ip_max_parallel_tranx_reg_n_0_[23] ;
  wire \ip_max_parallel_tranx_reg_n_0_[24] ;
  wire \ip_max_parallel_tranx_reg_n_0_[25] ;
  wire \ip_max_parallel_tranx_reg_n_0_[26] ;
  wire \ip_max_parallel_tranx_reg_n_0_[27] ;
  wire \ip_max_parallel_tranx_reg_n_0_[28] ;
  wire \ip_max_parallel_tranx_reg_n_0_[29] ;
  wire \ip_max_parallel_tranx_reg_n_0_[2] ;
  wire \ip_max_parallel_tranx_reg_n_0_[30] ;
  wire \ip_max_parallel_tranx_reg_n_0_[31] ;
  wire \ip_max_parallel_tranx_reg_n_0_[3] ;
  wire \ip_max_parallel_tranx_reg_n_0_[4] ;
  wire \ip_max_parallel_tranx_reg_n_0_[5] ;
  wire \ip_max_parallel_tranx_reg_n_0_[6] ;
  wire \ip_max_parallel_tranx_reg_n_0_[7] ;
  wire \ip_max_parallel_tranx_reg_n_0_[8] ;
  wire \ip_max_parallel_tranx_reg_n_0_[9] ;
  wire ip_start_count0;
  wire \ip_start_count[0]_i_3_n_0 ;
  wire \ip_start_count_reg[0]_i_2_n_0 ;
  wire \ip_start_count_reg[0]_i_2_n_1 ;
  wire \ip_start_count_reg[0]_i_2_n_10 ;
  wire \ip_start_count_reg[0]_i_2_n_11 ;
  wire \ip_start_count_reg[0]_i_2_n_12 ;
  wire \ip_start_count_reg[0]_i_2_n_13 ;
  wire \ip_start_count_reg[0]_i_2_n_14 ;
  wire \ip_start_count_reg[0]_i_2_n_15 ;
  wire \ip_start_count_reg[0]_i_2_n_2 ;
  wire \ip_start_count_reg[0]_i_2_n_3 ;
  wire \ip_start_count_reg[0]_i_2_n_4 ;
  wire \ip_start_count_reg[0]_i_2_n_5 ;
  wire \ip_start_count_reg[0]_i_2_n_6 ;
  wire \ip_start_count_reg[0]_i_2_n_7 ;
  wire \ip_start_count_reg[0]_i_2_n_8 ;
  wire \ip_start_count_reg[0]_i_2_n_9 ;
  wire \ip_start_count_reg[16]_i_1_n_0 ;
  wire \ip_start_count_reg[16]_i_1_n_1 ;
  wire \ip_start_count_reg[16]_i_1_n_10 ;
  wire \ip_start_count_reg[16]_i_1_n_11 ;
  wire \ip_start_count_reg[16]_i_1_n_12 ;
  wire \ip_start_count_reg[16]_i_1_n_13 ;
  wire \ip_start_count_reg[16]_i_1_n_14 ;
  wire \ip_start_count_reg[16]_i_1_n_15 ;
  wire \ip_start_count_reg[16]_i_1_n_2 ;
  wire \ip_start_count_reg[16]_i_1_n_3 ;
  wire \ip_start_count_reg[16]_i_1_n_4 ;
  wire \ip_start_count_reg[16]_i_1_n_5 ;
  wire \ip_start_count_reg[16]_i_1_n_6 ;
  wire \ip_start_count_reg[16]_i_1_n_7 ;
  wire \ip_start_count_reg[16]_i_1_n_8 ;
  wire \ip_start_count_reg[16]_i_1_n_9 ;
  wire \ip_start_count_reg[24]_i_1_n_0 ;
  wire \ip_start_count_reg[24]_i_1_n_1 ;
  wire \ip_start_count_reg[24]_i_1_n_10 ;
  wire \ip_start_count_reg[24]_i_1_n_11 ;
  wire \ip_start_count_reg[24]_i_1_n_12 ;
  wire \ip_start_count_reg[24]_i_1_n_13 ;
  wire \ip_start_count_reg[24]_i_1_n_14 ;
  wire \ip_start_count_reg[24]_i_1_n_15 ;
  wire \ip_start_count_reg[24]_i_1_n_2 ;
  wire \ip_start_count_reg[24]_i_1_n_3 ;
  wire \ip_start_count_reg[24]_i_1_n_4 ;
  wire \ip_start_count_reg[24]_i_1_n_5 ;
  wire \ip_start_count_reg[24]_i_1_n_6 ;
  wire \ip_start_count_reg[24]_i_1_n_7 ;
  wire \ip_start_count_reg[24]_i_1_n_8 ;
  wire \ip_start_count_reg[24]_i_1_n_9 ;
  wire \ip_start_count_reg[32]_i_1_n_0 ;
  wire \ip_start_count_reg[32]_i_1_n_1 ;
  wire \ip_start_count_reg[32]_i_1_n_10 ;
  wire \ip_start_count_reg[32]_i_1_n_11 ;
  wire \ip_start_count_reg[32]_i_1_n_12 ;
  wire \ip_start_count_reg[32]_i_1_n_13 ;
  wire \ip_start_count_reg[32]_i_1_n_14 ;
  wire \ip_start_count_reg[32]_i_1_n_15 ;
  wire \ip_start_count_reg[32]_i_1_n_2 ;
  wire \ip_start_count_reg[32]_i_1_n_3 ;
  wire \ip_start_count_reg[32]_i_1_n_4 ;
  wire \ip_start_count_reg[32]_i_1_n_5 ;
  wire \ip_start_count_reg[32]_i_1_n_6 ;
  wire \ip_start_count_reg[32]_i_1_n_7 ;
  wire \ip_start_count_reg[32]_i_1_n_8 ;
  wire \ip_start_count_reg[32]_i_1_n_9 ;
  wire \ip_start_count_reg[40]_i_1_n_0 ;
  wire \ip_start_count_reg[40]_i_1_n_1 ;
  wire \ip_start_count_reg[40]_i_1_n_10 ;
  wire \ip_start_count_reg[40]_i_1_n_11 ;
  wire \ip_start_count_reg[40]_i_1_n_12 ;
  wire \ip_start_count_reg[40]_i_1_n_13 ;
  wire \ip_start_count_reg[40]_i_1_n_14 ;
  wire \ip_start_count_reg[40]_i_1_n_15 ;
  wire \ip_start_count_reg[40]_i_1_n_2 ;
  wire \ip_start_count_reg[40]_i_1_n_3 ;
  wire \ip_start_count_reg[40]_i_1_n_4 ;
  wire \ip_start_count_reg[40]_i_1_n_5 ;
  wire \ip_start_count_reg[40]_i_1_n_6 ;
  wire \ip_start_count_reg[40]_i_1_n_7 ;
  wire \ip_start_count_reg[40]_i_1_n_8 ;
  wire \ip_start_count_reg[40]_i_1_n_9 ;
  wire \ip_start_count_reg[48]_i_1_n_0 ;
  wire \ip_start_count_reg[48]_i_1_n_1 ;
  wire \ip_start_count_reg[48]_i_1_n_10 ;
  wire \ip_start_count_reg[48]_i_1_n_11 ;
  wire \ip_start_count_reg[48]_i_1_n_12 ;
  wire \ip_start_count_reg[48]_i_1_n_13 ;
  wire \ip_start_count_reg[48]_i_1_n_14 ;
  wire \ip_start_count_reg[48]_i_1_n_15 ;
  wire \ip_start_count_reg[48]_i_1_n_2 ;
  wire \ip_start_count_reg[48]_i_1_n_3 ;
  wire \ip_start_count_reg[48]_i_1_n_4 ;
  wire \ip_start_count_reg[48]_i_1_n_5 ;
  wire \ip_start_count_reg[48]_i_1_n_6 ;
  wire \ip_start_count_reg[48]_i_1_n_7 ;
  wire \ip_start_count_reg[48]_i_1_n_8 ;
  wire \ip_start_count_reg[48]_i_1_n_9 ;
  wire \ip_start_count_reg[56]_i_1_n_1 ;
  wire \ip_start_count_reg[56]_i_1_n_10 ;
  wire \ip_start_count_reg[56]_i_1_n_11 ;
  wire \ip_start_count_reg[56]_i_1_n_12 ;
  wire \ip_start_count_reg[56]_i_1_n_13 ;
  wire \ip_start_count_reg[56]_i_1_n_14 ;
  wire \ip_start_count_reg[56]_i_1_n_15 ;
  wire \ip_start_count_reg[56]_i_1_n_2 ;
  wire \ip_start_count_reg[56]_i_1_n_3 ;
  wire \ip_start_count_reg[56]_i_1_n_4 ;
  wire \ip_start_count_reg[56]_i_1_n_5 ;
  wire \ip_start_count_reg[56]_i_1_n_6 ;
  wire \ip_start_count_reg[56]_i_1_n_7 ;
  wire \ip_start_count_reg[56]_i_1_n_8 ;
  wire \ip_start_count_reg[56]_i_1_n_9 ;
  wire \ip_start_count_reg[8]_i_1_n_0 ;
  wire \ip_start_count_reg[8]_i_1_n_1 ;
  wire \ip_start_count_reg[8]_i_1_n_10 ;
  wire \ip_start_count_reg[8]_i_1_n_11 ;
  wire \ip_start_count_reg[8]_i_1_n_12 ;
  wire \ip_start_count_reg[8]_i_1_n_13 ;
  wire \ip_start_count_reg[8]_i_1_n_14 ;
  wire \ip_start_count_reg[8]_i_1_n_15 ;
  wire \ip_start_count_reg[8]_i_1_n_2 ;
  wire \ip_start_count_reg[8]_i_1_n_3 ;
  wire \ip_start_count_reg[8]_i_1_n_4 ;
  wire \ip_start_count_reg[8]_i_1_n_5 ;
  wire \ip_start_count_reg[8]_i_1_n_6 ;
  wire \ip_start_count_reg[8]_i_1_n_7 ;
  wire \ip_start_count_reg[8]_i_1_n_8 ;
  wire \ip_start_count_reg[8]_i_1_n_9 ;
  wire \ip_start_count_reg_n_0_[0] ;
  wire \ip_start_count_reg_n_0_[10] ;
  wire \ip_start_count_reg_n_0_[11] ;
  wire \ip_start_count_reg_n_0_[12] ;
  wire \ip_start_count_reg_n_0_[13] ;
  wire \ip_start_count_reg_n_0_[14] ;
  wire \ip_start_count_reg_n_0_[15] ;
  wire \ip_start_count_reg_n_0_[16] ;
  wire \ip_start_count_reg_n_0_[17] ;
  wire \ip_start_count_reg_n_0_[18] ;
  wire \ip_start_count_reg_n_0_[19] ;
  wire \ip_start_count_reg_n_0_[1] ;
  wire \ip_start_count_reg_n_0_[20] ;
  wire \ip_start_count_reg_n_0_[21] ;
  wire \ip_start_count_reg_n_0_[22] ;
  wire \ip_start_count_reg_n_0_[23] ;
  wire \ip_start_count_reg_n_0_[24] ;
  wire \ip_start_count_reg_n_0_[25] ;
  wire \ip_start_count_reg_n_0_[26] ;
  wire \ip_start_count_reg_n_0_[27] ;
  wire \ip_start_count_reg_n_0_[28] ;
  wire \ip_start_count_reg_n_0_[29] ;
  wire \ip_start_count_reg_n_0_[2] ;
  wire \ip_start_count_reg_n_0_[30] ;
  wire \ip_start_count_reg_n_0_[31] ;
  wire \ip_start_count_reg_n_0_[3] ;
  wire \ip_start_count_reg_n_0_[4] ;
  wire \ip_start_count_reg_n_0_[5] ;
  wire \ip_start_count_reg_n_0_[6] ;
  wire \ip_start_count_reg_n_0_[7] ;
  wire \ip_start_count_reg_n_0_[8] ;
  wire \ip_start_count_reg_n_0_[9] ;
  wire [63:0]\max_ctr_reg[63] ;
  wire mon_clk;
  wire rd_en;
  wire \sample_ctr_val_reg[32] ;
  wire \sample_ctr_val_reg[33] ;
  wire \sample_ctr_val_reg[34] ;
  wire \sample_ctr_val_reg[35] ;
  wire \sample_ctr_val_reg[36] ;
  wire \sample_ctr_val_reg[37] ;
  wire \sample_ctr_val_reg[38] ;
  wire \sample_ctr_val_reg[39] ;
  wire \sample_ctr_val_reg[40] ;
  wire \sample_ctr_val_reg[41] ;
  wire \sample_ctr_val_reg[42] ;
  wire \sample_ctr_val_reg[43] ;
  wire \sample_ctr_val_reg[44] ;
  wire \sample_ctr_val_reg[45] ;
  wire \sample_ctr_val_reg[46] ;
  wire \sample_ctr_val_reg[47] ;
  wire \sample_ctr_val_reg[48] ;
  wire \sample_ctr_val_reg[49] ;
  wire \sample_ctr_val_reg[50] ;
  wire \sample_ctr_val_reg[51] ;
  wire \sample_ctr_val_reg[52] ;
  wire \sample_ctr_val_reg[53] ;
  wire \sample_ctr_val_reg[54] ;
  wire \sample_ctr_val_reg[55] ;
  wire \sample_ctr_val_reg[56] ;
  wire \sample_ctr_val_reg[57] ;
  wire \sample_ctr_val_reg[58] ;
  wire \sample_ctr_val_reg[59] ;
  wire \sample_ctr_val_reg[60] ;
  wire \sample_ctr_val_reg[61] ;
  wire \sample_ctr_val_reg[62] ;
  wire \sample_ctr_val_reg[63] ;
  wire [31:0]\sample_data_reg[31]_0 ;
  wire [2:0]slv_reg_addr;
  wire start_pulse;
  wire [7:7]\NLW_ip_cycles_avg_reg[56]_i_1_CO_UNCONNECTED ;
  wire [7:7]\NLW_ip_exec_count_reg[56]_i_1_CO_UNCONNECTED ;
  wire [7:0]NLW_ip_idle_carry_O_UNCONNECTED;
  wire [7:0]NLW_ip_idle_carry__0_O_UNCONNECTED;
  wire [7:6]NLW_ip_idle_carry__1_CO_UNCONNECTED;
  wire [7:0]NLW_ip_idle_carry__1_O_UNCONNECTED;
  wire [7:0]NLW_ip_max_parallel_tranx1_carry_O_UNCONNECTED;
  wire [7:0]NLW_ip_max_parallel_tranx1_carry__0_O_UNCONNECTED;
  wire [7:0]NLW_ip_max_parallel_tranx1_carry__1_O_UNCONNECTED;
  wire [7:0]NLW_ip_max_parallel_tranx1_carry__2_O_UNCONNECTED;
  wire [7:7]\NLW_ip_start_count_reg[56]_i_1_CO_UNCONNECTED ;

  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_event_mon event_mon_cu_i
       (.E(E),
        .RST_ACTIVE(RST_ACTIVE),
        .cnt_enabled_reg_0(cnt_enabled_reg),
        .data13(data13),
        .mon_clk(mon_clk),
        .\sample_ctr_val_reg[32]_0 (\sample_ctr_val_reg[32] ),
        .\sample_ctr_val_reg[33]_0 (\sample_ctr_val_reg[33] ),
        .\sample_ctr_val_reg[34]_0 (\sample_ctr_val_reg[34] ),
        .\sample_ctr_val_reg[35]_0 (\sample_ctr_val_reg[35] ),
        .\sample_ctr_val_reg[36]_0 (\sample_ctr_val_reg[36] ),
        .\sample_ctr_val_reg[37]_0 (\sample_ctr_val_reg[37] ),
        .\sample_ctr_val_reg[38]_0 (\sample_ctr_val_reg[38] ),
        .\sample_ctr_val_reg[39]_0 (\sample_ctr_val_reg[39] ),
        .\sample_ctr_val_reg[40]_0 (\sample_ctr_val_reg[40] ),
        .\sample_ctr_val_reg[41]_0 (\sample_ctr_val_reg[41] ),
        .\sample_ctr_val_reg[42]_0 (\sample_ctr_val_reg[42] ),
        .\sample_ctr_val_reg[43]_0 (\sample_ctr_val_reg[43] ),
        .\sample_ctr_val_reg[44]_0 (\sample_ctr_val_reg[44] ),
        .\sample_ctr_val_reg[45]_0 (\sample_ctr_val_reg[45] ),
        .\sample_ctr_val_reg[46]_0 (\sample_ctr_val_reg[46] ),
        .\sample_ctr_val_reg[47]_0 (\sample_ctr_val_reg[47] ),
        .\sample_ctr_val_reg[48]_0 (\sample_ctr_val_reg[48] ),
        .\sample_ctr_val_reg[49]_0 (\sample_ctr_val_reg[49] ),
        .\sample_ctr_val_reg[50]_0 (\sample_ctr_val_reg[50] ),
        .\sample_ctr_val_reg[51]_0 (\sample_ctr_val_reg[51] ),
        .\sample_ctr_val_reg[52]_0 (\sample_ctr_val_reg[52] ),
        .\sample_ctr_val_reg[53]_0 (\sample_ctr_val_reg[53] ),
        .\sample_ctr_val_reg[54]_0 (\sample_ctr_val_reg[54] ),
        .\sample_ctr_val_reg[55]_0 (\sample_ctr_val_reg[55] ),
        .\sample_ctr_val_reg[56]_0 (\sample_ctr_val_reg[56] ),
        .\sample_ctr_val_reg[57]_0 (\sample_ctr_val_reg[57] ),
        .\sample_ctr_val_reg[58]_0 (\sample_ctr_val_reg[58] ),
        .\sample_ctr_val_reg[59]_0 (\sample_ctr_val_reg[59] ),
        .\sample_ctr_val_reg[60]_0 (\sample_ctr_val_reg[60] ),
        .\sample_ctr_val_reg[61]_0 (\sample_ctr_val_reg[61] ),
        .\sample_ctr_val_reg[62]_0 (\sample_ctr_val_reg[62] ),
        .\sample_ctr_val_reg[63]_0 (\sample_ctr_val_reg[63] ),
        .\sample_data_reg[0] (\ip_max_parallel_tranx_reg_n_0_[0] ),
        .\sample_data_reg[10] (\ip_max_parallel_tranx_reg_n_0_[10] ),
        .\sample_data_reg[11] (\ip_max_parallel_tranx_reg_n_0_[11] ),
        .\sample_data_reg[12] (\ip_max_parallel_tranx_reg_n_0_[12] ),
        .\sample_data_reg[13] (\ip_max_parallel_tranx_reg_n_0_[13] ),
        .\sample_data_reg[14] (\ip_max_parallel_tranx_reg_n_0_[14] ),
        .\sample_data_reg[15] (\ip_max_parallel_tranx_reg_n_0_[15] ),
        .\sample_data_reg[16] (\ip_max_parallel_tranx_reg_n_0_[16] ),
        .\sample_data_reg[17] (\ip_max_parallel_tranx_reg_n_0_[17] ),
        .\sample_data_reg[18] (\ip_max_parallel_tranx_reg_n_0_[18] ),
        .\sample_data_reg[19] (\ip_max_parallel_tranx_reg_n_0_[19] ),
        .\sample_data_reg[1] (\ip_max_parallel_tranx_reg_n_0_[1] ),
        .\sample_data_reg[20] (\ip_max_parallel_tranx_reg_n_0_[20] ),
        .\sample_data_reg[21] (\ip_max_parallel_tranx_reg_n_0_[21] ),
        .\sample_data_reg[22] (\ip_max_parallel_tranx_reg_n_0_[22] ),
        .\sample_data_reg[23] (\ip_max_parallel_tranx_reg_n_0_[23] ),
        .\sample_data_reg[24] (\ip_max_parallel_tranx_reg_n_0_[24] ),
        .\sample_data_reg[25] (\ip_max_parallel_tranx_reg_n_0_[25] ),
        .\sample_data_reg[26] (\ip_max_parallel_tranx_reg_n_0_[26] ),
        .\sample_data_reg[27] (\ip_max_parallel_tranx_reg_n_0_[27] ),
        .\sample_data_reg[28] (\ip_max_parallel_tranx_reg_n_0_[28] ),
        .\sample_data_reg[29] (\ip_max_parallel_tranx_reg_n_0_[29] ),
        .\sample_data_reg[2] (\ip_max_parallel_tranx_reg_n_0_[2] ),
        .\sample_data_reg[30] (\ip_max_parallel_tranx_reg_n_0_[30] ),
        .\sample_data_reg[31] (\ip_max_parallel_tranx_reg_n_0_[31] ),
        .\sample_data_reg[3] (\ip_max_parallel_tranx_reg_n_0_[3] ),
        .\sample_data_reg[4] (\ip_max_parallel_tranx_reg_n_0_[4] ),
        .\sample_data_reg[5] (\ip_max_parallel_tranx_reg_n_0_[5] ),
        .\sample_data_reg[6] (\ip_max_parallel_tranx_reg_n_0_[6] ),
        .\sample_data_reg[7] (\ip_max_parallel_tranx_reg_n_0_[7] ),
        .\sample_data_reg[8] (\ip_max_parallel_tranx_reg_n_0_[8] ),
        .\sample_data_reg[9] (\ip_max_parallel_tranx_reg_n_0_[9] ),
        .slv_reg_addr(slv_reg_addr[1:0]));
  LUT6 #(
    .INIT(64'hFFFFFFF888888888)) 
    \ip_cur_tranx[0]_i_1 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(\ip_cur_tranx[0]_i_3_n_0 ),
        .I3(\ip_cur_tranx[0]_i_4_n_0 ),
        .I4(\ip_cur_tranx[0]_i_5_n_0 ),
        .I5(ip_exec_count0),
        .O(\ip_cur_tranx[0]_i_1_n_0 ));
  LUT2 #(
    .INIT(4'hE)) 
    \ip_cur_tranx[0]_i_22 
       (.I0(ip_cur_tranx_reg[52]),
        .I1(ip_cur_tranx_reg[53]),
        .O(\ip_cur_tranx[0]_i_22_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFFFFFFFE)) 
    \ip_cur_tranx[0]_i_23 
       (.I0(\ip_cur_tranx[0]_i_26_n_0 ),
        .I1(ip_cur_tranx_reg[47]),
        .I2(ip_cur_tranx_reg[46]),
        .I3(ip_cur_tranx_reg[45]),
        .I4(ip_cur_tranx_reg[44]),
        .I5(\ip_cur_tranx[0]_i_27_n_0 ),
        .O(\ip_cur_tranx[0]_i_23_n_0 ));
  LUT2 #(
    .INIT(4'hE)) 
    \ip_cur_tranx[0]_i_24 
       (.I0(ip_cur_tranx_reg[58]),
        .I1(ip_cur_tranx_reg[59]),
        .O(\ip_cur_tranx[0]_i_24_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFFFFFFFE)) 
    \ip_cur_tranx[0]_i_25 
       (.I0(\ip_cur_tranx[0]_i_28_n_0 ),
        .I1(\ip_cur_tranx[0]_i_29_n_0 ),
        .I2(ip_cur_tranx_reg[25]),
        .I3(ip_cur_tranx_reg[24]),
        .I4(ip_cur_tranx_reg[23]),
        .I5(ip_cur_tranx_reg[22]),
        .O(\ip_cur_tranx[0]_i_25_n_0 ));
  LUT4 #(
    .INIT(16'hFFFE)) 
    \ip_cur_tranx[0]_i_26 
       (.I0(ip_cur_tranx_reg[43]),
        .I1(ip_cur_tranx_reg[42]),
        .I2(ip_cur_tranx_reg[41]),
        .I3(ip_cur_tranx_reg[40]),
        .O(\ip_cur_tranx[0]_i_26_n_0 ));
  LUT5 #(
    .INIT(32'hFFFFFFFE)) 
    \ip_cur_tranx[0]_i_27 
       (.I0(ip_cur_tranx_reg[36]),
        .I1(ip_cur_tranx_reg[37]),
        .I2(ip_cur_tranx_reg[38]),
        .I3(ip_cur_tranx_reg[39]),
        .I4(\ip_cur_tranx[0]_i_30_n_0 ),
        .O(\ip_cur_tranx[0]_i_27_n_0 ));
  LUT5 #(
    .INIT(32'hFFFFFFFE)) 
    \ip_cur_tranx[0]_i_28 
       (.I0(\ip_cur_tranx[0]_i_31_n_0 ),
        .I1(ip_cur_tranx_reg[26]),
        .I2(ip_cur_tranx_reg[27]),
        .I3(ip_cur_tranx_reg[28]),
        .I4(ip_cur_tranx_reg[29]),
        .O(\ip_cur_tranx[0]_i_28_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFFFFFFFE)) 
    \ip_cur_tranx[0]_i_29 
       (.I0(ip_cur_tranx_reg[17]),
        .I1(ip_cur_tranx_reg[16]),
        .I2(\ip_cur_tranx[0]_i_32_n_0 ),
        .I3(\ip_cur_tranx[0]_i_33_n_0 ),
        .I4(\ip_cur_tranx[0]_i_34_n_0 ),
        .I5(\ip_cur_tranx[0]_i_35_n_0 ),
        .O(\ip_cur_tranx[0]_i_29_n_0 ));
  LUT4 #(
    .INIT(16'hFFFE)) 
    \ip_cur_tranx[0]_i_3 
       (.I0(ip_cur_tranx_reg[63]),
        .I1(ip_cur_tranx_reg[62]),
        .I2(ip_cur_tranx_reg[61]),
        .I3(ip_cur_tranx_reg[60]),
        .O(\ip_cur_tranx[0]_i_3_n_0 ));
  LUT4 #(
    .INIT(16'hFFFE)) 
    \ip_cur_tranx[0]_i_30 
       (.I0(ip_cur_tranx_reg[35]),
        .I1(ip_cur_tranx_reg[34]),
        .I2(ip_cur_tranx_reg[33]),
        .I3(ip_cur_tranx_reg[32]),
        .O(\ip_cur_tranx[0]_i_30_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFFFFFFFE)) 
    \ip_cur_tranx[0]_i_31 
       (.I0(ip_cur_tranx_reg[18]),
        .I1(ip_cur_tranx_reg[19]),
        .I2(ip_cur_tranx_reg[20]),
        .I3(ip_cur_tranx_reg[21]),
        .I4(ip_cur_tranx_reg[31]),
        .I5(ip_cur_tranx_reg[30]),
        .O(\ip_cur_tranx[0]_i_31_n_0 ));
  LUT4 #(
    .INIT(16'hFFFE)) 
    \ip_cur_tranx[0]_i_32 
       (.I0(ip_cur_tranx_reg[7]),
        .I1(ip_cur_tranx_reg[6]),
        .I2(ip_cur_tranx_reg[5]),
        .I3(ip_cur_tranx_reg[4]),
        .O(\ip_cur_tranx[0]_i_32_n_0 ));
  LUT4 #(
    .INIT(16'hFFFE)) 
    \ip_cur_tranx[0]_i_33 
       (.I0(ip_cur_tranx_reg[3]),
        .I1(ip_cur_tranx_reg[2]),
        .I2(ip_cur_tranx_reg[1]),
        .I3(ip_cur_tranx_reg[0]),
        .O(\ip_cur_tranx[0]_i_33_n_0 ));
  LUT4 #(
    .INIT(16'hFFFE)) 
    \ip_cur_tranx[0]_i_34 
       (.I0(ip_cur_tranx_reg[15]),
        .I1(ip_cur_tranx_reg[14]),
        .I2(ip_cur_tranx_reg[13]),
        .I3(ip_cur_tranx_reg[12]),
        .O(\ip_cur_tranx[0]_i_34_n_0 ));
  LUT4 #(
    .INIT(16'hFFFE)) 
    \ip_cur_tranx[0]_i_35 
       (.I0(ip_cur_tranx_reg[11]),
        .I1(ip_cur_tranx_reg[10]),
        .I2(ip_cur_tranx_reg[9]),
        .I3(ip_cur_tranx_reg[8]),
        .O(\ip_cur_tranx[0]_i_35_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFFFFFFFE)) 
    \ip_cur_tranx[0]_i_4 
       (.I0(\ip_cur_tranx[0]_i_22_n_0 ),
        .I1(ip_cur_tranx_reg[54]),
        .I2(ip_cur_tranx_reg[55]),
        .I3(\ip_cur_tranx[0]_i_23_n_0 ),
        .I4(ip_cur_tranx_reg[48]),
        .I5(ip_cur_tranx_reg[49]),
        .O(\ip_cur_tranx[0]_i_4_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFFFFFFFFFE)) 
    \ip_cur_tranx[0]_i_5 
       (.I0(ip_cur_tranx_reg[56]),
        .I1(ip_cur_tranx_reg[57]),
        .I2(\ip_cur_tranx[0]_i_24_n_0 ),
        .I3(\ip_cur_tranx[0]_i_25_n_0 ),
        .I4(ip_cur_tranx_reg[50]),
        .I5(ip_cur_tranx_reg[51]),
        .O(\ip_cur_tranx[0]_i_5_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[0] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(O[0]),
        .Q(ip_cur_tranx_reg[0]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[10] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[15]_0 [2]),
        .Q(ip_cur_tranx_reg[10]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[11] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[15]_0 [3]),
        .Q(ip_cur_tranx_reg[11]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[12] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[15]_0 [4]),
        .Q(ip_cur_tranx_reg[12]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[13] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[15]_0 [5]),
        .Q(ip_cur_tranx_reg[13]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[14] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[15]_0 [6]),
        .Q(ip_cur_tranx_reg[14]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[15] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[15]_0 [7]),
        .Q(ip_cur_tranx_reg[15]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[16] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[23]_0 [0]),
        .Q(ip_cur_tranx_reg[16]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[17] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[23]_0 [1]),
        .Q(ip_cur_tranx_reg[17]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[18] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[23]_0 [2]),
        .Q(ip_cur_tranx_reg[18]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[19] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[23]_0 [3]),
        .Q(ip_cur_tranx_reg[19]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[1] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(O[1]),
        .Q(ip_cur_tranx_reg[1]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[20] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[23]_0 [4]),
        .Q(ip_cur_tranx_reg[20]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[21] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[23]_0 [5]),
        .Q(ip_cur_tranx_reg[21]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[22] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[23]_0 [6]),
        .Q(ip_cur_tranx_reg[22]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[23] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[23]_0 [7]),
        .Q(ip_cur_tranx_reg[23]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[24] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[31]_0 [0]),
        .Q(ip_cur_tranx_reg[24]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[25] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[31]_0 [1]),
        .Q(ip_cur_tranx_reg[25]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[26] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[31]_0 [2]),
        .Q(ip_cur_tranx_reg[26]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[27] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[31]_0 [3]),
        .Q(ip_cur_tranx_reg[27]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[28] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[31]_0 [4]),
        .Q(ip_cur_tranx_reg[28]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[29] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[31]_0 [5]),
        .Q(ip_cur_tranx_reg[29]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[2] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(O[2]),
        .Q(ip_cur_tranx_reg[2]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[30] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[31]_0 [6]),
        .Q(ip_cur_tranx_reg[30]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[31] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[31]_0 [7]),
        .Q(ip_cur_tranx_reg[31]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[32] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[39]_0 [0]),
        .Q(ip_cur_tranx_reg[32]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[33] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[39]_0 [1]),
        .Q(ip_cur_tranx_reg[33]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[34] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[39]_0 [2]),
        .Q(ip_cur_tranx_reg[34]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[35] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[39]_0 [3]),
        .Q(ip_cur_tranx_reg[35]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[36] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[39]_0 [4]),
        .Q(ip_cur_tranx_reg[36]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[37] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[39]_0 [5]),
        .Q(ip_cur_tranx_reg[37]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[38] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[39]_0 [6]),
        .Q(ip_cur_tranx_reg[38]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[39] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[39]_0 [7]),
        .Q(ip_cur_tranx_reg[39]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[3] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(O[3]),
        .Q(ip_cur_tranx_reg[3]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[40] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[47]_0 [0]),
        .Q(ip_cur_tranx_reg[40]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[41] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[47]_0 [1]),
        .Q(ip_cur_tranx_reg[41]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[42] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[47]_0 [2]),
        .Q(ip_cur_tranx_reg[42]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[43] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[47]_0 [3]),
        .Q(ip_cur_tranx_reg[43]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[44] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[47]_0 [4]),
        .Q(ip_cur_tranx_reg[44]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[45] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[47]_0 [5]),
        .Q(ip_cur_tranx_reg[45]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[46] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[47]_0 [6]),
        .Q(ip_cur_tranx_reg[46]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[47] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[47]_0 [7]),
        .Q(ip_cur_tranx_reg[47]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[48] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[55]_0 [0]),
        .Q(ip_cur_tranx_reg[48]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[49] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[55]_0 [1]),
        .Q(ip_cur_tranx_reg[49]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[4] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(O[4]),
        .Q(ip_cur_tranx_reg[4]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[50] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[55]_0 [2]),
        .Q(ip_cur_tranx_reg[50]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[51] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[55]_0 [3]),
        .Q(ip_cur_tranx_reg[51]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[52] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[55]_0 [4]),
        .Q(ip_cur_tranx_reg[52]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[53] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[55]_0 [5]),
        .Q(ip_cur_tranx_reg[53]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[54] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[55]_0 [6]),
        .Q(ip_cur_tranx_reg[54]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[55] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[55]_0 [7]),
        .Q(ip_cur_tranx_reg[55]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[56] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[63]_0 [0]),
        .Q(ip_cur_tranx_reg[56]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[57] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[63]_0 [1]),
        .Q(ip_cur_tranx_reg[57]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[58] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[63]_0 [2]),
        .Q(ip_cur_tranx_reg[58]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[59] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[63]_0 [3]),
        .Q(ip_cur_tranx_reg[59]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[5] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(O[5]),
        .Q(ip_cur_tranx_reg[5]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[60] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[63]_0 [4]),
        .Q(ip_cur_tranx_reg[60]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[61] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[63]_0 [5]),
        .Q(ip_cur_tranx_reg[61]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[62] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[63]_0 [6]),
        .Q(ip_cur_tranx_reg[62]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[63] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[63]_0 [7]),
        .Q(ip_cur_tranx_reg[63]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[6] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(O[6]),
        .Q(ip_cur_tranx_reg[6]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[7] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(O[7]),
        .Q(ip_cur_tranx_reg[7]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[8] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[15]_0 [0]),
        .Q(ip_cur_tranx_reg[8]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cur_tranx_reg[9] 
       (.C(mon_clk),
        .CE(\ip_cur_tranx[0]_i_1_n_0 ),
        .D(\ip_cur_tranx_reg[15]_0 [1]),
        .Q(ip_cur_tranx_reg[9]),
        .R(RST_ACTIVE));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[0]_i_2 
       (.I0(ip_cur_tranx_reg[7]),
        .I1(\ip_cycles_avg_reg_n_0_[7] ),
        .O(\ip_cycles_avg[0]_i_2_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[0]_i_3 
       (.I0(ip_cur_tranx_reg[6]),
        .I1(\ip_cycles_avg_reg_n_0_[6] ),
        .O(\ip_cycles_avg[0]_i_3_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[0]_i_4 
       (.I0(ip_cur_tranx_reg[5]),
        .I1(\ip_cycles_avg_reg_n_0_[5] ),
        .O(\ip_cycles_avg[0]_i_4_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[0]_i_5 
       (.I0(ip_cur_tranx_reg[4]),
        .I1(\ip_cycles_avg_reg_n_0_[4] ),
        .O(\ip_cycles_avg[0]_i_5_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[0]_i_6 
       (.I0(ip_cur_tranx_reg[3]),
        .I1(\ip_cycles_avg_reg_n_0_[3] ),
        .O(\ip_cycles_avg[0]_i_6_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[0]_i_7 
       (.I0(ip_cur_tranx_reg[2]),
        .I1(\ip_cycles_avg_reg_n_0_[2] ),
        .O(\ip_cycles_avg[0]_i_7_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[0]_i_8 
       (.I0(ip_cur_tranx_reg[1]),
        .I1(\ip_cycles_avg_reg_n_0_[1] ),
        .O(\ip_cycles_avg[0]_i_8_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[0]_i_9 
       (.I0(ip_cur_tranx_reg[0]),
        .I1(\ip_cycles_avg_reg_n_0_[0] ),
        .O(\ip_cycles_avg[0]_i_9_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[16]_i_2 
       (.I0(ip_cur_tranx_reg[23]),
        .I1(\ip_cycles_avg_reg_n_0_[23] ),
        .O(\ip_cycles_avg[16]_i_2_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[16]_i_3 
       (.I0(ip_cur_tranx_reg[22]),
        .I1(\ip_cycles_avg_reg_n_0_[22] ),
        .O(\ip_cycles_avg[16]_i_3_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[16]_i_4 
       (.I0(ip_cur_tranx_reg[21]),
        .I1(\ip_cycles_avg_reg_n_0_[21] ),
        .O(\ip_cycles_avg[16]_i_4_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[16]_i_5 
       (.I0(ip_cur_tranx_reg[20]),
        .I1(\ip_cycles_avg_reg_n_0_[20] ),
        .O(\ip_cycles_avg[16]_i_5_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[16]_i_6 
       (.I0(ip_cur_tranx_reg[19]),
        .I1(\ip_cycles_avg_reg_n_0_[19] ),
        .O(\ip_cycles_avg[16]_i_6_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[16]_i_7 
       (.I0(ip_cur_tranx_reg[18]),
        .I1(\ip_cycles_avg_reg_n_0_[18] ),
        .O(\ip_cycles_avg[16]_i_7_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[16]_i_8 
       (.I0(ip_cur_tranx_reg[17]),
        .I1(\ip_cycles_avg_reg_n_0_[17] ),
        .O(\ip_cycles_avg[16]_i_8_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[16]_i_9 
       (.I0(ip_cur_tranx_reg[16]),
        .I1(\ip_cycles_avg_reg_n_0_[16] ),
        .O(\ip_cycles_avg[16]_i_9_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[24]_i_2 
       (.I0(ip_cur_tranx_reg[31]),
        .I1(\ip_cycles_avg_reg_n_0_[31] ),
        .O(\ip_cycles_avg[24]_i_2_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[24]_i_3 
       (.I0(ip_cur_tranx_reg[30]),
        .I1(\ip_cycles_avg_reg_n_0_[30] ),
        .O(\ip_cycles_avg[24]_i_3_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[24]_i_4 
       (.I0(ip_cur_tranx_reg[29]),
        .I1(\ip_cycles_avg_reg_n_0_[29] ),
        .O(\ip_cycles_avg[24]_i_4_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[24]_i_5 
       (.I0(ip_cur_tranx_reg[28]),
        .I1(\ip_cycles_avg_reg_n_0_[28] ),
        .O(\ip_cycles_avg[24]_i_5_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[24]_i_6 
       (.I0(ip_cur_tranx_reg[27]),
        .I1(\ip_cycles_avg_reg_n_0_[27] ),
        .O(\ip_cycles_avg[24]_i_6_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[24]_i_7 
       (.I0(ip_cur_tranx_reg[26]),
        .I1(\ip_cycles_avg_reg_n_0_[26] ),
        .O(\ip_cycles_avg[24]_i_7_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[24]_i_8 
       (.I0(ip_cur_tranx_reg[25]),
        .I1(\ip_cycles_avg_reg_n_0_[25] ),
        .O(\ip_cycles_avg[24]_i_8_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[24]_i_9 
       (.I0(ip_cur_tranx_reg[24]),
        .I1(\ip_cycles_avg_reg_n_0_[24] ),
        .O(\ip_cycles_avg[24]_i_9_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[32]_i_2 
       (.I0(ip_cur_tranx_reg[39]),
        .I1(data6[7]),
        .O(\ip_cycles_avg[32]_i_2_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[32]_i_3 
       (.I0(ip_cur_tranx_reg[38]),
        .I1(data6[6]),
        .O(\ip_cycles_avg[32]_i_3_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[32]_i_4 
       (.I0(ip_cur_tranx_reg[37]),
        .I1(data6[5]),
        .O(\ip_cycles_avg[32]_i_4_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[32]_i_5 
       (.I0(ip_cur_tranx_reg[36]),
        .I1(data6[4]),
        .O(\ip_cycles_avg[32]_i_5_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[32]_i_6 
       (.I0(ip_cur_tranx_reg[35]),
        .I1(data6[3]),
        .O(\ip_cycles_avg[32]_i_6_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[32]_i_7 
       (.I0(ip_cur_tranx_reg[34]),
        .I1(data6[2]),
        .O(\ip_cycles_avg[32]_i_7_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[32]_i_8 
       (.I0(ip_cur_tranx_reg[33]),
        .I1(data6[1]),
        .O(\ip_cycles_avg[32]_i_8_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[32]_i_9 
       (.I0(ip_cur_tranx_reg[32]),
        .I1(data6[0]),
        .O(\ip_cycles_avg[32]_i_9_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[40]_i_2 
       (.I0(ip_cur_tranx_reg[47]),
        .I1(data6[15]),
        .O(\ip_cycles_avg[40]_i_2_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[40]_i_3 
       (.I0(ip_cur_tranx_reg[46]),
        .I1(data6[14]),
        .O(\ip_cycles_avg[40]_i_3_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[40]_i_4 
       (.I0(ip_cur_tranx_reg[45]),
        .I1(data6[13]),
        .O(\ip_cycles_avg[40]_i_4_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[40]_i_5 
       (.I0(ip_cur_tranx_reg[44]),
        .I1(data6[12]),
        .O(\ip_cycles_avg[40]_i_5_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[40]_i_6 
       (.I0(ip_cur_tranx_reg[43]),
        .I1(data6[11]),
        .O(\ip_cycles_avg[40]_i_6_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[40]_i_7 
       (.I0(ip_cur_tranx_reg[42]),
        .I1(data6[10]),
        .O(\ip_cycles_avg[40]_i_7_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[40]_i_8 
       (.I0(ip_cur_tranx_reg[41]),
        .I1(data6[9]),
        .O(\ip_cycles_avg[40]_i_8_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[40]_i_9 
       (.I0(ip_cur_tranx_reg[40]),
        .I1(data6[8]),
        .O(\ip_cycles_avg[40]_i_9_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[48]_i_2 
       (.I0(ip_cur_tranx_reg[55]),
        .I1(data6[23]),
        .O(\ip_cycles_avg[48]_i_2_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[48]_i_3 
       (.I0(ip_cur_tranx_reg[54]),
        .I1(data6[22]),
        .O(\ip_cycles_avg[48]_i_3_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[48]_i_4 
       (.I0(ip_cur_tranx_reg[53]),
        .I1(data6[21]),
        .O(\ip_cycles_avg[48]_i_4_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[48]_i_5 
       (.I0(ip_cur_tranx_reg[52]),
        .I1(data6[20]),
        .O(\ip_cycles_avg[48]_i_5_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[48]_i_6 
       (.I0(ip_cur_tranx_reg[51]),
        .I1(data6[19]),
        .O(\ip_cycles_avg[48]_i_6_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[48]_i_7 
       (.I0(ip_cur_tranx_reg[50]),
        .I1(data6[18]),
        .O(\ip_cycles_avg[48]_i_7_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[48]_i_8 
       (.I0(ip_cur_tranx_reg[49]),
        .I1(data6[17]),
        .O(\ip_cycles_avg[48]_i_8_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[48]_i_9 
       (.I0(ip_cur_tranx_reg[48]),
        .I1(data6[16]),
        .O(\ip_cycles_avg[48]_i_9_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[56]_i_2 
       (.I0(ip_cur_tranx_reg[63]),
        .I1(data6[31]),
        .O(\ip_cycles_avg[56]_i_2_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[56]_i_3 
       (.I0(ip_cur_tranx_reg[62]),
        .I1(data6[30]),
        .O(\ip_cycles_avg[56]_i_3_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[56]_i_4 
       (.I0(ip_cur_tranx_reg[61]),
        .I1(data6[29]),
        .O(\ip_cycles_avg[56]_i_4_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[56]_i_5 
       (.I0(ip_cur_tranx_reg[60]),
        .I1(data6[28]),
        .O(\ip_cycles_avg[56]_i_5_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[56]_i_6 
       (.I0(ip_cur_tranx_reg[59]),
        .I1(data6[27]),
        .O(\ip_cycles_avg[56]_i_6_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[56]_i_7 
       (.I0(ip_cur_tranx_reg[58]),
        .I1(data6[26]),
        .O(\ip_cycles_avg[56]_i_7_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[56]_i_8 
       (.I0(ip_cur_tranx_reg[57]),
        .I1(data6[25]),
        .O(\ip_cycles_avg[56]_i_8_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[56]_i_9 
       (.I0(ip_cur_tranx_reg[56]),
        .I1(data6[24]),
        .O(\ip_cycles_avg[56]_i_9_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[8]_i_2 
       (.I0(ip_cur_tranx_reg[15]),
        .I1(\ip_cycles_avg_reg_n_0_[15] ),
        .O(\ip_cycles_avg[8]_i_2_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[8]_i_3 
       (.I0(ip_cur_tranx_reg[14]),
        .I1(\ip_cycles_avg_reg_n_0_[14] ),
        .O(\ip_cycles_avg[8]_i_3_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[8]_i_4 
       (.I0(ip_cur_tranx_reg[13]),
        .I1(\ip_cycles_avg_reg_n_0_[13] ),
        .O(\ip_cycles_avg[8]_i_4_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[8]_i_5 
       (.I0(ip_cur_tranx_reg[12]),
        .I1(\ip_cycles_avg_reg_n_0_[12] ),
        .O(\ip_cycles_avg[8]_i_5_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[8]_i_6 
       (.I0(ip_cur_tranx_reg[11]),
        .I1(\ip_cycles_avg_reg_n_0_[11] ),
        .O(\ip_cycles_avg[8]_i_6_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[8]_i_7 
       (.I0(ip_cur_tranx_reg[10]),
        .I1(\ip_cycles_avg_reg_n_0_[10] ),
        .O(\ip_cycles_avg[8]_i_7_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[8]_i_8 
       (.I0(ip_cur_tranx_reg[9]),
        .I1(\ip_cycles_avg_reg_n_0_[9] ),
        .O(\ip_cycles_avg[8]_i_8_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \ip_cycles_avg[8]_i_9 
       (.I0(ip_cur_tranx_reg[8]),
        .I1(\ip_cycles_avg_reg_n_0_[8] ),
        .O(\ip_cycles_avg[8]_i_9_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[0] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[0]_i_1_n_15 ),
        .Q(\ip_cycles_avg_reg_n_0_[0] ),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_cycles_avg_reg[0]_i_1 
       (.CI(1'b0),
        .CI_TOP(1'b0),
        .CO({\ip_cycles_avg_reg[0]_i_1_n_0 ,\ip_cycles_avg_reg[0]_i_1_n_1 ,\ip_cycles_avg_reg[0]_i_1_n_2 ,\ip_cycles_avg_reg[0]_i_1_n_3 ,\ip_cycles_avg_reg[0]_i_1_n_4 ,\ip_cycles_avg_reg[0]_i_1_n_5 ,\ip_cycles_avg_reg[0]_i_1_n_6 ,\ip_cycles_avg_reg[0]_i_1_n_7 }),
        .DI(ip_cur_tranx_reg[7:0]),
        .O({\ip_cycles_avg_reg[0]_i_1_n_8 ,\ip_cycles_avg_reg[0]_i_1_n_9 ,\ip_cycles_avg_reg[0]_i_1_n_10 ,\ip_cycles_avg_reg[0]_i_1_n_11 ,\ip_cycles_avg_reg[0]_i_1_n_12 ,\ip_cycles_avg_reg[0]_i_1_n_13 ,\ip_cycles_avg_reg[0]_i_1_n_14 ,\ip_cycles_avg_reg[0]_i_1_n_15 }),
        .S({\ip_cycles_avg[0]_i_2_n_0 ,\ip_cycles_avg[0]_i_3_n_0 ,\ip_cycles_avg[0]_i_4_n_0 ,\ip_cycles_avg[0]_i_5_n_0 ,\ip_cycles_avg[0]_i_6_n_0 ,\ip_cycles_avg[0]_i_7_n_0 ,\ip_cycles_avg[0]_i_8_n_0 ,\ip_cycles_avg[0]_i_9_n_0 }));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[10] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[8]_i_1_n_13 ),
        .Q(\ip_cycles_avg_reg_n_0_[10] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[11] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[8]_i_1_n_12 ),
        .Q(\ip_cycles_avg_reg_n_0_[11] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[12] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[8]_i_1_n_11 ),
        .Q(\ip_cycles_avg_reg_n_0_[12] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[13] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[8]_i_1_n_10 ),
        .Q(\ip_cycles_avg_reg_n_0_[13] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[14] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[8]_i_1_n_9 ),
        .Q(\ip_cycles_avg_reg_n_0_[14] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[15] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[8]_i_1_n_8 ),
        .Q(\ip_cycles_avg_reg_n_0_[15] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[16] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[16]_i_1_n_15 ),
        .Q(\ip_cycles_avg_reg_n_0_[16] ),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_cycles_avg_reg[16]_i_1 
       (.CI(\ip_cycles_avg_reg[8]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ip_cycles_avg_reg[16]_i_1_n_0 ,\ip_cycles_avg_reg[16]_i_1_n_1 ,\ip_cycles_avg_reg[16]_i_1_n_2 ,\ip_cycles_avg_reg[16]_i_1_n_3 ,\ip_cycles_avg_reg[16]_i_1_n_4 ,\ip_cycles_avg_reg[16]_i_1_n_5 ,\ip_cycles_avg_reg[16]_i_1_n_6 ,\ip_cycles_avg_reg[16]_i_1_n_7 }),
        .DI(ip_cur_tranx_reg[23:16]),
        .O({\ip_cycles_avg_reg[16]_i_1_n_8 ,\ip_cycles_avg_reg[16]_i_1_n_9 ,\ip_cycles_avg_reg[16]_i_1_n_10 ,\ip_cycles_avg_reg[16]_i_1_n_11 ,\ip_cycles_avg_reg[16]_i_1_n_12 ,\ip_cycles_avg_reg[16]_i_1_n_13 ,\ip_cycles_avg_reg[16]_i_1_n_14 ,\ip_cycles_avg_reg[16]_i_1_n_15 }),
        .S({\ip_cycles_avg[16]_i_2_n_0 ,\ip_cycles_avg[16]_i_3_n_0 ,\ip_cycles_avg[16]_i_4_n_0 ,\ip_cycles_avg[16]_i_5_n_0 ,\ip_cycles_avg[16]_i_6_n_0 ,\ip_cycles_avg[16]_i_7_n_0 ,\ip_cycles_avg[16]_i_8_n_0 ,\ip_cycles_avg[16]_i_9_n_0 }));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[17] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[16]_i_1_n_14 ),
        .Q(\ip_cycles_avg_reg_n_0_[17] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[18] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[16]_i_1_n_13 ),
        .Q(\ip_cycles_avg_reg_n_0_[18] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[19] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[16]_i_1_n_12 ),
        .Q(\ip_cycles_avg_reg_n_0_[19] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[1] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[0]_i_1_n_14 ),
        .Q(\ip_cycles_avg_reg_n_0_[1] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[20] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[16]_i_1_n_11 ),
        .Q(\ip_cycles_avg_reg_n_0_[20] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[21] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[16]_i_1_n_10 ),
        .Q(\ip_cycles_avg_reg_n_0_[21] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[22] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[16]_i_1_n_9 ),
        .Q(\ip_cycles_avg_reg_n_0_[22] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[23] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[16]_i_1_n_8 ),
        .Q(\ip_cycles_avg_reg_n_0_[23] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[24] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[24]_i_1_n_15 ),
        .Q(\ip_cycles_avg_reg_n_0_[24] ),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_cycles_avg_reg[24]_i_1 
       (.CI(\ip_cycles_avg_reg[16]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ip_cycles_avg_reg[24]_i_1_n_0 ,\ip_cycles_avg_reg[24]_i_1_n_1 ,\ip_cycles_avg_reg[24]_i_1_n_2 ,\ip_cycles_avg_reg[24]_i_1_n_3 ,\ip_cycles_avg_reg[24]_i_1_n_4 ,\ip_cycles_avg_reg[24]_i_1_n_5 ,\ip_cycles_avg_reg[24]_i_1_n_6 ,\ip_cycles_avg_reg[24]_i_1_n_7 }),
        .DI(ip_cur_tranx_reg[31:24]),
        .O({\ip_cycles_avg_reg[24]_i_1_n_8 ,\ip_cycles_avg_reg[24]_i_1_n_9 ,\ip_cycles_avg_reg[24]_i_1_n_10 ,\ip_cycles_avg_reg[24]_i_1_n_11 ,\ip_cycles_avg_reg[24]_i_1_n_12 ,\ip_cycles_avg_reg[24]_i_1_n_13 ,\ip_cycles_avg_reg[24]_i_1_n_14 ,\ip_cycles_avg_reg[24]_i_1_n_15 }),
        .S({\ip_cycles_avg[24]_i_2_n_0 ,\ip_cycles_avg[24]_i_3_n_0 ,\ip_cycles_avg[24]_i_4_n_0 ,\ip_cycles_avg[24]_i_5_n_0 ,\ip_cycles_avg[24]_i_6_n_0 ,\ip_cycles_avg[24]_i_7_n_0 ,\ip_cycles_avg[24]_i_8_n_0 ,\ip_cycles_avg[24]_i_9_n_0 }));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[25] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[24]_i_1_n_14 ),
        .Q(\ip_cycles_avg_reg_n_0_[25] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[26] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[24]_i_1_n_13 ),
        .Q(\ip_cycles_avg_reg_n_0_[26] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[27] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[24]_i_1_n_12 ),
        .Q(\ip_cycles_avg_reg_n_0_[27] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[28] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[24]_i_1_n_11 ),
        .Q(\ip_cycles_avg_reg_n_0_[28] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[29] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[24]_i_1_n_10 ),
        .Q(\ip_cycles_avg_reg_n_0_[29] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[2] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[0]_i_1_n_13 ),
        .Q(\ip_cycles_avg_reg_n_0_[2] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[30] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[24]_i_1_n_9 ),
        .Q(\ip_cycles_avg_reg_n_0_[30] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[31] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[24]_i_1_n_8 ),
        .Q(\ip_cycles_avg_reg_n_0_[31] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[32] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[32]_i_1_n_15 ),
        .Q(data6[0]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_cycles_avg_reg[32]_i_1 
       (.CI(\ip_cycles_avg_reg[24]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ip_cycles_avg_reg[32]_i_1_n_0 ,\ip_cycles_avg_reg[32]_i_1_n_1 ,\ip_cycles_avg_reg[32]_i_1_n_2 ,\ip_cycles_avg_reg[32]_i_1_n_3 ,\ip_cycles_avg_reg[32]_i_1_n_4 ,\ip_cycles_avg_reg[32]_i_1_n_5 ,\ip_cycles_avg_reg[32]_i_1_n_6 ,\ip_cycles_avg_reg[32]_i_1_n_7 }),
        .DI(ip_cur_tranx_reg[39:32]),
        .O({\ip_cycles_avg_reg[32]_i_1_n_8 ,\ip_cycles_avg_reg[32]_i_1_n_9 ,\ip_cycles_avg_reg[32]_i_1_n_10 ,\ip_cycles_avg_reg[32]_i_1_n_11 ,\ip_cycles_avg_reg[32]_i_1_n_12 ,\ip_cycles_avg_reg[32]_i_1_n_13 ,\ip_cycles_avg_reg[32]_i_1_n_14 ,\ip_cycles_avg_reg[32]_i_1_n_15 }),
        .S({\ip_cycles_avg[32]_i_2_n_0 ,\ip_cycles_avg[32]_i_3_n_0 ,\ip_cycles_avg[32]_i_4_n_0 ,\ip_cycles_avg[32]_i_5_n_0 ,\ip_cycles_avg[32]_i_6_n_0 ,\ip_cycles_avg[32]_i_7_n_0 ,\ip_cycles_avg[32]_i_8_n_0 ,\ip_cycles_avg[32]_i_9_n_0 }));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[33] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[32]_i_1_n_14 ),
        .Q(data6[1]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[34] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[32]_i_1_n_13 ),
        .Q(data6[2]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[35] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[32]_i_1_n_12 ),
        .Q(data6[3]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[36] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[32]_i_1_n_11 ),
        .Q(data6[4]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[37] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[32]_i_1_n_10 ),
        .Q(data6[5]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[38] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[32]_i_1_n_9 ),
        .Q(data6[6]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[39] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[32]_i_1_n_8 ),
        .Q(data6[7]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[3] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[0]_i_1_n_12 ),
        .Q(\ip_cycles_avg_reg_n_0_[3] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[40] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[40]_i_1_n_15 ),
        .Q(data6[8]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_cycles_avg_reg[40]_i_1 
       (.CI(\ip_cycles_avg_reg[32]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ip_cycles_avg_reg[40]_i_1_n_0 ,\ip_cycles_avg_reg[40]_i_1_n_1 ,\ip_cycles_avg_reg[40]_i_1_n_2 ,\ip_cycles_avg_reg[40]_i_1_n_3 ,\ip_cycles_avg_reg[40]_i_1_n_4 ,\ip_cycles_avg_reg[40]_i_1_n_5 ,\ip_cycles_avg_reg[40]_i_1_n_6 ,\ip_cycles_avg_reg[40]_i_1_n_7 }),
        .DI(ip_cur_tranx_reg[47:40]),
        .O({\ip_cycles_avg_reg[40]_i_1_n_8 ,\ip_cycles_avg_reg[40]_i_1_n_9 ,\ip_cycles_avg_reg[40]_i_1_n_10 ,\ip_cycles_avg_reg[40]_i_1_n_11 ,\ip_cycles_avg_reg[40]_i_1_n_12 ,\ip_cycles_avg_reg[40]_i_1_n_13 ,\ip_cycles_avg_reg[40]_i_1_n_14 ,\ip_cycles_avg_reg[40]_i_1_n_15 }),
        .S({\ip_cycles_avg[40]_i_2_n_0 ,\ip_cycles_avg[40]_i_3_n_0 ,\ip_cycles_avg[40]_i_4_n_0 ,\ip_cycles_avg[40]_i_5_n_0 ,\ip_cycles_avg[40]_i_6_n_0 ,\ip_cycles_avg[40]_i_7_n_0 ,\ip_cycles_avg[40]_i_8_n_0 ,\ip_cycles_avg[40]_i_9_n_0 }));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[41] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[40]_i_1_n_14 ),
        .Q(data6[9]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[42] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[40]_i_1_n_13 ),
        .Q(data6[10]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[43] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[40]_i_1_n_12 ),
        .Q(data6[11]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[44] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[40]_i_1_n_11 ),
        .Q(data6[12]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[45] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[40]_i_1_n_10 ),
        .Q(data6[13]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[46] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[40]_i_1_n_9 ),
        .Q(data6[14]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[47] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[40]_i_1_n_8 ),
        .Q(data6[15]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[48] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[48]_i_1_n_15 ),
        .Q(data6[16]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_cycles_avg_reg[48]_i_1 
       (.CI(\ip_cycles_avg_reg[40]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ip_cycles_avg_reg[48]_i_1_n_0 ,\ip_cycles_avg_reg[48]_i_1_n_1 ,\ip_cycles_avg_reg[48]_i_1_n_2 ,\ip_cycles_avg_reg[48]_i_1_n_3 ,\ip_cycles_avg_reg[48]_i_1_n_4 ,\ip_cycles_avg_reg[48]_i_1_n_5 ,\ip_cycles_avg_reg[48]_i_1_n_6 ,\ip_cycles_avg_reg[48]_i_1_n_7 }),
        .DI(ip_cur_tranx_reg[55:48]),
        .O({\ip_cycles_avg_reg[48]_i_1_n_8 ,\ip_cycles_avg_reg[48]_i_1_n_9 ,\ip_cycles_avg_reg[48]_i_1_n_10 ,\ip_cycles_avg_reg[48]_i_1_n_11 ,\ip_cycles_avg_reg[48]_i_1_n_12 ,\ip_cycles_avg_reg[48]_i_1_n_13 ,\ip_cycles_avg_reg[48]_i_1_n_14 ,\ip_cycles_avg_reg[48]_i_1_n_15 }),
        .S({\ip_cycles_avg[48]_i_2_n_0 ,\ip_cycles_avg[48]_i_3_n_0 ,\ip_cycles_avg[48]_i_4_n_0 ,\ip_cycles_avg[48]_i_5_n_0 ,\ip_cycles_avg[48]_i_6_n_0 ,\ip_cycles_avg[48]_i_7_n_0 ,\ip_cycles_avg[48]_i_8_n_0 ,\ip_cycles_avg[48]_i_9_n_0 }));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[49] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[48]_i_1_n_14 ),
        .Q(data6[17]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[4] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[0]_i_1_n_11 ),
        .Q(\ip_cycles_avg_reg_n_0_[4] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[50] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[48]_i_1_n_13 ),
        .Q(data6[18]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[51] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[48]_i_1_n_12 ),
        .Q(data6[19]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[52] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[48]_i_1_n_11 ),
        .Q(data6[20]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[53] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[48]_i_1_n_10 ),
        .Q(data6[21]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[54] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[48]_i_1_n_9 ),
        .Q(data6[22]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[55] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[48]_i_1_n_8 ),
        .Q(data6[23]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[56] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[56]_i_1_n_15 ),
        .Q(data6[24]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_cycles_avg_reg[56]_i_1 
       (.CI(\ip_cycles_avg_reg[48]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\NLW_ip_cycles_avg_reg[56]_i_1_CO_UNCONNECTED [7],\ip_cycles_avg_reg[56]_i_1_n_1 ,\ip_cycles_avg_reg[56]_i_1_n_2 ,\ip_cycles_avg_reg[56]_i_1_n_3 ,\ip_cycles_avg_reg[56]_i_1_n_4 ,\ip_cycles_avg_reg[56]_i_1_n_5 ,\ip_cycles_avg_reg[56]_i_1_n_6 ,\ip_cycles_avg_reg[56]_i_1_n_7 }),
        .DI({1'b0,ip_cur_tranx_reg[62:56]}),
        .O({\ip_cycles_avg_reg[56]_i_1_n_8 ,\ip_cycles_avg_reg[56]_i_1_n_9 ,\ip_cycles_avg_reg[56]_i_1_n_10 ,\ip_cycles_avg_reg[56]_i_1_n_11 ,\ip_cycles_avg_reg[56]_i_1_n_12 ,\ip_cycles_avg_reg[56]_i_1_n_13 ,\ip_cycles_avg_reg[56]_i_1_n_14 ,\ip_cycles_avg_reg[56]_i_1_n_15 }),
        .S({\ip_cycles_avg[56]_i_2_n_0 ,\ip_cycles_avg[56]_i_3_n_0 ,\ip_cycles_avg[56]_i_4_n_0 ,\ip_cycles_avg[56]_i_5_n_0 ,\ip_cycles_avg[56]_i_6_n_0 ,\ip_cycles_avg[56]_i_7_n_0 ,\ip_cycles_avg[56]_i_8_n_0 ,\ip_cycles_avg[56]_i_9_n_0 }));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[57] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[56]_i_1_n_14 ),
        .Q(data6[25]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[58] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[56]_i_1_n_13 ),
        .Q(data6[26]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[59] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[56]_i_1_n_12 ),
        .Q(data6[27]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[5] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[0]_i_1_n_10 ),
        .Q(\ip_cycles_avg_reg_n_0_[5] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[60] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[56]_i_1_n_11 ),
        .Q(data6[28]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[61] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[56]_i_1_n_10 ),
        .Q(data6[29]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[62] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[56]_i_1_n_9 ),
        .Q(data6[30]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[63] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[56]_i_1_n_8 ),
        .Q(data6[31]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[6] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[0]_i_1_n_9 ),
        .Q(\ip_cycles_avg_reg_n_0_[6] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[7] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[0]_i_1_n_8 ),
        .Q(\ip_cycles_avg_reg_n_0_[7] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[8] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[8]_i_1_n_15 ),
        .Q(\ip_cycles_avg_reg_n_0_[8] ),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_cycles_avg_reg[8]_i_1 
       (.CI(\ip_cycles_avg_reg[0]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ip_cycles_avg_reg[8]_i_1_n_0 ,\ip_cycles_avg_reg[8]_i_1_n_1 ,\ip_cycles_avg_reg[8]_i_1_n_2 ,\ip_cycles_avg_reg[8]_i_1_n_3 ,\ip_cycles_avg_reg[8]_i_1_n_4 ,\ip_cycles_avg_reg[8]_i_1_n_5 ,\ip_cycles_avg_reg[8]_i_1_n_6 ,\ip_cycles_avg_reg[8]_i_1_n_7 }),
        .DI(ip_cur_tranx_reg[15:8]),
        .O({\ip_cycles_avg_reg[8]_i_1_n_8 ,\ip_cycles_avg_reg[8]_i_1_n_9 ,\ip_cycles_avg_reg[8]_i_1_n_10 ,\ip_cycles_avg_reg[8]_i_1_n_11 ,\ip_cycles_avg_reg[8]_i_1_n_12 ,\ip_cycles_avg_reg[8]_i_1_n_13 ,\ip_cycles_avg_reg[8]_i_1_n_14 ,\ip_cycles_avg_reg[8]_i_1_n_15 }),
        .S({\ip_cycles_avg[8]_i_2_n_0 ,\ip_cycles_avg[8]_i_3_n_0 ,\ip_cycles_avg[8]_i_4_n_0 ,\ip_cycles_avg[8]_i_5_n_0 ,\ip_cycles_avg[8]_i_6_n_0 ,\ip_cycles_avg[8]_i_7_n_0 ,\ip_cycles_avg[8]_i_8_n_0 ,\ip_cycles_avg[8]_i_9_n_0 }));
  FDRE #(
    .INIT(1'b0)) 
    \ip_cycles_avg_reg[9] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\ip_cycles_avg_reg[8]_i_1_n_14 ),
        .Q(\ip_cycles_avg_reg_n_0_[9] ),
        .R(RST_ACTIVE));
  LUT1 #(
    .INIT(2'h1)) 
    \ip_exec_count[0]_i_3 
       (.I0(\ip_exec_count_reg_n_0_[0] ),
        .O(\ip_exec_count[0]_i_3_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[0] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[0]_i_2_n_15 ),
        .Q(\ip_exec_count_reg_n_0_[0] ),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_exec_count_reg[0]_i_2 
       (.CI(1'b0),
        .CI_TOP(1'b0),
        .CO({\ip_exec_count_reg[0]_i_2_n_0 ,\ip_exec_count_reg[0]_i_2_n_1 ,\ip_exec_count_reg[0]_i_2_n_2 ,\ip_exec_count_reg[0]_i_2_n_3 ,\ip_exec_count_reg[0]_i_2_n_4 ,\ip_exec_count_reg[0]_i_2_n_5 ,\ip_exec_count_reg[0]_i_2_n_6 ,\ip_exec_count_reg[0]_i_2_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b1}),
        .O({\ip_exec_count_reg[0]_i_2_n_8 ,\ip_exec_count_reg[0]_i_2_n_9 ,\ip_exec_count_reg[0]_i_2_n_10 ,\ip_exec_count_reg[0]_i_2_n_11 ,\ip_exec_count_reg[0]_i_2_n_12 ,\ip_exec_count_reg[0]_i_2_n_13 ,\ip_exec_count_reg[0]_i_2_n_14 ,\ip_exec_count_reg[0]_i_2_n_15 }),
        .S({\ip_exec_count_reg_n_0_[7] ,\ip_exec_count_reg_n_0_[6] ,\ip_exec_count_reg_n_0_[5] ,\ip_exec_count_reg_n_0_[4] ,\ip_exec_count_reg_n_0_[3] ,\ip_exec_count_reg_n_0_[2] ,\ip_exec_count_reg_n_0_[1] ,\ip_exec_count[0]_i_3_n_0 }));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[10] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[8]_i_1_n_13 ),
        .Q(\ip_exec_count_reg_n_0_[10] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[11] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[8]_i_1_n_12 ),
        .Q(\ip_exec_count_reg_n_0_[11] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[12] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[8]_i_1_n_11 ),
        .Q(\ip_exec_count_reg_n_0_[12] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[13] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[8]_i_1_n_10 ),
        .Q(\ip_exec_count_reg_n_0_[13] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[14] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[8]_i_1_n_9 ),
        .Q(\ip_exec_count_reg_n_0_[14] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[15] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[8]_i_1_n_8 ),
        .Q(\ip_exec_count_reg_n_0_[15] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[16] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[16]_i_1_n_15 ),
        .Q(\ip_exec_count_reg_n_0_[16] ),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_exec_count_reg[16]_i_1 
       (.CI(\ip_exec_count_reg[8]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ip_exec_count_reg[16]_i_1_n_0 ,\ip_exec_count_reg[16]_i_1_n_1 ,\ip_exec_count_reg[16]_i_1_n_2 ,\ip_exec_count_reg[16]_i_1_n_3 ,\ip_exec_count_reg[16]_i_1_n_4 ,\ip_exec_count_reg[16]_i_1_n_5 ,\ip_exec_count_reg[16]_i_1_n_6 ,\ip_exec_count_reg[16]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\ip_exec_count_reg[16]_i_1_n_8 ,\ip_exec_count_reg[16]_i_1_n_9 ,\ip_exec_count_reg[16]_i_1_n_10 ,\ip_exec_count_reg[16]_i_1_n_11 ,\ip_exec_count_reg[16]_i_1_n_12 ,\ip_exec_count_reg[16]_i_1_n_13 ,\ip_exec_count_reg[16]_i_1_n_14 ,\ip_exec_count_reg[16]_i_1_n_15 }),
        .S({\ip_exec_count_reg_n_0_[23] ,\ip_exec_count_reg_n_0_[22] ,\ip_exec_count_reg_n_0_[21] ,\ip_exec_count_reg_n_0_[20] ,\ip_exec_count_reg_n_0_[19] ,\ip_exec_count_reg_n_0_[18] ,\ip_exec_count_reg_n_0_[17] ,\ip_exec_count_reg_n_0_[16] }));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[17] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[16]_i_1_n_14 ),
        .Q(\ip_exec_count_reg_n_0_[17] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[18] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[16]_i_1_n_13 ),
        .Q(\ip_exec_count_reg_n_0_[18] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[19] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[16]_i_1_n_12 ),
        .Q(\ip_exec_count_reg_n_0_[19] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[1] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[0]_i_2_n_14 ),
        .Q(\ip_exec_count_reg_n_0_[1] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[20] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[16]_i_1_n_11 ),
        .Q(\ip_exec_count_reg_n_0_[20] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[21] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[16]_i_1_n_10 ),
        .Q(\ip_exec_count_reg_n_0_[21] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[22] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[16]_i_1_n_9 ),
        .Q(\ip_exec_count_reg_n_0_[22] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[23] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[16]_i_1_n_8 ),
        .Q(\ip_exec_count_reg_n_0_[23] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[24] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[24]_i_1_n_15 ),
        .Q(\ip_exec_count_reg_n_0_[24] ),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_exec_count_reg[24]_i_1 
       (.CI(\ip_exec_count_reg[16]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ip_exec_count_reg[24]_i_1_n_0 ,\ip_exec_count_reg[24]_i_1_n_1 ,\ip_exec_count_reg[24]_i_1_n_2 ,\ip_exec_count_reg[24]_i_1_n_3 ,\ip_exec_count_reg[24]_i_1_n_4 ,\ip_exec_count_reg[24]_i_1_n_5 ,\ip_exec_count_reg[24]_i_1_n_6 ,\ip_exec_count_reg[24]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\ip_exec_count_reg[24]_i_1_n_8 ,\ip_exec_count_reg[24]_i_1_n_9 ,\ip_exec_count_reg[24]_i_1_n_10 ,\ip_exec_count_reg[24]_i_1_n_11 ,\ip_exec_count_reg[24]_i_1_n_12 ,\ip_exec_count_reg[24]_i_1_n_13 ,\ip_exec_count_reg[24]_i_1_n_14 ,\ip_exec_count_reg[24]_i_1_n_15 }),
        .S({\ip_exec_count_reg_n_0_[31] ,\ip_exec_count_reg_n_0_[30] ,\ip_exec_count_reg_n_0_[29] ,\ip_exec_count_reg_n_0_[28] ,\ip_exec_count_reg_n_0_[27] ,\ip_exec_count_reg_n_0_[26] ,\ip_exec_count_reg_n_0_[25] ,\ip_exec_count_reg_n_0_[24] }));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[25] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[24]_i_1_n_14 ),
        .Q(\ip_exec_count_reg_n_0_[25] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[26] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[24]_i_1_n_13 ),
        .Q(\ip_exec_count_reg_n_0_[26] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[27] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[24]_i_1_n_12 ),
        .Q(\ip_exec_count_reg_n_0_[27] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[28] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[24]_i_1_n_11 ),
        .Q(\ip_exec_count_reg_n_0_[28] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[29] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[24]_i_1_n_10 ),
        .Q(\ip_exec_count_reg_n_0_[29] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[2] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[0]_i_2_n_13 ),
        .Q(\ip_exec_count_reg_n_0_[2] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[30] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[24]_i_1_n_9 ),
        .Q(\ip_exec_count_reg_n_0_[30] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[31] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[24]_i_1_n_8 ),
        .Q(\ip_exec_count_reg_n_0_[31] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[32] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[32]_i_1_n_15 ),
        .Q(data5[0]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_exec_count_reg[32]_i_1 
       (.CI(\ip_exec_count_reg[24]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ip_exec_count_reg[32]_i_1_n_0 ,\ip_exec_count_reg[32]_i_1_n_1 ,\ip_exec_count_reg[32]_i_1_n_2 ,\ip_exec_count_reg[32]_i_1_n_3 ,\ip_exec_count_reg[32]_i_1_n_4 ,\ip_exec_count_reg[32]_i_1_n_5 ,\ip_exec_count_reg[32]_i_1_n_6 ,\ip_exec_count_reg[32]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\ip_exec_count_reg[32]_i_1_n_8 ,\ip_exec_count_reg[32]_i_1_n_9 ,\ip_exec_count_reg[32]_i_1_n_10 ,\ip_exec_count_reg[32]_i_1_n_11 ,\ip_exec_count_reg[32]_i_1_n_12 ,\ip_exec_count_reg[32]_i_1_n_13 ,\ip_exec_count_reg[32]_i_1_n_14 ,\ip_exec_count_reg[32]_i_1_n_15 }),
        .S(data5[7:0]));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[33] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[32]_i_1_n_14 ),
        .Q(data5[1]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[34] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[32]_i_1_n_13 ),
        .Q(data5[2]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[35] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[32]_i_1_n_12 ),
        .Q(data5[3]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[36] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[32]_i_1_n_11 ),
        .Q(data5[4]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[37] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[32]_i_1_n_10 ),
        .Q(data5[5]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[38] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[32]_i_1_n_9 ),
        .Q(data5[6]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[39] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[32]_i_1_n_8 ),
        .Q(data5[7]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[3] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[0]_i_2_n_12 ),
        .Q(\ip_exec_count_reg_n_0_[3] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[40] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[40]_i_1_n_15 ),
        .Q(data5[8]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_exec_count_reg[40]_i_1 
       (.CI(\ip_exec_count_reg[32]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ip_exec_count_reg[40]_i_1_n_0 ,\ip_exec_count_reg[40]_i_1_n_1 ,\ip_exec_count_reg[40]_i_1_n_2 ,\ip_exec_count_reg[40]_i_1_n_3 ,\ip_exec_count_reg[40]_i_1_n_4 ,\ip_exec_count_reg[40]_i_1_n_5 ,\ip_exec_count_reg[40]_i_1_n_6 ,\ip_exec_count_reg[40]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\ip_exec_count_reg[40]_i_1_n_8 ,\ip_exec_count_reg[40]_i_1_n_9 ,\ip_exec_count_reg[40]_i_1_n_10 ,\ip_exec_count_reg[40]_i_1_n_11 ,\ip_exec_count_reg[40]_i_1_n_12 ,\ip_exec_count_reg[40]_i_1_n_13 ,\ip_exec_count_reg[40]_i_1_n_14 ,\ip_exec_count_reg[40]_i_1_n_15 }),
        .S(data5[15:8]));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[41] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[40]_i_1_n_14 ),
        .Q(data5[9]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[42] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[40]_i_1_n_13 ),
        .Q(data5[10]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[43] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[40]_i_1_n_12 ),
        .Q(data5[11]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[44] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[40]_i_1_n_11 ),
        .Q(data5[12]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[45] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[40]_i_1_n_10 ),
        .Q(data5[13]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[46] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[40]_i_1_n_9 ),
        .Q(data5[14]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[47] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[40]_i_1_n_8 ),
        .Q(data5[15]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[48] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[48]_i_1_n_15 ),
        .Q(data5[16]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_exec_count_reg[48]_i_1 
       (.CI(\ip_exec_count_reg[40]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ip_exec_count_reg[48]_i_1_n_0 ,\ip_exec_count_reg[48]_i_1_n_1 ,\ip_exec_count_reg[48]_i_1_n_2 ,\ip_exec_count_reg[48]_i_1_n_3 ,\ip_exec_count_reg[48]_i_1_n_4 ,\ip_exec_count_reg[48]_i_1_n_5 ,\ip_exec_count_reg[48]_i_1_n_6 ,\ip_exec_count_reg[48]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\ip_exec_count_reg[48]_i_1_n_8 ,\ip_exec_count_reg[48]_i_1_n_9 ,\ip_exec_count_reg[48]_i_1_n_10 ,\ip_exec_count_reg[48]_i_1_n_11 ,\ip_exec_count_reg[48]_i_1_n_12 ,\ip_exec_count_reg[48]_i_1_n_13 ,\ip_exec_count_reg[48]_i_1_n_14 ,\ip_exec_count_reg[48]_i_1_n_15 }),
        .S(data5[23:16]));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[49] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[48]_i_1_n_14 ),
        .Q(data5[17]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[4] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[0]_i_2_n_11 ),
        .Q(\ip_exec_count_reg_n_0_[4] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[50] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[48]_i_1_n_13 ),
        .Q(data5[18]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[51] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[48]_i_1_n_12 ),
        .Q(data5[19]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[52] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[48]_i_1_n_11 ),
        .Q(data5[20]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[53] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[48]_i_1_n_10 ),
        .Q(data5[21]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[54] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[48]_i_1_n_9 ),
        .Q(data5[22]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[55] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[48]_i_1_n_8 ),
        .Q(data5[23]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[56] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[56]_i_1_n_15 ),
        .Q(data5[24]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_exec_count_reg[56]_i_1 
       (.CI(\ip_exec_count_reg[48]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\NLW_ip_exec_count_reg[56]_i_1_CO_UNCONNECTED [7],\ip_exec_count_reg[56]_i_1_n_1 ,\ip_exec_count_reg[56]_i_1_n_2 ,\ip_exec_count_reg[56]_i_1_n_3 ,\ip_exec_count_reg[56]_i_1_n_4 ,\ip_exec_count_reg[56]_i_1_n_5 ,\ip_exec_count_reg[56]_i_1_n_6 ,\ip_exec_count_reg[56]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\ip_exec_count_reg[56]_i_1_n_8 ,\ip_exec_count_reg[56]_i_1_n_9 ,\ip_exec_count_reg[56]_i_1_n_10 ,\ip_exec_count_reg[56]_i_1_n_11 ,\ip_exec_count_reg[56]_i_1_n_12 ,\ip_exec_count_reg[56]_i_1_n_13 ,\ip_exec_count_reg[56]_i_1_n_14 ,\ip_exec_count_reg[56]_i_1_n_15 }),
        .S(data5[31:24]));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[57] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[56]_i_1_n_14 ),
        .Q(data5[25]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[58] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[56]_i_1_n_13 ),
        .Q(data5[26]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[59] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[56]_i_1_n_12 ),
        .Q(data5[27]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[5] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[0]_i_2_n_10 ),
        .Q(\ip_exec_count_reg_n_0_[5] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[60] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[56]_i_1_n_11 ),
        .Q(data5[28]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[61] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[56]_i_1_n_10 ),
        .Q(data5[29]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[62] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[56]_i_1_n_9 ),
        .Q(data5[30]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[63] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[56]_i_1_n_8 ),
        .Q(data5[31]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[6] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[0]_i_2_n_9 ),
        .Q(\ip_exec_count_reg_n_0_[6] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[7] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[0]_i_2_n_8 ),
        .Q(\ip_exec_count_reg_n_0_[7] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[8] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[8]_i_1_n_15 ),
        .Q(\ip_exec_count_reg_n_0_[8] ),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_exec_count_reg[8]_i_1 
       (.CI(\ip_exec_count_reg[0]_i_2_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ip_exec_count_reg[8]_i_1_n_0 ,\ip_exec_count_reg[8]_i_1_n_1 ,\ip_exec_count_reg[8]_i_1_n_2 ,\ip_exec_count_reg[8]_i_1_n_3 ,\ip_exec_count_reg[8]_i_1_n_4 ,\ip_exec_count_reg[8]_i_1_n_5 ,\ip_exec_count_reg[8]_i_1_n_6 ,\ip_exec_count_reg[8]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\ip_exec_count_reg[8]_i_1_n_8 ,\ip_exec_count_reg[8]_i_1_n_9 ,\ip_exec_count_reg[8]_i_1_n_10 ,\ip_exec_count_reg[8]_i_1_n_11 ,\ip_exec_count_reg[8]_i_1_n_12 ,\ip_exec_count_reg[8]_i_1_n_13 ,\ip_exec_count_reg[8]_i_1_n_14 ,\ip_exec_count_reg[8]_i_1_n_15 }),
        .S({\ip_exec_count_reg_n_0_[15] ,\ip_exec_count_reg_n_0_[14] ,\ip_exec_count_reg_n_0_[13] ,\ip_exec_count_reg_n_0_[12] ,\ip_exec_count_reg_n_0_[11] ,\ip_exec_count_reg_n_0_[10] ,\ip_exec_count_reg_n_0_[9] ,\ip_exec_count_reg_n_0_[8] }));
  FDRE #(
    .INIT(1'b0)) 
    \ip_exec_count_reg[9] 
       (.C(mon_clk),
        .CE(ip_exec_count0),
        .D(\ip_exec_count_reg[8]_i_1_n_14 ),
        .Q(\ip_exec_count_reg_n_0_[9] ),
        .R(RST_ACTIVE));
  CARRY8 ip_idle_carry
       (.CI(1'b1),
        .CI_TOP(1'b0),
        .CO({ip_idle_carry_n_0,ip_idle_carry_n_1,ip_idle_carry_n_2,ip_idle_carry_n_3,ip_idle_carry_n_4,ip_idle_carry_n_5,ip_idle_carry_n_6,ip_idle_carry_n_7}),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O(NLW_ip_idle_carry_O_UNCONNECTED[7:0]),
        .S({ip_idle_carry_i_1_n_0,ip_idle_carry_i_2_n_0,ip_idle_carry_i_3_n_0,ip_idle_carry_i_4_n_0,ip_idle_carry_i_5_n_0,ip_idle_carry_i_6_n_0,ip_idle_carry_i_7_n_0,ip_idle_carry_i_8_n_0}));
  CARRY8 ip_idle_carry__0
       (.CI(ip_idle_carry_n_0),
        .CI_TOP(1'b0),
        .CO({ip_idle_carry__0_n_0,ip_idle_carry__0_n_1,ip_idle_carry__0_n_2,ip_idle_carry__0_n_3,ip_idle_carry__0_n_4,ip_idle_carry__0_n_5,ip_idle_carry__0_n_6,ip_idle_carry__0_n_7}),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O(NLW_ip_idle_carry__0_O_UNCONNECTED[7:0]),
        .S({ip_idle_carry__0_i_1_n_0,ip_idle_carry__0_i_2_n_0,ip_idle_carry__0_i_3_n_0,ip_idle_carry__0_i_4_n_0,ip_idle_carry__0_i_5_n_0,ip_idle_carry__0_i_6_n_0,ip_idle_carry__0_i_7_n_0,ip_idle_carry__0_i_8_n_0}));
  LUT3 #(
    .INIT(8'h01)) 
    ip_idle_carry__0_i_1
       (.I0(ip_cur_tranx_reg[47]),
        .I1(ip_cur_tranx_reg[46]),
        .I2(ip_cur_tranx_reg[45]),
        .O(ip_idle_carry__0_i_1_n_0));
  LUT3 #(
    .INIT(8'h01)) 
    ip_idle_carry__0_i_2
       (.I0(ip_cur_tranx_reg[44]),
        .I1(ip_cur_tranx_reg[43]),
        .I2(ip_cur_tranx_reg[42]),
        .O(ip_idle_carry__0_i_2_n_0));
  LUT3 #(
    .INIT(8'h01)) 
    ip_idle_carry__0_i_3
       (.I0(ip_cur_tranx_reg[41]),
        .I1(ip_cur_tranx_reg[40]),
        .I2(ip_cur_tranx_reg[39]),
        .O(ip_idle_carry__0_i_3_n_0));
  LUT3 #(
    .INIT(8'h01)) 
    ip_idle_carry__0_i_4
       (.I0(ip_cur_tranx_reg[38]),
        .I1(ip_cur_tranx_reg[37]),
        .I2(ip_cur_tranx_reg[36]),
        .O(ip_idle_carry__0_i_4_n_0));
  LUT3 #(
    .INIT(8'h01)) 
    ip_idle_carry__0_i_5
       (.I0(ip_cur_tranx_reg[35]),
        .I1(ip_cur_tranx_reg[34]),
        .I2(ip_cur_tranx_reg[33]),
        .O(ip_idle_carry__0_i_5_n_0));
  LUT3 #(
    .INIT(8'h01)) 
    ip_idle_carry__0_i_6
       (.I0(ip_cur_tranx_reg[32]),
        .I1(ip_cur_tranx_reg[31]),
        .I2(ip_cur_tranx_reg[30]),
        .O(ip_idle_carry__0_i_6_n_0));
  LUT3 #(
    .INIT(8'h01)) 
    ip_idle_carry__0_i_7
       (.I0(ip_cur_tranx_reg[29]),
        .I1(ip_cur_tranx_reg[28]),
        .I2(ip_cur_tranx_reg[27]),
        .O(ip_idle_carry__0_i_7_n_0));
  LUT3 #(
    .INIT(8'h01)) 
    ip_idle_carry__0_i_8
       (.I0(ip_cur_tranx_reg[26]),
        .I1(ip_cur_tranx_reg[25]),
        .I2(ip_cur_tranx_reg[24]),
        .O(ip_idle_carry__0_i_8_n_0));
  CARRY8 ip_idle_carry__1
       (.CI(ip_idle_carry__0_n_0),
        .CI_TOP(1'b0),
        .CO({NLW_ip_idle_carry__1_CO_UNCONNECTED[7:6],CO,ip_idle_carry__1_n_3,ip_idle_carry__1_n_4,ip_idle_carry__1_n_5,ip_idle_carry__1_n_6,ip_idle_carry__1_n_7}),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O(NLW_ip_idle_carry__1_O_UNCONNECTED[7:0]),
        .S({1'b0,1'b0,ip_idle_carry__1_i_1_n_0,ip_idle_carry__1_i_2_n_0,ip_idle_carry__1_i_3_n_0,ip_idle_carry__1_i_4_n_0,ip_idle_carry__1_i_5_n_0,ip_idle_carry__1_i_6_n_0}));
  LUT1 #(
    .INIT(2'h1)) 
    ip_idle_carry__1_i_1
       (.I0(ip_cur_tranx_reg[63]),
        .O(ip_idle_carry__1_i_1_n_0));
  LUT3 #(
    .INIT(8'h01)) 
    ip_idle_carry__1_i_2
       (.I0(ip_cur_tranx_reg[62]),
        .I1(ip_cur_tranx_reg[61]),
        .I2(ip_cur_tranx_reg[60]),
        .O(ip_idle_carry__1_i_2_n_0));
  LUT3 #(
    .INIT(8'h01)) 
    ip_idle_carry__1_i_3
       (.I0(ip_cur_tranx_reg[59]),
        .I1(ip_cur_tranx_reg[58]),
        .I2(ip_cur_tranx_reg[57]),
        .O(ip_idle_carry__1_i_3_n_0));
  LUT3 #(
    .INIT(8'h01)) 
    ip_idle_carry__1_i_4
       (.I0(ip_cur_tranx_reg[56]),
        .I1(ip_cur_tranx_reg[55]),
        .I2(ip_cur_tranx_reg[54]),
        .O(ip_idle_carry__1_i_4_n_0));
  LUT3 #(
    .INIT(8'h01)) 
    ip_idle_carry__1_i_5
       (.I0(ip_cur_tranx_reg[53]),
        .I1(ip_cur_tranx_reg[52]),
        .I2(ip_cur_tranx_reg[51]),
        .O(ip_idle_carry__1_i_5_n_0));
  LUT3 #(
    .INIT(8'h01)) 
    ip_idle_carry__1_i_6
       (.I0(ip_cur_tranx_reg[50]),
        .I1(ip_cur_tranx_reg[49]),
        .I2(ip_cur_tranx_reg[48]),
        .O(ip_idle_carry__1_i_6_n_0));
  LUT3 #(
    .INIT(8'h01)) 
    ip_idle_carry_i_1
       (.I0(ip_cur_tranx_reg[23]),
        .I1(ip_cur_tranx_reg[22]),
        .I2(ip_cur_tranx_reg[21]),
        .O(ip_idle_carry_i_1_n_0));
  LUT3 #(
    .INIT(8'h01)) 
    ip_idle_carry_i_2
       (.I0(ip_cur_tranx_reg[20]),
        .I1(ip_cur_tranx_reg[19]),
        .I2(ip_cur_tranx_reg[18]),
        .O(ip_idle_carry_i_2_n_0));
  LUT3 #(
    .INIT(8'h01)) 
    ip_idle_carry_i_3
       (.I0(ip_cur_tranx_reg[17]),
        .I1(ip_cur_tranx_reg[16]),
        .I2(ip_cur_tranx_reg[15]),
        .O(ip_idle_carry_i_3_n_0));
  LUT3 #(
    .INIT(8'h01)) 
    ip_idle_carry_i_4
       (.I0(ip_cur_tranx_reg[14]),
        .I1(ip_cur_tranx_reg[13]),
        .I2(ip_cur_tranx_reg[12]),
        .O(ip_idle_carry_i_4_n_0));
  LUT3 #(
    .INIT(8'h01)) 
    ip_idle_carry_i_5
       (.I0(ip_cur_tranx_reg[11]),
        .I1(ip_cur_tranx_reg[10]),
        .I2(ip_cur_tranx_reg[9]),
        .O(ip_idle_carry_i_5_n_0));
  LUT3 #(
    .INIT(8'h01)) 
    ip_idle_carry_i_6
       (.I0(ip_cur_tranx_reg[8]),
        .I1(ip_cur_tranx_reg[7]),
        .I2(ip_cur_tranx_reg[6]),
        .O(ip_idle_carry_i_6_n_0));
  LUT3 #(
    .INIT(8'h01)) 
    ip_idle_carry_i_7
       (.I0(ip_cur_tranx_reg[5]),
        .I1(ip_cur_tranx_reg[4]),
        .I2(ip_cur_tranx_reg[3]),
        .O(ip_idle_carry_i_7_n_0));
  LUT3 #(
    .INIT(8'h01)) 
    ip_idle_carry_i_8
       (.I0(ip_cur_tranx_reg[2]),
        .I1(ip_cur_tranx_reg[1]),
        .I2(ip_cur_tranx_reg[0]),
        .O(ip_idle_carry_i_8_n_0));
  (* COMPARATOR_THRESHOLD = "11" *) 
  CARRY8 ip_max_parallel_tranx1_carry
       (.CI(1'b0),
        .CI_TOP(1'b0),
        .CO({ip_max_parallel_tranx1_carry_n_0,ip_max_parallel_tranx1_carry_n_1,ip_max_parallel_tranx1_carry_n_2,ip_max_parallel_tranx1_carry_n_3,ip_max_parallel_tranx1_carry_n_4,ip_max_parallel_tranx1_carry_n_5,ip_max_parallel_tranx1_carry_n_6,ip_max_parallel_tranx1_carry_n_7}),
        .DI({ip_max_parallel_tranx1_carry_i_1_n_0,ip_max_parallel_tranx1_carry_i_2_n_0,ip_max_parallel_tranx1_carry_i_3_n_0,ip_max_parallel_tranx1_carry_i_4_n_0,ip_max_parallel_tranx1_carry_i_5_n_0,ip_max_parallel_tranx1_carry_i_6_n_0,ip_max_parallel_tranx1_carry_i_7_n_0,ip_max_parallel_tranx1_carry_i_8_n_0}),
        .O(NLW_ip_max_parallel_tranx1_carry_O_UNCONNECTED[7:0]),
        .S({ip_max_parallel_tranx1_carry_i_9_n_0,ip_max_parallel_tranx1_carry_i_10_n_0,ip_max_parallel_tranx1_carry_i_11_n_0,ip_max_parallel_tranx1_carry_i_12_n_0,ip_max_parallel_tranx1_carry_i_13_n_0,ip_max_parallel_tranx1_carry_i_14_n_0,ip_max_parallel_tranx1_carry_i_15_n_0,ip_max_parallel_tranx1_carry_i_16_n_0}));
  (* COMPARATOR_THRESHOLD = "11" *) 
  CARRY8 ip_max_parallel_tranx1_carry__0
       (.CI(ip_max_parallel_tranx1_carry_n_0),
        .CI_TOP(1'b0),
        .CO({ip_max_parallel_tranx1_carry__0_n_0,ip_max_parallel_tranx1_carry__0_n_1,ip_max_parallel_tranx1_carry__0_n_2,ip_max_parallel_tranx1_carry__0_n_3,ip_max_parallel_tranx1_carry__0_n_4,ip_max_parallel_tranx1_carry__0_n_5,ip_max_parallel_tranx1_carry__0_n_6,ip_max_parallel_tranx1_carry__0_n_7}),
        .DI({ip_max_parallel_tranx1_carry__0_i_1_n_0,ip_max_parallel_tranx1_carry__0_i_2_n_0,ip_max_parallel_tranx1_carry__0_i_3_n_0,ip_max_parallel_tranx1_carry__0_i_4_n_0,ip_max_parallel_tranx1_carry__0_i_5_n_0,ip_max_parallel_tranx1_carry__0_i_6_n_0,ip_max_parallel_tranx1_carry__0_i_7_n_0,ip_max_parallel_tranx1_carry__0_i_8_n_0}),
        .O(NLW_ip_max_parallel_tranx1_carry__0_O_UNCONNECTED[7:0]),
        .S({ip_max_parallel_tranx1_carry__0_i_9_n_0,ip_max_parallel_tranx1_carry__0_i_10_n_0,ip_max_parallel_tranx1_carry__0_i_11_n_0,ip_max_parallel_tranx1_carry__0_i_12_n_0,ip_max_parallel_tranx1_carry__0_i_13_n_0,ip_max_parallel_tranx1_carry__0_i_14_n_0,ip_max_parallel_tranx1_carry__0_i_15_n_0,ip_max_parallel_tranx1_carry__0_i_16_n_0}));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry__0_i_1
       (.I0(ip_cur_tranx_reg[30]),
        .I1(\ip_max_parallel_tranx_reg_n_0_[30] ),
        .I2(\ip_max_parallel_tranx_reg_n_0_[31] ),
        .I3(ip_cur_tranx_reg[31]),
        .O(ip_max_parallel_tranx1_carry__0_i_1_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry__0_i_10
       (.I0(\ip_max_parallel_tranx_reg_n_0_[29] ),
        .I1(ip_cur_tranx_reg[29]),
        .I2(\ip_max_parallel_tranx_reg_n_0_[28] ),
        .I3(ip_cur_tranx_reg[28]),
        .O(ip_max_parallel_tranx1_carry__0_i_10_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry__0_i_11
       (.I0(\ip_max_parallel_tranx_reg_n_0_[27] ),
        .I1(ip_cur_tranx_reg[27]),
        .I2(\ip_max_parallel_tranx_reg_n_0_[26] ),
        .I3(ip_cur_tranx_reg[26]),
        .O(ip_max_parallel_tranx1_carry__0_i_11_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry__0_i_12
       (.I0(\ip_max_parallel_tranx_reg_n_0_[25] ),
        .I1(ip_cur_tranx_reg[25]),
        .I2(\ip_max_parallel_tranx_reg_n_0_[24] ),
        .I3(ip_cur_tranx_reg[24]),
        .O(ip_max_parallel_tranx1_carry__0_i_12_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry__0_i_13
       (.I0(\ip_max_parallel_tranx_reg_n_0_[23] ),
        .I1(ip_cur_tranx_reg[23]),
        .I2(\ip_max_parallel_tranx_reg_n_0_[22] ),
        .I3(ip_cur_tranx_reg[22]),
        .O(ip_max_parallel_tranx1_carry__0_i_13_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry__0_i_14
       (.I0(\ip_max_parallel_tranx_reg_n_0_[21] ),
        .I1(ip_cur_tranx_reg[21]),
        .I2(\ip_max_parallel_tranx_reg_n_0_[20] ),
        .I3(ip_cur_tranx_reg[20]),
        .O(ip_max_parallel_tranx1_carry__0_i_14_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry__0_i_15
       (.I0(\ip_max_parallel_tranx_reg_n_0_[19] ),
        .I1(ip_cur_tranx_reg[19]),
        .I2(\ip_max_parallel_tranx_reg_n_0_[18] ),
        .I3(ip_cur_tranx_reg[18]),
        .O(ip_max_parallel_tranx1_carry__0_i_15_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry__0_i_16
       (.I0(\ip_max_parallel_tranx_reg_n_0_[17] ),
        .I1(ip_cur_tranx_reg[17]),
        .I2(\ip_max_parallel_tranx_reg_n_0_[16] ),
        .I3(ip_cur_tranx_reg[16]),
        .O(ip_max_parallel_tranx1_carry__0_i_16_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry__0_i_2
       (.I0(ip_cur_tranx_reg[28]),
        .I1(\ip_max_parallel_tranx_reg_n_0_[28] ),
        .I2(\ip_max_parallel_tranx_reg_n_0_[29] ),
        .I3(ip_cur_tranx_reg[29]),
        .O(ip_max_parallel_tranx1_carry__0_i_2_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry__0_i_3
       (.I0(ip_cur_tranx_reg[26]),
        .I1(\ip_max_parallel_tranx_reg_n_0_[26] ),
        .I2(\ip_max_parallel_tranx_reg_n_0_[27] ),
        .I3(ip_cur_tranx_reg[27]),
        .O(ip_max_parallel_tranx1_carry__0_i_3_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry__0_i_4
       (.I0(ip_cur_tranx_reg[24]),
        .I1(\ip_max_parallel_tranx_reg_n_0_[24] ),
        .I2(\ip_max_parallel_tranx_reg_n_0_[25] ),
        .I3(ip_cur_tranx_reg[25]),
        .O(ip_max_parallel_tranx1_carry__0_i_4_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry__0_i_5
       (.I0(ip_cur_tranx_reg[22]),
        .I1(\ip_max_parallel_tranx_reg_n_0_[22] ),
        .I2(\ip_max_parallel_tranx_reg_n_0_[23] ),
        .I3(ip_cur_tranx_reg[23]),
        .O(ip_max_parallel_tranx1_carry__0_i_5_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry__0_i_6
       (.I0(ip_cur_tranx_reg[20]),
        .I1(\ip_max_parallel_tranx_reg_n_0_[20] ),
        .I2(\ip_max_parallel_tranx_reg_n_0_[21] ),
        .I3(ip_cur_tranx_reg[21]),
        .O(ip_max_parallel_tranx1_carry__0_i_6_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry__0_i_7
       (.I0(ip_cur_tranx_reg[18]),
        .I1(\ip_max_parallel_tranx_reg_n_0_[18] ),
        .I2(\ip_max_parallel_tranx_reg_n_0_[19] ),
        .I3(ip_cur_tranx_reg[19]),
        .O(ip_max_parallel_tranx1_carry__0_i_7_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry__0_i_8
       (.I0(ip_cur_tranx_reg[16]),
        .I1(\ip_max_parallel_tranx_reg_n_0_[16] ),
        .I2(\ip_max_parallel_tranx_reg_n_0_[17] ),
        .I3(ip_cur_tranx_reg[17]),
        .O(ip_max_parallel_tranx1_carry__0_i_8_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry__0_i_9
       (.I0(\ip_max_parallel_tranx_reg_n_0_[31] ),
        .I1(ip_cur_tranx_reg[31]),
        .I2(\ip_max_parallel_tranx_reg_n_0_[30] ),
        .I3(ip_cur_tranx_reg[30]),
        .O(ip_max_parallel_tranx1_carry__0_i_9_n_0));
  (* COMPARATOR_THRESHOLD = "11" *) 
  CARRY8 ip_max_parallel_tranx1_carry__1
       (.CI(ip_max_parallel_tranx1_carry__0_n_0),
        .CI_TOP(1'b0),
        .CO({ip_max_parallel_tranx1_carry__1_n_0,ip_max_parallel_tranx1_carry__1_n_1,ip_max_parallel_tranx1_carry__1_n_2,ip_max_parallel_tranx1_carry__1_n_3,ip_max_parallel_tranx1_carry__1_n_4,ip_max_parallel_tranx1_carry__1_n_5,ip_max_parallel_tranx1_carry__1_n_6,ip_max_parallel_tranx1_carry__1_n_7}),
        .DI({ip_max_parallel_tranx1_carry__1_i_1_n_0,ip_max_parallel_tranx1_carry__1_i_2_n_0,ip_max_parallel_tranx1_carry__1_i_3_n_0,ip_max_parallel_tranx1_carry__1_i_4_n_0,ip_max_parallel_tranx1_carry__1_i_5_n_0,ip_max_parallel_tranx1_carry__1_i_6_n_0,ip_max_parallel_tranx1_carry__1_i_7_n_0,ip_max_parallel_tranx1_carry__1_i_8_n_0}),
        .O(NLW_ip_max_parallel_tranx1_carry__1_O_UNCONNECTED[7:0]),
        .S({ip_max_parallel_tranx1_carry__1_i_9_n_0,ip_max_parallel_tranx1_carry__1_i_10_n_0,ip_max_parallel_tranx1_carry__1_i_11_n_0,ip_max_parallel_tranx1_carry__1_i_12_n_0,ip_max_parallel_tranx1_carry__1_i_13_n_0,ip_max_parallel_tranx1_carry__1_i_14_n_0,ip_max_parallel_tranx1_carry__1_i_15_n_0,ip_max_parallel_tranx1_carry__1_i_16_n_0}));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry__1_i_1
       (.I0(ip_cur_tranx_reg[46]),
        .I1(data13[14]),
        .I2(data13[15]),
        .I3(ip_cur_tranx_reg[47]),
        .O(ip_max_parallel_tranx1_carry__1_i_1_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry__1_i_10
       (.I0(data13[13]),
        .I1(ip_cur_tranx_reg[45]),
        .I2(data13[12]),
        .I3(ip_cur_tranx_reg[44]),
        .O(ip_max_parallel_tranx1_carry__1_i_10_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry__1_i_11
       (.I0(data13[11]),
        .I1(ip_cur_tranx_reg[43]),
        .I2(data13[10]),
        .I3(ip_cur_tranx_reg[42]),
        .O(ip_max_parallel_tranx1_carry__1_i_11_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry__1_i_12
       (.I0(data13[9]),
        .I1(ip_cur_tranx_reg[41]),
        .I2(data13[8]),
        .I3(ip_cur_tranx_reg[40]),
        .O(ip_max_parallel_tranx1_carry__1_i_12_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry__1_i_13
       (.I0(data13[7]),
        .I1(ip_cur_tranx_reg[39]),
        .I2(data13[6]),
        .I3(ip_cur_tranx_reg[38]),
        .O(ip_max_parallel_tranx1_carry__1_i_13_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry__1_i_14
       (.I0(data13[5]),
        .I1(ip_cur_tranx_reg[37]),
        .I2(data13[4]),
        .I3(ip_cur_tranx_reg[36]),
        .O(ip_max_parallel_tranx1_carry__1_i_14_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry__1_i_15
       (.I0(data13[3]),
        .I1(ip_cur_tranx_reg[35]),
        .I2(data13[2]),
        .I3(ip_cur_tranx_reg[34]),
        .O(ip_max_parallel_tranx1_carry__1_i_15_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry__1_i_16
       (.I0(data13[1]),
        .I1(ip_cur_tranx_reg[33]),
        .I2(data13[0]),
        .I3(ip_cur_tranx_reg[32]),
        .O(ip_max_parallel_tranx1_carry__1_i_16_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry__1_i_2
       (.I0(ip_cur_tranx_reg[44]),
        .I1(data13[12]),
        .I2(data13[13]),
        .I3(ip_cur_tranx_reg[45]),
        .O(ip_max_parallel_tranx1_carry__1_i_2_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry__1_i_3
       (.I0(ip_cur_tranx_reg[42]),
        .I1(data13[10]),
        .I2(data13[11]),
        .I3(ip_cur_tranx_reg[43]),
        .O(ip_max_parallel_tranx1_carry__1_i_3_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry__1_i_4
       (.I0(ip_cur_tranx_reg[40]),
        .I1(data13[8]),
        .I2(data13[9]),
        .I3(ip_cur_tranx_reg[41]),
        .O(ip_max_parallel_tranx1_carry__1_i_4_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry__1_i_5
       (.I0(ip_cur_tranx_reg[38]),
        .I1(data13[6]),
        .I2(data13[7]),
        .I3(ip_cur_tranx_reg[39]),
        .O(ip_max_parallel_tranx1_carry__1_i_5_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry__1_i_6
       (.I0(ip_cur_tranx_reg[36]),
        .I1(data13[4]),
        .I2(data13[5]),
        .I3(ip_cur_tranx_reg[37]),
        .O(ip_max_parallel_tranx1_carry__1_i_6_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry__1_i_7
       (.I0(ip_cur_tranx_reg[34]),
        .I1(data13[2]),
        .I2(data13[3]),
        .I3(ip_cur_tranx_reg[35]),
        .O(ip_max_parallel_tranx1_carry__1_i_7_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry__1_i_8
       (.I0(ip_cur_tranx_reg[32]),
        .I1(data13[0]),
        .I2(data13[1]),
        .I3(ip_cur_tranx_reg[33]),
        .O(ip_max_parallel_tranx1_carry__1_i_8_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry__1_i_9
       (.I0(data13[15]),
        .I1(ip_cur_tranx_reg[47]),
        .I2(data13[14]),
        .I3(ip_cur_tranx_reg[46]),
        .O(ip_max_parallel_tranx1_carry__1_i_9_n_0));
  (* COMPARATOR_THRESHOLD = "11" *) 
  CARRY8 ip_max_parallel_tranx1_carry__2
       (.CI(ip_max_parallel_tranx1_carry__1_n_0),
        .CI_TOP(1'b0),
        .CO({ip_max_parallel_tranx1_carry__2_n_0,ip_max_parallel_tranx1_carry__2_n_1,ip_max_parallel_tranx1_carry__2_n_2,ip_max_parallel_tranx1_carry__2_n_3,ip_max_parallel_tranx1_carry__2_n_4,ip_max_parallel_tranx1_carry__2_n_5,ip_max_parallel_tranx1_carry__2_n_6,ip_max_parallel_tranx1_carry__2_n_7}),
        .DI({ip_max_parallel_tranx1_carry__2_i_1_n_0,ip_max_parallel_tranx1_carry__2_i_2_n_0,ip_max_parallel_tranx1_carry__2_i_3_n_0,ip_max_parallel_tranx1_carry__2_i_4_n_0,ip_max_parallel_tranx1_carry__2_i_5_n_0,ip_max_parallel_tranx1_carry__2_i_6_n_0,ip_max_parallel_tranx1_carry__2_i_7_n_0,ip_max_parallel_tranx1_carry__2_i_8_n_0}),
        .O(NLW_ip_max_parallel_tranx1_carry__2_O_UNCONNECTED[7:0]),
        .S({ip_max_parallel_tranx1_carry__2_i_9_n_0,ip_max_parallel_tranx1_carry__2_i_10_n_0,ip_max_parallel_tranx1_carry__2_i_11_n_0,ip_max_parallel_tranx1_carry__2_i_12_n_0,ip_max_parallel_tranx1_carry__2_i_13_n_0,ip_max_parallel_tranx1_carry__2_i_14_n_0,ip_max_parallel_tranx1_carry__2_i_15_n_0,ip_max_parallel_tranx1_carry__2_i_16_n_0}));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry__2_i_1
       (.I0(ip_cur_tranx_reg[62]),
        .I1(data13[30]),
        .I2(data13[31]),
        .I3(ip_cur_tranx_reg[63]),
        .O(ip_max_parallel_tranx1_carry__2_i_1_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry__2_i_10
       (.I0(data13[29]),
        .I1(ip_cur_tranx_reg[61]),
        .I2(data13[28]),
        .I3(ip_cur_tranx_reg[60]),
        .O(ip_max_parallel_tranx1_carry__2_i_10_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry__2_i_11
       (.I0(data13[27]),
        .I1(ip_cur_tranx_reg[59]),
        .I2(data13[26]),
        .I3(ip_cur_tranx_reg[58]),
        .O(ip_max_parallel_tranx1_carry__2_i_11_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry__2_i_12
       (.I0(data13[25]),
        .I1(ip_cur_tranx_reg[57]),
        .I2(data13[24]),
        .I3(ip_cur_tranx_reg[56]),
        .O(ip_max_parallel_tranx1_carry__2_i_12_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry__2_i_13
       (.I0(data13[23]),
        .I1(ip_cur_tranx_reg[55]),
        .I2(data13[22]),
        .I3(ip_cur_tranx_reg[54]),
        .O(ip_max_parallel_tranx1_carry__2_i_13_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry__2_i_14
       (.I0(data13[21]),
        .I1(ip_cur_tranx_reg[53]),
        .I2(data13[20]),
        .I3(ip_cur_tranx_reg[52]),
        .O(ip_max_parallel_tranx1_carry__2_i_14_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry__2_i_15
       (.I0(data13[19]),
        .I1(ip_cur_tranx_reg[51]),
        .I2(data13[18]),
        .I3(ip_cur_tranx_reg[50]),
        .O(ip_max_parallel_tranx1_carry__2_i_15_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry__2_i_16
       (.I0(data13[17]),
        .I1(ip_cur_tranx_reg[49]),
        .I2(data13[16]),
        .I3(ip_cur_tranx_reg[48]),
        .O(ip_max_parallel_tranx1_carry__2_i_16_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry__2_i_2
       (.I0(ip_cur_tranx_reg[60]),
        .I1(data13[28]),
        .I2(data13[29]),
        .I3(ip_cur_tranx_reg[61]),
        .O(ip_max_parallel_tranx1_carry__2_i_2_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry__2_i_3
       (.I0(ip_cur_tranx_reg[58]),
        .I1(data13[26]),
        .I2(data13[27]),
        .I3(ip_cur_tranx_reg[59]),
        .O(ip_max_parallel_tranx1_carry__2_i_3_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry__2_i_4
       (.I0(ip_cur_tranx_reg[56]),
        .I1(data13[24]),
        .I2(data13[25]),
        .I3(ip_cur_tranx_reg[57]),
        .O(ip_max_parallel_tranx1_carry__2_i_4_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry__2_i_5
       (.I0(ip_cur_tranx_reg[54]),
        .I1(data13[22]),
        .I2(data13[23]),
        .I3(ip_cur_tranx_reg[55]),
        .O(ip_max_parallel_tranx1_carry__2_i_5_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry__2_i_6
       (.I0(ip_cur_tranx_reg[52]),
        .I1(data13[20]),
        .I2(data13[21]),
        .I3(ip_cur_tranx_reg[53]),
        .O(ip_max_parallel_tranx1_carry__2_i_6_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry__2_i_7
       (.I0(ip_cur_tranx_reg[50]),
        .I1(data13[18]),
        .I2(data13[19]),
        .I3(ip_cur_tranx_reg[51]),
        .O(ip_max_parallel_tranx1_carry__2_i_7_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry__2_i_8
       (.I0(ip_cur_tranx_reg[48]),
        .I1(data13[16]),
        .I2(data13[17]),
        .I3(ip_cur_tranx_reg[49]),
        .O(ip_max_parallel_tranx1_carry__2_i_8_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry__2_i_9
       (.I0(data13[31]),
        .I1(ip_cur_tranx_reg[63]),
        .I2(data13[30]),
        .I3(ip_cur_tranx_reg[62]),
        .O(ip_max_parallel_tranx1_carry__2_i_9_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry_i_1
       (.I0(ip_cur_tranx_reg[14]),
        .I1(\ip_max_parallel_tranx_reg_n_0_[14] ),
        .I2(\ip_max_parallel_tranx_reg_n_0_[15] ),
        .I3(ip_cur_tranx_reg[15]),
        .O(ip_max_parallel_tranx1_carry_i_1_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry_i_10
       (.I0(\ip_max_parallel_tranx_reg_n_0_[13] ),
        .I1(ip_cur_tranx_reg[13]),
        .I2(\ip_max_parallel_tranx_reg_n_0_[12] ),
        .I3(ip_cur_tranx_reg[12]),
        .O(ip_max_parallel_tranx1_carry_i_10_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry_i_11
       (.I0(\ip_max_parallel_tranx_reg_n_0_[11] ),
        .I1(ip_cur_tranx_reg[11]),
        .I2(\ip_max_parallel_tranx_reg_n_0_[10] ),
        .I3(ip_cur_tranx_reg[10]),
        .O(ip_max_parallel_tranx1_carry_i_11_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry_i_12
       (.I0(\ip_max_parallel_tranx_reg_n_0_[9] ),
        .I1(ip_cur_tranx_reg[9]),
        .I2(\ip_max_parallel_tranx_reg_n_0_[8] ),
        .I3(ip_cur_tranx_reg[8]),
        .O(ip_max_parallel_tranx1_carry_i_12_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry_i_13
       (.I0(\ip_max_parallel_tranx_reg_n_0_[7] ),
        .I1(ip_cur_tranx_reg[7]),
        .I2(\ip_max_parallel_tranx_reg_n_0_[6] ),
        .I3(ip_cur_tranx_reg[6]),
        .O(ip_max_parallel_tranx1_carry_i_13_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry_i_14
       (.I0(\ip_max_parallel_tranx_reg_n_0_[5] ),
        .I1(ip_cur_tranx_reg[5]),
        .I2(\ip_max_parallel_tranx_reg_n_0_[4] ),
        .I3(ip_cur_tranx_reg[4]),
        .O(ip_max_parallel_tranx1_carry_i_14_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry_i_15
       (.I0(\ip_max_parallel_tranx_reg_n_0_[3] ),
        .I1(ip_cur_tranx_reg[3]),
        .I2(\ip_max_parallel_tranx_reg_n_0_[2] ),
        .I3(ip_cur_tranx_reg[2]),
        .O(ip_max_parallel_tranx1_carry_i_15_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry_i_16
       (.I0(\ip_max_parallel_tranx_reg_n_0_[1] ),
        .I1(ip_cur_tranx_reg[1]),
        .I2(\ip_max_parallel_tranx_reg_n_0_[0] ),
        .I3(ip_cur_tranx_reg[0]),
        .O(ip_max_parallel_tranx1_carry_i_16_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry_i_2
       (.I0(ip_cur_tranx_reg[12]),
        .I1(\ip_max_parallel_tranx_reg_n_0_[12] ),
        .I2(\ip_max_parallel_tranx_reg_n_0_[13] ),
        .I3(ip_cur_tranx_reg[13]),
        .O(ip_max_parallel_tranx1_carry_i_2_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry_i_3
       (.I0(ip_cur_tranx_reg[10]),
        .I1(\ip_max_parallel_tranx_reg_n_0_[10] ),
        .I2(\ip_max_parallel_tranx_reg_n_0_[11] ),
        .I3(ip_cur_tranx_reg[11]),
        .O(ip_max_parallel_tranx1_carry_i_3_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry_i_4
       (.I0(ip_cur_tranx_reg[8]),
        .I1(\ip_max_parallel_tranx_reg_n_0_[8] ),
        .I2(\ip_max_parallel_tranx_reg_n_0_[9] ),
        .I3(ip_cur_tranx_reg[9]),
        .O(ip_max_parallel_tranx1_carry_i_4_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry_i_5
       (.I0(ip_cur_tranx_reg[6]),
        .I1(\ip_max_parallel_tranx_reg_n_0_[6] ),
        .I2(\ip_max_parallel_tranx_reg_n_0_[7] ),
        .I3(ip_cur_tranx_reg[7]),
        .O(ip_max_parallel_tranx1_carry_i_5_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry_i_6
       (.I0(ip_cur_tranx_reg[4]),
        .I1(\ip_max_parallel_tranx_reg_n_0_[4] ),
        .I2(\ip_max_parallel_tranx_reg_n_0_[5] ),
        .I3(ip_cur_tranx_reg[5]),
        .O(ip_max_parallel_tranx1_carry_i_6_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry_i_7
       (.I0(ip_cur_tranx_reg[2]),
        .I1(\ip_max_parallel_tranx_reg_n_0_[2] ),
        .I2(\ip_max_parallel_tranx_reg_n_0_[3] ),
        .I3(ip_cur_tranx_reg[3]),
        .O(ip_max_parallel_tranx1_carry_i_7_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    ip_max_parallel_tranx1_carry_i_8
       (.I0(ip_cur_tranx_reg[0]),
        .I1(\ip_max_parallel_tranx_reg_n_0_[0] ),
        .I2(\ip_max_parallel_tranx_reg_n_0_[1] ),
        .I3(ip_cur_tranx_reg[1]),
        .O(ip_max_parallel_tranx1_carry_i_8_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    ip_max_parallel_tranx1_carry_i_9
       (.I0(\ip_max_parallel_tranx_reg_n_0_[15] ),
        .I1(ip_cur_tranx_reg[15]),
        .I2(\ip_max_parallel_tranx_reg_n_0_[14] ),
        .I3(ip_cur_tranx_reg[14]),
        .O(ip_max_parallel_tranx1_carry_i_9_n_0));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[0] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[0]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[0] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[10] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[10]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[10] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[11] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[11]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[11] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[12] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[12]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[12] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[13] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[13]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[13] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[14] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[14]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[14] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[15] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[15]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[15] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[16] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[16]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[16] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[17] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[17]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[17] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[18] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[18]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[18] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[19] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[19]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[19] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[1] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[1]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[1] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[20] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[20]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[20] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[21] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[21]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[21] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[22] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[22]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[22] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[23] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[23]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[23] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[24] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[24]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[24] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[25] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[25]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[25] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[26] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[26]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[26] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[27] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[27]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[27] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[28] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[28]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[28] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[29] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[29]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[29] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[2] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[2]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[2] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[30] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[30]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[30] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[31] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[31]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[31] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[32] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[32]),
        .Q(data13[0]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[33] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[33]),
        .Q(data13[1]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[34] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[34]),
        .Q(data13[2]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[35] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[35]),
        .Q(data13[3]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[36] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[36]),
        .Q(data13[4]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[37] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[37]),
        .Q(data13[5]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[38] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[38]),
        .Q(data13[6]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[39] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[39]),
        .Q(data13[7]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[3] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[3]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[3] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[40] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[40]),
        .Q(data13[8]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[41] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[41]),
        .Q(data13[9]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[42] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[42]),
        .Q(data13[10]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[43] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[43]),
        .Q(data13[11]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[44] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[44]),
        .Q(data13[12]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[45] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[45]),
        .Q(data13[13]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[46] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[46]),
        .Q(data13[14]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[47] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[47]),
        .Q(data13[15]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[48] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[48]),
        .Q(data13[16]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[49] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[49]),
        .Q(data13[17]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[4] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[4]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[4] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[50] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[50]),
        .Q(data13[18]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[51] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[51]),
        .Q(data13[19]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[52] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[52]),
        .Q(data13[20]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[53] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[53]),
        .Q(data13[21]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[54] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[54]),
        .Q(data13[22]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[55] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[55]),
        .Q(data13[23]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[56] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[56]),
        .Q(data13[24]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[57] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[57]),
        .Q(data13[25]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[58] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[58]),
        .Q(data13[26]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[59] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[59]),
        .Q(data13[27]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[5] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[5]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[5] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[60] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[60]),
        .Q(data13[28]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[61] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[61]),
        .Q(data13[29]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[62] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[62]),
        .Q(data13[30]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[63] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[63]),
        .Q(data13[31]),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[6] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[6]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[6] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[7] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[7]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[7] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[8] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[8]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[8] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_max_parallel_tranx_reg[9] 
       (.C(mon_clk),
        .CE(ip_max_parallel_tranx1_carry__2_n_0),
        .D(ip_cur_tranx_reg[9]),
        .Q(\ip_max_parallel_tranx_reg_n_0_[9] ),
        .R(\ip_max_parallel_tranx_reg[0]_0 ));
  LUT1 #(
    .INIT(2'h1)) 
    \ip_start_count[0]_i_3 
       (.I0(\ip_start_count_reg_n_0_[0] ),
        .O(\ip_start_count[0]_i_3_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[0] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[0]_i_2_n_15 ),
        .Q(\ip_start_count_reg_n_0_[0] ),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_start_count_reg[0]_i_2 
       (.CI(1'b0),
        .CI_TOP(1'b0),
        .CO({\ip_start_count_reg[0]_i_2_n_0 ,\ip_start_count_reg[0]_i_2_n_1 ,\ip_start_count_reg[0]_i_2_n_2 ,\ip_start_count_reg[0]_i_2_n_3 ,\ip_start_count_reg[0]_i_2_n_4 ,\ip_start_count_reg[0]_i_2_n_5 ,\ip_start_count_reg[0]_i_2_n_6 ,\ip_start_count_reg[0]_i_2_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b1}),
        .O({\ip_start_count_reg[0]_i_2_n_8 ,\ip_start_count_reg[0]_i_2_n_9 ,\ip_start_count_reg[0]_i_2_n_10 ,\ip_start_count_reg[0]_i_2_n_11 ,\ip_start_count_reg[0]_i_2_n_12 ,\ip_start_count_reg[0]_i_2_n_13 ,\ip_start_count_reg[0]_i_2_n_14 ,\ip_start_count_reg[0]_i_2_n_15 }),
        .S({\ip_start_count_reg_n_0_[7] ,\ip_start_count_reg_n_0_[6] ,\ip_start_count_reg_n_0_[5] ,\ip_start_count_reg_n_0_[4] ,\ip_start_count_reg_n_0_[3] ,\ip_start_count_reg_n_0_[2] ,\ip_start_count_reg_n_0_[1] ,\ip_start_count[0]_i_3_n_0 }));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[10] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[8]_i_1_n_13 ),
        .Q(\ip_start_count_reg_n_0_[10] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[11] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[8]_i_1_n_12 ),
        .Q(\ip_start_count_reg_n_0_[11] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[12] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[8]_i_1_n_11 ),
        .Q(\ip_start_count_reg_n_0_[12] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[13] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[8]_i_1_n_10 ),
        .Q(\ip_start_count_reg_n_0_[13] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[14] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[8]_i_1_n_9 ),
        .Q(\ip_start_count_reg_n_0_[14] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[15] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[8]_i_1_n_8 ),
        .Q(\ip_start_count_reg_n_0_[15] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[16] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[16]_i_1_n_15 ),
        .Q(\ip_start_count_reg_n_0_[16] ),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_start_count_reg[16]_i_1 
       (.CI(\ip_start_count_reg[8]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ip_start_count_reg[16]_i_1_n_0 ,\ip_start_count_reg[16]_i_1_n_1 ,\ip_start_count_reg[16]_i_1_n_2 ,\ip_start_count_reg[16]_i_1_n_3 ,\ip_start_count_reg[16]_i_1_n_4 ,\ip_start_count_reg[16]_i_1_n_5 ,\ip_start_count_reg[16]_i_1_n_6 ,\ip_start_count_reg[16]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\ip_start_count_reg[16]_i_1_n_8 ,\ip_start_count_reg[16]_i_1_n_9 ,\ip_start_count_reg[16]_i_1_n_10 ,\ip_start_count_reg[16]_i_1_n_11 ,\ip_start_count_reg[16]_i_1_n_12 ,\ip_start_count_reg[16]_i_1_n_13 ,\ip_start_count_reg[16]_i_1_n_14 ,\ip_start_count_reg[16]_i_1_n_15 }),
        .S({\ip_start_count_reg_n_0_[23] ,\ip_start_count_reg_n_0_[22] ,\ip_start_count_reg_n_0_[21] ,\ip_start_count_reg_n_0_[20] ,\ip_start_count_reg_n_0_[19] ,\ip_start_count_reg_n_0_[18] ,\ip_start_count_reg_n_0_[17] ,\ip_start_count_reg_n_0_[16] }));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[17] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[16]_i_1_n_14 ),
        .Q(\ip_start_count_reg_n_0_[17] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[18] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[16]_i_1_n_13 ),
        .Q(\ip_start_count_reg_n_0_[18] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[19] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[16]_i_1_n_12 ),
        .Q(\ip_start_count_reg_n_0_[19] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[1] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[0]_i_2_n_14 ),
        .Q(\ip_start_count_reg_n_0_[1] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[20] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[16]_i_1_n_11 ),
        .Q(\ip_start_count_reg_n_0_[20] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[21] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[16]_i_1_n_10 ),
        .Q(\ip_start_count_reg_n_0_[21] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[22] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[16]_i_1_n_9 ),
        .Q(\ip_start_count_reg_n_0_[22] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[23] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[16]_i_1_n_8 ),
        .Q(\ip_start_count_reg_n_0_[23] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[24] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[24]_i_1_n_15 ),
        .Q(\ip_start_count_reg_n_0_[24] ),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_start_count_reg[24]_i_1 
       (.CI(\ip_start_count_reg[16]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ip_start_count_reg[24]_i_1_n_0 ,\ip_start_count_reg[24]_i_1_n_1 ,\ip_start_count_reg[24]_i_1_n_2 ,\ip_start_count_reg[24]_i_1_n_3 ,\ip_start_count_reg[24]_i_1_n_4 ,\ip_start_count_reg[24]_i_1_n_5 ,\ip_start_count_reg[24]_i_1_n_6 ,\ip_start_count_reg[24]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\ip_start_count_reg[24]_i_1_n_8 ,\ip_start_count_reg[24]_i_1_n_9 ,\ip_start_count_reg[24]_i_1_n_10 ,\ip_start_count_reg[24]_i_1_n_11 ,\ip_start_count_reg[24]_i_1_n_12 ,\ip_start_count_reg[24]_i_1_n_13 ,\ip_start_count_reg[24]_i_1_n_14 ,\ip_start_count_reg[24]_i_1_n_15 }),
        .S({\ip_start_count_reg_n_0_[31] ,\ip_start_count_reg_n_0_[30] ,\ip_start_count_reg_n_0_[29] ,\ip_start_count_reg_n_0_[28] ,\ip_start_count_reg_n_0_[27] ,\ip_start_count_reg_n_0_[26] ,\ip_start_count_reg_n_0_[25] ,\ip_start_count_reg_n_0_[24] }));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[25] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[24]_i_1_n_14 ),
        .Q(\ip_start_count_reg_n_0_[25] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[26] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[24]_i_1_n_13 ),
        .Q(\ip_start_count_reg_n_0_[26] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[27] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[24]_i_1_n_12 ),
        .Q(\ip_start_count_reg_n_0_[27] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[28] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[24]_i_1_n_11 ),
        .Q(\ip_start_count_reg_n_0_[28] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[29] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[24]_i_1_n_10 ),
        .Q(\ip_start_count_reg_n_0_[29] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[2] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[0]_i_2_n_13 ),
        .Q(\ip_start_count_reg_n_0_[2] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[30] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[24]_i_1_n_9 ),
        .Q(\ip_start_count_reg_n_0_[30] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[31] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[24]_i_1_n_8 ),
        .Q(\ip_start_count_reg_n_0_[31] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[32] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[32]_i_1_n_15 ),
        .Q(data9[0]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_start_count_reg[32]_i_1 
       (.CI(\ip_start_count_reg[24]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ip_start_count_reg[32]_i_1_n_0 ,\ip_start_count_reg[32]_i_1_n_1 ,\ip_start_count_reg[32]_i_1_n_2 ,\ip_start_count_reg[32]_i_1_n_3 ,\ip_start_count_reg[32]_i_1_n_4 ,\ip_start_count_reg[32]_i_1_n_5 ,\ip_start_count_reg[32]_i_1_n_6 ,\ip_start_count_reg[32]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\ip_start_count_reg[32]_i_1_n_8 ,\ip_start_count_reg[32]_i_1_n_9 ,\ip_start_count_reg[32]_i_1_n_10 ,\ip_start_count_reg[32]_i_1_n_11 ,\ip_start_count_reg[32]_i_1_n_12 ,\ip_start_count_reg[32]_i_1_n_13 ,\ip_start_count_reg[32]_i_1_n_14 ,\ip_start_count_reg[32]_i_1_n_15 }),
        .S(data9[7:0]));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[33] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[32]_i_1_n_14 ),
        .Q(data9[1]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[34] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[32]_i_1_n_13 ),
        .Q(data9[2]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[35] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[32]_i_1_n_12 ),
        .Q(data9[3]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[36] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[32]_i_1_n_11 ),
        .Q(data9[4]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[37] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[32]_i_1_n_10 ),
        .Q(data9[5]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[38] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[32]_i_1_n_9 ),
        .Q(data9[6]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[39] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[32]_i_1_n_8 ),
        .Q(data9[7]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[3] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[0]_i_2_n_12 ),
        .Q(\ip_start_count_reg_n_0_[3] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[40] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[40]_i_1_n_15 ),
        .Q(data9[8]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_start_count_reg[40]_i_1 
       (.CI(\ip_start_count_reg[32]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ip_start_count_reg[40]_i_1_n_0 ,\ip_start_count_reg[40]_i_1_n_1 ,\ip_start_count_reg[40]_i_1_n_2 ,\ip_start_count_reg[40]_i_1_n_3 ,\ip_start_count_reg[40]_i_1_n_4 ,\ip_start_count_reg[40]_i_1_n_5 ,\ip_start_count_reg[40]_i_1_n_6 ,\ip_start_count_reg[40]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\ip_start_count_reg[40]_i_1_n_8 ,\ip_start_count_reg[40]_i_1_n_9 ,\ip_start_count_reg[40]_i_1_n_10 ,\ip_start_count_reg[40]_i_1_n_11 ,\ip_start_count_reg[40]_i_1_n_12 ,\ip_start_count_reg[40]_i_1_n_13 ,\ip_start_count_reg[40]_i_1_n_14 ,\ip_start_count_reg[40]_i_1_n_15 }),
        .S(data9[15:8]));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[41] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[40]_i_1_n_14 ),
        .Q(data9[9]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[42] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[40]_i_1_n_13 ),
        .Q(data9[10]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[43] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[40]_i_1_n_12 ),
        .Q(data9[11]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[44] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[40]_i_1_n_11 ),
        .Q(data9[12]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[45] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[40]_i_1_n_10 ),
        .Q(data9[13]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[46] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[40]_i_1_n_9 ),
        .Q(data9[14]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[47] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[40]_i_1_n_8 ),
        .Q(data9[15]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[48] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[48]_i_1_n_15 ),
        .Q(data9[16]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_start_count_reg[48]_i_1 
       (.CI(\ip_start_count_reg[40]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ip_start_count_reg[48]_i_1_n_0 ,\ip_start_count_reg[48]_i_1_n_1 ,\ip_start_count_reg[48]_i_1_n_2 ,\ip_start_count_reg[48]_i_1_n_3 ,\ip_start_count_reg[48]_i_1_n_4 ,\ip_start_count_reg[48]_i_1_n_5 ,\ip_start_count_reg[48]_i_1_n_6 ,\ip_start_count_reg[48]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\ip_start_count_reg[48]_i_1_n_8 ,\ip_start_count_reg[48]_i_1_n_9 ,\ip_start_count_reg[48]_i_1_n_10 ,\ip_start_count_reg[48]_i_1_n_11 ,\ip_start_count_reg[48]_i_1_n_12 ,\ip_start_count_reg[48]_i_1_n_13 ,\ip_start_count_reg[48]_i_1_n_14 ,\ip_start_count_reg[48]_i_1_n_15 }),
        .S(data9[23:16]));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[49] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[48]_i_1_n_14 ),
        .Q(data9[17]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[4] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[0]_i_2_n_11 ),
        .Q(\ip_start_count_reg_n_0_[4] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[50] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[48]_i_1_n_13 ),
        .Q(data9[18]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[51] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[48]_i_1_n_12 ),
        .Q(data9[19]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[52] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[48]_i_1_n_11 ),
        .Q(data9[20]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[53] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[48]_i_1_n_10 ),
        .Q(data9[21]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[54] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[48]_i_1_n_9 ),
        .Q(data9[22]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[55] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[48]_i_1_n_8 ),
        .Q(data9[23]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[56] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[56]_i_1_n_15 ),
        .Q(data9[24]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_start_count_reg[56]_i_1 
       (.CI(\ip_start_count_reg[48]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\NLW_ip_start_count_reg[56]_i_1_CO_UNCONNECTED [7],\ip_start_count_reg[56]_i_1_n_1 ,\ip_start_count_reg[56]_i_1_n_2 ,\ip_start_count_reg[56]_i_1_n_3 ,\ip_start_count_reg[56]_i_1_n_4 ,\ip_start_count_reg[56]_i_1_n_5 ,\ip_start_count_reg[56]_i_1_n_6 ,\ip_start_count_reg[56]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\ip_start_count_reg[56]_i_1_n_8 ,\ip_start_count_reg[56]_i_1_n_9 ,\ip_start_count_reg[56]_i_1_n_10 ,\ip_start_count_reg[56]_i_1_n_11 ,\ip_start_count_reg[56]_i_1_n_12 ,\ip_start_count_reg[56]_i_1_n_13 ,\ip_start_count_reg[56]_i_1_n_14 ,\ip_start_count_reg[56]_i_1_n_15 }),
        .S(data9[31:24]));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[57] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[56]_i_1_n_14 ),
        .Q(data9[25]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[58] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[56]_i_1_n_13 ),
        .Q(data9[26]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[59] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[56]_i_1_n_12 ),
        .Q(data9[27]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[5] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[0]_i_2_n_10 ),
        .Q(\ip_start_count_reg_n_0_[5] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[60] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[56]_i_1_n_11 ),
        .Q(data9[28]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[61] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[56]_i_1_n_10 ),
        .Q(data9[29]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[62] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[56]_i_1_n_9 ),
        .Q(data9[30]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[63] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[56]_i_1_n_8 ),
        .Q(data9[31]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[6] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[0]_i_2_n_9 ),
        .Q(\ip_start_count_reg_n_0_[6] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[7] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[0]_i_2_n_8 ),
        .Q(\ip_start_count_reg_n_0_[7] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[8] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[8]_i_1_n_15 ),
        .Q(\ip_start_count_reg_n_0_[8] ),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_start_count_reg[8]_i_1 
       (.CI(\ip_start_count_reg[0]_i_2_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ip_start_count_reg[8]_i_1_n_0 ,\ip_start_count_reg[8]_i_1_n_1 ,\ip_start_count_reg[8]_i_1_n_2 ,\ip_start_count_reg[8]_i_1_n_3 ,\ip_start_count_reg[8]_i_1_n_4 ,\ip_start_count_reg[8]_i_1_n_5 ,\ip_start_count_reg[8]_i_1_n_6 ,\ip_start_count_reg[8]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\ip_start_count_reg[8]_i_1_n_8 ,\ip_start_count_reg[8]_i_1_n_9 ,\ip_start_count_reg[8]_i_1_n_10 ,\ip_start_count_reg[8]_i_1_n_11 ,\ip_start_count_reg[8]_i_1_n_12 ,\ip_start_count_reg[8]_i_1_n_13 ,\ip_start_count_reg[8]_i_1_n_14 ,\ip_start_count_reg[8]_i_1_n_15 }),
        .S({\ip_start_count_reg_n_0_[15] ,\ip_start_count_reg_n_0_[14] ,\ip_start_count_reg_n_0_[13] ,\ip_start_count_reg_n_0_[12] ,\ip_start_count_reg_n_0_[11] ,\ip_start_count_reg_n_0_[10] ,\ip_start_count_reg_n_0_[9] ,\ip_start_count_reg_n_0_[8] }));
  FDRE #(
    .INIT(1'b0)) 
    \ip_start_count_reg[9] 
       (.C(mon_clk),
        .CE(ip_start_count0),
        .D(\ip_start_count_reg[8]_i_1_n_14 ),
        .Q(\ip_start_count_reg_n_0_[9] ),
        .R(RST_ACTIVE));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_min_max_ctr min_max_ctr_i
       (.Q(Q),
        .RST_ACTIVE(RST_ACTIVE),
        .ap_continue_reg(ap_continue_reg),
        .ap_done_reg(ap_done_reg),
        .dataflow_en(dataflow_en),
        .empty(empty),
        .\max_ctr_reg[63]_0 (\max_ctr_reg[63] ),
        .mon_clk(mon_clk),
        .rd_en(rd_en),
        .start_pulse(start_pulse));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[0]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[0] ),
        .I1(\ip_cycles_avg_reg_n_0_[0] ),
        .I2(\ip_start_count_reg_n_0_[0] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[0]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[0]_i_6 
       (.I0(data5[0]),
        .I1(data6[0]),
        .I2(data9[0]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[32]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[10]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[10] ),
        .I1(\ip_cycles_avg_reg_n_0_[10] ),
        .I2(\ip_start_count_reg_n_0_[10] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[10]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[10]_i_6 
       (.I0(data5[10]),
        .I1(data6[10]),
        .I2(data9[10]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[42]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[11]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[11] ),
        .I1(\ip_cycles_avg_reg_n_0_[11] ),
        .I2(\ip_start_count_reg_n_0_[11] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[11]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[11]_i_6 
       (.I0(data5[11]),
        .I1(data6[11]),
        .I2(data9[11]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[43]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[12]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[12] ),
        .I1(\ip_cycles_avg_reg_n_0_[12] ),
        .I2(\ip_start_count_reg_n_0_[12] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[12]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[12]_i_6 
       (.I0(data5[12]),
        .I1(data6[12]),
        .I2(data9[12]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[44]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[13]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[13] ),
        .I1(\ip_cycles_avg_reg_n_0_[13] ),
        .I2(\ip_start_count_reg_n_0_[13] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[13]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[13]_i_6 
       (.I0(data5[13]),
        .I1(data6[13]),
        .I2(data9[13]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[45]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[14]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[14] ),
        .I1(\ip_cycles_avg_reg_n_0_[14] ),
        .I2(\ip_start_count_reg_n_0_[14] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[14]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[14]_i_6 
       (.I0(data5[14]),
        .I1(data6[14]),
        .I2(data9[14]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[46]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[15]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[15] ),
        .I1(\ip_cycles_avg_reg_n_0_[15] ),
        .I2(\ip_start_count_reg_n_0_[15] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[15]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[15]_i_6 
       (.I0(data5[15]),
        .I1(data6[15]),
        .I2(data9[15]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[47]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[16]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[16] ),
        .I1(\ip_cycles_avg_reg_n_0_[16] ),
        .I2(\ip_start_count_reg_n_0_[16] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[16]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[16]_i_6 
       (.I0(data5[16]),
        .I1(data6[16]),
        .I2(data9[16]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[48]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[17]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[17] ),
        .I1(\ip_cycles_avg_reg_n_0_[17] ),
        .I2(\ip_start_count_reg_n_0_[17] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[17]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[17]_i_6 
       (.I0(data5[17]),
        .I1(data6[17]),
        .I2(data9[17]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[49]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[18]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[18] ),
        .I1(\ip_cycles_avg_reg_n_0_[18] ),
        .I2(\ip_start_count_reg_n_0_[18] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[18]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[18]_i_6 
       (.I0(data5[18]),
        .I1(data6[18]),
        .I2(data9[18]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[50]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[19]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[19] ),
        .I1(\ip_cycles_avg_reg_n_0_[19] ),
        .I2(\ip_start_count_reg_n_0_[19] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[19]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[19]_i_6 
       (.I0(data5[19]),
        .I1(data6[19]),
        .I2(data9[19]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[51]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[1]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[1] ),
        .I1(\ip_cycles_avg_reg_n_0_[1] ),
        .I2(\ip_start_count_reg_n_0_[1] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[1]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[1]_i_6 
       (.I0(data5[1]),
        .I1(data6[1]),
        .I2(data9[1]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[33]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[20]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[20] ),
        .I1(\ip_cycles_avg_reg_n_0_[20] ),
        .I2(\ip_start_count_reg_n_0_[20] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[20]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[20]_i_6 
       (.I0(data5[20]),
        .I1(data6[20]),
        .I2(data9[20]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[52]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[21]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[21] ),
        .I1(\ip_cycles_avg_reg_n_0_[21] ),
        .I2(\ip_start_count_reg_n_0_[21] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[21]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[21]_i_6 
       (.I0(data5[21]),
        .I1(data6[21]),
        .I2(data9[21]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[53]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[22]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[22] ),
        .I1(\ip_cycles_avg_reg_n_0_[22] ),
        .I2(\ip_start_count_reg_n_0_[22] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[22]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[22]_i_6 
       (.I0(data5[22]),
        .I1(data6[22]),
        .I2(data9[22]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[54]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[23]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[23] ),
        .I1(\ip_cycles_avg_reg_n_0_[23] ),
        .I2(\ip_start_count_reg_n_0_[23] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[23]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[23]_i_6 
       (.I0(data5[23]),
        .I1(data6[23]),
        .I2(data9[23]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[55]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[24]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[24] ),
        .I1(\ip_cycles_avg_reg_n_0_[24] ),
        .I2(\ip_start_count_reg_n_0_[24] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[24]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[24]_i_6 
       (.I0(data5[24]),
        .I1(data6[24]),
        .I2(data9[24]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[56]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[25]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[25] ),
        .I1(\ip_cycles_avg_reg_n_0_[25] ),
        .I2(\ip_start_count_reg_n_0_[25] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[25]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[25]_i_6 
       (.I0(data5[25]),
        .I1(data6[25]),
        .I2(data9[25]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[57]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[26]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[26] ),
        .I1(\ip_cycles_avg_reg_n_0_[26] ),
        .I2(\ip_start_count_reg_n_0_[26] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[26]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[26]_i_6 
       (.I0(data5[26]),
        .I1(data6[26]),
        .I2(data9[26]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[58]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[27]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[27] ),
        .I1(\ip_cycles_avg_reg_n_0_[27] ),
        .I2(\ip_start_count_reg_n_0_[27] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[27]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[27]_i_6 
       (.I0(data5[27]),
        .I1(data6[27]),
        .I2(data9[27]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[59]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[28]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[28] ),
        .I1(\ip_cycles_avg_reg_n_0_[28] ),
        .I2(\ip_start_count_reg_n_0_[28] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[28]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[28]_i_6 
       (.I0(data5[28]),
        .I1(data6[28]),
        .I2(data9[28]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[60]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[29]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[29] ),
        .I1(\ip_cycles_avg_reg_n_0_[29] ),
        .I2(\ip_start_count_reg_n_0_[29] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[29]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[29]_i_6 
       (.I0(data5[29]),
        .I1(data6[29]),
        .I2(data9[29]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[61]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[2]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[2] ),
        .I1(\ip_cycles_avg_reg_n_0_[2] ),
        .I2(\ip_start_count_reg_n_0_[2] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[2]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[2]_i_6 
       (.I0(data5[2]),
        .I1(data6[2]),
        .I2(data9[2]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[34]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[30]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[30] ),
        .I1(\ip_cycles_avg_reg_n_0_[30] ),
        .I2(\ip_start_count_reg_n_0_[30] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[30]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[30]_i_6 
       (.I0(data5[30]),
        .I1(data6[30]),
        .I2(data9[30]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[62]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[31]_i_13 
       (.I0(\ip_exec_count_reg_n_0_[31] ),
        .I1(\ip_cycles_avg_reg_n_0_[31] ),
        .I2(\ip_start_count_reg_n_0_[31] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[31]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[31]_i_17 
       (.I0(data5[31]),
        .I1(data6[31]),
        .I2(data9[31]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[63]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[3]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[3] ),
        .I1(\ip_cycles_avg_reg_n_0_[3] ),
        .I2(\ip_start_count_reg_n_0_[3] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[3]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[3]_i_6 
       (.I0(data5[3]),
        .I1(data6[3]),
        .I2(data9[3]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[35]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[4]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[4] ),
        .I1(\ip_cycles_avg_reg_n_0_[4] ),
        .I2(\ip_start_count_reg_n_0_[4] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[4]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[4]_i_6 
       (.I0(data5[4]),
        .I1(data6[4]),
        .I2(data9[4]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[36]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[5]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[5] ),
        .I1(\ip_cycles_avg_reg_n_0_[5] ),
        .I2(\ip_start_count_reg_n_0_[5] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[5]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[5]_i_6 
       (.I0(data5[5]),
        .I1(data6[5]),
        .I2(data9[5]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[37]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[6]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[6] ),
        .I1(\ip_cycles_avg_reg_n_0_[6] ),
        .I2(\ip_start_count_reg_n_0_[6] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[6]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[6]_i_6 
       (.I0(data5[6]),
        .I1(data6[6]),
        .I2(data9[6]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[38]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[7]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[7] ),
        .I1(\ip_cycles_avg_reg_n_0_[7] ),
        .I2(\ip_start_count_reg_n_0_[7] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[7]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[7]_i_6 
       (.I0(data5[7]),
        .I1(data6[7]),
        .I2(data9[7]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[39]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[8]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[8] ),
        .I1(\ip_cycles_avg_reg_n_0_[8] ),
        .I2(\ip_start_count_reg_n_0_[8] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[8]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[8]_i_6 
       (.I0(data5[8]),
        .I1(data6[8]),
        .I2(data9[8]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[40]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[9]_i_5 
       (.I0(\ip_exec_count_reg_n_0_[9] ),
        .I1(\ip_cycles_avg_reg_n_0_[9] ),
        .I2(\ip_start_count_reg_n_0_[9] ),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[9]_0 ));
  LUT6 #(
    .INIT(64'hF00000CC000000AA)) 
    \sample_data[9]_i_6 
       (.I0(data5[9]),
        .I1(data6[9]),
        .I2(data9[9]),
        .I3(slv_reg_addr[1]),
        .I4(slv_reg_addr[2]),
        .I5(slv_reg_addr[0]),
        .O(\ip_exec_count_reg[41]_0 ));
  FDRE \sample_data_reg[0] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[0]),
        .Q(\sample_data_reg[31]_0 [0]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[10] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[10]),
        .Q(\sample_data_reg[31]_0 [10]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[11] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[11]),
        .Q(\sample_data_reg[31]_0 [11]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[12] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[12]),
        .Q(\sample_data_reg[31]_0 [12]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[13] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[13]),
        .Q(\sample_data_reg[31]_0 [13]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[14] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[14]),
        .Q(\sample_data_reg[31]_0 [14]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[15] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[15]),
        .Q(\sample_data_reg[31]_0 [15]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[16] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[16]),
        .Q(\sample_data_reg[31]_0 [16]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[17] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[17]),
        .Q(\sample_data_reg[31]_0 [17]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[18] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[18]),
        .Q(\sample_data_reg[31]_0 [18]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[19] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[19]),
        .Q(\sample_data_reg[31]_0 [19]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[1] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[1]),
        .Q(\sample_data_reg[31]_0 [1]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[20] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[20]),
        .Q(\sample_data_reg[31]_0 [20]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[21] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[21]),
        .Q(\sample_data_reg[31]_0 [21]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[22] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[22]),
        .Q(\sample_data_reg[31]_0 [22]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[23] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[23]),
        .Q(\sample_data_reg[31]_0 [23]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[24] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[24]),
        .Q(\sample_data_reg[31]_0 [24]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[25] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[25]),
        .Q(\sample_data_reg[31]_0 [25]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[26] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[26]),
        .Q(\sample_data_reg[31]_0 [26]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[27] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[27]),
        .Q(\sample_data_reg[31]_0 [27]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[28] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[28]),
        .Q(\sample_data_reg[31]_0 [28]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[29] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[29]),
        .Q(\sample_data_reg[31]_0 [29]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[2] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[2]),
        .Q(\sample_data_reg[31]_0 [2]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[30] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[30]),
        .Q(\sample_data_reg[31]_0 [30]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[31] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[31]),
        .Q(\sample_data_reg[31]_0 [31]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[3] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[3]),
        .Q(\sample_data_reg[31]_0 [3]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[4] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[4]),
        .Q(\sample_data_reg[31]_0 [4]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[5] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[5]),
        .Q(\sample_data_reg[31]_0 [5]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[6] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[6]),
        .Q(\sample_data_reg[31]_0 [6]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[7] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[7]),
        .Q(\sample_data_reg[31]_0 [7]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[8] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[8]),
        .Q(\sample_data_reg[31]_0 [8]),
        .R(RST_ACTIVE));
  FDRE \sample_data_reg[9] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(D[9]),
        .Q(\sample_data_reg[31]_0 [9]),
        .R(RST_ACTIVE));
endmodule

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_accelerator_monitor
   (s_axi_arready,
    s_axi_wready,
    s_axi_awready,
    s_axi_rdata,
    trace_data,
    trace_valid,
    s_axi_bvalid,
    s_axi_rvalid,
    mon_resetn,
    s_axi_awaddr_mon,
    mon_clk,
    s_axi_araddr_mon,
    s_axi_wvalid_mon,
    s_axi_wready_mon,
    s_axi_wstrb_mon,
    s_axi_wdata_mon,
    s_axi_rvalid_mon,
    s_axi_rdata_mon,
    s_axi_rready_mon,
    s_axi_wdata,
    trace_clk,
    trace_read,
    trace_counter_overflow,
    trace_counter,
    trace_rst,
    s_axi_awaddr,
    s_axi_araddr,
    s_axi_arvalid,
    s_axi_awvalid,
    s_axi_wvalid,
    s_axi_awvalid_mon,
    s_axi_awready_mon,
    s_axi_arvalid_mon,
    s_axi_arready_mon,
    s_axi_bready,
    s_axi_rready);
  output s_axi_arready;
  output s_axi_wready;
  output s_axi_awready;
  output [31:0]s_axi_rdata;
  output [55:0]trace_data;
  output trace_valid;
  output s_axi_bvalid;
  output s_axi_rvalid;
  input mon_resetn;
  input [7:0]s_axi_awaddr_mon;
  input mon_clk;
  input [7:0]s_axi_araddr_mon;
  input s_axi_wvalid_mon;
  input s_axi_wready_mon;
  input [0:0]s_axi_wstrb_mon;
  input [1:0]s_axi_wdata_mon;
  input s_axi_rvalid_mon;
  input [0:0]s_axi_rdata_mon;
  input s_axi_rready_mon;
  input [5:0]s_axi_wdata;
  input trace_clk;
  input trace_read;
  input trace_counter_overflow;
  input [44:0]trace_counter;
  input trace_rst;
  input [7:0]s_axi_awaddr;
  input [7:0]s_axi_araddr;
  input s_axi_arvalid;
  input s_axi_awvalid;
  input s_axi_wvalid;
  input s_axi_awvalid_mon;
  input s_axi_awready_mon;
  input s_axi_arvalid_mon;
  input s_axi_arready_mon;
  input s_axi_bready;
  input s_axi_rready;

  wire Metrics_Cnt_En;
  wire RST_ACTIVE;
  wire [0:0]_start_events;
  wire [0:0]_stop_events;
  wire acc_ctr_i_n_100;
  wire acc_ctr_i_n_101;
  wire acc_ctr_i_n_102;
  wire acc_ctr_i_n_103;
  wire acc_ctr_i_n_104;
  wire acc_ctr_i_n_105;
  wire acc_ctr_i_n_106;
  wire acc_ctr_i_n_107;
  wire acc_ctr_i_n_108;
  wire acc_ctr_i_n_109;
  wire acc_ctr_i_n_110;
  wire acc_ctr_i_n_111;
  wire acc_ctr_i_n_112;
  wire acc_ctr_i_n_113;
  wire acc_ctr_i_n_114;
  wire acc_ctr_i_n_115;
  wire acc_ctr_i_n_116;
  wire acc_ctr_i_n_117;
  wire acc_ctr_i_n_118;
  wire acc_ctr_i_n_119;
  wire acc_ctr_i_n_120;
  wire acc_ctr_i_n_121;
  wire acc_ctr_i_n_122;
  wire acc_ctr_i_n_123;
  wire acc_ctr_i_n_124;
  wire acc_ctr_i_n_125;
  wire acc_ctr_i_n_126;
  wire acc_ctr_i_n_127;
  wire acc_ctr_i_n_128;
  wire acc_ctr_i_n_129;
  wire acc_ctr_i_n_162;
  wire acc_ctr_i_n_163;
  wire acc_ctr_i_n_164;
  wire acc_ctr_i_n_165;
  wire acc_ctr_i_n_166;
  wire acc_ctr_i_n_167;
  wire acc_ctr_i_n_168;
  wire acc_ctr_i_n_169;
  wire acc_ctr_i_n_170;
  wire acc_ctr_i_n_171;
  wire acc_ctr_i_n_172;
  wire acc_ctr_i_n_173;
  wire acc_ctr_i_n_174;
  wire acc_ctr_i_n_175;
  wire acc_ctr_i_n_176;
  wire acc_ctr_i_n_177;
  wire acc_ctr_i_n_178;
  wire acc_ctr_i_n_179;
  wire acc_ctr_i_n_180;
  wire acc_ctr_i_n_181;
  wire acc_ctr_i_n_182;
  wire acc_ctr_i_n_183;
  wire acc_ctr_i_n_184;
  wire acc_ctr_i_n_185;
  wire acc_ctr_i_n_186;
  wire acc_ctr_i_n_187;
  wire acc_ctr_i_n_188;
  wire acc_ctr_i_n_189;
  wire acc_ctr_i_n_190;
  wire acc_ctr_i_n_191;
  wire acc_ctr_i_n_192;
  wire acc_ctr_i_n_193;
  wire acc_ctr_i_n_194;
  wire acc_ctr_i_n_195;
  wire acc_ctr_i_n_196;
  wire acc_ctr_i_n_197;
  wire acc_ctr_i_n_198;
  wire acc_ctr_i_n_199;
  wire acc_ctr_i_n_200;
  wire acc_ctr_i_n_201;
  wire acc_ctr_i_n_202;
  wire acc_ctr_i_n_203;
  wire acc_ctr_i_n_204;
  wire acc_ctr_i_n_205;
  wire acc_ctr_i_n_206;
  wire acc_ctr_i_n_207;
  wire acc_ctr_i_n_208;
  wire acc_ctr_i_n_209;
  wire acc_ctr_i_n_210;
  wire acc_ctr_i_n_211;
  wire acc_ctr_i_n_212;
  wire acc_ctr_i_n_213;
  wire acc_ctr_i_n_214;
  wire acc_ctr_i_n_215;
  wire acc_ctr_i_n_216;
  wire acc_ctr_i_n_217;
  wire acc_ctr_i_n_218;
  wire acc_ctr_i_n_219;
  wire acc_ctr_i_n_220;
  wire acc_ctr_i_n_221;
  wire acc_ctr_i_n_222;
  wire acc_ctr_i_n_223;
  wire acc_ctr_i_n_224;
  wire acc_ctr_i_n_225;
  wire acc_ctr_i_n_226;
  wire acc_ctr_i_n_227;
  wire acc_ctr_i_n_228;
  wire acc_ctr_i_n_229;
  wire acc_ctr_i_n_230;
  wire acc_ctr_i_n_231;
  wire acc_ctr_i_n_232;
  wire acc_ctr_i_n_233;
  wire acc_ctr_i_n_234;
  wire acc_ctr_i_n_235;
  wire acc_ctr_i_n_236;
  wire acc_ctr_i_n_237;
  wire acc_ctr_i_n_238;
  wire acc_ctr_i_n_239;
  wire acc_ctr_i_n_240;
  wire acc_ctr_i_n_241;
  wire acc_ctr_i_n_242;
  wire acc_ctr_i_n_243;
  wire acc_ctr_i_n_244;
  wire acc_ctr_i_n_245;
  wire acc_ctr_i_n_246;
  wire acc_ctr_i_n_247;
  wire acc_ctr_i_n_248;
  wire acc_ctr_i_n_249;
  wire acc_ctr_i_n_250;
  wire acc_ctr_i_n_251;
  wire acc_ctr_i_n_252;
  wire acc_ctr_i_n_253;
  wire acc_ctr_i_n_254;
  wire acc_ctr_i_n_255;
  wire acc_ctr_i_n_256;
  wire acc_ctr_i_n_257;
  wire acc_ctr_i_n_258;
  wire acc_ctr_i_n_259;
  wire acc_ctr_i_n_260;
  wire acc_ctr_i_n_261;
  wire acc_ctr_i_n_262;
  wire acc_ctr_i_n_263;
  wire acc_ctr_i_n_264;
  wire acc_ctr_i_n_265;
  wire acc_ctr_i_n_266;
  wire acc_ctr_i_n_267;
  wire acc_ctr_i_n_268;
  wire acc_ctr_i_n_269;
  wire acc_ctr_i_n_270;
  wire acc_ctr_i_n_271;
  wire acc_ctr_i_n_272;
  wire acc_ctr_i_n_273;
  wire acc_ctr_i_n_274;
  wire acc_ctr_i_n_275;
  wire acc_ctr_i_n_276;
  wire acc_ctr_i_n_277;
  wire acc_ctr_i_n_278;
  wire acc_ctr_i_n_279;
  wire acc_ctr_i_n_280;
  wire acc_ctr_i_n_281;
  wire acc_ctr_i_n_282;
  wire acc_ctr_i_n_283;
  wire acc_ctr_i_n_284;
  wire acc_ctr_i_n_285;
  wire acc_ctr_i_n_286;
  wire acc_ctr_i_n_287;
  wire acc_ctr_i_n_288;
  wire acc_ctr_i_n_289;
  wire acc_ctr_i_n_98;
  wire acc_ctr_i_n_99;
  wire ap_continue_reg;
  wire ap_done_reg;
  wire axi_lite_if_i_n_10;
  wire axi_lite_if_i_n_11;
  wire axi_lite_if_i_n_12;
  wire axi_lite_if_i_n_13;
  wire axi_lite_if_i_n_14;
  wire axi_lite_if_i_n_15;
  wire axi_lite_if_i_n_16;
  wire axi_lite_if_i_n_17;
  wire axi_lite_if_i_n_18;
  wire axi_lite_if_i_n_19;
  wire axi_lite_if_i_n_20;
  wire axi_lite_if_i_n_21;
  wire axi_lite_if_i_n_22;
  wire axi_lite_if_i_n_23;
  wire axi_lite_if_i_n_24;
  wire axi_lite_if_i_n_25;
  wire axi_lite_if_i_n_26;
  wire axi_lite_if_i_n_27;
  wire axi_lite_if_i_n_28;
  wire axi_lite_if_i_n_29;
  wire axi_lite_if_i_n_30;
  wire axi_lite_if_i_n_31;
  wire axi_lite_if_i_n_32;
  wire axi_lite_if_i_n_33;
  wire axi_lite_if_i_n_34;
  wire axi_lite_if_i_n_35;
  wire axi_lite_if_i_n_36;
  wire axi_lite_if_i_n_44;
  wire axi_lite_if_i_n_46;
  wire axi_lite_if_i_n_5;
  wire axi_lite_if_i_n_54;
  wire axi_lite_if_i_n_6;
  wire axi_lite_if_i_n_7;
  wire axi_lite_if_i_n_8;
  wire axi_lite_if_i_n_9;
  wire control_wr_en;
  wire [31:0]data7;
  wire [31:0]data8;
  wire dataflow_en;
  wire [63:0]ip_cur_tranx_reg;
  wire ip_exec_count0;
  wire ip_idle;
  wire ip_start_count0;
  wire \min_max_ctr_i/empty ;
  wire \min_max_ctr_i/read ;
  wire mon_axilite_i_n_10;
  wire mon_axilite_i_n_11;
  wire mon_axilite_i_n_12;
  wire mon_axilite_i_n_13;
  wire mon_axilite_i_n_14;
  wire mon_axilite_i_n_15;
  wire mon_axilite_i_n_16;
  wire mon_axilite_i_n_17;
  wire mon_axilite_i_n_18;
  wire mon_axilite_i_n_19;
  wire mon_axilite_i_n_20;
  wire mon_axilite_i_n_21;
  wire mon_axilite_i_n_22;
  wire mon_axilite_i_n_23;
  wire mon_axilite_i_n_24;
  wire mon_axilite_i_n_25;
  wire mon_axilite_i_n_26;
  wire mon_axilite_i_n_27;
  wire mon_axilite_i_n_28;
  wire mon_axilite_i_n_29;
  wire mon_axilite_i_n_30;
  wire mon_axilite_i_n_31;
  wire mon_axilite_i_n_32;
  wire mon_axilite_i_n_33;
  wire mon_axilite_i_n_34;
  wire mon_axilite_i_n_35;
  wire mon_axilite_i_n_36;
  wire mon_axilite_i_n_37;
  wire mon_axilite_i_n_38;
  wire mon_axilite_i_n_39;
  wire mon_axilite_i_n_40;
  wire mon_axilite_i_n_41;
  wire mon_axilite_i_n_42;
  wire mon_axilite_i_n_43;
  wire mon_axilite_i_n_44;
  wire mon_axilite_i_n_45;
  wire mon_axilite_i_n_46;
  wire mon_axilite_i_n_47;
  wire mon_axilite_i_n_48;
  wire mon_axilite_i_n_49;
  wire mon_axilite_i_n_50;
  wire mon_axilite_i_n_51;
  wire mon_axilite_i_n_52;
  wire mon_axilite_i_n_53;
  wire mon_axilite_i_n_54;
  wire mon_axilite_i_n_55;
  wire mon_axilite_i_n_56;
  wire mon_axilite_i_n_57;
  wire mon_axilite_i_n_58;
  wire mon_axilite_i_n_59;
  wire mon_axilite_i_n_60;
  wire mon_axilite_i_n_61;
  wire mon_axilite_i_n_62;
  wire mon_axilite_i_n_63;
  wire mon_axilite_i_n_64;
  wire mon_axilite_i_n_65;
  wire mon_axilite_i_n_66;
  wire mon_axilite_i_n_67;
  wire mon_axilite_i_n_68;
  wire mon_axilite_i_n_69;
  wire mon_axilite_i_n_7;
  wire mon_axilite_i_n_70;
  wire mon_axilite_i_n_8;
  wire mon_axilite_i_n_9;
  wire mon_clk;
  wire mon_resetn;
  wire [5:0]p_0_out;
  wire p_1_in;
  wire [1:1]register_select;
  wire registers_i_n_0;
  wire registers_i_n_13;
  wire registers_i_n_2;
  wire registers_i_n_6;
  wire registers_i_n_8;
  wire registers_i_n_9;
  wire reset_sample_reg__0;
  wire [7:0]s_axi_araddr;
  wire [7:0]s_axi_araddr_mon;
  wire s_axi_arready;
  wire s_axi_arready_mon;
  wire s_axi_arvalid;
  wire s_axi_arvalid_mon;
  wire [7:0]s_axi_awaddr;
  wire [7:0]s_axi_awaddr_mon;
  wire s_axi_awready;
  wire s_axi_awready_mon;
  wire s_axi_awvalid;
  wire s_axi_awvalid_mon;
  wire s_axi_bready;
  wire s_axi_bvalid;
  wire [31:0]s_axi_rdata;
  wire [0:0]s_axi_rdata_mon;
  wire s_axi_rready;
  wire s_axi_rready_mon;
  wire s_axi_rvalid;
  wire s_axi_rvalid_mon;
  wire [5:0]s_axi_wdata;
  wire [1:0]s_axi_wdata_mon;
  wire s_axi_wready;
  wire s_axi_wready_mon;
  wire [0:0]s_axi_wstrb_mon;
  wire s_axi_wvalid;
  wire s_axi_wvalid_mon;
  wire [31:0]sample_data;
  wire sample_en;
  wire sample_reg_rd_first;
  wire sample_time_diff_reg;
  wire [4:2]slv_reg_addr;
  wire slv_reg_addr_vld;
  wire [31:0]slv_reg_in;
  wire slv_reg_in_vld;
  wire slv_reg_out_vld;
  wire start_pulse;
  wire trace_clk;
  wire [0:0]trace_control;
  wire trace_control_wr_en;
  wire [44:0]trace_counter;
  wire trace_counter_overflow;
  wire [55:0]trace_data;
  wire trace_read;
  wire trace_rst;
  wire [3:0]trace_start_events;
  wire [3:0]trace_stop_events;
  wire trace_valid;

  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_accel_counters acc_ctr_i
       (.CO(ip_idle),
        .D({axi_lite_if_i_n_5,axi_lite_if_i_n_6,axi_lite_if_i_n_7,axi_lite_if_i_n_8,axi_lite_if_i_n_9,axi_lite_if_i_n_10,axi_lite_if_i_n_11,axi_lite_if_i_n_12,axi_lite_if_i_n_13,axi_lite_if_i_n_14,axi_lite_if_i_n_15,axi_lite_if_i_n_16,axi_lite_if_i_n_17,axi_lite_if_i_n_18,axi_lite_if_i_n_19,axi_lite_if_i_n_20,axi_lite_if_i_n_21,axi_lite_if_i_n_22,axi_lite_if_i_n_23,axi_lite_if_i_n_24,axi_lite_if_i_n_25,axi_lite_if_i_n_26,axi_lite_if_i_n_27,axi_lite_if_i_n_28,axi_lite_if_i_n_29,axi_lite_if_i_n_30,axi_lite_if_i_n_31,axi_lite_if_i_n_32,axi_lite_if_i_n_33,axi_lite_if_i_n_34,axi_lite_if_i_n_35,axi_lite_if_i_n_36}),
        .E(sample_en),
        .Metrics_Cnt_En(Metrics_Cnt_En),
        .O({mon_axilite_i_n_7,mon_axilite_i_n_8,mon_axilite_i_n_9,mon_axilite_i_n_10,mon_axilite_i_n_11,mon_axilite_i_n_12,mon_axilite_i_n_13,mon_axilite_i_n_14}),
        .Q({data7,acc_ctr_i_n_98,acc_ctr_i_n_99,acc_ctr_i_n_100,acc_ctr_i_n_101,acc_ctr_i_n_102,acc_ctr_i_n_103,acc_ctr_i_n_104,acc_ctr_i_n_105,acc_ctr_i_n_106,acc_ctr_i_n_107,acc_ctr_i_n_108,acc_ctr_i_n_109,acc_ctr_i_n_110,acc_ctr_i_n_111,acc_ctr_i_n_112,acc_ctr_i_n_113,acc_ctr_i_n_114,acc_ctr_i_n_115,acc_ctr_i_n_116,acc_ctr_i_n_117,acc_ctr_i_n_118,acc_ctr_i_n_119,acc_ctr_i_n_120,acc_ctr_i_n_121,acc_ctr_i_n_122,acc_ctr_i_n_123,acc_ctr_i_n_124,acc_ctr_i_n_125,acc_ctr_i_n_126,acc_ctr_i_n_127,acc_ctr_i_n_128,acc_ctr_i_n_129}),
        .RST_ACTIVE(RST_ACTIVE),
        .ap_continue_reg(ap_continue_reg),
        .ap_done_reg(ap_done_reg),
        .cnt_enabled_reg(registers_i_n_9),
        .dataflow_en(dataflow_en),
        .empty(\min_max_ctr_i/empty ),
        .ip_cur_tranx_reg(ip_cur_tranx_reg),
        .\ip_cur_tranx_reg[15]_0 ({mon_axilite_i_n_15,mon_axilite_i_n_16,mon_axilite_i_n_17,mon_axilite_i_n_18,mon_axilite_i_n_19,mon_axilite_i_n_20,mon_axilite_i_n_21,mon_axilite_i_n_22}),
        .\ip_cur_tranx_reg[23]_0 ({mon_axilite_i_n_23,mon_axilite_i_n_24,mon_axilite_i_n_25,mon_axilite_i_n_26,mon_axilite_i_n_27,mon_axilite_i_n_28,mon_axilite_i_n_29,mon_axilite_i_n_30}),
        .\ip_cur_tranx_reg[31]_0 ({mon_axilite_i_n_31,mon_axilite_i_n_32,mon_axilite_i_n_33,mon_axilite_i_n_34,mon_axilite_i_n_35,mon_axilite_i_n_36,mon_axilite_i_n_37,mon_axilite_i_n_38}),
        .\ip_cur_tranx_reg[39]_0 ({mon_axilite_i_n_39,mon_axilite_i_n_40,mon_axilite_i_n_41,mon_axilite_i_n_42,mon_axilite_i_n_43,mon_axilite_i_n_44,mon_axilite_i_n_45,mon_axilite_i_n_46}),
        .\ip_cur_tranx_reg[47]_0 ({mon_axilite_i_n_47,mon_axilite_i_n_48,mon_axilite_i_n_49,mon_axilite_i_n_50,mon_axilite_i_n_51,mon_axilite_i_n_52,mon_axilite_i_n_53,mon_axilite_i_n_54}),
        .\ip_cur_tranx_reg[55]_0 ({mon_axilite_i_n_55,mon_axilite_i_n_56,mon_axilite_i_n_57,mon_axilite_i_n_58,mon_axilite_i_n_59,mon_axilite_i_n_60,mon_axilite_i_n_61,mon_axilite_i_n_62}),
        .\ip_cur_tranx_reg[63]_0 ({mon_axilite_i_n_63,mon_axilite_i_n_64,mon_axilite_i_n_65,mon_axilite_i_n_66,mon_axilite_i_n_67,mon_axilite_i_n_68,mon_axilite_i_n_69,mon_axilite_i_n_70}),
        .ip_exec_count0(ip_exec_count0),
        .\ip_exec_count_reg[0]_0 (acc_ctr_i_n_226),
        .\ip_exec_count_reg[10]_0 (acc_ctr_i_n_246),
        .\ip_exec_count_reg[11]_0 (acc_ctr_i_n_248),
        .\ip_exec_count_reg[12]_0 (acc_ctr_i_n_250),
        .\ip_exec_count_reg[13]_0 (acc_ctr_i_n_252),
        .\ip_exec_count_reg[14]_0 (acc_ctr_i_n_254),
        .\ip_exec_count_reg[15]_0 (acc_ctr_i_n_256),
        .\ip_exec_count_reg[16]_0 (acc_ctr_i_n_258),
        .\ip_exec_count_reg[17]_0 (acc_ctr_i_n_260),
        .\ip_exec_count_reg[18]_0 (acc_ctr_i_n_262),
        .\ip_exec_count_reg[19]_0 (acc_ctr_i_n_264),
        .\ip_exec_count_reg[1]_0 (acc_ctr_i_n_228),
        .\ip_exec_count_reg[20]_0 (acc_ctr_i_n_266),
        .\ip_exec_count_reg[21]_0 (acc_ctr_i_n_268),
        .\ip_exec_count_reg[22]_0 (acc_ctr_i_n_270),
        .\ip_exec_count_reg[23]_0 (acc_ctr_i_n_272),
        .\ip_exec_count_reg[24]_0 (acc_ctr_i_n_274),
        .\ip_exec_count_reg[25]_0 (acc_ctr_i_n_276),
        .\ip_exec_count_reg[26]_0 (acc_ctr_i_n_278),
        .\ip_exec_count_reg[27]_0 (acc_ctr_i_n_280),
        .\ip_exec_count_reg[28]_0 (acc_ctr_i_n_282),
        .\ip_exec_count_reg[29]_0 (acc_ctr_i_n_284),
        .\ip_exec_count_reg[2]_0 (acc_ctr_i_n_230),
        .\ip_exec_count_reg[30]_0 (acc_ctr_i_n_286),
        .\ip_exec_count_reg[31]_0 (acc_ctr_i_n_288),
        .\ip_exec_count_reg[32]_0 (acc_ctr_i_n_227),
        .\ip_exec_count_reg[33]_0 (acc_ctr_i_n_229),
        .\ip_exec_count_reg[34]_0 (acc_ctr_i_n_231),
        .\ip_exec_count_reg[35]_0 (acc_ctr_i_n_233),
        .\ip_exec_count_reg[36]_0 (acc_ctr_i_n_235),
        .\ip_exec_count_reg[37]_0 (acc_ctr_i_n_237),
        .\ip_exec_count_reg[38]_0 (acc_ctr_i_n_239),
        .\ip_exec_count_reg[39]_0 (acc_ctr_i_n_241),
        .\ip_exec_count_reg[3]_0 (acc_ctr_i_n_232),
        .\ip_exec_count_reg[40]_0 (acc_ctr_i_n_243),
        .\ip_exec_count_reg[41]_0 (acc_ctr_i_n_245),
        .\ip_exec_count_reg[42]_0 (acc_ctr_i_n_247),
        .\ip_exec_count_reg[43]_0 (acc_ctr_i_n_249),
        .\ip_exec_count_reg[44]_0 (acc_ctr_i_n_251),
        .\ip_exec_count_reg[45]_0 (acc_ctr_i_n_253),
        .\ip_exec_count_reg[46]_0 (acc_ctr_i_n_255),
        .\ip_exec_count_reg[47]_0 (acc_ctr_i_n_257),
        .\ip_exec_count_reg[48]_0 (acc_ctr_i_n_259),
        .\ip_exec_count_reg[49]_0 (acc_ctr_i_n_261),
        .\ip_exec_count_reg[4]_0 (acc_ctr_i_n_234),
        .\ip_exec_count_reg[50]_0 (acc_ctr_i_n_263),
        .\ip_exec_count_reg[51]_0 (acc_ctr_i_n_265),
        .\ip_exec_count_reg[52]_0 (acc_ctr_i_n_267),
        .\ip_exec_count_reg[53]_0 (acc_ctr_i_n_269),
        .\ip_exec_count_reg[54]_0 (acc_ctr_i_n_271),
        .\ip_exec_count_reg[55]_0 (acc_ctr_i_n_273),
        .\ip_exec_count_reg[56]_0 (acc_ctr_i_n_275),
        .\ip_exec_count_reg[57]_0 (acc_ctr_i_n_277),
        .\ip_exec_count_reg[58]_0 (acc_ctr_i_n_279),
        .\ip_exec_count_reg[59]_0 (acc_ctr_i_n_281),
        .\ip_exec_count_reg[5]_0 (acc_ctr_i_n_236),
        .\ip_exec_count_reg[60]_0 (acc_ctr_i_n_283),
        .\ip_exec_count_reg[61]_0 (acc_ctr_i_n_285),
        .\ip_exec_count_reg[62]_0 (acc_ctr_i_n_287),
        .\ip_exec_count_reg[63]_0 (acc_ctr_i_n_289),
        .\ip_exec_count_reg[6]_0 (acc_ctr_i_n_238),
        .\ip_exec_count_reg[7]_0 (acc_ctr_i_n_240),
        .\ip_exec_count_reg[8]_0 (acc_ctr_i_n_242),
        .\ip_exec_count_reg[9]_0 (acc_ctr_i_n_244),
        .\ip_max_parallel_tranx_reg[0]_0 (registers_i_n_8),
        .ip_start_count0(ip_start_count0),
        .\max_ctr_reg[63] ({data8,acc_ctr_i_n_162,acc_ctr_i_n_163,acc_ctr_i_n_164,acc_ctr_i_n_165,acc_ctr_i_n_166,acc_ctr_i_n_167,acc_ctr_i_n_168,acc_ctr_i_n_169,acc_ctr_i_n_170,acc_ctr_i_n_171,acc_ctr_i_n_172,acc_ctr_i_n_173,acc_ctr_i_n_174,acc_ctr_i_n_175,acc_ctr_i_n_176,acc_ctr_i_n_177,acc_ctr_i_n_178,acc_ctr_i_n_179,acc_ctr_i_n_180,acc_ctr_i_n_181,acc_ctr_i_n_182,acc_ctr_i_n_183,acc_ctr_i_n_184,acc_ctr_i_n_185,acc_ctr_i_n_186,acc_ctr_i_n_187,acc_ctr_i_n_188,acc_ctr_i_n_189,acc_ctr_i_n_190,acc_ctr_i_n_191,acc_ctr_i_n_192,acc_ctr_i_n_193}),
        .mon_clk(mon_clk),
        .rd_en(\min_max_ctr_i/read ),
        .\sample_ctr_val_reg[32] (acc_ctr_i_n_194),
        .\sample_ctr_val_reg[33] (acc_ctr_i_n_195),
        .\sample_ctr_val_reg[34] (acc_ctr_i_n_196),
        .\sample_ctr_val_reg[35] (acc_ctr_i_n_197),
        .\sample_ctr_val_reg[36] (acc_ctr_i_n_198),
        .\sample_ctr_val_reg[37] (acc_ctr_i_n_199),
        .\sample_ctr_val_reg[38] (acc_ctr_i_n_200),
        .\sample_ctr_val_reg[39] (acc_ctr_i_n_201),
        .\sample_ctr_val_reg[40] (acc_ctr_i_n_202),
        .\sample_ctr_val_reg[41] (acc_ctr_i_n_203),
        .\sample_ctr_val_reg[42] (acc_ctr_i_n_204),
        .\sample_ctr_val_reg[43] (acc_ctr_i_n_205),
        .\sample_ctr_val_reg[44] (acc_ctr_i_n_206),
        .\sample_ctr_val_reg[45] (acc_ctr_i_n_207),
        .\sample_ctr_val_reg[46] (acc_ctr_i_n_208),
        .\sample_ctr_val_reg[47] (acc_ctr_i_n_209),
        .\sample_ctr_val_reg[48] (acc_ctr_i_n_210),
        .\sample_ctr_val_reg[49] (acc_ctr_i_n_211),
        .\sample_ctr_val_reg[50] (acc_ctr_i_n_212),
        .\sample_ctr_val_reg[51] (acc_ctr_i_n_213),
        .\sample_ctr_val_reg[52] (acc_ctr_i_n_214),
        .\sample_ctr_val_reg[53] (acc_ctr_i_n_215),
        .\sample_ctr_val_reg[54] (acc_ctr_i_n_216),
        .\sample_ctr_val_reg[55] (acc_ctr_i_n_217),
        .\sample_ctr_val_reg[56] (acc_ctr_i_n_218),
        .\sample_ctr_val_reg[57] (acc_ctr_i_n_219),
        .\sample_ctr_val_reg[58] (acc_ctr_i_n_220),
        .\sample_ctr_val_reg[59] (acc_ctr_i_n_221),
        .\sample_ctr_val_reg[60] (acc_ctr_i_n_222),
        .\sample_ctr_val_reg[61] (acc_ctr_i_n_223),
        .\sample_ctr_val_reg[62] (acc_ctr_i_n_224),
        .\sample_ctr_val_reg[63] (acc_ctr_i_n_225),
        .\sample_data_reg[31]_0 (sample_data),
        .slv_reg_addr(slv_reg_addr),
        .start_pulse(start_pulse));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AXI_LITE_IF axi_lite_if_i
       (.\Count_Out_i_reg[0] (axi_lite_if_i_n_44),
        .\Count_Out_i_reg[0]_0 (registers_i_n_2),
        .D({axi_lite_if_i_n_5,axi_lite_if_i_n_6,axi_lite_if_i_n_7,axi_lite_if_i_n_8,axi_lite_if_i_n_9,axi_lite_if_i_n_10,axi_lite_if_i_n_11,axi_lite_if_i_n_12,axi_lite_if_i_n_13,axi_lite_if_i_n_14,axi_lite_if_i_n_15,axi_lite_if_i_n_16,axi_lite_if_i_n_17,axi_lite_if_i_n_18,axi_lite_if_i_n_19,axi_lite_if_i_n_20,axi_lite_if_i_n_21,axi_lite_if_i_n_22,axi_lite_if_i_n_23,axi_lite_if_i_n_24,axi_lite_if_i_n_25,axi_lite_if_i_n_26,axi_lite_if_i_n_27,axi_lite_if_i_n_28,axi_lite_if_i_n_29,axi_lite_if_i_n_30,axi_lite_if_i_n_31,axi_lite_if_i_n_32,axi_lite_if_i_n_33,axi_lite_if_i_n_34,axi_lite_if_i_n_35,axi_lite_if_i_n_36}),
        .E(axi_lite_if_i_n_46),
        .Q(registers_i_n_6),
        .axi_arready_reg_0(s_axi_arready),
        .axi_awready_reg_0(s_axi_awready),
        .\axi_rdata_reg[0]_0 (registers_i_n_0),
        .\axi_rdata_reg[31]_0 (slv_reg_in_vld),
        .\axi_rdata_reg[31]_1 (slv_reg_in),
        .axi_rvalid_reg_0(registers_i_n_13),
        .axi_wready_reg_0(s_axi_wready),
        .control_wr_en(control_wr_en),
        .mon_clk(mon_clk),
        .mon_resetn(mon_resetn),
        .mon_resetn_0(sample_time_diff_reg),
        .p_1_in(p_1_in),
        .register_select(register_select),
        .\register_select_reg[1] (sample_en),
        .reset_sample_reg__0(reset_sample_reg__0),
        .s_axi_araddr(s_axi_araddr),
        .s_axi_arvalid(s_axi_arvalid),
        .s_axi_awaddr(s_axi_awaddr),
        .\s_axi_awaddr[3] (trace_control_wr_en),
        .\s_axi_awaddr[4] (slv_reg_addr),
        .\s_axi_awaddr[7] ({p_0_out[5:3],p_0_out[0]}),
        .s_axi_awvalid(s_axi_awvalid),
        .s_axi_bready(s_axi_bready),
        .s_axi_bvalid(s_axi_bvalid),
        .s_axi_rdata(s_axi_rdata),
        .s_axi_rvalid(s_axi_rvalid),
        .s_axi_wvalid(s_axi_wvalid),
        .\sample_data_reg[0] (acc_ctr_i_n_194),
        .\sample_data_reg[0]_0 (acc_ctr_i_n_226),
        .\sample_data_reg[0]_1 (acc_ctr_i_n_227),
        .\sample_data_reg[10] (acc_ctr_i_n_204),
        .\sample_data_reg[10]_0 (acc_ctr_i_n_246),
        .\sample_data_reg[10]_1 (acc_ctr_i_n_247),
        .\sample_data_reg[11] (acc_ctr_i_n_205),
        .\sample_data_reg[11]_0 (acc_ctr_i_n_248),
        .\sample_data_reg[11]_1 (acc_ctr_i_n_249),
        .\sample_data_reg[12] (acc_ctr_i_n_206),
        .\sample_data_reg[12]_0 (acc_ctr_i_n_250),
        .\sample_data_reg[12]_1 (acc_ctr_i_n_251),
        .\sample_data_reg[13] (acc_ctr_i_n_207),
        .\sample_data_reg[13]_0 (acc_ctr_i_n_252),
        .\sample_data_reg[13]_1 (acc_ctr_i_n_253),
        .\sample_data_reg[14] (acc_ctr_i_n_208),
        .\sample_data_reg[14]_0 (acc_ctr_i_n_254),
        .\sample_data_reg[14]_1 (acc_ctr_i_n_255),
        .\sample_data_reg[15] (acc_ctr_i_n_209),
        .\sample_data_reg[15]_0 (acc_ctr_i_n_256),
        .\sample_data_reg[15]_1 (acc_ctr_i_n_257),
        .\sample_data_reg[16] (acc_ctr_i_n_210),
        .\sample_data_reg[16]_0 (acc_ctr_i_n_258),
        .\sample_data_reg[16]_1 (acc_ctr_i_n_259),
        .\sample_data_reg[17] (acc_ctr_i_n_211),
        .\sample_data_reg[17]_0 (acc_ctr_i_n_260),
        .\sample_data_reg[17]_1 (acc_ctr_i_n_261),
        .\sample_data_reg[18] (acc_ctr_i_n_212),
        .\sample_data_reg[18]_0 (acc_ctr_i_n_262),
        .\sample_data_reg[18]_1 (acc_ctr_i_n_263),
        .\sample_data_reg[19] (acc_ctr_i_n_213),
        .\sample_data_reg[19]_0 (acc_ctr_i_n_264),
        .\sample_data_reg[19]_1 (acc_ctr_i_n_265),
        .\sample_data_reg[1] (acc_ctr_i_n_195),
        .\sample_data_reg[1]_0 (acc_ctr_i_n_228),
        .\sample_data_reg[1]_1 (acc_ctr_i_n_229),
        .\sample_data_reg[20] (acc_ctr_i_n_214),
        .\sample_data_reg[20]_0 (acc_ctr_i_n_266),
        .\sample_data_reg[20]_1 (acc_ctr_i_n_267),
        .\sample_data_reg[21] (acc_ctr_i_n_215),
        .\sample_data_reg[21]_0 (acc_ctr_i_n_268),
        .\sample_data_reg[21]_1 (acc_ctr_i_n_269),
        .\sample_data_reg[22] (acc_ctr_i_n_216),
        .\sample_data_reg[22]_0 (acc_ctr_i_n_270),
        .\sample_data_reg[22]_1 (acc_ctr_i_n_271),
        .\sample_data_reg[23] (acc_ctr_i_n_217),
        .\sample_data_reg[23]_0 (acc_ctr_i_n_272),
        .\sample_data_reg[23]_1 (acc_ctr_i_n_273),
        .\sample_data_reg[24] (acc_ctr_i_n_218),
        .\sample_data_reg[24]_0 (acc_ctr_i_n_274),
        .\sample_data_reg[24]_1 (acc_ctr_i_n_275),
        .\sample_data_reg[25] (acc_ctr_i_n_219),
        .\sample_data_reg[25]_0 (acc_ctr_i_n_276),
        .\sample_data_reg[25]_1 (acc_ctr_i_n_277),
        .\sample_data_reg[26] (acc_ctr_i_n_220),
        .\sample_data_reg[26]_0 (acc_ctr_i_n_278),
        .\sample_data_reg[26]_1 (acc_ctr_i_n_279),
        .\sample_data_reg[27] (acc_ctr_i_n_221),
        .\sample_data_reg[27]_0 (acc_ctr_i_n_280),
        .\sample_data_reg[27]_1 (acc_ctr_i_n_281),
        .\sample_data_reg[28] (acc_ctr_i_n_222),
        .\sample_data_reg[28]_0 (acc_ctr_i_n_282),
        .\sample_data_reg[28]_1 (acc_ctr_i_n_283),
        .\sample_data_reg[29] (acc_ctr_i_n_223),
        .\sample_data_reg[29]_0 (acc_ctr_i_n_284),
        .\sample_data_reg[29]_1 (acc_ctr_i_n_285),
        .\sample_data_reg[2] (acc_ctr_i_n_196),
        .\sample_data_reg[2]_0 (acc_ctr_i_n_230),
        .\sample_data_reg[2]_1 (acc_ctr_i_n_231),
        .\sample_data_reg[30] (acc_ctr_i_n_224),
        .\sample_data_reg[30]_0 (acc_ctr_i_n_286),
        .\sample_data_reg[30]_1 (acc_ctr_i_n_287),
        .\sample_data_reg[31] (acc_ctr_i_n_225),
        .\sample_data_reg[31]_0 ({data7,acc_ctr_i_n_98,acc_ctr_i_n_99,acc_ctr_i_n_100,acc_ctr_i_n_101,acc_ctr_i_n_102,acc_ctr_i_n_103,acc_ctr_i_n_104,acc_ctr_i_n_105,acc_ctr_i_n_106,acc_ctr_i_n_107,acc_ctr_i_n_108,acc_ctr_i_n_109,acc_ctr_i_n_110,acc_ctr_i_n_111,acc_ctr_i_n_112,acc_ctr_i_n_113,acc_ctr_i_n_114,acc_ctr_i_n_115,acc_ctr_i_n_116,acc_ctr_i_n_117,acc_ctr_i_n_118,acc_ctr_i_n_119,acc_ctr_i_n_120,acc_ctr_i_n_121,acc_ctr_i_n_122,acc_ctr_i_n_123,acc_ctr_i_n_124,acc_ctr_i_n_125,acc_ctr_i_n_126,acc_ctr_i_n_127,acc_ctr_i_n_128,acc_ctr_i_n_129}),
        .\sample_data_reg[31]_1 ({data8,acc_ctr_i_n_162,acc_ctr_i_n_163,acc_ctr_i_n_164,acc_ctr_i_n_165,acc_ctr_i_n_166,acc_ctr_i_n_167,acc_ctr_i_n_168,acc_ctr_i_n_169,acc_ctr_i_n_170,acc_ctr_i_n_171,acc_ctr_i_n_172,acc_ctr_i_n_173,acc_ctr_i_n_174,acc_ctr_i_n_175,acc_ctr_i_n_176,acc_ctr_i_n_177,acc_ctr_i_n_178,acc_ctr_i_n_179,acc_ctr_i_n_180,acc_ctr_i_n_181,acc_ctr_i_n_182,acc_ctr_i_n_183,acc_ctr_i_n_184,acc_ctr_i_n_185,acc_ctr_i_n_186,acc_ctr_i_n_187,acc_ctr_i_n_188,acc_ctr_i_n_189,acc_ctr_i_n_190,acc_ctr_i_n_191,acc_ctr_i_n_192,acc_ctr_i_n_193}),
        .\sample_data_reg[31]_2 (acc_ctr_i_n_288),
        .\sample_data_reg[31]_3 (acc_ctr_i_n_289),
        .\sample_data_reg[3] (acc_ctr_i_n_197),
        .\sample_data_reg[3]_0 (acc_ctr_i_n_232),
        .\sample_data_reg[3]_1 (acc_ctr_i_n_233),
        .\sample_data_reg[4] (acc_ctr_i_n_198),
        .\sample_data_reg[4]_0 (acc_ctr_i_n_234),
        .\sample_data_reg[4]_1 (acc_ctr_i_n_235),
        .\sample_data_reg[5] (acc_ctr_i_n_199),
        .\sample_data_reg[5]_0 (acc_ctr_i_n_236),
        .\sample_data_reg[5]_1 (acc_ctr_i_n_237),
        .\sample_data_reg[6] (acc_ctr_i_n_200),
        .\sample_data_reg[6]_0 (acc_ctr_i_n_238),
        .\sample_data_reg[6]_1 (acc_ctr_i_n_239),
        .\sample_data_reg[7] (acc_ctr_i_n_201),
        .\sample_data_reg[7]_0 (acc_ctr_i_n_240),
        .\sample_data_reg[7]_1 (acc_ctr_i_n_241),
        .\sample_data_reg[8] (acc_ctr_i_n_202),
        .\sample_data_reg[8]_0 (acc_ctr_i_n_242),
        .\sample_data_reg[8]_1 (acc_ctr_i_n_243),
        .\sample_data_reg[9] (acc_ctr_i_n_203),
        .\sample_data_reg[9]_0 (acc_ctr_i_n_244),
        .\sample_data_reg[9]_1 (acc_ctr_i_n_245),
        .sample_reg_rd_first(sample_reg_rd_first),
        .sample_reg_rd_first_reg(axi_lite_if_i_n_54),
        .slv_reg_addr_vld(slv_reg_addr_vld),
        .slv_reg_out_vld(slv_reg_out_vld));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_monitor_axilite mon_axilite_i
       (.Metrics_Cnt_En(Metrics_Cnt_En),
        .O({mon_axilite_i_n_7,mon_axilite_i_n_8,mon_axilite_i_n_9,mon_axilite_i_n_10,mon_axilite_i_n_11,mon_axilite_i_n_12,mon_axilite_i_n_13,mon_axilite_i_n_14}),
        .Q(trace_control),
        .SS(registers_i_n_0),
        .ap_continue_reg(ap_continue_reg),
        .ap_done_reg(ap_done_reg),
        .ap_start_reg_reg_0(_start_events),
        .ap_start_reg_reg_1({mon_axilite_i_n_23,mon_axilite_i_n_24,mon_axilite_i_n_25,mon_axilite_i_n_26,mon_axilite_i_n_27,mon_axilite_i_n_28,mon_axilite_i_n_29,mon_axilite_i_n_30}),
        .ap_start_reg_reg_2({mon_axilite_i_n_31,mon_axilite_i_n_32,mon_axilite_i_n_33,mon_axilite_i_n_34,mon_axilite_i_n_35,mon_axilite_i_n_36,mon_axilite_i_n_37,mon_axilite_i_n_38}),
        .ap_start_reg_reg_3({mon_axilite_i_n_39,mon_axilite_i_n_40,mon_axilite_i_n_41,mon_axilite_i_n_42,mon_axilite_i_n_43,mon_axilite_i_n_44,mon_axilite_i_n_45,mon_axilite_i_n_46}),
        .ap_start_reg_reg_4({mon_axilite_i_n_47,mon_axilite_i_n_48,mon_axilite_i_n_49,mon_axilite_i_n_50,mon_axilite_i_n_51,mon_axilite_i_n_52,mon_axilite_i_n_53,mon_axilite_i_n_54}),
        .ap_start_reg_reg_5({mon_axilite_i_n_55,mon_axilite_i_n_56,mon_axilite_i_n_57,mon_axilite_i_n_58,mon_axilite_i_n_59,mon_axilite_i_n_60,mon_axilite_i_n_61,mon_axilite_i_n_62}),
        .ap_start_reg_reg_6({mon_axilite_i_n_63,mon_axilite_i_n_64,mon_axilite_i_n_65,mon_axilite_i_n_66,mon_axilite_i_n_67,mon_axilite_i_n_68,mon_axilite_i_n_69,mon_axilite_i_n_70}),
        .dataflow_en(dataflow_en),
        .empty(\min_max_ctr_i/empty ),
        .ip_cur_tranx_reg(ip_cur_tranx_reg),
        .\ip_cur_tranx_reg[0] ({mon_axilite_i_n_15,mon_axilite_i_n_16,mon_axilite_i_n_17,mon_axilite_i_n_18,mon_axilite_i_n_19,mon_axilite_i_n_20,mon_axilite_i_n_21,mon_axilite_i_n_22}),
        .ip_exec_count0(ip_exec_count0),
        .mon_clk(mon_clk),
        .rd_en(\min_max_ctr_i/read ),
        .s_axi_araddr_mon(s_axi_araddr_mon),
        .s_axi_arready_mon(s_axi_arready_mon),
        .s_axi_arvalid_mon(s_axi_arvalid_mon),
        .s_axi_awaddr_mon(s_axi_awaddr_mon),
        .s_axi_awready_mon(s_axi_awready_mon),
        .s_axi_awvalid_mon(s_axi_awvalid_mon),
        .s_axi_rdata_mon(s_axi_rdata_mon),
        .s_axi_rready_mon(s_axi_rready_mon),
        .s_axi_rvalid_mon(s_axi_rvalid_mon),
        .s_axi_wdata_mon(s_axi_wdata_mon),
        .s_axi_wready_mon(s_axi_wready_mon),
        .s_axi_wstrb_mon(s_axi_wstrb_mon),
        .s_axi_wvalid_mon(s_axi_wvalid_mon),
        .src_in(_stop_events),
        .start_pulse(start_pulse));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_register_module registers_i
       (.CO(ip_idle),
        .D(axi_lite_if_i_n_44),
        .E(axi_lite_if_i_n_46),
        .Metrics_Cnt_En(Metrics_Cnt_En),
        .Q(registers_i_n_6),
        .RST_ACTIVE(RST_ACTIVE),
        .SS(registers_i_n_0),
        .control_wr_en(control_wr_en),
        .dataflow_en(dataflow_en),
        .ip_start_count0(ip_start_count0),
        .metrics_cnt_reset_reg_0(registers_i_n_8),
        .metrics_cnt_reset_reg_1(registers_i_n_9),
        .mon_clk(mon_clk),
        .mon_resetn(mon_resetn),
        .p_1_in(p_1_in),
        .\register_select_reg[1]_0 (register_select),
        .\register_select_reg[5]_0 ({p_0_out[5:3],p_0_out[0]}),
        .reset_on_sample_read_reg_0(registers_i_n_2),
        .reset_sample_reg__0(reset_sample_reg__0),
        .s_axi_rready(s_axi_rready),
        .s_axi_rvalid(s_axi_rvalid),
        .s_axi_wdata(s_axi_wdata),
        .sample_reg_rd_first(sample_reg_rd_first),
        .sample_reg_rd_first_reg_0(axi_lite_if_i_n_54),
        .\sample_time_diff_reg_reg[31]_0 (sample_time_diff_reg),
        .slv_reg_addr_vld(slv_reg_addr_vld),
        .\slv_reg_in_reg[31]_0 (slv_reg_in),
        .\slv_reg_in_reg[31]_1 (sample_data),
        .slv_reg_in_vld_reg_0(slv_reg_in_vld),
        .slv_reg_in_vld_reg_1(registers_i_n_13),
        .slv_reg_out_vld(slv_reg_out_vld),
        .start_pulse(start_pulse),
        .\trace_control_reg[0]_0 (trace_control),
        .\trace_control_reg[5]_0 (trace_control_wr_en));
  (* DEST_SYNC_FF = "4" *) 
  (* INIT_SYNC_FF = "0" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* SRC_INPUT_REG = "1" *) 
  (* VERSION = "0" *) 
  (* WIDTH = "4" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  (* XPM_MODULE = "TRUE" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__2 tr_cdc_start_0
       (.dest_clk(trace_clk),
        .dest_out(trace_start_events),
        .src_clk(mon_clk),
        .src_in({1'b0,1'b0,1'b0,_start_events}));
  (* DEST_SYNC_FF = "4" *) 
  (* INIT_SYNC_FF = "0" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* SRC_INPUT_REG = "1" *) 
  (* VERSION = "0" *) 
  (* WIDTH = "4" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  (* XPM_MODULE = "TRUE" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single tr_cdc_stop_0
       (.dest_clk(trace_clk),
        .dest_out(trace_stop_events),
        .src_clk(mon_clk),
        .src_in({1'b0,1'b0,1'b0,_stop_events}));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_timestamper trace_i
       (.dest_out(trace_start_events),
        .\event_i_buf_reg[52]_0 (trace_stop_events),
        .trace_clk(trace_clk),
        .trace_counter(trace_counter),
        .trace_counter_overflow(trace_counter_overflow),
        .trace_data(trace_data),
        .trace_read(trace_read),
        .trace_rst(trace_rst),
        .trace_valid(trace_valid));
endmodule

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_counter
   (Q,
    SR,
    reset_sample_reg__0,
    mon_resetn,
    E,
    mon_clk,
    D);
  output [31:0]Q;
  output [0:0]SR;
  input reset_sample_reg__0;
  input mon_resetn;
  input [0:0]E;
  input mon_clk;
  input [0:0]D;

  wire [31:1]Count_Out_i;
  wire Count_Out_i0_carry__0_n_0;
  wire Count_Out_i0_carry__0_n_1;
  wire Count_Out_i0_carry__0_n_2;
  wire Count_Out_i0_carry__0_n_3;
  wire Count_Out_i0_carry__0_n_4;
  wire Count_Out_i0_carry__0_n_5;
  wire Count_Out_i0_carry__0_n_6;
  wire Count_Out_i0_carry__0_n_7;
  wire Count_Out_i0_carry__1_n_0;
  wire Count_Out_i0_carry__1_n_1;
  wire Count_Out_i0_carry__1_n_2;
  wire Count_Out_i0_carry__1_n_3;
  wire Count_Out_i0_carry__1_n_4;
  wire Count_Out_i0_carry__1_n_5;
  wire Count_Out_i0_carry__1_n_6;
  wire Count_Out_i0_carry__1_n_7;
  wire Count_Out_i0_carry__2_n_2;
  wire Count_Out_i0_carry__2_n_3;
  wire Count_Out_i0_carry__2_n_4;
  wire Count_Out_i0_carry__2_n_5;
  wire Count_Out_i0_carry__2_n_6;
  wire Count_Out_i0_carry__2_n_7;
  wire Count_Out_i0_carry_n_0;
  wire Count_Out_i0_carry_n_1;
  wire Count_Out_i0_carry_n_2;
  wire Count_Out_i0_carry_n_3;
  wire Count_Out_i0_carry_n_4;
  wire Count_Out_i0_carry_n_5;
  wire Count_Out_i0_carry_n_6;
  wire Count_Out_i0_carry_n_7;
  wire \Count_Out_i[10]_i_1_n_0 ;
  wire \Count_Out_i[11]_i_1_n_0 ;
  wire \Count_Out_i[12]_i_1_n_0 ;
  wire \Count_Out_i[13]_i_1_n_0 ;
  wire \Count_Out_i[14]_i_1_n_0 ;
  wire \Count_Out_i[15]_i_1_n_0 ;
  wire \Count_Out_i[16]_i_1_n_0 ;
  wire \Count_Out_i[17]_i_1_n_0 ;
  wire \Count_Out_i[18]_i_1_n_0 ;
  wire \Count_Out_i[19]_i_1_n_0 ;
  wire \Count_Out_i[1]_i_1_n_0 ;
  wire \Count_Out_i[20]_i_1_n_0 ;
  wire \Count_Out_i[21]_i_1_n_0 ;
  wire \Count_Out_i[22]_i_1_n_0 ;
  wire \Count_Out_i[23]_i_1_n_0 ;
  wire \Count_Out_i[24]_i_1_n_0 ;
  wire \Count_Out_i[25]_i_1_n_0 ;
  wire \Count_Out_i[26]_i_1_n_0 ;
  wire \Count_Out_i[27]_i_1_n_0 ;
  wire \Count_Out_i[28]_i_1_n_0 ;
  wire \Count_Out_i[29]_i_1_n_0 ;
  wire \Count_Out_i[2]_i_1_n_0 ;
  wire \Count_Out_i[30]_i_1_n_0 ;
  wire \Count_Out_i[31]_i_2_n_0 ;
  wire \Count_Out_i[3]_i_1_n_0 ;
  wire \Count_Out_i[4]_i_1_n_0 ;
  wire \Count_Out_i[5]_i_1_n_0 ;
  wire \Count_Out_i[6]_i_1_n_0 ;
  wire \Count_Out_i[7]_i_1_n_0 ;
  wire \Count_Out_i[8]_i_1_n_0 ;
  wire \Count_Out_i[9]_i_1_n_0 ;
  wire [0:0]D;
  wire [0:0]E;
  wire [31:0]Q;
  wire [0:0]SR;
  wire mon_clk;
  wire mon_resetn;
  wire reset_sample_reg__0;
  wire [7:6]NLW_Count_Out_i0_carry__2_CO_UNCONNECTED;
  wire [7:7]NLW_Count_Out_i0_carry__2_O_UNCONNECTED;

  (* METHODOLOGY_DRC_VIOS = "{SYNTH-8 {cell *THIS*}}" *) 
  CARRY8 Count_Out_i0_carry
       (.CI(Q[0]),
        .CI_TOP(1'b0),
        .CO({Count_Out_i0_carry_n_0,Count_Out_i0_carry_n_1,Count_Out_i0_carry_n_2,Count_Out_i0_carry_n_3,Count_Out_i0_carry_n_4,Count_Out_i0_carry_n_5,Count_Out_i0_carry_n_6,Count_Out_i0_carry_n_7}),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O(Count_Out_i[8:1]),
        .S(Q[8:1]));
  (* METHODOLOGY_DRC_VIOS = "{SYNTH-8 {cell *THIS*}}" *) 
  CARRY8 Count_Out_i0_carry__0
       (.CI(Count_Out_i0_carry_n_0),
        .CI_TOP(1'b0),
        .CO({Count_Out_i0_carry__0_n_0,Count_Out_i0_carry__0_n_1,Count_Out_i0_carry__0_n_2,Count_Out_i0_carry__0_n_3,Count_Out_i0_carry__0_n_4,Count_Out_i0_carry__0_n_5,Count_Out_i0_carry__0_n_6,Count_Out_i0_carry__0_n_7}),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O(Count_Out_i[16:9]),
        .S(Q[16:9]));
  (* METHODOLOGY_DRC_VIOS = "{SYNTH-8 {cell *THIS*}}" *) 
  CARRY8 Count_Out_i0_carry__1
       (.CI(Count_Out_i0_carry__0_n_0),
        .CI_TOP(1'b0),
        .CO({Count_Out_i0_carry__1_n_0,Count_Out_i0_carry__1_n_1,Count_Out_i0_carry__1_n_2,Count_Out_i0_carry__1_n_3,Count_Out_i0_carry__1_n_4,Count_Out_i0_carry__1_n_5,Count_Out_i0_carry__1_n_6,Count_Out_i0_carry__1_n_7}),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O(Count_Out_i[24:17]),
        .S(Q[24:17]));
  (* METHODOLOGY_DRC_VIOS = "{SYNTH-8 {cell *THIS*}}" *) 
  CARRY8 Count_Out_i0_carry__2
       (.CI(Count_Out_i0_carry__1_n_0),
        .CI_TOP(1'b0),
        .CO({NLW_Count_Out_i0_carry__2_CO_UNCONNECTED[7:6],Count_Out_i0_carry__2_n_2,Count_Out_i0_carry__2_n_3,Count_Out_i0_carry__2_n_4,Count_Out_i0_carry__2_n_5,Count_Out_i0_carry__2_n_6,Count_Out_i0_carry__2_n_7}),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({NLW_Count_Out_i0_carry__2_O_UNCONNECTED[7],Count_Out_i[31:25]}),
        .S({1'b0,Q[31:25]}));
  (* SOFT_HLUTNM = "soft_lutpair35" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[10]_i_1 
       (.I0(Count_Out_i[10]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[10]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair36" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[11]_i_1 
       (.I0(Count_Out_i[11]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[11]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair36" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[12]_i_1 
       (.I0(Count_Out_i[12]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[12]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair37" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[13]_i_1 
       (.I0(Count_Out_i[13]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[13]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair37" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[14]_i_1 
       (.I0(Count_Out_i[14]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[14]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair38" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[15]_i_1 
       (.I0(Count_Out_i[15]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[15]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair38" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[16]_i_1 
       (.I0(Count_Out_i[16]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[16]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair39" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[17]_i_1 
       (.I0(Count_Out_i[17]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[17]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair39" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[18]_i_1 
       (.I0(Count_Out_i[18]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[18]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair40" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[19]_i_1 
       (.I0(Count_Out_i[19]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[19]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair31" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[1]_i_1 
       (.I0(Count_Out_i[1]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[1]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair40" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[20]_i_1 
       (.I0(Count_Out_i[20]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[20]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair41" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[21]_i_1 
       (.I0(Count_Out_i[21]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[21]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair41" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[22]_i_1 
       (.I0(Count_Out_i[22]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[22]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair42" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[23]_i_1 
       (.I0(Count_Out_i[23]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[23]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair42" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[24]_i_1 
       (.I0(Count_Out_i[24]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[24]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair43" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[25]_i_1 
       (.I0(Count_Out_i[25]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[25]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair43" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[26]_i_1 
       (.I0(Count_Out_i[26]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[26]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair44" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[27]_i_1 
       (.I0(Count_Out_i[27]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[27]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair44" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[28]_i_1 
       (.I0(Count_Out_i[28]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[28]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair45" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[29]_i_1 
       (.I0(Count_Out_i[29]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[29]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair31" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[2]_i_1 
       (.I0(Count_Out_i[2]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[2]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair45" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[30]_i_1 
       (.I0(Count_Out_i[30]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[30]_i_1_n_0 ));
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[31]_i_2 
       (.I0(Count_Out_i[31]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[31]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair32" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[3]_i_1 
       (.I0(Count_Out_i[3]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[3]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair32" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[4]_i_1 
       (.I0(Count_Out_i[4]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[4]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair33" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[5]_i_1 
       (.I0(Count_Out_i[5]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[5]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair33" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[6]_i_1 
       (.I0(Count_Out_i[6]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[6]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair34" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[7]_i_1 
       (.I0(Count_Out_i[7]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[7]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair34" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[8]_i_1 
       (.I0(Count_Out_i[8]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[8]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair35" *) 
  LUT2 #(
    .INIT(4'h2)) 
    \Count_Out_i[9]_i_1 
       (.I0(Count_Out_i[9]),
        .I1(reset_sample_reg__0),
        .O(\Count_Out_i[9]_i_1_n_0 ));
  FDRE \Count_Out_i_reg[0] 
       (.C(mon_clk),
        .CE(E),
        .D(D),
        .Q(Q[0]),
        .R(SR));
  FDRE \Count_Out_i_reg[10] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[10]_i_1_n_0 ),
        .Q(Q[10]),
        .R(SR));
  FDRE \Count_Out_i_reg[11] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[11]_i_1_n_0 ),
        .Q(Q[11]),
        .R(SR));
  FDRE \Count_Out_i_reg[12] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[12]_i_1_n_0 ),
        .Q(Q[12]),
        .R(SR));
  FDRE \Count_Out_i_reg[13] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[13]_i_1_n_0 ),
        .Q(Q[13]),
        .R(SR));
  FDRE \Count_Out_i_reg[14] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[14]_i_1_n_0 ),
        .Q(Q[14]),
        .R(SR));
  FDRE \Count_Out_i_reg[15] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[15]_i_1_n_0 ),
        .Q(Q[15]),
        .R(SR));
  FDRE \Count_Out_i_reg[16] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[16]_i_1_n_0 ),
        .Q(Q[16]),
        .R(SR));
  FDRE \Count_Out_i_reg[17] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[17]_i_1_n_0 ),
        .Q(Q[17]),
        .R(SR));
  FDRE \Count_Out_i_reg[18] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[18]_i_1_n_0 ),
        .Q(Q[18]),
        .R(SR));
  FDRE \Count_Out_i_reg[19] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[19]_i_1_n_0 ),
        .Q(Q[19]),
        .R(SR));
  FDRE \Count_Out_i_reg[1] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[1]_i_1_n_0 ),
        .Q(Q[1]),
        .R(SR));
  FDRE \Count_Out_i_reg[20] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[20]_i_1_n_0 ),
        .Q(Q[20]),
        .R(SR));
  FDRE \Count_Out_i_reg[21] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[21]_i_1_n_0 ),
        .Q(Q[21]),
        .R(SR));
  FDRE \Count_Out_i_reg[22] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[22]_i_1_n_0 ),
        .Q(Q[22]),
        .R(SR));
  FDRE \Count_Out_i_reg[23] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[23]_i_1_n_0 ),
        .Q(Q[23]),
        .R(SR));
  FDRE \Count_Out_i_reg[24] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[24]_i_1_n_0 ),
        .Q(Q[24]),
        .R(SR));
  FDRE \Count_Out_i_reg[25] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[25]_i_1_n_0 ),
        .Q(Q[25]),
        .R(SR));
  FDRE \Count_Out_i_reg[26] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[26]_i_1_n_0 ),
        .Q(Q[26]),
        .R(SR));
  FDRE \Count_Out_i_reg[27] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[27]_i_1_n_0 ),
        .Q(Q[27]),
        .R(SR));
  FDRE \Count_Out_i_reg[28] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[28]_i_1_n_0 ),
        .Q(Q[28]),
        .R(SR));
  FDRE \Count_Out_i_reg[29] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[29]_i_1_n_0 ),
        .Q(Q[29]),
        .R(SR));
  FDRE \Count_Out_i_reg[2] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[2]_i_1_n_0 ),
        .Q(Q[2]),
        .R(SR));
  FDRE \Count_Out_i_reg[30] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[30]_i_1_n_0 ),
        .Q(Q[30]),
        .R(SR));
  FDRE \Count_Out_i_reg[31] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[31]_i_2_n_0 ),
        .Q(Q[31]),
        .R(SR));
  FDRE \Count_Out_i_reg[3] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[3]_i_1_n_0 ),
        .Q(Q[3]),
        .R(SR));
  FDRE \Count_Out_i_reg[4] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[4]_i_1_n_0 ),
        .Q(Q[4]),
        .R(SR));
  FDRE \Count_Out_i_reg[5] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[5]_i_1_n_0 ),
        .Q(Q[5]),
        .R(SR));
  FDRE \Count_Out_i_reg[6] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[6]_i_1_n_0 ),
        .Q(Q[6]),
        .R(SR));
  FDRE \Count_Out_i_reg[7] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[7]_i_1_n_0 ),
        .Q(Q[7]),
        .R(SR));
  FDRE \Count_Out_i_reg[8] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[8]_i_1_n_0 ),
        .Q(Q[8]),
        .R(SR));
  FDRE \Count_Out_i_reg[9] 
       (.C(mon_clk),
        .CE(E),
        .D(\Count_Out_i[9]_i_1_n_0 ),
        .Q(Q[9]),
        .R(SR));
  LUT1 #(
    .INIT(2'h1)) 
    axi_awready_i_1
       (.I0(mon_resetn),
        .O(SR));
endmodule

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_event_mon
   (\sample_ctr_val_reg[32]_0 ,
    \sample_ctr_val_reg[33]_0 ,
    \sample_ctr_val_reg[34]_0 ,
    \sample_ctr_val_reg[35]_0 ,
    \sample_ctr_val_reg[36]_0 ,
    \sample_ctr_val_reg[37]_0 ,
    \sample_ctr_val_reg[38]_0 ,
    \sample_ctr_val_reg[39]_0 ,
    \sample_ctr_val_reg[40]_0 ,
    \sample_ctr_val_reg[41]_0 ,
    \sample_ctr_val_reg[42]_0 ,
    \sample_ctr_val_reg[43]_0 ,
    \sample_ctr_val_reg[44]_0 ,
    \sample_ctr_val_reg[45]_0 ,
    \sample_ctr_val_reg[46]_0 ,
    \sample_ctr_val_reg[47]_0 ,
    \sample_ctr_val_reg[48]_0 ,
    \sample_ctr_val_reg[49]_0 ,
    \sample_ctr_val_reg[50]_0 ,
    \sample_ctr_val_reg[51]_0 ,
    \sample_ctr_val_reg[52]_0 ,
    \sample_ctr_val_reg[53]_0 ,
    \sample_ctr_val_reg[54]_0 ,
    \sample_ctr_val_reg[55]_0 ,
    \sample_ctr_val_reg[56]_0 ,
    \sample_ctr_val_reg[57]_0 ,
    \sample_ctr_val_reg[58]_0 ,
    \sample_ctr_val_reg[59]_0 ,
    \sample_ctr_val_reg[60]_0 ,
    \sample_ctr_val_reg[61]_0 ,
    \sample_ctr_val_reg[62]_0 ,
    \sample_ctr_val_reg[63]_0 ,
    RST_ACTIVE,
    mon_clk,
    cnt_enabled_reg_0,
    data13,
    slv_reg_addr,
    \sample_data_reg[0] ,
    \sample_data_reg[1] ,
    \sample_data_reg[2] ,
    \sample_data_reg[3] ,
    \sample_data_reg[4] ,
    \sample_data_reg[5] ,
    \sample_data_reg[6] ,
    \sample_data_reg[7] ,
    \sample_data_reg[8] ,
    \sample_data_reg[9] ,
    \sample_data_reg[10] ,
    \sample_data_reg[11] ,
    \sample_data_reg[12] ,
    \sample_data_reg[13] ,
    \sample_data_reg[14] ,
    \sample_data_reg[15] ,
    \sample_data_reg[16] ,
    \sample_data_reg[17] ,
    \sample_data_reg[18] ,
    \sample_data_reg[19] ,
    \sample_data_reg[20] ,
    \sample_data_reg[21] ,
    \sample_data_reg[22] ,
    \sample_data_reg[23] ,
    \sample_data_reg[24] ,
    \sample_data_reg[25] ,
    \sample_data_reg[26] ,
    \sample_data_reg[27] ,
    \sample_data_reg[28] ,
    \sample_data_reg[29] ,
    \sample_data_reg[30] ,
    \sample_data_reg[31] ,
    E);
  output \sample_ctr_val_reg[32]_0 ;
  output \sample_ctr_val_reg[33]_0 ;
  output \sample_ctr_val_reg[34]_0 ;
  output \sample_ctr_val_reg[35]_0 ;
  output \sample_ctr_val_reg[36]_0 ;
  output \sample_ctr_val_reg[37]_0 ;
  output \sample_ctr_val_reg[38]_0 ;
  output \sample_ctr_val_reg[39]_0 ;
  output \sample_ctr_val_reg[40]_0 ;
  output \sample_ctr_val_reg[41]_0 ;
  output \sample_ctr_val_reg[42]_0 ;
  output \sample_ctr_val_reg[43]_0 ;
  output \sample_ctr_val_reg[44]_0 ;
  output \sample_ctr_val_reg[45]_0 ;
  output \sample_ctr_val_reg[46]_0 ;
  output \sample_ctr_val_reg[47]_0 ;
  output \sample_ctr_val_reg[48]_0 ;
  output \sample_ctr_val_reg[49]_0 ;
  output \sample_ctr_val_reg[50]_0 ;
  output \sample_ctr_val_reg[51]_0 ;
  output \sample_ctr_val_reg[52]_0 ;
  output \sample_ctr_val_reg[53]_0 ;
  output \sample_ctr_val_reg[54]_0 ;
  output \sample_ctr_val_reg[55]_0 ;
  output \sample_ctr_val_reg[56]_0 ;
  output \sample_ctr_val_reg[57]_0 ;
  output \sample_ctr_val_reg[58]_0 ;
  output \sample_ctr_val_reg[59]_0 ;
  output \sample_ctr_val_reg[60]_0 ;
  output \sample_ctr_val_reg[61]_0 ;
  output \sample_ctr_val_reg[62]_0 ;
  output \sample_ctr_val_reg[63]_0 ;
  input RST_ACTIVE;
  input mon_clk;
  input cnt_enabled_reg_0;
  input [31:0]data13;
  input [1:0]slv_reg_addr;
  input \sample_data_reg[0] ;
  input \sample_data_reg[1] ;
  input \sample_data_reg[2] ;
  input \sample_data_reg[3] ;
  input \sample_data_reg[4] ;
  input \sample_data_reg[5] ;
  input \sample_data_reg[6] ;
  input \sample_data_reg[7] ;
  input \sample_data_reg[8] ;
  input \sample_data_reg[9] ;
  input \sample_data_reg[10] ;
  input \sample_data_reg[11] ;
  input \sample_data_reg[12] ;
  input \sample_data_reg[13] ;
  input \sample_data_reg[14] ;
  input \sample_data_reg[15] ;
  input \sample_data_reg[16] ;
  input \sample_data_reg[17] ;
  input \sample_data_reg[18] ;
  input \sample_data_reg[19] ;
  input \sample_data_reg[20] ;
  input \sample_data_reg[21] ;
  input \sample_data_reg[22] ;
  input \sample_data_reg[23] ;
  input \sample_data_reg[24] ;
  input \sample_data_reg[25] ;
  input \sample_data_reg[26] ;
  input \sample_data_reg[27] ;
  input \sample_data_reg[28] ;
  input \sample_data_reg[29] ;
  input \sample_data_reg[30] ;
  input \sample_data_reg[31] ;
  input [0:0]E;

  wire [0:0]E;
  wire RST_ACTIVE;
  wire cnt_enabled;
  wire cnt_enabled_reg_0;
  wire [31:0]data11;
  wire [31:0]data13;
  wire \event_ctr_val[0]_i_2_n_0 ;
  wire [63:0]event_ctr_val_reg;
  wire \event_ctr_val_reg[0]_i_1_n_0 ;
  wire \event_ctr_val_reg[0]_i_1_n_1 ;
  wire \event_ctr_val_reg[0]_i_1_n_10 ;
  wire \event_ctr_val_reg[0]_i_1_n_11 ;
  wire \event_ctr_val_reg[0]_i_1_n_12 ;
  wire \event_ctr_val_reg[0]_i_1_n_13 ;
  wire \event_ctr_val_reg[0]_i_1_n_14 ;
  wire \event_ctr_val_reg[0]_i_1_n_15 ;
  wire \event_ctr_val_reg[0]_i_1_n_2 ;
  wire \event_ctr_val_reg[0]_i_1_n_3 ;
  wire \event_ctr_val_reg[0]_i_1_n_4 ;
  wire \event_ctr_val_reg[0]_i_1_n_5 ;
  wire \event_ctr_val_reg[0]_i_1_n_6 ;
  wire \event_ctr_val_reg[0]_i_1_n_7 ;
  wire \event_ctr_val_reg[0]_i_1_n_8 ;
  wire \event_ctr_val_reg[0]_i_1_n_9 ;
  wire \event_ctr_val_reg[16]_i_1_n_0 ;
  wire \event_ctr_val_reg[16]_i_1_n_1 ;
  wire \event_ctr_val_reg[16]_i_1_n_10 ;
  wire \event_ctr_val_reg[16]_i_1_n_11 ;
  wire \event_ctr_val_reg[16]_i_1_n_12 ;
  wire \event_ctr_val_reg[16]_i_1_n_13 ;
  wire \event_ctr_val_reg[16]_i_1_n_14 ;
  wire \event_ctr_val_reg[16]_i_1_n_15 ;
  wire \event_ctr_val_reg[16]_i_1_n_2 ;
  wire \event_ctr_val_reg[16]_i_1_n_3 ;
  wire \event_ctr_val_reg[16]_i_1_n_4 ;
  wire \event_ctr_val_reg[16]_i_1_n_5 ;
  wire \event_ctr_val_reg[16]_i_1_n_6 ;
  wire \event_ctr_val_reg[16]_i_1_n_7 ;
  wire \event_ctr_val_reg[16]_i_1_n_8 ;
  wire \event_ctr_val_reg[16]_i_1_n_9 ;
  wire \event_ctr_val_reg[24]_i_1_n_0 ;
  wire \event_ctr_val_reg[24]_i_1_n_1 ;
  wire \event_ctr_val_reg[24]_i_1_n_10 ;
  wire \event_ctr_val_reg[24]_i_1_n_11 ;
  wire \event_ctr_val_reg[24]_i_1_n_12 ;
  wire \event_ctr_val_reg[24]_i_1_n_13 ;
  wire \event_ctr_val_reg[24]_i_1_n_14 ;
  wire \event_ctr_val_reg[24]_i_1_n_15 ;
  wire \event_ctr_val_reg[24]_i_1_n_2 ;
  wire \event_ctr_val_reg[24]_i_1_n_3 ;
  wire \event_ctr_val_reg[24]_i_1_n_4 ;
  wire \event_ctr_val_reg[24]_i_1_n_5 ;
  wire \event_ctr_val_reg[24]_i_1_n_6 ;
  wire \event_ctr_val_reg[24]_i_1_n_7 ;
  wire \event_ctr_val_reg[24]_i_1_n_8 ;
  wire \event_ctr_val_reg[24]_i_1_n_9 ;
  wire \event_ctr_val_reg[32]_i_1_n_0 ;
  wire \event_ctr_val_reg[32]_i_1_n_1 ;
  wire \event_ctr_val_reg[32]_i_1_n_10 ;
  wire \event_ctr_val_reg[32]_i_1_n_11 ;
  wire \event_ctr_val_reg[32]_i_1_n_12 ;
  wire \event_ctr_val_reg[32]_i_1_n_13 ;
  wire \event_ctr_val_reg[32]_i_1_n_14 ;
  wire \event_ctr_val_reg[32]_i_1_n_15 ;
  wire \event_ctr_val_reg[32]_i_1_n_2 ;
  wire \event_ctr_val_reg[32]_i_1_n_3 ;
  wire \event_ctr_val_reg[32]_i_1_n_4 ;
  wire \event_ctr_val_reg[32]_i_1_n_5 ;
  wire \event_ctr_val_reg[32]_i_1_n_6 ;
  wire \event_ctr_val_reg[32]_i_1_n_7 ;
  wire \event_ctr_val_reg[32]_i_1_n_8 ;
  wire \event_ctr_val_reg[32]_i_1_n_9 ;
  wire \event_ctr_val_reg[40]_i_1_n_0 ;
  wire \event_ctr_val_reg[40]_i_1_n_1 ;
  wire \event_ctr_val_reg[40]_i_1_n_10 ;
  wire \event_ctr_val_reg[40]_i_1_n_11 ;
  wire \event_ctr_val_reg[40]_i_1_n_12 ;
  wire \event_ctr_val_reg[40]_i_1_n_13 ;
  wire \event_ctr_val_reg[40]_i_1_n_14 ;
  wire \event_ctr_val_reg[40]_i_1_n_15 ;
  wire \event_ctr_val_reg[40]_i_1_n_2 ;
  wire \event_ctr_val_reg[40]_i_1_n_3 ;
  wire \event_ctr_val_reg[40]_i_1_n_4 ;
  wire \event_ctr_val_reg[40]_i_1_n_5 ;
  wire \event_ctr_val_reg[40]_i_1_n_6 ;
  wire \event_ctr_val_reg[40]_i_1_n_7 ;
  wire \event_ctr_val_reg[40]_i_1_n_8 ;
  wire \event_ctr_val_reg[40]_i_1_n_9 ;
  wire \event_ctr_val_reg[48]_i_1_n_0 ;
  wire \event_ctr_val_reg[48]_i_1_n_1 ;
  wire \event_ctr_val_reg[48]_i_1_n_10 ;
  wire \event_ctr_val_reg[48]_i_1_n_11 ;
  wire \event_ctr_val_reg[48]_i_1_n_12 ;
  wire \event_ctr_val_reg[48]_i_1_n_13 ;
  wire \event_ctr_val_reg[48]_i_1_n_14 ;
  wire \event_ctr_val_reg[48]_i_1_n_15 ;
  wire \event_ctr_val_reg[48]_i_1_n_2 ;
  wire \event_ctr_val_reg[48]_i_1_n_3 ;
  wire \event_ctr_val_reg[48]_i_1_n_4 ;
  wire \event_ctr_val_reg[48]_i_1_n_5 ;
  wire \event_ctr_val_reg[48]_i_1_n_6 ;
  wire \event_ctr_val_reg[48]_i_1_n_7 ;
  wire \event_ctr_val_reg[48]_i_1_n_8 ;
  wire \event_ctr_val_reg[48]_i_1_n_9 ;
  wire \event_ctr_val_reg[56]_i_1_n_1 ;
  wire \event_ctr_val_reg[56]_i_1_n_10 ;
  wire \event_ctr_val_reg[56]_i_1_n_11 ;
  wire \event_ctr_val_reg[56]_i_1_n_12 ;
  wire \event_ctr_val_reg[56]_i_1_n_13 ;
  wire \event_ctr_val_reg[56]_i_1_n_14 ;
  wire \event_ctr_val_reg[56]_i_1_n_15 ;
  wire \event_ctr_val_reg[56]_i_1_n_2 ;
  wire \event_ctr_val_reg[56]_i_1_n_3 ;
  wire \event_ctr_val_reg[56]_i_1_n_4 ;
  wire \event_ctr_val_reg[56]_i_1_n_5 ;
  wire \event_ctr_val_reg[56]_i_1_n_6 ;
  wire \event_ctr_val_reg[56]_i_1_n_7 ;
  wire \event_ctr_val_reg[56]_i_1_n_8 ;
  wire \event_ctr_val_reg[56]_i_1_n_9 ;
  wire \event_ctr_val_reg[8]_i_1_n_0 ;
  wire \event_ctr_val_reg[8]_i_1_n_1 ;
  wire \event_ctr_val_reg[8]_i_1_n_10 ;
  wire \event_ctr_val_reg[8]_i_1_n_11 ;
  wire \event_ctr_val_reg[8]_i_1_n_12 ;
  wire \event_ctr_val_reg[8]_i_1_n_13 ;
  wire \event_ctr_val_reg[8]_i_1_n_14 ;
  wire \event_ctr_val_reg[8]_i_1_n_15 ;
  wire \event_ctr_val_reg[8]_i_1_n_2 ;
  wire \event_ctr_val_reg[8]_i_1_n_3 ;
  wire \event_ctr_val_reg[8]_i_1_n_4 ;
  wire \event_ctr_val_reg[8]_i_1_n_5 ;
  wire \event_ctr_val_reg[8]_i_1_n_6 ;
  wire \event_ctr_val_reg[8]_i_1_n_7 ;
  wire \event_ctr_val_reg[8]_i_1_n_8 ;
  wire \event_ctr_val_reg[8]_i_1_n_9 ;
  wire mon_clk;
  wire \sample_ctr_val_reg[32]_0 ;
  wire \sample_ctr_val_reg[33]_0 ;
  wire \sample_ctr_val_reg[34]_0 ;
  wire \sample_ctr_val_reg[35]_0 ;
  wire \sample_ctr_val_reg[36]_0 ;
  wire \sample_ctr_val_reg[37]_0 ;
  wire \sample_ctr_val_reg[38]_0 ;
  wire \sample_ctr_val_reg[39]_0 ;
  wire \sample_ctr_val_reg[40]_0 ;
  wire \sample_ctr_val_reg[41]_0 ;
  wire \sample_ctr_val_reg[42]_0 ;
  wire \sample_ctr_val_reg[43]_0 ;
  wire \sample_ctr_val_reg[44]_0 ;
  wire \sample_ctr_val_reg[45]_0 ;
  wire \sample_ctr_val_reg[46]_0 ;
  wire \sample_ctr_val_reg[47]_0 ;
  wire \sample_ctr_val_reg[48]_0 ;
  wire \sample_ctr_val_reg[49]_0 ;
  wire \sample_ctr_val_reg[50]_0 ;
  wire \sample_ctr_val_reg[51]_0 ;
  wire \sample_ctr_val_reg[52]_0 ;
  wire \sample_ctr_val_reg[53]_0 ;
  wire \sample_ctr_val_reg[54]_0 ;
  wire \sample_ctr_val_reg[55]_0 ;
  wire \sample_ctr_val_reg[56]_0 ;
  wire \sample_ctr_val_reg[57]_0 ;
  wire \sample_ctr_val_reg[58]_0 ;
  wire \sample_ctr_val_reg[59]_0 ;
  wire \sample_ctr_val_reg[60]_0 ;
  wire \sample_ctr_val_reg[61]_0 ;
  wire \sample_ctr_val_reg[62]_0 ;
  wire \sample_ctr_val_reg[63]_0 ;
  wire \sample_ctr_val_reg_n_0_[0] ;
  wire \sample_ctr_val_reg_n_0_[10] ;
  wire \sample_ctr_val_reg_n_0_[11] ;
  wire \sample_ctr_val_reg_n_0_[12] ;
  wire \sample_ctr_val_reg_n_0_[13] ;
  wire \sample_ctr_val_reg_n_0_[14] ;
  wire \sample_ctr_val_reg_n_0_[15] ;
  wire \sample_ctr_val_reg_n_0_[16] ;
  wire \sample_ctr_val_reg_n_0_[17] ;
  wire \sample_ctr_val_reg_n_0_[18] ;
  wire \sample_ctr_val_reg_n_0_[19] ;
  wire \sample_ctr_val_reg_n_0_[1] ;
  wire \sample_ctr_val_reg_n_0_[20] ;
  wire \sample_ctr_val_reg_n_0_[21] ;
  wire \sample_ctr_val_reg_n_0_[22] ;
  wire \sample_ctr_val_reg_n_0_[23] ;
  wire \sample_ctr_val_reg_n_0_[24] ;
  wire \sample_ctr_val_reg_n_0_[25] ;
  wire \sample_ctr_val_reg_n_0_[26] ;
  wire \sample_ctr_val_reg_n_0_[27] ;
  wire \sample_ctr_val_reg_n_0_[28] ;
  wire \sample_ctr_val_reg_n_0_[29] ;
  wire \sample_ctr_val_reg_n_0_[2] ;
  wire \sample_ctr_val_reg_n_0_[30] ;
  wire \sample_ctr_val_reg_n_0_[31] ;
  wire \sample_ctr_val_reg_n_0_[3] ;
  wire \sample_ctr_val_reg_n_0_[4] ;
  wire \sample_ctr_val_reg_n_0_[5] ;
  wire \sample_ctr_val_reg_n_0_[6] ;
  wire \sample_ctr_val_reg_n_0_[7] ;
  wire \sample_ctr_val_reg_n_0_[8] ;
  wire \sample_ctr_val_reg_n_0_[9] ;
  wire \sample_data_reg[0] ;
  wire \sample_data_reg[10] ;
  wire \sample_data_reg[11] ;
  wire \sample_data_reg[12] ;
  wire \sample_data_reg[13] ;
  wire \sample_data_reg[14] ;
  wire \sample_data_reg[15] ;
  wire \sample_data_reg[16] ;
  wire \sample_data_reg[17] ;
  wire \sample_data_reg[18] ;
  wire \sample_data_reg[19] ;
  wire \sample_data_reg[1] ;
  wire \sample_data_reg[20] ;
  wire \sample_data_reg[21] ;
  wire \sample_data_reg[22] ;
  wire \sample_data_reg[23] ;
  wire \sample_data_reg[24] ;
  wire \sample_data_reg[25] ;
  wire \sample_data_reg[26] ;
  wire \sample_data_reg[27] ;
  wire \sample_data_reg[28] ;
  wire \sample_data_reg[29] ;
  wire \sample_data_reg[2] ;
  wire \sample_data_reg[30] ;
  wire \sample_data_reg[31] ;
  wire \sample_data_reg[3] ;
  wire \sample_data_reg[4] ;
  wire \sample_data_reg[5] ;
  wire \sample_data_reg[6] ;
  wire \sample_data_reg[7] ;
  wire \sample_data_reg[8] ;
  wire \sample_data_reg[9] ;
  wire [1:0]slv_reg_addr;
  wire [7:7]\NLW_event_ctr_val_reg[56]_i_1_CO_UNCONNECTED ;

  FDRE #(
    .INIT(1'b0)) 
    cnt_enabled_reg
       (.C(mon_clk),
        .CE(1'b1),
        .D(cnt_enabled_reg_0),
        .Q(cnt_enabled),
        .R(1'b0));
  LUT1 #(
    .INIT(2'h1)) 
    \event_ctr_val[0]_i_2 
       (.I0(event_ctr_val_reg[0]),
        .O(\event_ctr_val[0]_i_2_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[0] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[0]_i_1_n_15 ),
        .Q(event_ctr_val_reg[0]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \event_ctr_val_reg[0]_i_1 
       (.CI(1'b0),
        .CI_TOP(1'b0),
        .CO({\event_ctr_val_reg[0]_i_1_n_0 ,\event_ctr_val_reg[0]_i_1_n_1 ,\event_ctr_val_reg[0]_i_1_n_2 ,\event_ctr_val_reg[0]_i_1_n_3 ,\event_ctr_val_reg[0]_i_1_n_4 ,\event_ctr_val_reg[0]_i_1_n_5 ,\event_ctr_val_reg[0]_i_1_n_6 ,\event_ctr_val_reg[0]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b1}),
        .O({\event_ctr_val_reg[0]_i_1_n_8 ,\event_ctr_val_reg[0]_i_1_n_9 ,\event_ctr_val_reg[0]_i_1_n_10 ,\event_ctr_val_reg[0]_i_1_n_11 ,\event_ctr_val_reg[0]_i_1_n_12 ,\event_ctr_val_reg[0]_i_1_n_13 ,\event_ctr_val_reg[0]_i_1_n_14 ,\event_ctr_val_reg[0]_i_1_n_15 }),
        .S({event_ctr_val_reg[7:1],\event_ctr_val[0]_i_2_n_0 }));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[10] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[8]_i_1_n_13 ),
        .Q(event_ctr_val_reg[10]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[11] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[8]_i_1_n_12 ),
        .Q(event_ctr_val_reg[11]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[12] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[8]_i_1_n_11 ),
        .Q(event_ctr_val_reg[12]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[13] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[8]_i_1_n_10 ),
        .Q(event_ctr_val_reg[13]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[14] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[8]_i_1_n_9 ),
        .Q(event_ctr_val_reg[14]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[15] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[8]_i_1_n_8 ),
        .Q(event_ctr_val_reg[15]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[16] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[16]_i_1_n_15 ),
        .Q(event_ctr_val_reg[16]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \event_ctr_val_reg[16]_i_1 
       (.CI(\event_ctr_val_reg[8]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\event_ctr_val_reg[16]_i_1_n_0 ,\event_ctr_val_reg[16]_i_1_n_1 ,\event_ctr_val_reg[16]_i_1_n_2 ,\event_ctr_val_reg[16]_i_1_n_3 ,\event_ctr_val_reg[16]_i_1_n_4 ,\event_ctr_val_reg[16]_i_1_n_5 ,\event_ctr_val_reg[16]_i_1_n_6 ,\event_ctr_val_reg[16]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\event_ctr_val_reg[16]_i_1_n_8 ,\event_ctr_val_reg[16]_i_1_n_9 ,\event_ctr_val_reg[16]_i_1_n_10 ,\event_ctr_val_reg[16]_i_1_n_11 ,\event_ctr_val_reg[16]_i_1_n_12 ,\event_ctr_val_reg[16]_i_1_n_13 ,\event_ctr_val_reg[16]_i_1_n_14 ,\event_ctr_val_reg[16]_i_1_n_15 }),
        .S(event_ctr_val_reg[23:16]));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[17] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[16]_i_1_n_14 ),
        .Q(event_ctr_val_reg[17]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[18] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[16]_i_1_n_13 ),
        .Q(event_ctr_val_reg[18]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[19] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[16]_i_1_n_12 ),
        .Q(event_ctr_val_reg[19]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[1] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[0]_i_1_n_14 ),
        .Q(event_ctr_val_reg[1]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[20] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[16]_i_1_n_11 ),
        .Q(event_ctr_val_reg[20]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[21] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[16]_i_1_n_10 ),
        .Q(event_ctr_val_reg[21]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[22] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[16]_i_1_n_9 ),
        .Q(event_ctr_val_reg[22]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[23] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[16]_i_1_n_8 ),
        .Q(event_ctr_val_reg[23]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[24] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[24]_i_1_n_15 ),
        .Q(event_ctr_val_reg[24]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \event_ctr_val_reg[24]_i_1 
       (.CI(\event_ctr_val_reg[16]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\event_ctr_val_reg[24]_i_1_n_0 ,\event_ctr_val_reg[24]_i_1_n_1 ,\event_ctr_val_reg[24]_i_1_n_2 ,\event_ctr_val_reg[24]_i_1_n_3 ,\event_ctr_val_reg[24]_i_1_n_4 ,\event_ctr_val_reg[24]_i_1_n_5 ,\event_ctr_val_reg[24]_i_1_n_6 ,\event_ctr_val_reg[24]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\event_ctr_val_reg[24]_i_1_n_8 ,\event_ctr_val_reg[24]_i_1_n_9 ,\event_ctr_val_reg[24]_i_1_n_10 ,\event_ctr_val_reg[24]_i_1_n_11 ,\event_ctr_val_reg[24]_i_1_n_12 ,\event_ctr_val_reg[24]_i_1_n_13 ,\event_ctr_val_reg[24]_i_1_n_14 ,\event_ctr_val_reg[24]_i_1_n_15 }),
        .S(event_ctr_val_reg[31:24]));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[25] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[24]_i_1_n_14 ),
        .Q(event_ctr_val_reg[25]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[26] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[24]_i_1_n_13 ),
        .Q(event_ctr_val_reg[26]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[27] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[24]_i_1_n_12 ),
        .Q(event_ctr_val_reg[27]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[28] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[24]_i_1_n_11 ),
        .Q(event_ctr_val_reg[28]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[29] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[24]_i_1_n_10 ),
        .Q(event_ctr_val_reg[29]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[2] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[0]_i_1_n_13 ),
        .Q(event_ctr_val_reg[2]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[30] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[24]_i_1_n_9 ),
        .Q(event_ctr_val_reg[30]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[31] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[24]_i_1_n_8 ),
        .Q(event_ctr_val_reg[31]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[32] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[32]_i_1_n_15 ),
        .Q(event_ctr_val_reg[32]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \event_ctr_val_reg[32]_i_1 
       (.CI(\event_ctr_val_reg[24]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\event_ctr_val_reg[32]_i_1_n_0 ,\event_ctr_val_reg[32]_i_1_n_1 ,\event_ctr_val_reg[32]_i_1_n_2 ,\event_ctr_val_reg[32]_i_1_n_3 ,\event_ctr_val_reg[32]_i_1_n_4 ,\event_ctr_val_reg[32]_i_1_n_5 ,\event_ctr_val_reg[32]_i_1_n_6 ,\event_ctr_val_reg[32]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\event_ctr_val_reg[32]_i_1_n_8 ,\event_ctr_val_reg[32]_i_1_n_9 ,\event_ctr_val_reg[32]_i_1_n_10 ,\event_ctr_val_reg[32]_i_1_n_11 ,\event_ctr_val_reg[32]_i_1_n_12 ,\event_ctr_val_reg[32]_i_1_n_13 ,\event_ctr_val_reg[32]_i_1_n_14 ,\event_ctr_val_reg[32]_i_1_n_15 }),
        .S(event_ctr_val_reg[39:32]));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[33] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[32]_i_1_n_14 ),
        .Q(event_ctr_val_reg[33]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[34] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[32]_i_1_n_13 ),
        .Q(event_ctr_val_reg[34]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[35] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[32]_i_1_n_12 ),
        .Q(event_ctr_val_reg[35]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[36] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[32]_i_1_n_11 ),
        .Q(event_ctr_val_reg[36]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[37] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[32]_i_1_n_10 ),
        .Q(event_ctr_val_reg[37]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[38] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[32]_i_1_n_9 ),
        .Q(event_ctr_val_reg[38]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[39] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[32]_i_1_n_8 ),
        .Q(event_ctr_val_reg[39]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[3] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[0]_i_1_n_12 ),
        .Q(event_ctr_val_reg[3]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[40] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[40]_i_1_n_15 ),
        .Q(event_ctr_val_reg[40]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \event_ctr_val_reg[40]_i_1 
       (.CI(\event_ctr_val_reg[32]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\event_ctr_val_reg[40]_i_1_n_0 ,\event_ctr_val_reg[40]_i_1_n_1 ,\event_ctr_val_reg[40]_i_1_n_2 ,\event_ctr_val_reg[40]_i_1_n_3 ,\event_ctr_val_reg[40]_i_1_n_4 ,\event_ctr_val_reg[40]_i_1_n_5 ,\event_ctr_val_reg[40]_i_1_n_6 ,\event_ctr_val_reg[40]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\event_ctr_val_reg[40]_i_1_n_8 ,\event_ctr_val_reg[40]_i_1_n_9 ,\event_ctr_val_reg[40]_i_1_n_10 ,\event_ctr_val_reg[40]_i_1_n_11 ,\event_ctr_val_reg[40]_i_1_n_12 ,\event_ctr_val_reg[40]_i_1_n_13 ,\event_ctr_val_reg[40]_i_1_n_14 ,\event_ctr_val_reg[40]_i_1_n_15 }),
        .S(event_ctr_val_reg[47:40]));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[41] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[40]_i_1_n_14 ),
        .Q(event_ctr_val_reg[41]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[42] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[40]_i_1_n_13 ),
        .Q(event_ctr_val_reg[42]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[43] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[40]_i_1_n_12 ),
        .Q(event_ctr_val_reg[43]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[44] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[40]_i_1_n_11 ),
        .Q(event_ctr_val_reg[44]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[45] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[40]_i_1_n_10 ),
        .Q(event_ctr_val_reg[45]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[46] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[40]_i_1_n_9 ),
        .Q(event_ctr_val_reg[46]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[47] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[40]_i_1_n_8 ),
        .Q(event_ctr_val_reg[47]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[48] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[48]_i_1_n_15 ),
        .Q(event_ctr_val_reg[48]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \event_ctr_val_reg[48]_i_1 
       (.CI(\event_ctr_val_reg[40]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\event_ctr_val_reg[48]_i_1_n_0 ,\event_ctr_val_reg[48]_i_1_n_1 ,\event_ctr_val_reg[48]_i_1_n_2 ,\event_ctr_val_reg[48]_i_1_n_3 ,\event_ctr_val_reg[48]_i_1_n_4 ,\event_ctr_val_reg[48]_i_1_n_5 ,\event_ctr_val_reg[48]_i_1_n_6 ,\event_ctr_val_reg[48]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\event_ctr_val_reg[48]_i_1_n_8 ,\event_ctr_val_reg[48]_i_1_n_9 ,\event_ctr_val_reg[48]_i_1_n_10 ,\event_ctr_val_reg[48]_i_1_n_11 ,\event_ctr_val_reg[48]_i_1_n_12 ,\event_ctr_val_reg[48]_i_1_n_13 ,\event_ctr_val_reg[48]_i_1_n_14 ,\event_ctr_val_reg[48]_i_1_n_15 }),
        .S(event_ctr_val_reg[55:48]));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[49] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[48]_i_1_n_14 ),
        .Q(event_ctr_val_reg[49]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[4] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[0]_i_1_n_11 ),
        .Q(event_ctr_val_reg[4]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[50] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[48]_i_1_n_13 ),
        .Q(event_ctr_val_reg[50]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[51] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[48]_i_1_n_12 ),
        .Q(event_ctr_val_reg[51]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[52] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[48]_i_1_n_11 ),
        .Q(event_ctr_val_reg[52]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[53] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[48]_i_1_n_10 ),
        .Q(event_ctr_val_reg[53]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[54] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[48]_i_1_n_9 ),
        .Q(event_ctr_val_reg[54]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[55] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[48]_i_1_n_8 ),
        .Q(event_ctr_val_reg[55]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[56] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[56]_i_1_n_15 ),
        .Q(event_ctr_val_reg[56]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \event_ctr_val_reg[56]_i_1 
       (.CI(\event_ctr_val_reg[48]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\NLW_event_ctr_val_reg[56]_i_1_CO_UNCONNECTED [7],\event_ctr_val_reg[56]_i_1_n_1 ,\event_ctr_val_reg[56]_i_1_n_2 ,\event_ctr_val_reg[56]_i_1_n_3 ,\event_ctr_val_reg[56]_i_1_n_4 ,\event_ctr_val_reg[56]_i_1_n_5 ,\event_ctr_val_reg[56]_i_1_n_6 ,\event_ctr_val_reg[56]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\event_ctr_val_reg[56]_i_1_n_8 ,\event_ctr_val_reg[56]_i_1_n_9 ,\event_ctr_val_reg[56]_i_1_n_10 ,\event_ctr_val_reg[56]_i_1_n_11 ,\event_ctr_val_reg[56]_i_1_n_12 ,\event_ctr_val_reg[56]_i_1_n_13 ,\event_ctr_val_reg[56]_i_1_n_14 ,\event_ctr_val_reg[56]_i_1_n_15 }),
        .S(event_ctr_val_reg[63:56]));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[57] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[56]_i_1_n_14 ),
        .Q(event_ctr_val_reg[57]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[58] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[56]_i_1_n_13 ),
        .Q(event_ctr_val_reg[58]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[59] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[56]_i_1_n_12 ),
        .Q(event_ctr_val_reg[59]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[5] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[0]_i_1_n_10 ),
        .Q(event_ctr_val_reg[5]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[60] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[56]_i_1_n_11 ),
        .Q(event_ctr_val_reg[60]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[61] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[56]_i_1_n_10 ),
        .Q(event_ctr_val_reg[61]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[62] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[56]_i_1_n_9 ),
        .Q(event_ctr_val_reg[62]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[63] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[56]_i_1_n_8 ),
        .Q(event_ctr_val_reg[63]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[6] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[0]_i_1_n_9 ),
        .Q(event_ctr_val_reg[6]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[7] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[0]_i_1_n_8 ),
        .Q(event_ctr_val_reg[7]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[8] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[8]_i_1_n_15 ),
        .Q(event_ctr_val_reg[8]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \event_ctr_val_reg[8]_i_1 
       (.CI(\event_ctr_val_reg[0]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\event_ctr_val_reg[8]_i_1_n_0 ,\event_ctr_val_reg[8]_i_1_n_1 ,\event_ctr_val_reg[8]_i_1_n_2 ,\event_ctr_val_reg[8]_i_1_n_3 ,\event_ctr_val_reg[8]_i_1_n_4 ,\event_ctr_val_reg[8]_i_1_n_5 ,\event_ctr_val_reg[8]_i_1_n_6 ,\event_ctr_val_reg[8]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\event_ctr_val_reg[8]_i_1_n_8 ,\event_ctr_val_reg[8]_i_1_n_9 ,\event_ctr_val_reg[8]_i_1_n_10 ,\event_ctr_val_reg[8]_i_1_n_11 ,\event_ctr_val_reg[8]_i_1_n_12 ,\event_ctr_val_reg[8]_i_1_n_13 ,\event_ctr_val_reg[8]_i_1_n_14 ,\event_ctr_val_reg[8]_i_1_n_15 }),
        .S(event_ctr_val_reg[15:8]));
  FDRE #(
    .INIT(1'b0)) 
    \event_ctr_val_reg[9] 
       (.C(mon_clk),
        .CE(cnt_enabled),
        .D(\event_ctr_val_reg[8]_i_1_n_14 ),
        .Q(event_ctr_val_reg[9]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[0] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[0]),
        .Q(\sample_ctr_val_reg_n_0_[0] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[10] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[10]),
        .Q(\sample_ctr_val_reg_n_0_[10] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[11] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[11]),
        .Q(\sample_ctr_val_reg_n_0_[11] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[12] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[12]),
        .Q(\sample_ctr_val_reg_n_0_[12] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[13] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[13]),
        .Q(\sample_ctr_val_reg_n_0_[13] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[14] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[14]),
        .Q(\sample_ctr_val_reg_n_0_[14] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[15] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[15]),
        .Q(\sample_ctr_val_reg_n_0_[15] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[16] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[16]),
        .Q(\sample_ctr_val_reg_n_0_[16] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[17] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[17]),
        .Q(\sample_ctr_val_reg_n_0_[17] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[18] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[18]),
        .Q(\sample_ctr_val_reg_n_0_[18] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[19] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[19]),
        .Q(\sample_ctr_val_reg_n_0_[19] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[1] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[1]),
        .Q(\sample_ctr_val_reg_n_0_[1] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[20] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[20]),
        .Q(\sample_ctr_val_reg_n_0_[20] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[21] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[21]),
        .Q(\sample_ctr_val_reg_n_0_[21] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[22] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[22]),
        .Q(\sample_ctr_val_reg_n_0_[22] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[23] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[23]),
        .Q(\sample_ctr_val_reg_n_0_[23] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[24] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[24]),
        .Q(\sample_ctr_val_reg_n_0_[24] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[25] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[25]),
        .Q(\sample_ctr_val_reg_n_0_[25] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[26] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[26]),
        .Q(\sample_ctr_val_reg_n_0_[26] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[27] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[27]),
        .Q(\sample_ctr_val_reg_n_0_[27] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[28] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[28]),
        .Q(\sample_ctr_val_reg_n_0_[28] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[29] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[29]),
        .Q(\sample_ctr_val_reg_n_0_[29] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[2] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[2]),
        .Q(\sample_ctr_val_reg_n_0_[2] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[30] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[30]),
        .Q(\sample_ctr_val_reg_n_0_[30] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[31] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[31]),
        .Q(\sample_ctr_val_reg_n_0_[31] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[32] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[32]),
        .Q(data11[0]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[33] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[33]),
        .Q(data11[1]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[34] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[34]),
        .Q(data11[2]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[35] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[35]),
        .Q(data11[3]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[36] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[36]),
        .Q(data11[4]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[37] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[37]),
        .Q(data11[5]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[38] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[38]),
        .Q(data11[6]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[39] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[39]),
        .Q(data11[7]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[3] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[3]),
        .Q(\sample_ctr_val_reg_n_0_[3] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[40] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[40]),
        .Q(data11[8]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[41] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[41]),
        .Q(data11[9]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[42] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[42]),
        .Q(data11[10]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[43] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[43]),
        .Q(data11[11]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[44] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[44]),
        .Q(data11[12]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[45] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[45]),
        .Q(data11[13]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[46] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[46]),
        .Q(data11[14]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[47] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[47]),
        .Q(data11[15]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[48] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[48]),
        .Q(data11[16]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[49] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[49]),
        .Q(data11[17]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[4] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[4]),
        .Q(\sample_ctr_val_reg_n_0_[4] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[50] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[50]),
        .Q(data11[18]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[51] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[51]),
        .Q(data11[19]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[52] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[52]),
        .Q(data11[20]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[53] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[53]),
        .Q(data11[21]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[54] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[54]),
        .Q(data11[22]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[55] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[55]),
        .Q(data11[23]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[56] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[56]),
        .Q(data11[24]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[57] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[57]),
        .Q(data11[25]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[58] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[58]),
        .Q(data11[26]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[59] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[59]),
        .Q(data11[27]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[5] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[5]),
        .Q(\sample_ctr_val_reg_n_0_[5] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[60] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[60]),
        .Q(data11[28]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[61] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[61]),
        .Q(data11[29]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[62] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[62]),
        .Q(data11[30]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[63] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[63]),
        .Q(data11[31]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[6] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[6]),
        .Q(\sample_ctr_val_reg_n_0_[6] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[7] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[7]),
        .Q(\sample_ctr_val_reg_n_0_[7] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[8] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[8]),
        .Q(\sample_ctr_val_reg_n_0_[8] ),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \sample_ctr_val_reg[9] 
       (.C(mon_clk),
        .CE(E),
        .D(event_ctr_val_reg[9]),
        .Q(\sample_ctr_val_reg_n_0_[9] ),
        .R(RST_ACTIVE));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[0]_i_3 
       (.I0(data11[0]),
        .I1(data13[0]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[0] ),
        .I5(\sample_data_reg[0] ),
        .O(\sample_ctr_val_reg[32]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[10]_i_3 
       (.I0(data11[10]),
        .I1(data13[10]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[10] ),
        .I5(\sample_data_reg[10] ),
        .O(\sample_ctr_val_reg[42]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[11]_i_3 
       (.I0(data11[11]),
        .I1(data13[11]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[11] ),
        .I5(\sample_data_reg[11] ),
        .O(\sample_ctr_val_reg[43]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[12]_i_3 
       (.I0(data11[12]),
        .I1(data13[12]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[12] ),
        .I5(\sample_data_reg[12] ),
        .O(\sample_ctr_val_reg[44]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[13]_i_3 
       (.I0(data11[13]),
        .I1(data13[13]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[13] ),
        .I5(\sample_data_reg[13] ),
        .O(\sample_ctr_val_reg[45]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[14]_i_3 
       (.I0(data11[14]),
        .I1(data13[14]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[14] ),
        .I5(\sample_data_reg[14] ),
        .O(\sample_ctr_val_reg[46]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[15]_i_3 
       (.I0(data11[15]),
        .I1(data13[15]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[15] ),
        .I5(\sample_data_reg[15] ),
        .O(\sample_ctr_val_reg[47]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[16]_i_3 
       (.I0(data11[16]),
        .I1(data13[16]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[16] ),
        .I5(\sample_data_reg[16] ),
        .O(\sample_ctr_val_reg[48]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[17]_i_3 
       (.I0(data11[17]),
        .I1(data13[17]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[17] ),
        .I5(\sample_data_reg[17] ),
        .O(\sample_ctr_val_reg[49]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[18]_i_3 
       (.I0(data11[18]),
        .I1(data13[18]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[18] ),
        .I5(\sample_data_reg[18] ),
        .O(\sample_ctr_val_reg[50]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[19]_i_3 
       (.I0(data11[19]),
        .I1(data13[19]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[19] ),
        .I5(\sample_data_reg[19] ),
        .O(\sample_ctr_val_reg[51]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[1]_i_3 
       (.I0(data11[1]),
        .I1(data13[1]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[1] ),
        .I5(\sample_data_reg[1] ),
        .O(\sample_ctr_val_reg[33]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[20]_i_3 
       (.I0(data11[20]),
        .I1(data13[20]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[20] ),
        .I5(\sample_data_reg[20] ),
        .O(\sample_ctr_val_reg[52]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[21]_i_3 
       (.I0(data11[21]),
        .I1(data13[21]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[21] ),
        .I5(\sample_data_reg[21] ),
        .O(\sample_ctr_val_reg[53]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[22]_i_3 
       (.I0(data11[22]),
        .I1(data13[22]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[22] ),
        .I5(\sample_data_reg[22] ),
        .O(\sample_ctr_val_reg[54]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[23]_i_3 
       (.I0(data11[23]),
        .I1(data13[23]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[23] ),
        .I5(\sample_data_reg[23] ),
        .O(\sample_ctr_val_reg[55]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[24]_i_3 
       (.I0(data11[24]),
        .I1(data13[24]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[24] ),
        .I5(\sample_data_reg[24] ),
        .O(\sample_ctr_val_reg[56]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[25]_i_3 
       (.I0(data11[25]),
        .I1(data13[25]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[25] ),
        .I5(\sample_data_reg[25] ),
        .O(\sample_ctr_val_reg[57]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[26]_i_3 
       (.I0(data11[26]),
        .I1(data13[26]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[26] ),
        .I5(\sample_data_reg[26] ),
        .O(\sample_ctr_val_reg[58]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[27]_i_3 
       (.I0(data11[27]),
        .I1(data13[27]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[27] ),
        .I5(\sample_data_reg[27] ),
        .O(\sample_ctr_val_reg[59]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[28]_i_3 
       (.I0(data11[28]),
        .I1(data13[28]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[28] ),
        .I5(\sample_data_reg[28] ),
        .O(\sample_ctr_val_reg[60]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[29]_i_3 
       (.I0(data11[29]),
        .I1(data13[29]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[29] ),
        .I5(\sample_data_reg[29] ),
        .O(\sample_ctr_val_reg[61]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[2]_i_3 
       (.I0(data11[2]),
        .I1(data13[2]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[2] ),
        .I5(\sample_data_reg[2] ),
        .O(\sample_ctr_val_reg[34]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[30]_i_3 
       (.I0(data11[30]),
        .I1(data13[30]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[30] ),
        .I5(\sample_data_reg[30] ),
        .O(\sample_ctr_val_reg[62]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[31]_i_6 
       (.I0(data11[31]),
        .I1(data13[31]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[31] ),
        .I5(\sample_data_reg[31] ),
        .O(\sample_ctr_val_reg[63]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[3]_i_3 
       (.I0(data11[3]),
        .I1(data13[3]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[3] ),
        .I5(\sample_data_reg[3] ),
        .O(\sample_ctr_val_reg[35]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[4]_i_3 
       (.I0(data11[4]),
        .I1(data13[4]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[4] ),
        .I5(\sample_data_reg[4] ),
        .O(\sample_ctr_val_reg[36]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[5]_i_3 
       (.I0(data11[5]),
        .I1(data13[5]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[5] ),
        .I5(\sample_data_reg[5] ),
        .O(\sample_ctr_val_reg[37]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[6]_i_3 
       (.I0(data11[6]),
        .I1(data13[6]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[6] ),
        .I5(\sample_data_reg[6] ),
        .O(\sample_ctr_val_reg[38]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[7]_i_3 
       (.I0(data11[7]),
        .I1(data13[7]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[7] ),
        .I5(\sample_data_reg[7] ),
        .O(\sample_ctr_val_reg[39]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[8]_i_3 
       (.I0(data11[8]),
        .I1(data13[8]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[8] ),
        .I5(\sample_data_reg[8] ),
        .O(\sample_ctr_val_reg[40]_0 ));
  LUT6 #(
    .INIT(64'hCFAFCFA0C0AFC0A0)) 
    \sample_data[9]_i_3 
       (.I0(data11[9]),
        .I1(data13[9]),
        .I2(slv_reg_addr[0]),
        .I3(slv_reg_addr[1]),
        .I4(\sample_ctr_val_reg_n_0_[9] ),
        .I5(\sample_data_reg[9] ),
        .O(\sample_ctr_val_reg[41]_0 ));
endmodule

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_min_max_ctr
   (empty,
    Q,
    \max_ctr_reg[63]_0 ,
    RST_ACTIVE,
    mon_clk,
    rd_en,
    ap_done_reg,
    dataflow_en,
    ap_continue_reg,
    start_pulse);
  output empty;
  output [63:0]Q;
  output [63:0]\max_ctr_reg[63]_0 ;
  input RST_ACTIVE;
  input mon_clk;
  input rd_en;
  input ap_done_reg;
  input dataflow_en;
  input ap_continue_reg;
  input start_pulse;

  wire [63:0]Q;
  wire RST_ACTIVE;
  wire ap_continue_reg;
  wire ap_done_reg;
  wire \counter[0]_i_2_n_0 ;
  wire [63:0]counter_reg;
  wire \counter_reg[0]_i_1_n_0 ;
  wire \counter_reg[0]_i_1_n_1 ;
  wire \counter_reg[0]_i_1_n_10 ;
  wire \counter_reg[0]_i_1_n_11 ;
  wire \counter_reg[0]_i_1_n_12 ;
  wire \counter_reg[0]_i_1_n_13 ;
  wire \counter_reg[0]_i_1_n_14 ;
  wire \counter_reg[0]_i_1_n_15 ;
  wire \counter_reg[0]_i_1_n_2 ;
  wire \counter_reg[0]_i_1_n_3 ;
  wire \counter_reg[0]_i_1_n_4 ;
  wire \counter_reg[0]_i_1_n_5 ;
  wire \counter_reg[0]_i_1_n_6 ;
  wire \counter_reg[0]_i_1_n_7 ;
  wire \counter_reg[0]_i_1_n_8 ;
  wire \counter_reg[0]_i_1_n_9 ;
  wire \counter_reg[16]_i_1_n_0 ;
  wire \counter_reg[16]_i_1_n_1 ;
  wire \counter_reg[16]_i_1_n_10 ;
  wire \counter_reg[16]_i_1_n_11 ;
  wire \counter_reg[16]_i_1_n_12 ;
  wire \counter_reg[16]_i_1_n_13 ;
  wire \counter_reg[16]_i_1_n_14 ;
  wire \counter_reg[16]_i_1_n_15 ;
  wire \counter_reg[16]_i_1_n_2 ;
  wire \counter_reg[16]_i_1_n_3 ;
  wire \counter_reg[16]_i_1_n_4 ;
  wire \counter_reg[16]_i_1_n_5 ;
  wire \counter_reg[16]_i_1_n_6 ;
  wire \counter_reg[16]_i_1_n_7 ;
  wire \counter_reg[16]_i_1_n_8 ;
  wire \counter_reg[16]_i_1_n_9 ;
  wire \counter_reg[24]_i_1_n_0 ;
  wire \counter_reg[24]_i_1_n_1 ;
  wire \counter_reg[24]_i_1_n_10 ;
  wire \counter_reg[24]_i_1_n_11 ;
  wire \counter_reg[24]_i_1_n_12 ;
  wire \counter_reg[24]_i_1_n_13 ;
  wire \counter_reg[24]_i_1_n_14 ;
  wire \counter_reg[24]_i_1_n_15 ;
  wire \counter_reg[24]_i_1_n_2 ;
  wire \counter_reg[24]_i_1_n_3 ;
  wire \counter_reg[24]_i_1_n_4 ;
  wire \counter_reg[24]_i_1_n_5 ;
  wire \counter_reg[24]_i_1_n_6 ;
  wire \counter_reg[24]_i_1_n_7 ;
  wire \counter_reg[24]_i_1_n_8 ;
  wire \counter_reg[24]_i_1_n_9 ;
  wire \counter_reg[32]_i_1_n_0 ;
  wire \counter_reg[32]_i_1_n_1 ;
  wire \counter_reg[32]_i_1_n_10 ;
  wire \counter_reg[32]_i_1_n_11 ;
  wire \counter_reg[32]_i_1_n_12 ;
  wire \counter_reg[32]_i_1_n_13 ;
  wire \counter_reg[32]_i_1_n_14 ;
  wire \counter_reg[32]_i_1_n_15 ;
  wire \counter_reg[32]_i_1_n_2 ;
  wire \counter_reg[32]_i_1_n_3 ;
  wire \counter_reg[32]_i_1_n_4 ;
  wire \counter_reg[32]_i_1_n_5 ;
  wire \counter_reg[32]_i_1_n_6 ;
  wire \counter_reg[32]_i_1_n_7 ;
  wire \counter_reg[32]_i_1_n_8 ;
  wire \counter_reg[32]_i_1_n_9 ;
  wire \counter_reg[40]_i_1_n_0 ;
  wire \counter_reg[40]_i_1_n_1 ;
  wire \counter_reg[40]_i_1_n_10 ;
  wire \counter_reg[40]_i_1_n_11 ;
  wire \counter_reg[40]_i_1_n_12 ;
  wire \counter_reg[40]_i_1_n_13 ;
  wire \counter_reg[40]_i_1_n_14 ;
  wire \counter_reg[40]_i_1_n_15 ;
  wire \counter_reg[40]_i_1_n_2 ;
  wire \counter_reg[40]_i_1_n_3 ;
  wire \counter_reg[40]_i_1_n_4 ;
  wire \counter_reg[40]_i_1_n_5 ;
  wire \counter_reg[40]_i_1_n_6 ;
  wire \counter_reg[40]_i_1_n_7 ;
  wire \counter_reg[40]_i_1_n_8 ;
  wire \counter_reg[40]_i_1_n_9 ;
  wire \counter_reg[48]_i_1_n_0 ;
  wire \counter_reg[48]_i_1_n_1 ;
  wire \counter_reg[48]_i_1_n_10 ;
  wire \counter_reg[48]_i_1_n_11 ;
  wire \counter_reg[48]_i_1_n_12 ;
  wire \counter_reg[48]_i_1_n_13 ;
  wire \counter_reg[48]_i_1_n_14 ;
  wire \counter_reg[48]_i_1_n_15 ;
  wire \counter_reg[48]_i_1_n_2 ;
  wire \counter_reg[48]_i_1_n_3 ;
  wire \counter_reg[48]_i_1_n_4 ;
  wire \counter_reg[48]_i_1_n_5 ;
  wire \counter_reg[48]_i_1_n_6 ;
  wire \counter_reg[48]_i_1_n_7 ;
  wire \counter_reg[48]_i_1_n_8 ;
  wire \counter_reg[48]_i_1_n_9 ;
  wire \counter_reg[56]_i_1_n_1 ;
  wire \counter_reg[56]_i_1_n_10 ;
  wire \counter_reg[56]_i_1_n_11 ;
  wire \counter_reg[56]_i_1_n_12 ;
  wire \counter_reg[56]_i_1_n_13 ;
  wire \counter_reg[56]_i_1_n_14 ;
  wire \counter_reg[56]_i_1_n_15 ;
  wire \counter_reg[56]_i_1_n_2 ;
  wire \counter_reg[56]_i_1_n_3 ;
  wire \counter_reg[56]_i_1_n_4 ;
  wire \counter_reg[56]_i_1_n_5 ;
  wire \counter_reg[56]_i_1_n_6 ;
  wire \counter_reg[56]_i_1_n_7 ;
  wire \counter_reg[56]_i_1_n_8 ;
  wire \counter_reg[56]_i_1_n_9 ;
  wire \counter_reg[8]_i_1_n_0 ;
  wire \counter_reg[8]_i_1_n_1 ;
  wire \counter_reg[8]_i_1_n_10 ;
  wire \counter_reg[8]_i_1_n_11 ;
  wire \counter_reg[8]_i_1_n_12 ;
  wire \counter_reg[8]_i_1_n_13 ;
  wire \counter_reg[8]_i_1_n_14 ;
  wire \counter_reg[8]_i_1_n_15 ;
  wire \counter_reg[8]_i_1_n_2 ;
  wire \counter_reg[8]_i_1_n_3 ;
  wire \counter_reg[8]_i_1_n_4 ;
  wire \counter_reg[8]_i_1_n_5 ;
  wire \counter_reg[8]_i_1_n_6 ;
  wire \counter_reg[8]_i_1_n_7 ;
  wire \counter_reg[8]_i_1_n_8 ;
  wire \counter_reg[8]_i_1_n_9 ;
  wire dataflow_en;
  wire empty;
  wire fifo_minmax_n_1;
  wire fifo_minmax_n_10;
  wire fifo_minmax_n_11;
  wire fifo_minmax_n_12;
  wire fifo_minmax_n_13;
  wire fifo_minmax_n_14;
  wire fifo_minmax_n_15;
  wire fifo_minmax_n_16;
  wire fifo_minmax_n_17;
  wire fifo_minmax_n_18;
  wire fifo_minmax_n_19;
  wire fifo_minmax_n_2;
  wire fifo_minmax_n_20;
  wire fifo_minmax_n_21;
  wire fifo_minmax_n_22;
  wire fifo_minmax_n_23;
  wire fifo_minmax_n_24;
  wire fifo_minmax_n_25;
  wire fifo_minmax_n_26;
  wire fifo_minmax_n_27;
  wire fifo_minmax_n_28;
  wire fifo_minmax_n_29;
  wire fifo_minmax_n_3;
  wire fifo_minmax_n_30;
  wire fifo_minmax_n_31;
  wire fifo_minmax_n_32;
  wire fifo_minmax_n_33;
  wire fifo_minmax_n_34;
  wire fifo_minmax_n_35;
  wire fifo_minmax_n_36;
  wire fifo_minmax_n_37;
  wire fifo_minmax_n_38;
  wire fifo_minmax_n_39;
  wire fifo_minmax_n_4;
  wire fifo_minmax_n_40;
  wire fifo_minmax_n_41;
  wire fifo_minmax_n_42;
  wire fifo_minmax_n_43;
  wire fifo_minmax_n_44;
  wire fifo_minmax_n_45;
  wire fifo_minmax_n_46;
  wire fifo_minmax_n_47;
  wire fifo_minmax_n_48;
  wire fifo_minmax_n_49;
  wire fifo_minmax_n_5;
  wire fifo_minmax_n_50;
  wire fifo_minmax_n_51;
  wire fifo_minmax_n_52;
  wire fifo_minmax_n_53;
  wire fifo_minmax_n_54;
  wire fifo_minmax_n_55;
  wire fifo_minmax_n_56;
  wire fifo_minmax_n_57;
  wire fifo_minmax_n_58;
  wire fifo_minmax_n_59;
  wire fifo_minmax_n_6;
  wire fifo_minmax_n_60;
  wire fifo_minmax_n_61;
  wire fifo_minmax_n_62;
  wire fifo_minmax_n_63;
  wire fifo_minmax_n_64;
  wire fifo_minmax_n_7;
  wire fifo_minmax_n_8;
  wire fifo_minmax_n_9;
  wire max_ctr0;
  wire max_ctr1;
  wire max_ctr1_carry__0_i_10_n_0;
  wire max_ctr1_carry__0_i_11_n_0;
  wire max_ctr1_carry__0_i_12_n_0;
  wire max_ctr1_carry__0_i_13_n_0;
  wire max_ctr1_carry__0_i_14_n_0;
  wire max_ctr1_carry__0_i_15_n_0;
  wire max_ctr1_carry__0_i_16_n_0;
  wire max_ctr1_carry__0_i_1_n_0;
  wire max_ctr1_carry__0_i_2_n_0;
  wire max_ctr1_carry__0_i_3_n_0;
  wire max_ctr1_carry__0_i_4_n_0;
  wire max_ctr1_carry__0_i_5_n_0;
  wire max_ctr1_carry__0_i_6_n_0;
  wire max_ctr1_carry__0_i_7_n_0;
  wire max_ctr1_carry__0_i_8_n_0;
  wire max_ctr1_carry__0_i_9_n_0;
  wire max_ctr1_carry__0_n_0;
  wire max_ctr1_carry__0_n_1;
  wire max_ctr1_carry__0_n_2;
  wire max_ctr1_carry__0_n_3;
  wire max_ctr1_carry__0_n_4;
  wire max_ctr1_carry__0_n_5;
  wire max_ctr1_carry__0_n_6;
  wire max_ctr1_carry__0_n_7;
  wire max_ctr1_carry__1_i_10_n_0;
  wire max_ctr1_carry__1_i_11_n_0;
  wire max_ctr1_carry__1_i_12_n_0;
  wire max_ctr1_carry__1_i_13_n_0;
  wire max_ctr1_carry__1_i_14_n_0;
  wire max_ctr1_carry__1_i_15_n_0;
  wire max_ctr1_carry__1_i_16_n_0;
  wire max_ctr1_carry__1_i_1_n_0;
  wire max_ctr1_carry__1_i_2_n_0;
  wire max_ctr1_carry__1_i_3_n_0;
  wire max_ctr1_carry__1_i_4_n_0;
  wire max_ctr1_carry__1_i_5_n_0;
  wire max_ctr1_carry__1_i_6_n_0;
  wire max_ctr1_carry__1_i_7_n_0;
  wire max_ctr1_carry__1_i_8_n_0;
  wire max_ctr1_carry__1_i_9_n_0;
  wire max_ctr1_carry__1_n_0;
  wire max_ctr1_carry__1_n_1;
  wire max_ctr1_carry__1_n_2;
  wire max_ctr1_carry__1_n_3;
  wire max_ctr1_carry__1_n_4;
  wire max_ctr1_carry__1_n_5;
  wire max_ctr1_carry__1_n_6;
  wire max_ctr1_carry__1_n_7;
  wire max_ctr1_carry__2_i_10_n_0;
  wire max_ctr1_carry__2_i_11_n_0;
  wire max_ctr1_carry__2_i_12_n_0;
  wire max_ctr1_carry__2_i_13_n_0;
  wire max_ctr1_carry__2_i_14_n_0;
  wire max_ctr1_carry__2_i_15_n_0;
  wire max_ctr1_carry__2_i_16_n_0;
  wire max_ctr1_carry__2_i_1_n_0;
  wire max_ctr1_carry__2_i_2_n_0;
  wire max_ctr1_carry__2_i_3_n_0;
  wire max_ctr1_carry__2_i_4_n_0;
  wire max_ctr1_carry__2_i_5_n_0;
  wire max_ctr1_carry__2_i_6_n_0;
  wire max_ctr1_carry__2_i_7_n_0;
  wire max_ctr1_carry__2_i_8_n_0;
  wire max_ctr1_carry__2_i_9_n_0;
  wire max_ctr1_carry__2_n_1;
  wire max_ctr1_carry__2_n_2;
  wire max_ctr1_carry__2_n_3;
  wire max_ctr1_carry__2_n_4;
  wire max_ctr1_carry__2_n_5;
  wire max_ctr1_carry__2_n_6;
  wire max_ctr1_carry__2_n_7;
  wire max_ctr1_carry_i_10_n_0;
  wire max_ctr1_carry_i_11_n_0;
  wire max_ctr1_carry_i_12_n_0;
  wire max_ctr1_carry_i_13_n_0;
  wire max_ctr1_carry_i_14_n_0;
  wire max_ctr1_carry_i_15_n_0;
  wire max_ctr1_carry_i_16_n_0;
  wire max_ctr1_carry_i_1_n_0;
  wire max_ctr1_carry_i_2_n_0;
  wire max_ctr1_carry_i_3_n_0;
  wire max_ctr1_carry_i_4_n_0;
  wire max_ctr1_carry_i_5_n_0;
  wire max_ctr1_carry_i_6_n_0;
  wire max_ctr1_carry_i_7_n_0;
  wire max_ctr1_carry_i_8_n_0;
  wire max_ctr1_carry_i_9_n_0;
  wire max_ctr1_carry_n_0;
  wire max_ctr1_carry_n_1;
  wire max_ctr1_carry_n_2;
  wire max_ctr1_carry_n_3;
  wire max_ctr1_carry_n_4;
  wire max_ctr1_carry_n_5;
  wire max_ctr1_carry_n_6;
  wire max_ctr1_carry_n_7;
  wire [63:0]\max_ctr_reg[63]_0 ;
  wire min_ctr0;
  wire min_ctr1;
  wire min_ctr1_carry__0_i_10_n_0;
  wire min_ctr1_carry__0_i_11_n_0;
  wire min_ctr1_carry__0_i_12_n_0;
  wire min_ctr1_carry__0_i_13_n_0;
  wire min_ctr1_carry__0_i_14_n_0;
  wire min_ctr1_carry__0_i_15_n_0;
  wire min_ctr1_carry__0_i_16_n_0;
  wire min_ctr1_carry__0_i_1_n_0;
  wire min_ctr1_carry__0_i_2_n_0;
  wire min_ctr1_carry__0_i_3_n_0;
  wire min_ctr1_carry__0_i_4_n_0;
  wire min_ctr1_carry__0_i_5_n_0;
  wire min_ctr1_carry__0_i_6_n_0;
  wire min_ctr1_carry__0_i_7_n_0;
  wire min_ctr1_carry__0_i_8_n_0;
  wire min_ctr1_carry__0_i_9_n_0;
  wire min_ctr1_carry__0_n_0;
  wire min_ctr1_carry__0_n_1;
  wire min_ctr1_carry__0_n_2;
  wire min_ctr1_carry__0_n_3;
  wire min_ctr1_carry__0_n_4;
  wire min_ctr1_carry__0_n_5;
  wire min_ctr1_carry__0_n_6;
  wire min_ctr1_carry__0_n_7;
  wire min_ctr1_carry__1_i_10_n_0;
  wire min_ctr1_carry__1_i_11_n_0;
  wire min_ctr1_carry__1_i_12_n_0;
  wire min_ctr1_carry__1_i_13_n_0;
  wire min_ctr1_carry__1_i_14_n_0;
  wire min_ctr1_carry__1_i_15_n_0;
  wire min_ctr1_carry__1_i_16_n_0;
  wire min_ctr1_carry__1_i_1_n_0;
  wire min_ctr1_carry__1_i_2_n_0;
  wire min_ctr1_carry__1_i_3_n_0;
  wire min_ctr1_carry__1_i_4_n_0;
  wire min_ctr1_carry__1_i_5_n_0;
  wire min_ctr1_carry__1_i_6_n_0;
  wire min_ctr1_carry__1_i_7_n_0;
  wire min_ctr1_carry__1_i_8_n_0;
  wire min_ctr1_carry__1_i_9_n_0;
  wire min_ctr1_carry__1_n_0;
  wire min_ctr1_carry__1_n_1;
  wire min_ctr1_carry__1_n_2;
  wire min_ctr1_carry__1_n_3;
  wire min_ctr1_carry__1_n_4;
  wire min_ctr1_carry__1_n_5;
  wire min_ctr1_carry__1_n_6;
  wire min_ctr1_carry__1_n_7;
  wire min_ctr1_carry__2_i_10_n_0;
  wire min_ctr1_carry__2_i_11_n_0;
  wire min_ctr1_carry__2_i_12_n_0;
  wire min_ctr1_carry__2_i_13_n_0;
  wire min_ctr1_carry__2_i_14_n_0;
  wire min_ctr1_carry__2_i_15_n_0;
  wire min_ctr1_carry__2_i_16_n_0;
  wire min_ctr1_carry__2_i_1_n_0;
  wire min_ctr1_carry__2_i_2_n_0;
  wire min_ctr1_carry__2_i_3_n_0;
  wire min_ctr1_carry__2_i_4_n_0;
  wire min_ctr1_carry__2_i_5_n_0;
  wire min_ctr1_carry__2_i_6_n_0;
  wire min_ctr1_carry__2_i_7_n_0;
  wire min_ctr1_carry__2_i_8_n_0;
  wire min_ctr1_carry__2_i_9_n_0;
  wire min_ctr1_carry__2_n_1;
  wire min_ctr1_carry__2_n_2;
  wire min_ctr1_carry__2_n_3;
  wire min_ctr1_carry__2_n_4;
  wire min_ctr1_carry__2_n_5;
  wire min_ctr1_carry__2_n_6;
  wire min_ctr1_carry__2_n_7;
  wire min_ctr1_carry_i_10_n_0;
  wire min_ctr1_carry_i_11_n_0;
  wire min_ctr1_carry_i_12_n_0;
  wire min_ctr1_carry_i_13_n_0;
  wire min_ctr1_carry_i_14_n_0;
  wire min_ctr1_carry_i_15_n_0;
  wire min_ctr1_carry_i_16_n_0;
  wire min_ctr1_carry_i_1_n_0;
  wire min_ctr1_carry_i_2_n_0;
  wire min_ctr1_carry_i_3_n_0;
  wire min_ctr1_carry_i_4_n_0;
  wire min_ctr1_carry_i_5_n_0;
  wire min_ctr1_carry_i_6_n_0;
  wire min_ctr1_carry_i_7_n_0;
  wire min_ctr1_carry_i_8_n_0;
  wire min_ctr1_carry_i_9_n_0;
  wire min_ctr1_carry_n_0;
  wire min_ctr1_carry_n_1;
  wire min_ctr1_carry_n_2;
  wire min_ctr1_carry_n_3;
  wire min_ctr1_carry_n_4;
  wire min_ctr1_carry_n_5;
  wire min_ctr1_carry_n_6;
  wire min_ctr1_carry_n_7;
  wire mon_clk;
  wire rd_en;
  wire start_pulse;
  wire [63:0]tx_length;
  wire tx_length_carry__0_n_0;
  wire tx_length_carry__0_n_1;
  wire tx_length_carry__0_n_2;
  wire tx_length_carry__0_n_3;
  wire tx_length_carry__0_n_4;
  wire tx_length_carry__0_n_5;
  wire tx_length_carry__0_n_6;
  wire tx_length_carry__0_n_7;
  wire tx_length_carry__1_n_0;
  wire tx_length_carry__1_n_1;
  wire tx_length_carry__1_n_2;
  wire tx_length_carry__1_n_3;
  wire tx_length_carry__1_n_4;
  wire tx_length_carry__1_n_5;
  wire tx_length_carry__1_n_6;
  wire tx_length_carry__1_n_7;
  wire tx_length_carry__2_n_0;
  wire tx_length_carry__2_n_1;
  wire tx_length_carry__2_n_2;
  wire tx_length_carry__2_n_3;
  wire tx_length_carry__2_n_4;
  wire tx_length_carry__2_n_5;
  wire tx_length_carry__2_n_6;
  wire tx_length_carry__2_n_7;
  wire tx_length_carry__3_n_0;
  wire tx_length_carry__3_n_1;
  wire tx_length_carry__3_n_2;
  wire tx_length_carry__3_n_3;
  wire tx_length_carry__3_n_4;
  wire tx_length_carry__3_n_5;
  wire tx_length_carry__3_n_6;
  wire tx_length_carry__3_n_7;
  wire tx_length_carry__4_n_0;
  wire tx_length_carry__4_n_1;
  wire tx_length_carry__4_n_2;
  wire tx_length_carry__4_n_3;
  wire tx_length_carry__4_n_4;
  wire tx_length_carry__4_n_5;
  wire tx_length_carry__4_n_6;
  wire tx_length_carry__4_n_7;
  wire tx_length_carry__5_n_0;
  wire tx_length_carry__5_n_1;
  wire tx_length_carry__5_n_2;
  wire tx_length_carry__5_n_3;
  wire tx_length_carry__5_n_4;
  wire tx_length_carry__5_n_5;
  wire tx_length_carry__5_n_6;
  wire tx_length_carry__5_n_7;
  wire tx_length_carry__6_n_1;
  wire tx_length_carry__6_n_2;
  wire tx_length_carry__6_n_3;
  wire tx_length_carry__6_n_4;
  wire tx_length_carry__6_n_5;
  wire tx_length_carry__6_n_6;
  wire tx_length_carry__6_n_7;
  wire tx_length_carry_n_0;
  wire tx_length_carry_n_1;
  wire tx_length_carry_n_2;
  wire tx_length_carry_n_3;
  wire tx_length_carry_n_4;
  wire tx_length_carry_n_5;
  wire tx_length_carry_n_6;
  wire tx_length_carry_n_7;
  wire [7:7]\NLW_counter_reg[56]_i_1_CO_UNCONNECTED ;
  wire [7:0]NLW_max_ctr1_carry_O_UNCONNECTED;
  wire [7:0]NLW_max_ctr1_carry__0_O_UNCONNECTED;
  wire [7:0]NLW_max_ctr1_carry__1_O_UNCONNECTED;
  wire [7:0]NLW_max_ctr1_carry__2_O_UNCONNECTED;
  wire [7:0]NLW_min_ctr1_carry_O_UNCONNECTED;
  wire [7:0]NLW_min_ctr1_carry__0_O_UNCONNECTED;
  wire [7:0]NLW_min_ctr1_carry__1_O_UNCONNECTED;
  wire [7:0]NLW_min_ctr1_carry__2_O_UNCONNECTED;
  wire [7:7]NLW_tx_length_carry__6_CO_UNCONNECTED;

  LUT1 #(
    .INIT(2'h1)) 
    \counter[0]_i_2 
       (.I0(counter_reg[0]),
        .O(\counter[0]_i_2_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[0] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[0]_i_1_n_15 ),
        .Q(counter_reg[0]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \counter_reg[0]_i_1 
       (.CI(1'b0),
        .CI_TOP(1'b0),
        .CO({\counter_reg[0]_i_1_n_0 ,\counter_reg[0]_i_1_n_1 ,\counter_reg[0]_i_1_n_2 ,\counter_reg[0]_i_1_n_3 ,\counter_reg[0]_i_1_n_4 ,\counter_reg[0]_i_1_n_5 ,\counter_reg[0]_i_1_n_6 ,\counter_reg[0]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b1}),
        .O({\counter_reg[0]_i_1_n_8 ,\counter_reg[0]_i_1_n_9 ,\counter_reg[0]_i_1_n_10 ,\counter_reg[0]_i_1_n_11 ,\counter_reg[0]_i_1_n_12 ,\counter_reg[0]_i_1_n_13 ,\counter_reg[0]_i_1_n_14 ,\counter_reg[0]_i_1_n_15 }),
        .S({counter_reg[7:1],\counter[0]_i_2_n_0 }));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[10] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[8]_i_1_n_13 ),
        .Q(counter_reg[10]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[11] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[8]_i_1_n_12 ),
        .Q(counter_reg[11]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[12] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[8]_i_1_n_11 ),
        .Q(counter_reg[12]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[13] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[8]_i_1_n_10 ),
        .Q(counter_reg[13]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[14] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[8]_i_1_n_9 ),
        .Q(counter_reg[14]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[15] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[8]_i_1_n_8 ),
        .Q(counter_reg[15]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[16] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[16]_i_1_n_15 ),
        .Q(counter_reg[16]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \counter_reg[16]_i_1 
       (.CI(\counter_reg[8]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\counter_reg[16]_i_1_n_0 ,\counter_reg[16]_i_1_n_1 ,\counter_reg[16]_i_1_n_2 ,\counter_reg[16]_i_1_n_3 ,\counter_reg[16]_i_1_n_4 ,\counter_reg[16]_i_1_n_5 ,\counter_reg[16]_i_1_n_6 ,\counter_reg[16]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\counter_reg[16]_i_1_n_8 ,\counter_reg[16]_i_1_n_9 ,\counter_reg[16]_i_1_n_10 ,\counter_reg[16]_i_1_n_11 ,\counter_reg[16]_i_1_n_12 ,\counter_reg[16]_i_1_n_13 ,\counter_reg[16]_i_1_n_14 ,\counter_reg[16]_i_1_n_15 }),
        .S(counter_reg[23:16]));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[17] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[16]_i_1_n_14 ),
        .Q(counter_reg[17]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[18] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[16]_i_1_n_13 ),
        .Q(counter_reg[18]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[19] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[16]_i_1_n_12 ),
        .Q(counter_reg[19]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[1] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[0]_i_1_n_14 ),
        .Q(counter_reg[1]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[20] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[16]_i_1_n_11 ),
        .Q(counter_reg[20]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[21] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[16]_i_1_n_10 ),
        .Q(counter_reg[21]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[22] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[16]_i_1_n_9 ),
        .Q(counter_reg[22]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[23] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[16]_i_1_n_8 ),
        .Q(counter_reg[23]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[24] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[24]_i_1_n_15 ),
        .Q(counter_reg[24]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \counter_reg[24]_i_1 
       (.CI(\counter_reg[16]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\counter_reg[24]_i_1_n_0 ,\counter_reg[24]_i_1_n_1 ,\counter_reg[24]_i_1_n_2 ,\counter_reg[24]_i_1_n_3 ,\counter_reg[24]_i_1_n_4 ,\counter_reg[24]_i_1_n_5 ,\counter_reg[24]_i_1_n_6 ,\counter_reg[24]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\counter_reg[24]_i_1_n_8 ,\counter_reg[24]_i_1_n_9 ,\counter_reg[24]_i_1_n_10 ,\counter_reg[24]_i_1_n_11 ,\counter_reg[24]_i_1_n_12 ,\counter_reg[24]_i_1_n_13 ,\counter_reg[24]_i_1_n_14 ,\counter_reg[24]_i_1_n_15 }),
        .S(counter_reg[31:24]));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[25] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[24]_i_1_n_14 ),
        .Q(counter_reg[25]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[26] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[24]_i_1_n_13 ),
        .Q(counter_reg[26]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[27] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[24]_i_1_n_12 ),
        .Q(counter_reg[27]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[28] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[24]_i_1_n_11 ),
        .Q(counter_reg[28]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[29] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[24]_i_1_n_10 ),
        .Q(counter_reg[29]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[2] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[0]_i_1_n_13 ),
        .Q(counter_reg[2]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[30] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[24]_i_1_n_9 ),
        .Q(counter_reg[30]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[31] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[24]_i_1_n_8 ),
        .Q(counter_reg[31]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[32] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[32]_i_1_n_15 ),
        .Q(counter_reg[32]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \counter_reg[32]_i_1 
       (.CI(\counter_reg[24]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\counter_reg[32]_i_1_n_0 ,\counter_reg[32]_i_1_n_1 ,\counter_reg[32]_i_1_n_2 ,\counter_reg[32]_i_1_n_3 ,\counter_reg[32]_i_1_n_4 ,\counter_reg[32]_i_1_n_5 ,\counter_reg[32]_i_1_n_6 ,\counter_reg[32]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\counter_reg[32]_i_1_n_8 ,\counter_reg[32]_i_1_n_9 ,\counter_reg[32]_i_1_n_10 ,\counter_reg[32]_i_1_n_11 ,\counter_reg[32]_i_1_n_12 ,\counter_reg[32]_i_1_n_13 ,\counter_reg[32]_i_1_n_14 ,\counter_reg[32]_i_1_n_15 }),
        .S(counter_reg[39:32]));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[33] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[32]_i_1_n_14 ),
        .Q(counter_reg[33]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[34] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[32]_i_1_n_13 ),
        .Q(counter_reg[34]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[35] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[32]_i_1_n_12 ),
        .Q(counter_reg[35]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[36] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[32]_i_1_n_11 ),
        .Q(counter_reg[36]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[37] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[32]_i_1_n_10 ),
        .Q(counter_reg[37]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[38] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[32]_i_1_n_9 ),
        .Q(counter_reg[38]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[39] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[32]_i_1_n_8 ),
        .Q(counter_reg[39]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[3] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[0]_i_1_n_12 ),
        .Q(counter_reg[3]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[40] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[40]_i_1_n_15 ),
        .Q(counter_reg[40]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \counter_reg[40]_i_1 
       (.CI(\counter_reg[32]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\counter_reg[40]_i_1_n_0 ,\counter_reg[40]_i_1_n_1 ,\counter_reg[40]_i_1_n_2 ,\counter_reg[40]_i_1_n_3 ,\counter_reg[40]_i_1_n_4 ,\counter_reg[40]_i_1_n_5 ,\counter_reg[40]_i_1_n_6 ,\counter_reg[40]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\counter_reg[40]_i_1_n_8 ,\counter_reg[40]_i_1_n_9 ,\counter_reg[40]_i_1_n_10 ,\counter_reg[40]_i_1_n_11 ,\counter_reg[40]_i_1_n_12 ,\counter_reg[40]_i_1_n_13 ,\counter_reg[40]_i_1_n_14 ,\counter_reg[40]_i_1_n_15 }),
        .S(counter_reg[47:40]));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[41] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[40]_i_1_n_14 ),
        .Q(counter_reg[41]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[42] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[40]_i_1_n_13 ),
        .Q(counter_reg[42]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[43] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[40]_i_1_n_12 ),
        .Q(counter_reg[43]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[44] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[40]_i_1_n_11 ),
        .Q(counter_reg[44]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[45] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[40]_i_1_n_10 ),
        .Q(counter_reg[45]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[46] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[40]_i_1_n_9 ),
        .Q(counter_reg[46]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[47] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[40]_i_1_n_8 ),
        .Q(counter_reg[47]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[48] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[48]_i_1_n_15 ),
        .Q(counter_reg[48]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \counter_reg[48]_i_1 
       (.CI(\counter_reg[40]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\counter_reg[48]_i_1_n_0 ,\counter_reg[48]_i_1_n_1 ,\counter_reg[48]_i_1_n_2 ,\counter_reg[48]_i_1_n_3 ,\counter_reg[48]_i_1_n_4 ,\counter_reg[48]_i_1_n_5 ,\counter_reg[48]_i_1_n_6 ,\counter_reg[48]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\counter_reg[48]_i_1_n_8 ,\counter_reg[48]_i_1_n_9 ,\counter_reg[48]_i_1_n_10 ,\counter_reg[48]_i_1_n_11 ,\counter_reg[48]_i_1_n_12 ,\counter_reg[48]_i_1_n_13 ,\counter_reg[48]_i_1_n_14 ,\counter_reg[48]_i_1_n_15 }),
        .S(counter_reg[55:48]));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[49] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[48]_i_1_n_14 ),
        .Q(counter_reg[49]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[4] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[0]_i_1_n_11 ),
        .Q(counter_reg[4]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[50] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[48]_i_1_n_13 ),
        .Q(counter_reg[50]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[51] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[48]_i_1_n_12 ),
        .Q(counter_reg[51]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[52] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[48]_i_1_n_11 ),
        .Q(counter_reg[52]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[53] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[48]_i_1_n_10 ),
        .Q(counter_reg[53]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[54] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[48]_i_1_n_9 ),
        .Q(counter_reg[54]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[55] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[48]_i_1_n_8 ),
        .Q(counter_reg[55]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[56] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[56]_i_1_n_15 ),
        .Q(counter_reg[56]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \counter_reg[56]_i_1 
       (.CI(\counter_reg[48]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\NLW_counter_reg[56]_i_1_CO_UNCONNECTED [7],\counter_reg[56]_i_1_n_1 ,\counter_reg[56]_i_1_n_2 ,\counter_reg[56]_i_1_n_3 ,\counter_reg[56]_i_1_n_4 ,\counter_reg[56]_i_1_n_5 ,\counter_reg[56]_i_1_n_6 ,\counter_reg[56]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\counter_reg[56]_i_1_n_8 ,\counter_reg[56]_i_1_n_9 ,\counter_reg[56]_i_1_n_10 ,\counter_reg[56]_i_1_n_11 ,\counter_reg[56]_i_1_n_12 ,\counter_reg[56]_i_1_n_13 ,\counter_reg[56]_i_1_n_14 ,\counter_reg[56]_i_1_n_15 }),
        .S(counter_reg[63:56]));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[57] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[56]_i_1_n_14 ),
        .Q(counter_reg[57]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[58] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[56]_i_1_n_13 ),
        .Q(counter_reg[58]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[59] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[56]_i_1_n_12 ),
        .Q(counter_reg[59]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[5] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[0]_i_1_n_10 ),
        .Q(counter_reg[5]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[60] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[56]_i_1_n_11 ),
        .Q(counter_reg[60]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[61] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[56]_i_1_n_10 ),
        .Q(counter_reg[61]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[62] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[56]_i_1_n_9 ),
        .Q(counter_reg[62]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[63] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[56]_i_1_n_8 ),
        .Q(counter_reg[63]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[6] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[0]_i_1_n_9 ),
        .Q(counter_reg[6]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[7] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[0]_i_1_n_8 ),
        .Q(counter_reg[7]),
        .R(RST_ACTIVE));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[8] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[8]_i_1_n_15 ),
        .Q(counter_reg[8]),
        .R(RST_ACTIVE));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \counter_reg[8]_i_1 
       (.CI(\counter_reg[0]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\counter_reg[8]_i_1_n_0 ,\counter_reg[8]_i_1_n_1 ,\counter_reg[8]_i_1_n_2 ,\counter_reg[8]_i_1_n_3 ,\counter_reg[8]_i_1_n_4 ,\counter_reg[8]_i_1_n_5 ,\counter_reg[8]_i_1_n_6 ,\counter_reg[8]_i_1_n_7 }),
        .DI({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .O({\counter_reg[8]_i_1_n_8 ,\counter_reg[8]_i_1_n_9 ,\counter_reg[8]_i_1_n_10 ,\counter_reg[8]_i_1_n_11 ,\counter_reg[8]_i_1_n_12 ,\counter_reg[8]_i_1_n_13 ,\counter_reg[8]_i_1_n_14 ,\counter_reg[8]_i_1_n_15 }),
        .S(counter_reg[15:8]));
  FDRE #(
    .INIT(1'b0)) 
    \counter_reg[9] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\counter_reg[8]_i_1_n_14 ),
        .Q(counter_reg[9]),
        .R(RST_ACTIVE));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_trace_fifo_i fifo_minmax
       (.CO(max_ctr1),
        .E(max_ctr0),
        .RST_ACTIVE(RST_ACTIVE),
        .S({fifo_minmax_n_1,fifo_minmax_n_2,fifo_minmax_n_3,fifo_minmax_n_4,fifo_minmax_n_5,fifo_minmax_n_6,fifo_minmax_n_7,fifo_minmax_n_8}),
        .ap_continue_reg(ap_continue_reg),
        .ap_done_reg(ap_done_reg),
        .\counter_reg[15] ({fifo_minmax_n_49,fifo_minmax_n_50,fifo_minmax_n_51,fifo_minmax_n_52,fifo_minmax_n_53,fifo_minmax_n_54,fifo_minmax_n_55,fifo_minmax_n_56}),
        .\counter_reg[23] ({fifo_minmax_n_41,fifo_minmax_n_42,fifo_minmax_n_43,fifo_minmax_n_44,fifo_minmax_n_45,fifo_minmax_n_46,fifo_minmax_n_47,fifo_minmax_n_48}),
        .\counter_reg[31] ({fifo_minmax_n_33,fifo_minmax_n_34,fifo_minmax_n_35,fifo_minmax_n_36,fifo_minmax_n_37,fifo_minmax_n_38,fifo_minmax_n_39,fifo_minmax_n_40}),
        .\counter_reg[39] ({fifo_minmax_n_25,fifo_minmax_n_26,fifo_minmax_n_27,fifo_minmax_n_28,fifo_minmax_n_29,fifo_minmax_n_30,fifo_minmax_n_31,fifo_minmax_n_32}),
        .\counter_reg[47] ({fifo_minmax_n_17,fifo_minmax_n_18,fifo_minmax_n_19,fifo_minmax_n_20,fifo_minmax_n_21,fifo_minmax_n_22,fifo_minmax_n_23,fifo_minmax_n_24}),
        .\counter_reg[55] ({fifo_minmax_n_9,fifo_minmax_n_10,fifo_minmax_n_11,fifo_minmax_n_12,fifo_minmax_n_13,fifo_minmax_n_14,fifo_minmax_n_15,fifo_minmax_n_16}),
        .\counter_reg[7] ({fifo_minmax_n_57,fifo_minmax_n_58,fifo_minmax_n_59,fifo_minmax_n_60,fifo_minmax_n_61,fifo_minmax_n_62,fifo_minmax_n_63,fifo_minmax_n_64}),
        .dataflow_en(dataflow_en),
        .din(counter_reg),
        .empty(empty),
        .\gen_fwft.empty_fwft_i_reg (min_ctr0),
        .\min_ctr_reg[0] (min_ctr1),
        .mon_clk(mon_clk),
        .rd_en(rd_en),
        .start_pulse(start_pulse));
  (* COMPARATOR_THRESHOLD = "11" *) 
  CARRY8 max_ctr1_carry
       (.CI(1'b0),
        .CI_TOP(1'b0),
        .CO({max_ctr1_carry_n_0,max_ctr1_carry_n_1,max_ctr1_carry_n_2,max_ctr1_carry_n_3,max_ctr1_carry_n_4,max_ctr1_carry_n_5,max_ctr1_carry_n_6,max_ctr1_carry_n_7}),
        .DI({max_ctr1_carry_i_1_n_0,max_ctr1_carry_i_2_n_0,max_ctr1_carry_i_3_n_0,max_ctr1_carry_i_4_n_0,max_ctr1_carry_i_5_n_0,max_ctr1_carry_i_6_n_0,max_ctr1_carry_i_7_n_0,max_ctr1_carry_i_8_n_0}),
        .O(NLW_max_ctr1_carry_O_UNCONNECTED[7:0]),
        .S({max_ctr1_carry_i_9_n_0,max_ctr1_carry_i_10_n_0,max_ctr1_carry_i_11_n_0,max_ctr1_carry_i_12_n_0,max_ctr1_carry_i_13_n_0,max_ctr1_carry_i_14_n_0,max_ctr1_carry_i_15_n_0,max_ctr1_carry_i_16_n_0}));
  (* COMPARATOR_THRESHOLD = "11" *) 
  CARRY8 max_ctr1_carry__0
       (.CI(max_ctr1_carry_n_0),
        .CI_TOP(1'b0),
        .CO({max_ctr1_carry__0_n_0,max_ctr1_carry__0_n_1,max_ctr1_carry__0_n_2,max_ctr1_carry__0_n_3,max_ctr1_carry__0_n_4,max_ctr1_carry__0_n_5,max_ctr1_carry__0_n_6,max_ctr1_carry__0_n_7}),
        .DI({max_ctr1_carry__0_i_1_n_0,max_ctr1_carry__0_i_2_n_0,max_ctr1_carry__0_i_3_n_0,max_ctr1_carry__0_i_4_n_0,max_ctr1_carry__0_i_5_n_0,max_ctr1_carry__0_i_6_n_0,max_ctr1_carry__0_i_7_n_0,max_ctr1_carry__0_i_8_n_0}),
        .O(NLW_max_ctr1_carry__0_O_UNCONNECTED[7:0]),
        .S({max_ctr1_carry__0_i_9_n_0,max_ctr1_carry__0_i_10_n_0,max_ctr1_carry__0_i_11_n_0,max_ctr1_carry__0_i_12_n_0,max_ctr1_carry__0_i_13_n_0,max_ctr1_carry__0_i_14_n_0,max_ctr1_carry__0_i_15_n_0,max_ctr1_carry__0_i_16_n_0}));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry__0_i_1
       (.I0(tx_length[30]),
        .I1(\max_ctr_reg[63]_0 [30]),
        .I2(\max_ctr_reg[63]_0 [31]),
        .I3(tx_length[31]),
        .O(max_ctr1_carry__0_i_1_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry__0_i_10
       (.I0(\max_ctr_reg[63]_0 [29]),
        .I1(tx_length[29]),
        .I2(\max_ctr_reg[63]_0 [28]),
        .I3(tx_length[28]),
        .O(max_ctr1_carry__0_i_10_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry__0_i_11
       (.I0(\max_ctr_reg[63]_0 [27]),
        .I1(tx_length[27]),
        .I2(\max_ctr_reg[63]_0 [26]),
        .I3(tx_length[26]),
        .O(max_ctr1_carry__0_i_11_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry__0_i_12
       (.I0(\max_ctr_reg[63]_0 [25]),
        .I1(tx_length[25]),
        .I2(\max_ctr_reg[63]_0 [24]),
        .I3(tx_length[24]),
        .O(max_ctr1_carry__0_i_12_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry__0_i_13
       (.I0(\max_ctr_reg[63]_0 [23]),
        .I1(tx_length[23]),
        .I2(\max_ctr_reg[63]_0 [22]),
        .I3(tx_length[22]),
        .O(max_ctr1_carry__0_i_13_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry__0_i_14
       (.I0(\max_ctr_reg[63]_0 [21]),
        .I1(tx_length[21]),
        .I2(\max_ctr_reg[63]_0 [20]),
        .I3(tx_length[20]),
        .O(max_ctr1_carry__0_i_14_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry__0_i_15
       (.I0(\max_ctr_reg[63]_0 [19]),
        .I1(tx_length[19]),
        .I2(\max_ctr_reg[63]_0 [18]),
        .I3(tx_length[18]),
        .O(max_ctr1_carry__0_i_15_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry__0_i_16
       (.I0(\max_ctr_reg[63]_0 [17]),
        .I1(tx_length[17]),
        .I2(\max_ctr_reg[63]_0 [16]),
        .I3(tx_length[16]),
        .O(max_ctr1_carry__0_i_16_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry__0_i_2
       (.I0(tx_length[28]),
        .I1(\max_ctr_reg[63]_0 [28]),
        .I2(\max_ctr_reg[63]_0 [29]),
        .I3(tx_length[29]),
        .O(max_ctr1_carry__0_i_2_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry__0_i_3
       (.I0(tx_length[26]),
        .I1(\max_ctr_reg[63]_0 [26]),
        .I2(\max_ctr_reg[63]_0 [27]),
        .I3(tx_length[27]),
        .O(max_ctr1_carry__0_i_3_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry__0_i_4
       (.I0(tx_length[24]),
        .I1(\max_ctr_reg[63]_0 [24]),
        .I2(\max_ctr_reg[63]_0 [25]),
        .I3(tx_length[25]),
        .O(max_ctr1_carry__0_i_4_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry__0_i_5
       (.I0(tx_length[22]),
        .I1(\max_ctr_reg[63]_0 [22]),
        .I2(\max_ctr_reg[63]_0 [23]),
        .I3(tx_length[23]),
        .O(max_ctr1_carry__0_i_5_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry__0_i_6
       (.I0(tx_length[20]),
        .I1(\max_ctr_reg[63]_0 [20]),
        .I2(\max_ctr_reg[63]_0 [21]),
        .I3(tx_length[21]),
        .O(max_ctr1_carry__0_i_6_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry__0_i_7
       (.I0(tx_length[18]),
        .I1(\max_ctr_reg[63]_0 [18]),
        .I2(\max_ctr_reg[63]_0 [19]),
        .I3(tx_length[19]),
        .O(max_ctr1_carry__0_i_7_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry__0_i_8
       (.I0(tx_length[16]),
        .I1(\max_ctr_reg[63]_0 [16]),
        .I2(\max_ctr_reg[63]_0 [17]),
        .I3(tx_length[17]),
        .O(max_ctr1_carry__0_i_8_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry__0_i_9
       (.I0(\max_ctr_reg[63]_0 [31]),
        .I1(tx_length[31]),
        .I2(\max_ctr_reg[63]_0 [30]),
        .I3(tx_length[30]),
        .O(max_ctr1_carry__0_i_9_n_0));
  (* COMPARATOR_THRESHOLD = "11" *) 
  CARRY8 max_ctr1_carry__1
       (.CI(max_ctr1_carry__0_n_0),
        .CI_TOP(1'b0),
        .CO({max_ctr1_carry__1_n_0,max_ctr1_carry__1_n_1,max_ctr1_carry__1_n_2,max_ctr1_carry__1_n_3,max_ctr1_carry__1_n_4,max_ctr1_carry__1_n_5,max_ctr1_carry__1_n_6,max_ctr1_carry__1_n_7}),
        .DI({max_ctr1_carry__1_i_1_n_0,max_ctr1_carry__1_i_2_n_0,max_ctr1_carry__1_i_3_n_0,max_ctr1_carry__1_i_4_n_0,max_ctr1_carry__1_i_5_n_0,max_ctr1_carry__1_i_6_n_0,max_ctr1_carry__1_i_7_n_0,max_ctr1_carry__1_i_8_n_0}),
        .O(NLW_max_ctr1_carry__1_O_UNCONNECTED[7:0]),
        .S({max_ctr1_carry__1_i_9_n_0,max_ctr1_carry__1_i_10_n_0,max_ctr1_carry__1_i_11_n_0,max_ctr1_carry__1_i_12_n_0,max_ctr1_carry__1_i_13_n_0,max_ctr1_carry__1_i_14_n_0,max_ctr1_carry__1_i_15_n_0,max_ctr1_carry__1_i_16_n_0}));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry__1_i_1
       (.I0(tx_length[46]),
        .I1(\max_ctr_reg[63]_0 [46]),
        .I2(\max_ctr_reg[63]_0 [47]),
        .I3(tx_length[47]),
        .O(max_ctr1_carry__1_i_1_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry__1_i_10
       (.I0(\max_ctr_reg[63]_0 [45]),
        .I1(tx_length[45]),
        .I2(\max_ctr_reg[63]_0 [44]),
        .I3(tx_length[44]),
        .O(max_ctr1_carry__1_i_10_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry__1_i_11
       (.I0(\max_ctr_reg[63]_0 [43]),
        .I1(tx_length[43]),
        .I2(\max_ctr_reg[63]_0 [42]),
        .I3(tx_length[42]),
        .O(max_ctr1_carry__1_i_11_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry__1_i_12
       (.I0(\max_ctr_reg[63]_0 [41]),
        .I1(tx_length[41]),
        .I2(\max_ctr_reg[63]_0 [40]),
        .I3(tx_length[40]),
        .O(max_ctr1_carry__1_i_12_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry__1_i_13
       (.I0(\max_ctr_reg[63]_0 [39]),
        .I1(tx_length[39]),
        .I2(\max_ctr_reg[63]_0 [38]),
        .I3(tx_length[38]),
        .O(max_ctr1_carry__1_i_13_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry__1_i_14
       (.I0(\max_ctr_reg[63]_0 [37]),
        .I1(tx_length[37]),
        .I2(\max_ctr_reg[63]_0 [36]),
        .I3(tx_length[36]),
        .O(max_ctr1_carry__1_i_14_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry__1_i_15
       (.I0(\max_ctr_reg[63]_0 [35]),
        .I1(tx_length[35]),
        .I2(\max_ctr_reg[63]_0 [34]),
        .I3(tx_length[34]),
        .O(max_ctr1_carry__1_i_15_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry__1_i_16
       (.I0(\max_ctr_reg[63]_0 [33]),
        .I1(tx_length[33]),
        .I2(\max_ctr_reg[63]_0 [32]),
        .I3(tx_length[32]),
        .O(max_ctr1_carry__1_i_16_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry__1_i_2
       (.I0(tx_length[44]),
        .I1(\max_ctr_reg[63]_0 [44]),
        .I2(\max_ctr_reg[63]_0 [45]),
        .I3(tx_length[45]),
        .O(max_ctr1_carry__1_i_2_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry__1_i_3
       (.I0(tx_length[42]),
        .I1(\max_ctr_reg[63]_0 [42]),
        .I2(\max_ctr_reg[63]_0 [43]),
        .I3(tx_length[43]),
        .O(max_ctr1_carry__1_i_3_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry__1_i_4
       (.I0(tx_length[40]),
        .I1(\max_ctr_reg[63]_0 [40]),
        .I2(\max_ctr_reg[63]_0 [41]),
        .I3(tx_length[41]),
        .O(max_ctr1_carry__1_i_4_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry__1_i_5
       (.I0(tx_length[38]),
        .I1(\max_ctr_reg[63]_0 [38]),
        .I2(\max_ctr_reg[63]_0 [39]),
        .I3(tx_length[39]),
        .O(max_ctr1_carry__1_i_5_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry__1_i_6
       (.I0(tx_length[36]),
        .I1(\max_ctr_reg[63]_0 [36]),
        .I2(\max_ctr_reg[63]_0 [37]),
        .I3(tx_length[37]),
        .O(max_ctr1_carry__1_i_6_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry__1_i_7
       (.I0(tx_length[34]),
        .I1(\max_ctr_reg[63]_0 [34]),
        .I2(\max_ctr_reg[63]_0 [35]),
        .I3(tx_length[35]),
        .O(max_ctr1_carry__1_i_7_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry__1_i_8
       (.I0(tx_length[32]),
        .I1(\max_ctr_reg[63]_0 [32]),
        .I2(\max_ctr_reg[63]_0 [33]),
        .I3(tx_length[33]),
        .O(max_ctr1_carry__1_i_8_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry__1_i_9
       (.I0(\max_ctr_reg[63]_0 [47]),
        .I1(tx_length[47]),
        .I2(\max_ctr_reg[63]_0 [46]),
        .I3(tx_length[46]),
        .O(max_ctr1_carry__1_i_9_n_0));
  (* COMPARATOR_THRESHOLD = "11" *) 
  CARRY8 max_ctr1_carry__2
       (.CI(max_ctr1_carry__1_n_0),
        .CI_TOP(1'b0),
        .CO({max_ctr1,max_ctr1_carry__2_n_1,max_ctr1_carry__2_n_2,max_ctr1_carry__2_n_3,max_ctr1_carry__2_n_4,max_ctr1_carry__2_n_5,max_ctr1_carry__2_n_6,max_ctr1_carry__2_n_7}),
        .DI({max_ctr1_carry__2_i_1_n_0,max_ctr1_carry__2_i_2_n_0,max_ctr1_carry__2_i_3_n_0,max_ctr1_carry__2_i_4_n_0,max_ctr1_carry__2_i_5_n_0,max_ctr1_carry__2_i_6_n_0,max_ctr1_carry__2_i_7_n_0,max_ctr1_carry__2_i_8_n_0}),
        .O(NLW_max_ctr1_carry__2_O_UNCONNECTED[7:0]),
        .S({max_ctr1_carry__2_i_9_n_0,max_ctr1_carry__2_i_10_n_0,max_ctr1_carry__2_i_11_n_0,max_ctr1_carry__2_i_12_n_0,max_ctr1_carry__2_i_13_n_0,max_ctr1_carry__2_i_14_n_0,max_ctr1_carry__2_i_15_n_0,max_ctr1_carry__2_i_16_n_0}));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry__2_i_1
       (.I0(tx_length[62]),
        .I1(\max_ctr_reg[63]_0 [62]),
        .I2(\max_ctr_reg[63]_0 [63]),
        .I3(tx_length[63]),
        .O(max_ctr1_carry__2_i_1_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry__2_i_10
       (.I0(\max_ctr_reg[63]_0 [61]),
        .I1(tx_length[61]),
        .I2(\max_ctr_reg[63]_0 [60]),
        .I3(tx_length[60]),
        .O(max_ctr1_carry__2_i_10_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry__2_i_11
       (.I0(\max_ctr_reg[63]_0 [59]),
        .I1(tx_length[59]),
        .I2(\max_ctr_reg[63]_0 [58]),
        .I3(tx_length[58]),
        .O(max_ctr1_carry__2_i_11_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry__2_i_12
       (.I0(\max_ctr_reg[63]_0 [57]),
        .I1(tx_length[57]),
        .I2(\max_ctr_reg[63]_0 [56]),
        .I3(tx_length[56]),
        .O(max_ctr1_carry__2_i_12_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry__2_i_13
       (.I0(\max_ctr_reg[63]_0 [55]),
        .I1(tx_length[55]),
        .I2(\max_ctr_reg[63]_0 [54]),
        .I3(tx_length[54]),
        .O(max_ctr1_carry__2_i_13_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry__2_i_14
       (.I0(\max_ctr_reg[63]_0 [53]),
        .I1(tx_length[53]),
        .I2(\max_ctr_reg[63]_0 [52]),
        .I3(tx_length[52]),
        .O(max_ctr1_carry__2_i_14_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry__2_i_15
       (.I0(\max_ctr_reg[63]_0 [51]),
        .I1(tx_length[51]),
        .I2(\max_ctr_reg[63]_0 [50]),
        .I3(tx_length[50]),
        .O(max_ctr1_carry__2_i_15_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry__2_i_16
       (.I0(\max_ctr_reg[63]_0 [49]),
        .I1(tx_length[49]),
        .I2(\max_ctr_reg[63]_0 [48]),
        .I3(tx_length[48]),
        .O(max_ctr1_carry__2_i_16_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry__2_i_2
       (.I0(tx_length[60]),
        .I1(\max_ctr_reg[63]_0 [60]),
        .I2(\max_ctr_reg[63]_0 [61]),
        .I3(tx_length[61]),
        .O(max_ctr1_carry__2_i_2_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry__2_i_3
       (.I0(tx_length[58]),
        .I1(\max_ctr_reg[63]_0 [58]),
        .I2(\max_ctr_reg[63]_0 [59]),
        .I3(tx_length[59]),
        .O(max_ctr1_carry__2_i_3_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry__2_i_4
       (.I0(tx_length[56]),
        .I1(\max_ctr_reg[63]_0 [56]),
        .I2(\max_ctr_reg[63]_0 [57]),
        .I3(tx_length[57]),
        .O(max_ctr1_carry__2_i_4_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry__2_i_5
       (.I0(tx_length[54]),
        .I1(\max_ctr_reg[63]_0 [54]),
        .I2(\max_ctr_reg[63]_0 [55]),
        .I3(tx_length[55]),
        .O(max_ctr1_carry__2_i_5_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry__2_i_6
       (.I0(tx_length[52]),
        .I1(\max_ctr_reg[63]_0 [52]),
        .I2(\max_ctr_reg[63]_0 [53]),
        .I3(tx_length[53]),
        .O(max_ctr1_carry__2_i_6_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry__2_i_7
       (.I0(tx_length[50]),
        .I1(\max_ctr_reg[63]_0 [50]),
        .I2(\max_ctr_reg[63]_0 [51]),
        .I3(tx_length[51]),
        .O(max_ctr1_carry__2_i_7_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry__2_i_8
       (.I0(tx_length[48]),
        .I1(\max_ctr_reg[63]_0 [48]),
        .I2(\max_ctr_reg[63]_0 [49]),
        .I3(tx_length[49]),
        .O(max_ctr1_carry__2_i_8_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry__2_i_9
       (.I0(\max_ctr_reg[63]_0 [63]),
        .I1(tx_length[63]),
        .I2(\max_ctr_reg[63]_0 [62]),
        .I3(tx_length[62]),
        .O(max_ctr1_carry__2_i_9_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry_i_1
       (.I0(tx_length[14]),
        .I1(\max_ctr_reg[63]_0 [14]),
        .I2(\max_ctr_reg[63]_0 [15]),
        .I3(tx_length[15]),
        .O(max_ctr1_carry_i_1_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry_i_10
       (.I0(\max_ctr_reg[63]_0 [13]),
        .I1(tx_length[13]),
        .I2(\max_ctr_reg[63]_0 [12]),
        .I3(tx_length[12]),
        .O(max_ctr1_carry_i_10_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry_i_11
       (.I0(\max_ctr_reg[63]_0 [11]),
        .I1(tx_length[11]),
        .I2(\max_ctr_reg[63]_0 [10]),
        .I3(tx_length[10]),
        .O(max_ctr1_carry_i_11_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry_i_12
       (.I0(\max_ctr_reg[63]_0 [9]),
        .I1(tx_length[9]),
        .I2(\max_ctr_reg[63]_0 [8]),
        .I3(tx_length[8]),
        .O(max_ctr1_carry_i_12_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry_i_13
       (.I0(\max_ctr_reg[63]_0 [7]),
        .I1(tx_length[7]),
        .I2(\max_ctr_reg[63]_0 [6]),
        .I3(tx_length[6]),
        .O(max_ctr1_carry_i_13_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry_i_14
       (.I0(\max_ctr_reg[63]_0 [5]),
        .I1(tx_length[5]),
        .I2(\max_ctr_reg[63]_0 [4]),
        .I3(tx_length[4]),
        .O(max_ctr1_carry_i_14_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry_i_15
       (.I0(\max_ctr_reg[63]_0 [3]),
        .I1(tx_length[3]),
        .I2(\max_ctr_reg[63]_0 [2]),
        .I3(tx_length[2]),
        .O(max_ctr1_carry_i_15_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry_i_16
       (.I0(\max_ctr_reg[63]_0 [1]),
        .I1(tx_length[1]),
        .I2(\max_ctr_reg[63]_0 [0]),
        .I3(tx_length[0]),
        .O(max_ctr1_carry_i_16_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry_i_2
       (.I0(tx_length[12]),
        .I1(\max_ctr_reg[63]_0 [12]),
        .I2(\max_ctr_reg[63]_0 [13]),
        .I3(tx_length[13]),
        .O(max_ctr1_carry_i_2_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry_i_3
       (.I0(tx_length[10]),
        .I1(\max_ctr_reg[63]_0 [10]),
        .I2(\max_ctr_reg[63]_0 [11]),
        .I3(tx_length[11]),
        .O(max_ctr1_carry_i_3_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry_i_4
       (.I0(tx_length[8]),
        .I1(\max_ctr_reg[63]_0 [8]),
        .I2(\max_ctr_reg[63]_0 [9]),
        .I3(tx_length[9]),
        .O(max_ctr1_carry_i_4_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry_i_5
       (.I0(tx_length[6]),
        .I1(\max_ctr_reg[63]_0 [6]),
        .I2(\max_ctr_reg[63]_0 [7]),
        .I3(tx_length[7]),
        .O(max_ctr1_carry_i_5_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry_i_6
       (.I0(tx_length[4]),
        .I1(\max_ctr_reg[63]_0 [4]),
        .I2(\max_ctr_reg[63]_0 [5]),
        .I3(tx_length[5]),
        .O(max_ctr1_carry_i_6_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry_i_7
       (.I0(tx_length[2]),
        .I1(\max_ctr_reg[63]_0 [2]),
        .I2(\max_ctr_reg[63]_0 [3]),
        .I3(tx_length[3]),
        .O(max_ctr1_carry_i_7_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    max_ctr1_carry_i_8
       (.I0(tx_length[0]),
        .I1(\max_ctr_reg[63]_0 [0]),
        .I2(\max_ctr_reg[63]_0 [1]),
        .I3(tx_length[1]),
        .O(max_ctr1_carry_i_8_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    max_ctr1_carry_i_9
       (.I0(\max_ctr_reg[63]_0 [15]),
        .I1(tx_length[15]),
        .I2(\max_ctr_reg[63]_0 [14]),
        .I3(tx_length[14]),
        .O(max_ctr1_carry_i_9_n_0));
  FDRE \max_ctr_reg[0] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[0]),
        .Q(\max_ctr_reg[63]_0 [0]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[10] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[10]),
        .Q(\max_ctr_reg[63]_0 [10]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[11] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[11]),
        .Q(\max_ctr_reg[63]_0 [11]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[12] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[12]),
        .Q(\max_ctr_reg[63]_0 [12]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[13] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[13]),
        .Q(\max_ctr_reg[63]_0 [13]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[14] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[14]),
        .Q(\max_ctr_reg[63]_0 [14]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[15] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[15]),
        .Q(\max_ctr_reg[63]_0 [15]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[16] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[16]),
        .Q(\max_ctr_reg[63]_0 [16]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[17] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[17]),
        .Q(\max_ctr_reg[63]_0 [17]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[18] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[18]),
        .Q(\max_ctr_reg[63]_0 [18]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[19] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[19]),
        .Q(\max_ctr_reg[63]_0 [19]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[1] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[1]),
        .Q(\max_ctr_reg[63]_0 [1]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[20] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[20]),
        .Q(\max_ctr_reg[63]_0 [20]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[21] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[21]),
        .Q(\max_ctr_reg[63]_0 [21]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[22] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[22]),
        .Q(\max_ctr_reg[63]_0 [22]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[23] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[23]),
        .Q(\max_ctr_reg[63]_0 [23]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[24] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[24]),
        .Q(\max_ctr_reg[63]_0 [24]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[25] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[25]),
        .Q(\max_ctr_reg[63]_0 [25]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[26] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[26]),
        .Q(\max_ctr_reg[63]_0 [26]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[27] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[27]),
        .Q(\max_ctr_reg[63]_0 [27]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[28] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[28]),
        .Q(\max_ctr_reg[63]_0 [28]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[29] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[29]),
        .Q(\max_ctr_reg[63]_0 [29]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[2] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[2]),
        .Q(\max_ctr_reg[63]_0 [2]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[30] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[30]),
        .Q(\max_ctr_reg[63]_0 [30]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[31] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[31]),
        .Q(\max_ctr_reg[63]_0 [31]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[32] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[32]),
        .Q(\max_ctr_reg[63]_0 [32]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[33] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[33]),
        .Q(\max_ctr_reg[63]_0 [33]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[34] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[34]),
        .Q(\max_ctr_reg[63]_0 [34]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[35] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[35]),
        .Q(\max_ctr_reg[63]_0 [35]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[36] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[36]),
        .Q(\max_ctr_reg[63]_0 [36]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[37] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[37]),
        .Q(\max_ctr_reg[63]_0 [37]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[38] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[38]),
        .Q(\max_ctr_reg[63]_0 [38]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[39] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[39]),
        .Q(\max_ctr_reg[63]_0 [39]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[3] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[3]),
        .Q(\max_ctr_reg[63]_0 [3]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[40] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[40]),
        .Q(\max_ctr_reg[63]_0 [40]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[41] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[41]),
        .Q(\max_ctr_reg[63]_0 [41]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[42] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[42]),
        .Q(\max_ctr_reg[63]_0 [42]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[43] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[43]),
        .Q(\max_ctr_reg[63]_0 [43]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[44] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[44]),
        .Q(\max_ctr_reg[63]_0 [44]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[45] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[45]),
        .Q(\max_ctr_reg[63]_0 [45]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[46] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[46]),
        .Q(\max_ctr_reg[63]_0 [46]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[47] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[47]),
        .Q(\max_ctr_reg[63]_0 [47]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[48] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[48]),
        .Q(\max_ctr_reg[63]_0 [48]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[49] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[49]),
        .Q(\max_ctr_reg[63]_0 [49]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[4] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[4]),
        .Q(\max_ctr_reg[63]_0 [4]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[50] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[50]),
        .Q(\max_ctr_reg[63]_0 [50]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[51] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[51]),
        .Q(\max_ctr_reg[63]_0 [51]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[52] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[52]),
        .Q(\max_ctr_reg[63]_0 [52]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[53] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[53]),
        .Q(\max_ctr_reg[63]_0 [53]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[54] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[54]),
        .Q(\max_ctr_reg[63]_0 [54]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[55] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[55]),
        .Q(\max_ctr_reg[63]_0 [55]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[56] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[56]),
        .Q(\max_ctr_reg[63]_0 [56]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[57] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[57]),
        .Q(\max_ctr_reg[63]_0 [57]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[58] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[58]),
        .Q(\max_ctr_reg[63]_0 [58]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[59] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[59]),
        .Q(\max_ctr_reg[63]_0 [59]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[5] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[5]),
        .Q(\max_ctr_reg[63]_0 [5]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[60] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[60]),
        .Q(\max_ctr_reg[63]_0 [60]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[61] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[61]),
        .Q(\max_ctr_reg[63]_0 [61]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[62] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[62]),
        .Q(\max_ctr_reg[63]_0 [62]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[63] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[63]),
        .Q(\max_ctr_reg[63]_0 [63]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[6] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[6]),
        .Q(\max_ctr_reg[63]_0 [6]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[7] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[7]),
        .Q(\max_ctr_reg[63]_0 [7]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[8] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[8]),
        .Q(\max_ctr_reg[63]_0 [8]),
        .R(RST_ACTIVE));
  FDRE \max_ctr_reg[9] 
       (.C(mon_clk),
        .CE(max_ctr0),
        .D(tx_length[9]),
        .Q(\max_ctr_reg[63]_0 [9]),
        .R(RST_ACTIVE));
  (* COMPARATOR_THRESHOLD = "11" *) 
  CARRY8 min_ctr1_carry
       (.CI(1'b0),
        .CI_TOP(1'b0),
        .CO({min_ctr1_carry_n_0,min_ctr1_carry_n_1,min_ctr1_carry_n_2,min_ctr1_carry_n_3,min_ctr1_carry_n_4,min_ctr1_carry_n_5,min_ctr1_carry_n_6,min_ctr1_carry_n_7}),
        .DI({min_ctr1_carry_i_1_n_0,min_ctr1_carry_i_2_n_0,min_ctr1_carry_i_3_n_0,min_ctr1_carry_i_4_n_0,min_ctr1_carry_i_5_n_0,min_ctr1_carry_i_6_n_0,min_ctr1_carry_i_7_n_0,min_ctr1_carry_i_8_n_0}),
        .O(NLW_min_ctr1_carry_O_UNCONNECTED[7:0]),
        .S({min_ctr1_carry_i_9_n_0,min_ctr1_carry_i_10_n_0,min_ctr1_carry_i_11_n_0,min_ctr1_carry_i_12_n_0,min_ctr1_carry_i_13_n_0,min_ctr1_carry_i_14_n_0,min_ctr1_carry_i_15_n_0,min_ctr1_carry_i_16_n_0}));
  (* COMPARATOR_THRESHOLD = "11" *) 
  CARRY8 min_ctr1_carry__0
       (.CI(min_ctr1_carry_n_0),
        .CI_TOP(1'b0),
        .CO({min_ctr1_carry__0_n_0,min_ctr1_carry__0_n_1,min_ctr1_carry__0_n_2,min_ctr1_carry__0_n_3,min_ctr1_carry__0_n_4,min_ctr1_carry__0_n_5,min_ctr1_carry__0_n_6,min_ctr1_carry__0_n_7}),
        .DI({min_ctr1_carry__0_i_1_n_0,min_ctr1_carry__0_i_2_n_0,min_ctr1_carry__0_i_3_n_0,min_ctr1_carry__0_i_4_n_0,min_ctr1_carry__0_i_5_n_0,min_ctr1_carry__0_i_6_n_0,min_ctr1_carry__0_i_7_n_0,min_ctr1_carry__0_i_8_n_0}),
        .O(NLW_min_ctr1_carry__0_O_UNCONNECTED[7:0]),
        .S({min_ctr1_carry__0_i_9_n_0,min_ctr1_carry__0_i_10_n_0,min_ctr1_carry__0_i_11_n_0,min_ctr1_carry__0_i_12_n_0,min_ctr1_carry__0_i_13_n_0,min_ctr1_carry__0_i_14_n_0,min_ctr1_carry__0_i_15_n_0,min_ctr1_carry__0_i_16_n_0}));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry__0_i_1
       (.I0(Q[30]),
        .I1(tx_length[30]),
        .I2(tx_length[31]),
        .I3(Q[31]),
        .O(min_ctr1_carry__0_i_1_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry__0_i_10
       (.I0(tx_length[29]),
        .I1(Q[29]),
        .I2(tx_length[28]),
        .I3(Q[28]),
        .O(min_ctr1_carry__0_i_10_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry__0_i_11
       (.I0(tx_length[27]),
        .I1(Q[27]),
        .I2(tx_length[26]),
        .I3(Q[26]),
        .O(min_ctr1_carry__0_i_11_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry__0_i_12
       (.I0(tx_length[25]),
        .I1(Q[25]),
        .I2(tx_length[24]),
        .I3(Q[24]),
        .O(min_ctr1_carry__0_i_12_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry__0_i_13
       (.I0(tx_length[23]),
        .I1(Q[23]),
        .I2(tx_length[22]),
        .I3(Q[22]),
        .O(min_ctr1_carry__0_i_13_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry__0_i_14
       (.I0(tx_length[21]),
        .I1(Q[21]),
        .I2(tx_length[20]),
        .I3(Q[20]),
        .O(min_ctr1_carry__0_i_14_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry__0_i_15
       (.I0(tx_length[19]),
        .I1(Q[19]),
        .I2(tx_length[18]),
        .I3(Q[18]),
        .O(min_ctr1_carry__0_i_15_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry__0_i_16
       (.I0(tx_length[17]),
        .I1(Q[17]),
        .I2(tx_length[16]),
        .I3(Q[16]),
        .O(min_ctr1_carry__0_i_16_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry__0_i_2
       (.I0(Q[28]),
        .I1(tx_length[28]),
        .I2(tx_length[29]),
        .I3(Q[29]),
        .O(min_ctr1_carry__0_i_2_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry__0_i_3
       (.I0(Q[26]),
        .I1(tx_length[26]),
        .I2(tx_length[27]),
        .I3(Q[27]),
        .O(min_ctr1_carry__0_i_3_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry__0_i_4
       (.I0(Q[24]),
        .I1(tx_length[24]),
        .I2(tx_length[25]),
        .I3(Q[25]),
        .O(min_ctr1_carry__0_i_4_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry__0_i_5
       (.I0(Q[22]),
        .I1(tx_length[22]),
        .I2(tx_length[23]),
        .I3(Q[23]),
        .O(min_ctr1_carry__0_i_5_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry__0_i_6
       (.I0(Q[20]),
        .I1(tx_length[20]),
        .I2(tx_length[21]),
        .I3(Q[21]),
        .O(min_ctr1_carry__0_i_6_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry__0_i_7
       (.I0(Q[18]),
        .I1(tx_length[18]),
        .I2(tx_length[19]),
        .I3(Q[19]),
        .O(min_ctr1_carry__0_i_7_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry__0_i_8
       (.I0(Q[16]),
        .I1(tx_length[16]),
        .I2(tx_length[17]),
        .I3(Q[17]),
        .O(min_ctr1_carry__0_i_8_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry__0_i_9
       (.I0(tx_length[31]),
        .I1(Q[31]),
        .I2(tx_length[30]),
        .I3(Q[30]),
        .O(min_ctr1_carry__0_i_9_n_0));
  (* COMPARATOR_THRESHOLD = "11" *) 
  CARRY8 min_ctr1_carry__1
       (.CI(min_ctr1_carry__0_n_0),
        .CI_TOP(1'b0),
        .CO({min_ctr1_carry__1_n_0,min_ctr1_carry__1_n_1,min_ctr1_carry__1_n_2,min_ctr1_carry__1_n_3,min_ctr1_carry__1_n_4,min_ctr1_carry__1_n_5,min_ctr1_carry__1_n_6,min_ctr1_carry__1_n_7}),
        .DI({min_ctr1_carry__1_i_1_n_0,min_ctr1_carry__1_i_2_n_0,min_ctr1_carry__1_i_3_n_0,min_ctr1_carry__1_i_4_n_0,min_ctr1_carry__1_i_5_n_0,min_ctr1_carry__1_i_6_n_0,min_ctr1_carry__1_i_7_n_0,min_ctr1_carry__1_i_8_n_0}),
        .O(NLW_min_ctr1_carry__1_O_UNCONNECTED[7:0]),
        .S({min_ctr1_carry__1_i_9_n_0,min_ctr1_carry__1_i_10_n_0,min_ctr1_carry__1_i_11_n_0,min_ctr1_carry__1_i_12_n_0,min_ctr1_carry__1_i_13_n_0,min_ctr1_carry__1_i_14_n_0,min_ctr1_carry__1_i_15_n_0,min_ctr1_carry__1_i_16_n_0}));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry__1_i_1
       (.I0(Q[46]),
        .I1(tx_length[46]),
        .I2(tx_length[47]),
        .I3(Q[47]),
        .O(min_ctr1_carry__1_i_1_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry__1_i_10
       (.I0(tx_length[45]),
        .I1(Q[45]),
        .I2(tx_length[44]),
        .I3(Q[44]),
        .O(min_ctr1_carry__1_i_10_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry__1_i_11
       (.I0(tx_length[43]),
        .I1(Q[43]),
        .I2(tx_length[42]),
        .I3(Q[42]),
        .O(min_ctr1_carry__1_i_11_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry__1_i_12
       (.I0(tx_length[41]),
        .I1(Q[41]),
        .I2(tx_length[40]),
        .I3(Q[40]),
        .O(min_ctr1_carry__1_i_12_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry__1_i_13
       (.I0(tx_length[39]),
        .I1(Q[39]),
        .I2(tx_length[38]),
        .I3(Q[38]),
        .O(min_ctr1_carry__1_i_13_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry__1_i_14
       (.I0(tx_length[37]),
        .I1(Q[37]),
        .I2(tx_length[36]),
        .I3(Q[36]),
        .O(min_ctr1_carry__1_i_14_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry__1_i_15
       (.I0(tx_length[35]),
        .I1(Q[35]),
        .I2(tx_length[34]),
        .I3(Q[34]),
        .O(min_ctr1_carry__1_i_15_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry__1_i_16
       (.I0(tx_length[33]),
        .I1(Q[33]),
        .I2(tx_length[32]),
        .I3(Q[32]),
        .O(min_ctr1_carry__1_i_16_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry__1_i_2
       (.I0(Q[44]),
        .I1(tx_length[44]),
        .I2(tx_length[45]),
        .I3(Q[45]),
        .O(min_ctr1_carry__1_i_2_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry__1_i_3
       (.I0(Q[42]),
        .I1(tx_length[42]),
        .I2(tx_length[43]),
        .I3(Q[43]),
        .O(min_ctr1_carry__1_i_3_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry__1_i_4
       (.I0(Q[40]),
        .I1(tx_length[40]),
        .I2(tx_length[41]),
        .I3(Q[41]),
        .O(min_ctr1_carry__1_i_4_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry__1_i_5
       (.I0(Q[38]),
        .I1(tx_length[38]),
        .I2(tx_length[39]),
        .I3(Q[39]),
        .O(min_ctr1_carry__1_i_5_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry__1_i_6
       (.I0(Q[36]),
        .I1(tx_length[36]),
        .I2(tx_length[37]),
        .I3(Q[37]),
        .O(min_ctr1_carry__1_i_6_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry__1_i_7
       (.I0(Q[34]),
        .I1(tx_length[34]),
        .I2(tx_length[35]),
        .I3(Q[35]),
        .O(min_ctr1_carry__1_i_7_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry__1_i_8
       (.I0(Q[32]),
        .I1(tx_length[32]),
        .I2(tx_length[33]),
        .I3(Q[33]),
        .O(min_ctr1_carry__1_i_8_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry__1_i_9
       (.I0(tx_length[47]),
        .I1(Q[47]),
        .I2(tx_length[46]),
        .I3(Q[46]),
        .O(min_ctr1_carry__1_i_9_n_0));
  (* COMPARATOR_THRESHOLD = "11" *) 
  CARRY8 min_ctr1_carry__2
       (.CI(min_ctr1_carry__1_n_0),
        .CI_TOP(1'b0),
        .CO({min_ctr1,min_ctr1_carry__2_n_1,min_ctr1_carry__2_n_2,min_ctr1_carry__2_n_3,min_ctr1_carry__2_n_4,min_ctr1_carry__2_n_5,min_ctr1_carry__2_n_6,min_ctr1_carry__2_n_7}),
        .DI({min_ctr1_carry__2_i_1_n_0,min_ctr1_carry__2_i_2_n_0,min_ctr1_carry__2_i_3_n_0,min_ctr1_carry__2_i_4_n_0,min_ctr1_carry__2_i_5_n_0,min_ctr1_carry__2_i_6_n_0,min_ctr1_carry__2_i_7_n_0,min_ctr1_carry__2_i_8_n_0}),
        .O(NLW_min_ctr1_carry__2_O_UNCONNECTED[7:0]),
        .S({min_ctr1_carry__2_i_9_n_0,min_ctr1_carry__2_i_10_n_0,min_ctr1_carry__2_i_11_n_0,min_ctr1_carry__2_i_12_n_0,min_ctr1_carry__2_i_13_n_0,min_ctr1_carry__2_i_14_n_0,min_ctr1_carry__2_i_15_n_0,min_ctr1_carry__2_i_16_n_0}));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry__2_i_1
       (.I0(Q[62]),
        .I1(tx_length[62]),
        .I2(tx_length[63]),
        .I3(Q[63]),
        .O(min_ctr1_carry__2_i_1_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry__2_i_10
       (.I0(tx_length[61]),
        .I1(Q[61]),
        .I2(tx_length[60]),
        .I3(Q[60]),
        .O(min_ctr1_carry__2_i_10_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry__2_i_11
       (.I0(tx_length[59]),
        .I1(Q[59]),
        .I2(tx_length[58]),
        .I3(Q[58]),
        .O(min_ctr1_carry__2_i_11_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry__2_i_12
       (.I0(tx_length[57]),
        .I1(Q[57]),
        .I2(tx_length[56]),
        .I3(Q[56]),
        .O(min_ctr1_carry__2_i_12_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry__2_i_13
       (.I0(tx_length[55]),
        .I1(Q[55]),
        .I2(tx_length[54]),
        .I3(Q[54]),
        .O(min_ctr1_carry__2_i_13_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry__2_i_14
       (.I0(tx_length[53]),
        .I1(Q[53]),
        .I2(tx_length[52]),
        .I3(Q[52]),
        .O(min_ctr1_carry__2_i_14_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry__2_i_15
       (.I0(tx_length[51]),
        .I1(Q[51]),
        .I2(tx_length[50]),
        .I3(Q[50]),
        .O(min_ctr1_carry__2_i_15_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry__2_i_16
       (.I0(tx_length[49]),
        .I1(Q[49]),
        .I2(tx_length[48]),
        .I3(Q[48]),
        .O(min_ctr1_carry__2_i_16_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry__2_i_2
       (.I0(Q[60]),
        .I1(tx_length[60]),
        .I2(tx_length[61]),
        .I3(Q[61]),
        .O(min_ctr1_carry__2_i_2_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry__2_i_3
       (.I0(Q[58]),
        .I1(tx_length[58]),
        .I2(tx_length[59]),
        .I3(Q[59]),
        .O(min_ctr1_carry__2_i_3_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry__2_i_4
       (.I0(Q[56]),
        .I1(tx_length[56]),
        .I2(tx_length[57]),
        .I3(Q[57]),
        .O(min_ctr1_carry__2_i_4_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry__2_i_5
       (.I0(Q[54]),
        .I1(tx_length[54]),
        .I2(tx_length[55]),
        .I3(Q[55]),
        .O(min_ctr1_carry__2_i_5_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry__2_i_6
       (.I0(Q[52]),
        .I1(tx_length[52]),
        .I2(tx_length[53]),
        .I3(Q[53]),
        .O(min_ctr1_carry__2_i_6_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry__2_i_7
       (.I0(Q[50]),
        .I1(tx_length[50]),
        .I2(tx_length[51]),
        .I3(Q[51]),
        .O(min_ctr1_carry__2_i_7_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry__2_i_8
       (.I0(Q[48]),
        .I1(tx_length[48]),
        .I2(tx_length[49]),
        .I3(Q[49]),
        .O(min_ctr1_carry__2_i_8_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry__2_i_9
       (.I0(tx_length[63]),
        .I1(Q[63]),
        .I2(tx_length[62]),
        .I3(Q[62]),
        .O(min_ctr1_carry__2_i_9_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry_i_1
       (.I0(Q[14]),
        .I1(tx_length[14]),
        .I2(tx_length[15]),
        .I3(Q[15]),
        .O(min_ctr1_carry_i_1_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry_i_10
       (.I0(tx_length[13]),
        .I1(Q[13]),
        .I2(tx_length[12]),
        .I3(Q[12]),
        .O(min_ctr1_carry_i_10_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry_i_11
       (.I0(tx_length[11]),
        .I1(Q[11]),
        .I2(tx_length[10]),
        .I3(Q[10]),
        .O(min_ctr1_carry_i_11_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry_i_12
       (.I0(tx_length[9]),
        .I1(Q[9]),
        .I2(tx_length[8]),
        .I3(Q[8]),
        .O(min_ctr1_carry_i_12_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry_i_13
       (.I0(tx_length[7]),
        .I1(Q[7]),
        .I2(tx_length[6]),
        .I3(Q[6]),
        .O(min_ctr1_carry_i_13_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry_i_14
       (.I0(tx_length[5]),
        .I1(Q[5]),
        .I2(tx_length[4]),
        .I3(Q[4]),
        .O(min_ctr1_carry_i_14_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry_i_15
       (.I0(tx_length[3]),
        .I1(Q[3]),
        .I2(tx_length[2]),
        .I3(Q[2]),
        .O(min_ctr1_carry_i_15_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry_i_16
       (.I0(tx_length[1]),
        .I1(Q[1]),
        .I2(tx_length[0]),
        .I3(Q[0]),
        .O(min_ctr1_carry_i_16_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry_i_2
       (.I0(Q[12]),
        .I1(tx_length[12]),
        .I2(tx_length[13]),
        .I3(Q[13]),
        .O(min_ctr1_carry_i_2_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry_i_3
       (.I0(Q[10]),
        .I1(tx_length[10]),
        .I2(tx_length[11]),
        .I3(Q[11]),
        .O(min_ctr1_carry_i_3_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry_i_4
       (.I0(Q[8]),
        .I1(tx_length[8]),
        .I2(tx_length[9]),
        .I3(Q[9]),
        .O(min_ctr1_carry_i_4_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry_i_5
       (.I0(Q[6]),
        .I1(tx_length[6]),
        .I2(tx_length[7]),
        .I3(Q[7]),
        .O(min_ctr1_carry_i_5_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry_i_6
       (.I0(Q[4]),
        .I1(tx_length[4]),
        .I2(tx_length[5]),
        .I3(Q[5]),
        .O(min_ctr1_carry_i_6_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry_i_7
       (.I0(Q[2]),
        .I1(tx_length[2]),
        .I2(tx_length[3]),
        .I3(Q[3]),
        .O(min_ctr1_carry_i_7_n_0));
  LUT4 #(
    .INIT(16'h2F02)) 
    min_ctr1_carry_i_8
       (.I0(Q[0]),
        .I1(tx_length[0]),
        .I2(tx_length[1]),
        .I3(Q[1]),
        .O(min_ctr1_carry_i_8_n_0));
  LUT4 #(
    .INIT(16'h9009)) 
    min_ctr1_carry_i_9
       (.I0(tx_length[15]),
        .I1(Q[15]),
        .I2(tx_length[14]),
        .I3(Q[14]),
        .O(min_ctr1_carry_i_9_n_0));
  FDSE \min_ctr_reg[0] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[0]),
        .Q(Q[0]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[10] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[10]),
        .Q(Q[10]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[11] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[11]),
        .Q(Q[11]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[12] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[12]),
        .Q(Q[12]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[13] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[13]),
        .Q(Q[13]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[14] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[14]),
        .Q(Q[14]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[15] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[15]),
        .Q(Q[15]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[16] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[16]),
        .Q(Q[16]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[17] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[17]),
        .Q(Q[17]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[18] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[18]),
        .Q(Q[18]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[19] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[19]),
        .Q(Q[19]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[1] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[1]),
        .Q(Q[1]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[20] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[20]),
        .Q(Q[20]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[21] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[21]),
        .Q(Q[21]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[22] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[22]),
        .Q(Q[22]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[23] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[23]),
        .Q(Q[23]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[24] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[24]),
        .Q(Q[24]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[25] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[25]),
        .Q(Q[25]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[26] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[26]),
        .Q(Q[26]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[27] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[27]),
        .Q(Q[27]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[28] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[28]),
        .Q(Q[28]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[29] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[29]),
        .Q(Q[29]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[2] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[2]),
        .Q(Q[2]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[30] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[30]),
        .Q(Q[30]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[31] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[31]),
        .Q(Q[31]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[32] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[32]),
        .Q(Q[32]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[33] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[33]),
        .Q(Q[33]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[34] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[34]),
        .Q(Q[34]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[35] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[35]),
        .Q(Q[35]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[36] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[36]),
        .Q(Q[36]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[37] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[37]),
        .Q(Q[37]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[38] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[38]),
        .Q(Q[38]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[39] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[39]),
        .Q(Q[39]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[3] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[3]),
        .Q(Q[3]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[40] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[40]),
        .Q(Q[40]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[41] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[41]),
        .Q(Q[41]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[42] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[42]),
        .Q(Q[42]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[43] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[43]),
        .Q(Q[43]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[44] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[44]),
        .Q(Q[44]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[45] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[45]),
        .Q(Q[45]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[46] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[46]),
        .Q(Q[46]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[47] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[47]),
        .Q(Q[47]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[48] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[48]),
        .Q(Q[48]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[49] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[49]),
        .Q(Q[49]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[4] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[4]),
        .Q(Q[4]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[50] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[50]),
        .Q(Q[50]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[51] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[51]),
        .Q(Q[51]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[52] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[52]),
        .Q(Q[52]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[53] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[53]),
        .Q(Q[53]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[54] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[54]),
        .Q(Q[54]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[55] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[55]),
        .Q(Q[55]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[56] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[56]),
        .Q(Q[56]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[57] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[57]),
        .Q(Q[57]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[58] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[58]),
        .Q(Q[58]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[59] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[59]),
        .Q(Q[59]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[5] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[5]),
        .Q(Q[5]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[60] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[60]),
        .Q(Q[60]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[61] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[61]),
        .Q(Q[61]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[62] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[62]),
        .Q(Q[62]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[63] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[63]),
        .Q(Q[63]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[6] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[6]),
        .Q(Q[6]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[7] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[7]),
        .Q(Q[7]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[8] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[8]),
        .Q(Q[8]),
        .S(RST_ACTIVE));
  FDSE \min_ctr_reg[9] 
       (.C(mon_clk),
        .CE(min_ctr0),
        .D(tx_length[9]),
        .Q(Q[9]),
        .S(RST_ACTIVE));
  (* ADDER_THRESHOLD = "35" *) 
  CARRY8 tx_length_carry
       (.CI(1'b1),
        .CI_TOP(1'b0),
        .CO({tx_length_carry_n_0,tx_length_carry_n_1,tx_length_carry_n_2,tx_length_carry_n_3,tx_length_carry_n_4,tx_length_carry_n_5,tx_length_carry_n_6,tx_length_carry_n_7}),
        .DI(counter_reg[7:0]),
        .O(tx_length[7:0]),
        .S({fifo_minmax_n_57,fifo_minmax_n_58,fifo_minmax_n_59,fifo_minmax_n_60,fifo_minmax_n_61,fifo_minmax_n_62,fifo_minmax_n_63,fifo_minmax_n_64}));
  (* ADDER_THRESHOLD = "35" *) 
  CARRY8 tx_length_carry__0
       (.CI(tx_length_carry_n_0),
        .CI_TOP(1'b0),
        .CO({tx_length_carry__0_n_0,tx_length_carry__0_n_1,tx_length_carry__0_n_2,tx_length_carry__0_n_3,tx_length_carry__0_n_4,tx_length_carry__0_n_5,tx_length_carry__0_n_6,tx_length_carry__0_n_7}),
        .DI(counter_reg[15:8]),
        .O(tx_length[15:8]),
        .S({fifo_minmax_n_49,fifo_minmax_n_50,fifo_minmax_n_51,fifo_minmax_n_52,fifo_minmax_n_53,fifo_minmax_n_54,fifo_minmax_n_55,fifo_minmax_n_56}));
  (* ADDER_THRESHOLD = "35" *) 
  CARRY8 tx_length_carry__1
       (.CI(tx_length_carry__0_n_0),
        .CI_TOP(1'b0),
        .CO({tx_length_carry__1_n_0,tx_length_carry__1_n_1,tx_length_carry__1_n_2,tx_length_carry__1_n_3,tx_length_carry__1_n_4,tx_length_carry__1_n_5,tx_length_carry__1_n_6,tx_length_carry__1_n_7}),
        .DI(counter_reg[23:16]),
        .O(tx_length[23:16]),
        .S({fifo_minmax_n_41,fifo_minmax_n_42,fifo_minmax_n_43,fifo_minmax_n_44,fifo_minmax_n_45,fifo_minmax_n_46,fifo_minmax_n_47,fifo_minmax_n_48}));
  (* ADDER_THRESHOLD = "35" *) 
  CARRY8 tx_length_carry__2
       (.CI(tx_length_carry__1_n_0),
        .CI_TOP(1'b0),
        .CO({tx_length_carry__2_n_0,tx_length_carry__2_n_1,tx_length_carry__2_n_2,tx_length_carry__2_n_3,tx_length_carry__2_n_4,tx_length_carry__2_n_5,tx_length_carry__2_n_6,tx_length_carry__2_n_7}),
        .DI(counter_reg[31:24]),
        .O(tx_length[31:24]),
        .S({fifo_minmax_n_33,fifo_minmax_n_34,fifo_minmax_n_35,fifo_minmax_n_36,fifo_minmax_n_37,fifo_minmax_n_38,fifo_minmax_n_39,fifo_minmax_n_40}));
  (* ADDER_THRESHOLD = "35" *) 
  CARRY8 tx_length_carry__3
       (.CI(tx_length_carry__2_n_0),
        .CI_TOP(1'b0),
        .CO({tx_length_carry__3_n_0,tx_length_carry__3_n_1,tx_length_carry__3_n_2,tx_length_carry__3_n_3,tx_length_carry__3_n_4,tx_length_carry__3_n_5,tx_length_carry__3_n_6,tx_length_carry__3_n_7}),
        .DI(counter_reg[39:32]),
        .O(tx_length[39:32]),
        .S({fifo_minmax_n_25,fifo_minmax_n_26,fifo_minmax_n_27,fifo_minmax_n_28,fifo_minmax_n_29,fifo_minmax_n_30,fifo_minmax_n_31,fifo_minmax_n_32}));
  (* ADDER_THRESHOLD = "35" *) 
  CARRY8 tx_length_carry__4
       (.CI(tx_length_carry__3_n_0),
        .CI_TOP(1'b0),
        .CO({tx_length_carry__4_n_0,tx_length_carry__4_n_1,tx_length_carry__4_n_2,tx_length_carry__4_n_3,tx_length_carry__4_n_4,tx_length_carry__4_n_5,tx_length_carry__4_n_6,tx_length_carry__4_n_7}),
        .DI(counter_reg[47:40]),
        .O(tx_length[47:40]),
        .S({fifo_minmax_n_17,fifo_minmax_n_18,fifo_minmax_n_19,fifo_minmax_n_20,fifo_minmax_n_21,fifo_minmax_n_22,fifo_minmax_n_23,fifo_minmax_n_24}));
  (* ADDER_THRESHOLD = "35" *) 
  CARRY8 tx_length_carry__5
       (.CI(tx_length_carry__4_n_0),
        .CI_TOP(1'b0),
        .CO({tx_length_carry__5_n_0,tx_length_carry__5_n_1,tx_length_carry__5_n_2,tx_length_carry__5_n_3,tx_length_carry__5_n_4,tx_length_carry__5_n_5,tx_length_carry__5_n_6,tx_length_carry__5_n_7}),
        .DI(counter_reg[55:48]),
        .O(tx_length[55:48]),
        .S({fifo_minmax_n_9,fifo_minmax_n_10,fifo_minmax_n_11,fifo_minmax_n_12,fifo_minmax_n_13,fifo_minmax_n_14,fifo_minmax_n_15,fifo_minmax_n_16}));
  (* ADDER_THRESHOLD = "35" *) 
  CARRY8 tx_length_carry__6
       (.CI(tx_length_carry__5_n_0),
        .CI_TOP(1'b0),
        .CO({NLW_tx_length_carry__6_CO_UNCONNECTED[7],tx_length_carry__6_n_1,tx_length_carry__6_n_2,tx_length_carry__6_n_3,tx_length_carry__6_n_4,tx_length_carry__6_n_5,tx_length_carry__6_n_6,tx_length_carry__6_n_7}),
        .DI({1'b0,counter_reg[62:56]}),
        .O(tx_length[63:56]),
        .S({fifo_minmax_n_1,fifo_minmax_n_2,fifo_minmax_n_3,fifo_minmax_n_4,fifo_minmax_n_5,fifo_minmax_n_6,fifo_minmax_n_7,fifo_minmax_n_8}));
endmodule

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_monitor_axilite
   (start_pulse,
    ap_done_reg,
    ap_continue_reg,
    src_in,
    ip_exec_count0,
    rd_en,
    ap_start_reg_reg_0,
    O,
    \ip_cur_tranx_reg[0] ,
    ap_start_reg_reg_1,
    ap_start_reg_reg_2,
    ap_start_reg_reg_3,
    ap_start_reg_reg_4,
    ap_start_reg_reg_5,
    ap_start_reg_reg_6,
    mon_clk,
    SS,
    Metrics_Cnt_En,
    ip_cur_tranx_reg,
    dataflow_en,
    Q,
    empty,
    s_axi_awvalid_mon,
    s_axi_awready_mon,
    s_axi_arvalid_mon,
    s_axi_arready_mon,
    s_axi_awaddr_mon,
    s_axi_araddr_mon,
    s_axi_wvalid_mon,
    s_axi_wready_mon,
    s_axi_wstrb_mon,
    s_axi_wdata_mon,
    s_axi_rvalid_mon,
    s_axi_rdata_mon,
    s_axi_rready_mon);
  output start_pulse;
  output ap_done_reg;
  output ap_continue_reg;
  output [0:0]src_in;
  output ip_exec_count0;
  output rd_en;
  output [0:0]ap_start_reg_reg_0;
  output [7:0]O;
  output [7:0]\ip_cur_tranx_reg[0] ;
  output [7:0]ap_start_reg_reg_1;
  output [7:0]ap_start_reg_reg_2;
  output [7:0]ap_start_reg_reg_3;
  output [7:0]ap_start_reg_reg_4;
  output [7:0]ap_start_reg_reg_5;
  output [7:0]ap_start_reg_reg_6;
  input mon_clk;
  input [0:0]SS;
  input Metrics_Cnt_En;
  input [63:0]ip_cur_tranx_reg;
  input dataflow_en;
  input [0:0]Q;
  input empty;
  input s_axi_awvalid_mon;
  input s_axi_awready_mon;
  input s_axi_arvalid_mon;
  input s_axi_arready_mon;
  input [7:0]s_axi_awaddr_mon;
  input [7:0]s_axi_araddr_mon;
  input s_axi_wvalid_mon;
  input s_axi_wready_mon;
  input [0:0]s_axi_wstrb_mon;
  input [1:0]s_axi_wdata_mon;
  input s_axi_rvalid_mon;
  input [0:0]s_axi_rdata_mon;
  input s_axi_rready_mon;

  wire Metrics_Cnt_En;
  wire [7:0]O;
  wire [0:0]Q;
  wire [0:0]SS;
  wire ap_continue_reg;
  wire ap_continue_write;
  wire ap_done_read;
  wire ap_done_reg;
  wire ap_done_reg_i_2_n_0;
  wire ap_done_reg_i_3_n_0;
  wire ap_start_reg_i_3_n_0;
  wire [0:0]ap_start_reg_reg_0;
  wire [7:0]ap_start_reg_reg_1;
  wire [7:0]ap_start_reg_reg_2;
  wire [7:0]ap_start_reg_reg_3;
  wire [7:0]ap_start_reg_reg_4;
  wire [7:0]ap_start_reg_reg_5;
  wire [7:0]ap_start_reg_reg_6;
  wire ap_start_write;
  wire ap_start_write0__6;
  wire dataflow_en;
  wire empty;
  wire \ip_cur_tranx[0]_i_10_n_0 ;
  wire \ip_cur_tranx[0]_i_11_n_0 ;
  wire \ip_cur_tranx[0]_i_12_n_0 ;
  wire \ip_cur_tranx[0]_i_13_n_0 ;
  wire \ip_cur_tranx[0]_i_14_n_0 ;
  wire \ip_cur_tranx[0]_i_15_n_0 ;
  wire \ip_cur_tranx[0]_i_16_n_0 ;
  wire \ip_cur_tranx[0]_i_17_n_0 ;
  wire \ip_cur_tranx[0]_i_18_n_0 ;
  wire \ip_cur_tranx[0]_i_19_n_0 ;
  wire \ip_cur_tranx[0]_i_20_n_0 ;
  wire \ip_cur_tranx[0]_i_21_n_0 ;
  wire \ip_cur_tranx[0]_i_6_n_0 ;
  wire \ip_cur_tranx[0]_i_7_n_0 ;
  wire \ip_cur_tranx[0]_i_8_n_0 ;
  wire \ip_cur_tranx[0]_i_9_n_0 ;
  wire \ip_cur_tranx[16]_i_10_n_0 ;
  wire \ip_cur_tranx[16]_i_11_n_0 ;
  wire \ip_cur_tranx[16]_i_12_n_0 ;
  wire \ip_cur_tranx[16]_i_13_n_0 ;
  wire \ip_cur_tranx[16]_i_14_n_0 ;
  wire \ip_cur_tranx[16]_i_15_n_0 ;
  wire \ip_cur_tranx[16]_i_16_n_0 ;
  wire \ip_cur_tranx[16]_i_17_n_0 ;
  wire \ip_cur_tranx[16]_i_2_n_0 ;
  wire \ip_cur_tranx[16]_i_3_n_0 ;
  wire \ip_cur_tranx[16]_i_4_n_0 ;
  wire \ip_cur_tranx[16]_i_5_n_0 ;
  wire \ip_cur_tranx[16]_i_6_n_0 ;
  wire \ip_cur_tranx[16]_i_7_n_0 ;
  wire \ip_cur_tranx[16]_i_8_n_0 ;
  wire \ip_cur_tranx[16]_i_9_n_0 ;
  wire \ip_cur_tranx[24]_i_10_n_0 ;
  wire \ip_cur_tranx[24]_i_11_n_0 ;
  wire \ip_cur_tranx[24]_i_12_n_0 ;
  wire \ip_cur_tranx[24]_i_13_n_0 ;
  wire \ip_cur_tranx[24]_i_14_n_0 ;
  wire \ip_cur_tranx[24]_i_15_n_0 ;
  wire \ip_cur_tranx[24]_i_16_n_0 ;
  wire \ip_cur_tranx[24]_i_17_n_0 ;
  wire \ip_cur_tranx[24]_i_2_n_0 ;
  wire \ip_cur_tranx[24]_i_3_n_0 ;
  wire \ip_cur_tranx[24]_i_4_n_0 ;
  wire \ip_cur_tranx[24]_i_5_n_0 ;
  wire \ip_cur_tranx[24]_i_6_n_0 ;
  wire \ip_cur_tranx[24]_i_7_n_0 ;
  wire \ip_cur_tranx[24]_i_8_n_0 ;
  wire \ip_cur_tranx[24]_i_9_n_0 ;
  wire \ip_cur_tranx[32]_i_10_n_0 ;
  wire \ip_cur_tranx[32]_i_11_n_0 ;
  wire \ip_cur_tranx[32]_i_12_n_0 ;
  wire \ip_cur_tranx[32]_i_13_n_0 ;
  wire \ip_cur_tranx[32]_i_14_n_0 ;
  wire \ip_cur_tranx[32]_i_15_n_0 ;
  wire \ip_cur_tranx[32]_i_16_n_0 ;
  wire \ip_cur_tranx[32]_i_17_n_0 ;
  wire \ip_cur_tranx[32]_i_2_n_0 ;
  wire \ip_cur_tranx[32]_i_3_n_0 ;
  wire \ip_cur_tranx[32]_i_4_n_0 ;
  wire \ip_cur_tranx[32]_i_5_n_0 ;
  wire \ip_cur_tranx[32]_i_6_n_0 ;
  wire \ip_cur_tranx[32]_i_7_n_0 ;
  wire \ip_cur_tranx[32]_i_8_n_0 ;
  wire \ip_cur_tranx[32]_i_9_n_0 ;
  wire \ip_cur_tranx[40]_i_10_n_0 ;
  wire \ip_cur_tranx[40]_i_11_n_0 ;
  wire \ip_cur_tranx[40]_i_12_n_0 ;
  wire \ip_cur_tranx[40]_i_13_n_0 ;
  wire \ip_cur_tranx[40]_i_14_n_0 ;
  wire \ip_cur_tranx[40]_i_15_n_0 ;
  wire \ip_cur_tranx[40]_i_16_n_0 ;
  wire \ip_cur_tranx[40]_i_17_n_0 ;
  wire \ip_cur_tranx[40]_i_2_n_0 ;
  wire \ip_cur_tranx[40]_i_3_n_0 ;
  wire \ip_cur_tranx[40]_i_4_n_0 ;
  wire \ip_cur_tranx[40]_i_5_n_0 ;
  wire \ip_cur_tranx[40]_i_6_n_0 ;
  wire \ip_cur_tranx[40]_i_7_n_0 ;
  wire \ip_cur_tranx[40]_i_8_n_0 ;
  wire \ip_cur_tranx[40]_i_9_n_0 ;
  wire \ip_cur_tranx[48]_i_10_n_0 ;
  wire \ip_cur_tranx[48]_i_11_n_0 ;
  wire \ip_cur_tranx[48]_i_12_n_0 ;
  wire \ip_cur_tranx[48]_i_13_n_0 ;
  wire \ip_cur_tranx[48]_i_14_n_0 ;
  wire \ip_cur_tranx[48]_i_15_n_0 ;
  wire \ip_cur_tranx[48]_i_16_n_0 ;
  wire \ip_cur_tranx[48]_i_17_n_0 ;
  wire \ip_cur_tranx[48]_i_2_n_0 ;
  wire \ip_cur_tranx[48]_i_3_n_0 ;
  wire \ip_cur_tranx[48]_i_4_n_0 ;
  wire \ip_cur_tranx[48]_i_5_n_0 ;
  wire \ip_cur_tranx[48]_i_6_n_0 ;
  wire \ip_cur_tranx[48]_i_7_n_0 ;
  wire \ip_cur_tranx[48]_i_8_n_0 ;
  wire \ip_cur_tranx[48]_i_9_n_0 ;
  wire \ip_cur_tranx[56]_i_10_n_0 ;
  wire \ip_cur_tranx[56]_i_11_n_0 ;
  wire \ip_cur_tranx[56]_i_12_n_0 ;
  wire \ip_cur_tranx[56]_i_13_n_0 ;
  wire \ip_cur_tranx[56]_i_14_n_0 ;
  wire \ip_cur_tranx[56]_i_15_n_0 ;
  wire \ip_cur_tranx[56]_i_16_n_0 ;
  wire \ip_cur_tranx[56]_i_2_n_0 ;
  wire \ip_cur_tranx[56]_i_3_n_0 ;
  wire \ip_cur_tranx[56]_i_4_n_0 ;
  wire \ip_cur_tranx[56]_i_5_n_0 ;
  wire \ip_cur_tranx[56]_i_6_n_0 ;
  wire \ip_cur_tranx[56]_i_7_n_0 ;
  wire \ip_cur_tranx[56]_i_8_n_0 ;
  wire \ip_cur_tranx[56]_i_9_n_0 ;
  wire \ip_cur_tranx[8]_i_10_n_0 ;
  wire \ip_cur_tranx[8]_i_11_n_0 ;
  wire \ip_cur_tranx[8]_i_12_n_0 ;
  wire \ip_cur_tranx[8]_i_13_n_0 ;
  wire \ip_cur_tranx[8]_i_14_n_0 ;
  wire \ip_cur_tranx[8]_i_15_n_0 ;
  wire \ip_cur_tranx[8]_i_16_n_0 ;
  wire \ip_cur_tranx[8]_i_17_n_0 ;
  wire \ip_cur_tranx[8]_i_2_n_0 ;
  wire \ip_cur_tranx[8]_i_3_n_0 ;
  wire \ip_cur_tranx[8]_i_4_n_0 ;
  wire \ip_cur_tranx[8]_i_5_n_0 ;
  wire \ip_cur_tranx[8]_i_6_n_0 ;
  wire \ip_cur_tranx[8]_i_7_n_0 ;
  wire \ip_cur_tranx[8]_i_8_n_0 ;
  wire \ip_cur_tranx[8]_i_9_n_0 ;
  wire [63:0]ip_cur_tranx_reg;
  wire [7:0]\ip_cur_tranx_reg[0] ;
  wire \ip_cur_tranx_reg[0]_i_2_n_0 ;
  wire \ip_cur_tranx_reg[0]_i_2_n_1 ;
  wire \ip_cur_tranx_reg[0]_i_2_n_2 ;
  wire \ip_cur_tranx_reg[0]_i_2_n_3 ;
  wire \ip_cur_tranx_reg[0]_i_2_n_4 ;
  wire \ip_cur_tranx_reg[0]_i_2_n_5 ;
  wire \ip_cur_tranx_reg[0]_i_2_n_6 ;
  wire \ip_cur_tranx_reg[0]_i_2_n_7 ;
  wire \ip_cur_tranx_reg[16]_i_1_n_0 ;
  wire \ip_cur_tranx_reg[16]_i_1_n_1 ;
  wire \ip_cur_tranx_reg[16]_i_1_n_2 ;
  wire \ip_cur_tranx_reg[16]_i_1_n_3 ;
  wire \ip_cur_tranx_reg[16]_i_1_n_4 ;
  wire \ip_cur_tranx_reg[16]_i_1_n_5 ;
  wire \ip_cur_tranx_reg[16]_i_1_n_6 ;
  wire \ip_cur_tranx_reg[16]_i_1_n_7 ;
  wire \ip_cur_tranx_reg[24]_i_1_n_0 ;
  wire \ip_cur_tranx_reg[24]_i_1_n_1 ;
  wire \ip_cur_tranx_reg[24]_i_1_n_2 ;
  wire \ip_cur_tranx_reg[24]_i_1_n_3 ;
  wire \ip_cur_tranx_reg[24]_i_1_n_4 ;
  wire \ip_cur_tranx_reg[24]_i_1_n_5 ;
  wire \ip_cur_tranx_reg[24]_i_1_n_6 ;
  wire \ip_cur_tranx_reg[24]_i_1_n_7 ;
  wire \ip_cur_tranx_reg[32]_i_1_n_0 ;
  wire \ip_cur_tranx_reg[32]_i_1_n_1 ;
  wire \ip_cur_tranx_reg[32]_i_1_n_2 ;
  wire \ip_cur_tranx_reg[32]_i_1_n_3 ;
  wire \ip_cur_tranx_reg[32]_i_1_n_4 ;
  wire \ip_cur_tranx_reg[32]_i_1_n_5 ;
  wire \ip_cur_tranx_reg[32]_i_1_n_6 ;
  wire \ip_cur_tranx_reg[32]_i_1_n_7 ;
  wire \ip_cur_tranx_reg[40]_i_1_n_0 ;
  wire \ip_cur_tranx_reg[40]_i_1_n_1 ;
  wire \ip_cur_tranx_reg[40]_i_1_n_2 ;
  wire \ip_cur_tranx_reg[40]_i_1_n_3 ;
  wire \ip_cur_tranx_reg[40]_i_1_n_4 ;
  wire \ip_cur_tranx_reg[40]_i_1_n_5 ;
  wire \ip_cur_tranx_reg[40]_i_1_n_6 ;
  wire \ip_cur_tranx_reg[40]_i_1_n_7 ;
  wire \ip_cur_tranx_reg[48]_i_1_n_0 ;
  wire \ip_cur_tranx_reg[48]_i_1_n_1 ;
  wire \ip_cur_tranx_reg[48]_i_1_n_2 ;
  wire \ip_cur_tranx_reg[48]_i_1_n_3 ;
  wire \ip_cur_tranx_reg[48]_i_1_n_4 ;
  wire \ip_cur_tranx_reg[48]_i_1_n_5 ;
  wire \ip_cur_tranx_reg[48]_i_1_n_6 ;
  wire \ip_cur_tranx_reg[48]_i_1_n_7 ;
  wire \ip_cur_tranx_reg[56]_i_1_n_1 ;
  wire \ip_cur_tranx_reg[56]_i_1_n_2 ;
  wire \ip_cur_tranx_reg[56]_i_1_n_3 ;
  wire \ip_cur_tranx_reg[56]_i_1_n_4 ;
  wire \ip_cur_tranx_reg[56]_i_1_n_5 ;
  wire \ip_cur_tranx_reg[56]_i_1_n_6 ;
  wire \ip_cur_tranx_reg[56]_i_1_n_7 ;
  wire \ip_cur_tranx_reg[8]_i_1_n_0 ;
  wire \ip_cur_tranx_reg[8]_i_1_n_1 ;
  wire \ip_cur_tranx_reg[8]_i_1_n_2 ;
  wire \ip_cur_tranx_reg[8]_i_1_n_3 ;
  wire \ip_cur_tranx_reg[8]_i_1_n_4 ;
  wire \ip_cur_tranx_reg[8]_i_1_n_5 ;
  wire \ip_cur_tranx_reg[8]_i_1_n_6 ;
  wire \ip_cur_tranx_reg[8]_i_1_n_7 ;
  wire ip_exec_count0;
  wire [7:0]last_read_addr;
  wire [7:0]last_write_addr;
  wire mon_clk;
  wire rd_en;
  wire read_addr_valid;
  wire [7:0]s_axi_araddr_mon;
  wire s_axi_arready_mon;
  wire s_axi_arvalid_mon;
  wire [7:0]s_axi_awaddr_mon;
  wire s_axi_awready_mon;
  wire s_axi_awvalid_mon;
  wire [0:0]s_axi_rdata_mon;
  wire s_axi_rready_mon;
  wire s_axi_rvalid_mon;
  wire [1:0]s_axi_wdata_mon;
  wire s_axi_wready_mon;
  wire [0:0]s_axi_wstrb_mon;
  wire s_axi_wvalid_mon;
  wire [0:0]src_in;
  wire start_pulse;
  wire started;
  wire started_i_1_n_0;
  wire write_addr_valid;
  wire [7:7]\NLW_ip_cur_tranx_reg[56]_i_1_CO_UNCONNECTED ;

  LUT6 #(
    .INIT(64'h8000000000000000)) 
    ap_continue_reg_i_1
       (.I0(started),
        .I1(s_axi_wvalid_mon),
        .I2(s_axi_wready_mon),
        .I3(s_axi_wstrb_mon),
        .I4(s_axi_wdata_mon[1]),
        .I5(ap_start_write0__6),
        .O(ap_continue_write));
  FDRE ap_continue_reg_reg
       (.C(mon_clk),
        .CE(1'b1),
        .D(ap_continue_write),
        .Q(ap_continue_reg),
        .R(1'b0));
  LUT6 #(
    .INIT(64'h0000008000000000)) 
    ap_done_reg_i_1
       (.I0(s_axi_rvalid_mon),
        .I1(s_axi_rdata_mon),
        .I2(s_axi_rready_mon),
        .I3(ap_done_reg_i_2_n_0),
        .I4(ap_done_reg_i_3_n_0),
        .I5(started),
        .O(ap_done_read));
  LUT4 #(
    .INIT(16'hFFFE)) 
    ap_done_reg_i_2
       (.I0(last_read_addr[2]),
        .I1(last_read_addr[3]),
        .I2(last_read_addr[0]),
        .I3(last_read_addr[1]),
        .O(ap_done_reg_i_2_n_0));
  LUT4 #(
    .INIT(16'hFFFE)) 
    ap_done_reg_i_3
       (.I0(last_read_addr[6]),
        .I1(last_read_addr[7]),
        .I2(last_read_addr[4]),
        .I3(last_read_addr[5]),
        .O(ap_done_reg_i_3_n_0));
  FDRE ap_done_reg_reg
       (.C(mon_clk),
        .CE(1'b1),
        .D(ap_done_read),
        .Q(ap_done_reg),
        .R(1'b0));
  LUT5 #(
    .INIT(32'h80000000)) 
    ap_start_reg_i_1
       (.I0(s_axi_wstrb_mon),
        .I1(s_axi_wready_mon),
        .I2(s_axi_wvalid_mon),
        .I3(s_axi_wdata_mon[0]),
        .I4(ap_start_write0__6),
        .O(ap_start_write));
  LUT5 #(
    .INIT(32'h00000001)) 
    ap_start_reg_i_2
       (.I0(last_write_addr[4]),
        .I1(last_write_addr[5]),
        .I2(last_write_addr[6]),
        .I3(last_write_addr[7]),
        .I4(ap_start_reg_i_3_n_0),
        .O(ap_start_write0__6));
  LUT4 #(
    .INIT(16'hFFFE)) 
    ap_start_reg_i_3
       (.I0(last_write_addr[1]),
        .I1(last_write_addr[0]),
        .I2(last_write_addr[3]),
        .I3(last_write_addr[2]),
        .O(ap_start_reg_i_3_n_0));
  FDRE ap_start_reg_reg
       (.C(mon_clk),
        .CE(1'b1),
        .D(ap_start_write),
        .Q(start_pulse),
        .R(1'b0));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[0]_i_10 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[0]_i_10_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[0]_i_11 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[0]_i_11_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[0]_i_12 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[0]_i_12_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[0]_i_13 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[0]_i_13_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[0]_i_14 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[7]),
        .O(\ip_cur_tranx[0]_i_14_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[0]_i_15 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[6]),
        .O(\ip_cur_tranx[0]_i_15_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[0]_i_16 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[5]),
        .O(\ip_cur_tranx[0]_i_16_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[0]_i_17 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[4]),
        .O(\ip_cur_tranx[0]_i_17_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[0]_i_18 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[3]),
        .O(\ip_cur_tranx[0]_i_18_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[0]_i_19 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[2]),
        .O(\ip_cur_tranx[0]_i_19_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[0]_i_20 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[1]),
        .O(\ip_cur_tranx[0]_i_20_n_0 ));
  LUT3 #(
    .INIT(8'h78)) 
    \ip_cur_tranx[0]_i_21 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[0]),
        .O(\ip_cur_tranx[0]_i_21_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[0]_i_6 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[0]_i_6_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[0]_i_7 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[0]_i_7_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[0]_i_8 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[0]_i_8_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[0]_i_9 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[0]_i_9_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[16]_i_10 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[23]),
        .O(\ip_cur_tranx[16]_i_10_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[16]_i_11 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[22]),
        .O(\ip_cur_tranx[16]_i_11_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[16]_i_12 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[21]),
        .O(\ip_cur_tranx[16]_i_12_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[16]_i_13 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[20]),
        .O(\ip_cur_tranx[16]_i_13_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[16]_i_14 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[19]),
        .O(\ip_cur_tranx[16]_i_14_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[16]_i_15 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[18]),
        .O(\ip_cur_tranx[16]_i_15_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[16]_i_16 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[17]),
        .O(\ip_cur_tranx[16]_i_16_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[16]_i_17 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[16]),
        .O(\ip_cur_tranx[16]_i_17_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[16]_i_2 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[16]_i_2_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[16]_i_3 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[16]_i_3_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[16]_i_4 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[16]_i_4_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[16]_i_5 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[16]_i_5_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[16]_i_6 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[16]_i_6_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[16]_i_7 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[16]_i_7_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[16]_i_8 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[16]_i_8_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[16]_i_9 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[16]_i_9_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[24]_i_10 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[31]),
        .O(\ip_cur_tranx[24]_i_10_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[24]_i_11 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[30]),
        .O(\ip_cur_tranx[24]_i_11_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[24]_i_12 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[29]),
        .O(\ip_cur_tranx[24]_i_12_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[24]_i_13 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[28]),
        .O(\ip_cur_tranx[24]_i_13_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[24]_i_14 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[27]),
        .O(\ip_cur_tranx[24]_i_14_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[24]_i_15 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[26]),
        .O(\ip_cur_tranx[24]_i_15_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[24]_i_16 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[25]),
        .O(\ip_cur_tranx[24]_i_16_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[24]_i_17 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[24]),
        .O(\ip_cur_tranx[24]_i_17_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[24]_i_2 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[24]_i_2_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[24]_i_3 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[24]_i_3_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[24]_i_4 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[24]_i_4_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[24]_i_5 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[24]_i_5_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[24]_i_6 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[24]_i_6_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[24]_i_7 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[24]_i_7_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[24]_i_8 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[24]_i_8_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[24]_i_9 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[24]_i_9_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[32]_i_10 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[39]),
        .O(\ip_cur_tranx[32]_i_10_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[32]_i_11 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[38]),
        .O(\ip_cur_tranx[32]_i_11_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[32]_i_12 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[37]),
        .O(\ip_cur_tranx[32]_i_12_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[32]_i_13 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[36]),
        .O(\ip_cur_tranx[32]_i_13_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[32]_i_14 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[35]),
        .O(\ip_cur_tranx[32]_i_14_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[32]_i_15 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[34]),
        .O(\ip_cur_tranx[32]_i_15_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[32]_i_16 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[33]),
        .O(\ip_cur_tranx[32]_i_16_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[32]_i_17 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[32]),
        .O(\ip_cur_tranx[32]_i_17_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[32]_i_2 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[32]_i_2_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[32]_i_3 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[32]_i_3_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[32]_i_4 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[32]_i_4_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[32]_i_5 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[32]_i_5_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[32]_i_6 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[32]_i_6_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[32]_i_7 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[32]_i_7_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[32]_i_8 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[32]_i_8_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[32]_i_9 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[32]_i_9_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[40]_i_10 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[47]),
        .O(\ip_cur_tranx[40]_i_10_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[40]_i_11 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[46]),
        .O(\ip_cur_tranx[40]_i_11_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[40]_i_12 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[45]),
        .O(\ip_cur_tranx[40]_i_12_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[40]_i_13 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[44]),
        .O(\ip_cur_tranx[40]_i_13_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[40]_i_14 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[43]),
        .O(\ip_cur_tranx[40]_i_14_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[40]_i_15 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[42]),
        .O(\ip_cur_tranx[40]_i_15_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[40]_i_16 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[41]),
        .O(\ip_cur_tranx[40]_i_16_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[40]_i_17 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[40]),
        .O(\ip_cur_tranx[40]_i_17_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[40]_i_2 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[40]_i_2_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[40]_i_3 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[40]_i_3_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[40]_i_4 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[40]_i_4_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[40]_i_5 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[40]_i_5_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[40]_i_6 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[40]_i_6_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[40]_i_7 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[40]_i_7_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[40]_i_8 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[40]_i_8_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[40]_i_9 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[40]_i_9_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[48]_i_10 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[55]),
        .O(\ip_cur_tranx[48]_i_10_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[48]_i_11 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[54]),
        .O(\ip_cur_tranx[48]_i_11_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[48]_i_12 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[53]),
        .O(\ip_cur_tranx[48]_i_12_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[48]_i_13 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[52]),
        .O(\ip_cur_tranx[48]_i_13_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[48]_i_14 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[51]),
        .O(\ip_cur_tranx[48]_i_14_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[48]_i_15 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[50]),
        .O(\ip_cur_tranx[48]_i_15_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[48]_i_16 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[49]),
        .O(\ip_cur_tranx[48]_i_16_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[48]_i_17 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[48]),
        .O(\ip_cur_tranx[48]_i_17_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[48]_i_2 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[48]_i_2_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[48]_i_3 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[48]_i_3_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[48]_i_4 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[48]_i_4_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[48]_i_5 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[48]_i_5_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[48]_i_6 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[48]_i_6_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[48]_i_7 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[48]_i_7_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[48]_i_8 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[48]_i_8_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[48]_i_9 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[48]_i_9_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[56]_i_10 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[62]),
        .O(\ip_cur_tranx[56]_i_10_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[56]_i_11 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[61]),
        .O(\ip_cur_tranx[56]_i_11_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[56]_i_12 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[60]),
        .O(\ip_cur_tranx[56]_i_12_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[56]_i_13 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[59]),
        .O(\ip_cur_tranx[56]_i_13_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[56]_i_14 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[58]),
        .O(\ip_cur_tranx[56]_i_14_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[56]_i_15 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[57]),
        .O(\ip_cur_tranx[56]_i_15_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[56]_i_16 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[56]),
        .O(\ip_cur_tranx[56]_i_16_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[56]_i_2 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[56]_i_2_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[56]_i_3 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[56]_i_3_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[56]_i_4 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[56]_i_4_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[56]_i_5 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[56]_i_5_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[56]_i_6 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[56]_i_6_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[56]_i_7 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[56]_i_7_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[56]_i_8 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[56]_i_8_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[56]_i_9 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[63]),
        .O(\ip_cur_tranx[56]_i_9_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[8]_i_10 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[15]),
        .O(\ip_cur_tranx[8]_i_10_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[8]_i_11 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[14]),
        .O(\ip_cur_tranx[8]_i_11_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[8]_i_12 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[13]),
        .O(\ip_cur_tranx[8]_i_12_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[8]_i_13 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[12]),
        .O(\ip_cur_tranx[8]_i_13_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[8]_i_14 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[11]),
        .O(\ip_cur_tranx[8]_i_14_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[8]_i_15 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[10]),
        .O(\ip_cur_tranx[8]_i_15_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[8]_i_16 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[9]),
        .O(\ip_cur_tranx[8]_i_16_n_0 ));
  LUT3 #(
    .INIT(8'h87)) 
    \ip_cur_tranx[8]_i_17 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .I2(ip_cur_tranx_reg[8]),
        .O(\ip_cur_tranx[8]_i_17_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[8]_i_2 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[8]_i_2_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[8]_i_3 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[8]_i_3_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[8]_i_4 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[8]_i_4_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[8]_i_5 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[8]_i_5_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[8]_i_6 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[8]_i_6_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[8]_i_7 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[8]_i_7_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[8]_i_8 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[8]_i_8_n_0 ));
  LUT2 #(
    .INIT(4'h7)) 
    \ip_cur_tranx[8]_i_9 
       (.I0(start_pulse),
        .I1(Metrics_Cnt_En),
        .O(\ip_cur_tranx[8]_i_9_n_0 ));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_cur_tranx_reg[0]_i_2 
       (.CI(\ip_cur_tranx[0]_i_6_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ip_cur_tranx_reg[0]_i_2_n_0 ,\ip_cur_tranx_reg[0]_i_2_n_1 ,\ip_cur_tranx_reg[0]_i_2_n_2 ,\ip_cur_tranx_reg[0]_i_2_n_3 ,\ip_cur_tranx_reg[0]_i_2_n_4 ,\ip_cur_tranx_reg[0]_i_2_n_5 ,\ip_cur_tranx_reg[0]_i_2_n_6 ,\ip_cur_tranx_reg[0]_i_2_n_7 }),
        .DI({\ip_cur_tranx[0]_i_7_n_0 ,\ip_cur_tranx[0]_i_8_n_0 ,\ip_cur_tranx[0]_i_9_n_0 ,\ip_cur_tranx[0]_i_10_n_0 ,\ip_cur_tranx[0]_i_11_n_0 ,\ip_cur_tranx[0]_i_12_n_0 ,\ip_cur_tranx[0]_i_13_n_0 ,ip_cur_tranx_reg[0]}),
        .O(O),
        .S({\ip_cur_tranx[0]_i_14_n_0 ,\ip_cur_tranx[0]_i_15_n_0 ,\ip_cur_tranx[0]_i_16_n_0 ,\ip_cur_tranx[0]_i_17_n_0 ,\ip_cur_tranx[0]_i_18_n_0 ,\ip_cur_tranx[0]_i_19_n_0 ,\ip_cur_tranx[0]_i_20_n_0 ,\ip_cur_tranx[0]_i_21_n_0 }));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_cur_tranx_reg[16]_i_1 
       (.CI(\ip_cur_tranx_reg[8]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ip_cur_tranx_reg[16]_i_1_n_0 ,\ip_cur_tranx_reg[16]_i_1_n_1 ,\ip_cur_tranx_reg[16]_i_1_n_2 ,\ip_cur_tranx_reg[16]_i_1_n_3 ,\ip_cur_tranx_reg[16]_i_1_n_4 ,\ip_cur_tranx_reg[16]_i_1_n_5 ,\ip_cur_tranx_reg[16]_i_1_n_6 ,\ip_cur_tranx_reg[16]_i_1_n_7 }),
        .DI({\ip_cur_tranx[16]_i_2_n_0 ,\ip_cur_tranx[16]_i_3_n_0 ,\ip_cur_tranx[16]_i_4_n_0 ,\ip_cur_tranx[16]_i_5_n_0 ,\ip_cur_tranx[16]_i_6_n_0 ,\ip_cur_tranx[16]_i_7_n_0 ,\ip_cur_tranx[16]_i_8_n_0 ,\ip_cur_tranx[16]_i_9_n_0 }),
        .O(ap_start_reg_reg_1),
        .S({\ip_cur_tranx[16]_i_10_n_0 ,\ip_cur_tranx[16]_i_11_n_0 ,\ip_cur_tranx[16]_i_12_n_0 ,\ip_cur_tranx[16]_i_13_n_0 ,\ip_cur_tranx[16]_i_14_n_0 ,\ip_cur_tranx[16]_i_15_n_0 ,\ip_cur_tranx[16]_i_16_n_0 ,\ip_cur_tranx[16]_i_17_n_0 }));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_cur_tranx_reg[24]_i_1 
       (.CI(\ip_cur_tranx_reg[16]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ip_cur_tranx_reg[24]_i_1_n_0 ,\ip_cur_tranx_reg[24]_i_1_n_1 ,\ip_cur_tranx_reg[24]_i_1_n_2 ,\ip_cur_tranx_reg[24]_i_1_n_3 ,\ip_cur_tranx_reg[24]_i_1_n_4 ,\ip_cur_tranx_reg[24]_i_1_n_5 ,\ip_cur_tranx_reg[24]_i_1_n_6 ,\ip_cur_tranx_reg[24]_i_1_n_7 }),
        .DI({\ip_cur_tranx[24]_i_2_n_0 ,\ip_cur_tranx[24]_i_3_n_0 ,\ip_cur_tranx[24]_i_4_n_0 ,\ip_cur_tranx[24]_i_5_n_0 ,\ip_cur_tranx[24]_i_6_n_0 ,\ip_cur_tranx[24]_i_7_n_0 ,\ip_cur_tranx[24]_i_8_n_0 ,\ip_cur_tranx[24]_i_9_n_0 }),
        .O(ap_start_reg_reg_2),
        .S({\ip_cur_tranx[24]_i_10_n_0 ,\ip_cur_tranx[24]_i_11_n_0 ,\ip_cur_tranx[24]_i_12_n_0 ,\ip_cur_tranx[24]_i_13_n_0 ,\ip_cur_tranx[24]_i_14_n_0 ,\ip_cur_tranx[24]_i_15_n_0 ,\ip_cur_tranx[24]_i_16_n_0 ,\ip_cur_tranx[24]_i_17_n_0 }));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_cur_tranx_reg[32]_i_1 
       (.CI(\ip_cur_tranx_reg[24]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ip_cur_tranx_reg[32]_i_1_n_0 ,\ip_cur_tranx_reg[32]_i_1_n_1 ,\ip_cur_tranx_reg[32]_i_1_n_2 ,\ip_cur_tranx_reg[32]_i_1_n_3 ,\ip_cur_tranx_reg[32]_i_1_n_4 ,\ip_cur_tranx_reg[32]_i_1_n_5 ,\ip_cur_tranx_reg[32]_i_1_n_6 ,\ip_cur_tranx_reg[32]_i_1_n_7 }),
        .DI({\ip_cur_tranx[32]_i_2_n_0 ,\ip_cur_tranx[32]_i_3_n_0 ,\ip_cur_tranx[32]_i_4_n_0 ,\ip_cur_tranx[32]_i_5_n_0 ,\ip_cur_tranx[32]_i_6_n_0 ,\ip_cur_tranx[32]_i_7_n_0 ,\ip_cur_tranx[32]_i_8_n_0 ,\ip_cur_tranx[32]_i_9_n_0 }),
        .O(ap_start_reg_reg_3),
        .S({\ip_cur_tranx[32]_i_10_n_0 ,\ip_cur_tranx[32]_i_11_n_0 ,\ip_cur_tranx[32]_i_12_n_0 ,\ip_cur_tranx[32]_i_13_n_0 ,\ip_cur_tranx[32]_i_14_n_0 ,\ip_cur_tranx[32]_i_15_n_0 ,\ip_cur_tranx[32]_i_16_n_0 ,\ip_cur_tranx[32]_i_17_n_0 }));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_cur_tranx_reg[40]_i_1 
       (.CI(\ip_cur_tranx_reg[32]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ip_cur_tranx_reg[40]_i_1_n_0 ,\ip_cur_tranx_reg[40]_i_1_n_1 ,\ip_cur_tranx_reg[40]_i_1_n_2 ,\ip_cur_tranx_reg[40]_i_1_n_3 ,\ip_cur_tranx_reg[40]_i_1_n_4 ,\ip_cur_tranx_reg[40]_i_1_n_5 ,\ip_cur_tranx_reg[40]_i_1_n_6 ,\ip_cur_tranx_reg[40]_i_1_n_7 }),
        .DI({\ip_cur_tranx[40]_i_2_n_0 ,\ip_cur_tranx[40]_i_3_n_0 ,\ip_cur_tranx[40]_i_4_n_0 ,\ip_cur_tranx[40]_i_5_n_0 ,\ip_cur_tranx[40]_i_6_n_0 ,\ip_cur_tranx[40]_i_7_n_0 ,\ip_cur_tranx[40]_i_8_n_0 ,\ip_cur_tranx[40]_i_9_n_0 }),
        .O(ap_start_reg_reg_4),
        .S({\ip_cur_tranx[40]_i_10_n_0 ,\ip_cur_tranx[40]_i_11_n_0 ,\ip_cur_tranx[40]_i_12_n_0 ,\ip_cur_tranx[40]_i_13_n_0 ,\ip_cur_tranx[40]_i_14_n_0 ,\ip_cur_tranx[40]_i_15_n_0 ,\ip_cur_tranx[40]_i_16_n_0 ,\ip_cur_tranx[40]_i_17_n_0 }));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_cur_tranx_reg[48]_i_1 
       (.CI(\ip_cur_tranx_reg[40]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ip_cur_tranx_reg[48]_i_1_n_0 ,\ip_cur_tranx_reg[48]_i_1_n_1 ,\ip_cur_tranx_reg[48]_i_1_n_2 ,\ip_cur_tranx_reg[48]_i_1_n_3 ,\ip_cur_tranx_reg[48]_i_1_n_4 ,\ip_cur_tranx_reg[48]_i_1_n_5 ,\ip_cur_tranx_reg[48]_i_1_n_6 ,\ip_cur_tranx_reg[48]_i_1_n_7 }),
        .DI({\ip_cur_tranx[48]_i_2_n_0 ,\ip_cur_tranx[48]_i_3_n_0 ,\ip_cur_tranx[48]_i_4_n_0 ,\ip_cur_tranx[48]_i_5_n_0 ,\ip_cur_tranx[48]_i_6_n_0 ,\ip_cur_tranx[48]_i_7_n_0 ,\ip_cur_tranx[48]_i_8_n_0 ,\ip_cur_tranx[48]_i_9_n_0 }),
        .O(ap_start_reg_reg_5),
        .S({\ip_cur_tranx[48]_i_10_n_0 ,\ip_cur_tranx[48]_i_11_n_0 ,\ip_cur_tranx[48]_i_12_n_0 ,\ip_cur_tranx[48]_i_13_n_0 ,\ip_cur_tranx[48]_i_14_n_0 ,\ip_cur_tranx[48]_i_15_n_0 ,\ip_cur_tranx[48]_i_16_n_0 ,\ip_cur_tranx[48]_i_17_n_0 }));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_cur_tranx_reg[56]_i_1 
       (.CI(\ip_cur_tranx_reg[48]_i_1_n_0 ),
        .CI_TOP(1'b0),
        .CO({\NLW_ip_cur_tranx_reg[56]_i_1_CO_UNCONNECTED [7],\ip_cur_tranx_reg[56]_i_1_n_1 ,\ip_cur_tranx_reg[56]_i_1_n_2 ,\ip_cur_tranx_reg[56]_i_1_n_3 ,\ip_cur_tranx_reg[56]_i_1_n_4 ,\ip_cur_tranx_reg[56]_i_1_n_5 ,\ip_cur_tranx_reg[56]_i_1_n_6 ,\ip_cur_tranx_reg[56]_i_1_n_7 }),
        .DI({1'b0,\ip_cur_tranx[56]_i_2_n_0 ,\ip_cur_tranx[56]_i_3_n_0 ,\ip_cur_tranx[56]_i_4_n_0 ,\ip_cur_tranx[56]_i_5_n_0 ,\ip_cur_tranx[56]_i_6_n_0 ,\ip_cur_tranx[56]_i_7_n_0 ,\ip_cur_tranx[56]_i_8_n_0 }),
        .O(ap_start_reg_reg_6),
        .S({\ip_cur_tranx[56]_i_9_n_0 ,\ip_cur_tranx[56]_i_10_n_0 ,\ip_cur_tranx[56]_i_11_n_0 ,\ip_cur_tranx[56]_i_12_n_0 ,\ip_cur_tranx[56]_i_13_n_0 ,\ip_cur_tranx[56]_i_14_n_0 ,\ip_cur_tranx[56]_i_15_n_0 ,\ip_cur_tranx[56]_i_16_n_0 }));
  (* ADDER_THRESHOLD = "16" *) 
  CARRY8 \ip_cur_tranx_reg[8]_i_1 
       (.CI(\ip_cur_tranx_reg[0]_i_2_n_0 ),
        .CI_TOP(1'b0),
        .CO({\ip_cur_tranx_reg[8]_i_1_n_0 ,\ip_cur_tranx_reg[8]_i_1_n_1 ,\ip_cur_tranx_reg[8]_i_1_n_2 ,\ip_cur_tranx_reg[8]_i_1_n_3 ,\ip_cur_tranx_reg[8]_i_1_n_4 ,\ip_cur_tranx_reg[8]_i_1_n_5 ,\ip_cur_tranx_reg[8]_i_1_n_6 ,\ip_cur_tranx_reg[8]_i_1_n_7 }),
        .DI({\ip_cur_tranx[8]_i_2_n_0 ,\ip_cur_tranx[8]_i_3_n_0 ,\ip_cur_tranx[8]_i_4_n_0 ,\ip_cur_tranx[8]_i_5_n_0 ,\ip_cur_tranx[8]_i_6_n_0 ,\ip_cur_tranx[8]_i_7_n_0 ,\ip_cur_tranx[8]_i_8_n_0 ,\ip_cur_tranx[8]_i_9_n_0 }),
        .O(\ip_cur_tranx_reg[0] ),
        .S({\ip_cur_tranx[8]_i_10_n_0 ,\ip_cur_tranx[8]_i_11_n_0 ,\ip_cur_tranx[8]_i_12_n_0 ,\ip_cur_tranx[8]_i_13_n_0 ,\ip_cur_tranx[8]_i_14_n_0 ,\ip_cur_tranx[8]_i_15_n_0 ,\ip_cur_tranx[8]_i_16_n_0 ,\ip_cur_tranx[8]_i_17_n_0 }));
  (* SOFT_HLUTNM = "soft_lutpair29" *) 
  LUT4 #(
    .INIT(16'hE200)) 
    \ip_exec_count[0]_i_1 
       (.I0(ap_done_reg),
        .I1(dataflow_en),
        .I2(ap_continue_reg),
        .I3(Metrics_Cnt_En),
        .O(ip_exec_count0));
  LUT2 #(
    .INIT(4'h8)) 
    \last_read_addr[7]_i_1 
       (.I0(s_axi_arvalid_mon),
        .I1(s_axi_arready_mon),
        .O(read_addr_valid));
  FDRE \last_read_addr_reg[0] 
       (.C(mon_clk),
        .CE(read_addr_valid),
        .D(s_axi_araddr_mon[0]),
        .Q(last_read_addr[0]),
        .R(SS));
  FDRE \last_read_addr_reg[1] 
       (.C(mon_clk),
        .CE(read_addr_valid),
        .D(s_axi_araddr_mon[1]),
        .Q(last_read_addr[1]),
        .R(SS));
  FDRE \last_read_addr_reg[2] 
       (.C(mon_clk),
        .CE(read_addr_valid),
        .D(s_axi_araddr_mon[2]),
        .Q(last_read_addr[2]),
        .R(SS));
  FDRE \last_read_addr_reg[3] 
       (.C(mon_clk),
        .CE(read_addr_valid),
        .D(s_axi_araddr_mon[3]),
        .Q(last_read_addr[3]),
        .R(SS));
  FDRE \last_read_addr_reg[4] 
       (.C(mon_clk),
        .CE(read_addr_valid),
        .D(s_axi_araddr_mon[4]),
        .Q(last_read_addr[4]),
        .R(SS));
  FDRE \last_read_addr_reg[5] 
       (.C(mon_clk),
        .CE(read_addr_valid),
        .D(s_axi_araddr_mon[5]),
        .Q(last_read_addr[5]),
        .R(SS));
  FDRE \last_read_addr_reg[6] 
       (.C(mon_clk),
        .CE(read_addr_valid),
        .D(s_axi_araddr_mon[6]),
        .Q(last_read_addr[6]),
        .R(SS));
  FDRE \last_read_addr_reg[7] 
       (.C(mon_clk),
        .CE(read_addr_valid),
        .D(s_axi_araddr_mon[7]),
        .Q(last_read_addr[7]),
        .R(SS));
  LUT2 #(
    .INIT(4'h8)) 
    \last_write_addr[7]_i_1 
       (.I0(s_axi_awvalid_mon),
        .I1(s_axi_awready_mon),
        .O(write_addr_valid));
  FDRE \last_write_addr_reg[0] 
       (.C(mon_clk),
        .CE(write_addr_valid),
        .D(s_axi_awaddr_mon[0]),
        .Q(last_write_addr[0]),
        .R(SS));
  FDRE \last_write_addr_reg[1] 
       (.C(mon_clk),
        .CE(write_addr_valid),
        .D(s_axi_awaddr_mon[1]),
        .Q(last_write_addr[1]),
        .R(SS));
  FDRE \last_write_addr_reg[2] 
       (.C(mon_clk),
        .CE(write_addr_valid),
        .D(s_axi_awaddr_mon[2]),
        .Q(last_write_addr[2]),
        .R(SS));
  FDRE \last_write_addr_reg[3] 
       (.C(mon_clk),
        .CE(write_addr_valid),
        .D(s_axi_awaddr_mon[3]),
        .Q(last_write_addr[3]),
        .R(SS));
  FDRE \last_write_addr_reg[4] 
       (.C(mon_clk),
        .CE(write_addr_valid),
        .D(s_axi_awaddr_mon[4]),
        .Q(last_write_addr[4]),
        .R(SS));
  FDRE \last_write_addr_reg[5] 
       (.C(mon_clk),
        .CE(write_addr_valid),
        .D(s_axi_awaddr_mon[5]),
        .Q(last_write_addr[5]),
        .R(SS));
  FDRE \last_write_addr_reg[6] 
       (.C(mon_clk),
        .CE(write_addr_valid),
        .D(s_axi_awaddr_mon[6]),
        .Q(last_write_addr[6]),
        .R(SS));
  FDRE \last_write_addr_reg[7] 
       (.C(mon_clk),
        .CE(write_addr_valid),
        .D(s_axi_awaddr_mon[7]),
        .Q(last_write_addr[7]),
        .R(SS));
  (* SOFT_HLUTNM = "soft_lutpair30" *) 
  LUT2 #(
    .INIT(4'hE)) 
    started_i_1
       (.I0(start_pulse),
        .I1(started),
        .O(started_i_1_n_0));
  FDRE #(
    .INIT(1'b0)) 
    started_reg
       (.C(mon_clk),
        .CE(1'b1),
        .D(started_i_1_n_0),
        .Q(started),
        .R(SS));
  (* SOFT_HLUTNM = "soft_lutpair30" *) 
  LUT2 #(
    .INIT(4'h8)) 
    tr_cdc_start_0_i_1
       (.I0(start_pulse),
        .I1(Q),
        .O(ap_start_reg_reg_0));
  (* SOFT_HLUTNM = "soft_lutpair29" *) 
  LUT4 #(
    .INIT(16'hE200)) 
    tr_cdc_stop_0_i_1
       (.I0(ap_done_reg),
        .I1(dataflow_en),
        .I2(ap_continue_reg),
        .I3(Q),
        .O(src_in));
  LUT4 #(
    .INIT(16'h00E2)) 
    xpm_fifo_async_inst_i_3
       (.I0(ap_done_reg),
        .I1(dataflow_en),
        .I2(ap_continue_reg),
        .I3(empty),
        .O(rd_en));
endmodule

(* CHECK_LICENSE_TYPE = "pfm_dynamic_dpa_mon20_0,accelerator_monitor,{}" *) (* DowngradeIPIdentifiedWarnings = "yes" *) (* X_CORE_INFO = "accelerator_monitor,Vivado 2020.2" *) 
(* NotValidForBitStream *)
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix
   (mon_clk,
    mon_resetn,
    trace_clk,
    trace_rst,
    trace_counter_overflow,
    trace_counter,
    trace_event,
    trace_data,
    trace_valid,
    trace_read,
    s_axi_awaddr,
    s_axi_awprot,
    s_axi_awvalid,
    s_axi_awready,
    s_axi_wdata,
    s_axi_wstrb,
    s_axi_wvalid,
    s_axi_wready,
    s_axi_bvalid,
    s_axi_bready,
    s_axi_bresp,
    s_axi_araddr,
    s_axi_arprot,
    s_axi_arvalid,
    s_axi_arready,
    s_axi_rdata,
    s_axi_rresp,
    s_axi_rvalid,
    s_axi_rready,
    s_axi_awaddr_mon,
    s_axi_awprot_mon,
    s_axi_awvalid_mon,
    s_axi_awready_mon,
    s_axi_wdata_mon,
    s_axi_wstrb_mon,
    s_axi_wvalid_mon,
    s_axi_wready_mon,
    s_axi_bresp_mon,
    s_axi_bvalid_mon,
    s_axi_bready_mon,
    s_axi_araddr_mon,
    s_axi_arprot_mon,
    s_axi_arvalid_mon,
    s_axi_arready_mon,
    s_axi_rdata_mon,
    s_axi_rresp_mon,
    s_axi_rvalid_mon,
    s_axi_rready_mon);
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 mon_clk CLK" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME mon_clk, ASSOCIATED_RESET mon_resetn, ASSOCIATED_BUSIF S_AXI:MON_AP_CTRL:S_AXI_MON, FREQ_HZ 300000000, FREQ_TOLERANCE_HZ 0, PHASE 0.000, CLK_DOMAIN pfm_dynamic_clkwiz_kernel_clk_out1, INSERT_VIP 0" *) input mon_clk;
  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 mon_resetn RST" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME mon_resetn, POLARITY ACTIVE_LOW, INSERT_VIP 0" *) input mon_resetn;
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 trace_clk CLK" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME trace_clk, ASSOCIATED_RESET trace_rst, ASSOCIATED_BUSIF TRACE_OUT, FREQ_HZ 300000000, FREQ_TOLERANCE_HZ 0, PHASE 0.000, CLK_DOMAIN pfm_dynamic_clkwiz_kernel_clk_out1, INSERT_VIP 0" *) input trace_clk;
  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 trace_rst RST" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME trace_rst, POLARITY ACTIVE_LOW, INSERT_VIP 0" *) input trace_rst;
  (* X_INTERFACE_INFO = "xilinx.com:interface:sdsoc_trace:2.0 TRACE_OUT counter_overflow" *) input trace_counter_overflow;
  (* X_INTERFACE_INFO = "xilinx.com:interface:sdsoc_trace:2.0 TRACE_OUT counter" *) input [44:0]trace_counter;
  (* X_INTERFACE_INFO = "xilinx.com:interface:sdsoc_trace:2.0 TRACE_OUT event" *) output trace_event;
  (* X_INTERFACE_INFO = "xilinx.com:interface:sdsoc_trace:2.0 TRACE_OUT data" *) output [63:0]trace_data;
  (* X_INTERFACE_INFO = "xilinx.com:interface:sdsoc_trace:2.0 TRACE_OUT valid" *) output trace_valid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:sdsoc_trace:2.0 TRACE_OUT read" *) input trace_read;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWADDR" *) input [31:0]s_axi_awaddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWPROT" *) input [2:0]s_axi_awprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWVALID" *) input s_axi_awvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWREADY" *) output s_axi_awready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI WDATA" *) input [31:0]s_axi_wdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI WSTRB" *) input [3:0]s_axi_wstrb;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI WVALID" *) input s_axi_wvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI WREADY" *) output s_axi_wready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI BVALID" *) output s_axi_bvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI BREADY" *) input s_axi_bready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI BRESP" *) output [1:0]s_axi_bresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI ARADDR" *) input [31:0]s_axi_araddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI ARPROT" *) input [2:0]s_axi_arprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI ARVALID" *) input s_axi_arvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI ARREADY" *) output s_axi_arready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI RDATA" *) output [31:0]s_axi_rdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI RRESP" *) output [1:0]s_axi_rresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI RVALID" *) output s_axi_rvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI RREADY" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME S_AXI, DATA_WIDTH 32, PROTOCOL AXI4LITE, FREQ_HZ 300000000, ID_WIDTH 0, ADDR_WIDTH 32, AWUSER_WIDTH 0, ARUSER_WIDTH 0, WUSER_WIDTH 0, RUSER_WIDTH 0, BUSER_WIDTH 0, READ_WRITE_MODE READ_WRITE, HAS_BURST 0, HAS_LOCK 0, HAS_PROT 1, HAS_CACHE 0, HAS_QOS 0, HAS_REGION 0, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 1, SUPPORTS_NARROW_BURST 0, NUM_READ_OUTSTANDING 1, NUM_WRITE_OUTSTANDING 1, MAX_BURST_LENGTH 1, PHASE 0.000, CLK_DOMAIN pfm_dynamic_clkwiz_kernel_clk_out1, NUM_READ_THREADS 1, NUM_WRITE_THREADS 1, RUSER_BITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0" *) input s_axi_rready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI_MON AWADDR" *) input [7:0]s_axi_awaddr_mon;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI_MON AWPROT" *) input [2:0]s_axi_awprot_mon;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI_MON AWVALID" *) input s_axi_awvalid_mon;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI_MON AWREADY" *) input s_axi_awready_mon;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI_MON WDATA" *) input [31:0]s_axi_wdata_mon;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI_MON WSTRB" *) input [3:0]s_axi_wstrb_mon;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI_MON WVALID" *) input s_axi_wvalid_mon;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI_MON WREADY" *) input s_axi_wready_mon;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI_MON BRESP" *) input [1:0]s_axi_bresp_mon;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI_MON BVALID" *) input s_axi_bvalid_mon;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI_MON BREADY" *) input s_axi_bready_mon;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI_MON ARADDR" *) input [7:0]s_axi_araddr_mon;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI_MON ARPROT" *) input [2:0]s_axi_arprot_mon;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI_MON ARVALID" *) input s_axi_arvalid_mon;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI_MON ARREADY" *) input s_axi_arready_mon;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI_MON RDATA" *) input [31:0]s_axi_rdata_mon;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI_MON RRESP" *) input [1:0]s_axi_rresp_mon;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI_MON RVALID" *) input s_axi_rvalid_mon;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI_MON RREADY" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME S_AXI_MON, DATA_WIDTH 32, PROTOCOL AXI4LITE, FREQ_HZ 300000000, ID_WIDTH 0, ADDR_WIDTH 8, AWUSER_WIDTH 0, ARUSER_WIDTH 0, WUSER_WIDTH 0, RUSER_WIDTH 0, BUSER_WIDTH 0, READ_WRITE_MODE READ_WRITE, HAS_BURST 0, HAS_LOCK 0, HAS_PROT 1, HAS_CACHE 0, HAS_QOS 0, HAS_REGION 0, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 1, SUPPORTS_NARROW_BURST 0, NUM_READ_OUTSTANDING 1, NUM_WRITE_OUTSTANDING 1, MAX_BURST_LENGTH 1, PHASE 0.000, CLK_DOMAIN pfm_dynamic_clkwiz_kernel_clk_out1, NUM_READ_THREADS 1, NUM_WRITE_THREADS 1, RUSER_BITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0" *) input s_axi_rready_mon;

  wire \<const0> ;
  wire \<const1> ;
  wire mon_clk;
  wire mon_resetn;
  wire [31:0]s_axi_araddr;
  wire [7:0]s_axi_araddr_mon;
  wire s_axi_arready;
  wire s_axi_arready_mon;
  wire s_axi_arvalid;
  wire s_axi_arvalid_mon;
  wire [31:0]s_axi_awaddr;
  wire [7:0]s_axi_awaddr_mon;
  wire s_axi_awready;
  wire s_axi_awready_mon;
  wire s_axi_awvalid;
  wire s_axi_awvalid_mon;
  wire s_axi_bready;
  wire s_axi_bvalid;
  wire [31:0]s_axi_rdata;
  wire [31:0]s_axi_rdata_mon;
  wire s_axi_rready;
  wire s_axi_rready_mon;
  wire s_axi_rvalid;
  wire s_axi_rvalid_mon;
  wire [31:0]s_axi_wdata;
  wire [31:0]s_axi_wdata_mon;
  wire s_axi_wready;
  wire s_axi_wready_mon;
  wire [3:0]s_axi_wstrb_mon;
  wire s_axi_wvalid;
  wire s_axi_wvalid_mon;
  wire trace_clk;
  wire [44:0]trace_counter;
  wire trace_counter_overflow;
  wire [63:0]\^trace_data ;
  wire trace_read;
  wire trace_rst;
  wire trace_valid;

  assign s_axi_bresp[1] = \<const0> ;
  assign s_axi_bresp[0] = \<const0> ;
  assign s_axi_rresp[1] = \<const0> ;
  assign s_axi_rresp[0] = \<const0> ;
  assign trace_data[63:61] = \^trace_data [63:61];
  assign trace_data[60] = \<const0> ;
  assign trace_data[59] = \<const0> ;
  assign trace_data[58] = \<const0> ;
  assign trace_data[57] = \<const0> ;
  assign trace_data[56] = \<const1> ;
  assign trace_data[55] = \<const0> ;
  assign trace_data[54] = \<const0> ;
  assign trace_data[53] = \<const1> ;
  assign trace_data[52:0] = \^trace_data [52:0];
  assign trace_event = \<const0> ;
  GND GND
       (.G(\<const0> ));
  VCC VCC
       (.P(\<const1> ));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_accelerator_monitor inst
       (.mon_clk(mon_clk),
        .mon_resetn(mon_resetn),
        .s_axi_araddr(s_axi_araddr[7:0]),
        .s_axi_araddr_mon(s_axi_araddr_mon),
        .s_axi_arready(s_axi_arready),
        .s_axi_arready_mon(s_axi_arready_mon),
        .s_axi_arvalid(s_axi_arvalid),
        .s_axi_arvalid_mon(s_axi_arvalid_mon),
        .s_axi_awaddr(s_axi_awaddr[7:0]),
        .s_axi_awaddr_mon(s_axi_awaddr_mon),
        .s_axi_awready(s_axi_awready),
        .s_axi_awready_mon(s_axi_awready_mon),
        .s_axi_awvalid(s_axi_awvalid),
        .s_axi_awvalid_mon(s_axi_awvalid_mon),
        .s_axi_bready(s_axi_bready),
        .s_axi_bvalid(s_axi_bvalid),
        .s_axi_rdata(s_axi_rdata),
        .s_axi_rdata_mon(s_axi_rdata_mon[1]),
        .s_axi_rready(s_axi_rready),
        .s_axi_rready_mon(s_axi_rready_mon),
        .s_axi_rvalid(s_axi_rvalid),
        .s_axi_rvalid_mon(s_axi_rvalid_mon),
        .s_axi_wdata(s_axi_wdata[5:0]),
        .s_axi_wdata_mon({s_axi_wdata_mon[4],s_axi_wdata_mon[0]}),
        .s_axi_wready(s_axi_wready),
        .s_axi_wready_mon(s_axi_wready_mon),
        .s_axi_wstrb_mon(s_axi_wstrb_mon[0]),
        .s_axi_wvalid(s_axi_wvalid),
        .s_axi_wvalid_mon(s_axi_wvalid_mon),
        .trace_clk(trace_clk),
        .trace_counter(trace_counter),
        .trace_counter_overflow(trace_counter_overflow),
        .trace_data({\^trace_data [63:61],\^trace_data [52:0]}),
        .trace_read(trace_read),
        .trace_rst(trace_rst),
        .trace_valid(trace_valid));
endmodule

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_register_module
   (SS,
    Metrics_Cnt_En,
    reset_on_sample_read_reg_0,
    dataflow_en,
    slv_reg_in_vld_reg_0,
    \register_select_reg[1]_0 ,
    Q,
    sample_reg_rd_first,
    metrics_cnt_reset_reg_0,
    metrics_cnt_reset_reg_1,
    RST_ACTIVE,
    ip_start_count0,
    \trace_control_reg[0]_0 ,
    slv_reg_in_vld_reg_1,
    \slv_reg_in_reg[31]_0 ,
    slv_reg_out_vld,
    mon_clk,
    slv_reg_addr_vld,
    control_wr_en,
    s_axi_wdata,
    p_1_in,
    sample_reg_rd_first_reg_0,
    mon_resetn,
    CO,
    start_pulse,
    reset_sample_reg__0,
    \slv_reg_in_reg[31]_1 ,
    s_axi_rvalid,
    s_axi_rready,
    E,
    D,
    \trace_control_reg[5]_0 ,
    \sample_time_diff_reg_reg[31]_0 ,
    \register_select_reg[5]_0 );
  output [0:0]SS;
  output Metrics_Cnt_En;
  output reset_on_sample_read_reg_0;
  output dataflow_en;
  output [0:0]slv_reg_in_vld_reg_0;
  output [0:0]\register_select_reg[1]_0 ;
  output [0:0]Q;
  output sample_reg_rd_first;
  output metrics_cnt_reset_reg_0;
  output metrics_cnt_reset_reg_1;
  output RST_ACTIVE;
  output ip_start_count0;
  output [0:0]\trace_control_reg[0]_0 ;
  output slv_reg_in_vld_reg_1;
  output [31:0]\slv_reg_in_reg[31]_0 ;
  input slv_reg_out_vld;
  input mon_clk;
  input slv_reg_addr_vld;
  input control_wr_en;
  input [5:0]s_axi_wdata;
  input p_1_in;
  input sample_reg_rd_first_reg_0;
  input mon_resetn;
  input [0:0]CO;
  input start_pulse;
  input reset_sample_reg__0;
  input [31:0]\slv_reg_in_reg[31]_1 ;
  input s_axi_rvalid;
  input s_axi_rready;
  input [0:0]E;
  input [0:0]D;
  input [0:0]\trace_control_reg[5]_0 ;
  input [0:0]\sample_time_diff_reg_reg[31]_0 ;
  input [3:0]\register_select_reg[5]_0 ;

  wire [0:0]CO;
  wire [0:0]D;
  wire [0:0]E;
  wire Metrics_Cnt_En;
  wire Metrics_Cnt_Reset;
  wire [0:0]Q;
  wire RST_ACTIVE;
  wire [0:0]SS;
  wire control_wr_en;
  wire dataflow_en;
  wire ip_start_count0;
  wire metrics_cnt_reset_reg_0;
  wire metrics_cnt_reset_reg_1;
  wire mon_clk;
  wire mon_resetn;
  wire p_1_in;
  wire [5:0]register_select;
  wire [0:0]\register_select_reg[1]_0 ;
  wire [3:0]\register_select_reg[5]_0 ;
  wire reset_on_sample_read_reg_0;
  wire reset_sample_reg__0;
  wire s_axi_rready;
  wire s_axi_rvalid;
  wire [5:0]s_axi_wdata;
  wire sample_reg_counter_inst_n_0;
  wire sample_reg_counter_inst_n_1;
  wire sample_reg_counter_inst_n_10;
  wire sample_reg_counter_inst_n_11;
  wire sample_reg_counter_inst_n_12;
  wire sample_reg_counter_inst_n_13;
  wire sample_reg_counter_inst_n_14;
  wire sample_reg_counter_inst_n_15;
  wire sample_reg_counter_inst_n_16;
  wire sample_reg_counter_inst_n_17;
  wire sample_reg_counter_inst_n_18;
  wire sample_reg_counter_inst_n_19;
  wire sample_reg_counter_inst_n_2;
  wire sample_reg_counter_inst_n_20;
  wire sample_reg_counter_inst_n_21;
  wire sample_reg_counter_inst_n_22;
  wire sample_reg_counter_inst_n_23;
  wire sample_reg_counter_inst_n_24;
  wire sample_reg_counter_inst_n_25;
  wire sample_reg_counter_inst_n_26;
  wire sample_reg_counter_inst_n_27;
  wire sample_reg_counter_inst_n_28;
  wire sample_reg_counter_inst_n_29;
  wire sample_reg_counter_inst_n_3;
  wire sample_reg_counter_inst_n_30;
  wire sample_reg_counter_inst_n_4;
  wire sample_reg_counter_inst_n_5;
  wire sample_reg_counter_inst_n_6;
  wire sample_reg_counter_inst_n_7;
  wire sample_reg_counter_inst_n_8;
  wire sample_reg_counter_inst_n_9;
  wire sample_reg_rd_first;
  wire sample_reg_rd_first_reg_0;
  wire [0:0]\sample_time_diff_reg_reg[31]_0 ;
  wire \sample_time_diff_reg_reg_n_0_[0] ;
  wire \sample_time_diff_reg_reg_n_0_[10] ;
  wire \sample_time_diff_reg_reg_n_0_[11] ;
  wire \sample_time_diff_reg_reg_n_0_[12] ;
  wire \sample_time_diff_reg_reg_n_0_[13] ;
  wire \sample_time_diff_reg_reg_n_0_[14] ;
  wire \sample_time_diff_reg_reg_n_0_[15] ;
  wire \sample_time_diff_reg_reg_n_0_[16] ;
  wire \sample_time_diff_reg_reg_n_0_[17] ;
  wire \sample_time_diff_reg_reg_n_0_[18] ;
  wire \sample_time_diff_reg_reg_n_0_[19] ;
  wire \sample_time_diff_reg_reg_n_0_[1] ;
  wire \sample_time_diff_reg_reg_n_0_[20] ;
  wire \sample_time_diff_reg_reg_n_0_[21] ;
  wire \sample_time_diff_reg_reg_n_0_[22] ;
  wire \sample_time_diff_reg_reg_n_0_[23] ;
  wire \sample_time_diff_reg_reg_n_0_[24] ;
  wire \sample_time_diff_reg_reg_n_0_[25] ;
  wire \sample_time_diff_reg_reg_n_0_[26] ;
  wire \sample_time_diff_reg_reg_n_0_[27] ;
  wire \sample_time_diff_reg_reg_n_0_[28] ;
  wire \sample_time_diff_reg_reg_n_0_[29] ;
  wire \sample_time_diff_reg_reg_n_0_[2] ;
  wire \sample_time_diff_reg_reg_n_0_[30] ;
  wire \sample_time_diff_reg_reg_n_0_[31] ;
  wire \sample_time_diff_reg_reg_n_0_[3] ;
  wire \sample_time_diff_reg_reg_n_0_[4] ;
  wire \sample_time_diff_reg_reg_n_0_[5] ;
  wire \sample_time_diff_reg_reg_n_0_[6] ;
  wire \sample_time_diff_reg_reg_n_0_[7] ;
  wire \sample_time_diff_reg_reg_n_0_[8] ;
  wire \sample_time_diff_reg_reg_n_0_[9] ;
  wire slv_reg_addr_vld;
  wire slv_reg_addr_vld_reg;
  wire slv_reg_in1;
  wire \slv_reg_in[0]_i_1_n_0 ;
  wire \slv_reg_in[0]_i_2_n_0 ;
  wire \slv_reg_in[10]_i_1_n_0 ;
  wire \slv_reg_in[11]_i_1_n_0 ;
  wire \slv_reg_in[12]_i_1_n_0 ;
  wire \slv_reg_in[13]_i_1_n_0 ;
  wire \slv_reg_in[14]_i_1_n_0 ;
  wire \slv_reg_in[15]_i_1_n_0 ;
  wire \slv_reg_in[16]_i_1_n_0 ;
  wire \slv_reg_in[17]_i_1_n_0 ;
  wire \slv_reg_in[18]_i_1_n_0 ;
  wire \slv_reg_in[19]_i_1_n_0 ;
  wire \slv_reg_in[1]_i_1_n_0 ;
  wire \slv_reg_in[1]_i_2_n_0 ;
  wire \slv_reg_in[20]_i_1_n_0 ;
  wire \slv_reg_in[21]_i_1_n_0 ;
  wire \slv_reg_in[22]_i_1_n_0 ;
  wire \slv_reg_in[23]_i_1_n_0 ;
  wire \slv_reg_in[24]_i_1_n_0 ;
  wire \slv_reg_in[25]_i_1_n_0 ;
  wire \slv_reg_in[26]_i_1_n_0 ;
  wire \slv_reg_in[27]_i_1_n_0 ;
  wire \slv_reg_in[28]_i_1_n_0 ;
  wire \slv_reg_in[29]_i_1_n_0 ;
  wire \slv_reg_in[2]_i_1_n_0 ;
  wire \slv_reg_in[2]_i_2_n_0 ;
  wire \slv_reg_in[30]_i_1_n_0 ;
  wire \slv_reg_in[31]_i_1_n_0 ;
  wire \slv_reg_in[31]_i_2_n_0 ;
  wire \slv_reg_in[31]_i_3_n_0 ;
  wire \slv_reg_in[31]_i_4_n_0 ;
  wire \slv_reg_in[31]_i_5_n_0 ;
  wire \slv_reg_in[3]_i_1_n_0 ;
  wire \slv_reg_in[3]_i_2_n_0 ;
  wire \slv_reg_in[3]_i_3_n_0 ;
  wire \slv_reg_in[4]_i_1_n_0 ;
  wire \slv_reg_in[5]_i_1_n_0 ;
  wire \slv_reg_in[5]_i_2_n_0 ;
  wire \slv_reg_in[5]_i_3_n_0 ;
  wire \slv_reg_in[6]_i_1_n_0 ;
  wire \slv_reg_in[7]_i_1_n_0 ;
  wire \slv_reg_in[8]_i_1_n_0 ;
  wire \slv_reg_in[9]_i_1_n_0 ;
  wire [31:0]\slv_reg_in_reg[31]_0 ;
  wire [31:0]\slv_reg_in_reg[31]_1 ;
  wire [0:0]slv_reg_in_vld_reg_0;
  wire slv_reg_in_vld_reg_1;
  wire slv_reg_out_vld;
  wire slv_reg_out_vld_reg;
  wire start_pulse;
  wire [3:1]trace_control;
  wire [0:0]\trace_control_reg[0]_0 ;
  wire [0:0]\trace_control_reg[5]_0 ;
  wire \trace_control_reg_n_0_[4] ;
  wire \trace_control_reg_n_0_[5] ;

  LUT3 #(
    .INIT(8'hAE)) 
    axi_rvalid_i_1
       (.I0(slv_reg_in_vld_reg_0),
        .I1(s_axi_rvalid),
        .I2(s_axi_rready),
        .O(slv_reg_in_vld_reg_1));
  (* SOFT_HLUTNM = "soft_lutpair48" *) 
  LUT4 #(
    .INIT(16'h0400)) 
    cnt_enabled_i_1
       (.I0(Metrics_Cnt_Reset),
        .I1(mon_resetn),
        .I2(CO),
        .I3(Metrics_Cnt_En),
        .O(metrics_cnt_reset_reg_1));
  FDRE dataflow_en_reg
       (.C(mon_clk),
        .CE(control_wr_en),
        .D(s_axi_wdata[3]),
        .Q(dataflow_en),
        .R(SS));
  (* SOFT_HLUTNM = "soft_lutpair48" *) 
  LUT2 #(
    .INIT(4'hB)) 
    \ip_max_parallel_tranx[63]_i_1 
       (.I0(Metrics_Cnt_Reset),
        .I1(mon_resetn),
        .O(metrics_cnt_reset_reg_0));
  LUT2 #(
    .INIT(4'h8)) 
    \ip_start_count[0]_i_1 
       (.I0(Metrics_Cnt_En),
        .I1(start_pulse),
        .O(ip_start_count0));
  FDSE metrics_cnt_en_reg
       (.C(mon_clk),
        .CE(control_wr_en),
        .D(s_axi_wdata[0]),
        .Q(Metrics_Cnt_En),
        .S(SS));
  FDRE metrics_cnt_reset_reg
       (.C(mon_clk),
        .CE(control_wr_en),
        .D(s_axi_wdata[1]),
        .Q(Metrics_Cnt_Reset),
        .R(SS));
  FDRE \register_select_reg[0] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\register_select_reg[5]_0 [0]),
        .Q(register_select[0]),
        .R(SS));
  FDRE \register_select_reg[1] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(p_1_in),
        .Q(\register_select_reg[1]_0 ),
        .R(SS));
  FDRE \register_select_reg[3] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\register_select_reg[5]_0 [1]),
        .Q(register_select[3]),
        .R(SS));
  FDRE \register_select_reg[4] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\register_select_reg[5]_0 [2]),
        .Q(register_select[4]),
        .R(SS));
  FDRE \register_select_reg[5] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\register_select_reg[5]_0 [3]),
        .Q(register_select[5]),
        .R(SS));
  FDRE reset_on_sample_read_reg
       (.C(mon_clk),
        .CE(control_wr_en),
        .D(s_axi_wdata[2]),
        .Q(reset_on_sample_read_reg_0),
        .R(SS));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_counter sample_reg_counter_inst
       (.D(D),
        .E(E),
        .Q({sample_reg_counter_inst_n_0,sample_reg_counter_inst_n_1,sample_reg_counter_inst_n_2,sample_reg_counter_inst_n_3,sample_reg_counter_inst_n_4,sample_reg_counter_inst_n_5,sample_reg_counter_inst_n_6,sample_reg_counter_inst_n_7,sample_reg_counter_inst_n_8,sample_reg_counter_inst_n_9,sample_reg_counter_inst_n_10,sample_reg_counter_inst_n_11,sample_reg_counter_inst_n_12,sample_reg_counter_inst_n_13,sample_reg_counter_inst_n_14,sample_reg_counter_inst_n_15,sample_reg_counter_inst_n_16,sample_reg_counter_inst_n_17,sample_reg_counter_inst_n_18,sample_reg_counter_inst_n_19,sample_reg_counter_inst_n_20,sample_reg_counter_inst_n_21,sample_reg_counter_inst_n_22,sample_reg_counter_inst_n_23,sample_reg_counter_inst_n_24,sample_reg_counter_inst_n_25,sample_reg_counter_inst_n_26,sample_reg_counter_inst_n_27,sample_reg_counter_inst_n_28,sample_reg_counter_inst_n_29,sample_reg_counter_inst_n_30,Q}),
        .SR(SS),
        .mon_clk(mon_clk),
        .mon_resetn(mon_resetn),
        .reset_sample_reg__0(reset_sample_reg__0));
  FDRE sample_reg_rd_first_reg
       (.C(mon_clk),
        .CE(1'b1),
        .D(sample_reg_rd_first_reg_0),
        .Q(sample_reg_rd_first),
        .R(SS));
  FDRE \sample_time_diff_reg_reg[0] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(Q),
        .Q(\sample_time_diff_reg_reg_n_0_[0] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[10] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_21),
        .Q(\sample_time_diff_reg_reg_n_0_[10] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[11] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_20),
        .Q(\sample_time_diff_reg_reg_n_0_[11] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[12] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_19),
        .Q(\sample_time_diff_reg_reg_n_0_[12] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[13] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_18),
        .Q(\sample_time_diff_reg_reg_n_0_[13] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[14] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_17),
        .Q(\sample_time_diff_reg_reg_n_0_[14] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[15] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_16),
        .Q(\sample_time_diff_reg_reg_n_0_[15] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[16] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_15),
        .Q(\sample_time_diff_reg_reg_n_0_[16] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[17] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_14),
        .Q(\sample_time_diff_reg_reg_n_0_[17] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[18] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_13),
        .Q(\sample_time_diff_reg_reg_n_0_[18] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[19] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_12),
        .Q(\sample_time_diff_reg_reg_n_0_[19] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[1] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_30),
        .Q(\sample_time_diff_reg_reg_n_0_[1] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[20] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_11),
        .Q(\sample_time_diff_reg_reg_n_0_[20] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[21] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_10),
        .Q(\sample_time_diff_reg_reg_n_0_[21] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[22] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_9),
        .Q(\sample_time_diff_reg_reg_n_0_[22] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[23] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_8),
        .Q(\sample_time_diff_reg_reg_n_0_[23] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[24] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_7),
        .Q(\sample_time_diff_reg_reg_n_0_[24] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[25] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_6),
        .Q(\sample_time_diff_reg_reg_n_0_[25] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[26] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_5),
        .Q(\sample_time_diff_reg_reg_n_0_[26] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[27] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_4),
        .Q(\sample_time_diff_reg_reg_n_0_[27] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[28] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_3),
        .Q(\sample_time_diff_reg_reg_n_0_[28] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[29] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_2),
        .Q(\sample_time_diff_reg_reg_n_0_[29] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[2] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_29),
        .Q(\sample_time_diff_reg_reg_n_0_[2] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[30] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_1),
        .Q(\sample_time_diff_reg_reg_n_0_[30] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[31] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_0),
        .Q(\sample_time_diff_reg_reg_n_0_[31] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[3] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_28),
        .Q(\sample_time_diff_reg_reg_n_0_[3] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[4] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_27),
        .Q(\sample_time_diff_reg_reg_n_0_[4] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[5] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_26),
        .Q(\sample_time_diff_reg_reg_n_0_[5] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[6] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_25),
        .Q(\sample_time_diff_reg_reg_n_0_[6] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[7] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_24),
        .Q(\sample_time_diff_reg_reg_n_0_[7] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[8] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_23),
        .Q(\sample_time_diff_reg_reg_n_0_[8] ),
        .R(1'b0));
  FDRE \sample_time_diff_reg_reg[9] 
       (.C(mon_clk),
        .CE(\sample_time_diff_reg_reg[31]_0 ),
        .D(sample_reg_counter_inst_n_22),
        .Q(\sample_time_diff_reg_reg_n_0_[9] ),
        .R(1'b0));
  FDRE slv_reg_addr_vld_reg_reg
       (.C(mon_clk),
        .CE(1'b1),
        .D(slv_reg_addr_vld),
        .Q(slv_reg_addr_vld_reg),
        .R(SS));
  LUT5 #(
    .INIT(32'hFFFFF888)) 
    \slv_reg_in[0]_i_1 
       (.I0(\slv_reg_in[31]_i_4_n_0 ),
        .I1(\sample_time_diff_reg_reg_n_0_[0] ),
        .I2(\slv_reg_in[5]_i_3_n_0 ),
        .I3(\slv_reg_in_reg[31]_1 [0]),
        .I4(\slv_reg_in[0]_i_2_n_0 ),
        .O(\slv_reg_in[0]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'h000C00000A000000)) 
    \slv_reg_in[0]_i_2 
       (.I0(Metrics_Cnt_En),
        .I1(\trace_control_reg[0]_0 ),
        .I2(register_select[0]),
        .I3(register_select[4]),
        .I4(\slv_reg_in[3]_i_3_n_0 ),
        .I5(register_select[3]),
        .O(\slv_reg_in[0]_i_2_n_0 ));
  LUT4 #(
    .INIT(16'hF888)) 
    \slv_reg_in[10]_i_1 
       (.I0(\slv_reg_in_reg[31]_1 [10]),
        .I1(\slv_reg_in[31]_i_5_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[10] ),
        .I3(\slv_reg_in[31]_i_4_n_0 ),
        .O(\slv_reg_in[10]_i_1_n_0 ));
  LUT4 #(
    .INIT(16'hF888)) 
    \slv_reg_in[11]_i_1 
       (.I0(\slv_reg_in_reg[31]_1 [11]),
        .I1(\slv_reg_in[31]_i_5_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[11] ),
        .I3(\slv_reg_in[31]_i_4_n_0 ),
        .O(\slv_reg_in[11]_i_1_n_0 ));
  LUT4 #(
    .INIT(16'hF888)) 
    \slv_reg_in[12]_i_1 
       (.I0(\slv_reg_in_reg[31]_1 [12]),
        .I1(\slv_reg_in[31]_i_5_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[12] ),
        .I3(\slv_reg_in[31]_i_4_n_0 ),
        .O(\slv_reg_in[12]_i_1_n_0 ));
  LUT4 #(
    .INIT(16'hF888)) 
    \slv_reg_in[13]_i_1 
       (.I0(\slv_reg_in_reg[31]_1 [13]),
        .I1(\slv_reg_in[31]_i_5_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[13] ),
        .I3(\slv_reg_in[31]_i_4_n_0 ),
        .O(\slv_reg_in[13]_i_1_n_0 ));
  LUT4 #(
    .INIT(16'hF888)) 
    \slv_reg_in[14]_i_1 
       (.I0(\slv_reg_in_reg[31]_1 [14]),
        .I1(\slv_reg_in[31]_i_5_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[14] ),
        .I3(\slv_reg_in[31]_i_4_n_0 ),
        .O(\slv_reg_in[14]_i_1_n_0 ));
  LUT4 #(
    .INIT(16'hF888)) 
    \slv_reg_in[15]_i_1 
       (.I0(\slv_reg_in_reg[31]_1 [15]),
        .I1(\slv_reg_in[31]_i_5_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[15] ),
        .I3(\slv_reg_in[31]_i_4_n_0 ),
        .O(\slv_reg_in[15]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hFFEAEAEA)) 
    \slv_reg_in[16]_i_1 
       (.I0(\slv_reg_in[31]_i_3_n_0 ),
        .I1(\slv_reg_in[31]_i_4_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[16] ),
        .I3(\slv_reg_in_reg[31]_1 [16]),
        .I4(\slv_reg_in[31]_i_5_n_0 ),
        .O(\slv_reg_in[16]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hFFEAEAEA)) 
    \slv_reg_in[17]_i_1 
       (.I0(\slv_reg_in[31]_i_3_n_0 ),
        .I1(\slv_reg_in[31]_i_4_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[17] ),
        .I3(\slv_reg_in_reg[31]_1 [17]),
        .I4(\slv_reg_in[31]_i_5_n_0 ),
        .O(\slv_reg_in[17]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hFFEAEAEA)) 
    \slv_reg_in[18]_i_1 
       (.I0(\slv_reg_in[31]_i_3_n_0 ),
        .I1(\slv_reg_in[31]_i_4_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[18] ),
        .I3(\slv_reg_in_reg[31]_1 [18]),
        .I4(\slv_reg_in[31]_i_5_n_0 ),
        .O(\slv_reg_in[18]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hFFEAEAEA)) 
    \slv_reg_in[19]_i_1 
       (.I0(\slv_reg_in[31]_i_3_n_0 ),
        .I1(\slv_reg_in[31]_i_4_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[19] ),
        .I3(\slv_reg_in_reg[31]_1 [19]),
        .I4(\slv_reg_in[31]_i_5_n_0 ),
        .O(\slv_reg_in[19]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hFFFFF888)) 
    \slv_reg_in[1]_i_1 
       (.I0(\slv_reg_in[31]_i_4_n_0 ),
        .I1(\sample_time_diff_reg_reg_n_0_[1] ),
        .I2(\slv_reg_in[5]_i_3_n_0 ),
        .I3(\slv_reg_in_reg[31]_1 [1]),
        .I4(\slv_reg_in[1]_i_2_n_0 ),
        .O(\slv_reg_in[1]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'h000C00000A000000)) 
    \slv_reg_in[1]_i_2 
       (.I0(Metrics_Cnt_Reset),
        .I1(trace_control[1]),
        .I2(register_select[0]),
        .I3(register_select[4]),
        .I4(\slv_reg_in[3]_i_3_n_0 ),
        .I5(register_select[3]),
        .O(\slv_reg_in[1]_i_2_n_0 ));
  LUT4 #(
    .INIT(16'hF888)) 
    \slv_reg_in[20]_i_1 
       (.I0(\slv_reg_in_reg[31]_1 [20]),
        .I1(\slv_reg_in[31]_i_5_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[20] ),
        .I3(\slv_reg_in[31]_i_4_n_0 ),
        .O(\slv_reg_in[20]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hFFEAEAEA)) 
    \slv_reg_in[21]_i_1 
       (.I0(\slv_reg_in[31]_i_3_n_0 ),
        .I1(\slv_reg_in[31]_i_4_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[21] ),
        .I3(\slv_reg_in_reg[31]_1 [21]),
        .I4(\slv_reg_in[31]_i_5_n_0 ),
        .O(\slv_reg_in[21]_i_1_n_0 ));
  LUT4 #(
    .INIT(16'hF888)) 
    \slv_reg_in[22]_i_1 
       (.I0(\slv_reg_in_reg[31]_1 [22]),
        .I1(\slv_reg_in[31]_i_5_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[22] ),
        .I3(\slv_reg_in[31]_i_4_n_0 ),
        .O(\slv_reg_in[22]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hFFEAEAEA)) 
    \slv_reg_in[23]_i_1 
       (.I0(\slv_reg_in[31]_i_3_n_0 ),
        .I1(\slv_reg_in[31]_i_4_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[23] ),
        .I3(\slv_reg_in_reg[31]_1 [23]),
        .I4(\slv_reg_in[31]_i_5_n_0 ),
        .O(\slv_reg_in[23]_i_1_n_0 ));
  LUT4 #(
    .INIT(16'hF888)) 
    \slv_reg_in[24]_i_1 
       (.I0(\slv_reg_in_reg[31]_1 [24]),
        .I1(\slv_reg_in[31]_i_5_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[24] ),
        .I3(\slv_reg_in[31]_i_4_n_0 ),
        .O(\slv_reg_in[24]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hFFEAEAEA)) 
    \slv_reg_in[25]_i_1 
       (.I0(\slv_reg_in[31]_i_3_n_0 ),
        .I1(\slv_reg_in[31]_i_4_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[25] ),
        .I3(\slv_reg_in_reg[31]_1 [25]),
        .I4(\slv_reg_in[31]_i_5_n_0 ),
        .O(\slv_reg_in[25]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hFFEAEAEA)) 
    \slv_reg_in[26]_i_1 
       (.I0(\slv_reg_in[31]_i_3_n_0 ),
        .I1(\slv_reg_in[31]_i_4_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[26] ),
        .I3(\slv_reg_in_reg[31]_1 [26]),
        .I4(\slv_reg_in[31]_i_5_n_0 ),
        .O(\slv_reg_in[26]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hFFEAEAEA)) 
    \slv_reg_in[27]_i_1 
       (.I0(\slv_reg_in[31]_i_3_n_0 ),
        .I1(\slv_reg_in[31]_i_4_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[27] ),
        .I3(\slv_reg_in_reg[31]_1 [27]),
        .I4(\slv_reg_in[31]_i_5_n_0 ),
        .O(\slv_reg_in[27]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hFFEAEAEA)) 
    \slv_reg_in[28]_i_1 
       (.I0(\slv_reg_in[31]_i_3_n_0 ),
        .I1(\slv_reg_in[31]_i_4_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[28] ),
        .I3(\slv_reg_in_reg[31]_1 [28]),
        .I4(\slv_reg_in[31]_i_5_n_0 ),
        .O(\slv_reg_in[28]_i_1_n_0 ));
  LUT4 #(
    .INIT(16'hF888)) 
    \slv_reg_in[29]_i_1 
       (.I0(\slv_reg_in_reg[31]_1 [29]),
        .I1(\slv_reg_in[31]_i_5_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[29] ),
        .I3(\slv_reg_in[31]_i_4_n_0 ),
        .O(\slv_reg_in[29]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hFFFFF888)) 
    \slv_reg_in[2]_i_1 
       (.I0(\slv_reg_in[31]_i_4_n_0 ),
        .I1(\sample_time_diff_reg_reg_n_0_[2] ),
        .I2(\slv_reg_in[5]_i_3_n_0 ),
        .I3(\slv_reg_in_reg[31]_1 [2]),
        .I4(\slv_reg_in[2]_i_2_n_0 ),
        .O(\slv_reg_in[2]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'h000C00000A000000)) 
    \slv_reg_in[2]_i_2 
       (.I0(reset_on_sample_read_reg_0),
        .I1(trace_control[2]),
        .I2(register_select[0]),
        .I3(register_select[4]),
        .I4(\slv_reg_in[3]_i_3_n_0 ),
        .I5(register_select[3]),
        .O(\slv_reg_in[2]_i_2_n_0 ));
  LUT5 #(
    .INIT(32'hFFEAEAEA)) 
    \slv_reg_in[30]_i_1 
       (.I0(\slv_reg_in[31]_i_3_n_0 ),
        .I1(\slv_reg_in[31]_i_4_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[30] ),
        .I3(\slv_reg_in_reg[31]_1 [30]),
        .I4(\slv_reg_in[31]_i_5_n_0 ),
        .O(\slv_reg_in[30]_i_1_n_0 ));
  LUT3 #(
    .INIT(8'hBF)) 
    \slv_reg_in[31]_i_1 
       (.I0(slv_reg_out_vld_reg),
        .I1(slv_reg_addr_vld_reg),
        .I2(mon_resetn),
        .O(\slv_reg_in[31]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hFFEAEAEA)) 
    \slv_reg_in[31]_i_2 
       (.I0(\slv_reg_in[31]_i_3_n_0 ),
        .I1(\slv_reg_in[31]_i_4_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[31] ),
        .I3(\slv_reg_in_reg[31]_1 [31]),
        .I4(\slv_reg_in[31]_i_5_n_0 ),
        .O(\slv_reg_in[31]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair46" *) 
  LUT5 #(
    .INIT(32'h00010000)) 
    \slv_reg_in[31]_i_3 
       (.I0(register_select[5]),
        .I1(\register_select_reg[1]_0 ),
        .I2(register_select[3]),
        .I3(register_select[4]),
        .I4(register_select[0]),
        .O(\slv_reg_in[31]_i_3_n_0 ));
  LUT5 #(
    .INIT(32'h00000004)) 
    \slv_reg_in[31]_i_4 
       (.I0(register_select[5]),
        .I1(\register_select_reg[1]_0 ),
        .I2(register_select[0]),
        .I3(register_select[3]),
        .I4(register_select[4]),
        .O(\slv_reg_in[31]_i_4_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair46" *) 
  LUT5 #(
    .INIT(32'h00000004)) 
    \slv_reg_in[31]_i_5 
       (.I0(\register_select_reg[1]_0 ),
        .I1(register_select[5]),
        .I2(register_select[0]),
        .I3(register_select[3]),
        .I4(register_select[4]),
        .O(\slv_reg_in[31]_i_5_n_0 ));
  LUT5 #(
    .INIT(32'hFFFFF888)) 
    \slv_reg_in[3]_i_1 
       (.I0(\slv_reg_in[31]_i_4_n_0 ),
        .I1(\sample_time_diff_reg_reg_n_0_[3] ),
        .I2(\slv_reg_in[5]_i_3_n_0 ),
        .I3(\slv_reg_in_reg[31]_1 [3]),
        .I4(\slv_reg_in[3]_i_2_n_0 ),
        .O(\slv_reg_in[3]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'h000C00000A000000)) 
    \slv_reg_in[3]_i_2 
       (.I0(dataflow_en),
        .I1(trace_control[3]),
        .I2(register_select[0]),
        .I3(register_select[4]),
        .I4(\slv_reg_in[3]_i_3_n_0 ),
        .I5(register_select[3]),
        .O(\slv_reg_in[3]_i_2_n_0 ));
  LUT2 #(
    .INIT(4'h1)) 
    \slv_reg_in[3]_i_3 
       (.I0(register_select[5]),
        .I1(\register_select_reg[1]_0 ),
        .O(\slv_reg_in[3]_i_3_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFF888F888F888)) 
    \slv_reg_in[4]_i_1 
       (.I0(\slv_reg_in[5]_i_2_n_0 ),
        .I1(\trace_control_reg_n_0_[4] ),
        .I2(\slv_reg_in[31]_i_4_n_0 ),
        .I3(\sample_time_diff_reg_reg_n_0_[4] ),
        .I4(\slv_reg_in_reg[31]_1 [4]),
        .I5(\slv_reg_in[5]_i_3_n_0 ),
        .O(\slv_reg_in[4]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFF888F888F888)) 
    \slv_reg_in[5]_i_1 
       (.I0(\slv_reg_in[5]_i_2_n_0 ),
        .I1(\trace_control_reg_n_0_[5] ),
        .I2(\slv_reg_in[31]_i_4_n_0 ),
        .I3(\sample_time_diff_reg_reg_n_0_[5] ),
        .I4(\slv_reg_in_reg[31]_1 [5]),
        .I5(\slv_reg_in[5]_i_3_n_0 ),
        .O(\slv_reg_in[5]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair47" *) 
  LUT5 #(
    .INIT(32'h00010000)) 
    \slv_reg_in[5]_i_2 
       (.I0(register_select[0]),
        .I1(register_select[4]),
        .I2(register_select[5]),
        .I3(\register_select_reg[1]_0 ),
        .I4(register_select[3]),
        .O(\slv_reg_in[5]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair47" *) 
  LUT5 #(
    .INIT(32'h00000002)) 
    \slv_reg_in[5]_i_3 
       (.I0(register_select[5]),
        .I1(\register_select_reg[1]_0 ),
        .I2(register_select[0]),
        .I3(register_select[4]),
        .I4(register_select[3]),
        .O(\slv_reg_in[5]_i_3_n_0 ));
  LUT4 #(
    .INIT(16'hF888)) 
    \slv_reg_in[6]_i_1 
       (.I0(\slv_reg_in_reg[31]_1 [6]),
        .I1(\slv_reg_in[31]_i_5_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[6] ),
        .I3(\slv_reg_in[31]_i_4_n_0 ),
        .O(\slv_reg_in[6]_i_1_n_0 ));
  LUT4 #(
    .INIT(16'hF888)) 
    \slv_reg_in[7]_i_1 
       (.I0(\slv_reg_in_reg[31]_1 [7]),
        .I1(\slv_reg_in[31]_i_5_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[7] ),
        .I3(\slv_reg_in[31]_i_4_n_0 ),
        .O(\slv_reg_in[7]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hFFEAEAEA)) 
    \slv_reg_in[8]_i_1 
       (.I0(\slv_reg_in[31]_i_3_n_0 ),
        .I1(\slv_reg_in[31]_i_4_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[8] ),
        .I3(\slv_reg_in_reg[31]_1 [8]),
        .I4(\slv_reg_in[31]_i_5_n_0 ),
        .O(\slv_reg_in[8]_i_1_n_0 ));
  LUT4 #(
    .INIT(16'hF888)) 
    \slv_reg_in[9]_i_1 
       (.I0(\slv_reg_in_reg[31]_1 [9]),
        .I1(\slv_reg_in[31]_i_5_n_0 ),
        .I2(\sample_time_diff_reg_reg_n_0_[9] ),
        .I3(\slv_reg_in[31]_i_4_n_0 ),
        .O(\slv_reg_in[9]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[0] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[0]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [0]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[10] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[10]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [10]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[11] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[11]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [11]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[12] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[12]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [12]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[13] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[13]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [13]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[14] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[14]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [14]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[15] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[15]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [15]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[16] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[16]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [16]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[17] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[17]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [17]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[18] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[18]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [18]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[19] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[19]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [19]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[1] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[1]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [1]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[20] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[20]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [20]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[21] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[21]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [21]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[22] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[22]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [22]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[23] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[23]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [23]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[24] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[24]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [24]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[25] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[25]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [25]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[26] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[26]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [26]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[27] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[27]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [27]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[28] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[28]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [28]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[29] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[29]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [29]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[2] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[2]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [2]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[30] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[30]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [30]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[31] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[31]_i_2_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [31]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[3] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[3]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [3]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[4] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[4]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [4]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[5] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[5]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [5]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[6] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[6]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [6]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[7] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[7]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [7]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[8] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[8]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [8]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  FDRE \slv_reg_in_reg[9] 
       (.C(mon_clk),
        .CE(1'b1),
        .D(\slv_reg_in[9]_i_1_n_0 ),
        .Q(\slv_reg_in_reg[31]_0 [9]),
        .R(\slv_reg_in[31]_i_1_n_0 ));
  LUT2 #(
    .INIT(4'h2)) 
    slv_reg_in_vld_i_1
       (.I0(slv_reg_addr_vld_reg),
        .I1(slv_reg_out_vld_reg),
        .O(slv_reg_in1));
  FDRE slv_reg_in_vld_reg
       (.C(mon_clk),
        .CE(1'b1),
        .D(slv_reg_in1),
        .Q(slv_reg_in_vld_reg_0),
        .R(SS));
  FDRE slv_reg_out_vld_reg_reg
       (.C(mon_clk),
        .CE(1'b1),
        .D(slv_reg_out_vld),
        .Q(slv_reg_out_vld_reg),
        .R(SS));
  FDSE \trace_control_reg[0] 
       (.C(mon_clk),
        .CE(\trace_control_reg[5]_0 ),
        .D(s_axi_wdata[0]),
        .Q(\trace_control_reg[0]_0 ),
        .S(SS));
  FDSE \trace_control_reg[1] 
       (.C(mon_clk),
        .CE(\trace_control_reg[5]_0 ),
        .D(s_axi_wdata[1]),
        .Q(trace_control[1]),
        .S(SS));
  FDSE \trace_control_reg[2] 
       (.C(mon_clk),
        .CE(\trace_control_reg[5]_0 ),
        .D(s_axi_wdata[2]),
        .Q(trace_control[2]),
        .S(SS));
  FDSE \trace_control_reg[3] 
       (.C(mon_clk),
        .CE(\trace_control_reg[5]_0 ),
        .D(s_axi_wdata[3]),
        .Q(trace_control[3]),
        .S(SS));
  FDSE \trace_control_reg[4] 
       (.C(mon_clk),
        .CE(\trace_control_reg[5]_0 ),
        .D(s_axi_wdata[4]),
        .Q(\trace_control_reg_n_0_[4] ),
        .S(SS));
  FDSE \trace_control_reg[5] 
       (.C(mon_clk),
        .CE(\trace_control_reg[5]_0 ),
        .D(s_axi_wdata[5]),
        .Q(\trace_control_reg_n_0_[5] ),
        .S(SS));
  LUT2 #(
    .INIT(4'hB)) 
    xpm_fifo_async_inst_i_1__0
       (.I0(Metrics_Cnt_Reset),
        .I1(mon_resetn),
        .O(RST_ACTIVE));
endmodule

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_timestamper
   (trace_data,
    trace_valid,
    trace_clk,
    trace_read,
    trace_counter_overflow,
    dest_out,
    trace_counter,
    \event_i_buf_reg[52]_0 ,
    trace_rst);
  output [55:0]trace_data;
  output trace_valid;
  input trace_clk;
  input trace_read;
  input trace_counter_overflow;
  input [3:0]dest_out;
  input [44:0]trace_counter;
  input [3:0]\event_i_buf_reg[52]_0 ;
  input trace_rst;

  wire [3:0]dest_out;
  wire error;
  wire error_write;
  wire [46:46]event_i;
  wire [54:0]event_i_buf;
  wire \event_i_buf[49]_i_1_n_0 ;
  wire \event_i_buf[50]_i_1_n_0 ;
  wire \event_i_buf[51]_i_1_n_0 ;
  wire \event_i_buf[52]_i_1_n_0 ;
  wire \event_i_buf[53]_i_1_n_0 ;
  wire \event_i_buf[53]_i_2_n_0 ;
  wire [3:0]\event_i_buf_reg[52]_0 ;
  wire event_valid_i;
  wire fifo_i_n_57;
  wire fifo_i_n_58;
  wire fifo_i_n_60;
  wire p_1_in;
  wire trace_clk;
  wire [44:0]trace_counter;
  wire trace_counter_overflow;
  wire [55:0]trace_data;
  wire trace_read;
  wire trace_rst;
  wire trace_valid;
  wire wr_rst_busy;
  wire write__0;

  FDRE #(
    .INIT(1'b0)) 
    error_reg
       (.C(trace_clk),
        .CE(1'b1),
        .D(fifo_i_n_60),
        .Q(error),
        .R(fifo_i_n_57));
  FDSE error_write_reg
       (.C(trace_clk),
        .CE(1'b1),
        .D(fifo_i_n_58),
        .Q(error_write),
        .S(fifo_i_n_57));
  (* SOFT_HLUTNM = "soft_lutpair73" *) 
  LUT4 #(
    .INIT(16'h0001)) 
    \event_i_buf[46]_i_1 
       (.I0(dest_out[2]),
        .I1(dest_out[0]),
        .I2(dest_out[1]),
        .I3(dest_out[3]),
        .O(event_i));
  LUT2 #(
    .INIT(4'hE)) 
    \event_i_buf[49]_i_1 
       (.I0(\event_i_buf_reg[52]_0 [0]),
        .I1(dest_out[0]),
        .O(\event_i_buf[49]_i_1_n_0 ));
  LUT2 #(
    .INIT(4'hE)) 
    \event_i_buf[50]_i_1 
       (.I0(\event_i_buf_reg[52]_0 [1]),
        .I1(dest_out[1]),
        .O(\event_i_buf[50]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair73" *) 
  LUT2 #(
    .INIT(4'hE)) 
    \event_i_buf[51]_i_1 
       (.I0(\event_i_buf_reg[52]_0 [2]),
        .I1(dest_out[2]),
        .O(\event_i_buf[51]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair72" *) 
  LUT2 #(
    .INIT(4'hE)) 
    \event_i_buf[52]_i_1 
       (.I0(\event_i_buf_reg[52]_0 [3]),
        .I1(dest_out[3]),
        .O(\event_i_buf[52]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair71" *) 
  LUT5 #(
    .INIT(32'hFF00FE00)) 
    \event_i_buf[53]_i_1 
       (.I0(dest_out[3]),
        .I1(dest_out[2]),
        .I2(dest_out[1]),
        .I3(\event_i_buf[53]_i_2_n_0 ),
        .I4(dest_out[0]),
        .O(\event_i_buf[53]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair72" *) 
  LUT4 #(
    .INIT(16'hFFFE)) 
    \event_i_buf[53]_i_2 
       (.I0(\event_i_buf_reg[52]_0 [3]),
        .I1(\event_i_buf_reg[52]_0 [1]),
        .I2(\event_i_buf_reg[52]_0 [0]),
        .I3(\event_i_buf_reg[52]_0 [2]),
        .O(\event_i_buf[53]_i_2_n_0 ));
  FDRE \event_i_buf_reg[0] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[0]),
        .Q(event_i_buf[0]),
        .R(1'b0));
  FDRE \event_i_buf_reg[10] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[10]),
        .Q(event_i_buf[10]),
        .R(1'b0));
  FDRE \event_i_buf_reg[11] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[11]),
        .Q(event_i_buf[11]),
        .R(1'b0));
  FDRE \event_i_buf_reg[12] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[12]),
        .Q(event_i_buf[12]),
        .R(1'b0));
  FDRE \event_i_buf_reg[13] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[13]),
        .Q(event_i_buf[13]),
        .R(1'b0));
  FDRE \event_i_buf_reg[14] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[14]),
        .Q(event_i_buf[14]),
        .R(1'b0));
  FDRE \event_i_buf_reg[15] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[15]),
        .Q(event_i_buf[15]),
        .R(1'b0));
  FDRE \event_i_buf_reg[16] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[16]),
        .Q(event_i_buf[16]),
        .R(1'b0));
  FDRE \event_i_buf_reg[17] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[17]),
        .Q(event_i_buf[17]),
        .R(1'b0));
  FDRE \event_i_buf_reg[18] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[18]),
        .Q(event_i_buf[18]),
        .R(1'b0));
  FDRE \event_i_buf_reg[19] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[19]),
        .Q(event_i_buf[19]),
        .R(1'b0));
  FDRE \event_i_buf_reg[1] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[1]),
        .Q(event_i_buf[1]),
        .R(1'b0));
  FDRE \event_i_buf_reg[20] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[20]),
        .Q(event_i_buf[20]),
        .R(1'b0));
  FDRE \event_i_buf_reg[21] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[21]),
        .Q(event_i_buf[21]),
        .R(1'b0));
  FDRE \event_i_buf_reg[22] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[22]),
        .Q(event_i_buf[22]),
        .R(1'b0));
  FDRE \event_i_buf_reg[23] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[23]),
        .Q(event_i_buf[23]),
        .R(1'b0));
  FDRE \event_i_buf_reg[24] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[24]),
        .Q(event_i_buf[24]),
        .R(1'b0));
  FDRE \event_i_buf_reg[25] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[25]),
        .Q(event_i_buf[25]),
        .R(1'b0));
  FDRE \event_i_buf_reg[26] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[26]),
        .Q(event_i_buf[26]),
        .R(1'b0));
  FDRE \event_i_buf_reg[27] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[27]),
        .Q(event_i_buf[27]),
        .R(1'b0));
  FDRE \event_i_buf_reg[28] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[28]),
        .Q(event_i_buf[28]),
        .R(1'b0));
  FDRE \event_i_buf_reg[29] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[29]),
        .Q(event_i_buf[29]),
        .R(1'b0));
  FDRE \event_i_buf_reg[2] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[2]),
        .Q(event_i_buf[2]),
        .R(1'b0));
  FDRE \event_i_buf_reg[30] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[30]),
        .Q(event_i_buf[30]),
        .R(1'b0));
  FDRE \event_i_buf_reg[31] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[31]),
        .Q(event_i_buf[31]),
        .R(1'b0));
  FDRE \event_i_buf_reg[32] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[32]),
        .Q(event_i_buf[32]),
        .R(1'b0));
  FDRE \event_i_buf_reg[33] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[33]),
        .Q(event_i_buf[33]),
        .R(1'b0));
  FDRE \event_i_buf_reg[34] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[34]),
        .Q(event_i_buf[34]),
        .R(1'b0));
  FDRE \event_i_buf_reg[35] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[35]),
        .Q(event_i_buf[35]),
        .R(1'b0));
  FDRE \event_i_buf_reg[36] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[36]),
        .Q(event_i_buf[36]),
        .R(1'b0));
  FDRE \event_i_buf_reg[37] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[37]),
        .Q(event_i_buf[37]),
        .R(1'b0));
  FDRE \event_i_buf_reg[38] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[38]),
        .Q(event_i_buf[38]),
        .R(1'b0));
  FDRE \event_i_buf_reg[39] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[39]),
        .Q(event_i_buf[39]),
        .R(1'b0));
  FDRE \event_i_buf_reg[3] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[3]),
        .Q(event_i_buf[3]),
        .R(1'b0));
  FDRE \event_i_buf_reg[40] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[40]),
        .Q(event_i_buf[40]),
        .R(1'b0));
  FDRE \event_i_buf_reg[41] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[41]),
        .Q(event_i_buf[41]),
        .R(1'b0));
  FDRE \event_i_buf_reg[42] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[42]),
        .Q(event_i_buf[42]),
        .R(1'b0));
  FDRE \event_i_buf_reg[43] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[43]),
        .Q(event_i_buf[43]),
        .R(1'b0));
  FDRE \event_i_buf_reg[44] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[44]),
        .Q(event_i_buf[44]),
        .R(1'b0));
  FDRE \event_i_buf_reg[45] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(dest_out[0]),
        .Q(event_i_buf[45]),
        .R(1'b0));
  FDRE \event_i_buf_reg[46] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(event_i),
        .Q(event_i_buf[46]),
        .R(1'b0));
  FDRE \event_i_buf_reg[49] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(\event_i_buf[49]_i_1_n_0 ),
        .Q(event_i_buf[49]),
        .R(1'b0));
  FDRE \event_i_buf_reg[4] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[4]),
        .Q(event_i_buf[4]),
        .R(1'b0));
  FDRE \event_i_buf_reg[50] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(\event_i_buf[50]_i_1_n_0 ),
        .Q(event_i_buf[50]),
        .R(1'b0));
  FDRE \event_i_buf_reg[51] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(\event_i_buf[51]_i_1_n_0 ),
        .Q(event_i_buf[51]),
        .R(1'b0));
  FDRE \event_i_buf_reg[52] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(\event_i_buf[52]_i_1_n_0 ),
        .Q(event_i_buf[52]),
        .R(1'b0));
  FDRE \event_i_buf_reg[53] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(\event_i_buf[53]_i_1_n_0 ),
        .Q(event_i_buf[53]),
        .R(1'b0));
  FDRE \event_i_buf_reg[54] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter_overflow),
        .Q(event_i_buf[54]),
        .R(1'b0));
  FDRE \event_i_buf_reg[5] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[5]),
        .Q(event_i_buf[5]),
        .R(1'b0));
  FDRE \event_i_buf_reg[6] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[6]),
        .Q(event_i_buf[6]),
        .R(1'b0));
  FDRE \event_i_buf_reg[7] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[7]),
        .Q(event_i_buf[7]),
        .R(1'b0));
  FDRE \event_i_buf_reg[8] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[8]),
        .Q(event_i_buf[8]),
        .R(1'b0));
  FDRE \event_i_buf_reg[9] 
       (.C(trace_clk),
        .CE(1'b1),
        .D(trace_counter[9]),
        .Q(event_i_buf[9]),
        .R(1'b0));
  (* SOFT_HLUTNM = "soft_lutpair71" *) 
  LUT5 #(
    .INIT(32'hFFFFFFFE)) 
    event_valid_i_i_1
       (.I0(dest_out[2]),
        .I1(dest_out[0]),
        .I2(\event_i_buf[53]_i_2_n_0 ),
        .I3(dest_out[1]),
        .I4(dest_out[3]),
        .O(p_1_in));
  FDRE #(
    .INIT(1'b0)) 
    event_valid_i_reg
       (.C(trace_clk),
        .CE(1'b1),
        .D(p_1_in),
        .Q(event_valid_i),
        .R(1'b0));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_trace_fifo_i__parameterized0 fifo_i
       (.din({event_i_buf[54:49],event_i_buf[46:0]}),
        .error(error),
        .error_write(error_write),
        .error_write_reg(fifo_i_n_58),
        .event_valid_i(event_valid_i),
        .\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg (fifo_i_n_60),
        .rst(fifo_i_n_57),
        .trace_clk(trace_clk),
        .trace_data(trace_data),
        .trace_read(trace_read),
        .trace_rst(trace_rst),
        .trace_valid(trace_valid),
        .wr_en(write__0),
        .wr_rst_busy(wr_rst_busy));
  LUT4 #(
    .INIT(16'h00F8)) 
    write
       (.I0(error_write),
        .I1(error),
        .I2(event_valid_i),
        .I3(wr_rst_busy),
        .O(write__0));
endmodule

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_trace_fifo_i
   (empty,
    S,
    \counter_reg[55] ,
    \counter_reg[47] ,
    \counter_reg[39] ,
    \counter_reg[31] ,
    \counter_reg[23] ,
    \counter_reg[15] ,
    \counter_reg[7] ,
    E,
    \gen_fwft.empty_fwft_i_reg ,
    RST_ACTIVE,
    mon_clk,
    din,
    rd_en,
    CO,
    ap_done_reg,
    dataflow_en,
    ap_continue_reg,
    \min_ctr_reg[0] ,
    start_pulse);
  output empty;
  output [7:0]S;
  output [7:0]\counter_reg[55] ;
  output [7:0]\counter_reg[47] ;
  output [7:0]\counter_reg[39] ;
  output [7:0]\counter_reg[31] ;
  output [7:0]\counter_reg[23] ;
  output [7:0]\counter_reg[15] ;
  output [7:0]\counter_reg[7] ;
  output [0:0]E;
  output [0:0]\gen_fwft.empty_fwft_i_reg ;
  input RST_ACTIVE;
  input mon_clk;
  input [63:0]din;
  input rd_en;
  input [0:0]CO;
  input ap_done_reg;
  input dataflow_en;
  input ap_continue_reg;
  input [0:0]\min_ctr_reg[0] ;
  input start_pulse;

  wire [0:0]CO;
  wire [0:0]E;
  wire RST_ACTIVE;
  wire [7:0]S;
  wire ap_continue_reg;
  wire ap_done_reg;
  wire [7:0]\counter_reg[15] ;
  wire [7:0]\counter_reg[23] ;
  wire [7:0]\counter_reg[31] ;
  wire [7:0]\counter_reg[39] ;
  wire [7:0]\counter_reg[47] ;
  wire [7:0]\counter_reg[55] ;
  wire [7:0]\counter_reg[7] ;
  wire dataflow_en;
  wire [63:0]din;
  wire empty;
  wire [63:0]fifo_out;
  wire full_i;
  wire [0:0]\gen_fwft.empty_fwft_i_reg ;
  wire [0:0]\min_ctr_reg[0] ;
  wire mon_clk;
  wire rd_en;
  wire start_pulse;
  wire wr_rst_busy;
  wire write;
  wire NLW_xpm_fifo_async_inst_almost_empty_UNCONNECTED;
  wire NLW_xpm_fifo_async_inst_almost_full_UNCONNECTED;
  wire NLW_xpm_fifo_async_inst_data_valid_UNCONNECTED;
  wire NLW_xpm_fifo_async_inst_dbiterr_UNCONNECTED;
  wire NLW_xpm_fifo_async_inst_overflow_UNCONNECTED;
  wire NLW_xpm_fifo_async_inst_prog_empty_UNCONNECTED;
  wire NLW_xpm_fifo_async_inst_prog_full_UNCONNECTED;
  wire NLW_xpm_fifo_async_inst_rd_rst_busy_UNCONNECTED;
  wire NLW_xpm_fifo_async_inst_sbiterr_UNCONNECTED;
  wire NLW_xpm_fifo_async_inst_underflow_UNCONNECTED;
  wire NLW_xpm_fifo_async_inst_wr_ack_UNCONNECTED;
  wire [3:0]NLW_xpm_fifo_async_inst_rd_data_count_UNCONNECTED;
  wire [3:0]NLW_xpm_fifo_async_inst_wr_data_count_UNCONNECTED;

  LUT5 #(
    .INIT(32'h44400040)) 
    \max_ctr[63]_i_1 
       (.I0(empty),
        .I1(CO),
        .I2(ap_done_reg),
        .I3(dataflow_en),
        .I4(ap_continue_reg),
        .O(E));
  LUT5 #(
    .INIT(32'h44400040)) 
    \min_ctr[63]_i_1 
       (.I0(empty),
        .I1(\min_ctr_reg[0] ),
        .I2(ap_done_reg),
        .I3(dataflow_en),
        .I4(ap_continue_reg),
        .O(\gen_fwft.empty_fwft_i_reg ));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__0_i_1
       (.I0(din[15]),
        .I1(fifo_out[15]),
        .O(\counter_reg[15] [7]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__0_i_2
       (.I0(din[14]),
        .I1(fifo_out[14]),
        .O(\counter_reg[15] [6]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__0_i_3
       (.I0(din[13]),
        .I1(fifo_out[13]),
        .O(\counter_reg[15] [5]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__0_i_4
       (.I0(din[12]),
        .I1(fifo_out[12]),
        .O(\counter_reg[15] [4]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__0_i_5
       (.I0(din[11]),
        .I1(fifo_out[11]),
        .O(\counter_reg[15] [3]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__0_i_6
       (.I0(din[10]),
        .I1(fifo_out[10]),
        .O(\counter_reg[15] [2]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__0_i_7
       (.I0(din[9]),
        .I1(fifo_out[9]),
        .O(\counter_reg[15] [1]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__0_i_8
       (.I0(din[8]),
        .I1(fifo_out[8]),
        .O(\counter_reg[15] [0]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__1_i_1
       (.I0(din[23]),
        .I1(fifo_out[23]),
        .O(\counter_reg[23] [7]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__1_i_2
       (.I0(din[22]),
        .I1(fifo_out[22]),
        .O(\counter_reg[23] [6]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__1_i_3
       (.I0(din[21]),
        .I1(fifo_out[21]),
        .O(\counter_reg[23] [5]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__1_i_4
       (.I0(din[20]),
        .I1(fifo_out[20]),
        .O(\counter_reg[23] [4]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__1_i_5
       (.I0(din[19]),
        .I1(fifo_out[19]),
        .O(\counter_reg[23] [3]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__1_i_6
       (.I0(din[18]),
        .I1(fifo_out[18]),
        .O(\counter_reg[23] [2]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__1_i_7
       (.I0(din[17]),
        .I1(fifo_out[17]),
        .O(\counter_reg[23] [1]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__1_i_8
       (.I0(din[16]),
        .I1(fifo_out[16]),
        .O(\counter_reg[23] [0]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__2_i_1
       (.I0(din[31]),
        .I1(fifo_out[31]),
        .O(\counter_reg[31] [7]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__2_i_2
       (.I0(din[30]),
        .I1(fifo_out[30]),
        .O(\counter_reg[31] [6]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__2_i_3
       (.I0(din[29]),
        .I1(fifo_out[29]),
        .O(\counter_reg[31] [5]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__2_i_4
       (.I0(din[28]),
        .I1(fifo_out[28]),
        .O(\counter_reg[31] [4]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__2_i_5
       (.I0(din[27]),
        .I1(fifo_out[27]),
        .O(\counter_reg[31] [3]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__2_i_6
       (.I0(din[26]),
        .I1(fifo_out[26]),
        .O(\counter_reg[31] [2]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__2_i_7
       (.I0(din[25]),
        .I1(fifo_out[25]),
        .O(\counter_reg[31] [1]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__2_i_8
       (.I0(din[24]),
        .I1(fifo_out[24]),
        .O(\counter_reg[31] [0]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__3_i_1
       (.I0(din[39]),
        .I1(fifo_out[39]),
        .O(\counter_reg[39] [7]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__3_i_2
       (.I0(din[38]),
        .I1(fifo_out[38]),
        .O(\counter_reg[39] [6]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__3_i_3
       (.I0(din[37]),
        .I1(fifo_out[37]),
        .O(\counter_reg[39] [5]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__3_i_4
       (.I0(din[36]),
        .I1(fifo_out[36]),
        .O(\counter_reg[39] [4]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__3_i_5
       (.I0(din[35]),
        .I1(fifo_out[35]),
        .O(\counter_reg[39] [3]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__3_i_6
       (.I0(din[34]),
        .I1(fifo_out[34]),
        .O(\counter_reg[39] [2]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__3_i_7
       (.I0(din[33]),
        .I1(fifo_out[33]),
        .O(\counter_reg[39] [1]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__3_i_8
       (.I0(din[32]),
        .I1(fifo_out[32]),
        .O(\counter_reg[39] [0]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__4_i_1
       (.I0(din[47]),
        .I1(fifo_out[47]),
        .O(\counter_reg[47] [7]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__4_i_2
       (.I0(din[46]),
        .I1(fifo_out[46]),
        .O(\counter_reg[47] [6]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__4_i_3
       (.I0(din[45]),
        .I1(fifo_out[45]),
        .O(\counter_reg[47] [5]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__4_i_4
       (.I0(din[44]),
        .I1(fifo_out[44]),
        .O(\counter_reg[47] [4]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__4_i_5
       (.I0(din[43]),
        .I1(fifo_out[43]),
        .O(\counter_reg[47] [3]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__4_i_6
       (.I0(din[42]),
        .I1(fifo_out[42]),
        .O(\counter_reg[47] [2]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__4_i_7
       (.I0(din[41]),
        .I1(fifo_out[41]),
        .O(\counter_reg[47] [1]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__4_i_8
       (.I0(din[40]),
        .I1(fifo_out[40]),
        .O(\counter_reg[47] [0]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__5_i_1
       (.I0(din[55]),
        .I1(fifo_out[55]),
        .O(\counter_reg[55] [7]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__5_i_2
       (.I0(din[54]),
        .I1(fifo_out[54]),
        .O(\counter_reg[55] [6]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__5_i_3
       (.I0(din[53]),
        .I1(fifo_out[53]),
        .O(\counter_reg[55] [5]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__5_i_4
       (.I0(din[52]),
        .I1(fifo_out[52]),
        .O(\counter_reg[55] [4]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__5_i_5
       (.I0(din[51]),
        .I1(fifo_out[51]),
        .O(\counter_reg[55] [3]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__5_i_6
       (.I0(din[50]),
        .I1(fifo_out[50]),
        .O(\counter_reg[55] [2]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__5_i_7
       (.I0(din[49]),
        .I1(fifo_out[49]),
        .O(\counter_reg[55] [1]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__5_i_8
       (.I0(din[48]),
        .I1(fifo_out[48]),
        .O(\counter_reg[55] [0]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__6_i_1
       (.I0(din[63]),
        .I1(fifo_out[63]),
        .O(S[7]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__6_i_2
       (.I0(din[62]),
        .I1(fifo_out[62]),
        .O(S[6]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__6_i_3
       (.I0(din[61]),
        .I1(fifo_out[61]),
        .O(S[5]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__6_i_4
       (.I0(din[60]),
        .I1(fifo_out[60]),
        .O(S[4]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__6_i_5
       (.I0(din[59]),
        .I1(fifo_out[59]),
        .O(S[3]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__6_i_6
       (.I0(din[58]),
        .I1(fifo_out[58]),
        .O(S[2]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__6_i_7
       (.I0(din[57]),
        .I1(fifo_out[57]),
        .O(S[1]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry__6_i_8
       (.I0(din[56]),
        .I1(fifo_out[56]),
        .O(S[0]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry_i_1
       (.I0(din[7]),
        .I1(fifo_out[7]),
        .O(\counter_reg[7] [7]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry_i_2
       (.I0(din[6]),
        .I1(fifo_out[6]),
        .O(\counter_reg[7] [6]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry_i_3
       (.I0(din[5]),
        .I1(fifo_out[5]),
        .O(\counter_reg[7] [5]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry_i_4
       (.I0(din[4]),
        .I1(fifo_out[4]),
        .O(\counter_reg[7] [4]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry_i_5
       (.I0(din[3]),
        .I1(fifo_out[3]),
        .O(\counter_reg[7] [3]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry_i_6
       (.I0(din[2]),
        .I1(fifo_out[2]),
        .O(\counter_reg[7] [2]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry_i_7
       (.I0(din[1]),
        .I1(fifo_out[1]),
        .O(\counter_reg[7] [1]));
  LUT2 #(
    .INIT(4'h9)) 
    tx_length_carry_i_8
       (.I0(din[0]),
        .I1(fifo_out[0]),
        .O(\counter_reg[7] [0]));
  (* CASCADE_HEIGHT = "0" *) 
  (* CDC_SYNC_STAGES = "3" *) 
  (* DOUT_RESET_VALUE = "0" *) 
  (* ECC_MODE = "no_ecc" *) 
  (* EN_ADV_FEATURE_ASYNC = "16'b0000011100000111" *) 
  (* FIFO_MEMORY_TYPE = "distributed" *) 
  (* FIFO_READ_LATENCY = "1" *) 
  (* FIFO_WRITE_DEPTH = "16" *) 
  (* FULL_RESET_VALUE = "0" *) 
  (* PROG_EMPTY_THRESH = "10" *) 
  (* PROG_FULL_THRESH = "10" *) 
  (* P_COMMON_CLOCK = "0" *) 
  (* P_ECC_MODE = "0" *) 
  (* P_FIFO_MEMORY_TYPE = "1" *) 
  (* P_READ_MODE = "1" *) 
  (* P_WAKEUP_TIME = "2" *) 
  (* RD_DATA_COUNT_WIDTH = "4" *) 
  (* READ_DATA_WIDTH = "64" *) 
  (* READ_MODE = "fwft" *) 
  (* RELATED_CLOCKS = "0" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* USE_ADV_FEATURES = "0707" *) 
  (* WAKEUP_TIME = "0" *) 
  (* WRITE_DATA_WIDTH = "64" *) 
  (* WR_DATA_COUNT_WIDTH = "4" *) 
  (* XPM_MODULE = "TRUE" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_async xpm_fifo_async_inst
       (.almost_empty(NLW_xpm_fifo_async_inst_almost_empty_UNCONNECTED),
        .almost_full(NLW_xpm_fifo_async_inst_almost_full_UNCONNECTED),
        .data_valid(NLW_xpm_fifo_async_inst_data_valid_UNCONNECTED),
        .dbiterr(NLW_xpm_fifo_async_inst_dbiterr_UNCONNECTED),
        .din(din),
        .dout(fifo_out),
        .empty(empty),
        .full(full_i),
        .injectdbiterr(1'b0),
        .injectsbiterr(1'b0),
        .overflow(NLW_xpm_fifo_async_inst_overflow_UNCONNECTED),
        .prog_empty(NLW_xpm_fifo_async_inst_prog_empty_UNCONNECTED),
        .prog_full(NLW_xpm_fifo_async_inst_prog_full_UNCONNECTED),
        .rd_clk(mon_clk),
        .rd_data_count(NLW_xpm_fifo_async_inst_rd_data_count_UNCONNECTED[3:0]),
        .rd_en(rd_en),
        .rd_rst_busy(NLW_xpm_fifo_async_inst_rd_rst_busy_UNCONNECTED),
        .rst(RST_ACTIVE),
        .sbiterr(NLW_xpm_fifo_async_inst_sbiterr_UNCONNECTED),
        .sleep(1'b0),
        .underflow(NLW_xpm_fifo_async_inst_underflow_UNCONNECTED),
        .wr_ack(NLW_xpm_fifo_async_inst_wr_ack_UNCONNECTED),
        .wr_clk(mon_clk),
        .wr_data_count(NLW_xpm_fifo_async_inst_wr_data_count_UNCONNECTED[3:0]),
        .wr_en(write),
        .wr_rst_busy(wr_rst_busy));
  LUT3 #(
    .INIT(8'h04)) 
    xpm_fifo_async_inst_i_2
       (.I0(wr_rst_busy),
        .I1(start_pulse),
        .I2(full_i),
        .O(write));
endmodule

(* ORIG_REF_NAME = "trace_fifo_i" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_trace_fifo_i__parameterized0
   (wr_rst_busy,
    trace_data,
    rst,
    error_write_reg,
    trace_valid,
    \gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg ,
    trace_clk,
    wr_en,
    din,
    trace_read,
    error_write,
    error,
    trace_rst,
    event_valid_i);
  output wr_rst_busy;
  output [55:0]trace_data;
  output rst;
  output error_write_reg;
  output trace_valid;
  output \gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg ;
  input trace_clk;
  input wr_en;
  input [52:0]din;
  input trace_read;
  input error_write;
  input error;
  input trace_rst;
  input event_valid_i;

  wire [52:0]din;
  wire empty;
  wire error;
  wire error_write;
  wire error_write_reg;
  wire event_valid_i;
  wire full_i;
  wire \gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg ;
  wire rst;
  wire trace_clk;
  wire [55:0]trace_data;
  wire trace_read;
  wire trace_rst;
  wire trace_valid;
  wire wr_en;
  wire wr_rst_busy;
  wire NLW_xpm_fifo_async_inst_almost_empty_UNCONNECTED;
  wire NLW_xpm_fifo_async_inst_almost_full_UNCONNECTED;
  wire NLW_xpm_fifo_async_inst_data_valid_UNCONNECTED;
  wire NLW_xpm_fifo_async_inst_dbiterr_UNCONNECTED;
  wire NLW_xpm_fifo_async_inst_overflow_UNCONNECTED;
  wire NLW_xpm_fifo_async_inst_prog_empty_UNCONNECTED;
  wire NLW_xpm_fifo_async_inst_prog_full_UNCONNECTED;
  wire NLW_xpm_fifo_async_inst_rd_rst_busy_UNCONNECTED;
  wire NLW_xpm_fifo_async_inst_sbiterr_UNCONNECTED;
  wire NLW_xpm_fifo_async_inst_underflow_UNCONNECTED;
  wire NLW_xpm_fifo_async_inst_wr_ack_UNCONNECTED;
  wire [3:0]NLW_xpm_fifo_async_inst_rd_data_count_UNCONNECTED;
  wire [3:0]NLW_xpm_fifo_async_inst_wr_data_count_UNCONNECTED;

  (* SOFT_HLUTNM = "soft_lutpair70" *) 
  LUT3 #(
    .INIT(8'hF8)) 
    error_i_1
       (.I0(full_i),
        .I1(event_valid_i),
        .I2(error),
        .O(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg ));
  (* SOFT_HLUTNM = "soft_lutpair70" *) 
  LUT3 #(
    .INIT(8'h8A)) 
    error_write_i_1
       (.I0(error_write),
        .I1(full_i),
        .I2(error),
        .O(error_write_reg));
  LUT1 #(
    .INIT(2'h1)) 
    trace_valid_INST_0
       (.I0(empty),
        .O(trace_valid));
  (* CASCADE_HEIGHT = "0" *) 
  (* CDC_SYNC_STAGES = "3" *) 
  (* DOUT_RESET_VALUE = "0" *) 
  (* ECC_MODE = "no_ecc" *) 
  (* EN_ADV_FEATURE_ASYNC = "16'b0000011100000111" *) 
  (* FIFO_MEMORY_TYPE = "distributed" *) 
  (* FIFO_READ_LATENCY = "1" *) 
  (* FIFO_WRITE_DEPTH = "16" *) 
  (* FULL_RESET_VALUE = "0" *) 
  (* PROG_EMPTY_THRESH = "10" *) 
  (* PROG_FULL_THRESH = "10" *) 
  (* P_COMMON_CLOCK = "0" *) 
  (* P_ECC_MODE = "0" *) 
  (* P_FIFO_MEMORY_TYPE = "1" *) 
  (* P_READ_MODE = "1" *) 
  (* P_WAKEUP_TIME = "2" *) 
  (* RD_DATA_COUNT_WIDTH = "4" *) 
  (* READ_DATA_WIDTH = "56" *) 
  (* READ_MODE = "fwft" *) 
  (* RELATED_CLOCKS = "0" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* USE_ADV_FEATURES = "0707" *) 
  (* WAKEUP_TIME = "0" *) 
  (* WRITE_DATA_WIDTH = "56" *) 
  (* WR_DATA_COUNT_WIDTH = "4" *) 
  (* XPM_MODULE = "TRUE" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_async__parameterized0 xpm_fifo_async_inst
       (.almost_empty(NLW_xpm_fifo_async_inst_almost_empty_UNCONNECTED),
        .almost_full(NLW_xpm_fifo_async_inst_almost_full_UNCONNECTED),
        .data_valid(NLW_xpm_fifo_async_inst_data_valid_UNCONNECTED),
        .dbiterr(NLW_xpm_fifo_async_inst_dbiterr_UNCONNECTED),
        .din({1'b0,din[52:47],1'b0,1'b0,din[46:0]}),
        .dout(trace_data),
        .empty(empty),
        .full(full_i),
        .injectdbiterr(1'b0),
        .injectsbiterr(1'b0),
        .overflow(NLW_xpm_fifo_async_inst_overflow_UNCONNECTED),
        .prog_empty(NLW_xpm_fifo_async_inst_prog_empty_UNCONNECTED),
        .prog_full(NLW_xpm_fifo_async_inst_prog_full_UNCONNECTED),
        .rd_clk(trace_clk),
        .rd_data_count(NLW_xpm_fifo_async_inst_rd_data_count_UNCONNECTED[3:0]),
        .rd_en(trace_read),
        .rd_rst_busy(NLW_xpm_fifo_async_inst_rd_rst_busy_UNCONNECTED),
        .rst(rst),
        .sbiterr(NLW_xpm_fifo_async_inst_sbiterr_UNCONNECTED),
        .sleep(1'b0),
        .underflow(NLW_xpm_fifo_async_inst_underflow_UNCONNECTED),
        .wr_ack(NLW_xpm_fifo_async_inst_wr_ack_UNCONNECTED),
        .wr_clk(trace_clk),
        .wr_data_count(NLW_xpm_fifo_async_inst_wr_data_count_UNCONNECTED[3:0]),
        .wr_en(wr_en),
        .wr_rst_busy(wr_rst_busy));
  LUT1 #(
    .INIT(2'h1)) 
    xpm_fifo_async_inst_i_1
       (.I0(trace_rst),
        .O(rst));
endmodule

(* DEST_SYNC_FF = "4" *) (* INIT_SYNC_FF = "0" *) (* SIM_ASSERT_CHK = "0" *) 
(* SRC_INPUT_REG = "1" *) (* VERSION = "0" *) (* WIDTH = "4" *) 
(* XPM_MODULE = "TRUE" *) (* keep_hierarchy = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single
   (src_clk,
    src_in,
    dest_clk,
    dest_out);
  input src_clk;
  input [3:0]src_in;
  input dest_clk;
  output [3:0]dest_out;

  wire [3:0]async_path_bit;
  wire dest_clk;
  wire src_clk;
  wire [3:0]src_in;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) wire [3:0]\syncstages_ff[0] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) wire [3:0]\syncstages_ff[1] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) wire [3:0]\syncstages_ff[2] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) wire [3:0]\syncstages_ff[3] ;

  assign dest_out[3:0] = \syncstages_ff[3] ;
  LUT1 #(
    .INIT(2'h2)) 
    i_0
       (.I0(1'b0),
        .O(async_path_bit[3]));
  LUT1 #(
    .INIT(2'h2)) 
    i_1
       (.I0(1'b0),
        .O(async_path_bit[2]));
  LUT1 #(
    .INIT(2'h2)) 
    i_2
       (.I0(1'b0),
        .O(async_path_bit[1]));
  FDRE \src_ff_reg[0] 
       (.C(src_clk),
        .CE(1'b1),
        .D(src_in[0]),
        .Q(async_path_bit[0]),
        .R(1'b0));
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
  FDRE \syncstages_ff_reg[0][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[1]),
        .Q(\syncstages_ff[0] [1]),
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
  FDRE \syncstages_ff_reg[0][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[3]),
        .Q(\syncstages_ff[0] [3]),
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
  FDRE \syncstages_ff_reg[1][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [1]),
        .Q(\syncstages_ff[1] [1]),
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
  FDRE \syncstages_ff_reg[1][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [3]),
        .Q(\syncstages_ff[1] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[2][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[1] [0]),
        .Q(\syncstages_ff[2] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[2][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[1] [1]),
        .Q(\syncstages_ff[2] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[2][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[1] [2]),
        .Q(\syncstages_ff[2] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[2][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[1] [3]),
        .Q(\syncstages_ff[2] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[3][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[2] [0]),
        .Q(\syncstages_ff[3] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[3][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[2] [1]),
        .Q(\syncstages_ff[3] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[3][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[2] [2]),
        .Q(\syncstages_ff[3] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[3][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[2] [3]),
        .Q(\syncstages_ff[3] [3]),
        .R(1'b0));
endmodule

(* DEST_SYNC_FF = "4" *) (* INIT_SYNC_FF = "0" *) (* ORIG_REF_NAME = "xpm_cdc_array_single" *) 
(* SIM_ASSERT_CHK = "0" *) (* SRC_INPUT_REG = "1" *) (* VERSION = "0" *) 
(* WIDTH = "4" *) (* XPM_MODULE = "TRUE" *) (* keep_hierarchy = "true" *) 
(* xpm_cdc = "ARRAY_SINGLE" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__2
   (src_clk,
    src_in,
    dest_clk,
    dest_out);
  input src_clk;
  input [3:0]src_in;
  input dest_clk;
  output [3:0]dest_out;

  wire [3:0]async_path_bit;
  wire dest_clk;
  wire src_clk;
  wire [3:0]src_in;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) wire [3:0]\syncstages_ff[0] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) wire [3:0]\syncstages_ff[1] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) wire [3:0]\syncstages_ff[2] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "ARRAY_SINGLE" *) wire [3:0]\syncstages_ff[3] ;

  assign dest_out[3:0] = \syncstages_ff[3] ;
  LUT1 #(
    .INIT(2'h2)) 
    i_0
       (.I0(1'b0),
        .O(async_path_bit[3]));
  LUT1 #(
    .INIT(2'h2)) 
    i_1
       (.I0(1'b0),
        .O(async_path_bit[2]));
  LUT1 #(
    .INIT(2'h2)) 
    i_2
       (.I0(1'b0),
        .O(async_path_bit[1]));
  FDRE \src_ff_reg[0] 
       (.C(src_clk),
        .CE(1'b1),
        .D(src_in[0]),
        .Q(async_path_bit[0]),
        .R(1'b0));
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
  FDRE \syncstages_ff_reg[0][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[1]),
        .Q(\syncstages_ff[0] [1]),
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
  FDRE \syncstages_ff_reg[0][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path_bit[3]),
        .Q(\syncstages_ff[0] [3]),
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
  FDRE \syncstages_ff_reg[1][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [1]),
        .Q(\syncstages_ff[1] [1]),
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
  FDRE \syncstages_ff_reg[1][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[0] [3]),
        .Q(\syncstages_ff[1] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[2][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[1] [0]),
        .Q(\syncstages_ff[2] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[2][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[1] [1]),
        .Q(\syncstages_ff[2] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[2][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[1] [2]),
        .Q(\syncstages_ff[2] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[2][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[1] [3]),
        .Q(\syncstages_ff[2] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[3][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[2] [0]),
        .Q(\syncstages_ff[3] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[3][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[2] [1]),
        .Q(\syncstages_ff[3] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[3][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[2] [2]),
        .Q(\syncstages_ff[3] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ARRAY_SINGLE" *) 
  FDRE \syncstages_ff_reg[3][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\syncstages_ff[2] [3]),
        .Q(\syncstages_ff[3] [3]),
        .R(1'b0));
endmodule

(* DEST_SYNC_FF = "3" *) (* INIT_SYNC_FF = "1" *) (* REG_OUTPUT = "0" *) 
(* SIM_ASSERT_CHK = "0" *) (* SIM_LOSSLESS_GRAY_CHK = "0" *) (* VERSION = "0" *) 
(* WIDTH = "4" *) (* XPM_MODULE = "TRUE" *) (* keep_hierarchy = "true" *) 
(* xpm_cdc = "GRAY" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_gray
   (src_clk,
    src_in_bin,
    dest_clk,
    dest_out_bin);
  input src_clk;
  input [3:0]src_in_bin;
  input dest_clk;
  output [3:0]dest_out_bin;

  wire [3:0]async_path;
  wire dest_clk;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [3:0]\dest_graysync_ff[0] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [3:0]\dest_graysync_ff[1] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [3:0]\dest_graysync_ff[2] ;
  wire [2:0]\^dest_out_bin ;
  wire [2:0]gray_enc;
  wire src_clk;
  wire [3:0]src_in_bin;

  assign dest_out_bin[3] = \dest_graysync_ff[2] [3];
  assign dest_out_bin[2:0] = \^dest_out_bin [2:0];
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[0]),
        .Q(\dest_graysync_ff[0] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[1]),
        .Q(\dest_graysync_ff[0] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[2]),
        .Q(\dest_graysync_ff[0] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[3]),
        .Q(\dest_graysync_ff[0] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [0]),
        .Q(\dest_graysync_ff[1] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [1]),
        .Q(\dest_graysync_ff[1] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [2]),
        .Q(\dest_graysync_ff[1] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [3]),
        .Q(\dest_graysync_ff[1] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [0]),
        .Q(\dest_graysync_ff[2] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [1]),
        .Q(\dest_graysync_ff[2] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [2]),
        .Q(\dest_graysync_ff[2] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [3]),
        .Q(\dest_graysync_ff[2] [3]),
        .R(1'b0));
  LUT4 #(
    .INIT(16'h6996)) 
    \dest_out_bin[0]_INST_0 
       (.I0(\dest_graysync_ff[2] [0]),
        .I1(\dest_graysync_ff[2] [2]),
        .I2(\dest_graysync_ff[2] [3]),
        .I3(\dest_graysync_ff[2] [1]),
        .O(\^dest_out_bin [0]));
  LUT3 #(
    .INIT(8'h96)) 
    \dest_out_bin[1]_INST_0 
       (.I0(\dest_graysync_ff[2] [1]),
        .I1(\dest_graysync_ff[2] [3]),
        .I2(\dest_graysync_ff[2] [2]),
        .O(\^dest_out_bin [1]));
  LUT2 #(
    .INIT(4'h6)) 
    \dest_out_bin[2]_INST_0 
       (.I0(\dest_graysync_ff[2] [2]),
        .I1(\dest_graysync_ff[2] [3]),
        .O(\^dest_out_bin [2]));
  (* SOFT_HLUTNM = "soft_lutpair52" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[0]_i_1 
       (.I0(src_in_bin[1]),
        .I1(src_in_bin[0]),
        .O(gray_enc[0]));
  (* SOFT_HLUTNM = "soft_lutpair52" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[1]_i_1 
       (.I0(src_in_bin[2]),
        .I1(src_in_bin[1]),
        .O(gray_enc[1]));
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[2]_i_1 
       (.I0(src_in_bin[3]),
        .I1(src_in_bin[2]),
        .O(gray_enc[2]));
  FDRE \src_gray_ff_reg[0] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[0]),
        .Q(async_path[0]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[1] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[1]),
        .Q(async_path[1]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[2] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[2]),
        .Q(async_path[2]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[3] 
       (.C(src_clk),
        .CE(1'b1),
        .D(src_in_bin[3]),
        .Q(async_path[3]),
        .R(1'b0));
endmodule

(* DEST_SYNC_FF = "3" *) (* INIT_SYNC_FF = "1" *) (* ORIG_REF_NAME = "xpm_cdc_gray" *) 
(* REG_OUTPUT = "0" *) (* SIM_ASSERT_CHK = "0" *) (* SIM_LOSSLESS_GRAY_CHK = "0" *) 
(* VERSION = "0" *) (* WIDTH = "4" *) (* XPM_MODULE = "TRUE" *) 
(* keep_hierarchy = "true" *) (* xpm_cdc = "GRAY" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_gray__4
   (src_clk,
    src_in_bin,
    dest_clk,
    dest_out_bin);
  input src_clk;
  input [3:0]src_in_bin;
  input dest_clk;
  output [3:0]dest_out_bin;

  wire [3:0]async_path;
  wire dest_clk;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [3:0]\dest_graysync_ff[0] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [3:0]\dest_graysync_ff[1] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [3:0]\dest_graysync_ff[2] ;
  wire [2:0]\^dest_out_bin ;
  wire [2:0]gray_enc;
  wire src_clk;
  wire [3:0]src_in_bin;

  assign dest_out_bin[3] = \dest_graysync_ff[2] [3];
  assign dest_out_bin[2:0] = \^dest_out_bin [2:0];
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[0]),
        .Q(\dest_graysync_ff[0] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[1]),
        .Q(\dest_graysync_ff[0] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[2]),
        .Q(\dest_graysync_ff[0] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[3]),
        .Q(\dest_graysync_ff[0] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [0]),
        .Q(\dest_graysync_ff[1] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [1]),
        .Q(\dest_graysync_ff[1] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [2]),
        .Q(\dest_graysync_ff[1] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [3]),
        .Q(\dest_graysync_ff[1] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [0]),
        .Q(\dest_graysync_ff[2] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [1]),
        .Q(\dest_graysync_ff[2] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [2]),
        .Q(\dest_graysync_ff[2] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [3]),
        .Q(\dest_graysync_ff[2] [3]),
        .R(1'b0));
  LUT4 #(
    .INIT(16'h6996)) 
    \dest_out_bin[0]_INST_0 
       (.I0(\dest_graysync_ff[2] [0]),
        .I1(\dest_graysync_ff[2] [2]),
        .I2(\dest_graysync_ff[2] [3]),
        .I3(\dest_graysync_ff[2] [1]),
        .O(\^dest_out_bin [0]));
  LUT3 #(
    .INIT(8'h96)) 
    \dest_out_bin[1]_INST_0 
       (.I0(\dest_graysync_ff[2] [1]),
        .I1(\dest_graysync_ff[2] [3]),
        .I2(\dest_graysync_ff[2] [2]),
        .O(\^dest_out_bin [1]));
  LUT2 #(
    .INIT(4'h6)) 
    \dest_out_bin[2]_INST_0 
       (.I0(\dest_graysync_ff[2] [2]),
        .I1(\dest_graysync_ff[2] [3]),
        .O(\^dest_out_bin [2]));
  (* SOFT_HLUTNM = "soft_lutpair0" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[0]_i_1 
       (.I0(src_in_bin[1]),
        .I1(src_in_bin[0]),
        .O(gray_enc[0]));
  (* SOFT_HLUTNM = "soft_lutpair0" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[1]_i_1 
       (.I0(src_in_bin[2]),
        .I1(src_in_bin[1]),
        .O(gray_enc[1]));
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[2]_i_1 
       (.I0(src_in_bin[3]),
        .I1(src_in_bin[2]),
        .O(gray_enc[2]));
  FDRE \src_gray_ff_reg[0] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[0]),
        .Q(async_path[0]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[1] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[1]),
        .Q(async_path[1]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[2] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[2]),
        .Q(async_path[2]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[3] 
       (.C(src_clk),
        .CE(1'b1),
        .D(src_in_bin[3]),
        .Q(async_path[3]),
        .R(1'b0));
endmodule

(* DEST_SYNC_FF = "3" *) (* INIT_SYNC_FF = "1" *) (* ORIG_REF_NAME = "xpm_cdc_gray" *) 
(* REG_OUTPUT = "0" *) (* SIM_ASSERT_CHK = "0" *) (* SIM_LOSSLESS_GRAY_CHK = "0" *) 
(* VERSION = "0" *) (* WIDTH = "4" *) (* XPM_MODULE = "TRUE" *) 
(* keep_hierarchy = "true" *) (* xpm_cdc = "GRAY" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_gray__5
   (src_clk,
    src_in_bin,
    dest_clk,
    dest_out_bin);
  input src_clk;
  input [3:0]src_in_bin;
  input dest_clk;
  output [3:0]dest_out_bin;

  wire [3:0]async_path;
  wire dest_clk;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [3:0]\dest_graysync_ff[0] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [3:0]\dest_graysync_ff[1] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [3:0]\dest_graysync_ff[2] ;
  wire [2:0]\^dest_out_bin ;
  wire [2:0]gray_enc;
  wire src_clk;
  wire [3:0]src_in_bin;

  assign dest_out_bin[3] = \dest_graysync_ff[2] [3];
  assign dest_out_bin[2:0] = \^dest_out_bin [2:0];
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[0]),
        .Q(\dest_graysync_ff[0] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[1]),
        .Q(\dest_graysync_ff[0] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[2]),
        .Q(\dest_graysync_ff[0] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[3]),
        .Q(\dest_graysync_ff[0] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [0]),
        .Q(\dest_graysync_ff[1] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [1]),
        .Q(\dest_graysync_ff[1] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [2]),
        .Q(\dest_graysync_ff[1] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [3]),
        .Q(\dest_graysync_ff[1] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [0]),
        .Q(\dest_graysync_ff[2] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [1]),
        .Q(\dest_graysync_ff[2] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [2]),
        .Q(\dest_graysync_ff[2] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [3]),
        .Q(\dest_graysync_ff[2] [3]),
        .R(1'b0));
  LUT4 #(
    .INIT(16'h6996)) 
    \dest_out_bin[0]_INST_0 
       (.I0(\dest_graysync_ff[2] [0]),
        .I1(\dest_graysync_ff[2] [2]),
        .I2(\dest_graysync_ff[2] [3]),
        .I3(\dest_graysync_ff[2] [1]),
        .O(\^dest_out_bin [0]));
  LUT3 #(
    .INIT(8'h96)) 
    \dest_out_bin[1]_INST_0 
       (.I0(\dest_graysync_ff[2] [1]),
        .I1(\dest_graysync_ff[2] [3]),
        .I2(\dest_graysync_ff[2] [2]),
        .O(\^dest_out_bin [1]));
  LUT2 #(
    .INIT(4'h6)) 
    \dest_out_bin[2]_INST_0 
       (.I0(\dest_graysync_ff[2] [2]),
        .I1(\dest_graysync_ff[2] [3]),
        .O(\^dest_out_bin [2]));
  (* SOFT_HLUTNM = "soft_lutpair3" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[0]_i_1 
       (.I0(src_in_bin[1]),
        .I1(src_in_bin[0]),
        .O(gray_enc[0]));
  (* SOFT_HLUTNM = "soft_lutpair3" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[1]_i_1 
       (.I0(src_in_bin[2]),
        .I1(src_in_bin[1]),
        .O(gray_enc[1]));
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[2]_i_1 
       (.I0(src_in_bin[3]),
        .I1(src_in_bin[2]),
        .O(gray_enc[2]));
  FDRE \src_gray_ff_reg[0] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[0]),
        .Q(async_path[0]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[1] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[1]),
        .Q(async_path[1]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[2] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[2]),
        .Q(async_path[2]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[3] 
       (.C(src_clk),
        .CE(1'b1),
        .D(src_in_bin[3]),
        .Q(async_path[3]),
        .R(1'b0));
endmodule

(* DEST_SYNC_FF = "3" *) (* INIT_SYNC_FF = "1" *) (* ORIG_REF_NAME = "xpm_cdc_gray" *) 
(* REG_OUTPUT = "0" *) (* SIM_ASSERT_CHK = "0" *) (* SIM_LOSSLESS_GRAY_CHK = "0" *) 
(* VERSION = "0" *) (* WIDTH = "4" *) (* XPM_MODULE = "TRUE" *) 
(* keep_hierarchy = "true" *) (* xpm_cdc = "GRAY" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_gray__6
   (src_clk,
    src_in_bin,
    dest_clk,
    dest_out_bin);
  input src_clk;
  input [3:0]src_in_bin;
  input dest_clk;
  output [3:0]dest_out_bin;

  wire [3:0]async_path;
  wire dest_clk;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [3:0]\dest_graysync_ff[0] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [3:0]\dest_graysync_ff[1] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [3:0]\dest_graysync_ff[2] ;
  wire [2:0]\^dest_out_bin ;
  wire [2:0]gray_enc;
  wire src_clk;
  wire [3:0]src_in_bin;

  assign dest_out_bin[3] = \dest_graysync_ff[2] [3];
  assign dest_out_bin[2:0] = \^dest_out_bin [2:0];
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[0]),
        .Q(\dest_graysync_ff[0] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[1]),
        .Q(\dest_graysync_ff[0] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[2]),
        .Q(\dest_graysync_ff[0] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[3]),
        .Q(\dest_graysync_ff[0] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [0]),
        .Q(\dest_graysync_ff[1] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [1]),
        .Q(\dest_graysync_ff[1] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [2]),
        .Q(\dest_graysync_ff[1] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [3]),
        .Q(\dest_graysync_ff[1] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [0]),
        .Q(\dest_graysync_ff[2] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [1]),
        .Q(\dest_graysync_ff[2] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [2]),
        .Q(\dest_graysync_ff[2] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [3]),
        .Q(\dest_graysync_ff[2] [3]),
        .R(1'b0));
  LUT4 #(
    .INIT(16'h6996)) 
    \dest_out_bin[0]_INST_0 
       (.I0(\dest_graysync_ff[2] [0]),
        .I1(\dest_graysync_ff[2] [2]),
        .I2(\dest_graysync_ff[2] [3]),
        .I3(\dest_graysync_ff[2] [1]),
        .O(\^dest_out_bin [0]));
  LUT3 #(
    .INIT(8'h96)) 
    \dest_out_bin[1]_INST_0 
       (.I0(\dest_graysync_ff[2] [1]),
        .I1(\dest_graysync_ff[2] [3]),
        .I2(\dest_graysync_ff[2] [2]),
        .O(\^dest_out_bin [1]));
  LUT2 #(
    .INIT(4'h6)) 
    \dest_out_bin[2]_INST_0 
       (.I0(\dest_graysync_ff[2] [2]),
        .I1(\dest_graysync_ff[2] [3]),
        .O(\^dest_out_bin [2]));
  (* SOFT_HLUTNM = "soft_lutpair49" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[0]_i_1 
       (.I0(src_in_bin[1]),
        .I1(src_in_bin[0]),
        .O(gray_enc[0]));
  (* SOFT_HLUTNM = "soft_lutpair49" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[1]_i_1 
       (.I0(src_in_bin[2]),
        .I1(src_in_bin[1]),
        .O(gray_enc[1]));
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[2]_i_1 
       (.I0(src_in_bin[3]),
        .I1(src_in_bin[2]),
        .O(gray_enc[2]));
  FDRE \src_gray_ff_reg[0] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[0]),
        .Q(async_path[0]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[1] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[1]),
        .Q(async_path[1]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[2] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[2]),
        .Q(async_path[2]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[3] 
       (.C(src_clk),
        .CE(1'b1),
        .D(src_in_bin[3]),
        .Q(async_path[3]),
        .R(1'b0));
endmodule

(* DEST_SYNC_FF = "5" *) (* INIT_SYNC_FF = "1" *) (* ORIG_REF_NAME = "xpm_cdc_gray" *) 
(* REG_OUTPUT = "0" *) (* SIM_ASSERT_CHK = "0" *) (* SIM_LOSSLESS_GRAY_CHK = "0" *) 
(* VERSION = "0" *) (* WIDTH = "5" *) (* XPM_MODULE = "TRUE" *) 
(* keep_hierarchy = "true" *) (* xpm_cdc = "GRAY" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_gray__parameterized0
   (src_clk,
    src_in_bin,
    dest_clk,
    dest_out_bin);
  input src_clk;
  input [4:0]src_in_bin;
  input dest_clk;
  output [4:0]dest_out_bin;

  wire [4:0]async_path;
  wire dest_clk;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [4:0]\dest_graysync_ff[0] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [4:0]\dest_graysync_ff[1] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [4:0]\dest_graysync_ff[2] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [4:0]\dest_graysync_ff[3] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [4:0]\dest_graysync_ff[4] ;
  wire [3:0]\^dest_out_bin ;
  wire [3:0]gray_enc;
  wire src_clk;
  wire [4:0]src_in_bin;

  assign dest_out_bin[4] = \dest_graysync_ff[4] [4];
  assign dest_out_bin[3:0] = \^dest_out_bin [3:0];
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[0]),
        .Q(\dest_graysync_ff[0] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[1]),
        .Q(\dest_graysync_ff[0] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[2]),
        .Q(\dest_graysync_ff[0] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[3]),
        .Q(\dest_graysync_ff[0] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][4] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[4]),
        .Q(\dest_graysync_ff[0] [4]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [0]),
        .Q(\dest_graysync_ff[1] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [1]),
        .Q(\dest_graysync_ff[1] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [2]),
        .Q(\dest_graysync_ff[1] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [3]),
        .Q(\dest_graysync_ff[1] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][4] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [4]),
        .Q(\dest_graysync_ff[1] [4]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [0]),
        .Q(\dest_graysync_ff[2] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [1]),
        .Q(\dest_graysync_ff[2] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [2]),
        .Q(\dest_graysync_ff[2] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [3]),
        .Q(\dest_graysync_ff[2] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][4] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [4]),
        .Q(\dest_graysync_ff[2] [4]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[3][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[2] [0]),
        .Q(\dest_graysync_ff[3] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[3][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[2] [1]),
        .Q(\dest_graysync_ff[3] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[3][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[2] [2]),
        .Q(\dest_graysync_ff[3] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[3][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[2] [3]),
        .Q(\dest_graysync_ff[3] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[3][4] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[2] [4]),
        .Q(\dest_graysync_ff[3] [4]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[4][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[3] [0]),
        .Q(\dest_graysync_ff[4] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[4][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[3] [1]),
        .Q(\dest_graysync_ff[4] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[4][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[3] [2]),
        .Q(\dest_graysync_ff[4] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[4][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[3] [3]),
        .Q(\dest_graysync_ff[4] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[4][4] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[3] [4]),
        .Q(\dest_graysync_ff[4] [4]),
        .R(1'b0));
  LUT5 #(
    .INIT(32'h96696996)) 
    \dest_out_bin[0]_INST_0 
       (.I0(\dest_graysync_ff[4] [0]),
        .I1(\dest_graysync_ff[4] [2]),
        .I2(\dest_graysync_ff[4] [4]),
        .I3(\dest_graysync_ff[4] [3]),
        .I4(\dest_graysync_ff[4] [1]),
        .O(\^dest_out_bin [0]));
  LUT4 #(
    .INIT(16'h6996)) 
    \dest_out_bin[1]_INST_0 
       (.I0(\dest_graysync_ff[4] [1]),
        .I1(\dest_graysync_ff[4] [3]),
        .I2(\dest_graysync_ff[4] [4]),
        .I3(\dest_graysync_ff[4] [2]),
        .O(\^dest_out_bin [1]));
  LUT3 #(
    .INIT(8'h96)) 
    \dest_out_bin[2]_INST_0 
       (.I0(\dest_graysync_ff[4] [2]),
        .I1(\dest_graysync_ff[4] [4]),
        .I2(\dest_graysync_ff[4] [3]),
        .O(\^dest_out_bin [2]));
  LUT2 #(
    .INIT(4'h6)) 
    \dest_out_bin[3]_INST_0 
       (.I0(\dest_graysync_ff[4] [3]),
        .I1(\dest_graysync_ff[4] [4]),
        .O(\^dest_out_bin [3]));
  (* SOFT_HLUTNM = "soft_lutpair50" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[0]_i_1 
       (.I0(src_in_bin[1]),
        .I1(src_in_bin[0]),
        .O(gray_enc[0]));
  (* SOFT_HLUTNM = "soft_lutpair50" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[1]_i_1 
       (.I0(src_in_bin[2]),
        .I1(src_in_bin[1]),
        .O(gray_enc[1]));
  (* SOFT_HLUTNM = "soft_lutpair51" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[2]_i_1 
       (.I0(src_in_bin[3]),
        .I1(src_in_bin[2]),
        .O(gray_enc[2]));
  (* SOFT_HLUTNM = "soft_lutpair51" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[3]_i_1 
       (.I0(src_in_bin[4]),
        .I1(src_in_bin[3]),
        .O(gray_enc[3]));
  FDRE \src_gray_ff_reg[0] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[0]),
        .Q(async_path[0]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[1] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[1]),
        .Q(async_path[1]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[2] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[2]),
        .Q(async_path[2]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[3] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[3]),
        .Q(async_path[3]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[4] 
       (.C(src_clk),
        .CE(1'b1),
        .D(src_in_bin[4]),
        .Q(async_path[4]),
        .R(1'b0));
endmodule

(* DEST_SYNC_FF = "5" *) (* INIT_SYNC_FF = "1" *) (* ORIG_REF_NAME = "xpm_cdc_gray" *) 
(* REG_OUTPUT = "0" *) (* SIM_ASSERT_CHK = "0" *) (* SIM_LOSSLESS_GRAY_CHK = "0" *) 
(* VERSION = "0" *) (* WIDTH = "5" *) (* XPM_MODULE = "TRUE" *) 
(* keep_hierarchy = "true" *) (* xpm_cdc = "GRAY" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_gray__parameterized0__2
   (src_clk,
    src_in_bin,
    dest_clk,
    dest_out_bin);
  input src_clk;
  input [4:0]src_in_bin;
  input dest_clk;
  output [4:0]dest_out_bin;

  wire [4:0]async_path;
  wire dest_clk;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [4:0]\dest_graysync_ff[0] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [4:0]\dest_graysync_ff[1] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [4:0]\dest_graysync_ff[2] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [4:0]\dest_graysync_ff[3] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [4:0]\dest_graysync_ff[4] ;
  wire [3:0]\^dest_out_bin ;
  wire [3:0]gray_enc;
  wire src_clk;
  wire [4:0]src_in_bin;

  assign dest_out_bin[4] = \dest_graysync_ff[4] [4];
  assign dest_out_bin[3:0] = \^dest_out_bin [3:0];
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[0]),
        .Q(\dest_graysync_ff[0] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[1]),
        .Q(\dest_graysync_ff[0] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[2]),
        .Q(\dest_graysync_ff[0] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[3]),
        .Q(\dest_graysync_ff[0] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][4] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[4]),
        .Q(\dest_graysync_ff[0] [4]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [0]),
        .Q(\dest_graysync_ff[1] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [1]),
        .Q(\dest_graysync_ff[1] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [2]),
        .Q(\dest_graysync_ff[1] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [3]),
        .Q(\dest_graysync_ff[1] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][4] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [4]),
        .Q(\dest_graysync_ff[1] [4]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [0]),
        .Q(\dest_graysync_ff[2] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [1]),
        .Q(\dest_graysync_ff[2] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [2]),
        .Q(\dest_graysync_ff[2] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [3]),
        .Q(\dest_graysync_ff[2] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][4] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [4]),
        .Q(\dest_graysync_ff[2] [4]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[3][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[2] [0]),
        .Q(\dest_graysync_ff[3] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[3][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[2] [1]),
        .Q(\dest_graysync_ff[3] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[3][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[2] [2]),
        .Q(\dest_graysync_ff[3] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[3][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[2] [3]),
        .Q(\dest_graysync_ff[3] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[3][4] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[2] [4]),
        .Q(\dest_graysync_ff[3] [4]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[4][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[3] [0]),
        .Q(\dest_graysync_ff[4] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[4][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[3] [1]),
        .Q(\dest_graysync_ff[4] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[4][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[3] [2]),
        .Q(\dest_graysync_ff[4] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[4][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[3] [3]),
        .Q(\dest_graysync_ff[4] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[4][4] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[3] [4]),
        .Q(\dest_graysync_ff[4] [4]),
        .R(1'b0));
  LUT5 #(
    .INIT(32'h96696996)) 
    \dest_out_bin[0]_INST_0 
       (.I0(\dest_graysync_ff[4] [0]),
        .I1(\dest_graysync_ff[4] [2]),
        .I2(\dest_graysync_ff[4] [4]),
        .I3(\dest_graysync_ff[4] [3]),
        .I4(\dest_graysync_ff[4] [1]),
        .O(\^dest_out_bin [0]));
  LUT4 #(
    .INIT(16'h6996)) 
    \dest_out_bin[1]_INST_0 
       (.I0(\dest_graysync_ff[4] [1]),
        .I1(\dest_graysync_ff[4] [3]),
        .I2(\dest_graysync_ff[4] [4]),
        .I3(\dest_graysync_ff[4] [2]),
        .O(\^dest_out_bin [1]));
  LUT3 #(
    .INIT(8'h96)) 
    \dest_out_bin[2]_INST_0 
       (.I0(\dest_graysync_ff[4] [2]),
        .I1(\dest_graysync_ff[4] [4]),
        .I2(\dest_graysync_ff[4] [3]),
        .O(\^dest_out_bin [2]));
  LUT2 #(
    .INIT(4'h6)) 
    \dest_out_bin[3]_INST_0 
       (.I0(\dest_graysync_ff[4] [3]),
        .I1(\dest_graysync_ff[4] [4]),
        .O(\^dest_out_bin [3]));
  (* SOFT_HLUTNM = "soft_lutpair1" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[0]_i_1 
       (.I0(src_in_bin[1]),
        .I1(src_in_bin[0]),
        .O(gray_enc[0]));
  (* SOFT_HLUTNM = "soft_lutpair1" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[1]_i_1 
       (.I0(src_in_bin[2]),
        .I1(src_in_bin[1]),
        .O(gray_enc[1]));
  (* SOFT_HLUTNM = "soft_lutpair2" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[2]_i_1 
       (.I0(src_in_bin[3]),
        .I1(src_in_bin[2]),
        .O(gray_enc[2]));
  (* SOFT_HLUTNM = "soft_lutpair2" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[3]_i_1 
       (.I0(src_in_bin[4]),
        .I1(src_in_bin[3]),
        .O(gray_enc[3]));
  FDRE \src_gray_ff_reg[0] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[0]),
        .Q(async_path[0]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[1] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[1]),
        .Q(async_path[1]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[2] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[2]),
        .Q(async_path[2]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[3] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[3]),
        .Q(async_path[3]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[4] 
       (.C(src_clk),
        .CE(1'b1),
        .D(src_in_bin[4]),
        .Q(async_path[4]),
        .R(1'b0));
endmodule

(* DEST_SYNC_FF = "3" *) (* INIT_SYNC_FF = "1" *) (* ORIG_REF_NAME = "xpm_cdc_gray" *) 
(* REG_OUTPUT = "0" *) (* SIM_ASSERT_CHK = "0" *) (* SIM_LOSSLESS_GRAY_CHK = "0" *) 
(* VERSION = "0" *) (* WIDTH = "5" *) (* XPM_MODULE = "TRUE" *) 
(* keep_hierarchy = "true" *) (* xpm_cdc = "GRAY" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_gray__parameterized1
   (src_clk,
    src_in_bin,
    dest_clk,
    dest_out_bin);
  input src_clk;
  input [4:0]src_in_bin;
  input dest_clk;
  output [4:0]dest_out_bin;

  wire [4:0]async_path;
  wire dest_clk;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [4:0]\dest_graysync_ff[0] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [4:0]\dest_graysync_ff[1] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [4:0]\dest_graysync_ff[2] ;
  wire [3:0]\^dest_out_bin ;
  wire [3:0]gray_enc;
  wire src_clk;
  wire [4:0]src_in_bin;

  assign dest_out_bin[4] = \dest_graysync_ff[2] [4];
  assign dest_out_bin[3:0] = \^dest_out_bin [3:0];
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[0]),
        .Q(\dest_graysync_ff[0] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[1]),
        .Q(\dest_graysync_ff[0] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[2]),
        .Q(\dest_graysync_ff[0] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[3]),
        .Q(\dest_graysync_ff[0] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][4] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[4]),
        .Q(\dest_graysync_ff[0] [4]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [0]),
        .Q(\dest_graysync_ff[1] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [1]),
        .Q(\dest_graysync_ff[1] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [2]),
        .Q(\dest_graysync_ff[1] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [3]),
        .Q(\dest_graysync_ff[1] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][4] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [4]),
        .Q(\dest_graysync_ff[1] [4]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [0]),
        .Q(\dest_graysync_ff[2] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [1]),
        .Q(\dest_graysync_ff[2] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [2]),
        .Q(\dest_graysync_ff[2] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [3]),
        .Q(\dest_graysync_ff[2] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][4] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [4]),
        .Q(\dest_graysync_ff[2] [4]),
        .R(1'b0));
  LUT5 #(
    .INIT(32'h96696996)) 
    \dest_out_bin[0]_INST_0 
       (.I0(\dest_graysync_ff[2] [0]),
        .I1(\dest_graysync_ff[2] [2]),
        .I2(\dest_graysync_ff[2] [4]),
        .I3(\dest_graysync_ff[2] [3]),
        .I4(\dest_graysync_ff[2] [1]),
        .O(\^dest_out_bin [0]));
  LUT4 #(
    .INIT(16'h6996)) 
    \dest_out_bin[1]_INST_0 
       (.I0(\dest_graysync_ff[2] [1]),
        .I1(\dest_graysync_ff[2] [3]),
        .I2(\dest_graysync_ff[2] [4]),
        .I3(\dest_graysync_ff[2] [2]),
        .O(\^dest_out_bin [1]));
  LUT3 #(
    .INIT(8'h96)) 
    \dest_out_bin[2]_INST_0 
       (.I0(\dest_graysync_ff[2] [2]),
        .I1(\dest_graysync_ff[2] [4]),
        .I2(\dest_graysync_ff[2] [3]),
        .O(\^dest_out_bin [2]));
  LUT2 #(
    .INIT(4'h6)) 
    \dest_out_bin[3]_INST_0 
       (.I0(\dest_graysync_ff[2] [3]),
        .I1(\dest_graysync_ff[2] [4]),
        .O(\^dest_out_bin [3]));
  (* SOFT_HLUTNM = "soft_lutpair53" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[0]_i_1 
       (.I0(src_in_bin[1]),
        .I1(src_in_bin[0]),
        .O(gray_enc[0]));
  (* SOFT_HLUTNM = "soft_lutpair53" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[1]_i_1 
       (.I0(src_in_bin[2]),
        .I1(src_in_bin[1]),
        .O(gray_enc[1]));
  (* SOFT_HLUTNM = "soft_lutpair54" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[2]_i_1 
       (.I0(src_in_bin[3]),
        .I1(src_in_bin[2]),
        .O(gray_enc[2]));
  (* SOFT_HLUTNM = "soft_lutpair54" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[3]_i_1 
       (.I0(src_in_bin[4]),
        .I1(src_in_bin[3]),
        .O(gray_enc[3]));
  FDRE \src_gray_ff_reg[0] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[0]),
        .Q(async_path[0]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[1] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[1]),
        .Q(async_path[1]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[2] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[2]),
        .Q(async_path[2]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[3] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[3]),
        .Q(async_path[3]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[4] 
       (.C(src_clk),
        .CE(1'b1),
        .D(src_in_bin[4]),
        .Q(async_path[4]),
        .R(1'b0));
endmodule

(* DEST_SYNC_FF = "3" *) (* INIT_SYNC_FF = "1" *) (* ORIG_REF_NAME = "xpm_cdc_gray" *) 
(* REG_OUTPUT = "0" *) (* SIM_ASSERT_CHK = "0" *) (* SIM_LOSSLESS_GRAY_CHK = "0" *) 
(* VERSION = "0" *) (* WIDTH = "5" *) (* XPM_MODULE = "TRUE" *) 
(* keep_hierarchy = "true" *) (* xpm_cdc = "GRAY" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_gray__parameterized1__2
   (src_clk,
    src_in_bin,
    dest_clk,
    dest_out_bin);
  input src_clk;
  input [4:0]src_in_bin;
  input dest_clk;
  output [4:0]dest_out_bin;

  wire [4:0]async_path;
  wire dest_clk;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [4:0]\dest_graysync_ff[0] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [4:0]\dest_graysync_ff[1] ;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "GRAY" *) wire [4:0]\dest_graysync_ff[2] ;
  wire [3:0]\^dest_out_bin ;
  wire [3:0]gray_enc;
  wire src_clk;
  wire [4:0]src_in_bin;

  assign dest_out_bin[4] = \dest_graysync_ff[2] [4];
  assign dest_out_bin[3:0] = \^dest_out_bin [3:0];
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[0]),
        .Q(\dest_graysync_ff[0] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[1]),
        .Q(\dest_graysync_ff[0] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[2]),
        .Q(\dest_graysync_ff[0] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[3]),
        .Q(\dest_graysync_ff[0] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[0][4] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(async_path[4]),
        .Q(\dest_graysync_ff[0] [4]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [0]),
        .Q(\dest_graysync_ff[1] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [1]),
        .Q(\dest_graysync_ff[1] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [2]),
        .Q(\dest_graysync_ff[1] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [3]),
        .Q(\dest_graysync_ff[1] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[1][4] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[0] [4]),
        .Q(\dest_graysync_ff[1] [4]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [0]),
        .Q(\dest_graysync_ff[2] [0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [1]),
        .Q(\dest_graysync_ff[2] [1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [2]),
        .Q(\dest_graysync_ff[2] [2]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][3] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [3]),
        .Q(\dest_graysync_ff[2] [3]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "GRAY" *) 
  FDRE \dest_graysync_ff_reg[2][4] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(\dest_graysync_ff[1] [4]),
        .Q(\dest_graysync_ff[2] [4]),
        .R(1'b0));
  LUT5 #(
    .INIT(32'h96696996)) 
    \dest_out_bin[0]_INST_0 
       (.I0(\dest_graysync_ff[2] [0]),
        .I1(\dest_graysync_ff[2] [2]),
        .I2(\dest_graysync_ff[2] [4]),
        .I3(\dest_graysync_ff[2] [3]),
        .I4(\dest_graysync_ff[2] [1]),
        .O(\^dest_out_bin [0]));
  LUT4 #(
    .INIT(16'h6996)) 
    \dest_out_bin[1]_INST_0 
       (.I0(\dest_graysync_ff[2] [1]),
        .I1(\dest_graysync_ff[2] [3]),
        .I2(\dest_graysync_ff[2] [4]),
        .I3(\dest_graysync_ff[2] [2]),
        .O(\^dest_out_bin [1]));
  LUT3 #(
    .INIT(8'h96)) 
    \dest_out_bin[2]_INST_0 
       (.I0(\dest_graysync_ff[2] [2]),
        .I1(\dest_graysync_ff[2] [4]),
        .I2(\dest_graysync_ff[2] [3]),
        .O(\^dest_out_bin [2]));
  LUT2 #(
    .INIT(4'h6)) 
    \dest_out_bin[3]_INST_0 
       (.I0(\dest_graysync_ff[2] [3]),
        .I1(\dest_graysync_ff[2] [4]),
        .O(\^dest_out_bin [3]));
  (* SOFT_HLUTNM = "soft_lutpair4" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[0]_i_1 
       (.I0(src_in_bin[1]),
        .I1(src_in_bin[0]),
        .O(gray_enc[0]));
  (* SOFT_HLUTNM = "soft_lutpair4" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[1]_i_1 
       (.I0(src_in_bin[2]),
        .I1(src_in_bin[1]),
        .O(gray_enc[1]));
  (* SOFT_HLUTNM = "soft_lutpair5" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[2]_i_1 
       (.I0(src_in_bin[3]),
        .I1(src_in_bin[2]),
        .O(gray_enc[2]));
  (* SOFT_HLUTNM = "soft_lutpair5" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \src_gray_ff[3]_i_1 
       (.I0(src_in_bin[4]),
        .I1(src_in_bin[3]),
        .O(gray_enc[3]));
  FDRE \src_gray_ff_reg[0] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[0]),
        .Q(async_path[0]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[1] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[1]),
        .Q(async_path[1]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[2] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[2]),
        .Q(async_path[2]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[3] 
       (.C(src_clk),
        .CE(1'b1),
        .D(gray_enc[3]),
        .Q(async_path[3]),
        .R(1'b0));
  FDRE \src_gray_ff_reg[4] 
       (.C(src_clk),
        .CE(1'b1),
        .D(src_in_bin[4]),
        .Q(async_path[4]),
        .R(1'b0));
endmodule

(* DEF_VAL = "1'b0" *) (* DEST_SYNC_FF = "3" *) (* INIT = "0" *) 
(* INIT_SYNC_FF = "1" *) (* SIM_ASSERT_CHK = "0" *) (* VERSION = "0" *) 
(* XPM_MODULE = "TRUE" *) (* keep_hierarchy = "true" *) (* xpm_cdc = "SYNC_RST" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_sync_rst
   (src_rst,
    dest_clk,
    dest_rst);
  input src_rst;
  input dest_clk;
  output dest_rst;

  wire dest_clk;
  wire src_rst;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "SYNC_RST" *) wire [2:0]syncstages_ff;

  assign dest_rst = syncstages_ff[2];
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "SYNC_RST" *) 
  FDRE #(
    .INIT(1'b0)) 
    \syncstages_ff_reg[0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(src_rst),
        .Q(syncstages_ff[0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "SYNC_RST" *) 
  FDRE #(
    .INIT(1'b0)) 
    \syncstages_ff_reg[1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(syncstages_ff[0]),
        .Q(syncstages_ff[1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "SYNC_RST" *) 
  FDRE #(
    .INIT(1'b0)) 
    \syncstages_ff_reg[2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(syncstages_ff[1]),
        .Q(syncstages_ff[2]),
        .R(1'b0));
endmodule

(* DEF_VAL = "1'b0" *) (* DEST_SYNC_FF = "3" *) (* INIT = "0" *) 
(* INIT_SYNC_FF = "1" *) (* ORIG_REF_NAME = "xpm_cdc_sync_rst" *) (* SIM_ASSERT_CHK = "0" *) 
(* VERSION = "0" *) (* XPM_MODULE = "TRUE" *) (* keep_hierarchy = "true" *) 
(* xpm_cdc = "SYNC_RST" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_sync_rst__4
   (src_rst,
    dest_clk,
    dest_rst);
  input src_rst;
  input dest_clk;
  output dest_rst;

  wire dest_clk;
  wire src_rst;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "SYNC_RST" *) wire [2:0]syncstages_ff;

  assign dest_rst = syncstages_ff[2];
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "SYNC_RST" *) 
  FDRE #(
    .INIT(1'b0)) 
    \syncstages_ff_reg[0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(src_rst),
        .Q(syncstages_ff[0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "SYNC_RST" *) 
  FDRE #(
    .INIT(1'b0)) 
    \syncstages_ff_reg[1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(syncstages_ff[0]),
        .Q(syncstages_ff[1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "SYNC_RST" *) 
  FDRE #(
    .INIT(1'b0)) 
    \syncstages_ff_reg[2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(syncstages_ff[1]),
        .Q(syncstages_ff[2]),
        .R(1'b0));
endmodule

(* DEF_VAL = "1'b0" *) (* DEST_SYNC_FF = "3" *) (* INIT = "0" *) 
(* INIT_SYNC_FF = "1" *) (* ORIG_REF_NAME = "xpm_cdc_sync_rst" *) (* SIM_ASSERT_CHK = "0" *) 
(* VERSION = "0" *) (* XPM_MODULE = "TRUE" *) (* keep_hierarchy = "true" *) 
(* xpm_cdc = "SYNC_RST" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_sync_rst__5
   (src_rst,
    dest_clk,
    dest_rst);
  input src_rst;
  input dest_clk;
  output dest_rst;

  wire dest_clk;
  wire src_rst;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "SYNC_RST" *) wire [2:0]syncstages_ff;

  assign dest_rst = syncstages_ff[2];
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "SYNC_RST" *) 
  FDRE #(
    .INIT(1'b0)) 
    \syncstages_ff_reg[0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(src_rst),
        .Q(syncstages_ff[0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "SYNC_RST" *) 
  FDRE #(
    .INIT(1'b0)) 
    \syncstages_ff_reg[1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(syncstages_ff[0]),
        .Q(syncstages_ff[1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "SYNC_RST" *) 
  FDRE #(
    .INIT(1'b0)) 
    \syncstages_ff_reg[2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(syncstages_ff[1]),
        .Q(syncstages_ff[2]),
        .R(1'b0));
endmodule

(* DEF_VAL = "1'b0" *) (* DEST_SYNC_FF = "3" *) (* INIT = "0" *) 
(* INIT_SYNC_FF = "1" *) (* ORIG_REF_NAME = "xpm_cdc_sync_rst" *) (* SIM_ASSERT_CHK = "0" *) 
(* VERSION = "0" *) (* XPM_MODULE = "TRUE" *) (* keep_hierarchy = "true" *) 
(* xpm_cdc = "SYNC_RST" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_sync_rst__6
   (src_rst,
    dest_clk,
    dest_rst);
  input src_rst;
  input dest_clk;
  output dest_rst;

  wire dest_clk;
  wire src_rst;
  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "SYNC_RST" *) wire [2:0]syncstages_ff;

  assign dest_rst = syncstages_ff[2];
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "SYNC_RST" *) 
  FDRE #(
    .INIT(1'b0)) 
    \syncstages_ff_reg[0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(src_rst),
        .Q(syncstages_ff[0]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "SYNC_RST" *) 
  FDRE #(
    .INIT(1'b0)) 
    \syncstages_ff_reg[1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(syncstages_ff[0]),
        .Q(syncstages_ff[1]),
        .R(1'b0));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "SYNC_RST" *) 
  FDRE #(
    .INIT(1'b0)) 
    \syncstages_ff_reg[2] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(syncstages_ff[1]),
        .Q(syncstages_ff[2]),
        .R(1'b0));
endmodule

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_counter_updn
   (Q,
    D,
    \count_value_i_reg[1]_0 ,
    src_in_bin,
    ram_empty_i,
    \count_value_i_reg[1]_1 ,
    rd_en,
    \grdc.rd_data_count_i_reg[2] ,
    \grdc.rd_data_count_i_reg[2]_0 ,
    SR,
    rd_clk);
  output [1:0]Q;
  output [0:0]D;
  output \count_value_i_reg[1]_0 ;
  output [0:0]src_in_bin;
  input ram_empty_i;
  input [1:0]\count_value_i_reg[1]_1 ;
  input rd_en;
  input [2:0]\grdc.rd_data_count_i_reg[2] ;
  input [2:0]\grdc.rd_data_count_i_reg[2]_0 ;
  input [0:0]SR;
  input rd_clk;

  wire [0:0]D;
  wire [1:0]Q;
  wire [0:0]SR;
  wire \count_value_i[0]_i_1__3_n_0 ;
  wire \count_value_i[1]_i_3_n_0 ;
  wire \count_value_i_reg[1]_0 ;
  wire [1:0]\count_value_i_reg[1]_1 ;
  wire \gen_fwft.count_en ;
  wire [2:0]\grdc.rd_data_count_i_reg[2] ;
  wire [2:0]\grdc.rd_data_count_i_reg[2]_0 ;
  wire ram_empty_i;
  wire rd_clk;
  wire rd_en;
  wire [0:0]src_in_bin;

  LUT5 #(
    .INIT(32'h5AAAA655)) 
    \count_value_i[0]_i_1__3 
       (.I0(Q[0]),
        .I1(\count_value_i_reg[1]_1 [0]),
        .I2(rd_en),
        .I3(\count_value_i_reg[1]_1 [1]),
        .I4(ram_empty_i),
        .O(\count_value_i[0]_i_1__3_n_0 ));
  LUT4 #(
    .INIT(16'hC02F)) 
    \count_value_i[1]_i_2 
       (.I0(\count_value_i_reg[1]_1 [0]),
        .I1(rd_en),
        .I2(\count_value_i_reg[1]_1 [1]),
        .I3(ram_empty_i),
        .O(\gen_fwft.count_en ));
  LUT6 #(
    .INIT(64'hA999A9A96AAA6AAA)) 
    \count_value_i[1]_i_3 
       (.I0(Q[1]),
        .I1(ram_empty_i),
        .I2(\count_value_i_reg[1]_1 [1]),
        .I3(rd_en),
        .I4(\count_value_i_reg[1]_1 [0]),
        .I5(Q[0]),
        .O(\count_value_i[1]_i_3_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[0] 
       (.C(rd_clk),
        .CE(\gen_fwft.count_en ),
        .D(\count_value_i[0]_i_1__3_n_0 ),
        .Q(Q[0]),
        .R(SR));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[1] 
       (.C(rd_clk),
        .CE(\gen_fwft.count_en ),
        .D(\count_value_i[1]_i_3_n_0 ),
        .Q(Q[1]),
        .R(SR));
  LUT4 #(
    .INIT(16'h2DD2)) 
    \gen_cdc_pntr.rd_pntr_cdc_dc_inst_i_4 
       (.I0(Q[0]),
        .I1(\grdc.rd_data_count_i_reg[2]_0 [0]),
        .I2(Q[1]),
        .I3(\grdc.rd_data_count_i_reg[2]_0 [1]),
        .O(src_in_bin));
  LUT6 #(
    .INIT(64'h9696699669966969)) 
    \grdc.rd_data_count_i[2]_i_1 
       (.I0(\count_value_i_reg[1]_0 ),
        .I1(\grdc.rd_data_count_i_reg[2] [2]),
        .I2(\grdc.rd_data_count_i_reg[2]_0 [2]),
        .I3(\grdc.rd_data_count_i_reg[2]_0 [1]),
        .I4(Q[1]),
        .I5(\grdc.rd_data_count_i_reg[2] [1]),
        .O(D));
  LUT6 #(
    .INIT(64'h69FF696969690069)) 
    \grdc.rd_data_count_i[4]_i_4 
       (.I0(Q[1]),
        .I1(\grdc.rd_data_count_i_reg[2]_0 [1]),
        .I2(\grdc.rd_data_count_i_reg[2] [1]),
        .I3(\grdc.rd_data_count_i_reg[2]_0 [0]),
        .I4(Q[0]),
        .I5(\grdc.rd_data_count_i_reg[2] [0]),
        .O(\count_value_i_reg[1]_0 ));
endmodule

(* ORIG_REF_NAME = "xpm_counter_updn" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_counter_updn_8
   (Q,
    D,
    \count_value_i_reg[1]_0 ,
    src_in_bin,
    ram_empty_i,
    \count_value_i_reg[1]_1 ,
    rd_en,
    \grdc.rd_data_count_i_reg[2] ,
    \grdc.rd_data_count_i_reg[2]_0 ,
    SR,
    rd_clk);
  output [1:0]Q;
  output [0:0]D;
  output \count_value_i_reg[1]_0 ;
  output [0:0]src_in_bin;
  input ram_empty_i;
  input [1:0]\count_value_i_reg[1]_1 ;
  input rd_en;
  input [2:0]\grdc.rd_data_count_i_reg[2] ;
  input [2:0]\grdc.rd_data_count_i_reg[2]_0 ;
  input [0:0]SR;
  input rd_clk;

  wire [0:0]D;
  wire [1:0]Q;
  wire [0:0]SR;
  wire \count_value_i[0]_i_1__3_n_0 ;
  wire \count_value_i[1]_i_3_n_0 ;
  wire \count_value_i_reg[1]_0 ;
  wire [1:0]\count_value_i_reg[1]_1 ;
  wire \gen_fwft.count_en ;
  wire [2:0]\grdc.rd_data_count_i_reg[2] ;
  wire [2:0]\grdc.rd_data_count_i_reg[2]_0 ;
  wire ram_empty_i;
  wire rd_clk;
  wire rd_en;
  wire [0:0]src_in_bin;

  LUT5 #(
    .INIT(32'h5AAAA655)) 
    \count_value_i[0]_i_1__3 
       (.I0(Q[0]),
        .I1(\count_value_i_reg[1]_1 [0]),
        .I2(rd_en),
        .I3(\count_value_i_reg[1]_1 [1]),
        .I4(ram_empty_i),
        .O(\count_value_i[0]_i_1__3_n_0 ));
  LUT4 #(
    .INIT(16'hC02F)) 
    \count_value_i[1]_i_2 
       (.I0(\count_value_i_reg[1]_1 [0]),
        .I1(rd_en),
        .I2(\count_value_i_reg[1]_1 [1]),
        .I3(ram_empty_i),
        .O(\gen_fwft.count_en ));
  LUT6 #(
    .INIT(64'hA999A9A96AAA6AAA)) 
    \count_value_i[1]_i_3 
       (.I0(Q[1]),
        .I1(ram_empty_i),
        .I2(\count_value_i_reg[1]_1 [1]),
        .I3(rd_en),
        .I4(\count_value_i_reg[1]_1 [0]),
        .I5(Q[0]),
        .O(\count_value_i[1]_i_3_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[0] 
       (.C(rd_clk),
        .CE(\gen_fwft.count_en ),
        .D(\count_value_i[0]_i_1__3_n_0 ),
        .Q(Q[0]),
        .R(SR));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[1] 
       (.C(rd_clk),
        .CE(\gen_fwft.count_en ),
        .D(\count_value_i[1]_i_3_n_0 ),
        .Q(Q[1]),
        .R(SR));
  LUT4 #(
    .INIT(16'h2DD2)) 
    \gen_cdc_pntr.rd_pntr_cdc_dc_inst_i_4 
       (.I0(Q[0]),
        .I1(\grdc.rd_data_count_i_reg[2]_0 [0]),
        .I2(Q[1]),
        .I3(\grdc.rd_data_count_i_reg[2]_0 [1]),
        .O(src_in_bin));
  LUT6 #(
    .INIT(64'h9696699669966969)) 
    \grdc.rd_data_count_i[2]_i_1 
       (.I0(\count_value_i_reg[1]_0 ),
        .I1(\grdc.rd_data_count_i_reg[2] [2]),
        .I2(\grdc.rd_data_count_i_reg[2]_0 [2]),
        .I3(\grdc.rd_data_count_i_reg[2]_0 [1]),
        .I4(Q[1]),
        .I5(\grdc.rd_data_count_i_reg[2] [1]),
        .O(D));
  LUT6 #(
    .INIT(64'h69FF696969690069)) 
    \grdc.rd_data_count_i[4]_i_4 
       (.I0(Q[1]),
        .I1(\grdc.rd_data_count_i_reg[2]_0 [1]),
        .I2(\grdc.rd_data_count_i_reg[2] [1]),
        .I3(\grdc.rd_data_count_i_reg[2]_0 [0]),
        .I4(Q[0]),
        .I5(\grdc.rd_data_count_i_reg[2] [0]),
        .O(\count_value_i_reg[1]_0 ));
endmodule

(* ORIG_REF_NAME = "xpm_counter_updn" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_counter_updn__parameterized0
   (D,
    Q,
    \reg_out_i_reg[2] ,
    E,
    src_in_bin,
    \gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[3] ,
    ram_empty_i,
    rd_en,
    \count_value_i_reg[0]_0 ,
    \grdc.rd_data_count_i_reg[4] ,
    \grdc.rd_data_count_i_reg[4]_0 ,
    \grdc.rd_data_count_i_reg[4]_1 ,
    \src_gray_ff_reg[4] ,
    \count_value_i_reg[4]_0 ,
    rd_clk);
  output [1:0]D;
  output [3:0]Q;
  output [1:0]\reg_out_i_reg[2] ;
  output [0:0]E;
  output [3:0]src_in_bin;
  input [3:0]\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[3] ;
  input ram_empty_i;
  input rd_en;
  input [1:0]\count_value_i_reg[0]_0 ;
  input \grdc.rd_data_count_i_reg[4] ;
  input [3:0]\grdc.rd_data_count_i_reg[4]_0 ;
  input \grdc.rd_data_count_i_reg[4]_1 ;
  input [1:0]\src_gray_ff_reg[4] ;
  input \count_value_i_reg[4]_0 ;
  input rd_clk;

  wire [1:0]D;
  wire [0:0]E;
  wire [3:0]Q;
  wire \count_value_i[0]_i_1__4_n_0 ;
  wire \count_value_i[1]_i_1__3_n_0 ;
  wire \count_value_i[2]_i_1__3_n_0 ;
  wire \count_value_i[3]_i_1__3_n_0 ;
  wire \count_value_i[4]_i_1__0_n_0 ;
  wire [1:0]\count_value_i_reg[0]_0 ;
  wire \count_value_i_reg[4]_0 ;
  wire \count_value_i_reg_n_0_[4] ;
  wire \gen_cdc_pntr.rd_pntr_cdc_dc_inst_i_6_n_0 ;
  wire \gen_pf_ic_rc.gpe_ic.diff_pntr_pe[3]_i_2_n_0 ;
  wire \gen_pf_ic_rc.gpe_ic.diff_pntr_pe[3]_i_3_n_0 ;
  wire [3:0]\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[3] ;
  wire \grdc.rd_data_count_i[4]_i_3_n_0 ;
  wire \grdc.rd_data_count_i[4]_i_6_n_0 ;
  wire \grdc.rd_data_count_i_reg[4] ;
  wire [3:0]\grdc.rd_data_count_i_reg[4]_0 ;
  wire \grdc.rd_data_count_i_reg[4]_1 ;
  wire ram_empty_i;
  wire rd_clk;
  wire rd_en;
  wire [1:0]\reg_out_i_reg[2] ;
  wire [1:0]\src_gray_ff_reg[4] ;
  wire [3:0]src_in_bin;

  (* SOFT_HLUTNM = "soft_lutpair57" *) 
  LUT4 #(
    .INIT(16'h10EF)) 
    \count_value_i[0]_i_1__4 
       (.I0(rd_en),
        .I1(\count_value_i_reg[0]_0 [0]),
        .I2(\count_value_i_reg[0]_0 [1]),
        .I3(Q[0]),
        .O(\count_value_i[0]_i_1__4_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair57" *) 
  LUT5 #(
    .INIT(32'h02FFFD00)) 
    \count_value_i[1]_i_1__3 
       (.I0(\count_value_i_reg[0]_0 [1]),
        .I1(\count_value_i_reg[0]_0 [0]),
        .I2(rd_en),
        .I3(Q[0]),
        .I4(Q[1]),
        .O(\count_value_i[1]_i_1__3_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair58" *) 
  LUT3 #(
    .INIT(8'h78)) 
    \count_value_i[2]_i_1__3 
       (.I0(Q[0]),
        .I1(Q[1]),
        .I2(Q[2]),
        .O(\count_value_i[2]_i_1__3_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair56" *) 
  LUT4 #(
    .INIT(16'h7F80)) 
    \count_value_i[3]_i_1__3 
       (.I0(Q[1]),
        .I1(Q[0]),
        .I2(Q[2]),
        .I3(Q[3]),
        .O(\count_value_i[3]_i_1__3_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair56" *) 
  LUT5 #(
    .INIT(32'h7FFF8000)) 
    \count_value_i[4]_i_1__0 
       (.I0(Q[2]),
        .I1(Q[0]),
        .I2(Q[1]),
        .I3(Q[3]),
        .I4(\count_value_i_reg_n_0_[4] ),
        .O(\count_value_i[4]_i_1__0_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[0] 
       (.C(rd_clk),
        .CE(E),
        .D(\count_value_i[0]_i_1__4_n_0 ),
        .Q(Q[0]),
        .R(\count_value_i_reg[4]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[1] 
       (.C(rd_clk),
        .CE(E),
        .D(\count_value_i[1]_i_1__3_n_0 ),
        .Q(Q[1]),
        .R(\count_value_i_reg[4]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[2] 
       (.C(rd_clk),
        .CE(E),
        .D(\count_value_i[2]_i_1__3_n_0 ),
        .Q(Q[2]),
        .R(\count_value_i_reg[4]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[3] 
       (.C(rd_clk),
        .CE(E),
        .D(\count_value_i[3]_i_1__3_n_0 ),
        .Q(Q[3]),
        .R(\count_value_i_reg[4]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[4] 
       (.C(rd_clk),
        .CE(E),
        .D(\count_value_i[4]_i_1__0_n_0 ),
        .Q(\count_value_i_reg_n_0_[4] ),
        .R(\count_value_i_reg[4]_0 ));
  LUT6 #(
    .INIT(64'hFFFFEAFE00001501)) 
    \gen_cdc_pntr.rd_pntr_cdc_dc_inst_i_1 
       (.I0(Q[3]),
        .I1(\gen_cdc_pntr.rd_pntr_cdc_dc_inst_i_6_n_0 ),
        .I2(Q[1]),
        .I3(\src_gray_ff_reg[4] [1]),
        .I4(Q[2]),
        .I5(\count_value_i_reg_n_0_[4] ),
        .O(src_in_bin[3]));
  LUT6 #(
    .INIT(64'hFBFBBAFB04044504)) 
    \gen_cdc_pntr.rd_pntr_cdc_dc_inst_i_2 
       (.I0(Q[2]),
        .I1(\src_gray_ff_reg[4] [1]),
        .I2(Q[1]),
        .I3(\src_gray_ff_reg[4] [0]),
        .I4(Q[0]),
        .I5(Q[3]),
        .O(src_in_bin[2]));
  (* SOFT_HLUTNM = "soft_lutpair58" *) 
  LUT5 #(
    .INIT(32'hB0FB4F04)) 
    \gen_cdc_pntr.rd_pntr_cdc_dc_inst_i_3 
       (.I0(Q[0]),
        .I1(\src_gray_ff_reg[4] [0]),
        .I2(Q[1]),
        .I3(\src_gray_ff_reg[4] [1]),
        .I4(Q[2]),
        .O(src_in_bin[1]));
  (* SOFT_HLUTNM = "soft_lutpair60" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \gen_cdc_pntr.rd_pntr_cdc_dc_inst_i_5 
       (.I0(Q[0]),
        .I1(\src_gray_ff_reg[4] [0]),
        .O(src_in_bin[0]));
  (* SOFT_HLUTNM = "soft_lutpair60" *) 
  LUT2 #(
    .INIT(4'hB)) 
    \gen_cdc_pntr.rd_pntr_cdc_dc_inst_i_6 
       (.I0(Q[0]),
        .I1(\src_gray_ff_reg[4] [0]),
        .O(\gen_cdc_pntr.rd_pntr_cdc_dc_inst_i_6_n_0 ));
  LUT5 #(
    .INIT(32'h718E8E71)) 
    \gen_pf_ic_rc.gpe_ic.diff_pntr_pe[2]_i_1 
       (.I0(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe[3]_i_2_n_0 ),
        .I1(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[3] [1]),
        .I2(Q[1]),
        .I3(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[3] [2]),
        .I4(Q[2]),
        .O(D[0]));
  LUT6 #(
    .INIT(64'h2BFF002BD400FFD4)) 
    \gen_pf_ic_rc.gpe_ic.diff_pntr_pe[3]_i_1 
       (.I0(Q[1]),
        .I1(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[3] [1]),
        .I2(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe[3]_i_2_n_0 ),
        .I3(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[3] [2]),
        .I4(Q[2]),
        .I5(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe[3]_i_3_n_0 ),
        .O(D[1]));
  LUT6 #(
    .INIT(64'hDDDFDDDD44454444)) 
    \gen_pf_ic_rc.gpe_ic.diff_pntr_pe[3]_i_2 
       (.I0(Q[0]),
        .I1(ram_empty_i),
        .I2(rd_en),
        .I3(\count_value_i_reg[0]_0 [0]),
        .I4(\count_value_i_reg[0]_0 [1]),
        .I5(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[3] [0]),
        .O(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe[3]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair59" *) 
  LUT2 #(
    .INIT(4'h9)) 
    \gen_pf_ic_rc.gpe_ic.diff_pntr_pe[3]_i_3 
       (.I0(Q[3]),
        .I1(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[3] [3]),
        .O(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe[3]_i_3_n_0 ));
  LUT4 #(
    .INIT(16'h00FD)) 
    \gen_sdpram.xpm_memory_base_inst_i_2 
       (.I0(\count_value_i_reg[0]_0 [1]),
        .I1(\count_value_i_reg[0]_0 [0]),
        .I2(rd_en),
        .I3(ram_empty_i),
        .O(E));
  LUT6 #(
    .INIT(64'h1EE1788787781EE1)) 
    \grdc.rd_data_count_i[3]_i_1 
       (.I0(\grdc.rd_data_count_i_reg[4] ),
        .I1(\grdc.rd_data_count_i[4]_i_3_n_0 ),
        .I2(\grdc.rd_data_count_i_reg[4]_0 [2]),
        .I3(Q[3]),
        .I4(\grdc.rd_data_count_i_reg[4]_0 [1]),
        .I5(Q[2]),
        .O(\reg_out_i_reg[2] [0]));
  LUT6 #(
    .INIT(64'h1701FF7FE8FE0080)) 
    \grdc.rd_data_count_i[4]_i_2 
       (.I0(\grdc.rd_data_count_i[4]_i_3_n_0 ),
        .I1(\grdc.rd_data_count_i_reg[4] ),
        .I2(\grdc.rd_data_count_i_reg[4]_0 [1]),
        .I3(Q[2]),
        .I4(\grdc.rd_data_count_i_reg[4]_1 ),
        .I5(\grdc.rd_data_count_i[4]_i_6_n_0 ),
        .O(\reg_out_i_reg[2] [1]));
  LUT3 #(
    .INIT(8'hD4)) 
    \grdc.rd_data_count_i[4]_i_3 
       (.I0(Q[1]),
        .I1(\src_gray_ff_reg[4] [1]),
        .I2(\grdc.rd_data_count_i_reg[4]_0 [0]),
        .O(\grdc.rd_data_count_i[4]_i_3_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair59" *) 
  LUT4 #(
    .INIT(16'hB44B)) 
    \grdc.rd_data_count_i[4]_i_6 
       (.I0(Q[3]),
        .I1(\grdc.rd_data_count_i_reg[4]_0 [2]),
        .I2(\count_value_i_reg_n_0_[4] ),
        .I3(\grdc.rd_data_count_i_reg[4]_0 [3]),
        .O(\grdc.rd_data_count_i[4]_i_6_n_0 ));
endmodule

(* ORIG_REF_NAME = "xpm_counter_updn" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_counter_updn__parameterized0_12
   (D,
    Q,
    \gwdc.wr_data_count_i_reg[2] ,
    wrst_busy,
    E,
    wr_clk);
  output [0:0]D;
  output [4:0]Q;
  input [2:0]\gwdc.wr_data_count_i_reg[2] ;
  input wrst_busy;
  input [0:0]E;
  input wr_clk;

  wire [0:0]D;
  wire [0:0]E;
  wire [4:0]Q;
  wire \count_value_i[0]_i_1__1_n_0 ;
  wire \count_value_i[1]_i_1__1_n_0 ;
  wire \count_value_i[2]_i_1__1_n_0 ;
  wire \count_value_i[3]_i_1__1_n_0 ;
  wire \count_value_i[4]_i_1_n_0 ;
  wire [2:0]\gwdc.wr_data_count_i_reg[2] ;
  wire wr_clk;
  wire wrst_busy;

  LUT1 #(
    .INIT(2'h1)) 
    \count_value_i[0]_i_1__1 
       (.I0(Q[0]),
        .O(\count_value_i[0]_i_1__1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair14" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \count_value_i[1]_i_1__1 
       (.I0(Q[0]),
        .I1(Q[1]),
        .O(\count_value_i[1]_i_1__1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair14" *) 
  LUT3 #(
    .INIT(8'h78)) 
    \count_value_i[2]_i_1__1 
       (.I0(Q[0]),
        .I1(Q[1]),
        .I2(Q[2]),
        .O(\count_value_i[2]_i_1__1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair13" *) 
  LUT4 #(
    .INIT(16'h7F80)) 
    \count_value_i[3]_i_1__1 
       (.I0(Q[1]),
        .I1(Q[0]),
        .I2(Q[2]),
        .I3(Q[3]),
        .O(\count_value_i[3]_i_1__1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair13" *) 
  LUT5 #(
    .INIT(32'h7FFF8000)) 
    \count_value_i[4]_i_1 
       (.I0(Q[2]),
        .I1(Q[0]),
        .I2(Q[1]),
        .I3(Q[3]),
        .I4(Q[4]),
        .O(\count_value_i[4]_i_1_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[0] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[0]_i_1__1_n_0 ),
        .Q(Q[0]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[1] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[1]_i_1__1_n_0 ),
        .Q(Q[1]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[2] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[2]_i_1__1_n_0 ),
        .Q(Q[2]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[3] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[3]_i_1__1_n_0 ),
        .Q(Q[3]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[4] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[4]_i_1_n_0 ),
        .Q(Q[4]),
        .R(wrst_busy));
  LUT6 #(
    .INIT(64'h4F04B0FBB0FB4F04)) 
    \gwdc.wr_data_count_i[2]_i_1 
       (.I0(Q[0]),
        .I1(\gwdc.wr_data_count_i_reg[2] [0]),
        .I2(Q[1]),
        .I3(\gwdc.wr_data_count_i_reg[2] [1]),
        .I4(\gwdc.wr_data_count_i_reg[2] [2]),
        .I5(Q[2]),
        .O(D));
endmodule

(* ORIG_REF_NAME = "xpm_counter_updn" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_counter_updn__parameterized0_2
   (D,
    Q,
    \gwdc.wr_data_count_i_reg[2] ,
    wrst_busy,
    E,
    wr_clk);
  output [0:0]D;
  output [4:0]Q;
  input [2:0]\gwdc.wr_data_count_i_reg[2] ;
  input wrst_busy;
  input [0:0]E;
  input wr_clk;

  wire [0:0]D;
  wire [0:0]E;
  wire [4:0]Q;
  wire \count_value_i[0]_i_1__1_n_0 ;
  wire \count_value_i[1]_i_1__1_n_0 ;
  wire \count_value_i[2]_i_1__1_n_0 ;
  wire \count_value_i[3]_i_1__1_n_0 ;
  wire \count_value_i[4]_i_1_n_0 ;
  wire [2:0]\gwdc.wr_data_count_i_reg[2] ;
  wire wr_clk;
  wire wrst_busy;

  LUT1 #(
    .INIT(2'h1)) 
    \count_value_i[0]_i_1__1 
       (.I0(Q[0]),
        .O(\count_value_i[0]_i_1__1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair63" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \count_value_i[1]_i_1__1 
       (.I0(Q[0]),
        .I1(Q[1]),
        .O(\count_value_i[1]_i_1__1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair63" *) 
  LUT3 #(
    .INIT(8'h78)) 
    \count_value_i[2]_i_1__1 
       (.I0(Q[0]),
        .I1(Q[1]),
        .I2(Q[2]),
        .O(\count_value_i[2]_i_1__1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair62" *) 
  LUT4 #(
    .INIT(16'h7F80)) 
    \count_value_i[3]_i_1__1 
       (.I0(Q[1]),
        .I1(Q[0]),
        .I2(Q[2]),
        .I3(Q[3]),
        .O(\count_value_i[3]_i_1__1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair62" *) 
  LUT5 #(
    .INIT(32'h7FFF8000)) 
    \count_value_i[4]_i_1 
       (.I0(Q[2]),
        .I1(Q[0]),
        .I2(Q[1]),
        .I3(Q[3]),
        .I4(Q[4]),
        .O(\count_value_i[4]_i_1_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[0] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[0]_i_1__1_n_0 ),
        .Q(Q[0]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[1] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[1]_i_1__1_n_0 ),
        .Q(Q[1]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[2] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[2]_i_1__1_n_0 ),
        .Q(Q[2]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[3] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[3]_i_1__1_n_0 ),
        .Q(Q[3]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[4] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[4]_i_1_n_0 ),
        .Q(Q[4]),
        .R(wrst_busy));
  LUT6 #(
    .INIT(64'h4F04B0FBB0FB4F04)) 
    \gwdc.wr_data_count_i[2]_i_1 
       (.I0(Q[0]),
        .I1(\gwdc.wr_data_count_i_reg[2] [0]),
        .I2(Q[1]),
        .I3(\gwdc.wr_data_count_i_reg[2] [1]),
        .I4(\gwdc.wr_data_count_i_reg[2] [2]),
        .I5(Q[2]),
        .O(D));
endmodule

(* ORIG_REF_NAME = "xpm_counter_updn" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_counter_updn__parameterized0_9
   (D,
    Q,
    \reg_out_i_reg[2] ,
    E,
    src_in_bin,
    \gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[3] ,
    ram_empty_i,
    rd_en,
    \count_value_i_reg[0]_0 ,
    \grdc.rd_data_count_i_reg[4] ,
    \grdc.rd_data_count_i_reg[4]_0 ,
    \grdc.rd_data_count_i_reg[4]_1 ,
    \src_gray_ff_reg[4] ,
    \count_value_i_reg[4]_0 ,
    rd_clk);
  output [1:0]D;
  output [3:0]Q;
  output [1:0]\reg_out_i_reg[2] ;
  output [0:0]E;
  output [3:0]src_in_bin;
  input [3:0]\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[3] ;
  input ram_empty_i;
  input rd_en;
  input [1:0]\count_value_i_reg[0]_0 ;
  input \grdc.rd_data_count_i_reg[4] ;
  input [3:0]\grdc.rd_data_count_i_reg[4]_0 ;
  input \grdc.rd_data_count_i_reg[4]_1 ;
  input [1:0]\src_gray_ff_reg[4] ;
  input \count_value_i_reg[4]_0 ;
  input rd_clk;

  wire [1:0]D;
  wire [0:0]E;
  wire [3:0]Q;
  wire \count_value_i[0]_i_1__4_n_0 ;
  wire \count_value_i[1]_i_1__3_n_0 ;
  wire \count_value_i[2]_i_1__3_n_0 ;
  wire \count_value_i[3]_i_1__3_n_0 ;
  wire \count_value_i[4]_i_1__0_n_0 ;
  wire [1:0]\count_value_i_reg[0]_0 ;
  wire \count_value_i_reg[4]_0 ;
  wire \count_value_i_reg_n_0_[4] ;
  wire \gen_cdc_pntr.rd_pntr_cdc_dc_inst_i_6_n_0 ;
  wire \gen_pf_ic_rc.gpe_ic.diff_pntr_pe[3]_i_2_n_0 ;
  wire \gen_pf_ic_rc.gpe_ic.diff_pntr_pe[3]_i_3_n_0 ;
  wire [3:0]\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[3] ;
  wire \grdc.rd_data_count_i[4]_i_3_n_0 ;
  wire \grdc.rd_data_count_i[4]_i_6_n_0 ;
  wire \grdc.rd_data_count_i_reg[4] ;
  wire [3:0]\grdc.rd_data_count_i_reg[4]_0 ;
  wire \grdc.rd_data_count_i_reg[4]_1 ;
  wire ram_empty_i;
  wire rd_clk;
  wire rd_en;
  wire [1:0]\reg_out_i_reg[2] ;
  wire [1:0]\src_gray_ff_reg[4] ;
  wire [3:0]src_in_bin;

  (* SOFT_HLUTNM = "soft_lutpair8" *) 
  LUT4 #(
    .INIT(16'h10EF)) 
    \count_value_i[0]_i_1__4 
       (.I0(rd_en),
        .I1(\count_value_i_reg[0]_0 [0]),
        .I2(\count_value_i_reg[0]_0 [1]),
        .I3(Q[0]),
        .O(\count_value_i[0]_i_1__4_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair8" *) 
  LUT5 #(
    .INIT(32'h02FFFD00)) 
    \count_value_i[1]_i_1__3 
       (.I0(\count_value_i_reg[0]_0 [1]),
        .I1(\count_value_i_reg[0]_0 [0]),
        .I2(rd_en),
        .I3(Q[0]),
        .I4(Q[1]),
        .O(\count_value_i[1]_i_1__3_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair9" *) 
  LUT3 #(
    .INIT(8'h78)) 
    \count_value_i[2]_i_1__3 
       (.I0(Q[0]),
        .I1(Q[1]),
        .I2(Q[2]),
        .O(\count_value_i[2]_i_1__3_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair7" *) 
  LUT4 #(
    .INIT(16'h7F80)) 
    \count_value_i[3]_i_1__3 
       (.I0(Q[1]),
        .I1(Q[0]),
        .I2(Q[2]),
        .I3(Q[3]),
        .O(\count_value_i[3]_i_1__3_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair7" *) 
  LUT5 #(
    .INIT(32'h7FFF8000)) 
    \count_value_i[4]_i_1__0 
       (.I0(Q[2]),
        .I1(Q[0]),
        .I2(Q[1]),
        .I3(Q[3]),
        .I4(\count_value_i_reg_n_0_[4] ),
        .O(\count_value_i[4]_i_1__0_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[0] 
       (.C(rd_clk),
        .CE(E),
        .D(\count_value_i[0]_i_1__4_n_0 ),
        .Q(Q[0]),
        .R(\count_value_i_reg[4]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[1] 
       (.C(rd_clk),
        .CE(E),
        .D(\count_value_i[1]_i_1__3_n_0 ),
        .Q(Q[1]),
        .R(\count_value_i_reg[4]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[2] 
       (.C(rd_clk),
        .CE(E),
        .D(\count_value_i[2]_i_1__3_n_0 ),
        .Q(Q[2]),
        .R(\count_value_i_reg[4]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[3] 
       (.C(rd_clk),
        .CE(E),
        .D(\count_value_i[3]_i_1__3_n_0 ),
        .Q(Q[3]),
        .R(\count_value_i_reg[4]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[4] 
       (.C(rd_clk),
        .CE(E),
        .D(\count_value_i[4]_i_1__0_n_0 ),
        .Q(\count_value_i_reg_n_0_[4] ),
        .R(\count_value_i_reg[4]_0 ));
  LUT6 #(
    .INIT(64'hFFFFEAFE00001501)) 
    \gen_cdc_pntr.rd_pntr_cdc_dc_inst_i_1 
       (.I0(Q[3]),
        .I1(\gen_cdc_pntr.rd_pntr_cdc_dc_inst_i_6_n_0 ),
        .I2(Q[1]),
        .I3(\src_gray_ff_reg[4] [1]),
        .I4(Q[2]),
        .I5(\count_value_i_reg_n_0_[4] ),
        .O(src_in_bin[3]));
  LUT6 #(
    .INIT(64'hFBFBBAFB04044504)) 
    \gen_cdc_pntr.rd_pntr_cdc_dc_inst_i_2 
       (.I0(Q[2]),
        .I1(\src_gray_ff_reg[4] [1]),
        .I2(Q[1]),
        .I3(\src_gray_ff_reg[4] [0]),
        .I4(Q[0]),
        .I5(Q[3]),
        .O(src_in_bin[2]));
  (* SOFT_HLUTNM = "soft_lutpair9" *) 
  LUT5 #(
    .INIT(32'hB0FB4F04)) 
    \gen_cdc_pntr.rd_pntr_cdc_dc_inst_i_3 
       (.I0(Q[0]),
        .I1(\src_gray_ff_reg[4] [0]),
        .I2(Q[1]),
        .I3(\src_gray_ff_reg[4] [1]),
        .I4(Q[2]),
        .O(src_in_bin[1]));
  (* SOFT_HLUTNM = "soft_lutpair11" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \gen_cdc_pntr.rd_pntr_cdc_dc_inst_i_5 
       (.I0(Q[0]),
        .I1(\src_gray_ff_reg[4] [0]),
        .O(src_in_bin[0]));
  (* SOFT_HLUTNM = "soft_lutpair11" *) 
  LUT2 #(
    .INIT(4'hB)) 
    \gen_cdc_pntr.rd_pntr_cdc_dc_inst_i_6 
       (.I0(Q[0]),
        .I1(\src_gray_ff_reg[4] [0]),
        .O(\gen_cdc_pntr.rd_pntr_cdc_dc_inst_i_6_n_0 ));
  LUT5 #(
    .INIT(32'h718E8E71)) 
    \gen_pf_ic_rc.gpe_ic.diff_pntr_pe[2]_i_1 
       (.I0(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe[3]_i_2_n_0 ),
        .I1(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[3] [1]),
        .I2(Q[1]),
        .I3(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[3] [2]),
        .I4(Q[2]),
        .O(D[0]));
  LUT6 #(
    .INIT(64'h2BFF002BD400FFD4)) 
    \gen_pf_ic_rc.gpe_ic.diff_pntr_pe[3]_i_1 
       (.I0(Q[1]),
        .I1(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[3] [1]),
        .I2(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe[3]_i_2_n_0 ),
        .I3(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[3] [2]),
        .I4(Q[2]),
        .I5(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe[3]_i_3_n_0 ),
        .O(D[1]));
  LUT6 #(
    .INIT(64'hDDDFDDDD44454444)) 
    \gen_pf_ic_rc.gpe_ic.diff_pntr_pe[3]_i_2 
       (.I0(Q[0]),
        .I1(ram_empty_i),
        .I2(rd_en),
        .I3(\count_value_i_reg[0]_0 [0]),
        .I4(\count_value_i_reg[0]_0 [1]),
        .I5(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[3] [0]),
        .O(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe[3]_i_2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair10" *) 
  LUT2 #(
    .INIT(4'h9)) 
    \gen_pf_ic_rc.gpe_ic.diff_pntr_pe[3]_i_3 
       (.I0(Q[3]),
        .I1(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[3] [3]),
        .O(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe[3]_i_3_n_0 ));
  LUT4 #(
    .INIT(16'h00FD)) 
    \gen_sdpram.xpm_memory_base_inst_i_2 
       (.I0(\count_value_i_reg[0]_0 [1]),
        .I1(\count_value_i_reg[0]_0 [0]),
        .I2(rd_en),
        .I3(ram_empty_i),
        .O(E));
  LUT6 #(
    .INIT(64'h1EE1788787781EE1)) 
    \grdc.rd_data_count_i[3]_i_1 
       (.I0(\grdc.rd_data_count_i_reg[4] ),
        .I1(\grdc.rd_data_count_i[4]_i_3_n_0 ),
        .I2(\grdc.rd_data_count_i_reg[4]_0 [2]),
        .I3(Q[3]),
        .I4(\grdc.rd_data_count_i_reg[4]_0 [1]),
        .I5(Q[2]),
        .O(\reg_out_i_reg[2] [0]));
  LUT6 #(
    .INIT(64'h1701FF7FE8FE0080)) 
    \grdc.rd_data_count_i[4]_i_2 
       (.I0(\grdc.rd_data_count_i[4]_i_3_n_0 ),
        .I1(\grdc.rd_data_count_i_reg[4] ),
        .I2(\grdc.rd_data_count_i_reg[4]_0 [1]),
        .I3(Q[2]),
        .I4(\grdc.rd_data_count_i_reg[4]_1 ),
        .I5(\grdc.rd_data_count_i[4]_i_6_n_0 ),
        .O(\reg_out_i_reg[2] [1]));
  LUT3 #(
    .INIT(8'hD4)) 
    \grdc.rd_data_count_i[4]_i_3 
       (.I0(Q[1]),
        .I1(\src_gray_ff_reg[4] [1]),
        .I2(\grdc.rd_data_count_i_reg[4]_0 [0]),
        .O(\grdc.rd_data_count_i[4]_i_3_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair10" *) 
  LUT4 #(
    .INIT(16'hB44B)) 
    \grdc.rd_data_count_i[4]_i_6 
       (.I0(Q[3]),
        .I1(\grdc.rd_data_count_i_reg[4]_0 [2]),
        .I2(\count_value_i_reg_n_0_[4] ),
        .I3(\grdc.rd_data_count_i_reg[4]_0 [3]),
        .O(\grdc.rd_data_count_i[4]_i_6_n_0 ));
endmodule

(* ORIG_REF_NAME = "xpm_counter_updn" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_counter_updn__parameterized1
   (Q,
    \count_value_i_reg[1]_0 ,
    rd_en,
    \count_value_i_reg[0]_0 ,
    E,
    rd_clk);
  output [3:0]Q;
  input [1:0]\count_value_i_reg[1]_0 ;
  input rd_en;
  input \count_value_i_reg[0]_0 ;
  input [0:0]E;
  input rd_clk;

  wire [0:0]E;
  wire [3:0]Q;
  wire \count_value_i[0]_i_1__2_n_0 ;
  wire \count_value_i[1]_i_1__2_n_0 ;
  wire \count_value_i[2]_i_1__2_n_0 ;
  wire \count_value_i[3]_i_1__2_n_0 ;
  wire \count_value_i_reg[0]_0 ;
  wire [1:0]\count_value_i_reg[1]_0 ;
  wire rd_clk;
  wire rd_en;

  LUT4 #(
    .INIT(16'h10EF)) 
    \count_value_i[0]_i_1__2 
       (.I0(rd_en),
        .I1(\count_value_i_reg[1]_0 [0]),
        .I2(\count_value_i_reg[1]_0 [1]),
        .I3(Q[0]),
        .O(\count_value_i[0]_i_1__2_n_0 ));
  LUT5 #(
    .INIT(32'h02FFFD00)) 
    \count_value_i[1]_i_1__2 
       (.I0(\count_value_i_reg[1]_0 [1]),
        .I1(\count_value_i_reg[1]_0 [0]),
        .I2(rd_en),
        .I3(Q[0]),
        .I4(Q[1]),
        .O(\count_value_i[1]_i_1__2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair61" *) 
  LUT3 #(
    .INIT(8'h78)) 
    \count_value_i[2]_i_1__2 
       (.I0(Q[0]),
        .I1(Q[1]),
        .I2(Q[2]),
        .O(\count_value_i[2]_i_1__2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair61" *) 
  LUT4 #(
    .INIT(16'h7F80)) 
    \count_value_i[3]_i_1__2 
       (.I0(Q[1]),
        .I1(Q[0]),
        .I2(Q[2]),
        .I3(Q[3]),
        .O(\count_value_i[3]_i_1__2_n_0 ));
  FDSE #(
    .INIT(1'b1)) 
    \count_value_i_reg[0] 
       (.C(rd_clk),
        .CE(E),
        .D(\count_value_i[0]_i_1__2_n_0 ),
        .Q(Q[0]),
        .S(\count_value_i_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[1] 
       (.C(rd_clk),
        .CE(E),
        .D(\count_value_i[1]_i_1__2_n_0 ),
        .Q(Q[1]),
        .R(\count_value_i_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[2] 
       (.C(rd_clk),
        .CE(E),
        .D(\count_value_i[2]_i_1__2_n_0 ),
        .Q(Q[2]),
        .R(\count_value_i_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[3] 
       (.C(rd_clk),
        .CE(E),
        .D(\count_value_i[3]_i_1__2_n_0 ),
        .Q(Q[3]),
        .R(\count_value_i_reg[0]_0 ));
endmodule

(* ORIG_REF_NAME = "xpm_counter_updn" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_counter_updn__parameterized1_10
   (Q,
    \count_value_i_reg[1]_0 ,
    rd_en,
    \count_value_i_reg[0]_0 ,
    E,
    rd_clk);
  output [3:0]Q;
  input [1:0]\count_value_i_reg[1]_0 ;
  input rd_en;
  input \count_value_i_reg[0]_0 ;
  input [0:0]E;
  input rd_clk;

  wire [0:0]E;
  wire [3:0]Q;
  wire \count_value_i[0]_i_1__2_n_0 ;
  wire \count_value_i[1]_i_1__2_n_0 ;
  wire \count_value_i[2]_i_1__2_n_0 ;
  wire \count_value_i[3]_i_1__2_n_0 ;
  wire \count_value_i_reg[0]_0 ;
  wire [1:0]\count_value_i_reg[1]_0 ;
  wire rd_clk;
  wire rd_en;

  LUT4 #(
    .INIT(16'h10EF)) 
    \count_value_i[0]_i_1__2 
       (.I0(rd_en),
        .I1(\count_value_i_reg[1]_0 [0]),
        .I2(\count_value_i_reg[1]_0 [1]),
        .I3(Q[0]),
        .O(\count_value_i[0]_i_1__2_n_0 ));
  LUT5 #(
    .INIT(32'h02FFFD00)) 
    \count_value_i[1]_i_1__2 
       (.I0(\count_value_i_reg[1]_0 [1]),
        .I1(\count_value_i_reg[1]_0 [0]),
        .I2(rd_en),
        .I3(Q[0]),
        .I4(Q[1]),
        .O(\count_value_i[1]_i_1__2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair12" *) 
  LUT3 #(
    .INIT(8'h78)) 
    \count_value_i[2]_i_1__2 
       (.I0(Q[0]),
        .I1(Q[1]),
        .I2(Q[2]),
        .O(\count_value_i[2]_i_1__2_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair12" *) 
  LUT4 #(
    .INIT(16'h7F80)) 
    \count_value_i[3]_i_1__2 
       (.I0(Q[1]),
        .I1(Q[0]),
        .I2(Q[2]),
        .I3(Q[3]),
        .O(\count_value_i[3]_i_1__2_n_0 ));
  FDSE #(
    .INIT(1'b1)) 
    \count_value_i_reg[0] 
       (.C(rd_clk),
        .CE(E),
        .D(\count_value_i[0]_i_1__2_n_0 ),
        .Q(Q[0]),
        .S(\count_value_i_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[1] 
       (.C(rd_clk),
        .CE(E),
        .D(\count_value_i[1]_i_1__2_n_0 ),
        .Q(Q[1]),
        .R(\count_value_i_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[2] 
       (.C(rd_clk),
        .CE(E),
        .D(\count_value_i[2]_i_1__2_n_0 ),
        .Q(Q[2]),
        .R(\count_value_i_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[3] 
       (.C(rd_clk),
        .CE(E),
        .D(\count_value_i[3]_i_1__2_n_0 ),
        .Q(Q[3]),
        .R(\count_value_i_reg[0]_0 ));
endmodule

(* ORIG_REF_NAME = "xpm_counter_updn" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_counter_updn__parameterized1_13
   (Q,
    \count_value_i_reg[3]_0 ,
    \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] ,
    wrst_busy,
    E,
    wr_clk);
  output [3:0]Q;
  output \count_value_i_reg[3]_0 ;
  input [0:0]\gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] ;
  input wrst_busy;
  input [0:0]E;
  input wr_clk;

  wire [0:0]E;
  wire [3:0]Q;
  wire \count_value_i[0]_i_1__0_n_0 ;
  wire \count_value_i[1]_i_1__0_n_0 ;
  wire \count_value_i[2]_i_1__0_n_0 ;
  wire \count_value_i[3]_i_1__0_n_0 ;
  wire \count_value_i_reg[3]_0 ;
  wire [0:0]\gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] ;
  wire wr_clk;
  wire wrst_busy;

  LUT1 #(
    .INIT(2'h1)) 
    \count_value_i[0]_i_1__0 
       (.I0(Q[0]),
        .O(\count_value_i[0]_i_1__0_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \count_value_i[1]_i_1__0 
       (.I0(Q[0]),
        .I1(Q[1]),
        .O(\count_value_i[1]_i_1__0_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair15" *) 
  LUT3 #(
    .INIT(8'h78)) 
    \count_value_i[2]_i_1__0 
       (.I0(Q[0]),
        .I1(Q[1]),
        .I2(Q[2]),
        .O(\count_value_i[2]_i_1__0_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair15" *) 
  LUT4 #(
    .INIT(16'h7F80)) 
    \count_value_i[3]_i_1__0 
       (.I0(Q[1]),
        .I1(Q[0]),
        .I2(Q[2]),
        .I3(Q[3]),
        .O(\count_value_i[3]_i_1__0_n_0 ));
  FDSE #(
    .INIT(1'b1)) 
    \count_value_i_reg[0] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[0]_i_1__0_n_0 ),
        .Q(Q[0]),
        .S(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[1] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[1]_i_1__0_n_0 ),
        .Q(Q[1]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[2] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[2]_i_1__0_n_0 ),
        .Q(Q[2]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[3] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[3]_i_1__0_n_0 ),
        .Q(Q[3]),
        .R(wrst_busy));
  LUT2 #(
    .INIT(4'h9)) 
    \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q[4]_i_3 
       (.I0(Q[3]),
        .I1(\gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] ),
        .O(\count_value_i_reg[3]_0 ));
endmodule

(* ORIG_REF_NAME = "xpm_counter_updn" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_counter_updn__parameterized1_3
   (Q,
    \count_value_i_reg[3]_0 ,
    \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] ,
    wrst_busy,
    E,
    wr_clk);
  output [3:0]Q;
  output \count_value_i_reg[3]_0 ;
  input [0:0]\gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] ;
  input wrst_busy;
  input [0:0]E;
  input wr_clk;

  wire [0:0]E;
  wire [3:0]Q;
  wire \count_value_i[0]_i_1__0_n_0 ;
  wire \count_value_i[1]_i_1__0_n_0 ;
  wire \count_value_i[2]_i_1__0_n_0 ;
  wire \count_value_i[3]_i_1__0_n_0 ;
  wire \count_value_i_reg[3]_0 ;
  wire [0:0]\gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] ;
  wire wr_clk;
  wire wrst_busy;

  LUT1 #(
    .INIT(2'h1)) 
    \count_value_i[0]_i_1__0 
       (.I0(Q[0]),
        .O(\count_value_i[0]_i_1__0_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \count_value_i[1]_i_1__0 
       (.I0(Q[0]),
        .I1(Q[1]),
        .O(\count_value_i[1]_i_1__0_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair64" *) 
  LUT3 #(
    .INIT(8'h78)) 
    \count_value_i[2]_i_1__0 
       (.I0(Q[0]),
        .I1(Q[1]),
        .I2(Q[2]),
        .O(\count_value_i[2]_i_1__0_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair64" *) 
  LUT4 #(
    .INIT(16'h7F80)) 
    \count_value_i[3]_i_1__0 
       (.I0(Q[1]),
        .I1(Q[0]),
        .I2(Q[2]),
        .I3(Q[3]),
        .O(\count_value_i[3]_i_1__0_n_0 ));
  FDSE #(
    .INIT(1'b1)) 
    \count_value_i_reg[0] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[0]_i_1__0_n_0 ),
        .Q(Q[0]),
        .S(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[1] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[1]_i_1__0_n_0 ),
        .Q(Q[1]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[2] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[2]_i_1__0_n_0 ),
        .Q(Q[2]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[3] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[3]_i_1__0_n_0 ),
        .Q(Q[3]),
        .R(wrst_busy));
  LUT2 #(
    .INIT(4'h9)) 
    \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q[4]_i_3 
       (.I0(Q[3]),
        .I1(\gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] ),
        .O(\count_value_i_reg[3]_0 ));
endmodule

(* ORIG_REF_NAME = "xpm_counter_updn" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_counter_updn__parameterized2
   (Q,
    wrst_busy,
    E,
    wr_clk);
  output [3:0]Q;
  input wrst_busy;
  input [0:0]E;
  input wr_clk;

  wire [0:0]E;
  wire [3:0]Q;
  wire \count_value_i[0]_i_1_n_0 ;
  wire \count_value_i[1]_i_1_n_0 ;
  wire \count_value_i[2]_i_1_n_0 ;
  wire \count_value_i[3]_i_1_n_0 ;
  wire wr_clk;
  wire wrst_busy;

  LUT1 #(
    .INIT(2'h1)) 
    \count_value_i[0]_i_1 
       (.I0(Q[0]),
        .O(\count_value_i[0]_i_1_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \count_value_i[1]_i_1 
       (.I0(Q[0]),
        .I1(Q[1]),
        .O(\count_value_i[1]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair65" *) 
  LUT3 #(
    .INIT(8'h78)) 
    \count_value_i[2]_i_1 
       (.I0(Q[0]),
        .I1(Q[1]),
        .I2(Q[2]),
        .O(\count_value_i[2]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair65" *) 
  LUT4 #(
    .INIT(16'h7F80)) 
    \count_value_i[3]_i_1 
       (.I0(Q[1]),
        .I1(Q[0]),
        .I2(Q[2]),
        .I3(Q[3]),
        .O(\count_value_i[3]_i_1_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[0] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[0]_i_1_n_0 ),
        .Q(Q[0]),
        .R(wrst_busy));
  FDSE #(
    .INIT(1'b1)) 
    \count_value_i_reg[1] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[1]_i_1_n_0 ),
        .Q(Q[1]),
        .S(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[2] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[2]_i_1_n_0 ),
        .Q(Q[2]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[3] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[3]_i_1_n_0 ),
        .Q(Q[3]),
        .R(wrst_busy));
endmodule

(* ORIG_REF_NAME = "xpm_counter_updn" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_counter_updn__parameterized2_14
   (Q,
    wrst_busy,
    E,
    wr_clk);
  output [3:0]Q;
  input wrst_busy;
  input [0:0]E;
  input wr_clk;

  wire [0:0]E;
  wire [3:0]Q;
  wire \count_value_i[0]_i_1_n_0 ;
  wire \count_value_i[1]_i_1_n_0 ;
  wire \count_value_i[2]_i_1_n_0 ;
  wire \count_value_i[3]_i_1_n_0 ;
  wire wr_clk;
  wire wrst_busy;

  LUT1 #(
    .INIT(2'h1)) 
    \count_value_i[0]_i_1 
       (.I0(Q[0]),
        .O(\count_value_i[0]_i_1_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \count_value_i[1]_i_1 
       (.I0(Q[0]),
        .I1(Q[1]),
        .O(\count_value_i[1]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair16" *) 
  LUT3 #(
    .INIT(8'h78)) 
    \count_value_i[2]_i_1 
       (.I0(Q[0]),
        .I1(Q[1]),
        .I2(Q[2]),
        .O(\count_value_i[2]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair16" *) 
  LUT4 #(
    .INIT(16'h7F80)) 
    \count_value_i[3]_i_1 
       (.I0(Q[1]),
        .I1(Q[0]),
        .I2(Q[2]),
        .I3(Q[3]),
        .O(\count_value_i[3]_i_1_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[0] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[0]_i_1_n_0 ),
        .Q(Q[0]),
        .R(wrst_busy));
  FDSE #(
    .INIT(1'b1)) 
    \count_value_i_reg[1] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[1]_i_1_n_0 ),
        .Q(Q[1]),
        .S(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[2] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[2]_i_1_n_0 ),
        .Q(Q[2]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \count_value_i_reg[3] 
       (.C(wr_clk),
        .CE(E),
        .D(\count_value_i[3]_i_1_n_0 ),
        .Q(Q[3]),
        .R(wrst_busy));
endmodule

(* CASCADE_HEIGHT = "0" *) (* CDC_SYNC_STAGES = "3" *) (* DOUT_RESET_VALUE = "0" *) 
(* ECC_MODE = "no_ecc" *) (* EN_ADV_FEATURE_ASYNC = "16'b0000011100000111" *) (* FIFO_MEMORY_TYPE = "distributed" *) 
(* FIFO_READ_LATENCY = "1" *) (* FIFO_WRITE_DEPTH = "16" *) (* FULL_RESET_VALUE = "0" *) 
(* PROG_EMPTY_THRESH = "10" *) (* PROG_FULL_THRESH = "10" *) (* P_COMMON_CLOCK = "0" *) 
(* P_ECC_MODE = "0" *) (* P_FIFO_MEMORY_TYPE = "1" *) (* P_READ_MODE = "1" *) 
(* P_WAKEUP_TIME = "2" *) (* RD_DATA_COUNT_WIDTH = "4" *) (* READ_DATA_WIDTH = "64" *) 
(* READ_MODE = "fwft" *) (* RELATED_CLOCKS = "0" *) (* SIM_ASSERT_CHK = "0" *) 
(* USE_ADV_FEATURES = "0707" *) (* WAKEUP_TIME = "0" *) (* WRITE_DATA_WIDTH = "64" *) 
(* WR_DATA_COUNT_WIDTH = "4" *) (* XPM_MODULE = "TRUE" *) (* dont_touch = "true" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_async
   (sleep,
    rst,
    wr_clk,
    wr_en,
    din,
    full,
    prog_full,
    wr_data_count,
    overflow,
    wr_rst_busy,
    almost_full,
    wr_ack,
    rd_clk,
    rd_en,
    dout,
    empty,
    prog_empty,
    rd_data_count,
    underflow,
    rd_rst_busy,
    almost_empty,
    data_valid,
    injectsbiterr,
    injectdbiterr,
    sbiterr,
    dbiterr);
  input sleep;
  input rst;
  input wr_clk;
  input wr_en;
  input [63:0]din;
  output full;
  output prog_full;
  output [3:0]wr_data_count;
  output overflow;
  output wr_rst_busy;
  output almost_full;
  output wr_ack;
  input rd_clk;
  input rd_en;
  output [63:0]dout;
  output empty;
  output prog_empty;
  output [3:0]rd_data_count;
  output underflow;
  output rd_rst_busy;
  output almost_empty;
  output data_valid;
  input injectsbiterr;
  input injectdbiterr;
  output sbiterr;
  output dbiterr;

  wire \<const0> ;
  wire [63:0]din;
  wire [63:0]dout;
  wire empty;
  wire full;
  wire overflow;
  wire prog_empty;
  wire prog_full;
  wire rd_clk;
  wire [3:0]rd_data_count;
  wire rd_en;
  wire rd_rst_busy;
  wire rst;
  wire sleep;
  wire underflow;
  wire wr_clk;
  wire [3:0]wr_data_count;
  wire wr_en;
  wire wr_rst_busy;
  wire \NLW_gnuram_async_fifo.xpm_fifo_base_inst_almost_empty_UNCONNECTED ;
  wire \NLW_gnuram_async_fifo.xpm_fifo_base_inst_almost_full_UNCONNECTED ;
  wire \NLW_gnuram_async_fifo.xpm_fifo_base_inst_data_valid_UNCONNECTED ;
  wire \NLW_gnuram_async_fifo.xpm_fifo_base_inst_dbiterr_UNCONNECTED ;
  wire \NLW_gnuram_async_fifo.xpm_fifo_base_inst_full_n_UNCONNECTED ;
  wire \NLW_gnuram_async_fifo.xpm_fifo_base_inst_sbiterr_UNCONNECTED ;
  wire \NLW_gnuram_async_fifo.xpm_fifo_base_inst_wr_ack_UNCONNECTED ;

  assign almost_empty = \<const0> ;
  assign almost_full = \<const0> ;
  assign data_valid = \<const0> ;
  assign dbiterr = \<const0> ;
  assign sbiterr = \<const0> ;
  assign wr_ack = \<const0> ;
  GND GND
       (.G(\<const0> ));
  (* CASCADE_HEIGHT = "0" *) 
  (* CDC_DEST_SYNC_FF = "3" *) 
  (* COMMON_CLOCK = "0" *) 
  (* DOUT_RESET_VALUE = "0" *) 
  (* ECC_MODE = "0" *) 
  (* ENABLE_ECC = "0" *) 
  (* EN_ADV_FEATURE = "16'b0000011100000111" *) 
  (* EN_AE = "1'b0" *) 
  (* EN_AF = "1'b0" *) 
  (* EN_DVLD = "1'b0" *) 
  (* EN_OF = "1'b1" *) 
  (* EN_PE = "1'b1" *) 
  (* EN_PF = "1'b1" *) 
  (* EN_RDC = "1'b1" *) 
  (* EN_UF = "1'b1" *) 
  (* EN_WACK = "1'b0" *) 
  (* EN_WDC = "1'b1" *) 
  (* FG_EQ_ASYM_DOUT = "1'b0" *) 
  (* FIFO_MEMORY_TYPE = "1" *) 
  (* FIFO_MEM_TYPE = "1" *) 
  (* FIFO_READ_DEPTH = "16" *) 
  (* FIFO_READ_LATENCY = "1" *) 
  (* FIFO_SIZE = "1024" *) 
  (* FIFO_WRITE_DEPTH = "16" *) 
  (* FULL_RESET_VALUE = "0" *) 
  (* FULL_RST_VAL = "1'b0" *) 
  (* KEEP_HIERARCHY = "soft" *) 
  (* PE_THRESH_ADJ = "8" *) 
  (* PE_THRESH_MAX = "11" *) 
  (* PE_THRESH_MIN = "5" *) 
  (* PF_THRESH_ADJ = "8" *) 
  (* PF_THRESH_MAX = "11" *) 
  (* PF_THRESH_MIN = "8" *) 
  (* PROG_EMPTY_THRESH = "10" *) 
  (* PROG_FULL_THRESH = "10" *) 
  (* RD_DATA_COUNT_WIDTH = "4" *) 
  (* RD_DC_WIDTH_EXT = "5" *) 
  (* RD_LATENCY = "2" *) 
  (* RD_MODE = "1" *) 
  (* RD_PNTR_WIDTH = "4" *) 
  (* READ_DATA_WIDTH = "64" *) 
  (* READ_MODE = "1" *) 
  (* READ_MODE_LL = "1" *) 
  (* RELATED_CLOCKS = "0" *) 
  (* REMOVE_WR_RD_PROT_LOGIC = "0" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* USE_ADV_FEATURES = "0707" *) 
  (* VERSION = "0" *) 
  (* WAKEUP_TIME = "0" *) 
  (* WIDTH_RATIO = "1" *) 
  (* WRITE_DATA_WIDTH = "64" *) 
  (* WR_DATA_COUNT_WIDTH = "4" *) 
  (* WR_DC_WIDTH_EXT = "5" *) 
  (* WR_DEPTH_LOG = "4" *) 
  (* WR_PNTR_WIDTH = "4" *) 
  (* WR_RD_RATIO = "0" *) 
  (* WR_WIDTH_LOG = "6" *) 
  (* XPM_MODULE = "TRUE" *) 
  (* both_stages_valid = "3" *) 
  (* invalid = "0" *) 
  (* stage1_valid = "2" *) 
  (* stage2_valid = "1" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_base \gnuram_async_fifo.xpm_fifo_base_inst 
       (.almost_empty(\NLW_gnuram_async_fifo.xpm_fifo_base_inst_almost_empty_UNCONNECTED ),
        .almost_full(\NLW_gnuram_async_fifo.xpm_fifo_base_inst_almost_full_UNCONNECTED ),
        .data_valid(\NLW_gnuram_async_fifo.xpm_fifo_base_inst_data_valid_UNCONNECTED ),
        .dbiterr(\NLW_gnuram_async_fifo.xpm_fifo_base_inst_dbiterr_UNCONNECTED ),
        .din(din),
        .dout(dout),
        .empty(empty),
        .full(full),
        .full_n(\NLW_gnuram_async_fifo.xpm_fifo_base_inst_full_n_UNCONNECTED ),
        .injectdbiterr(1'b0),
        .injectsbiterr(1'b0),
        .overflow(overflow),
        .prog_empty(prog_empty),
        .prog_full(prog_full),
        .rd_clk(rd_clk),
        .rd_data_count(rd_data_count),
        .rd_en(rd_en),
        .rd_rst_busy(rd_rst_busy),
        .rst(rst),
        .sbiterr(\NLW_gnuram_async_fifo.xpm_fifo_base_inst_sbiterr_UNCONNECTED ),
        .sleep(sleep),
        .underflow(underflow),
        .wr_ack(\NLW_gnuram_async_fifo.xpm_fifo_base_inst_wr_ack_UNCONNECTED ),
        .wr_clk(wr_clk),
        .wr_data_count(wr_data_count),
        .wr_en(wr_en),
        .wr_rst_busy(wr_rst_busy));
endmodule

(* CASCADE_HEIGHT = "0" *) (* CDC_SYNC_STAGES = "3" *) (* DOUT_RESET_VALUE = "0" *) 
(* ECC_MODE = "no_ecc" *) (* EN_ADV_FEATURE_ASYNC = "16'b0000011100000111" *) (* FIFO_MEMORY_TYPE = "distributed" *) 
(* FIFO_READ_LATENCY = "1" *) (* FIFO_WRITE_DEPTH = "16" *) (* FULL_RESET_VALUE = "0" *) 
(* ORIG_REF_NAME = "xpm_fifo_async" *) (* PROG_EMPTY_THRESH = "10" *) (* PROG_FULL_THRESH = "10" *) 
(* P_COMMON_CLOCK = "0" *) (* P_ECC_MODE = "0" *) (* P_FIFO_MEMORY_TYPE = "1" *) 
(* P_READ_MODE = "1" *) (* P_WAKEUP_TIME = "2" *) (* RD_DATA_COUNT_WIDTH = "4" *) 
(* READ_DATA_WIDTH = "56" *) (* READ_MODE = "fwft" *) (* RELATED_CLOCKS = "0" *) 
(* SIM_ASSERT_CHK = "0" *) (* USE_ADV_FEATURES = "0707" *) (* WAKEUP_TIME = "0" *) 
(* WRITE_DATA_WIDTH = "56" *) (* WR_DATA_COUNT_WIDTH = "4" *) (* XPM_MODULE = "TRUE" *) 
(* dont_touch = "true" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_async__parameterized0
   (sleep,
    rst,
    wr_clk,
    wr_en,
    din,
    full,
    prog_full,
    wr_data_count,
    overflow,
    wr_rst_busy,
    almost_full,
    wr_ack,
    rd_clk,
    rd_en,
    dout,
    empty,
    prog_empty,
    rd_data_count,
    underflow,
    rd_rst_busy,
    almost_empty,
    data_valid,
    injectsbiterr,
    injectdbiterr,
    sbiterr,
    dbiterr);
  input sleep;
  input rst;
  input wr_clk;
  input wr_en;
  input [55:0]din;
  output full;
  output prog_full;
  output [3:0]wr_data_count;
  output overflow;
  output wr_rst_busy;
  output almost_full;
  output wr_ack;
  input rd_clk;
  input rd_en;
  output [55:0]dout;
  output empty;
  output prog_empty;
  output [3:0]rd_data_count;
  output underflow;
  output rd_rst_busy;
  output almost_empty;
  output data_valid;
  input injectsbiterr;
  input injectdbiterr;
  output sbiterr;
  output dbiterr;

  wire \<const0> ;
  wire [55:0]din;
  wire [55:0]dout;
  wire empty;
  wire full;
  wire overflow;
  wire prog_empty;
  wire prog_full;
  wire rd_clk;
  wire [3:0]rd_data_count;
  wire rd_en;
  wire rd_rst_busy;
  wire rst;
  wire sleep;
  wire underflow;
  wire wr_clk;
  wire [3:0]wr_data_count;
  wire wr_en;
  wire wr_rst_busy;
  wire \NLW_gnuram_async_fifo.xpm_fifo_base_inst_almost_empty_UNCONNECTED ;
  wire \NLW_gnuram_async_fifo.xpm_fifo_base_inst_almost_full_UNCONNECTED ;
  wire \NLW_gnuram_async_fifo.xpm_fifo_base_inst_data_valid_UNCONNECTED ;
  wire \NLW_gnuram_async_fifo.xpm_fifo_base_inst_dbiterr_UNCONNECTED ;
  wire \NLW_gnuram_async_fifo.xpm_fifo_base_inst_full_n_UNCONNECTED ;
  wire \NLW_gnuram_async_fifo.xpm_fifo_base_inst_sbiterr_UNCONNECTED ;
  wire \NLW_gnuram_async_fifo.xpm_fifo_base_inst_wr_ack_UNCONNECTED ;

  assign almost_empty = \<const0> ;
  assign almost_full = \<const0> ;
  assign data_valid = \<const0> ;
  assign dbiterr = \<const0> ;
  assign sbiterr = \<const0> ;
  assign wr_ack = \<const0> ;
  GND GND
       (.G(\<const0> ));
  (* CASCADE_HEIGHT = "0" *) 
  (* CDC_DEST_SYNC_FF = "3" *) 
  (* COMMON_CLOCK = "0" *) 
  (* DOUT_RESET_VALUE = "0" *) 
  (* ECC_MODE = "0" *) 
  (* ENABLE_ECC = "0" *) 
  (* EN_ADV_FEATURE = "16'b0000011100000111" *) 
  (* EN_AE = "1'b0" *) 
  (* EN_AF = "1'b0" *) 
  (* EN_DVLD = "1'b0" *) 
  (* EN_OF = "1'b1" *) 
  (* EN_PE = "1'b1" *) 
  (* EN_PF = "1'b1" *) 
  (* EN_RDC = "1'b1" *) 
  (* EN_UF = "1'b1" *) 
  (* EN_WACK = "1'b0" *) 
  (* EN_WDC = "1'b1" *) 
  (* FG_EQ_ASYM_DOUT = "1'b0" *) 
  (* FIFO_MEMORY_TYPE = "1" *) 
  (* FIFO_MEM_TYPE = "1" *) 
  (* FIFO_READ_DEPTH = "16" *) 
  (* FIFO_READ_LATENCY = "1" *) 
  (* FIFO_SIZE = "896" *) 
  (* FIFO_WRITE_DEPTH = "16" *) 
  (* FULL_RESET_VALUE = "0" *) 
  (* FULL_RST_VAL = "1'b0" *) 
  (* KEEP_HIERARCHY = "soft" *) 
  (* PE_THRESH_ADJ = "8" *) 
  (* PE_THRESH_MAX = "11" *) 
  (* PE_THRESH_MIN = "5" *) 
  (* PF_THRESH_ADJ = "8" *) 
  (* PF_THRESH_MAX = "11" *) 
  (* PF_THRESH_MIN = "8" *) 
  (* PROG_EMPTY_THRESH = "10" *) 
  (* PROG_FULL_THRESH = "10" *) 
  (* RD_DATA_COUNT_WIDTH = "4" *) 
  (* RD_DC_WIDTH_EXT = "5" *) 
  (* RD_LATENCY = "2" *) 
  (* RD_MODE = "1" *) 
  (* RD_PNTR_WIDTH = "4" *) 
  (* READ_DATA_WIDTH = "56" *) 
  (* READ_MODE = "1" *) 
  (* READ_MODE_LL = "1" *) 
  (* RELATED_CLOCKS = "0" *) 
  (* REMOVE_WR_RD_PROT_LOGIC = "0" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* USE_ADV_FEATURES = "0707" *) 
  (* VERSION = "0" *) 
  (* WAKEUP_TIME = "0" *) 
  (* WIDTH_RATIO = "1" *) 
  (* WRITE_DATA_WIDTH = "56" *) 
  (* WR_DATA_COUNT_WIDTH = "4" *) 
  (* WR_DC_WIDTH_EXT = "5" *) 
  (* WR_DEPTH_LOG = "4" *) 
  (* WR_PNTR_WIDTH = "4" *) 
  (* WR_RD_RATIO = "0" *) 
  (* WR_WIDTH_LOG = "6" *) 
  (* XPM_MODULE = "TRUE" *) 
  (* both_stages_valid = "3" *) 
  (* invalid = "0" *) 
  (* stage1_valid = "2" *) 
  (* stage2_valid = "1" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_base__parameterized0 \gnuram_async_fifo.xpm_fifo_base_inst 
       (.almost_empty(\NLW_gnuram_async_fifo.xpm_fifo_base_inst_almost_empty_UNCONNECTED ),
        .almost_full(\NLW_gnuram_async_fifo.xpm_fifo_base_inst_almost_full_UNCONNECTED ),
        .data_valid(\NLW_gnuram_async_fifo.xpm_fifo_base_inst_data_valid_UNCONNECTED ),
        .dbiterr(\NLW_gnuram_async_fifo.xpm_fifo_base_inst_dbiterr_UNCONNECTED ),
        .din(din),
        .dout(dout),
        .empty(empty),
        .full(full),
        .full_n(\NLW_gnuram_async_fifo.xpm_fifo_base_inst_full_n_UNCONNECTED ),
        .injectdbiterr(1'b0),
        .injectsbiterr(1'b0),
        .overflow(overflow),
        .prog_empty(prog_empty),
        .prog_full(prog_full),
        .rd_clk(rd_clk),
        .rd_data_count(rd_data_count),
        .rd_en(rd_en),
        .rd_rst_busy(rd_rst_busy),
        .rst(rst),
        .sbiterr(\NLW_gnuram_async_fifo.xpm_fifo_base_inst_sbiterr_UNCONNECTED ),
        .sleep(sleep),
        .underflow(underflow),
        .wr_ack(\NLW_gnuram_async_fifo.xpm_fifo_base_inst_wr_ack_UNCONNECTED ),
        .wr_clk(wr_clk),
        .wr_data_count(wr_data_count),
        .wr_en(wr_en),
        .wr_rst_busy(wr_rst_busy));
endmodule

(* CASCADE_HEIGHT = "0" *) (* CDC_DEST_SYNC_FF = "3" *) (* COMMON_CLOCK = "0" *) 
(* DOUT_RESET_VALUE = "0" *) (* ECC_MODE = "0" *) (* ENABLE_ECC = "0" *) 
(* EN_ADV_FEATURE = "16'b0000011100000111" *) (* EN_AE = "1'b0" *) (* EN_AF = "1'b0" *) 
(* EN_DVLD = "1'b0" *) (* EN_OF = "1'b1" *) (* EN_PE = "1'b1" *) 
(* EN_PF = "1'b1" *) (* EN_RDC = "1'b1" *) (* EN_UF = "1'b1" *) 
(* EN_WACK = "1'b0" *) (* EN_WDC = "1'b1" *) (* FG_EQ_ASYM_DOUT = "1'b0" *) 
(* FIFO_MEMORY_TYPE = "1" *) (* FIFO_MEM_TYPE = "1" *) (* FIFO_READ_DEPTH = "16" *) 
(* FIFO_READ_LATENCY = "1" *) (* FIFO_SIZE = "1024" *) (* FIFO_WRITE_DEPTH = "16" *) 
(* FULL_RESET_VALUE = "0" *) (* FULL_RST_VAL = "1'b0" *) (* PE_THRESH_ADJ = "8" *) 
(* PE_THRESH_MAX = "11" *) (* PE_THRESH_MIN = "5" *) (* PF_THRESH_ADJ = "8" *) 
(* PF_THRESH_MAX = "11" *) (* PF_THRESH_MIN = "8" *) (* PROG_EMPTY_THRESH = "10" *) 
(* PROG_FULL_THRESH = "10" *) (* RD_DATA_COUNT_WIDTH = "4" *) (* RD_DC_WIDTH_EXT = "5" *) 
(* RD_LATENCY = "2" *) (* RD_MODE = "1" *) (* RD_PNTR_WIDTH = "4" *) 
(* READ_DATA_WIDTH = "64" *) (* READ_MODE = "1" *) (* READ_MODE_LL = "1" *) 
(* RELATED_CLOCKS = "0" *) (* REMOVE_WR_RD_PROT_LOGIC = "0" *) (* SIM_ASSERT_CHK = "0" *) 
(* USE_ADV_FEATURES = "0707" *) (* VERSION = "0" *) (* WAKEUP_TIME = "0" *) 
(* WIDTH_RATIO = "1" *) (* WRITE_DATA_WIDTH = "64" *) (* WR_DATA_COUNT_WIDTH = "4" *) 
(* WR_DC_WIDTH_EXT = "5" *) (* WR_DEPTH_LOG = "4" *) (* WR_PNTR_WIDTH = "4" *) 
(* WR_RD_RATIO = "0" *) (* WR_WIDTH_LOG = "6" *) (* XPM_MODULE = "TRUE" *) 
(* both_stages_valid = "3" *) (* invalid = "0" *) (* keep_hierarchy = "soft" *) 
(* stage1_valid = "2" *) (* stage2_valid = "1" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_base
   (sleep,
    rst,
    wr_clk,
    wr_en,
    din,
    full,
    full_n,
    prog_full,
    wr_data_count,
    overflow,
    wr_rst_busy,
    almost_full,
    wr_ack,
    rd_clk,
    rd_en,
    dout,
    empty,
    prog_empty,
    rd_data_count,
    underflow,
    rd_rst_busy,
    almost_empty,
    data_valid,
    injectsbiterr,
    injectdbiterr,
    sbiterr,
    dbiterr);
  input sleep;
  input rst;
  input wr_clk;
  input wr_en;
  input [63:0]din;
  output full;
  output full_n;
  output prog_full;
  output [3:0]wr_data_count;
  output overflow;
  output wr_rst_busy;
  output almost_full;
  output wr_ack;
  input rd_clk;
  input rd_en;
  output [63:0]dout;
  output empty;
  output prog_empty;
  output [3:0]rd_data_count;
  output underflow;
  output rd_rst_busy;
  output almost_empty;
  output data_valid;
  input injectsbiterr;
  input injectdbiterr;
  output sbiterr;
  output dbiterr;

  wire \<const0> ;
  wire [1:0]count_value_i;
  wire [1:0]curr_fwft_state;
  wire [3:0]diff_pntr_pe;
  wire [4:4]diff_pntr_pf_q0;
  wire [63:0]din;
  wire [63:0]dout;
  wire empty;
  wire empty_fwft_i0;
  wire full;
  wire \gen_cdc_pntr.rpw_gray_reg_dc_n_3 ;
  wire \gen_cdc_pntr.rpw_gray_reg_dc_n_4 ;
  wire \gen_cdc_pntr.rpw_gray_reg_dc_n_5 ;
  wire \gen_cdc_pntr.wpr_gray_reg_dc_n_1 ;
  wire \gen_cdc_pntr.wpr_gray_reg_dc_n_2 ;
  wire \gen_cdc_pntr.wpr_gray_reg_dc_n_3 ;
  wire \gen_cdc_pntr.wpr_gray_reg_dc_n_4 ;
  wire \gen_cdc_pntr.wpr_gray_reg_dc_n_5 ;
  wire \gen_cdc_pntr.wpr_gray_reg_dc_n_6 ;
  wire \gen_cdc_pntr.wpr_gray_reg_n_2 ;
  wire \gen_cdc_pntr.wpr_gray_reg_n_3 ;
  wire \gen_cdc_pntr.wpr_gray_reg_n_4 ;
  wire \gen_cdc_pntr.wpr_gray_reg_n_5 ;
  wire \gen_fwft.count_rst ;
  wire \gen_fwft.ram_regout_en ;
  wire \gen_fwft.rdpp1_inst_n_3 ;
  wire \gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg_n_0_[0] ;
  wire \gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg_n_0_[1] ;
  wire \gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg_n_0_[2] ;
  wire \gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg_n_0_[3] ;
  wire \gen_pf_ic_rc.gpe_ic.prog_empty_i_i_1_n_0 ;
  wire [4:1]\grdc.diff_wr_rd_pntr_rdc ;
  wire \grdc.rd_data_count_i0 ;
  wire [4:1]\gwdc.diff_wr_rd_pntr1_out ;
  wire [1:0]next_fwft_state__0;
  wire overflow;
  wire overflow_i0;
  wire p_1_in__0;
  wire prog_empty;
  wire prog_full;
  wire ram_empty_i;
  wire ram_empty_i0;
  wire ram_full_i0;
  wire rd_clk;
  wire [3:0]rd_data_count;
  wire rd_en;
  wire [3:0]rd_pntr_ext;
  wire [3:3]rd_pntr_wr;
  wire [3:0]rd_pntr_wr_cdc;
  wire [4:0]rd_pntr_wr_cdc_dc;
  wire rd_rst_busy;
  wire rdp_inst_n_10;
  wire rdp_inst_n_11;
  wire rdp_inst_n_12;
  wire rdp_inst_n_8;
  wire rdp_inst_n_9;
  wire rdpp1_inst_n_0;
  wire rdpp1_inst_n_1;
  wire rdpp1_inst_n_2;
  wire rdpp1_inst_n_3;
  wire rst;
  wire rst_d1;
  wire rst_d1_inst_n_1;
  wire sleep;
  wire [1:1]src_in_bin00_out;
  wire underflow;
  wire underflow_i0;
  wire wr_clk;
  wire [3:0]wr_data_count;
  wire wr_en;
  wire [4:0]wr_pntr_ext;
  wire [4:1]wr_pntr_plus1_pf;
  wire wr_pntr_plus1_pf_carry;
  wire [3:0]wr_pntr_rd_cdc;
  wire [4:0]wr_pntr_rd_cdc_dc;
  wire wr_rst_busy;
  wire wrpp1_inst_n_4;
  wire wrpp2_inst_n_0;
  wire wrpp2_inst_n_1;
  wire wrpp2_inst_n_2;
  wire wrpp2_inst_n_3;
  wire wrst_busy;
  wire \NLW_gen_sdpram.xpm_memory_base_inst_dbiterra_UNCONNECTED ;
  wire \NLW_gen_sdpram.xpm_memory_base_inst_dbiterrb_UNCONNECTED ;
  wire \NLW_gen_sdpram.xpm_memory_base_inst_sbiterra_UNCONNECTED ;
  wire \NLW_gen_sdpram.xpm_memory_base_inst_sbiterrb_UNCONNECTED ;
  wire [63:0]\NLW_gen_sdpram.xpm_memory_base_inst_douta_UNCONNECTED ;

  assign almost_empty = \<const0> ;
  assign almost_full = \<const0> ;
  assign data_valid = \<const0> ;
  assign dbiterr = \<const0> ;
  assign full_n = \<const0> ;
  assign sbiterr = \<const0> ;
  assign wr_ack = \<const0> ;
  (* SOFT_HLUTNM = "soft_lutpair20" *) 
  LUT4 #(
    .INIT(16'h6A85)) 
    \FSM_sequential_gen_fwft.curr_fwft_state[0]_i_1 
       (.I0(curr_fwft_state[0]),
        .I1(rd_en),
        .I2(curr_fwft_state[1]),
        .I3(ram_empty_i),
        .O(next_fwft_state__0[0]));
  LUT3 #(
    .INIT(8'h7C)) 
    \FSM_sequential_gen_fwft.curr_fwft_state[1]_i_1 
       (.I0(rd_en),
        .I1(curr_fwft_state[1]),
        .I2(curr_fwft_state[0]),
        .O(next_fwft_state__0[1]));
  (* FSM_ENCODED_STATES = "invalid:00,stage1_valid:01,both_stages_valid:10,stage2_valid:11" *) 
  FDRE #(
    .INIT(1'b0)) 
    \FSM_sequential_gen_fwft.curr_fwft_state_reg[0] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(next_fwft_state__0[0]),
        .Q(curr_fwft_state[0]),
        .R(rd_rst_busy));
  (* FSM_ENCODED_STATES = "invalid:00,stage1_valid:01,both_stages_valid:10,stage2_valid:11" *) 
  FDRE #(
    .INIT(1'b0)) 
    \FSM_sequential_gen_fwft.curr_fwft_state_reg[1] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(next_fwft_state__0[1]),
        .Q(curr_fwft_state[1]),
        .R(rd_rst_busy));
  GND GND
       (.G(\<const0> ));
  (* DEST_SYNC_FF = "3" *) 
  (* INIT_SYNC_FF = "1" *) 
  (* REG_OUTPUT = "0" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* SIM_LOSSLESS_GRAY_CHK = "0" *) 
  (* VERSION = "0" *) 
  (* WIDTH = "5" *) 
  (* XPM_CDC = "GRAY" *) 
  (* XPM_MODULE = "TRUE" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_gray__parameterized1__2 \gen_cdc_pntr.rd_pntr_cdc_dc_inst 
       (.dest_clk(wr_clk),
        .dest_out_bin(rd_pntr_wr_cdc_dc),
        .src_clk(rd_clk),
        .src_in_bin({rdp_inst_n_9,rdp_inst_n_10,rdp_inst_n_11,src_in_bin00_out,rdp_inst_n_12}));
  (* DEST_SYNC_FF = "3" *) 
  (* INIT_SYNC_FF = "1" *) 
  (* REG_OUTPUT = "0" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* SIM_LOSSLESS_GRAY_CHK = "0" *) 
  (* VERSION = "0" *) 
  (* WIDTH = "4" *) 
  (* XPM_CDC = "GRAY" *) 
  (* XPM_MODULE = "TRUE" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_gray__5 \gen_cdc_pntr.rd_pntr_cdc_inst 
       (.dest_clk(wr_clk),
        .dest_out_bin(rd_pntr_wr_cdc),
        .src_clk(rd_clk),
        .src_in_bin(rd_pntr_ext));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_reg_vec_4 \gen_cdc_pntr.rpw_gray_reg 
       (.D(rd_pntr_wr_cdc),
        .E(wr_pntr_plus1_pf_carry),
        .Q(rd_pntr_wr),
        .diff_pntr_pf_q0(diff_pntr_pf_q0),
        .\gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] (wrpp1_inst_n_4),
        .\gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4]_0 (full),
        .\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg (wr_pntr_plus1_pf),
        .\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg_0 ({wrpp2_inst_n_0,wrpp2_inst_n_1,wrpp2_inst_n_2,wrpp2_inst_n_3}),
        .ram_full_i0(ram_full_i0),
        .rst_d1(rst_d1),
        .wr_clk(wr_clk),
        .wr_en(wr_en),
        .wrst_busy(wrst_busy));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_reg_vec__parameterized0_5 \gen_cdc_pntr.rpw_gray_reg_dc 
       (.D({\gwdc.diff_wr_rd_pntr1_out [4:3],\gwdc.diff_wr_rd_pntr1_out [1]}),
        .Q({\gen_cdc_pntr.rpw_gray_reg_dc_n_3 ,\gen_cdc_pntr.rpw_gray_reg_dc_n_4 ,\gen_cdc_pntr.rpw_gray_reg_dc_n_5 }),
        .\gwdc.wr_data_count_i_reg[4] (wr_pntr_ext),
        .\reg_out_i_reg[4]_0 (rd_pntr_wr_cdc_dc),
        .wr_clk(wr_clk),
        .wrst_busy(wrst_busy));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_reg_vec_6 \gen_cdc_pntr.wpr_gray_reg 
       (.D(diff_pntr_pe[1:0]),
        .Q({\gen_cdc_pntr.wpr_gray_reg_n_2 ,\gen_cdc_pntr.wpr_gray_reg_n_3 ,\gen_cdc_pntr.wpr_gray_reg_n_4 ,\gen_cdc_pntr.wpr_gray_reg_n_5 }),
        .enb(rdp_inst_n_8),
        .\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[0] (curr_fwft_state),
        .\gen_pf_ic_rc.ram_empty_i_reg (rd_pntr_ext),
        .\gen_pf_ic_rc.ram_empty_i_reg_0 ({rdpp1_inst_n_0,rdpp1_inst_n_1,rdpp1_inst_n_2,rdpp1_inst_n_3}),
        .ram_empty_i(ram_empty_i),
        .ram_empty_i0(ram_empty_i0),
        .rd_clk(rd_clk),
        .rd_en(rd_en),
        .\reg_out_i_reg[0]_0 (rd_rst_busy),
        .\reg_out_i_reg[3]_0 (wr_pntr_rd_cdc));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_reg_vec__parameterized0_7 \gen_cdc_pntr.wpr_gray_reg_dc 
       (.D(\grdc.diff_wr_rd_pntr_rdc [1]),
        .Q({\gen_cdc_pntr.wpr_gray_reg_dc_n_1 ,\gen_cdc_pntr.wpr_gray_reg_dc_n_2 ,\gen_cdc_pntr.wpr_gray_reg_dc_n_3 ,\gen_cdc_pntr.wpr_gray_reg_dc_n_4 ,\gen_cdc_pntr.wpr_gray_reg_dc_n_5 }),
        .\grdc.rd_data_count_i_reg[1] (count_value_i),
        .\grdc.rd_data_count_i_reg[4] ({rd_pntr_ext[3],rd_pntr_ext[1:0]}),
        .rd_clk(rd_clk),
        .\reg_out_i_reg[3]_0 (\gen_cdc_pntr.wpr_gray_reg_dc_n_6 ),
        .\reg_out_i_reg[4]_0 (rd_rst_busy),
        .\reg_out_i_reg[4]_1 (wr_pntr_rd_cdc_dc));
  (* DEST_SYNC_FF = "5" *) 
  (* INIT_SYNC_FF = "1" *) 
  (* REG_OUTPUT = "0" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* SIM_LOSSLESS_GRAY_CHK = "0" *) 
  (* VERSION = "0" *) 
  (* WIDTH = "5" *) 
  (* XPM_CDC = "GRAY" *) 
  (* XPM_MODULE = "TRUE" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_gray__parameterized0__2 \gen_cdc_pntr.wr_pntr_cdc_dc_inst 
       (.dest_clk(rd_clk),
        .dest_out_bin(wr_pntr_rd_cdc_dc),
        .src_clk(wr_clk),
        .src_in_bin(wr_pntr_ext));
  (* DEST_SYNC_FF = "3" *) 
  (* INIT_SYNC_FF = "1" *) 
  (* REG_OUTPUT = "0" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* SIM_LOSSLESS_GRAY_CHK = "0" *) 
  (* VERSION = "0" *) 
  (* WIDTH = "4" *) 
  (* XPM_CDC = "GRAY" *) 
  (* XPM_MODULE = "TRUE" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_gray__4 \gen_cdc_pntr.wr_pntr_cdc_inst 
       (.dest_clk(rd_clk),
        .dest_out_bin(wr_pntr_rd_cdc),
        .src_clk(wr_clk),
        .src_in_bin(wr_pntr_ext[3:0]));
  LUT4 #(
    .INIT(16'hF380)) 
    \gen_fwft.empty_fwft_i_i_1 
       (.I0(rd_en),
        .I1(curr_fwft_state[0]),
        .I2(curr_fwft_state[1]),
        .I3(empty),
        .O(empty_fwft_i0));
  FDSE #(
    .INIT(1'b1)) 
    \gen_fwft.empty_fwft_i_reg 
       (.C(rd_clk),
        .CE(1'b1),
        .D(empty_fwft_i0),
        .Q(empty),
        .S(rd_rst_busy));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_counter_updn_8 \gen_fwft.rdpp1_inst 
       (.D(\grdc.diff_wr_rd_pntr_rdc [2]),
        .Q(count_value_i),
        .SR(\gen_fwft.count_rst ),
        .\count_value_i_reg[1]_0 (\gen_fwft.rdpp1_inst_n_3 ),
        .\count_value_i_reg[1]_1 (curr_fwft_state),
        .\grdc.rd_data_count_i_reg[2] ({\gen_cdc_pntr.wpr_gray_reg_dc_n_3 ,\gen_cdc_pntr.wpr_gray_reg_dc_n_4 ,\gen_cdc_pntr.wpr_gray_reg_dc_n_5 }),
        .\grdc.rd_data_count_i_reg[2]_0 (rd_pntr_ext[2:0]),
        .ram_empty_i(ram_empty_i),
        .rd_clk(rd_clk),
        .rd_en(rd_en),
        .src_in_bin(src_in_bin00_out));
  FDRE #(
    .INIT(1'b0)) 
    \gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[0] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(diff_pntr_pe[0]),
        .Q(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg_n_0_[0] ),
        .R(rd_rst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[1] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(diff_pntr_pe[1]),
        .Q(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg_n_0_[1] ),
        .R(rd_rst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[2] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(diff_pntr_pe[2]),
        .Q(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg_n_0_[2] ),
        .R(rd_rst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[3] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(diff_pntr_pe[3]),
        .Q(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg_n_0_[3] ),
        .R(rd_rst_busy));
  LUT6 #(
    .INIT(64'h8888888BBBBBBBBB)) 
    \gen_pf_ic_rc.gpe_ic.prog_empty_i_i_1 
       (.I0(prog_empty),
        .I1(empty),
        .I2(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg_n_0_[0] ),
        .I3(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg_n_0_[1] ),
        .I4(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg_n_0_[2] ),
        .I5(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg_n_0_[3] ),
        .O(\gen_pf_ic_rc.gpe_ic.prog_empty_i_i_1_n_0 ));
  FDSE #(
    .INIT(1'b1)) 
    \gen_pf_ic_rc.gpe_ic.prog_empty_i_reg 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\gen_pf_ic_rc.gpe_ic.prog_empty_i_i_1_n_0 ),
        .Q(prog_empty),
        .S(rd_rst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(diff_pntr_pf_q0),
        .Q(p_1_in__0),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \gen_pf_ic_rc.gpf_ic.prog_full_i_reg 
       (.C(wr_clk),
        .CE(1'b1),
        .D(rst_d1_inst_n_1),
        .Q(prog_full),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg 
       (.C(wr_clk),
        .CE(1'b1),
        .D(ram_full_i0),
        .Q(full),
        .R(wrst_busy));
  FDSE #(
    .INIT(1'b1)) 
    \gen_pf_ic_rc.ram_empty_i_reg 
       (.C(rd_clk),
        .CE(1'b1),
        .D(ram_empty_i0),
        .Q(ram_empty_i),
        .S(rd_rst_busy));
  (* ADDR_WIDTH_A = "4" *) 
  (* ADDR_WIDTH_B = "4" *) 
  (* AUTO_SLEEP_TIME = "0" *) 
  (* BYTE_WRITE_WIDTH_A = "64" *) 
  (* BYTE_WRITE_WIDTH_B = "64" *) 
  (* CASCADE_HEIGHT = "0" *) 
  (* CLOCKING_MODE = "1" *) 
  (* ECC_MODE = "0" *) 
  (* KEEP_HIERARCHY = "soft" *) 
  (* MAX_NUM_CHAR = "0" *) 
  (* MEMORY_INIT_FILE = "none" *) 
  (* MEMORY_INIT_PARAM = "" *) 
  (* MEMORY_OPTIMIZATION = "true" *) 
  (* MEMORY_PRIMITIVE = "1" *) 
  (* MEMORY_SIZE = "1024" *) 
  (* MEMORY_TYPE = "1" *) 
  (* MESSAGE_CONTROL = "0" *) 
  (* NUM_CHAR_LOC = "0" *) 
  (* P_ECC_MODE = "no_ecc" *) 
  (* P_ENABLE_BYTE_WRITE_A = "0" *) 
  (* P_ENABLE_BYTE_WRITE_B = "0" *) 
  (* P_MAX_DEPTH_DATA = "16" *) 
  (* P_MEMORY_OPT = "yes" *) 
  (* P_MEMORY_PRIMITIVE = "distributed" *) 
  (* P_MIN_WIDTH_DATA = "64" *) 
  (* P_MIN_WIDTH_DATA_A = "64" *) 
  (* P_MIN_WIDTH_DATA_B = "64" *) 
  (* P_MIN_WIDTH_DATA_ECC = "64" *) 
  (* P_MIN_WIDTH_DATA_LDW = "4" *) 
  (* P_MIN_WIDTH_DATA_SHFT = "64" *) 
  (* P_NUM_COLS_WRITE_A = "1" *) 
  (* P_NUM_COLS_WRITE_B = "1" *) 
  (* P_NUM_ROWS_READ_A = "1" *) 
  (* P_NUM_ROWS_READ_B = "1" *) 
  (* P_NUM_ROWS_WRITE_A = "1" *) 
  (* P_NUM_ROWS_WRITE_B = "1" *) 
  (* P_SDP_WRITE_MODE = "yes" *) 
  (* P_WIDTH_ADDR_LSB_READ_A = "0" *) 
  (* P_WIDTH_ADDR_LSB_READ_B = "0" *) 
  (* P_WIDTH_ADDR_LSB_WRITE_A = "0" *) 
  (* P_WIDTH_ADDR_LSB_WRITE_B = "0" *) 
  (* P_WIDTH_ADDR_READ_A = "4" *) 
  (* P_WIDTH_ADDR_READ_B = "4" *) 
  (* P_WIDTH_ADDR_WRITE_A = "4" *) 
  (* P_WIDTH_ADDR_WRITE_B = "4" *) 
  (* P_WIDTH_COL_WRITE_A = "64" *) 
  (* P_WIDTH_COL_WRITE_B = "64" *) 
  (* READ_DATA_WIDTH_A = "64" *) 
  (* READ_DATA_WIDTH_B = "64" *) 
  (* READ_LATENCY_A = "2" *) 
  (* READ_LATENCY_B = "2" *) 
  (* READ_RESET_VALUE_A = "0" *) 
  (* READ_RESET_VALUE_B = "0" *) 
  (* RST_MODE_A = "SYNC" *) 
  (* RST_MODE_B = "SYNC" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* USE_EMBEDDED_CONSTRAINT = "1" *) 
  (* USE_MEM_INIT = "0" *) 
  (* USE_MEM_INIT_MMI = "0" *) 
  (* VERSION = "0" *) 
  (* WAKEUP_TIME = "0" *) 
  (* WRITE_DATA_WIDTH_A = "64" *) 
  (* WRITE_DATA_WIDTH_B = "64" *) 
  (* WRITE_MODE_A = "2" *) 
  (* WRITE_MODE_B = "1" *) 
  (* WRITE_PROTECT = "1" *) 
  (* XPM_MODULE = "TRUE" *) 
  (* rsta_loop_iter = "64" *) 
  (* rstb_loop_iter = "64" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_memory_base \gen_sdpram.xpm_memory_base_inst 
       (.addra(wr_pntr_ext[3:0]),
        .addrb(rd_pntr_ext),
        .clka(wr_clk),
        .clkb(rd_clk),
        .dbiterra(\NLW_gen_sdpram.xpm_memory_base_inst_dbiterra_UNCONNECTED ),
        .dbiterrb(\NLW_gen_sdpram.xpm_memory_base_inst_dbiterrb_UNCONNECTED ),
        .dina(din),
        .dinb({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .douta(\NLW_gen_sdpram.xpm_memory_base_inst_douta_UNCONNECTED [63:0]),
        .doutb(dout),
        .ena(wr_pntr_plus1_pf_carry),
        .enb(rdp_inst_n_8),
        .injectdbiterra(1'b0),
        .injectdbiterrb(1'b0),
        .injectsbiterra(1'b0),
        .injectsbiterrb(1'b0),
        .regcea(1'b0),
        .regceb(\gen_fwft.ram_regout_en ),
        .rsta(1'b0),
        .rstb(rd_rst_busy),
        .sbiterra(\NLW_gen_sdpram.xpm_memory_base_inst_sbiterra_UNCONNECTED ),
        .sbiterrb(\NLW_gen_sdpram.xpm_memory_base_inst_sbiterrb_UNCONNECTED ),
        .sleep(sleep),
        .wea(1'b0),
        .web(1'b0));
  (* SOFT_HLUTNM = "soft_lutpair20" *) 
  LUT3 #(
    .INIT(8'h62)) 
    \gen_sdpram.xpm_memory_base_inst_i_3 
       (.I0(curr_fwft_state[0]),
        .I1(curr_fwft_state[1]),
        .I2(rd_en),
        .O(\gen_fwft.ram_regout_en ));
  FDRE #(
    .INIT(1'b0)) 
    \gof.overflow_i_reg 
       (.C(wr_clk),
        .CE(1'b1),
        .D(overflow_i0),
        .Q(overflow),
        .R(1'b0));
  FDRE \grdc.rd_data_count_i_reg[1] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\grdc.diff_wr_rd_pntr_rdc [1]),
        .Q(rd_data_count[0]),
        .R(\grdc.rd_data_count_i0 ));
  FDRE \grdc.rd_data_count_i_reg[2] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\grdc.diff_wr_rd_pntr_rdc [2]),
        .Q(rd_data_count[1]),
        .R(\grdc.rd_data_count_i0 ));
  FDRE \grdc.rd_data_count_i_reg[3] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\grdc.diff_wr_rd_pntr_rdc [3]),
        .Q(rd_data_count[2]),
        .R(\grdc.rd_data_count_i0 ));
  FDRE \grdc.rd_data_count_i_reg[4] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\grdc.diff_wr_rd_pntr_rdc [4]),
        .Q(rd_data_count[3]),
        .R(\grdc.rd_data_count_i0 ));
  FDRE #(
    .INIT(1'b0)) 
    \guf.underflow_i_reg 
       (.C(rd_clk),
        .CE(1'b1),
        .D(underflow_i0),
        .Q(underflow),
        .R(1'b0));
  FDRE \gwdc.wr_data_count_i_reg[1] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\gwdc.diff_wr_rd_pntr1_out [1]),
        .Q(wr_data_count[0]),
        .R(wrst_busy));
  FDRE \gwdc.wr_data_count_i_reg[2] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\gwdc.diff_wr_rd_pntr1_out [2]),
        .Q(wr_data_count[1]),
        .R(wrst_busy));
  FDRE \gwdc.wr_data_count_i_reg[3] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\gwdc.diff_wr_rd_pntr1_out [3]),
        .Q(wr_data_count[2]),
        .R(wrst_busy));
  FDRE \gwdc.wr_data_count_i_reg[4] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\gwdc.diff_wr_rd_pntr1_out [4]),
        .Q(wr_data_count[3]),
        .R(wrst_busy));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_counter_updn__parameterized0_9 rdp_inst
       (.D(diff_pntr_pe[3:2]),
        .E(rdp_inst_n_8),
        .Q(rd_pntr_ext),
        .\count_value_i_reg[0]_0 (curr_fwft_state),
        .\count_value_i_reg[4]_0 (rd_rst_busy),
        .\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[3] ({\gen_cdc_pntr.wpr_gray_reg_n_2 ,\gen_cdc_pntr.wpr_gray_reg_n_3 ,\gen_cdc_pntr.wpr_gray_reg_n_4 ,\gen_cdc_pntr.wpr_gray_reg_n_5 }),
        .\grdc.rd_data_count_i_reg[4] (\gen_fwft.rdpp1_inst_n_3 ),
        .\grdc.rd_data_count_i_reg[4]_0 ({\gen_cdc_pntr.wpr_gray_reg_dc_n_1 ,\gen_cdc_pntr.wpr_gray_reg_dc_n_2 ,\gen_cdc_pntr.wpr_gray_reg_dc_n_3 ,\gen_cdc_pntr.wpr_gray_reg_dc_n_4 }),
        .\grdc.rd_data_count_i_reg[4]_1 (\gen_cdc_pntr.wpr_gray_reg_dc_n_6 ),
        .ram_empty_i(ram_empty_i),
        .rd_clk(rd_clk),
        .rd_en(rd_en),
        .\reg_out_i_reg[2] (\grdc.diff_wr_rd_pntr_rdc [4:3]),
        .\src_gray_ff_reg[4] (count_value_i),
        .src_in_bin({rdp_inst_n_9,rdp_inst_n_10,rdp_inst_n_11,rdp_inst_n_12}));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_counter_updn__parameterized1_10 rdpp1_inst
       (.E(rdp_inst_n_8),
        .Q({rdpp1_inst_n_0,rdpp1_inst_n_1,rdpp1_inst_n_2,rdpp1_inst_n_3}),
        .\count_value_i_reg[0]_0 (rd_rst_busy),
        .\count_value_i_reg[1]_0 (curr_fwft_state),
        .rd_clk(rd_clk),
        .rd_en(rd_en));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_reg_bit_11 rst_d1_inst
       (.\gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] (rst_d1_inst_n_1),
        .\gen_pf_ic_rc.gpf_ic.prog_full_i_reg (full),
        .overflow_i0(overflow_i0),
        .p_1_in__0(p_1_in__0),
        .prog_full(prog_full),
        .rst(rst),
        .rst_d1(rst_d1),
        .wr_clk(wr_clk),
        .wr_en(wr_en),
        .wrst_busy(wrst_busy));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_counter_updn__parameterized0_12 wrp_inst
       (.D(\gwdc.diff_wr_rd_pntr1_out [2]),
        .E(wr_pntr_plus1_pf_carry),
        .Q(wr_pntr_ext),
        .\gwdc.wr_data_count_i_reg[2] ({\gen_cdc_pntr.rpw_gray_reg_dc_n_3 ,\gen_cdc_pntr.rpw_gray_reg_dc_n_4 ,\gen_cdc_pntr.rpw_gray_reg_dc_n_5 }),
        .wr_clk(wr_clk),
        .wrst_busy(wrst_busy));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_counter_updn__parameterized1_13 wrpp1_inst
       (.E(wr_pntr_plus1_pf_carry),
        .Q(wr_pntr_plus1_pf),
        .\count_value_i_reg[3]_0 (wrpp1_inst_n_4),
        .\gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] (rd_pntr_wr),
        .wr_clk(wr_clk),
        .wrst_busy(wrst_busy));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_counter_updn__parameterized2_14 wrpp2_inst
       (.E(wr_pntr_plus1_pf_carry),
        .Q({wrpp2_inst_n_0,wrpp2_inst_n_1,wrpp2_inst_n_2,wrpp2_inst_n_3}),
        .wr_clk(wr_clk),
        .wrst_busy(wrst_busy));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_rst__xdcDup__1 xpm_fifo_rst_inst
       (.E(wr_pntr_plus1_pf_carry),
        .Q(curr_fwft_state),
        .SR(\grdc.rd_data_count_i0 ),
        .\count_value_i_reg[3] (full),
        .\gen_rst_ic.fifo_rd_rst_ic_reg_0 (rd_rst_busy),
        .\gen_rst_ic.fifo_rd_rst_ic_reg_1 (\gen_fwft.count_rst ),
        .\guf.underflow_i_reg (empty),
        .ram_empty_i(ram_empty_i),
        .rd_clk(rd_clk),
        .rd_en(rd_en),
        .rst(rst),
        .rst_d1(rst_d1),
        .underflow_i0(underflow_i0),
        .wr_clk(wr_clk),
        .wr_en(wr_en),
        .wr_rst_busy(wr_rst_busy),
        .wrst_busy(wrst_busy));
endmodule

(* CASCADE_HEIGHT = "0" *) (* CDC_DEST_SYNC_FF = "3" *) (* COMMON_CLOCK = "0" *) 
(* DOUT_RESET_VALUE = "0" *) (* ECC_MODE = "0" *) (* ENABLE_ECC = "0" *) 
(* EN_ADV_FEATURE = "16'b0000011100000111" *) (* EN_AE = "1'b0" *) (* EN_AF = "1'b0" *) 
(* EN_DVLD = "1'b0" *) (* EN_OF = "1'b1" *) (* EN_PE = "1'b1" *) 
(* EN_PF = "1'b1" *) (* EN_RDC = "1'b1" *) (* EN_UF = "1'b1" *) 
(* EN_WACK = "1'b0" *) (* EN_WDC = "1'b1" *) (* FG_EQ_ASYM_DOUT = "1'b0" *) 
(* FIFO_MEMORY_TYPE = "1" *) (* FIFO_MEM_TYPE = "1" *) (* FIFO_READ_DEPTH = "16" *) 
(* FIFO_READ_LATENCY = "1" *) (* FIFO_SIZE = "896" *) (* FIFO_WRITE_DEPTH = "16" *) 
(* FULL_RESET_VALUE = "0" *) (* FULL_RST_VAL = "1'b0" *) (* ORIG_REF_NAME = "xpm_fifo_base" *) 
(* PE_THRESH_ADJ = "8" *) (* PE_THRESH_MAX = "11" *) (* PE_THRESH_MIN = "5" *) 
(* PF_THRESH_ADJ = "8" *) (* PF_THRESH_MAX = "11" *) (* PF_THRESH_MIN = "8" *) 
(* PROG_EMPTY_THRESH = "10" *) (* PROG_FULL_THRESH = "10" *) (* RD_DATA_COUNT_WIDTH = "4" *) 
(* RD_DC_WIDTH_EXT = "5" *) (* RD_LATENCY = "2" *) (* RD_MODE = "1" *) 
(* RD_PNTR_WIDTH = "4" *) (* READ_DATA_WIDTH = "56" *) (* READ_MODE = "1" *) 
(* READ_MODE_LL = "1" *) (* RELATED_CLOCKS = "0" *) (* REMOVE_WR_RD_PROT_LOGIC = "0" *) 
(* SIM_ASSERT_CHK = "0" *) (* USE_ADV_FEATURES = "0707" *) (* VERSION = "0" *) 
(* WAKEUP_TIME = "0" *) (* WIDTH_RATIO = "1" *) (* WRITE_DATA_WIDTH = "56" *) 
(* WR_DATA_COUNT_WIDTH = "4" *) (* WR_DC_WIDTH_EXT = "5" *) (* WR_DEPTH_LOG = "4" *) 
(* WR_PNTR_WIDTH = "4" *) (* WR_RD_RATIO = "0" *) (* WR_WIDTH_LOG = "6" *) 
(* XPM_MODULE = "TRUE" *) (* both_stages_valid = "3" *) (* invalid = "0" *) 
(* keep_hierarchy = "soft" *) (* stage1_valid = "2" *) (* stage2_valid = "1" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_base__parameterized0
   (sleep,
    rst,
    wr_clk,
    wr_en,
    din,
    full,
    full_n,
    prog_full,
    wr_data_count,
    overflow,
    wr_rst_busy,
    almost_full,
    wr_ack,
    rd_clk,
    rd_en,
    dout,
    empty,
    prog_empty,
    rd_data_count,
    underflow,
    rd_rst_busy,
    almost_empty,
    data_valid,
    injectsbiterr,
    injectdbiterr,
    sbiterr,
    dbiterr);
  input sleep;
  input rst;
  input wr_clk;
  input wr_en;
  input [55:0]din;
  output full;
  output full_n;
  output prog_full;
  output [3:0]wr_data_count;
  output overflow;
  output wr_rst_busy;
  output almost_full;
  output wr_ack;
  input rd_clk;
  input rd_en;
  output [55:0]dout;
  output empty;
  output prog_empty;
  output [3:0]rd_data_count;
  output underflow;
  output rd_rst_busy;
  output almost_empty;
  output data_valid;
  input injectsbiterr;
  input injectdbiterr;
  output sbiterr;
  output dbiterr;

  wire \<const0> ;
  wire [1:0]count_value_i;
  wire [1:0]curr_fwft_state;
  wire [3:0]diff_pntr_pe;
  wire [4:4]diff_pntr_pf_q0;
  wire [55:0]din;
  wire [55:0]dout;
  wire empty;
  wire empty_fwft_i0;
  wire full;
  wire \gen_cdc_pntr.rpw_gray_reg_dc_n_3 ;
  wire \gen_cdc_pntr.rpw_gray_reg_dc_n_4 ;
  wire \gen_cdc_pntr.rpw_gray_reg_dc_n_5 ;
  wire \gen_cdc_pntr.wpr_gray_reg_dc_n_1 ;
  wire \gen_cdc_pntr.wpr_gray_reg_dc_n_2 ;
  wire \gen_cdc_pntr.wpr_gray_reg_dc_n_3 ;
  wire \gen_cdc_pntr.wpr_gray_reg_dc_n_4 ;
  wire \gen_cdc_pntr.wpr_gray_reg_dc_n_5 ;
  wire \gen_cdc_pntr.wpr_gray_reg_dc_n_6 ;
  wire \gen_cdc_pntr.wpr_gray_reg_n_2 ;
  wire \gen_cdc_pntr.wpr_gray_reg_n_3 ;
  wire \gen_cdc_pntr.wpr_gray_reg_n_4 ;
  wire \gen_cdc_pntr.wpr_gray_reg_n_5 ;
  wire \gen_fwft.count_rst ;
  wire \gen_fwft.ram_regout_en ;
  wire \gen_fwft.rdpp1_inst_n_3 ;
  wire \gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg_n_0_[0] ;
  wire \gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg_n_0_[1] ;
  wire \gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg_n_0_[2] ;
  wire \gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg_n_0_[3] ;
  wire \gen_pf_ic_rc.gpe_ic.prog_empty_i_i_1_n_0 ;
  wire [4:1]\grdc.diff_wr_rd_pntr_rdc ;
  wire \grdc.rd_data_count_i0 ;
  wire [4:1]\gwdc.diff_wr_rd_pntr1_out ;
  wire [1:0]next_fwft_state__0;
  wire overflow;
  wire overflow_i0;
  wire p_1_in__0;
  wire prog_empty;
  wire prog_full;
  wire ram_empty_i;
  wire ram_empty_i0;
  wire ram_full_i0;
  wire rd_clk;
  wire [3:0]rd_data_count;
  wire rd_en;
  wire [3:0]rd_pntr_ext;
  wire [3:3]rd_pntr_wr;
  wire [3:0]rd_pntr_wr_cdc;
  wire [4:0]rd_pntr_wr_cdc_dc;
  wire rd_rst_busy;
  wire rdp_inst_n_10;
  wire rdp_inst_n_11;
  wire rdp_inst_n_12;
  wire rdp_inst_n_8;
  wire rdp_inst_n_9;
  wire rdpp1_inst_n_0;
  wire rdpp1_inst_n_1;
  wire rdpp1_inst_n_2;
  wire rdpp1_inst_n_3;
  wire rst;
  wire rst_d1;
  wire rst_d1_inst_n_1;
  wire sleep;
  wire [1:1]src_in_bin00_out;
  wire underflow;
  wire underflow_i0;
  wire wr_clk;
  wire [3:0]wr_data_count;
  wire wr_en;
  wire [4:0]wr_pntr_ext;
  wire [4:1]wr_pntr_plus1_pf;
  wire wr_pntr_plus1_pf_carry;
  wire [3:0]wr_pntr_rd_cdc;
  wire [4:0]wr_pntr_rd_cdc_dc;
  wire wr_rst_busy;
  wire wrpp1_inst_n_4;
  wire wrpp2_inst_n_0;
  wire wrpp2_inst_n_1;
  wire wrpp2_inst_n_2;
  wire wrpp2_inst_n_3;
  wire wrst_busy;
  wire \NLW_gen_sdpram.xpm_memory_base_inst_dbiterra_UNCONNECTED ;
  wire \NLW_gen_sdpram.xpm_memory_base_inst_dbiterrb_UNCONNECTED ;
  wire \NLW_gen_sdpram.xpm_memory_base_inst_sbiterra_UNCONNECTED ;
  wire \NLW_gen_sdpram.xpm_memory_base_inst_sbiterrb_UNCONNECTED ;
  wire [55:0]\NLW_gen_sdpram.xpm_memory_base_inst_douta_UNCONNECTED ;

  assign almost_empty = \<const0> ;
  assign almost_full = \<const0> ;
  assign data_valid = \<const0> ;
  assign dbiterr = \<const0> ;
  assign full_n = \<const0> ;
  assign sbiterr = \<const0> ;
  assign wr_ack = \<const0> ;
  (* SOFT_HLUTNM = "soft_lutpair69" *) 
  LUT4 #(
    .INIT(16'h6A85)) 
    \FSM_sequential_gen_fwft.curr_fwft_state[0]_i_1 
       (.I0(curr_fwft_state[0]),
        .I1(rd_en),
        .I2(curr_fwft_state[1]),
        .I3(ram_empty_i),
        .O(next_fwft_state__0[0]));
  LUT3 #(
    .INIT(8'h7C)) 
    \FSM_sequential_gen_fwft.curr_fwft_state[1]_i_1 
       (.I0(rd_en),
        .I1(curr_fwft_state[1]),
        .I2(curr_fwft_state[0]),
        .O(next_fwft_state__0[1]));
  (* FSM_ENCODED_STATES = "invalid:00,stage1_valid:01,both_stages_valid:10,stage2_valid:11" *) 
  FDRE #(
    .INIT(1'b0)) 
    \FSM_sequential_gen_fwft.curr_fwft_state_reg[0] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(next_fwft_state__0[0]),
        .Q(curr_fwft_state[0]),
        .R(rd_rst_busy));
  (* FSM_ENCODED_STATES = "invalid:00,stage1_valid:01,both_stages_valid:10,stage2_valid:11" *) 
  FDRE #(
    .INIT(1'b0)) 
    \FSM_sequential_gen_fwft.curr_fwft_state_reg[1] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(next_fwft_state__0[1]),
        .Q(curr_fwft_state[1]),
        .R(rd_rst_busy));
  GND GND
       (.G(\<const0> ));
  (* DEST_SYNC_FF = "3" *) 
  (* INIT_SYNC_FF = "1" *) 
  (* REG_OUTPUT = "0" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* SIM_LOSSLESS_GRAY_CHK = "0" *) 
  (* VERSION = "0" *) 
  (* WIDTH = "5" *) 
  (* XPM_CDC = "GRAY" *) 
  (* XPM_MODULE = "TRUE" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_gray__parameterized1 \gen_cdc_pntr.rd_pntr_cdc_dc_inst 
       (.dest_clk(wr_clk),
        .dest_out_bin(rd_pntr_wr_cdc_dc),
        .src_clk(rd_clk),
        .src_in_bin({rdp_inst_n_9,rdp_inst_n_10,rdp_inst_n_11,src_in_bin00_out,rdp_inst_n_12}));
  (* DEST_SYNC_FF = "3" *) 
  (* INIT_SYNC_FF = "1" *) 
  (* REG_OUTPUT = "0" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* SIM_LOSSLESS_GRAY_CHK = "0" *) 
  (* VERSION = "0" *) 
  (* WIDTH = "4" *) 
  (* XPM_CDC = "GRAY" *) 
  (* XPM_MODULE = "TRUE" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_gray \gen_cdc_pntr.rd_pntr_cdc_inst 
       (.dest_clk(wr_clk),
        .dest_out_bin(rd_pntr_wr_cdc),
        .src_clk(rd_clk),
        .src_in_bin(rd_pntr_ext));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_reg_vec \gen_cdc_pntr.rpw_gray_reg 
       (.D(rd_pntr_wr_cdc),
        .E(wr_pntr_plus1_pf_carry),
        .Q(rd_pntr_wr),
        .diff_pntr_pf_q0(diff_pntr_pf_q0),
        .\gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] (wrpp1_inst_n_4),
        .\gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4]_0 (full),
        .\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg (wr_pntr_plus1_pf),
        .\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg_0 ({wrpp2_inst_n_0,wrpp2_inst_n_1,wrpp2_inst_n_2,wrpp2_inst_n_3}),
        .ram_full_i0(ram_full_i0),
        .rst_d1(rst_d1),
        .wr_clk(wr_clk),
        .wr_en(wr_en),
        .wrst_busy(wrst_busy));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_reg_vec__parameterized0 \gen_cdc_pntr.rpw_gray_reg_dc 
       (.D({\gwdc.diff_wr_rd_pntr1_out [4:3],\gwdc.diff_wr_rd_pntr1_out [1]}),
        .Q({\gen_cdc_pntr.rpw_gray_reg_dc_n_3 ,\gen_cdc_pntr.rpw_gray_reg_dc_n_4 ,\gen_cdc_pntr.rpw_gray_reg_dc_n_5 }),
        .\gwdc.wr_data_count_i_reg[4] (wr_pntr_ext),
        .\reg_out_i_reg[4]_0 (rd_pntr_wr_cdc_dc),
        .wr_clk(wr_clk),
        .wrst_busy(wrst_busy));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_reg_vec_0 \gen_cdc_pntr.wpr_gray_reg 
       (.D(diff_pntr_pe[1:0]),
        .Q({\gen_cdc_pntr.wpr_gray_reg_n_2 ,\gen_cdc_pntr.wpr_gray_reg_n_3 ,\gen_cdc_pntr.wpr_gray_reg_n_4 ,\gen_cdc_pntr.wpr_gray_reg_n_5 }),
        .enb(rdp_inst_n_8),
        .\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[0] (curr_fwft_state),
        .\gen_pf_ic_rc.ram_empty_i_reg (rd_pntr_ext),
        .\gen_pf_ic_rc.ram_empty_i_reg_0 ({rdpp1_inst_n_0,rdpp1_inst_n_1,rdpp1_inst_n_2,rdpp1_inst_n_3}),
        .ram_empty_i(ram_empty_i),
        .ram_empty_i0(ram_empty_i0),
        .rd_clk(rd_clk),
        .rd_en(rd_en),
        .\reg_out_i_reg[0]_0 (rd_rst_busy),
        .\reg_out_i_reg[3]_0 (wr_pntr_rd_cdc));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_reg_vec__parameterized0_1 \gen_cdc_pntr.wpr_gray_reg_dc 
       (.D(\grdc.diff_wr_rd_pntr_rdc [1]),
        .Q({\gen_cdc_pntr.wpr_gray_reg_dc_n_1 ,\gen_cdc_pntr.wpr_gray_reg_dc_n_2 ,\gen_cdc_pntr.wpr_gray_reg_dc_n_3 ,\gen_cdc_pntr.wpr_gray_reg_dc_n_4 ,\gen_cdc_pntr.wpr_gray_reg_dc_n_5 }),
        .\grdc.rd_data_count_i_reg[1] (count_value_i),
        .\grdc.rd_data_count_i_reg[4] ({rd_pntr_ext[3],rd_pntr_ext[1:0]}),
        .rd_clk(rd_clk),
        .\reg_out_i_reg[3]_0 (\gen_cdc_pntr.wpr_gray_reg_dc_n_6 ),
        .\reg_out_i_reg[4]_0 (rd_rst_busy),
        .\reg_out_i_reg[4]_1 (wr_pntr_rd_cdc_dc));
  (* DEST_SYNC_FF = "5" *) 
  (* INIT_SYNC_FF = "1" *) 
  (* REG_OUTPUT = "0" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* SIM_LOSSLESS_GRAY_CHK = "0" *) 
  (* VERSION = "0" *) 
  (* WIDTH = "5" *) 
  (* XPM_CDC = "GRAY" *) 
  (* XPM_MODULE = "TRUE" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_gray__parameterized0 \gen_cdc_pntr.wr_pntr_cdc_dc_inst 
       (.dest_clk(rd_clk),
        .dest_out_bin(wr_pntr_rd_cdc_dc),
        .src_clk(wr_clk),
        .src_in_bin(wr_pntr_ext));
  (* DEST_SYNC_FF = "3" *) 
  (* INIT_SYNC_FF = "1" *) 
  (* REG_OUTPUT = "0" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* SIM_LOSSLESS_GRAY_CHK = "0" *) 
  (* VERSION = "0" *) 
  (* WIDTH = "4" *) 
  (* XPM_CDC = "GRAY" *) 
  (* XPM_MODULE = "TRUE" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_gray__6 \gen_cdc_pntr.wr_pntr_cdc_inst 
       (.dest_clk(rd_clk),
        .dest_out_bin(wr_pntr_rd_cdc),
        .src_clk(wr_clk),
        .src_in_bin(wr_pntr_ext[3:0]));
  LUT4 #(
    .INIT(16'hF380)) 
    \gen_fwft.empty_fwft_i_i_1 
       (.I0(rd_en),
        .I1(curr_fwft_state[0]),
        .I2(curr_fwft_state[1]),
        .I3(empty),
        .O(empty_fwft_i0));
  FDSE #(
    .INIT(1'b1)) 
    \gen_fwft.empty_fwft_i_reg 
       (.C(rd_clk),
        .CE(1'b1),
        .D(empty_fwft_i0),
        .Q(empty),
        .S(rd_rst_busy));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_counter_updn \gen_fwft.rdpp1_inst 
       (.D(\grdc.diff_wr_rd_pntr_rdc [2]),
        .Q(count_value_i),
        .SR(\gen_fwft.count_rst ),
        .\count_value_i_reg[1]_0 (\gen_fwft.rdpp1_inst_n_3 ),
        .\count_value_i_reg[1]_1 (curr_fwft_state),
        .\grdc.rd_data_count_i_reg[2] ({\gen_cdc_pntr.wpr_gray_reg_dc_n_3 ,\gen_cdc_pntr.wpr_gray_reg_dc_n_4 ,\gen_cdc_pntr.wpr_gray_reg_dc_n_5 }),
        .\grdc.rd_data_count_i_reg[2]_0 (rd_pntr_ext[2:0]),
        .ram_empty_i(ram_empty_i),
        .rd_clk(rd_clk),
        .rd_en(rd_en),
        .src_in_bin(src_in_bin00_out));
  FDRE #(
    .INIT(1'b0)) 
    \gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[0] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(diff_pntr_pe[0]),
        .Q(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg_n_0_[0] ),
        .R(rd_rst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[1] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(diff_pntr_pe[1]),
        .Q(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg_n_0_[1] ),
        .R(rd_rst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[2] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(diff_pntr_pe[2]),
        .Q(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg_n_0_[2] ),
        .R(rd_rst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[3] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(diff_pntr_pe[3]),
        .Q(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg_n_0_[3] ),
        .R(rd_rst_busy));
  LUT6 #(
    .INIT(64'h8888888BBBBBBBBB)) 
    \gen_pf_ic_rc.gpe_ic.prog_empty_i_i_1 
       (.I0(prog_empty),
        .I1(empty),
        .I2(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg_n_0_[0] ),
        .I3(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg_n_0_[1] ),
        .I4(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg_n_0_[2] ),
        .I5(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg_n_0_[3] ),
        .O(\gen_pf_ic_rc.gpe_ic.prog_empty_i_i_1_n_0 ));
  FDSE #(
    .INIT(1'b1)) 
    \gen_pf_ic_rc.gpe_ic.prog_empty_i_reg 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\gen_pf_ic_rc.gpe_ic.prog_empty_i_i_1_n_0 ),
        .Q(prog_empty),
        .S(rd_rst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(diff_pntr_pf_q0),
        .Q(p_1_in__0),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \gen_pf_ic_rc.gpf_ic.prog_full_i_reg 
       (.C(wr_clk),
        .CE(1'b1),
        .D(rst_d1_inst_n_1),
        .Q(prog_full),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg 
       (.C(wr_clk),
        .CE(1'b1),
        .D(ram_full_i0),
        .Q(full),
        .R(wrst_busy));
  FDSE #(
    .INIT(1'b1)) 
    \gen_pf_ic_rc.ram_empty_i_reg 
       (.C(rd_clk),
        .CE(1'b1),
        .D(ram_empty_i0),
        .Q(ram_empty_i),
        .S(rd_rst_busy));
  (* ADDR_WIDTH_A = "4" *) 
  (* ADDR_WIDTH_B = "4" *) 
  (* AUTO_SLEEP_TIME = "0" *) 
  (* BYTE_WRITE_WIDTH_A = "56" *) 
  (* BYTE_WRITE_WIDTH_B = "56" *) 
  (* CASCADE_HEIGHT = "0" *) 
  (* CLOCKING_MODE = "1" *) 
  (* ECC_MODE = "0" *) 
  (* KEEP_HIERARCHY = "soft" *) 
  (* MAX_NUM_CHAR = "0" *) 
  (* MEMORY_INIT_FILE = "none" *) 
  (* MEMORY_INIT_PARAM = "" *) 
  (* MEMORY_OPTIMIZATION = "true" *) 
  (* MEMORY_PRIMITIVE = "1" *) 
  (* MEMORY_SIZE = "896" *) 
  (* MEMORY_TYPE = "1" *) 
  (* MESSAGE_CONTROL = "0" *) 
  (* NUM_CHAR_LOC = "0" *) 
  (* P_ECC_MODE = "no_ecc" *) 
  (* P_ENABLE_BYTE_WRITE_A = "0" *) 
  (* P_ENABLE_BYTE_WRITE_B = "0" *) 
  (* P_MAX_DEPTH_DATA = "16" *) 
  (* P_MEMORY_OPT = "yes" *) 
  (* P_MEMORY_PRIMITIVE = "distributed" *) 
  (* P_MIN_WIDTH_DATA = "56" *) 
  (* P_MIN_WIDTH_DATA_A = "56" *) 
  (* P_MIN_WIDTH_DATA_B = "56" *) 
  (* P_MIN_WIDTH_DATA_ECC = "56" *) 
  (* P_MIN_WIDTH_DATA_LDW = "4" *) 
  (* P_MIN_WIDTH_DATA_SHFT = "56" *) 
  (* P_NUM_COLS_WRITE_A = "1" *) 
  (* P_NUM_COLS_WRITE_B = "1" *) 
  (* P_NUM_ROWS_READ_A = "1" *) 
  (* P_NUM_ROWS_READ_B = "1" *) 
  (* P_NUM_ROWS_WRITE_A = "1" *) 
  (* P_NUM_ROWS_WRITE_B = "1" *) 
  (* P_SDP_WRITE_MODE = "yes" *) 
  (* P_WIDTH_ADDR_LSB_READ_A = "0" *) 
  (* P_WIDTH_ADDR_LSB_READ_B = "0" *) 
  (* P_WIDTH_ADDR_LSB_WRITE_A = "0" *) 
  (* P_WIDTH_ADDR_LSB_WRITE_B = "0" *) 
  (* P_WIDTH_ADDR_READ_A = "4" *) 
  (* P_WIDTH_ADDR_READ_B = "4" *) 
  (* P_WIDTH_ADDR_WRITE_A = "4" *) 
  (* P_WIDTH_ADDR_WRITE_B = "4" *) 
  (* P_WIDTH_COL_WRITE_A = "56" *) 
  (* P_WIDTH_COL_WRITE_B = "56" *) 
  (* READ_DATA_WIDTH_A = "56" *) 
  (* READ_DATA_WIDTH_B = "56" *) 
  (* READ_LATENCY_A = "2" *) 
  (* READ_LATENCY_B = "2" *) 
  (* READ_RESET_VALUE_A = "0" *) 
  (* READ_RESET_VALUE_B = "0" *) 
  (* RST_MODE_A = "SYNC" *) 
  (* RST_MODE_B = "SYNC" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* USE_EMBEDDED_CONSTRAINT = "1" *) 
  (* USE_MEM_INIT = "0" *) 
  (* USE_MEM_INIT_MMI = "0" *) 
  (* VERSION = "0" *) 
  (* WAKEUP_TIME = "0" *) 
  (* WRITE_DATA_WIDTH_A = "56" *) 
  (* WRITE_DATA_WIDTH_B = "56" *) 
  (* WRITE_MODE_A = "2" *) 
  (* WRITE_MODE_B = "1" *) 
  (* WRITE_PROTECT = "1" *) 
  (* XPM_MODULE = "TRUE" *) 
  (* rsta_loop_iter = "56" *) 
  (* rstb_loop_iter = "56" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_memory_base__parameterized0 \gen_sdpram.xpm_memory_base_inst 
       (.addra(wr_pntr_ext[3:0]),
        .addrb(rd_pntr_ext),
        .clka(wr_clk),
        .clkb(rd_clk),
        .dbiterra(\NLW_gen_sdpram.xpm_memory_base_inst_dbiterra_UNCONNECTED ),
        .dbiterrb(\NLW_gen_sdpram.xpm_memory_base_inst_dbiterrb_UNCONNECTED ),
        .dina(din),
        .dinb({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .douta(\NLW_gen_sdpram.xpm_memory_base_inst_douta_UNCONNECTED [55:0]),
        .doutb(dout),
        .ena(wr_pntr_plus1_pf_carry),
        .enb(rdp_inst_n_8),
        .injectdbiterra(1'b0),
        .injectdbiterrb(1'b0),
        .injectsbiterra(1'b0),
        .injectsbiterrb(1'b0),
        .regcea(1'b0),
        .regceb(\gen_fwft.ram_regout_en ),
        .rsta(1'b0),
        .rstb(rd_rst_busy),
        .sbiterra(\NLW_gen_sdpram.xpm_memory_base_inst_sbiterra_UNCONNECTED ),
        .sbiterrb(\NLW_gen_sdpram.xpm_memory_base_inst_sbiterrb_UNCONNECTED ),
        .sleep(sleep),
        .wea(1'b0),
        .web(1'b0));
  (* SOFT_HLUTNM = "soft_lutpair69" *) 
  LUT3 #(
    .INIT(8'h62)) 
    \gen_sdpram.xpm_memory_base_inst_i_3 
       (.I0(curr_fwft_state[0]),
        .I1(curr_fwft_state[1]),
        .I2(rd_en),
        .O(\gen_fwft.ram_regout_en ));
  FDRE #(
    .INIT(1'b0)) 
    \gof.overflow_i_reg 
       (.C(wr_clk),
        .CE(1'b1),
        .D(overflow_i0),
        .Q(overflow),
        .R(1'b0));
  FDRE \grdc.rd_data_count_i_reg[1] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\grdc.diff_wr_rd_pntr_rdc [1]),
        .Q(rd_data_count[0]),
        .R(\grdc.rd_data_count_i0 ));
  FDRE \grdc.rd_data_count_i_reg[2] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\grdc.diff_wr_rd_pntr_rdc [2]),
        .Q(rd_data_count[1]),
        .R(\grdc.rd_data_count_i0 ));
  FDRE \grdc.rd_data_count_i_reg[3] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\grdc.diff_wr_rd_pntr_rdc [3]),
        .Q(rd_data_count[2]),
        .R(\grdc.rd_data_count_i0 ));
  FDRE \grdc.rd_data_count_i_reg[4] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\grdc.diff_wr_rd_pntr_rdc [4]),
        .Q(rd_data_count[3]),
        .R(\grdc.rd_data_count_i0 ));
  FDRE #(
    .INIT(1'b0)) 
    \guf.underflow_i_reg 
       (.C(rd_clk),
        .CE(1'b1),
        .D(underflow_i0),
        .Q(underflow),
        .R(1'b0));
  FDRE \gwdc.wr_data_count_i_reg[1] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\gwdc.diff_wr_rd_pntr1_out [1]),
        .Q(wr_data_count[0]),
        .R(wrst_busy));
  FDRE \gwdc.wr_data_count_i_reg[2] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\gwdc.diff_wr_rd_pntr1_out [2]),
        .Q(wr_data_count[1]),
        .R(wrst_busy));
  FDRE \gwdc.wr_data_count_i_reg[3] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\gwdc.diff_wr_rd_pntr1_out [3]),
        .Q(wr_data_count[2]),
        .R(wrst_busy));
  FDRE \gwdc.wr_data_count_i_reg[4] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\gwdc.diff_wr_rd_pntr1_out [4]),
        .Q(wr_data_count[3]),
        .R(wrst_busy));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_counter_updn__parameterized0 rdp_inst
       (.D(diff_pntr_pe[3:2]),
        .E(rdp_inst_n_8),
        .Q(rd_pntr_ext),
        .\count_value_i_reg[0]_0 (curr_fwft_state),
        .\count_value_i_reg[4]_0 (rd_rst_busy),
        .\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[3] ({\gen_cdc_pntr.wpr_gray_reg_n_2 ,\gen_cdc_pntr.wpr_gray_reg_n_3 ,\gen_cdc_pntr.wpr_gray_reg_n_4 ,\gen_cdc_pntr.wpr_gray_reg_n_5 }),
        .\grdc.rd_data_count_i_reg[4] (\gen_fwft.rdpp1_inst_n_3 ),
        .\grdc.rd_data_count_i_reg[4]_0 ({\gen_cdc_pntr.wpr_gray_reg_dc_n_1 ,\gen_cdc_pntr.wpr_gray_reg_dc_n_2 ,\gen_cdc_pntr.wpr_gray_reg_dc_n_3 ,\gen_cdc_pntr.wpr_gray_reg_dc_n_4 }),
        .\grdc.rd_data_count_i_reg[4]_1 (\gen_cdc_pntr.wpr_gray_reg_dc_n_6 ),
        .ram_empty_i(ram_empty_i),
        .rd_clk(rd_clk),
        .rd_en(rd_en),
        .\reg_out_i_reg[2] (\grdc.diff_wr_rd_pntr_rdc [4:3]),
        .\src_gray_ff_reg[4] (count_value_i),
        .src_in_bin({rdp_inst_n_9,rdp_inst_n_10,rdp_inst_n_11,rdp_inst_n_12}));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_counter_updn__parameterized1 rdpp1_inst
       (.E(rdp_inst_n_8),
        .Q({rdpp1_inst_n_0,rdpp1_inst_n_1,rdpp1_inst_n_2,rdpp1_inst_n_3}),
        .\count_value_i_reg[0]_0 (rd_rst_busy),
        .\count_value_i_reg[1]_0 (curr_fwft_state),
        .rd_clk(rd_clk),
        .rd_en(rd_en));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_reg_bit rst_d1_inst
       (.\gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] (rst_d1_inst_n_1),
        .\gen_pf_ic_rc.gpf_ic.prog_full_i_reg (full),
        .overflow_i0(overflow_i0),
        .p_1_in__0(p_1_in__0),
        .prog_full(prog_full),
        .rst(rst),
        .rst_d1(rst_d1),
        .wr_clk(wr_clk),
        .wr_en(wr_en),
        .wrst_busy(wrst_busy));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_counter_updn__parameterized0_2 wrp_inst
       (.D(\gwdc.diff_wr_rd_pntr1_out [2]),
        .E(wr_pntr_plus1_pf_carry),
        .Q(wr_pntr_ext),
        .\gwdc.wr_data_count_i_reg[2] ({\gen_cdc_pntr.rpw_gray_reg_dc_n_3 ,\gen_cdc_pntr.rpw_gray_reg_dc_n_4 ,\gen_cdc_pntr.rpw_gray_reg_dc_n_5 }),
        .wr_clk(wr_clk),
        .wrst_busy(wrst_busy));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_counter_updn__parameterized1_3 wrpp1_inst
       (.E(wr_pntr_plus1_pf_carry),
        .Q(wr_pntr_plus1_pf),
        .\count_value_i_reg[3]_0 (wrpp1_inst_n_4),
        .\gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] (rd_pntr_wr),
        .wr_clk(wr_clk),
        .wrst_busy(wrst_busy));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_counter_updn__parameterized2 wrpp2_inst
       (.E(wr_pntr_plus1_pf_carry),
        .Q({wrpp2_inst_n_0,wrpp2_inst_n_1,wrpp2_inst_n_2,wrpp2_inst_n_3}),
        .wr_clk(wr_clk),
        .wrst_busy(wrst_busy));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_rst xpm_fifo_rst_inst
       (.E(wr_pntr_plus1_pf_carry),
        .Q(curr_fwft_state),
        .SR(\grdc.rd_data_count_i0 ),
        .\count_value_i_reg[3] (full),
        .\gen_rst_ic.fifo_rd_rst_ic_reg_0 (rd_rst_busy),
        .\gen_rst_ic.fifo_rd_rst_ic_reg_1 (\gen_fwft.count_rst ),
        .\guf.underflow_i_reg (empty),
        .ram_empty_i(ram_empty_i),
        .rd_clk(rd_clk),
        .rd_en(rd_en),
        .rst(rst),
        .rst_d1(rst_d1),
        .underflow_i0(underflow_i0),
        .wr_clk(wr_clk),
        .wr_en(wr_en),
        .wr_rst_busy(wr_rst_busy),
        .wrst_busy(wrst_busy));
endmodule

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_reg_bit
   (rst_d1,
    \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] ,
    overflow_i0,
    wrst_busy,
    wr_clk,
    p_1_in__0,
    \gen_pf_ic_rc.gpf_ic.prog_full_i_reg ,
    prog_full,
    rst,
    wr_en);
  output rst_d1;
  output \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] ;
  output overflow_i0;
  input wrst_busy;
  input wr_clk;
  input p_1_in__0;
  input \gen_pf_ic_rc.gpf_ic.prog_full_i_reg ;
  input prog_full;
  input rst;
  input wr_en;

  wire \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] ;
  wire \gen_pf_ic_rc.gpf_ic.prog_full_i_reg ;
  wire overflow_i0;
  wire p_1_in__0;
  wire prog_full;
  wire rst;
  wire rst_d1;
  wire wr_clk;
  wire wr_en;
  wire wrst_busy;

  FDRE #(
    .INIT(1'b0)) 
    d_out_reg
       (.C(wr_clk),
        .CE(1'b1),
        .D(wrst_busy),
        .Q(rst_d1),
        .R(1'b0));
  LUT6 #(
    .INIT(64'h00000000E200E2E2)) 
    \gen_pf_ic_rc.gpf_ic.prog_full_i_i_1 
       (.I0(p_1_in__0),
        .I1(\gen_pf_ic_rc.gpf_ic.prog_full_i_reg ),
        .I2(prog_full),
        .I3(rst),
        .I4(rst_d1),
        .I5(wrst_busy),
        .O(\gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] ));
  LUT4 #(
    .INIT(16'hFE00)) 
    \gof.overflow_i_i_1 
       (.I0(rst_d1),
        .I1(wrst_busy),
        .I2(\gen_pf_ic_rc.gpf_ic.prog_full_i_reg ),
        .I3(wr_en),
        .O(overflow_i0));
endmodule

(* ORIG_REF_NAME = "xpm_fifo_reg_bit" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_reg_bit_11
   (rst_d1,
    \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] ,
    overflow_i0,
    wrst_busy,
    wr_clk,
    p_1_in__0,
    \gen_pf_ic_rc.gpf_ic.prog_full_i_reg ,
    prog_full,
    rst,
    wr_en);
  output rst_d1;
  output \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] ;
  output overflow_i0;
  input wrst_busy;
  input wr_clk;
  input p_1_in__0;
  input \gen_pf_ic_rc.gpf_ic.prog_full_i_reg ;
  input prog_full;
  input rst;
  input wr_en;

  wire \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] ;
  wire \gen_pf_ic_rc.gpf_ic.prog_full_i_reg ;
  wire overflow_i0;
  wire p_1_in__0;
  wire prog_full;
  wire rst;
  wire rst_d1;
  wire wr_clk;
  wire wr_en;
  wire wrst_busy;

  FDRE #(
    .INIT(1'b0)) 
    d_out_reg
       (.C(wr_clk),
        .CE(1'b1),
        .D(wrst_busy),
        .Q(rst_d1),
        .R(1'b0));
  LUT6 #(
    .INIT(64'h00000000E200E2E2)) 
    \gen_pf_ic_rc.gpf_ic.prog_full_i_i_1 
       (.I0(p_1_in__0),
        .I1(\gen_pf_ic_rc.gpf_ic.prog_full_i_reg ),
        .I2(prog_full),
        .I3(rst),
        .I4(rst_d1),
        .I5(wrst_busy),
        .O(\gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] ));
  LUT4 #(
    .INIT(16'hFE00)) 
    \gof.overflow_i_i_1 
       (.I0(rst_d1),
        .I1(wrst_busy),
        .I2(\gen_pf_ic_rc.gpf_ic.prog_full_i_reg ),
        .I3(wr_en),
        .O(overflow_i0));
endmodule

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_reg_vec
   (diff_pntr_pf_q0,
    Q,
    ram_full_i0,
    \gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg ,
    \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] ,
    E,
    \gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg_0 ,
    rst_d1,
    wrst_busy,
    \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4]_0 ,
    wr_en,
    D,
    wr_clk);
  output [0:0]diff_pntr_pf_q0;
  output [0:0]Q;
  output ram_full_i0;
  input [3:0]\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg ;
  input \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] ;
  input [0:0]E;
  input [3:0]\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg_0 ;
  input rst_d1;
  input wrst_busy;
  input \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4]_0 ;
  input wr_en;
  input [3:0]D;
  input wr_clk;

  wire [3:0]D;
  wire [0:0]E;
  wire [0:0]Q;
  wire [0:0]diff_pntr_pf_q0;
  wire \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q[4]_i_2_n_0 ;
  wire \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] ;
  wire \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4]_0 ;
  wire \gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_i_2_n_0 ;
  wire \gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_i_3_n_0 ;
  wire [3:0]\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg ;
  wire [3:0]\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg_0 ;
  wire ram_full_i0;
  wire [2:0]rd_pntr_wr;
  wire rst_d1;
  wire wr_clk;
  wire wr_en;
  wire wrst_busy;

  LUT6 #(
    .INIT(64'h2BFF002BD400FFD4)) 
    \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q[4]_i_1 
       (.I0(rd_pntr_wr[1]),
        .I1(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg [1]),
        .I2(\gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q[4]_i_2_n_0 ),
        .I3(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg [2]),
        .I4(rd_pntr_wr[2]),
        .I5(\gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] ),
        .O(diff_pntr_pf_q0));
  LUT6 #(
    .INIT(64'h4444444D44444444)) 
    \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q[4]_i_2 
       (.I0(rd_pntr_wr[0]),
        .I1(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg [0]),
        .I2(rst_d1),
        .I3(wrst_busy),
        .I4(\gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4]_0 ),
        .I5(wr_en),
        .O(\gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q[4]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFF8080802020FF20)) 
    \gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_i_1 
       (.I0(E),
        .I1(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg_0 [3]),
        .I2(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_i_2_n_0 ),
        .I3(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_i_3_n_0 ),
        .I4(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg [3]),
        .I5(Q),
        .O(ram_full_i0));
  LUT6 #(
    .INIT(64'h9009000000009009)) 
    \gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_i_2 
       (.I0(rd_pntr_wr[0]),
        .I1(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg_0 [0]),
        .I2(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg_0 [2]),
        .I3(rd_pntr_wr[2]),
        .I4(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg_0 [1]),
        .I5(rd_pntr_wr[1]),
        .O(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_i_2_n_0 ));
  LUT6 #(
    .INIT(64'h9009000000009009)) 
    \gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_i_3 
       (.I0(rd_pntr_wr[0]),
        .I1(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg [0]),
        .I2(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg [2]),
        .I3(rd_pntr_wr[2]),
        .I4(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg [1]),
        .I5(rd_pntr_wr[1]),
        .O(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_i_3_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[0] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(D[0]),
        .Q(rd_pntr_wr[0]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[1] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(D[1]),
        .Q(rd_pntr_wr[1]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[2] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(D[2]),
        .Q(rd_pntr_wr[2]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[3] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(D[3]),
        .Q(Q),
        .R(wrst_busy));
endmodule

(* ORIG_REF_NAME = "xpm_fifo_reg_vec" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_reg_vec_0
   (D,
    Q,
    ram_empty_i0,
    enb,
    \gen_pf_ic_rc.ram_empty_i_reg ,
    \gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[0] ,
    rd_en,
    ram_empty_i,
    \gen_pf_ic_rc.ram_empty_i_reg_0 ,
    \reg_out_i_reg[0]_0 ,
    \reg_out_i_reg[3]_0 ,
    rd_clk);
  output [1:0]D;
  output [3:0]Q;
  output ram_empty_i0;
  input enb;
  input [3:0]\gen_pf_ic_rc.ram_empty_i_reg ;
  input [1:0]\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[0] ;
  input rd_en;
  input ram_empty_i;
  input [3:0]\gen_pf_ic_rc.ram_empty_i_reg_0 ;
  input \reg_out_i_reg[0]_0 ;
  input [3:0]\reg_out_i_reg[3]_0 ;
  input rd_clk;

  wire [1:0]D;
  wire [3:0]Q;
  wire enb;
  wire [1:0]\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[0] ;
  wire \gen_pf_ic_rc.ram_empty_i_i_2_n_0 ;
  wire \gen_pf_ic_rc.ram_empty_i_i_3_n_0 ;
  wire [3:0]\gen_pf_ic_rc.ram_empty_i_reg ;
  wire [3:0]\gen_pf_ic_rc.ram_empty_i_reg_0 ;
  wire ram_empty_i;
  wire ram_empty_i0;
  wire rd_clk;
  wire rd_en;
  wire \reg_out_i_reg[0]_0 ;
  wire [3:0]\reg_out_i_reg[3]_0 ;

  LUT6 #(
    .INIT(64'h6666666699999969)) 
    \gen_pf_ic_rc.gpe_ic.diff_pntr_pe[0]_i_1 
       (.I0(Q[0]),
        .I1(\gen_pf_ic_rc.ram_empty_i_reg [0]),
        .I2(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[0] [1]),
        .I3(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[0] [0]),
        .I4(rd_en),
        .I5(ram_empty_i),
        .O(D[0]));
  LUT5 #(
    .INIT(32'hD42B2BD4)) 
    \gen_pf_ic_rc.gpe_ic.diff_pntr_pe[1]_i_1 
       (.I0(Q[0]),
        .I1(enb),
        .I2(\gen_pf_ic_rc.ram_empty_i_reg [0]),
        .I3(Q[1]),
        .I4(\gen_pf_ic_rc.ram_empty_i_reg [1]),
        .O(D[1]));
  LUT6 #(
    .INIT(64'hFF8080802020FF20)) 
    \gen_pf_ic_rc.ram_empty_i_i_1 
       (.I0(enb),
        .I1(\gen_pf_ic_rc.ram_empty_i_reg_0 [3]),
        .I2(\gen_pf_ic_rc.ram_empty_i_i_2_n_0 ),
        .I3(\gen_pf_ic_rc.ram_empty_i_i_3_n_0 ),
        .I4(\gen_pf_ic_rc.ram_empty_i_reg [3]),
        .I5(Q[3]),
        .O(ram_empty_i0));
  LUT6 #(
    .INIT(64'h9009000000009009)) 
    \gen_pf_ic_rc.ram_empty_i_i_2 
       (.I0(Q[0]),
        .I1(\gen_pf_ic_rc.ram_empty_i_reg_0 [0]),
        .I2(\gen_pf_ic_rc.ram_empty_i_reg_0 [2]),
        .I3(Q[2]),
        .I4(\gen_pf_ic_rc.ram_empty_i_reg_0 [1]),
        .I5(Q[1]),
        .O(\gen_pf_ic_rc.ram_empty_i_i_2_n_0 ));
  LUT6 #(
    .INIT(64'h9009000000009009)) 
    \gen_pf_ic_rc.ram_empty_i_i_3 
       (.I0(Q[0]),
        .I1(\gen_pf_ic_rc.ram_empty_i_reg [0]),
        .I2(\gen_pf_ic_rc.ram_empty_i_reg [2]),
        .I3(Q[2]),
        .I4(\gen_pf_ic_rc.ram_empty_i_reg [1]),
        .I5(Q[1]),
        .O(\gen_pf_ic_rc.ram_empty_i_i_3_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[0] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[3]_0 [0]),
        .Q(Q[0]),
        .R(\reg_out_i_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[1] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[3]_0 [1]),
        .Q(Q[1]),
        .R(\reg_out_i_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[2] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[3]_0 [2]),
        .Q(Q[2]),
        .R(\reg_out_i_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[3] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[3]_0 [3]),
        .Q(Q[3]),
        .R(\reg_out_i_reg[0]_0 ));
endmodule

(* ORIG_REF_NAME = "xpm_fifo_reg_vec" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_reg_vec_4
   (diff_pntr_pf_q0,
    Q,
    ram_full_i0,
    \gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg ,
    \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] ,
    E,
    \gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg_0 ,
    rst_d1,
    wrst_busy,
    \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4]_0 ,
    wr_en,
    D,
    wr_clk);
  output [0:0]diff_pntr_pf_q0;
  output [0:0]Q;
  output ram_full_i0;
  input [3:0]\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg ;
  input \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] ;
  input [0:0]E;
  input [3:0]\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg_0 ;
  input rst_d1;
  input wrst_busy;
  input \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4]_0 ;
  input wr_en;
  input [3:0]D;
  input wr_clk;

  wire [3:0]D;
  wire [0:0]E;
  wire [0:0]Q;
  wire [0:0]diff_pntr_pf_q0;
  wire \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q[4]_i_2_n_0 ;
  wire \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] ;
  wire \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4]_0 ;
  wire \gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_i_2_n_0 ;
  wire \gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_i_3_n_0 ;
  wire [3:0]\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg ;
  wire [3:0]\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg_0 ;
  wire ram_full_i0;
  wire [2:0]rd_pntr_wr;
  wire rst_d1;
  wire wr_clk;
  wire wr_en;
  wire wrst_busy;

  LUT6 #(
    .INIT(64'h2BFF002BD400FFD4)) 
    \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q[4]_i_1 
       (.I0(rd_pntr_wr[1]),
        .I1(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg [1]),
        .I2(\gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q[4]_i_2_n_0 ),
        .I3(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg [2]),
        .I4(rd_pntr_wr[2]),
        .I5(\gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4] ),
        .O(diff_pntr_pf_q0));
  LUT6 #(
    .INIT(64'h4444444D44444444)) 
    \gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q[4]_i_2 
       (.I0(rd_pntr_wr[0]),
        .I1(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg [0]),
        .I2(rst_d1),
        .I3(wrst_busy),
        .I4(\gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q_reg[4]_0 ),
        .I5(wr_en),
        .O(\gen_pf_ic_rc.gpf_ic.diff_pntr_pf_q[4]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hFF8080802020FF20)) 
    \gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_i_1 
       (.I0(E),
        .I1(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg_0 [3]),
        .I2(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_i_2_n_0 ),
        .I3(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_i_3_n_0 ),
        .I4(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg [3]),
        .I5(Q),
        .O(ram_full_i0));
  LUT6 #(
    .INIT(64'h9009000000009009)) 
    \gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_i_2 
       (.I0(rd_pntr_wr[0]),
        .I1(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg_0 [0]),
        .I2(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg_0 [2]),
        .I3(rd_pntr_wr[2]),
        .I4(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg_0 [1]),
        .I5(rd_pntr_wr[1]),
        .O(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_i_2_n_0 ));
  LUT6 #(
    .INIT(64'h9009000000009009)) 
    \gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_i_3 
       (.I0(rd_pntr_wr[0]),
        .I1(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg [0]),
        .I2(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg [2]),
        .I3(rd_pntr_wr[2]),
        .I4(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_reg [1]),
        .I5(rd_pntr_wr[1]),
        .O(\gen_pf_ic_rc.ngen_full_rst_val.ram_full_i_i_3_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[0] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(D[0]),
        .Q(rd_pntr_wr[0]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[1] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(D[1]),
        .Q(rd_pntr_wr[1]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[2] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(D[2]),
        .Q(rd_pntr_wr[2]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[3] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(D[3]),
        .Q(Q),
        .R(wrst_busy));
endmodule

(* ORIG_REF_NAME = "xpm_fifo_reg_vec" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_reg_vec_6
   (D,
    Q,
    ram_empty_i0,
    enb,
    \gen_pf_ic_rc.ram_empty_i_reg ,
    \gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[0] ,
    rd_en,
    ram_empty_i,
    \gen_pf_ic_rc.ram_empty_i_reg_0 ,
    \reg_out_i_reg[0]_0 ,
    \reg_out_i_reg[3]_0 ,
    rd_clk);
  output [1:0]D;
  output [3:0]Q;
  output ram_empty_i0;
  input enb;
  input [3:0]\gen_pf_ic_rc.ram_empty_i_reg ;
  input [1:0]\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[0] ;
  input rd_en;
  input ram_empty_i;
  input [3:0]\gen_pf_ic_rc.ram_empty_i_reg_0 ;
  input \reg_out_i_reg[0]_0 ;
  input [3:0]\reg_out_i_reg[3]_0 ;
  input rd_clk;

  wire [1:0]D;
  wire [3:0]Q;
  wire enb;
  wire [1:0]\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[0] ;
  wire \gen_pf_ic_rc.ram_empty_i_i_2_n_0 ;
  wire \gen_pf_ic_rc.ram_empty_i_i_3_n_0 ;
  wire [3:0]\gen_pf_ic_rc.ram_empty_i_reg ;
  wire [3:0]\gen_pf_ic_rc.ram_empty_i_reg_0 ;
  wire ram_empty_i;
  wire ram_empty_i0;
  wire rd_clk;
  wire rd_en;
  wire \reg_out_i_reg[0]_0 ;
  wire [3:0]\reg_out_i_reg[3]_0 ;

  LUT6 #(
    .INIT(64'h6666666699999969)) 
    \gen_pf_ic_rc.gpe_ic.diff_pntr_pe[0]_i_1 
       (.I0(Q[0]),
        .I1(\gen_pf_ic_rc.ram_empty_i_reg [0]),
        .I2(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[0] [1]),
        .I3(\gen_pf_ic_rc.gpe_ic.diff_pntr_pe_reg[0] [0]),
        .I4(rd_en),
        .I5(ram_empty_i),
        .O(D[0]));
  LUT5 #(
    .INIT(32'hD42B2BD4)) 
    \gen_pf_ic_rc.gpe_ic.diff_pntr_pe[1]_i_1 
       (.I0(Q[0]),
        .I1(enb),
        .I2(\gen_pf_ic_rc.ram_empty_i_reg [0]),
        .I3(Q[1]),
        .I4(\gen_pf_ic_rc.ram_empty_i_reg [1]),
        .O(D[1]));
  LUT6 #(
    .INIT(64'hFF8080802020FF20)) 
    \gen_pf_ic_rc.ram_empty_i_i_1 
       (.I0(enb),
        .I1(\gen_pf_ic_rc.ram_empty_i_reg_0 [3]),
        .I2(\gen_pf_ic_rc.ram_empty_i_i_2_n_0 ),
        .I3(\gen_pf_ic_rc.ram_empty_i_i_3_n_0 ),
        .I4(\gen_pf_ic_rc.ram_empty_i_reg [3]),
        .I5(Q[3]),
        .O(ram_empty_i0));
  LUT6 #(
    .INIT(64'h9009000000009009)) 
    \gen_pf_ic_rc.ram_empty_i_i_2 
       (.I0(Q[0]),
        .I1(\gen_pf_ic_rc.ram_empty_i_reg_0 [0]),
        .I2(\gen_pf_ic_rc.ram_empty_i_reg_0 [2]),
        .I3(Q[2]),
        .I4(\gen_pf_ic_rc.ram_empty_i_reg_0 [1]),
        .I5(Q[1]),
        .O(\gen_pf_ic_rc.ram_empty_i_i_2_n_0 ));
  LUT6 #(
    .INIT(64'h9009000000009009)) 
    \gen_pf_ic_rc.ram_empty_i_i_3 
       (.I0(Q[0]),
        .I1(\gen_pf_ic_rc.ram_empty_i_reg [0]),
        .I2(\gen_pf_ic_rc.ram_empty_i_reg [2]),
        .I3(Q[2]),
        .I4(\gen_pf_ic_rc.ram_empty_i_reg [1]),
        .I5(Q[1]),
        .O(\gen_pf_ic_rc.ram_empty_i_i_3_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[0] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[3]_0 [0]),
        .Q(Q[0]),
        .R(\reg_out_i_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[1] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[3]_0 [1]),
        .Q(Q[1]),
        .R(\reg_out_i_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[2] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[3]_0 [2]),
        .Q(Q[2]),
        .R(\reg_out_i_reg[0]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[3] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[3]_0 [3]),
        .Q(Q[3]),
        .R(\reg_out_i_reg[0]_0 ));
endmodule

(* ORIG_REF_NAME = "xpm_fifo_reg_vec" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_reg_vec__parameterized0
   (D,
    Q,
    \gwdc.wr_data_count_i_reg[4] ,
    wrst_busy,
    \reg_out_i_reg[4]_0 ,
    wr_clk);
  output [2:0]D;
  output [2:0]Q;
  input [4:0]\gwdc.wr_data_count_i_reg[4] ;
  input wrst_busy;
  input [4:0]\reg_out_i_reg[4]_0 ;
  input wr_clk;

  wire [2:0]D;
  wire [2:0]Q;
  wire \gwdc.wr_data_count_i[4]_i_2_n_0 ;
  wire [4:0]\gwdc.wr_data_count_i_reg[4] ;
  wire [4:0]\reg_out_i_reg[4]_0 ;
  wire \reg_out_i_reg_n_0_[3] ;
  wire \reg_out_i_reg_n_0_[4] ;
  wire wr_clk;
  wire wrst_busy;

  LUT4 #(
    .INIT(16'h2DD2)) 
    \gwdc.wr_data_count_i[1]_i_1 
       (.I0(Q[0]),
        .I1(\gwdc.wr_data_count_i_reg[4] [0]),
        .I2(Q[1]),
        .I3(\gwdc.wr_data_count_i_reg[4] [1]),
        .O(D[0]));
  (* SOFT_HLUTNM = "soft_lutpair55" *) 
  LUT3 #(
    .INIT(8'h69)) 
    \gwdc.wr_data_count_i[3]_i_1 
       (.I0(\gwdc.wr_data_count_i[4]_i_2_n_0 ),
        .I1(\reg_out_i_reg_n_0_[3] ),
        .I2(\gwdc.wr_data_count_i_reg[4] [3]),
        .O(D[1]));
  (* SOFT_HLUTNM = "soft_lutpair55" *) 
  LUT5 #(
    .INIT(32'h718E8E71)) 
    \gwdc.wr_data_count_i[4]_i_1 
       (.I0(\gwdc.wr_data_count_i[4]_i_2_n_0 ),
        .I1(\gwdc.wr_data_count_i_reg[4] [3]),
        .I2(\reg_out_i_reg_n_0_[3] ),
        .I3(\reg_out_i_reg_n_0_[4] ),
        .I4(\gwdc.wr_data_count_i_reg[4] [4]),
        .O(D[2]));
  LUT6 #(
    .INIT(64'hD4DD4444DDDDD4DD)) 
    \gwdc.wr_data_count_i[4]_i_2 
       (.I0(Q[2]),
        .I1(\gwdc.wr_data_count_i_reg[4] [2]),
        .I2(\gwdc.wr_data_count_i_reg[4] [0]),
        .I3(Q[0]),
        .I4(\gwdc.wr_data_count_i_reg[4] [1]),
        .I5(Q[1]),
        .O(\gwdc.wr_data_count_i[4]_i_2_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[0] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[4]_0 [0]),
        .Q(Q[0]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[1] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[4]_0 [1]),
        .Q(Q[1]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[2] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[4]_0 [2]),
        .Q(Q[2]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[3] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[4]_0 [3]),
        .Q(\reg_out_i_reg_n_0_[3] ),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[4] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[4]_0 [4]),
        .Q(\reg_out_i_reg_n_0_[4] ),
        .R(wrst_busy));
endmodule

(* ORIG_REF_NAME = "xpm_fifo_reg_vec" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_reg_vec__parameterized0_1
   (D,
    Q,
    \reg_out_i_reg[3]_0 ,
    \grdc.rd_data_count_i_reg[4] ,
    \grdc.rd_data_count_i_reg[1] ,
    \reg_out_i_reg[4]_0 ,
    \reg_out_i_reg[4]_1 ,
    rd_clk);
  output [0:0]D;
  output [4:0]Q;
  output \reg_out_i_reg[3]_0 ;
  input [2:0]\grdc.rd_data_count_i_reg[4] ;
  input [1:0]\grdc.rd_data_count_i_reg[1] ;
  input \reg_out_i_reg[4]_0 ;
  input [4:0]\reg_out_i_reg[4]_1 ;
  input rd_clk;

  wire [0:0]D;
  wire [4:0]Q;
  wire [1:0]\grdc.rd_data_count_i_reg[1] ;
  wire [2:0]\grdc.rd_data_count_i_reg[4] ;
  wire rd_clk;
  wire \reg_out_i_reg[3]_0 ;
  wire \reg_out_i_reg[4]_0 ;
  wire [4:0]\reg_out_i_reg[4]_1 ;

  LUT6 #(
    .INIT(64'hC33C96696996C33C)) 
    \grdc.rd_data_count_i[1]_i_1 
       (.I0(Q[0]),
        .I1(Q[1]),
        .I2(\grdc.rd_data_count_i_reg[4] [1]),
        .I3(\grdc.rd_data_count_i_reg[1] [1]),
        .I4(\grdc.rd_data_count_i_reg[1] [0]),
        .I5(\grdc.rd_data_count_i_reg[4] [0]),
        .O(D));
  LUT2 #(
    .INIT(4'h9)) 
    \grdc.rd_data_count_i[4]_i_5 
       (.I0(Q[3]),
        .I1(\grdc.rd_data_count_i_reg[4] [2]),
        .O(\reg_out_i_reg[3]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[0] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[4]_1 [0]),
        .Q(Q[0]),
        .R(\reg_out_i_reg[4]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[1] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[4]_1 [1]),
        .Q(Q[1]),
        .R(\reg_out_i_reg[4]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[2] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[4]_1 [2]),
        .Q(Q[2]),
        .R(\reg_out_i_reg[4]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[3] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[4]_1 [3]),
        .Q(Q[3]),
        .R(\reg_out_i_reg[4]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[4] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[4]_1 [4]),
        .Q(Q[4]),
        .R(\reg_out_i_reg[4]_0 ));
endmodule

(* ORIG_REF_NAME = "xpm_fifo_reg_vec" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_reg_vec__parameterized0_5
   (D,
    Q,
    \gwdc.wr_data_count_i_reg[4] ,
    wrst_busy,
    \reg_out_i_reg[4]_0 ,
    wr_clk);
  output [2:0]D;
  output [2:0]Q;
  input [4:0]\gwdc.wr_data_count_i_reg[4] ;
  input wrst_busy;
  input [4:0]\reg_out_i_reg[4]_0 ;
  input wr_clk;

  wire [2:0]D;
  wire [2:0]Q;
  wire \gwdc.wr_data_count_i[4]_i_2_n_0 ;
  wire [4:0]\gwdc.wr_data_count_i_reg[4] ;
  wire [4:0]\reg_out_i_reg[4]_0 ;
  wire \reg_out_i_reg_n_0_[3] ;
  wire \reg_out_i_reg_n_0_[4] ;
  wire wr_clk;
  wire wrst_busy;

  LUT4 #(
    .INIT(16'h2DD2)) 
    \gwdc.wr_data_count_i[1]_i_1 
       (.I0(Q[0]),
        .I1(\gwdc.wr_data_count_i_reg[4] [0]),
        .I2(Q[1]),
        .I3(\gwdc.wr_data_count_i_reg[4] [1]),
        .O(D[0]));
  (* SOFT_HLUTNM = "soft_lutpair6" *) 
  LUT3 #(
    .INIT(8'h69)) 
    \gwdc.wr_data_count_i[3]_i_1 
       (.I0(\gwdc.wr_data_count_i[4]_i_2_n_0 ),
        .I1(\reg_out_i_reg_n_0_[3] ),
        .I2(\gwdc.wr_data_count_i_reg[4] [3]),
        .O(D[1]));
  (* SOFT_HLUTNM = "soft_lutpair6" *) 
  LUT5 #(
    .INIT(32'h718E8E71)) 
    \gwdc.wr_data_count_i[4]_i_1 
       (.I0(\gwdc.wr_data_count_i[4]_i_2_n_0 ),
        .I1(\gwdc.wr_data_count_i_reg[4] [3]),
        .I2(\reg_out_i_reg_n_0_[3] ),
        .I3(\reg_out_i_reg_n_0_[4] ),
        .I4(\gwdc.wr_data_count_i_reg[4] [4]),
        .O(D[2]));
  LUT6 #(
    .INIT(64'hD4DD4444DDDDD4DD)) 
    \gwdc.wr_data_count_i[4]_i_2 
       (.I0(Q[2]),
        .I1(\gwdc.wr_data_count_i_reg[4] [2]),
        .I2(\gwdc.wr_data_count_i_reg[4] [0]),
        .I3(Q[0]),
        .I4(\gwdc.wr_data_count_i_reg[4] [1]),
        .I5(Q[1]),
        .O(\gwdc.wr_data_count_i[4]_i_2_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[0] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[4]_0 [0]),
        .Q(Q[0]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[1] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[4]_0 [1]),
        .Q(Q[1]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[2] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[4]_0 [2]),
        .Q(Q[2]),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[3] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[4]_0 [3]),
        .Q(\reg_out_i_reg_n_0_[3] ),
        .R(wrst_busy));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[4] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[4]_0 [4]),
        .Q(\reg_out_i_reg_n_0_[4] ),
        .R(wrst_busy));
endmodule

(* ORIG_REF_NAME = "xpm_fifo_reg_vec" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_reg_vec__parameterized0_7
   (D,
    Q,
    \reg_out_i_reg[3]_0 ,
    \grdc.rd_data_count_i_reg[4] ,
    \grdc.rd_data_count_i_reg[1] ,
    \reg_out_i_reg[4]_0 ,
    \reg_out_i_reg[4]_1 ,
    rd_clk);
  output [0:0]D;
  output [4:0]Q;
  output \reg_out_i_reg[3]_0 ;
  input [2:0]\grdc.rd_data_count_i_reg[4] ;
  input [1:0]\grdc.rd_data_count_i_reg[1] ;
  input \reg_out_i_reg[4]_0 ;
  input [4:0]\reg_out_i_reg[4]_1 ;
  input rd_clk;

  wire [0:0]D;
  wire [4:0]Q;
  wire [1:0]\grdc.rd_data_count_i_reg[1] ;
  wire [2:0]\grdc.rd_data_count_i_reg[4] ;
  wire rd_clk;
  wire \reg_out_i_reg[3]_0 ;
  wire \reg_out_i_reg[4]_0 ;
  wire [4:0]\reg_out_i_reg[4]_1 ;

  LUT6 #(
    .INIT(64'hC33C96696996C33C)) 
    \grdc.rd_data_count_i[1]_i_1 
       (.I0(Q[0]),
        .I1(Q[1]),
        .I2(\grdc.rd_data_count_i_reg[4] [1]),
        .I3(\grdc.rd_data_count_i_reg[1] [1]),
        .I4(\grdc.rd_data_count_i_reg[1] [0]),
        .I5(\grdc.rd_data_count_i_reg[4] [0]),
        .O(D));
  LUT2 #(
    .INIT(4'h9)) 
    \grdc.rd_data_count_i[4]_i_5 
       (.I0(Q[3]),
        .I1(\grdc.rd_data_count_i_reg[4] [2]),
        .O(\reg_out_i_reg[3]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[0] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[4]_1 [0]),
        .Q(Q[0]),
        .R(\reg_out_i_reg[4]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[1] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[4]_1 [1]),
        .Q(Q[1]),
        .R(\reg_out_i_reg[4]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[2] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[4]_1 [2]),
        .Q(Q[2]),
        .R(\reg_out_i_reg[4]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[3] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[4]_1 [3]),
        .Q(Q[3]),
        .R(\reg_out_i_reg[4]_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \reg_out_i_reg[4] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\reg_out_i_reg[4]_1 [4]),
        .Q(Q[4]),
        .R(\reg_out_i_reg[4]_0 ));
endmodule

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_rst
   (\gen_rst_ic.fifo_rd_rst_ic_reg_0 ,
    wrst_busy,
    E,
    wr_rst_busy,
    SR,
    underflow_i0,
    \gen_rst_ic.fifo_rd_rst_ic_reg_1 ,
    rd_clk,
    wr_clk,
    rst,
    wr_en,
    \count_value_i_reg[3] ,
    rst_d1,
    Q,
    \guf.underflow_i_reg ,
    rd_en,
    ram_empty_i);
  output \gen_rst_ic.fifo_rd_rst_ic_reg_0 ;
  output wrst_busy;
  output [0:0]E;
  output wr_rst_busy;
  output [0:0]SR;
  output underflow_i0;
  output [0:0]\gen_rst_ic.fifo_rd_rst_ic_reg_1 ;
  input rd_clk;
  input wr_clk;
  input rst;
  input wr_en;
  input \count_value_i_reg[3] ;
  input rst_d1;
  input [1:0]Q;
  input \guf.underflow_i_reg ;
  input rd_en;
  input ram_empty_i;

  wire \/i__n_0 ;
  wire [0:0]E;
  wire \FSM_onehot_gen_rst_ic.curr_wrst_state[0]_i_1_n_0 ;
  wire \FSM_onehot_gen_rst_ic.curr_wrst_state[1]_i_1_n_0 ;
  wire \FSM_onehot_gen_rst_ic.curr_wrst_state[1]_i_2_n_0 ;
  wire \FSM_onehot_gen_rst_ic.curr_wrst_state[2]_i_1_n_0 ;
  wire \FSM_onehot_gen_rst_ic.curr_wrst_state[3]_i_1_n_0 ;
  wire \FSM_onehot_gen_rst_ic.curr_wrst_state[4]_i_1_n_0 ;
  wire \FSM_onehot_gen_rst_ic.curr_wrst_state[4]_i_2_n_0 ;
  (* RTL_KEEP = "yes" *) wire \FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[0] ;
  (* RTL_KEEP = "yes" *) wire \FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[1] ;
  (* RTL_KEEP = "yes" *) wire \FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[2] ;
  (* RTL_KEEP = "yes" *) wire \FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[3] ;
  (* RTL_KEEP = "yes" *) wire \FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[4] ;
  wire \FSM_sequential_gen_rst_ic.curr_rrst_state[1]_i_1_n_0 ;
  wire [1:0]Q;
  wire [0:0]SR;
  wire \__0/i__n_0 ;
  wire \count_value_i_reg[3] ;
  (* RTL_KEEP = "yes" *) wire [1:0]\gen_rst_ic.curr_rrst_state ;
  wire \gen_rst_ic.fifo_rd_rst_i0 ;
  wire \gen_rst_ic.fifo_rd_rst_ic_reg_0 ;
  wire [0:0]\gen_rst_ic.fifo_rd_rst_ic_reg_1 ;
  wire \gen_rst_ic.fifo_rd_rst_wr_i ;
  wire \gen_rst_ic.fifo_wr_rst_ic ;
  wire \gen_rst_ic.fifo_wr_rst_ic_i_1_n_0 ;
  wire \gen_rst_ic.fifo_wr_rst_ic_i_3_n_0 ;
  wire \gen_rst_ic.fifo_wr_rst_rd ;
  wire \gen_rst_ic.rst_seq_reentered_i_1_n_0 ;
  wire \gen_rst_ic.rst_seq_reentered_i_2_n_0 ;
  wire \gen_rst_ic.rst_seq_reentered_reg_n_0 ;
  wire \gen_rst_ic.wr_rst_busy_ic_i_1_n_0 ;
  wire \gen_rst_ic.wr_rst_busy_ic_i_2_n_0 ;
  wire \guf.underflow_i_reg ;
  wire p_0_in;
  wire \power_on_rst_reg_n_0_[0] ;
  wire ram_empty_i;
  wire rd_clk;
  wire rd_en;
  wire rst;
  wire rst_d1;
  wire rst_i__0;
  wire underflow_i0;
  wire wr_clk;
  wire wr_en;
  wire wr_rst_busy;
  wire wrst_busy;

  LUT5 #(
    .INIT(32'h00010116)) 
    \/i_ 
       (.I0(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[0] ),
        .I1(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[1] ),
        .I2(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[2] ),
        .I3(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[3] ),
        .I4(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[4] ),
        .O(\/i__n_0 ));
  LUT6 #(
    .INIT(64'h03030200FFFFFFFF)) 
    \FSM_onehot_gen_rst_ic.curr_wrst_state[0]_i_1 
       (.I0(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[3] ),
        .I1(p_0_in),
        .I2(rst),
        .I3(\gen_rst_ic.rst_seq_reentered_reg_n_0 ),
        .I4(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[0] ),
        .I5(\/i__n_0 ),
        .O(\FSM_onehot_gen_rst_ic.curr_wrst_state[0]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hFEFEFEEE)) 
    \FSM_onehot_gen_rst_ic.curr_wrst_state[1]_i_1 
       (.I0(\FSM_onehot_gen_rst_ic.curr_wrst_state[1]_i_2_n_0 ),
        .I1(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[4] ),
        .I2(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[3] ),
        .I3(rst),
        .I4(p_0_in),
        .O(\FSM_onehot_gen_rst_ic.curr_wrst_state[1]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'hFFF0EEE0FFFFEEE0)) 
    \FSM_onehot_gen_rst_ic.curr_wrst_state[1]_i_2 
       (.I0(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[0] ),
        .I1(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[2] ),
        .I2(rst),
        .I3(p_0_in),
        .I4(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[1] ),
        .I5(\gen_rst_ic.fifo_rd_rst_wr_i ),
        .O(\FSM_onehot_gen_rst_ic.curr_wrst_state[1]_i_2_n_0 ));
  LUT5 #(
    .INIT(32'h000C0008)) 
    \FSM_onehot_gen_rst_ic.curr_wrst_state[2]_i_1 
       (.I0(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[1] ),
        .I1(\gen_rst_ic.fifo_rd_rst_wr_i ),
        .I2(rst),
        .I3(p_0_in),
        .I4(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[2] ),
        .O(\FSM_onehot_gen_rst_ic.curr_wrst_state[2]_i_1_n_0 ));
  LUT4 #(
    .INIT(16'h0004)) 
    \FSM_onehot_gen_rst_ic.curr_wrst_state[3]_i_1 
       (.I0(\gen_rst_ic.fifo_rd_rst_wr_i ),
        .I1(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[2] ),
        .I2(rst),
        .I3(p_0_in),
        .O(\FSM_onehot_gen_rst_ic.curr_wrst_state[3]_i_1_n_0 ));
  LUT1 #(
    .INIT(2'h1)) 
    \FSM_onehot_gen_rst_ic.curr_wrst_state[4]_i_1 
       (.I0(\/i__n_0 ),
        .O(\FSM_onehot_gen_rst_ic.curr_wrst_state[4]_i_1_n_0 ));
  LUT4 #(
    .INIT(16'h0002)) 
    \FSM_onehot_gen_rst_ic.curr_wrst_state[4]_i_2 
       (.I0(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[3] ),
        .I1(p_0_in),
        .I2(rst),
        .I3(\gen_rst_ic.rst_seq_reentered_reg_n_0 ),
        .O(\FSM_onehot_gen_rst_ic.curr_wrst_state[4]_i_2_n_0 ));
  (* FSM_ENCODED_STATES = "WRST_OUT:00100,WRST_IN:00010,WRST_GO2IDLE:10000,WRST_EXIT:01000,WRST_IDLE:00001" *) 
  (* KEEP = "yes" *) 
  FDRE #(
    .INIT(1'b1)) 
    \FSM_onehot_gen_rst_ic.curr_wrst_state_reg[0] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\FSM_onehot_gen_rst_ic.curr_wrst_state[0]_i_1_n_0 ),
        .Q(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[0] ),
        .R(1'b0));
  (* FSM_ENCODED_STATES = "WRST_OUT:00100,WRST_IN:00010,WRST_GO2IDLE:10000,WRST_EXIT:01000,WRST_IDLE:00001" *) 
  (* KEEP = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \FSM_onehot_gen_rst_ic.curr_wrst_state_reg[1] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\FSM_onehot_gen_rst_ic.curr_wrst_state[1]_i_1_n_0 ),
        .Q(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[1] ),
        .R(\FSM_onehot_gen_rst_ic.curr_wrst_state[4]_i_1_n_0 ));
  (* FSM_ENCODED_STATES = "WRST_OUT:00100,WRST_IN:00010,WRST_GO2IDLE:10000,WRST_EXIT:01000,WRST_IDLE:00001" *) 
  (* KEEP = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \FSM_onehot_gen_rst_ic.curr_wrst_state_reg[2] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\FSM_onehot_gen_rst_ic.curr_wrst_state[2]_i_1_n_0 ),
        .Q(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[2] ),
        .R(\FSM_onehot_gen_rst_ic.curr_wrst_state[4]_i_1_n_0 ));
  (* FSM_ENCODED_STATES = "WRST_OUT:00100,WRST_IN:00010,WRST_GO2IDLE:10000,WRST_EXIT:01000,WRST_IDLE:00001" *) 
  (* KEEP = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \FSM_onehot_gen_rst_ic.curr_wrst_state_reg[3] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\FSM_onehot_gen_rst_ic.curr_wrst_state[3]_i_1_n_0 ),
        .Q(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[3] ),
        .R(\FSM_onehot_gen_rst_ic.curr_wrst_state[4]_i_1_n_0 ));
  (* FSM_ENCODED_STATES = "WRST_OUT:00100,WRST_IN:00010,WRST_GO2IDLE:10000,WRST_EXIT:01000,WRST_IDLE:00001" *) 
  (* KEEP = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \FSM_onehot_gen_rst_ic.curr_wrst_state_reg[4] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\FSM_onehot_gen_rst_ic.curr_wrst_state[4]_i_2_n_0 ),
        .Q(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[4] ),
        .R(\FSM_onehot_gen_rst_ic.curr_wrst_state[4]_i_1_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \FSM_sequential_gen_rst_ic.curr_rrst_state[1]_i_1 
       (.I0(\gen_rst_ic.curr_rrst_state [0]),
        .I1(\gen_rst_ic.curr_rrst_state [1]),
        .O(\FSM_sequential_gen_rst_ic.curr_rrst_state[1]_i_1_n_0 ));
  (* FSM_ENCODED_STATES = "iSTATE:00,iSTATE0:01,iSTATE1:10,iSTATE2:11" *) 
  (* KEEP = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \FSM_sequential_gen_rst_ic.curr_rrst_state_reg[0] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\__0/i__n_0 ),
        .Q(\gen_rst_ic.curr_rrst_state [0]),
        .R(1'b0));
  (* FSM_ENCODED_STATES = "iSTATE:00,iSTATE0:01,iSTATE1:10,iSTATE2:11" *) 
  (* KEEP = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \FSM_sequential_gen_rst_ic.curr_rrst_state_reg[1] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\FSM_sequential_gen_rst_ic.curr_rrst_state[1]_i_1_n_0 ),
        .Q(\gen_rst_ic.curr_rrst_state [1]),
        .R(1'b0));
  LUT3 #(
    .INIT(8'h06)) 
    \__0/i_ 
       (.I0(\gen_rst_ic.fifo_wr_rst_rd ),
        .I1(\gen_rst_ic.curr_rrst_state [1]),
        .I2(\gen_rst_ic.curr_rrst_state [0]),
        .O(\__0/i__n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair67" *) 
  LUT4 #(
    .INIT(16'hAAAE)) 
    \count_value_i[1]_i_1__4 
       (.I0(\gen_rst_ic.fifo_rd_rst_ic_reg_0 ),
        .I1(ram_empty_i),
        .I2(Q[1]),
        .I3(Q[0]),
        .O(\gen_rst_ic.fifo_rd_rst_ic_reg_1 ));
  LUT3 #(
    .INIT(8'h3E)) 
    \gen_rst_ic.fifo_rd_rst_ic_i_1 
       (.I0(\gen_rst_ic.fifo_wr_rst_rd ),
        .I1(\gen_rst_ic.curr_rrst_state [1]),
        .I2(\gen_rst_ic.curr_rrst_state [0]),
        .O(\gen_rst_ic.fifo_rd_rst_i0 ));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rst_ic.fifo_rd_rst_ic_reg 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\gen_rst_ic.fifo_rd_rst_i0 ),
        .Q(\gen_rst_ic.fifo_rd_rst_ic_reg_0 ),
        .R(1'b0));
  LUT6 #(
    .INIT(64'hFFEAFFFFFFEA0000)) 
    \gen_rst_ic.fifo_wr_rst_ic_i_1 
       (.I0(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[4] ),
        .I1(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[0] ),
        .I2(rst_i__0),
        .I3(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[1] ),
        .I4(\gen_rst_ic.fifo_wr_rst_ic_i_3_n_0 ),
        .I5(\gen_rst_ic.fifo_wr_rst_ic ),
        .O(\gen_rst_ic.fifo_wr_rst_ic_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair68" *) 
  LUT2 #(
    .INIT(4'hE)) 
    \gen_rst_ic.fifo_wr_rst_ic_i_2 
       (.I0(p_0_in),
        .I1(rst),
        .O(rst_i__0));
  LUT5 #(
    .INIT(32'h00010116)) 
    \gen_rst_ic.fifo_wr_rst_ic_i_3 
       (.I0(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[0] ),
        .I1(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[1] ),
        .I2(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[2] ),
        .I3(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[3] ),
        .I4(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[4] ),
        .O(\gen_rst_ic.fifo_wr_rst_ic_i_3_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rst_ic.fifo_wr_rst_ic_reg 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\gen_rst_ic.fifo_wr_rst_ic_i_1_n_0 ),
        .Q(\gen_rst_ic.fifo_wr_rst_ic ),
        .R(1'b0));
  (* DEF_VAL = "1'b0" *) 
  (* DEST_SYNC_FF = "3" *) 
  (* INIT = "0" *) 
  (* INIT_SYNC_FF = "1" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* VERSION = "0" *) 
  (* XPM_CDC = "SYNC_RST" *) 
  (* XPM_MODULE = "TRUE" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_sync_rst \gen_rst_ic.rrst_wr_inst 
       (.dest_clk(wr_clk),
        .dest_rst(\gen_rst_ic.fifo_rd_rst_wr_i ),
        .src_rst(\gen_rst_ic.fifo_rd_rst_ic_reg_0 ));
  (* SOFT_HLUTNM = "soft_lutpair68" *) 
  LUT3 #(
    .INIT(8'h02)) 
    \gen_rst_ic.rst_seq_reentered_i_1 
       (.I0(\gen_rst_ic.rst_seq_reentered_i_2_n_0 ),
        .I1(rst),
        .I2(p_0_in),
        .O(\gen_rst_ic.rst_seq_reentered_i_1_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF00010000)) 
    \gen_rst_ic.rst_seq_reentered_i_2 
       (.I0(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[0] ),
        .I1(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[3] ),
        .I2(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[1] ),
        .I3(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[2] ),
        .I4(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[4] ),
        .I5(\gen_rst_ic.rst_seq_reentered_reg_n_0 ),
        .O(\gen_rst_ic.rst_seq_reentered_i_2_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rst_ic.rst_seq_reentered_reg 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\gen_rst_ic.rst_seq_reentered_i_1_n_0 ),
        .Q(\gen_rst_ic.rst_seq_reentered_reg_n_0 ),
        .R(1'b0));
  LUT5 #(
    .INIT(32'hEFFFEF00)) 
    \gen_rst_ic.wr_rst_busy_ic_i_1 
       (.I0(rst),
        .I1(p_0_in),
        .I2(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[0] ),
        .I3(\gen_rst_ic.wr_rst_busy_ic_i_2_n_0 ),
        .I4(wrst_busy),
        .O(\gen_rst_ic.wr_rst_busy_ic_i_1_n_0 ));
  LUT5 #(
    .INIT(32'h00000116)) 
    \gen_rst_ic.wr_rst_busy_ic_i_2 
       (.I0(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[3] ),
        .I1(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[2] ),
        .I2(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[1] ),
        .I3(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[0] ),
        .I4(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[4] ),
        .O(\gen_rst_ic.wr_rst_busy_ic_i_2_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rst_ic.wr_rst_busy_ic_reg 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\gen_rst_ic.wr_rst_busy_ic_i_1_n_0 ),
        .Q(wrst_busy),
        .R(1'b0));
  (* DEF_VAL = "1'b0" *) 
  (* DEST_SYNC_FF = "3" *) 
  (* INIT = "0" *) 
  (* INIT_SYNC_FF = "1" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* VERSION = "0" *) 
  (* XPM_CDC = "SYNC_RST" *) 
  (* XPM_MODULE = "TRUE" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_sync_rst__6 \gen_rst_ic.wrst_rd_inst 
       (.dest_clk(rd_clk),
        .dest_rst(\gen_rst_ic.fifo_wr_rst_rd ),
        .src_rst(\gen_rst_ic.fifo_wr_rst_ic ));
  (* SOFT_HLUTNM = "soft_lutpair66" *) 
  LUT4 #(
    .INIT(16'h0002)) 
    \gen_sdpram.xpm_memory_base_inst_i_1 
       (.I0(wr_en),
        .I1(\count_value_i_reg[3] ),
        .I2(wrst_busy),
        .I3(rst_d1),
        .O(E));
  (* SOFT_HLUTNM = "soft_lutpair67" *) 
  LUT3 #(
    .INIT(8'hAB)) 
    \grdc.rd_data_count_i[4]_i_1 
       (.I0(\gen_rst_ic.fifo_rd_rst_ic_reg_0 ),
        .I1(Q[0]),
        .I2(Q[1]),
        .O(SR));
  LUT3 #(
    .INIT(8'hE0)) 
    \guf.underflow_i_i_1 
       (.I0(\guf.underflow_i_reg ),
        .I1(\gen_rst_ic.fifo_rd_rst_ic_reg_0 ),
        .I2(rd_en),
        .O(underflow_i0));
  FDRE #(
    .INIT(1'b1)) 
    \power_on_rst_reg[0] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(1'b0),
        .Q(\power_on_rst_reg_n_0_[0] ),
        .R(1'b0));
  FDRE #(
    .INIT(1'b1)) 
    \power_on_rst_reg[1] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\power_on_rst_reg_n_0_[0] ),
        .Q(p_0_in),
        .R(1'b0));
  (* SOFT_HLUTNM = "soft_lutpair66" *) 
  LUT2 #(
    .INIT(4'hE)) 
    wr_rst_busy_INST_0
       (.I0(wrst_busy),
        .I1(rst_d1),
        .O(wr_rst_busy));
endmodule

(* ORIG_REF_NAME = "xpm_fifo_rst" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_fifo_rst__xdcDup__1
   (\gen_rst_ic.fifo_rd_rst_ic_reg_0 ,
    wrst_busy,
    E,
    wr_rst_busy,
    SR,
    underflow_i0,
    \gen_rst_ic.fifo_rd_rst_ic_reg_1 ,
    rd_clk,
    wr_clk,
    rst,
    wr_en,
    \count_value_i_reg[3] ,
    rst_d1,
    Q,
    \guf.underflow_i_reg ,
    rd_en,
    ram_empty_i);
  output \gen_rst_ic.fifo_rd_rst_ic_reg_0 ;
  output wrst_busy;
  output [0:0]E;
  output wr_rst_busy;
  output [0:0]SR;
  output underflow_i0;
  output [0:0]\gen_rst_ic.fifo_rd_rst_ic_reg_1 ;
  input rd_clk;
  input wr_clk;
  input rst;
  input wr_en;
  input \count_value_i_reg[3] ;
  input rst_d1;
  input [1:0]Q;
  input \guf.underflow_i_reg ;
  input rd_en;
  input ram_empty_i;

  wire \/i__n_0 ;
  wire [0:0]E;
  wire \FSM_onehot_gen_rst_ic.curr_wrst_state[0]_i_1_n_0 ;
  wire \FSM_onehot_gen_rst_ic.curr_wrst_state[1]_i_1_n_0 ;
  wire \FSM_onehot_gen_rst_ic.curr_wrst_state[1]_i_2_n_0 ;
  wire \FSM_onehot_gen_rst_ic.curr_wrst_state[2]_i_1_n_0 ;
  wire \FSM_onehot_gen_rst_ic.curr_wrst_state[3]_i_1_n_0 ;
  wire \FSM_onehot_gen_rst_ic.curr_wrst_state[4]_i_1_n_0 ;
  wire \FSM_onehot_gen_rst_ic.curr_wrst_state[4]_i_2_n_0 ;
  (* RTL_KEEP = "yes" *) wire \FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[0] ;
  (* RTL_KEEP = "yes" *) wire \FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[1] ;
  (* RTL_KEEP = "yes" *) wire \FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[2] ;
  (* RTL_KEEP = "yes" *) wire \FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[3] ;
  (* RTL_KEEP = "yes" *) wire \FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[4] ;
  wire \FSM_sequential_gen_rst_ic.curr_rrst_state[1]_i_1_n_0 ;
  wire [1:0]Q;
  wire [0:0]SR;
  wire \__0/i__n_0 ;
  wire \count_value_i_reg[3] ;
  (* RTL_KEEP = "yes" *) wire [1:0]\gen_rst_ic.curr_rrst_state ;
  wire \gen_rst_ic.fifo_rd_rst_i0 ;
  wire \gen_rst_ic.fifo_rd_rst_ic_reg_0 ;
  wire [0:0]\gen_rst_ic.fifo_rd_rst_ic_reg_1 ;
  wire \gen_rst_ic.fifo_rd_rst_wr_i ;
  wire \gen_rst_ic.fifo_wr_rst_ic ;
  wire \gen_rst_ic.fifo_wr_rst_ic_i_1_n_0 ;
  wire \gen_rst_ic.fifo_wr_rst_ic_i_3_n_0 ;
  wire \gen_rst_ic.fifo_wr_rst_rd ;
  wire \gen_rst_ic.rst_seq_reentered_i_1_n_0 ;
  wire \gen_rst_ic.rst_seq_reentered_i_2_n_0 ;
  wire \gen_rst_ic.rst_seq_reentered_reg_n_0 ;
  wire \gen_rst_ic.wr_rst_busy_ic_i_1_n_0 ;
  wire \gen_rst_ic.wr_rst_busy_ic_i_2_n_0 ;
  wire \guf.underflow_i_reg ;
  wire p_0_in;
  wire \power_on_rst_reg_n_0_[0] ;
  wire ram_empty_i;
  wire rd_clk;
  wire rd_en;
  wire rst;
  wire rst_d1;
  wire rst_i__0;
  wire underflow_i0;
  wire wr_clk;
  wire wr_en;
  wire wr_rst_busy;
  wire wrst_busy;

  LUT5 #(
    .INIT(32'h00010116)) 
    \/i_ 
       (.I0(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[0] ),
        .I1(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[1] ),
        .I2(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[2] ),
        .I3(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[3] ),
        .I4(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[4] ),
        .O(\/i__n_0 ));
  LUT6 #(
    .INIT(64'h03030200FFFFFFFF)) 
    \FSM_onehot_gen_rst_ic.curr_wrst_state[0]_i_1 
       (.I0(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[3] ),
        .I1(p_0_in),
        .I2(rst),
        .I3(\gen_rst_ic.rst_seq_reentered_reg_n_0 ),
        .I4(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[0] ),
        .I5(\/i__n_0 ),
        .O(\FSM_onehot_gen_rst_ic.curr_wrst_state[0]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'hFEFEFEEE)) 
    \FSM_onehot_gen_rst_ic.curr_wrst_state[1]_i_1 
       (.I0(\FSM_onehot_gen_rst_ic.curr_wrst_state[1]_i_2_n_0 ),
        .I1(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[4] ),
        .I2(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[3] ),
        .I3(rst),
        .I4(p_0_in),
        .O(\FSM_onehot_gen_rst_ic.curr_wrst_state[1]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'hFFF0EEE0FFFFEEE0)) 
    \FSM_onehot_gen_rst_ic.curr_wrst_state[1]_i_2 
       (.I0(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[0] ),
        .I1(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[2] ),
        .I2(rst),
        .I3(p_0_in),
        .I4(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[1] ),
        .I5(\gen_rst_ic.fifo_rd_rst_wr_i ),
        .O(\FSM_onehot_gen_rst_ic.curr_wrst_state[1]_i_2_n_0 ));
  LUT5 #(
    .INIT(32'h000C0008)) 
    \FSM_onehot_gen_rst_ic.curr_wrst_state[2]_i_1 
       (.I0(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[1] ),
        .I1(\gen_rst_ic.fifo_rd_rst_wr_i ),
        .I2(rst),
        .I3(p_0_in),
        .I4(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[2] ),
        .O(\FSM_onehot_gen_rst_ic.curr_wrst_state[2]_i_1_n_0 ));
  LUT4 #(
    .INIT(16'h0004)) 
    \FSM_onehot_gen_rst_ic.curr_wrst_state[3]_i_1 
       (.I0(\gen_rst_ic.fifo_rd_rst_wr_i ),
        .I1(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[2] ),
        .I2(rst),
        .I3(p_0_in),
        .O(\FSM_onehot_gen_rst_ic.curr_wrst_state[3]_i_1_n_0 ));
  LUT1 #(
    .INIT(2'h1)) 
    \FSM_onehot_gen_rst_ic.curr_wrst_state[4]_i_1 
       (.I0(\/i__n_0 ),
        .O(\FSM_onehot_gen_rst_ic.curr_wrst_state[4]_i_1_n_0 ));
  LUT4 #(
    .INIT(16'h0002)) 
    \FSM_onehot_gen_rst_ic.curr_wrst_state[4]_i_2 
       (.I0(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[3] ),
        .I1(p_0_in),
        .I2(rst),
        .I3(\gen_rst_ic.rst_seq_reentered_reg_n_0 ),
        .O(\FSM_onehot_gen_rst_ic.curr_wrst_state[4]_i_2_n_0 ));
  (* FSM_ENCODED_STATES = "WRST_OUT:00100,WRST_IN:00010,WRST_GO2IDLE:10000,WRST_EXIT:01000,WRST_IDLE:00001" *) 
  (* KEEP = "yes" *) 
  FDRE #(
    .INIT(1'b1)) 
    \FSM_onehot_gen_rst_ic.curr_wrst_state_reg[0] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\FSM_onehot_gen_rst_ic.curr_wrst_state[0]_i_1_n_0 ),
        .Q(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[0] ),
        .R(1'b0));
  (* FSM_ENCODED_STATES = "WRST_OUT:00100,WRST_IN:00010,WRST_GO2IDLE:10000,WRST_EXIT:01000,WRST_IDLE:00001" *) 
  (* KEEP = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \FSM_onehot_gen_rst_ic.curr_wrst_state_reg[1] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\FSM_onehot_gen_rst_ic.curr_wrst_state[1]_i_1_n_0 ),
        .Q(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[1] ),
        .R(\FSM_onehot_gen_rst_ic.curr_wrst_state[4]_i_1_n_0 ));
  (* FSM_ENCODED_STATES = "WRST_OUT:00100,WRST_IN:00010,WRST_GO2IDLE:10000,WRST_EXIT:01000,WRST_IDLE:00001" *) 
  (* KEEP = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \FSM_onehot_gen_rst_ic.curr_wrst_state_reg[2] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\FSM_onehot_gen_rst_ic.curr_wrst_state[2]_i_1_n_0 ),
        .Q(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[2] ),
        .R(\FSM_onehot_gen_rst_ic.curr_wrst_state[4]_i_1_n_0 ));
  (* FSM_ENCODED_STATES = "WRST_OUT:00100,WRST_IN:00010,WRST_GO2IDLE:10000,WRST_EXIT:01000,WRST_IDLE:00001" *) 
  (* KEEP = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \FSM_onehot_gen_rst_ic.curr_wrst_state_reg[3] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\FSM_onehot_gen_rst_ic.curr_wrst_state[3]_i_1_n_0 ),
        .Q(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[3] ),
        .R(\FSM_onehot_gen_rst_ic.curr_wrst_state[4]_i_1_n_0 ));
  (* FSM_ENCODED_STATES = "WRST_OUT:00100,WRST_IN:00010,WRST_GO2IDLE:10000,WRST_EXIT:01000,WRST_IDLE:00001" *) 
  (* KEEP = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \FSM_onehot_gen_rst_ic.curr_wrst_state_reg[4] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\FSM_onehot_gen_rst_ic.curr_wrst_state[4]_i_2_n_0 ),
        .Q(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[4] ),
        .R(\FSM_onehot_gen_rst_ic.curr_wrst_state[4]_i_1_n_0 ));
  LUT2 #(
    .INIT(4'h6)) 
    \FSM_sequential_gen_rst_ic.curr_rrst_state[1]_i_1 
       (.I0(\gen_rst_ic.curr_rrst_state [0]),
        .I1(\gen_rst_ic.curr_rrst_state [1]),
        .O(\FSM_sequential_gen_rst_ic.curr_rrst_state[1]_i_1_n_0 ));
  (* FSM_ENCODED_STATES = "iSTATE:00,iSTATE0:01,iSTATE1:10,iSTATE2:11" *) 
  (* KEEP = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \FSM_sequential_gen_rst_ic.curr_rrst_state_reg[0] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\__0/i__n_0 ),
        .Q(\gen_rst_ic.curr_rrst_state [0]),
        .R(1'b0));
  (* FSM_ENCODED_STATES = "iSTATE:00,iSTATE0:01,iSTATE1:10,iSTATE2:11" *) 
  (* KEEP = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \FSM_sequential_gen_rst_ic.curr_rrst_state_reg[1] 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\FSM_sequential_gen_rst_ic.curr_rrst_state[1]_i_1_n_0 ),
        .Q(\gen_rst_ic.curr_rrst_state [1]),
        .R(1'b0));
  LUT3 #(
    .INIT(8'h06)) 
    \__0/i_ 
       (.I0(\gen_rst_ic.fifo_wr_rst_rd ),
        .I1(\gen_rst_ic.curr_rrst_state [1]),
        .I2(\gen_rst_ic.curr_rrst_state [0]),
        .O(\__0/i__n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair18" *) 
  LUT4 #(
    .INIT(16'hAAAE)) 
    \count_value_i[1]_i_1__4 
       (.I0(\gen_rst_ic.fifo_rd_rst_ic_reg_0 ),
        .I1(ram_empty_i),
        .I2(Q[1]),
        .I3(Q[0]),
        .O(\gen_rst_ic.fifo_rd_rst_ic_reg_1 ));
  LUT3 #(
    .INIT(8'h3E)) 
    \gen_rst_ic.fifo_rd_rst_ic_i_1 
       (.I0(\gen_rst_ic.fifo_wr_rst_rd ),
        .I1(\gen_rst_ic.curr_rrst_state [1]),
        .I2(\gen_rst_ic.curr_rrst_state [0]),
        .O(\gen_rst_ic.fifo_rd_rst_i0 ));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rst_ic.fifo_rd_rst_ic_reg 
       (.C(rd_clk),
        .CE(1'b1),
        .D(\gen_rst_ic.fifo_rd_rst_i0 ),
        .Q(\gen_rst_ic.fifo_rd_rst_ic_reg_0 ),
        .R(1'b0));
  LUT6 #(
    .INIT(64'hFFEAFFFFFFEA0000)) 
    \gen_rst_ic.fifo_wr_rst_ic_i_1 
       (.I0(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[4] ),
        .I1(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[0] ),
        .I2(rst_i__0),
        .I3(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[1] ),
        .I4(\gen_rst_ic.fifo_wr_rst_ic_i_3_n_0 ),
        .I5(\gen_rst_ic.fifo_wr_rst_ic ),
        .O(\gen_rst_ic.fifo_wr_rst_ic_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair19" *) 
  LUT2 #(
    .INIT(4'hE)) 
    \gen_rst_ic.fifo_wr_rst_ic_i_2 
       (.I0(p_0_in),
        .I1(rst),
        .O(rst_i__0));
  LUT5 #(
    .INIT(32'h00010116)) 
    \gen_rst_ic.fifo_wr_rst_ic_i_3 
       (.I0(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[0] ),
        .I1(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[1] ),
        .I2(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[2] ),
        .I3(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[3] ),
        .I4(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[4] ),
        .O(\gen_rst_ic.fifo_wr_rst_ic_i_3_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rst_ic.fifo_wr_rst_ic_reg 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\gen_rst_ic.fifo_wr_rst_ic_i_1_n_0 ),
        .Q(\gen_rst_ic.fifo_wr_rst_ic ),
        .R(1'b0));
  (* DEF_VAL = "1'b0" *) 
  (* DEST_SYNC_FF = "3" *) 
  (* INIT = "0" *) 
  (* INIT_SYNC_FF = "1" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* VERSION = "0" *) 
  (* XPM_CDC = "SYNC_RST" *) 
  (* XPM_MODULE = "TRUE" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_sync_rst__5 \gen_rst_ic.rrst_wr_inst 
       (.dest_clk(wr_clk),
        .dest_rst(\gen_rst_ic.fifo_rd_rst_wr_i ),
        .src_rst(\gen_rst_ic.fifo_rd_rst_ic_reg_0 ));
  (* SOFT_HLUTNM = "soft_lutpair19" *) 
  LUT3 #(
    .INIT(8'h02)) 
    \gen_rst_ic.rst_seq_reentered_i_1 
       (.I0(\gen_rst_ic.rst_seq_reentered_i_2_n_0 ),
        .I1(rst),
        .I2(p_0_in),
        .O(\gen_rst_ic.rst_seq_reentered_i_1_n_0 ));
  LUT6 #(
    .INIT(64'hFFFFFFFF00010000)) 
    \gen_rst_ic.rst_seq_reentered_i_2 
       (.I0(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[0] ),
        .I1(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[3] ),
        .I2(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[1] ),
        .I3(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[2] ),
        .I4(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[4] ),
        .I5(\gen_rst_ic.rst_seq_reentered_reg_n_0 ),
        .O(\gen_rst_ic.rst_seq_reentered_i_2_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rst_ic.rst_seq_reentered_reg 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\gen_rst_ic.rst_seq_reentered_i_1_n_0 ),
        .Q(\gen_rst_ic.rst_seq_reentered_reg_n_0 ),
        .R(1'b0));
  LUT5 #(
    .INIT(32'hEFFFEF00)) 
    \gen_rst_ic.wr_rst_busy_ic_i_1 
       (.I0(rst),
        .I1(p_0_in),
        .I2(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[0] ),
        .I3(\gen_rst_ic.wr_rst_busy_ic_i_2_n_0 ),
        .I4(wrst_busy),
        .O(\gen_rst_ic.wr_rst_busy_ic_i_1_n_0 ));
  LUT5 #(
    .INIT(32'h00000116)) 
    \gen_rst_ic.wr_rst_busy_ic_i_2 
       (.I0(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[3] ),
        .I1(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[2] ),
        .I2(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[1] ),
        .I3(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[0] ),
        .I4(\FSM_onehot_gen_rst_ic.curr_wrst_state_reg_n_0_[4] ),
        .O(\gen_rst_ic.wr_rst_busy_ic_i_2_n_0 ));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rst_ic.wr_rst_busy_ic_reg 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\gen_rst_ic.wr_rst_busy_ic_i_1_n_0 ),
        .Q(wrst_busy),
        .R(1'b0));
  (* DEF_VAL = "1'b0" *) 
  (* DEST_SYNC_FF = "3" *) 
  (* INIT = "0" *) 
  (* INIT_SYNC_FF = "1" *) 
  (* SIM_ASSERT_CHK = "0" *) 
  (* VERSION = "0" *) 
  (* XPM_CDC = "SYNC_RST" *) 
  (* XPM_MODULE = "TRUE" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_sync_rst__4 \gen_rst_ic.wrst_rd_inst 
       (.dest_clk(rd_clk),
        .dest_rst(\gen_rst_ic.fifo_wr_rst_rd ),
        .src_rst(\gen_rst_ic.fifo_wr_rst_ic ));
  (* SOFT_HLUTNM = "soft_lutpair17" *) 
  LUT4 #(
    .INIT(16'h0002)) 
    \gen_sdpram.xpm_memory_base_inst_i_1 
       (.I0(wr_en),
        .I1(\count_value_i_reg[3] ),
        .I2(wrst_busy),
        .I3(rst_d1),
        .O(E));
  (* SOFT_HLUTNM = "soft_lutpair18" *) 
  LUT3 #(
    .INIT(8'hAB)) 
    \grdc.rd_data_count_i[4]_i_1 
       (.I0(\gen_rst_ic.fifo_rd_rst_ic_reg_0 ),
        .I1(Q[0]),
        .I2(Q[1]),
        .O(SR));
  LUT3 #(
    .INIT(8'hE0)) 
    \guf.underflow_i_i_1 
       (.I0(\guf.underflow_i_reg ),
        .I1(\gen_rst_ic.fifo_rd_rst_ic_reg_0 ),
        .I2(rd_en),
        .O(underflow_i0));
  FDRE #(
    .INIT(1'b1)) 
    \power_on_rst_reg[0] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(1'b0),
        .Q(\power_on_rst_reg_n_0_[0] ),
        .R(1'b0));
  FDRE #(
    .INIT(1'b1)) 
    \power_on_rst_reg[1] 
       (.C(wr_clk),
        .CE(1'b1),
        .D(\power_on_rst_reg_n_0_[0] ),
        .Q(p_0_in),
        .R(1'b0));
  (* SOFT_HLUTNM = "soft_lutpair17" *) 
  LUT2 #(
    .INIT(4'hE)) 
    wr_rst_busy_INST_0
       (.I0(wrst_busy),
        .I1(rst_d1),
        .O(wr_rst_busy));
endmodule

(* ADDR_WIDTH_A = "4" *) (* ADDR_WIDTH_B = "4" *) (* AUTO_SLEEP_TIME = "0" *) 
(* BYTE_WRITE_WIDTH_A = "64" *) (* BYTE_WRITE_WIDTH_B = "64" *) (* CASCADE_HEIGHT = "0" *) 
(* CLOCKING_MODE = "1" *) (* ECC_MODE = "0" *) (* MAX_NUM_CHAR = "0" *) 
(* MEMORY_INIT_FILE = "none" *) (* MEMORY_INIT_PARAM = "" *) (* MEMORY_OPTIMIZATION = "true" *) 
(* MEMORY_PRIMITIVE = "1" *) (* MEMORY_SIZE = "1024" *) (* MEMORY_TYPE = "1" *) 
(* MESSAGE_CONTROL = "0" *) (* NUM_CHAR_LOC = "0" *) (* P_ECC_MODE = "no_ecc" *) 
(* P_ENABLE_BYTE_WRITE_A = "0" *) (* P_ENABLE_BYTE_WRITE_B = "0" *) (* P_MAX_DEPTH_DATA = "16" *) 
(* P_MEMORY_OPT = "yes" *) (* P_MEMORY_PRIMITIVE = "distributed" *) (* P_MIN_WIDTH_DATA = "64" *) 
(* P_MIN_WIDTH_DATA_A = "64" *) (* P_MIN_WIDTH_DATA_B = "64" *) (* P_MIN_WIDTH_DATA_ECC = "64" *) 
(* P_MIN_WIDTH_DATA_LDW = "4" *) (* P_MIN_WIDTH_DATA_SHFT = "64" *) (* P_NUM_COLS_WRITE_A = "1" *) 
(* P_NUM_COLS_WRITE_B = "1" *) (* P_NUM_ROWS_READ_A = "1" *) (* P_NUM_ROWS_READ_B = "1" *) 
(* P_NUM_ROWS_WRITE_A = "1" *) (* P_NUM_ROWS_WRITE_B = "1" *) (* P_SDP_WRITE_MODE = "yes" *) 
(* P_WIDTH_ADDR_LSB_READ_A = "0" *) (* P_WIDTH_ADDR_LSB_READ_B = "0" *) (* P_WIDTH_ADDR_LSB_WRITE_A = "0" *) 
(* P_WIDTH_ADDR_LSB_WRITE_B = "0" *) (* P_WIDTH_ADDR_READ_A = "4" *) (* P_WIDTH_ADDR_READ_B = "4" *) 
(* P_WIDTH_ADDR_WRITE_A = "4" *) (* P_WIDTH_ADDR_WRITE_B = "4" *) (* P_WIDTH_COL_WRITE_A = "64" *) 
(* P_WIDTH_COL_WRITE_B = "64" *) (* READ_DATA_WIDTH_A = "64" *) (* READ_DATA_WIDTH_B = "64" *) 
(* READ_LATENCY_A = "2" *) (* READ_LATENCY_B = "2" *) (* READ_RESET_VALUE_A = "0" *) 
(* READ_RESET_VALUE_B = "0" *) (* RST_MODE_A = "SYNC" *) (* RST_MODE_B = "SYNC" *) 
(* SIM_ASSERT_CHK = "0" *) (* USE_EMBEDDED_CONSTRAINT = "1" *) (* USE_MEM_INIT = "0" *) 
(* USE_MEM_INIT_MMI = "0" *) (* VERSION = "0" *) (* WAKEUP_TIME = "0" *) 
(* WRITE_DATA_WIDTH_A = "64" *) (* WRITE_DATA_WIDTH_B = "64" *) (* WRITE_MODE_A = "2" *) 
(* WRITE_MODE_B = "1" *) (* WRITE_PROTECT = "1" *) (* XPM_MODULE = "TRUE" *) 
(* keep_hierarchy = "soft" *) (* rsta_loop_iter = "64" *) (* rstb_loop_iter = "64" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_memory_base
   (sleep,
    clka,
    rsta,
    ena,
    regcea,
    wea,
    addra,
    dina,
    injectsbiterra,
    injectdbiterra,
    douta,
    sbiterra,
    dbiterra,
    clkb,
    rstb,
    enb,
    regceb,
    web,
    addrb,
    dinb,
    injectsbiterrb,
    injectdbiterrb,
    doutb,
    sbiterrb,
    dbiterrb);
  input sleep;
  input clka;
  input rsta;
  input ena;
  input regcea;
  input [0:0]wea;
  input [3:0]addra;
  input [63:0]dina;
  input injectsbiterra;
  input injectdbiterra;
  output [63:0]douta;
  output sbiterra;
  output dbiterra;
  input clkb;
  input rstb;
  input enb;
  input regceb;
  input [0:0]web;
  input [3:0]addrb;
  input [63:0]dinb;
  input injectsbiterrb;
  input injectdbiterrb;
  output [63:0]doutb;
  output sbiterrb;
  output dbiterrb;

  wire \<const0> ;
  wire [3:0]addra;
  wire [3:0]addrb;
  wire clka;
  wire clkb;
  wire [63:0]dina;
  wire [63:0]doutb;
  wire ena;
  wire enb;
  wire [63:0]\gen_rd_b.doutb_reg ;
  wire [63:0]\gen_rd_b.doutb_reg0 ;
  wire regceb;
  wire rstb;
  wire sleep;
  wire [1:0]\NLW_gen_wr_a.gen_word_narrow.mem_reg_0_15_0_13_DOH_UNCONNECTED ;
  wire [1:0]\NLW_gen_wr_a.gen_word_narrow.mem_reg_0_15_14_27_DOH_UNCONNECTED ;
  wire [1:0]\NLW_gen_wr_a.gen_word_narrow.mem_reg_0_15_28_41_DOH_UNCONNECTED ;
  wire [1:0]\NLW_gen_wr_a.gen_word_narrow.mem_reg_0_15_42_55_DOH_UNCONNECTED ;
  wire [1:0]\NLW_gen_wr_a.gen_word_narrow.mem_reg_0_15_56_63_DOE_UNCONNECTED ;
  wire [1:0]\NLW_gen_wr_a.gen_word_narrow.mem_reg_0_15_56_63_DOF_UNCONNECTED ;
  wire [1:0]\NLW_gen_wr_a.gen_word_narrow.mem_reg_0_15_56_63_DOG_UNCONNECTED ;
  wire [1:0]\NLW_gen_wr_a.gen_word_narrow.mem_reg_0_15_56_63_DOH_UNCONNECTED ;

  assign dbiterra = \<const0> ;
  assign dbiterrb = \<const0> ;
  assign douta[63] = \<const0> ;
  assign douta[62] = \<const0> ;
  assign douta[61] = \<const0> ;
  assign douta[60] = \<const0> ;
  assign douta[59] = \<const0> ;
  assign douta[58] = \<const0> ;
  assign douta[57] = \<const0> ;
  assign douta[56] = \<const0> ;
  assign douta[55] = \<const0> ;
  assign douta[54] = \<const0> ;
  assign douta[53] = \<const0> ;
  assign douta[52] = \<const0> ;
  assign douta[51] = \<const0> ;
  assign douta[50] = \<const0> ;
  assign douta[49] = \<const0> ;
  assign douta[48] = \<const0> ;
  assign douta[47] = \<const0> ;
  assign douta[46] = \<const0> ;
  assign douta[45] = \<const0> ;
  assign douta[44] = \<const0> ;
  assign douta[43] = \<const0> ;
  assign douta[42] = \<const0> ;
  assign douta[41] = \<const0> ;
  assign douta[40] = \<const0> ;
  assign douta[39] = \<const0> ;
  assign douta[38] = \<const0> ;
  assign douta[37] = \<const0> ;
  assign douta[36] = \<const0> ;
  assign douta[35] = \<const0> ;
  assign douta[34] = \<const0> ;
  assign douta[33] = \<const0> ;
  assign douta[32] = \<const0> ;
  assign douta[31] = \<const0> ;
  assign douta[30] = \<const0> ;
  assign douta[29] = \<const0> ;
  assign douta[28] = \<const0> ;
  assign douta[27] = \<const0> ;
  assign douta[26] = \<const0> ;
  assign douta[25] = \<const0> ;
  assign douta[24] = \<const0> ;
  assign douta[23] = \<const0> ;
  assign douta[22] = \<const0> ;
  assign douta[21] = \<const0> ;
  assign douta[20] = \<const0> ;
  assign douta[19] = \<const0> ;
  assign douta[18] = \<const0> ;
  assign douta[17] = \<const0> ;
  assign douta[16] = \<const0> ;
  assign douta[15] = \<const0> ;
  assign douta[14] = \<const0> ;
  assign douta[13] = \<const0> ;
  assign douta[12] = \<const0> ;
  assign douta[11] = \<const0> ;
  assign douta[10] = \<const0> ;
  assign douta[9] = \<const0> ;
  assign douta[8] = \<const0> ;
  assign douta[7] = \<const0> ;
  assign douta[6] = \<const0> ;
  assign douta[5] = \<const0> ;
  assign douta[4] = \<const0> ;
  assign douta[3] = \<const0> ;
  assign douta[2] = \<const0> ;
  assign douta[1] = \<const0> ;
  assign douta[0] = \<const0> ;
  assign sbiterra = \<const0> ;
  assign sbiterrb = \<const0> ;
  GND GND
       (.G(\<const0> ));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[0] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [0]),
        .Q(\gen_rd_b.doutb_reg [0]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[10] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [10]),
        .Q(\gen_rd_b.doutb_reg [10]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[11] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [11]),
        .Q(\gen_rd_b.doutb_reg [11]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[12] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [12]),
        .Q(\gen_rd_b.doutb_reg [12]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[13] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [13]),
        .Q(\gen_rd_b.doutb_reg [13]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[14] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [14]),
        .Q(\gen_rd_b.doutb_reg [14]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[15] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [15]),
        .Q(\gen_rd_b.doutb_reg [15]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[16] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [16]),
        .Q(\gen_rd_b.doutb_reg [16]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[17] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [17]),
        .Q(\gen_rd_b.doutb_reg [17]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[18] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [18]),
        .Q(\gen_rd_b.doutb_reg [18]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[19] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [19]),
        .Q(\gen_rd_b.doutb_reg [19]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[1] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [1]),
        .Q(\gen_rd_b.doutb_reg [1]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[20] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [20]),
        .Q(\gen_rd_b.doutb_reg [20]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[21] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [21]),
        .Q(\gen_rd_b.doutb_reg [21]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[22] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [22]),
        .Q(\gen_rd_b.doutb_reg [22]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[23] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [23]),
        .Q(\gen_rd_b.doutb_reg [23]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[24] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [24]),
        .Q(\gen_rd_b.doutb_reg [24]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[25] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [25]),
        .Q(\gen_rd_b.doutb_reg [25]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[26] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [26]),
        .Q(\gen_rd_b.doutb_reg [26]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[27] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [27]),
        .Q(\gen_rd_b.doutb_reg [27]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[28] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [28]),
        .Q(\gen_rd_b.doutb_reg [28]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[29] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [29]),
        .Q(\gen_rd_b.doutb_reg [29]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[2] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [2]),
        .Q(\gen_rd_b.doutb_reg [2]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[30] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [30]),
        .Q(\gen_rd_b.doutb_reg [30]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[31] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [31]),
        .Q(\gen_rd_b.doutb_reg [31]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[32] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [32]),
        .Q(\gen_rd_b.doutb_reg [32]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[33] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [33]),
        .Q(\gen_rd_b.doutb_reg [33]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[34] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [34]),
        .Q(\gen_rd_b.doutb_reg [34]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[35] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [35]),
        .Q(\gen_rd_b.doutb_reg [35]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[36] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [36]),
        .Q(\gen_rd_b.doutb_reg [36]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[37] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [37]),
        .Q(\gen_rd_b.doutb_reg [37]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[38] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [38]),
        .Q(\gen_rd_b.doutb_reg [38]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[39] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [39]),
        .Q(\gen_rd_b.doutb_reg [39]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[3] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [3]),
        .Q(\gen_rd_b.doutb_reg [3]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[40] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [40]),
        .Q(\gen_rd_b.doutb_reg [40]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[41] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [41]),
        .Q(\gen_rd_b.doutb_reg [41]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[42] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [42]),
        .Q(\gen_rd_b.doutb_reg [42]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[43] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [43]),
        .Q(\gen_rd_b.doutb_reg [43]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[44] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [44]),
        .Q(\gen_rd_b.doutb_reg [44]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[45] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [45]),
        .Q(\gen_rd_b.doutb_reg [45]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[46] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [46]),
        .Q(\gen_rd_b.doutb_reg [46]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[47] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [47]),
        .Q(\gen_rd_b.doutb_reg [47]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[48] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [48]),
        .Q(\gen_rd_b.doutb_reg [48]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[49] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [49]),
        .Q(\gen_rd_b.doutb_reg [49]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[4] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [4]),
        .Q(\gen_rd_b.doutb_reg [4]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[50] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [50]),
        .Q(\gen_rd_b.doutb_reg [50]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[51] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [51]),
        .Q(\gen_rd_b.doutb_reg [51]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[52] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [52]),
        .Q(\gen_rd_b.doutb_reg [52]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[53] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [53]),
        .Q(\gen_rd_b.doutb_reg [53]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[54] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [54]),
        .Q(\gen_rd_b.doutb_reg [54]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[55] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [55]),
        .Q(\gen_rd_b.doutb_reg [55]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[56] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [56]),
        .Q(\gen_rd_b.doutb_reg [56]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[57] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [57]),
        .Q(\gen_rd_b.doutb_reg [57]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[58] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [58]),
        .Q(\gen_rd_b.doutb_reg [58]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[59] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [59]),
        .Q(\gen_rd_b.doutb_reg [59]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[5] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [5]),
        .Q(\gen_rd_b.doutb_reg [5]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[60] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [60]),
        .Q(\gen_rd_b.doutb_reg [60]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[61] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [61]),
        .Q(\gen_rd_b.doutb_reg [61]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[62] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [62]),
        .Q(\gen_rd_b.doutb_reg [62]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[63] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [63]),
        .Q(\gen_rd_b.doutb_reg [63]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[6] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [6]),
        .Q(\gen_rd_b.doutb_reg [6]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[7] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [7]),
        .Q(\gen_rd_b.doutb_reg [7]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[8] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [8]),
        .Q(\gen_rd_b.doutb_reg [8]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[9] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [9]),
        .Q(\gen_rd_b.doutb_reg [9]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][0] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [0]),
        .Q(doutb[0]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][10] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [10]),
        .Q(doutb[10]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][11] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [11]),
        .Q(doutb[11]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][12] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [12]),
        .Q(doutb[12]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][13] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [13]),
        .Q(doutb[13]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][14] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [14]),
        .Q(doutb[14]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][15] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [15]),
        .Q(doutb[15]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][16] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [16]),
        .Q(doutb[16]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][17] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [17]),
        .Q(doutb[17]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][18] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [18]),
        .Q(doutb[18]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][19] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [19]),
        .Q(doutb[19]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][1] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [1]),
        .Q(doutb[1]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][20] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [20]),
        .Q(doutb[20]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][21] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [21]),
        .Q(doutb[21]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][22] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [22]),
        .Q(doutb[22]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][23] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [23]),
        .Q(doutb[23]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][24] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [24]),
        .Q(doutb[24]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][25] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [25]),
        .Q(doutb[25]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][26] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [26]),
        .Q(doutb[26]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][27] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [27]),
        .Q(doutb[27]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][28] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [28]),
        .Q(doutb[28]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][29] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [29]),
        .Q(doutb[29]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][2] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [2]),
        .Q(doutb[2]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][30] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [30]),
        .Q(doutb[30]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][31] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [31]),
        .Q(doutb[31]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][32] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [32]),
        .Q(doutb[32]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][33] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [33]),
        .Q(doutb[33]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][34] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [34]),
        .Q(doutb[34]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][35] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [35]),
        .Q(doutb[35]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][36] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [36]),
        .Q(doutb[36]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][37] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [37]),
        .Q(doutb[37]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][38] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [38]),
        .Q(doutb[38]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][39] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [39]),
        .Q(doutb[39]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][3] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [3]),
        .Q(doutb[3]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][40] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [40]),
        .Q(doutb[40]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][41] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [41]),
        .Q(doutb[41]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][42] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [42]),
        .Q(doutb[42]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][43] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [43]),
        .Q(doutb[43]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][44] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [44]),
        .Q(doutb[44]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][45] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [45]),
        .Q(doutb[45]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][46] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [46]),
        .Q(doutb[46]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][47] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [47]),
        .Q(doutb[47]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][48] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [48]),
        .Q(doutb[48]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][49] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [49]),
        .Q(doutb[49]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][4] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [4]),
        .Q(doutb[4]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][50] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [50]),
        .Q(doutb[50]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][51] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [51]),
        .Q(doutb[51]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][52] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [52]),
        .Q(doutb[52]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][53] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [53]),
        .Q(doutb[53]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][54] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [54]),
        .Q(doutb[54]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][55] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [55]),
        .Q(doutb[55]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][56] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [56]),
        .Q(doutb[56]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][57] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [57]),
        .Q(doutb[57]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][58] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [58]),
        .Q(doutb[58]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][59] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [59]),
        .Q(doutb[59]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][5] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [5]),
        .Q(doutb[5]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][60] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [60]),
        .Q(doutb[60]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][61] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [61]),
        .Q(doutb[61]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][62] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [62]),
        .Q(doutb[62]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][63] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [63]),
        .Q(doutb[63]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][6] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [6]),
        .Q(doutb[6]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][7] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [7]),
        .Q(doutb[7]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][8] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [8]),
        .Q(doutb[8]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][9] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [9]),
        .Q(doutb[9]),
        .R(rstb));
  (* METHODOLOGY_DRC_VIOS = "" *) 
  (* RTL_RAM_BITS = "1024" *) 
  (* RTL_RAM_NAME = "gen_wr_a.gen_word_narrow.mem" *) 
  (* RTL_RAM_TYPE = "RAM_SDP" *) 
  (* dram_emb_xdc = "yes" *) 
  (* ram_addr_begin = "0" *) 
  (* ram_addr_end = "15" *) 
  (* ram_offset = "0" *) 
  (* ram_slice_begin = "0" *) 
  (* ram_slice_end = "13" *) 
  RAM32M16 #(
    .INIT_A(64'h0000000000000000),
    .INIT_B(64'h0000000000000000),
    .INIT_C(64'h0000000000000000),
    .INIT_D(64'h0000000000000000),
    .INIT_E(64'h0000000000000000),
    .INIT_F(64'h0000000000000000),
    .INIT_G(64'h0000000000000000),
    .INIT_H(64'h0000000000000000)) 
    \gen_wr_a.gen_word_narrow.mem_reg_0_15_0_13 
       (.ADDRA({1'b0,addrb}),
        .ADDRB({1'b0,addrb}),
        .ADDRC({1'b0,addrb}),
        .ADDRD({1'b0,addrb}),
        .ADDRE({1'b0,addrb}),
        .ADDRF({1'b0,addrb}),
        .ADDRG({1'b0,addrb}),
        .ADDRH({1'b0,addra}),
        .DIA(dina[1:0]),
        .DIB(dina[3:2]),
        .DIC(dina[5:4]),
        .DID(dina[7:6]),
        .DIE(dina[9:8]),
        .DIF(dina[11:10]),
        .DIG(dina[13:12]),
        .DIH({1'b0,1'b0}),
        .DOA(\gen_rd_b.doutb_reg0 [1:0]),
        .DOB(\gen_rd_b.doutb_reg0 [3:2]),
        .DOC(\gen_rd_b.doutb_reg0 [5:4]),
        .DOD(\gen_rd_b.doutb_reg0 [7:6]),
        .DOE(\gen_rd_b.doutb_reg0 [9:8]),
        .DOF(\gen_rd_b.doutb_reg0 [11:10]),
        .DOG(\gen_rd_b.doutb_reg0 [13:12]),
        .DOH(\NLW_gen_wr_a.gen_word_narrow.mem_reg_0_15_0_13_DOH_UNCONNECTED [1:0]),
        .WCLK(clka),
        .WE(ena));
  (* METHODOLOGY_DRC_VIOS = "" *) 
  (* RTL_RAM_BITS = "1024" *) 
  (* RTL_RAM_NAME = "gen_wr_a.gen_word_narrow.mem" *) 
  (* RTL_RAM_TYPE = "RAM_SDP" *) 
  (* dram_emb_xdc = "yes" *) 
  (* ram_addr_begin = "0" *) 
  (* ram_addr_end = "15" *) 
  (* ram_offset = "0" *) 
  (* ram_slice_begin = "14" *) 
  (* ram_slice_end = "27" *) 
  RAM32M16 #(
    .INIT_A(64'h0000000000000000),
    .INIT_B(64'h0000000000000000),
    .INIT_C(64'h0000000000000000),
    .INIT_D(64'h0000000000000000),
    .INIT_E(64'h0000000000000000),
    .INIT_F(64'h0000000000000000),
    .INIT_G(64'h0000000000000000),
    .INIT_H(64'h0000000000000000)) 
    \gen_wr_a.gen_word_narrow.mem_reg_0_15_14_27 
       (.ADDRA({1'b0,addrb}),
        .ADDRB({1'b0,addrb}),
        .ADDRC({1'b0,addrb}),
        .ADDRD({1'b0,addrb}),
        .ADDRE({1'b0,addrb}),
        .ADDRF({1'b0,addrb}),
        .ADDRG({1'b0,addrb}),
        .ADDRH({1'b0,addra}),
        .DIA(dina[15:14]),
        .DIB(dina[17:16]),
        .DIC(dina[19:18]),
        .DID(dina[21:20]),
        .DIE(dina[23:22]),
        .DIF(dina[25:24]),
        .DIG(dina[27:26]),
        .DIH({1'b0,1'b0}),
        .DOA(\gen_rd_b.doutb_reg0 [15:14]),
        .DOB(\gen_rd_b.doutb_reg0 [17:16]),
        .DOC(\gen_rd_b.doutb_reg0 [19:18]),
        .DOD(\gen_rd_b.doutb_reg0 [21:20]),
        .DOE(\gen_rd_b.doutb_reg0 [23:22]),
        .DOF(\gen_rd_b.doutb_reg0 [25:24]),
        .DOG(\gen_rd_b.doutb_reg0 [27:26]),
        .DOH(\NLW_gen_wr_a.gen_word_narrow.mem_reg_0_15_14_27_DOH_UNCONNECTED [1:0]),
        .WCLK(clka),
        .WE(ena));
  (* METHODOLOGY_DRC_VIOS = "" *) 
  (* RTL_RAM_BITS = "1024" *) 
  (* RTL_RAM_NAME = "gen_wr_a.gen_word_narrow.mem" *) 
  (* RTL_RAM_TYPE = "RAM_SDP" *) 
  (* dram_emb_xdc = "yes" *) 
  (* ram_addr_begin = "0" *) 
  (* ram_addr_end = "15" *) 
  (* ram_offset = "0" *) 
  (* ram_slice_begin = "28" *) 
  (* ram_slice_end = "41" *) 
  RAM32M16 #(
    .INIT_A(64'h0000000000000000),
    .INIT_B(64'h0000000000000000),
    .INIT_C(64'h0000000000000000),
    .INIT_D(64'h0000000000000000),
    .INIT_E(64'h0000000000000000),
    .INIT_F(64'h0000000000000000),
    .INIT_G(64'h0000000000000000),
    .INIT_H(64'h0000000000000000)) 
    \gen_wr_a.gen_word_narrow.mem_reg_0_15_28_41 
       (.ADDRA({1'b0,addrb}),
        .ADDRB({1'b0,addrb}),
        .ADDRC({1'b0,addrb}),
        .ADDRD({1'b0,addrb}),
        .ADDRE({1'b0,addrb}),
        .ADDRF({1'b0,addrb}),
        .ADDRG({1'b0,addrb}),
        .ADDRH({1'b0,addra}),
        .DIA(dina[29:28]),
        .DIB(dina[31:30]),
        .DIC(dina[33:32]),
        .DID(dina[35:34]),
        .DIE(dina[37:36]),
        .DIF(dina[39:38]),
        .DIG(dina[41:40]),
        .DIH({1'b0,1'b0}),
        .DOA(\gen_rd_b.doutb_reg0 [29:28]),
        .DOB(\gen_rd_b.doutb_reg0 [31:30]),
        .DOC(\gen_rd_b.doutb_reg0 [33:32]),
        .DOD(\gen_rd_b.doutb_reg0 [35:34]),
        .DOE(\gen_rd_b.doutb_reg0 [37:36]),
        .DOF(\gen_rd_b.doutb_reg0 [39:38]),
        .DOG(\gen_rd_b.doutb_reg0 [41:40]),
        .DOH(\NLW_gen_wr_a.gen_word_narrow.mem_reg_0_15_28_41_DOH_UNCONNECTED [1:0]),
        .WCLK(clka),
        .WE(ena));
  (* METHODOLOGY_DRC_VIOS = "" *) 
  (* RTL_RAM_BITS = "1024" *) 
  (* RTL_RAM_NAME = "gen_wr_a.gen_word_narrow.mem" *) 
  (* RTL_RAM_TYPE = "RAM_SDP" *) 
  (* dram_emb_xdc = "yes" *) 
  (* ram_addr_begin = "0" *) 
  (* ram_addr_end = "15" *) 
  (* ram_offset = "0" *) 
  (* ram_slice_begin = "42" *) 
  (* ram_slice_end = "55" *) 
  RAM32M16 #(
    .INIT_A(64'h0000000000000000),
    .INIT_B(64'h0000000000000000),
    .INIT_C(64'h0000000000000000),
    .INIT_D(64'h0000000000000000),
    .INIT_E(64'h0000000000000000),
    .INIT_F(64'h0000000000000000),
    .INIT_G(64'h0000000000000000),
    .INIT_H(64'h0000000000000000)) 
    \gen_wr_a.gen_word_narrow.mem_reg_0_15_42_55 
       (.ADDRA({1'b0,addrb}),
        .ADDRB({1'b0,addrb}),
        .ADDRC({1'b0,addrb}),
        .ADDRD({1'b0,addrb}),
        .ADDRE({1'b0,addrb}),
        .ADDRF({1'b0,addrb}),
        .ADDRG({1'b0,addrb}),
        .ADDRH({1'b0,addra}),
        .DIA(dina[43:42]),
        .DIB(dina[45:44]),
        .DIC(dina[47:46]),
        .DID(dina[49:48]),
        .DIE(dina[51:50]),
        .DIF(dina[53:52]),
        .DIG(dina[55:54]),
        .DIH({1'b0,1'b0}),
        .DOA(\gen_rd_b.doutb_reg0 [43:42]),
        .DOB(\gen_rd_b.doutb_reg0 [45:44]),
        .DOC(\gen_rd_b.doutb_reg0 [47:46]),
        .DOD(\gen_rd_b.doutb_reg0 [49:48]),
        .DOE(\gen_rd_b.doutb_reg0 [51:50]),
        .DOF(\gen_rd_b.doutb_reg0 [53:52]),
        .DOG(\gen_rd_b.doutb_reg0 [55:54]),
        .DOH(\NLW_gen_wr_a.gen_word_narrow.mem_reg_0_15_42_55_DOH_UNCONNECTED [1:0]),
        .WCLK(clka),
        .WE(ena));
  (* METHODOLOGY_DRC_VIOS = "" *) 
  (* RTL_RAM_BITS = "1024" *) 
  (* RTL_RAM_NAME = "gen_wr_a.gen_word_narrow.mem" *) 
  (* RTL_RAM_TYPE = "RAM_SDP" *) 
  (* dram_emb_xdc = "yes" *) 
  (* ram_addr_begin = "0" *) 
  (* ram_addr_end = "15" *) 
  (* ram_offset = "0" *) 
  (* ram_slice_begin = "56" *) 
  (* ram_slice_end = "63" *) 
  RAM32M16 #(
    .INIT_A(64'h0000000000000000),
    .INIT_B(64'h0000000000000000),
    .INIT_C(64'h0000000000000000),
    .INIT_D(64'h0000000000000000),
    .INIT_E(64'h0000000000000000),
    .INIT_F(64'h0000000000000000),
    .INIT_G(64'h0000000000000000),
    .INIT_H(64'h0000000000000000)) 
    \gen_wr_a.gen_word_narrow.mem_reg_0_15_56_63 
       (.ADDRA({1'b0,addrb}),
        .ADDRB({1'b0,addrb}),
        .ADDRC({1'b0,addrb}),
        .ADDRD({1'b0,addrb}),
        .ADDRE({1'b0,addrb}),
        .ADDRF({1'b0,addrb}),
        .ADDRG({1'b0,addrb}),
        .ADDRH({1'b0,addra}),
        .DIA(dina[57:56]),
        .DIB(dina[59:58]),
        .DIC(dina[61:60]),
        .DID(dina[63:62]),
        .DIE({1'b0,1'b0}),
        .DIF({1'b0,1'b0}),
        .DIG({1'b0,1'b0}),
        .DIH({1'b0,1'b0}),
        .DOA(\gen_rd_b.doutb_reg0 [57:56]),
        .DOB(\gen_rd_b.doutb_reg0 [59:58]),
        .DOC(\gen_rd_b.doutb_reg0 [61:60]),
        .DOD(\gen_rd_b.doutb_reg0 [63:62]),
        .DOE(\NLW_gen_wr_a.gen_word_narrow.mem_reg_0_15_56_63_DOE_UNCONNECTED [1:0]),
        .DOF(\NLW_gen_wr_a.gen_word_narrow.mem_reg_0_15_56_63_DOF_UNCONNECTED [1:0]),
        .DOG(\NLW_gen_wr_a.gen_word_narrow.mem_reg_0_15_56_63_DOG_UNCONNECTED [1:0]),
        .DOH(\NLW_gen_wr_a.gen_word_narrow.mem_reg_0_15_56_63_DOH_UNCONNECTED [1:0]),
        .WCLK(clka),
        .WE(ena));
endmodule

(* ADDR_WIDTH_A = "4" *) (* ADDR_WIDTH_B = "4" *) (* AUTO_SLEEP_TIME = "0" *) 
(* BYTE_WRITE_WIDTH_A = "56" *) (* BYTE_WRITE_WIDTH_B = "56" *) (* CASCADE_HEIGHT = "0" *) 
(* CLOCKING_MODE = "1" *) (* ECC_MODE = "0" *) (* MAX_NUM_CHAR = "0" *) 
(* MEMORY_INIT_FILE = "none" *) (* MEMORY_INIT_PARAM = "" *) (* MEMORY_OPTIMIZATION = "true" *) 
(* MEMORY_PRIMITIVE = "1" *) (* MEMORY_SIZE = "896" *) (* MEMORY_TYPE = "1" *) 
(* MESSAGE_CONTROL = "0" *) (* NUM_CHAR_LOC = "0" *) (* ORIG_REF_NAME = "xpm_memory_base" *) 
(* P_ECC_MODE = "no_ecc" *) (* P_ENABLE_BYTE_WRITE_A = "0" *) (* P_ENABLE_BYTE_WRITE_B = "0" *) 
(* P_MAX_DEPTH_DATA = "16" *) (* P_MEMORY_OPT = "yes" *) (* P_MEMORY_PRIMITIVE = "distributed" *) 
(* P_MIN_WIDTH_DATA = "56" *) (* P_MIN_WIDTH_DATA_A = "56" *) (* P_MIN_WIDTH_DATA_B = "56" *) 
(* P_MIN_WIDTH_DATA_ECC = "56" *) (* P_MIN_WIDTH_DATA_LDW = "4" *) (* P_MIN_WIDTH_DATA_SHFT = "56" *) 
(* P_NUM_COLS_WRITE_A = "1" *) (* P_NUM_COLS_WRITE_B = "1" *) (* P_NUM_ROWS_READ_A = "1" *) 
(* P_NUM_ROWS_READ_B = "1" *) (* P_NUM_ROWS_WRITE_A = "1" *) (* P_NUM_ROWS_WRITE_B = "1" *) 
(* P_SDP_WRITE_MODE = "yes" *) (* P_WIDTH_ADDR_LSB_READ_A = "0" *) (* P_WIDTH_ADDR_LSB_READ_B = "0" *) 
(* P_WIDTH_ADDR_LSB_WRITE_A = "0" *) (* P_WIDTH_ADDR_LSB_WRITE_B = "0" *) (* P_WIDTH_ADDR_READ_A = "4" *) 
(* P_WIDTH_ADDR_READ_B = "4" *) (* P_WIDTH_ADDR_WRITE_A = "4" *) (* P_WIDTH_ADDR_WRITE_B = "4" *) 
(* P_WIDTH_COL_WRITE_A = "56" *) (* P_WIDTH_COL_WRITE_B = "56" *) (* READ_DATA_WIDTH_A = "56" *) 
(* READ_DATA_WIDTH_B = "56" *) (* READ_LATENCY_A = "2" *) (* READ_LATENCY_B = "2" *) 
(* READ_RESET_VALUE_A = "0" *) (* READ_RESET_VALUE_B = "0" *) (* RST_MODE_A = "SYNC" *) 
(* RST_MODE_B = "SYNC" *) (* SIM_ASSERT_CHK = "0" *) (* USE_EMBEDDED_CONSTRAINT = "1" *) 
(* USE_MEM_INIT = "0" *) (* USE_MEM_INIT_MMI = "0" *) (* VERSION = "0" *) 
(* WAKEUP_TIME = "0" *) (* WRITE_DATA_WIDTH_A = "56" *) (* WRITE_DATA_WIDTH_B = "56" *) 
(* WRITE_MODE_A = "2" *) (* WRITE_MODE_B = "1" *) (* WRITE_PROTECT = "1" *) 
(* XPM_MODULE = "TRUE" *) (* keep_hierarchy = "soft" *) (* rsta_loop_iter = "56" *) 
(* rstb_loop_iter = "56" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_memory_base__parameterized0
   (sleep,
    clka,
    rsta,
    ena,
    regcea,
    wea,
    addra,
    dina,
    injectsbiterra,
    injectdbiterra,
    douta,
    sbiterra,
    dbiterra,
    clkb,
    rstb,
    enb,
    regceb,
    web,
    addrb,
    dinb,
    injectsbiterrb,
    injectdbiterrb,
    doutb,
    sbiterrb,
    dbiterrb);
  input sleep;
  input clka;
  input rsta;
  input ena;
  input regcea;
  input [0:0]wea;
  input [3:0]addra;
  input [55:0]dina;
  input injectsbiterra;
  input injectdbiterra;
  output [55:0]douta;
  output sbiterra;
  output dbiterra;
  input clkb;
  input rstb;
  input enb;
  input regceb;
  input [0:0]web;
  input [3:0]addrb;
  input [55:0]dinb;
  input injectsbiterrb;
  input injectdbiterrb;
  output [55:0]doutb;
  output sbiterrb;
  output dbiterrb;

  wire \<const0> ;
  wire [3:0]addra;
  wire [3:0]addrb;
  wire clka;
  wire clkb;
  wire [55:0]dina;
  wire [55:0]doutb;
  wire ena;
  wire enb;
  wire [55:0]\gen_rd_b.doutb_reg ;
  wire [55:0]\gen_rd_b.doutb_reg0 ;
  wire regceb;
  wire rstb;
  wire sleep;
  wire [1:0]\NLW_gen_wr_a.gen_word_narrow.mem_reg_0_15_0_13_DOH_UNCONNECTED ;
  wire [1:0]\NLW_gen_wr_a.gen_word_narrow.mem_reg_0_15_14_27_DOH_UNCONNECTED ;
  wire [1:0]\NLW_gen_wr_a.gen_word_narrow.mem_reg_0_15_28_41_DOH_UNCONNECTED ;
  wire [1:0]\NLW_gen_wr_a.gen_word_narrow.mem_reg_0_15_42_55_DOH_UNCONNECTED ;

  assign dbiterra = \<const0> ;
  assign dbiterrb = \<const0> ;
  assign douta[55] = \<const0> ;
  assign douta[54] = \<const0> ;
  assign douta[53] = \<const0> ;
  assign douta[52] = \<const0> ;
  assign douta[51] = \<const0> ;
  assign douta[50] = \<const0> ;
  assign douta[49] = \<const0> ;
  assign douta[48] = \<const0> ;
  assign douta[47] = \<const0> ;
  assign douta[46] = \<const0> ;
  assign douta[45] = \<const0> ;
  assign douta[44] = \<const0> ;
  assign douta[43] = \<const0> ;
  assign douta[42] = \<const0> ;
  assign douta[41] = \<const0> ;
  assign douta[40] = \<const0> ;
  assign douta[39] = \<const0> ;
  assign douta[38] = \<const0> ;
  assign douta[37] = \<const0> ;
  assign douta[36] = \<const0> ;
  assign douta[35] = \<const0> ;
  assign douta[34] = \<const0> ;
  assign douta[33] = \<const0> ;
  assign douta[32] = \<const0> ;
  assign douta[31] = \<const0> ;
  assign douta[30] = \<const0> ;
  assign douta[29] = \<const0> ;
  assign douta[28] = \<const0> ;
  assign douta[27] = \<const0> ;
  assign douta[26] = \<const0> ;
  assign douta[25] = \<const0> ;
  assign douta[24] = \<const0> ;
  assign douta[23] = \<const0> ;
  assign douta[22] = \<const0> ;
  assign douta[21] = \<const0> ;
  assign douta[20] = \<const0> ;
  assign douta[19] = \<const0> ;
  assign douta[18] = \<const0> ;
  assign douta[17] = \<const0> ;
  assign douta[16] = \<const0> ;
  assign douta[15] = \<const0> ;
  assign douta[14] = \<const0> ;
  assign douta[13] = \<const0> ;
  assign douta[12] = \<const0> ;
  assign douta[11] = \<const0> ;
  assign douta[10] = \<const0> ;
  assign douta[9] = \<const0> ;
  assign douta[8] = \<const0> ;
  assign douta[7] = \<const0> ;
  assign douta[6] = \<const0> ;
  assign douta[5] = \<const0> ;
  assign douta[4] = \<const0> ;
  assign douta[3] = \<const0> ;
  assign douta[2] = \<const0> ;
  assign douta[1] = \<const0> ;
  assign douta[0] = \<const0> ;
  assign sbiterra = \<const0> ;
  assign sbiterrb = \<const0> ;
  GND GND
       (.G(\<const0> ));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[0] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [0]),
        .Q(\gen_rd_b.doutb_reg [0]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[10] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [10]),
        .Q(\gen_rd_b.doutb_reg [10]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[11] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [11]),
        .Q(\gen_rd_b.doutb_reg [11]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[12] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [12]),
        .Q(\gen_rd_b.doutb_reg [12]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[13] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [13]),
        .Q(\gen_rd_b.doutb_reg [13]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[14] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [14]),
        .Q(\gen_rd_b.doutb_reg [14]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[15] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [15]),
        .Q(\gen_rd_b.doutb_reg [15]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[16] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [16]),
        .Q(\gen_rd_b.doutb_reg [16]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[17] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [17]),
        .Q(\gen_rd_b.doutb_reg [17]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[18] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [18]),
        .Q(\gen_rd_b.doutb_reg [18]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[19] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [19]),
        .Q(\gen_rd_b.doutb_reg [19]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[1] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [1]),
        .Q(\gen_rd_b.doutb_reg [1]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[20] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [20]),
        .Q(\gen_rd_b.doutb_reg [20]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[21] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [21]),
        .Q(\gen_rd_b.doutb_reg [21]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[22] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [22]),
        .Q(\gen_rd_b.doutb_reg [22]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[23] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [23]),
        .Q(\gen_rd_b.doutb_reg [23]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[24] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [24]),
        .Q(\gen_rd_b.doutb_reg [24]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[25] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [25]),
        .Q(\gen_rd_b.doutb_reg [25]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[26] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [26]),
        .Q(\gen_rd_b.doutb_reg [26]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[27] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [27]),
        .Q(\gen_rd_b.doutb_reg [27]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[28] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [28]),
        .Q(\gen_rd_b.doutb_reg [28]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[29] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [29]),
        .Q(\gen_rd_b.doutb_reg [29]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[2] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [2]),
        .Q(\gen_rd_b.doutb_reg [2]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[30] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [30]),
        .Q(\gen_rd_b.doutb_reg [30]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[31] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [31]),
        .Q(\gen_rd_b.doutb_reg [31]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[32] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [32]),
        .Q(\gen_rd_b.doutb_reg [32]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[33] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [33]),
        .Q(\gen_rd_b.doutb_reg [33]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[34] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [34]),
        .Q(\gen_rd_b.doutb_reg [34]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[35] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [35]),
        .Q(\gen_rd_b.doutb_reg [35]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[36] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [36]),
        .Q(\gen_rd_b.doutb_reg [36]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[37] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [37]),
        .Q(\gen_rd_b.doutb_reg [37]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[38] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [38]),
        .Q(\gen_rd_b.doutb_reg [38]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[39] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [39]),
        .Q(\gen_rd_b.doutb_reg [39]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[3] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [3]),
        .Q(\gen_rd_b.doutb_reg [3]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[40] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [40]),
        .Q(\gen_rd_b.doutb_reg [40]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[41] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [41]),
        .Q(\gen_rd_b.doutb_reg [41]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[42] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [42]),
        .Q(\gen_rd_b.doutb_reg [42]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[43] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [43]),
        .Q(\gen_rd_b.doutb_reg [43]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[44] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [44]),
        .Q(\gen_rd_b.doutb_reg [44]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[45] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [45]),
        .Q(\gen_rd_b.doutb_reg [45]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[46] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [46]),
        .Q(\gen_rd_b.doutb_reg [46]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[47] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [47]),
        .Q(\gen_rd_b.doutb_reg [47]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[48] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [48]),
        .Q(\gen_rd_b.doutb_reg [48]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[49] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [49]),
        .Q(\gen_rd_b.doutb_reg [49]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[4] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [4]),
        .Q(\gen_rd_b.doutb_reg [4]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[50] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [50]),
        .Q(\gen_rd_b.doutb_reg [50]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[51] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [51]),
        .Q(\gen_rd_b.doutb_reg [51]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[52] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [52]),
        .Q(\gen_rd_b.doutb_reg [52]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[53] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [53]),
        .Q(\gen_rd_b.doutb_reg [53]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[54] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [54]),
        .Q(\gen_rd_b.doutb_reg [54]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[55] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [55]),
        .Q(\gen_rd_b.doutb_reg [55]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[5] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [5]),
        .Q(\gen_rd_b.doutb_reg [5]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[6] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [6]),
        .Q(\gen_rd_b.doutb_reg [6]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[7] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [7]),
        .Q(\gen_rd_b.doutb_reg [7]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[8] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [8]),
        .Q(\gen_rd_b.doutb_reg [8]),
        .R(1'b0));
  (* dram_emb_xdc = "yes" *) 
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.doutb_reg_reg[9] 
       (.C(clkb),
        .CE(enb),
        .D(\gen_rd_b.doutb_reg0 [9]),
        .Q(\gen_rd_b.doutb_reg [9]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][0] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [0]),
        .Q(doutb[0]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][10] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [10]),
        .Q(doutb[10]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][11] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [11]),
        .Q(doutb[11]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][12] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [12]),
        .Q(doutb[12]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][13] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [13]),
        .Q(doutb[13]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][14] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [14]),
        .Q(doutb[14]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][15] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [15]),
        .Q(doutb[15]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][16] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [16]),
        .Q(doutb[16]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][17] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [17]),
        .Q(doutb[17]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][18] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [18]),
        .Q(doutb[18]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][19] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [19]),
        .Q(doutb[19]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][1] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [1]),
        .Q(doutb[1]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][20] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [20]),
        .Q(doutb[20]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][21] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [21]),
        .Q(doutb[21]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][22] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [22]),
        .Q(doutb[22]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][23] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [23]),
        .Q(doutb[23]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][24] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [24]),
        .Q(doutb[24]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][25] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [25]),
        .Q(doutb[25]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][26] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [26]),
        .Q(doutb[26]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][27] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [27]),
        .Q(doutb[27]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][28] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [28]),
        .Q(doutb[28]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][29] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [29]),
        .Q(doutb[29]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][2] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [2]),
        .Q(doutb[2]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][30] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [30]),
        .Q(doutb[30]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][31] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [31]),
        .Q(doutb[31]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][32] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [32]),
        .Q(doutb[32]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][33] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [33]),
        .Q(doutb[33]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][34] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [34]),
        .Q(doutb[34]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][35] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [35]),
        .Q(doutb[35]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][36] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [36]),
        .Q(doutb[36]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][37] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [37]),
        .Q(doutb[37]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][38] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [38]),
        .Q(doutb[38]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][39] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [39]),
        .Q(doutb[39]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][3] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [3]),
        .Q(doutb[3]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][40] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [40]),
        .Q(doutb[40]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][41] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [41]),
        .Q(doutb[41]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][42] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [42]),
        .Q(doutb[42]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][43] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [43]),
        .Q(doutb[43]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][44] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [44]),
        .Q(doutb[44]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][45] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [45]),
        .Q(doutb[45]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][46] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [46]),
        .Q(doutb[46]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][47] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [47]),
        .Q(doutb[47]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][48] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [48]),
        .Q(doutb[48]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][49] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [49]),
        .Q(doutb[49]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][4] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [4]),
        .Q(doutb[4]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][50] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [50]),
        .Q(doutb[50]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][51] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [51]),
        .Q(doutb[51]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][52] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [52]),
        .Q(doutb[52]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][53] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [53]),
        .Q(doutb[53]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][54] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [54]),
        .Q(doutb[54]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][55] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [55]),
        .Q(doutb[55]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][5] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [5]),
        .Q(doutb[5]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][6] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [6]),
        .Q(doutb[6]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][7] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [7]),
        .Q(doutb[7]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][8] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [8]),
        .Q(doutb[8]),
        .R(rstb));
  FDRE #(
    .INIT(1'b0)) 
    \gen_rd_b.gen_doutb_pipe.doutb_pipe_reg[0][9] 
       (.C(clkb),
        .CE(regceb),
        .D(\gen_rd_b.doutb_reg [9]),
        .Q(doutb[9]),
        .R(rstb));
  (* METHODOLOGY_DRC_VIOS = "" *) 
  (* RTL_RAM_BITS = "896" *) 
  (* RTL_RAM_NAME = "gen_wr_a.gen_word_narrow.mem" *) 
  (* RTL_RAM_TYPE = "RAM_SDP" *) 
  (* dram_emb_xdc = "yes" *) 
  (* ram_addr_begin = "0" *) 
  (* ram_addr_end = "15" *) 
  (* ram_offset = "0" *) 
  (* ram_slice_begin = "0" *) 
  (* ram_slice_end = "13" *) 
  RAM32M16 #(
    .INIT_A(64'h0000000000000000),
    .INIT_B(64'h0000000000000000),
    .INIT_C(64'h0000000000000000),
    .INIT_D(64'h0000000000000000),
    .INIT_E(64'h0000000000000000),
    .INIT_F(64'h0000000000000000),
    .INIT_G(64'h0000000000000000),
    .INIT_H(64'h0000000000000000)) 
    \gen_wr_a.gen_word_narrow.mem_reg_0_15_0_13 
       (.ADDRA({1'b0,addrb}),
        .ADDRB({1'b0,addrb}),
        .ADDRC({1'b0,addrb}),
        .ADDRD({1'b0,addrb}),
        .ADDRE({1'b0,addrb}),
        .ADDRF({1'b0,addrb}),
        .ADDRG({1'b0,addrb}),
        .ADDRH({1'b0,addra}),
        .DIA(dina[1:0]),
        .DIB(dina[3:2]),
        .DIC(dina[5:4]),
        .DID(dina[7:6]),
        .DIE(dina[9:8]),
        .DIF(dina[11:10]),
        .DIG(dina[13:12]),
        .DIH({1'b0,1'b0}),
        .DOA(\gen_rd_b.doutb_reg0 [1:0]),
        .DOB(\gen_rd_b.doutb_reg0 [3:2]),
        .DOC(\gen_rd_b.doutb_reg0 [5:4]),
        .DOD(\gen_rd_b.doutb_reg0 [7:6]),
        .DOE(\gen_rd_b.doutb_reg0 [9:8]),
        .DOF(\gen_rd_b.doutb_reg0 [11:10]),
        .DOG(\gen_rd_b.doutb_reg0 [13:12]),
        .DOH(\NLW_gen_wr_a.gen_word_narrow.mem_reg_0_15_0_13_DOH_UNCONNECTED [1:0]),
        .WCLK(clka),
        .WE(ena));
  (* METHODOLOGY_DRC_VIOS = "" *) 
  (* RTL_RAM_BITS = "896" *) 
  (* RTL_RAM_NAME = "gen_wr_a.gen_word_narrow.mem" *) 
  (* RTL_RAM_TYPE = "RAM_SDP" *) 
  (* dram_emb_xdc = "yes" *) 
  (* ram_addr_begin = "0" *) 
  (* ram_addr_end = "15" *) 
  (* ram_offset = "0" *) 
  (* ram_slice_begin = "14" *) 
  (* ram_slice_end = "27" *) 
  RAM32M16 #(
    .INIT_A(64'h0000000000000000),
    .INIT_B(64'h0000000000000000),
    .INIT_C(64'h0000000000000000),
    .INIT_D(64'h0000000000000000),
    .INIT_E(64'h0000000000000000),
    .INIT_F(64'h0000000000000000),
    .INIT_G(64'h0000000000000000),
    .INIT_H(64'h0000000000000000)) 
    \gen_wr_a.gen_word_narrow.mem_reg_0_15_14_27 
       (.ADDRA({1'b0,addrb}),
        .ADDRB({1'b0,addrb}),
        .ADDRC({1'b0,addrb}),
        .ADDRD({1'b0,addrb}),
        .ADDRE({1'b0,addrb}),
        .ADDRF({1'b0,addrb}),
        .ADDRG({1'b0,addrb}),
        .ADDRH({1'b0,addra}),
        .DIA(dina[15:14]),
        .DIB(dina[17:16]),
        .DIC(dina[19:18]),
        .DID(dina[21:20]),
        .DIE(dina[23:22]),
        .DIF(dina[25:24]),
        .DIG(dina[27:26]),
        .DIH({1'b0,1'b0}),
        .DOA(\gen_rd_b.doutb_reg0 [15:14]),
        .DOB(\gen_rd_b.doutb_reg0 [17:16]),
        .DOC(\gen_rd_b.doutb_reg0 [19:18]),
        .DOD(\gen_rd_b.doutb_reg0 [21:20]),
        .DOE(\gen_rd_b.doutb_reg0 [23:22]),
        .DOF(\gen_rd_b.doutb_reg0 [25:24]),
        .DOG(\gen_rd_b.doutb_reg0 [27:26]),
        .DOH(\NLW_gen_wr_a.gen_word_narrow.mem_reg_0_15_14_27_DOH_UNCONNECTED [1:0]),
        .WCLK(clka),
        .WE(ena));
  (* METHODOLOGY_DRC_VIOS = "" *) 
  (* RTL_RAM_BITS = "896" *) 
  (* RTL_RAM_NAME = "gen_wr_a.gen_word_narrow.mem" *) 
  (* RTL_RAM_TYPE = "RAM_SDP" *) 
  (* dram_emb_xdc = "yes" *) 
  (* ram_addr_begin = "0" *) 
  (* ram_addr_end = "15" *) 
  (* ram_offset = "0" *) 
  (* ram_slice_begin = "28" *) 
  (* ram_slice_end = "41" *) 
  RAM32M16 #(
    .INIT_A(64'h0000000000000000),
    .INIT_B(64'h0000000000000000),
    .INIT_C(64'h0000000000000000),
    .INIT_D(64'h0000000000000000),
    .INIT_E(64'h0000000000000000),
    .INIT_F(64'h0000000000000000),
    .INIT_G(64'h0000000000000000),
    .INIT_H(64'h0000000000000000)) 
    \gen_wr_a.gen_word_narrow.mem_reg_0_15_28_41 
       (.ADDRA({1'b0,addrb}),
        .ADDRB({1'b0,addrb}),
        .ADDRC({1'b0,addrb}),
        .ADDRD({1'b0,addrb}),
        .ADDRE({1'b0,addrb}),
        .ADDRF({1'b0,addrb}),
        .ADDRG({1'b0,addrb}),
        .ADDRH({1'b0,addra}),
        .DIA(dina[29:28]),
        .DIB(dina[31:30]),
        .DIC(dina[33:32]),
        .DID(dina[35:34]),
        .DIE(dina[37:36]),
        .DIF(dina[39:38]),
        .DIG(dina[41:40]),
        .DIH({1'b0,1'b0}),
        .DOA(\gen_rd_b.doutb_reg0 [29:28]),
        .DOB(\gen_rd_b.doutb_reg0 [31:30]),
        .DOC(\gen_rd_b.doutb_reg0 [33:32]),
        .DOD(\gen_rd_b.doutb_reg0 [35:34]),
        .DOE(\gen_rd_b.doutb_reg0 [37:36]),
        .DOF(\gen_rd_b.doutb_reg0 [39:38]),
        .DOG(\gen_rd_b.doutb_reg0 [41:40]),
        .DOH(\NLW_gen_wr_a.gen_word_narrow.mem_reg_0_15_28_41_DOH_UNCONNECTED [1:0]),
        .WCLK(clka),
        .WE(ena));
  (* METHODOLOGY_DRC_VIOS = "" *) 
  (* RTL_RAM_BITS = "896" *) 
  (* RTL_RAM_NAME = "gen_wr_a.gen_word_narrow.mem" *) 
  (* RTL_RAM_TYPE = "RAM_SDP" *) 
  (* dram_emb_xdc = "yes" *) 
  (* ram_addr_begin = "0" *) 
  (* ram_addr_end = "15" *) 
  (* ram_offset = "0" *) 
  (* ram_slice_begin = "42" *) 
  (* ram_slice_end = "55" *) 
  RAM32M16 #(
    .INIT_A(64'h0000000000000000),
    .INIT_B(64'h0000000000000000),
    .INIT_C(64'h0000000000000000),
    .INIT_D(64'h0000000000000000),
    .INIT_E(64'h0000000000000000),
    .INIT_F(64'h0000000000000000),
    .INIT_G(64'h0000000000000000),
    .INIT_H(64'h0000000000000000)) 
    \gen_wr_a.gen_word_narrow.mem_reg_0_15_42_55 
       (.ADDRA({1'b0,addrb}),
        .ADDRB({1'b0,addrb}),
        .ADDRC({1'b0,addrb}),
        .ADDRD({1'b0,addrb}),
        .ADDRE({1'b0,addrb}),
        .ADDRF({1'b0,addrb}),
        .ADDRG({1'b0,addrb}),
        .ADDRH({1'b0,addra}),
        .DIA(dina[43:42]),
        .DIB(dina[45:44]),
        .DIC(dina[47:46]),
        .DID(dina[49:48]),
        .DIE(dina[51:50]),
        .DIF(dina[53:52]),
        .DIG(dina[55:54]),
        .DIH({1'b0,1'b0}),
        .DOA(\gen_rd_b.doutb_reg0 [43:42]),
        .DOB(\gen_rd_b.doutb_reg0 [45:44]),
        .DOC(\gen_rd_b.doutb_reg0 [47:46]),
        .DOD(\gen_rd_b.doutb_reg0 [49:48]),
        .DOE(\gen_rd_b.doutb_reg0 [51:50]),
        .DOF(\gen_rd_b.doutb_reg0 [53:52]),
        .DOG(\gen_rd_b.doutb_reg0 [55:54]),
        .DOH(\NLW_gen_wr_a.gen_word_narrow.mem_reg_0_15_42_55_DOH_UNCONNECTED [1:0]),
        .WCLK(clka),
        .WE(ena));
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

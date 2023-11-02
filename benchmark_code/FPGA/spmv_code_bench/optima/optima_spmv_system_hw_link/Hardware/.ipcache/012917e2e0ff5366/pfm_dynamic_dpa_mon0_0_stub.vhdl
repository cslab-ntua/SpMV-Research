-- Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
-- --------------------------------------------------------------------------------
-- Tool Version: Vivado v.2020.2 (lin64) Build 3064766 Wed Nov 18 09:12:47 MST 2020
-- Date        : Fri Oct 20 18:49:14 2023
-- Host        : gold1 running 64-bit Ubuntu 18.04.6 LTS
-- Command     : write_vhdl -force -mode synth_stub -rename_top decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix -prefix
--               decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_ pfm_dynamic_dpa_mon0_0_stub.vhdl
-- Design      : pfm_dynamic_dpa_mon0_0
-- Purpose     : Stub declaration of top-level module interface
-- Device      : xcu280-fsvh2892-2L-e
-- --------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix is
  Port ( 
    mon_clk : in STD_LOGIC;
    mon_resetn : in STD_LOGIC;
    trace_clk : in STD_LOGIC;
    trace_rst : in STD_LOGIC;
    trace_counter_overflow : in STD_LOGIC;
    trace_counter : in STD_LOGIC_VECTOR ( 44 downto 0 );
    trace_event : out STD_LOGIC;
    trace_data : out STD_LOGIC_VECTOR ( 63 downto 0 );
    trace_valid : out STD_LOGIC;
    trace_read : in STD_LOGIC;
    s_axi_awaddr : in STD_LOGIC_VECTOR ( 31 downto 0 );
    s_axi_awprot : in STD_LOGIC_VECTOR ( 2 downto 0 );
    s_axi_awvalid : in STD_LOGIC;
    s_axi_awready : out STD_LOGIC;
    s_axi_wdata : in STD_LOGIC_VECTOR ( 31 downto 0 );
    s_axi_wstrb : in STD_LOGIC_VECTOR ( 3 downto 0 );
    s_axi_wvalid : in STD_LOGIC;
    s_axi_wready : out STD_LOGIC;
    s_axi_bvalid : out STD_LOGIC;
    s_axi_bready : in STD_LOGIC;
    s_axi_bresp : out STD_LOGIC_VECTOR ( 1 downto 0 );
    s_axi_araddr : in STD_LOGIC_VECTOR ( 31 downto 0 );
    s_axi_arprot : in STD_LOGIC_VECTOR ( 2 downto 0 );
    s_axi_arvalid : in STD_LOGIC;
    s_axi_arready : out STD_LOGIC;
    s_axi_rdata : out STD_LOGIC_VECTOR ( 31 downto 0 );
    s_axi_rresp : out STD_LOGIC_VECTOR ( 1 downto 0 );
    s_axi_rvalid : out STD_LOGIC;
    s_axi_rready : in STD_LOGIC;
    s_axi_awaddr_mon : in STD_LOGIC_VECTOR ( 7 downto 0 );
    s_axi_awprot_mon : in STD_LOGIC_VECTOR ( 2 downto 0 );
    s_axi_awvalid_mon : in STD_LOGIC;
    s_axi_awready_mon : in STD_LOGIC;
    s_axi_wdata_mon : in STD_LOGIC_VECTOR ( 31 downto 0 );
    s_axi_wstrb_mon : in STD_LOGIC_VECTOR ( 3 downto 0 );
    s_axi_wvalid_mon : in STD_LOGIC;
    s_axi_wready_mon : in STD_LOGIC;
    s_axi_bresp_mon : in STD_LOGIC_VECTOR ( 1 downto 0 );
    s_axi_bvalid_mon : in STD_LOGIC;
    s_axi_bready_mon : in STD_LOGIC;
    s_axi_araddr_mon : in STD_LOGIC_VECTOR ( 7 downto 0 );
    s_axi_arprot_mon : in STD_LOGIC_VECTOR ( 2 downto 0 );
    s_axi_arvalid_mon : in STD_LOGIC;
    s_axi_arready_mon : in STD_LOGIC;
    s_axi_rdata_mon : in STD_LOGIC_VECTOR ( 31 downto 0 );
    s_axi_rresp_mon : in STD_LOGIC_VECTOR ( 1 downto 0 );
    s_axi_rvalid_mon : in STD_LOGIC;
    s_axi_rready_mon : in STD_LOGIC
  );

end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix;

architecture stub of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix is
attribute syn_black_box : boolean;
attribute black_box_pad_pin : string;
attribute syn_black_box of stub : architecture is true;
attribute black_box_pad_pin of stub : architecture is "mon_clk,mon_resetn,trace_clk,trace_rst,trace_counter_overflow,trace_counter[44:0],trace_event,trace_data[63:0],trace_valid,trace_read,s_axi_awaddr[31:0],s_axi_awprot[2:0],s_axi_awvalid,s_axi_awready,s_axi_wdata[31:0],s_axi_wstrb[3:0],s_axi_wvalid,s_axi_wready,s_axi_bvalid,s_axi_bready,s_axi_bresp[1:0],s_axi_araddr[31:0],s_axi_arprot[2:0],s_axi_arvalid,s_axi_arready,s_axi_rdata[31:0],s_axi_rresp[1:0],s_axi_rvalid,s_axi_rready,s_axi_awaddr_mon[7:0],s_axi_awprot_mon[2:0],s_axi_awvalid_mon,s_axi_awready_mon,s_axi_wdata_mon[31:0],s_axi_wstrb_mon[3:0],s_axi_wvalid_mon,s_axi_wready_mon,s_axi_bresp_mon[1:0],s_axi_bvalid_mon,s_axi_bready_mon,s_axi_araddr_mon[7:0],s_axi_arprot_mon[2:0],s_axi_arvalid_mon,s_axi_arready_mon,s_axi_rdata_mon[31:0],s_axi_rresp_mon[1:0],s_axi_rvalid_mon,s_axi_rready_mon";
attribute X_CORE_INFO : string;
attribute X_CORE_INFO of stub : architecture is "accelerator_monitor,Vivado 2020.2";
begin
end;

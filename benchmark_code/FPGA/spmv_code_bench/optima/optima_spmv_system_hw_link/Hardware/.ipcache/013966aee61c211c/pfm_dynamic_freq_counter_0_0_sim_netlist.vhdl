-- Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
-- --------------------------------------------------------------------------------
-- Tool Version: Vivado v.2020.2 (lin64) Build 3064766 Wed Nov 18 09:12:47 MST 2020
-- Date        : Fri Oct 20 19:40:59 2023
-- Host        : gold1 running 64-bit Ubuntu 18.04.6 LTS
-- Command     : write_vhdl -force -mode funcsim -rename_top decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix -prefix
--               decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_ pfm_dynamic_freq_counter_0_0_sim_netlist.vhdl
-- Design      : pfm_dynamic_freq_counter_0_0
-- Purpose     : This VHDL netlist is a functional simulation representation of the design and should not be modified or
--               synthesized. This netlist cannot be used for SDF annotated simulation.
-- Device      : xcu280-fsvh2892-2L-e
-- --------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single is
  port (
    src_clk : in STD_LOGIC;
    src_in : in STD_LOGIC_VECTOR ( 0 to 0 );
    dest_clk : in STD_LOGIC;
    dest_out : out STD_LOGIC_VECTOR ( 0 to 0 )
  );
  attribute DEST_SYNC_FF : integer;
  attribute DEST_SYNC_FF of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single : entity is 2;
  attribute INIT_SYNC_FF : integer;
  attribute INIT_SYNC_FF of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single : entity is 0;
  attribute SIM_ASSERT_CHK : integer;
  attribute SIM_ASSERT_CHK of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single : entity is 0;
  attribute SRC_INPUT_REG : integer;
  attribute SRC_INPUT_REG of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single : entity is 0;
  attribute VERSION : integer;
  attribute VERSION of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single : entity is 0;
  attribute WIDTH : integer;
  attribute WIDTH of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single : entity is 1;
  attribute XPM_MODULE : string;
  attribute XPM_MODULE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single : entity is "TRUE";
  attribute keep_hierarchy : string;
  attribute keep_hierarchy of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single : entity is "true";
  attribute xpm_cdc : string;
  attribute xpm_cdc of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single : entity is "ARRAY_SINGLE";
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single is
  signal async_path_bit : STD_LOGIC;
  signal \syncstages_ff[0]\ : STD_LOGIC;
  attribute RTL_KEEP : string;
  attribute RTL_KEEP of \syncstages_ff[0]\ : signal is "true";
  attribute async_reg : string;
  attribute async_reg of \syncstages_ff[0]\ : signal is "true";
  attribute xpm_cdc of \syncstages_ff[0]\ : signal is "ARRAY_SINGLE";
  signal \syncstages_ff[1]\ : STD_LOGIC;
  attribute RTL_KEEP of \syncstages_ff[1]\ : signal is "true";
  attribute async_reg of \syncstages_ff[1]\ : signal is "true";
  attribute xpm_cdc of \syncstages_ff[1]\ : signal is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean : boolean;
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][0]\ : label is std.standard.true;
  attribute KEEP : string;
  attribute KEEP of \syncstages_ff_reg[0][0]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][0]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][0]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][0]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][0]\ : label is "ARRAY_SINGLE";
begin
  async_path_bit <= src_in(0);
  dest_out(0) <= \syncstages_ff[1]\;
\syncstages_ff_reg[0][0]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit,
      Q => \syncstages_ff[0]\,
      R => '0'
    );
\syncstages_ff_reg[1][0]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\,
      Q => \syncstages_ff[1]\,
      R => '0'
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__10\ is
  port (
    src_clk : in STD_LOGIC;
    src_in : in STD_LOGIC_VECTOR ( 0 to 0 );
    dest_clk : in STD_LOGIC;
    dest_out : out STD_LOGIC_VECTOR ( 0 to 0 )
  );
  attribute DEST_SYNC_FF : integer;
  attribute DEST_SYNC_FF of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__10\ : entity is 2;
  attribute INIT_SYNC_FF : integer;
  attribute INIT_SYNC_FF of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__10\ : entity is 0;
  attribute ORIG_REF_NAME : string;
  attribute ORIG_REF_NAME of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__10\ : entity is "xpm_cdc_array_single";
  attribute SIM_ASSERT_CHK : integer;
  attribute SIM_ASSERT_CHK of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__10\ : entity is 0;
  attribute SRC_INPUT_REG : integer;
  attribute SRC_INPUT_REG of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__10\ : entity is 0;
  attribute VERSION : integer;
  attribute VERSION of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__10\ : entity is 0;
  attribute WIDTH : integer;
  attribute WIDTH of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__10\ : entity is 1;
  attribute XPM_MODULE : string;
  attribute XPM_MODULE of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__10\ : entity is "TRUE";
  attribute keep_hierarchy : string;
  attribute keep_hierarchy of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__10\ : entity is "true";
  attribute xpm_cdc : string;
  attribute xpm_cdc of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__10\ : entity is "ARRAY_SINGLE";
end \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__10\;

architecture STRUCTURE of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__10\ is
  signal async_path_bit : STD_LOGIC;
  signal \syncstages_ff[0]\ : STD_LOGIC;
  attribute RTL_KEEP : string;
  attribute RTL_KEEP of \syncstages_ff[0]\ : signal is "true";
  attribute async_reg : string;
  attribute async_reg of \syncstages_ff[0]\ : signal is "true";
  attribute xpm_cdc of \syncstages_ff[0]\ : signal is "ARRAY_SINGLE";
  signal \syncstages_ff[1]\ : STD_LOGIC;
  attribute RTL_KEEP of \syncstages_ff[1]\ : signal is "true";
  attribute async_reg of \syncstages_ff[1]\ : signal is "true";
  attribute xpm_cdc of \syncstages_ff[1]\ : signal is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean : boolean;
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][0]\ : label is std.standard.true;
  attribute KEEP : string;
  attribute KEEP of \syncstages_ff_reg[0][0]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][0]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][0]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][0]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][0]\ : label is "ARRAY_SINGLE";
begin
  async_path_bit <= src_in(0);
  dest_out(0) <= \syncstages_ff[1]\;
\syncstages_ff_reg[0][0]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit,
      Q => \syncstages_ff[0]\,
      R => '0'
    );
\syncstages_ff_reg[1][0]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\,
      Q => \syncstages_ff[1]\,
      R => '0'
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__6\ is
  port (
    src_clk : in STD_LOGIC;
    src_in : in STD_LOGIC_VECTOR ( 0 to 0 );
    dest_clk : in STD_LOGIC;
    dest_out : out STD_LOGIC_VECTOR ( 0 to 0 )
  );
  attribute DEST_SYNC_FF : integer;
  attribute DEST_SYNC_FF of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__6\ : entity is 2;
  attribute INIT_SYNC_FF : integer;
  attribute INIT_SYNC_FF of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__6\ : entity is 0;
  attribute ORIG_REF_NAME : string;
  attribute ORIG_REF_NAME of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__6\ : entity is "xpm_cdc_array_single";
  attribute SIM_ASSERT_CHK : integer;
  attribute SIM_ASSERT_CHK of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__6\ : entity is 0;
  attribute SRC_INPUT_REG : integer;
  attribute SRC_INPUT_REG of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__6\ : entity is 0;
  attribute VERSION : integer;
  attribute VERSION of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__6\ : entity is 0;
  attribute WIDTH : integer;
  attribute WIDTH of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__6\ : entity is 1;
  attribute XPM_MODULE : string;
  attribute XPM_MODULE of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__6\ : entity is "TRUE";
  attribute keep_hierarchy : string;
  attribute keep_hierarchy of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__6\ : entity is "true";
  attribute xpm_cdc : string;
  attribute xpm_cdc of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__6\ : entity is "ARRAY_SINGLE";
end \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__6\;

architecture STRUCTURE of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__6\ is
  signal async_path_bit : STD_LOGIC;
  signal \syncstages_ff[0]\ : STD_LOGIC;
  attribute RTL_KEEP : string;
  attribute RTL_KEEP of \syncstages_ff[0]\ : signal is "true";
  attribute async_reg : string;
  attribute async_reg of \syncstages_ff[0]\ : signal is "true";
  attribute xpm_cdc of \syncstages_ff[0]\ : signal is "ARRAY_SINGLE";
  signal \syncstages_ff[1]\ : STD_LOGIC;
  attribute RTL_KEEP of \syncstages_ff[1]\ : signal is "true";
  attribute async_reg of \syncstages_ff[1]\ : signal is "true";
  attribute xpm_cdc of \syncstages_ff[1]\ : signal is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean : boolean;
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][0]\ : label is std.standard.true;
  attribute KEEP : string;
  attribute KEEP of \syncstages_ff_reg[0][0]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][0]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][0]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][0]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][0]\ : label is "ARRAY_SINGLE";
begin
  async_path_bit <= src_in(0);
  dest_out(0) <= \syncstages_ff[1]\;
\syncstages_ff_reg[0][0]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit,
      Q => \syncstages_ff[0]\,
      R => '0'
    );
\syncstages_ff_reg[1][0]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\,
      Q => \syncstages_ff[1]\,
      R => '0'
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__7\ is
  port (
    src_clk : in STD_LOGIC;
    src_in : in STD_LOGIC_VECTOR ( 0 to 0 );
    dest_clk : in STD_LOGIC;
    dest_out : out STD_LOGIC_VECTOR ( 0 to 0 )
  );
  attribute DEST_SYNC_FF : integer;
  attribute DEST_SYNC_FF of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__7\ : entity is 2;
  attribute INIT_SYNC_FF : integer;
  attribute INIT_SYNC_FF of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__7\ : entity is 0;
  attribute ORIG_REF_NAME : string;
  attribute ORIG_REF_NAME of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__7\ : entity is "xpm_cdc_array_single";
  attribute SIM_ASSERT_CHK : integer;
  attribute SIM_ASSERT_CHK of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__7\ : entity is 0;
  attribute SRC_INPUT_REG : integer;
  attribute SRC_INPUT_REG of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__7\ : entity is 0;
  attribute VERSION : integer;
  attribute VERSION of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__7\ : entity is 0;
  attribute WIDTH : integer;
  attribute WIDTH of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__7\ : entity is 1;
  attribute XPM_MODULE : string;
  attribute XPM_MODULE of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__7\ : entity is "TRUE";
  attribute keep_hierarchy : string;
  attribute keep_hierarchy of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__7\ : entity is "true";
  attribute xpm_cdc : string;
  attribute xpm_cdc of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__7\ : entity is "ARRAY_SINGLE";
end \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__7\;

architecture STRUCTURE of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__7\ is
  signal async_path_bit : STD_LOGIC;
  signal \syncstages_ff[0]\ : STD_LOGIC;
  attribute RTL_KEEP : string;
  attribute RTL_KEEP of \syncstages_ff[0]\ : signal is "true";
  attribute async_reg : string;
  attribute async_reg of \syncstages_ff[0]\ : signal is "true";
  attribute xpm_cdc of \syncstages_ff[0]\ : signal is "ARRAY_SINGLE";
  signal \syncstages_ff[1]\ : STD_LOGIC;
  attribute RTL_KEEP of \syncstages_ff[1]\ : signal is "true";
  attribute async_reg of \syncstages_ff[1]\ : signal is "true";
  attribute xpm_cdc of \syncstages_ff[1]\ : signal is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean : boolean;
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][0]\ : label is std.standard.true;
  attribute KEEP : string;
  attribute KEEP of \syncstages_ff_reg[0][0]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][0]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][0]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][0]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][0]\ : label is "ARRAY_SINGLE";
begin
  async_path_bit <= src_in(0);
  dest_out(0) <= \syncstages_ff[1]\;
\syncstages_ff_reg[0][0]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit,
      Q => \syncstages_ff[0]\,
      R => '0'
    );
\syncstages_ff_reg[1][0]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\,
      Q => \syncstages_ff[1]\,
      R => '0'
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__8\ is
  port (
    src_clk : in STD_LOGIC;
    src_in : in STD_LOGIC_VECTOR ( 0 to 0 );
    dest_clk : in STD_LOGIC;
    dest_out : out STD_LOGIC_VECTOR ( 0 to 0 )
  );
  attribute DEST_SYNC_FF : integer;
  attribute DEST_SYNC_FF of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__8\ : entity is 2;
  attribute INIT_SYNC_FF : integer;
  attribute INIT_SYNC_FF of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__8\ : entity is 0;
  attribute ORIG_REF_NAME : string;
  attribute ORIG_REF_NAME of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__8\ : entity is "xpm_cdc_array_single";
  attribute SIM_ASSERT_CHK : integer;
  attribute SIM_ASSERT_CHK of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__8\ : entity is 0;
  attribute SRC_INPUT_REG : integer;
  attribute SRC_INPUT_REG of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__8\ : entity is 0;
  attribute VERSION : integer;
  attribute VERSION of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__8\ : entity is 0;
  attribute WIDTH : integer;
  attribute WIDTH of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__8\ : entity is 1;
  attribute XPM_MODULE : string;
  attribute XPM_MODULE of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__8\ : entity is "TRUE";
  attribute keep_hierarchy : string;
  attribute keep_hierarchy of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__8\ : entity is "true";
  attribute xpm_cdc : string;
  attribute xpm_cdc of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__8\ : entity is "ARRAY_SINGLE";
end \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__8\;

architecture STRUCTURE of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__8\ is
  signal async_path_bit : STD_LOGIC;
  signal \syncstages_ff[0]\ : STD_LOGIC;
  attribute RTL_KEEP : string;
  attribute RTL_KEEP of \syncstages_ff[0]\ : signal is "true";
  attribute async_reg : string;
  attribute async_reg of \syncstages_ff[0]\ : signal is "true";
  attribute xpm_cdc of \syncstages_ff[0]\ : signal is "ARRAY_SINGLE";
  signal \syncstages_ff[1]\ : STD_LOGIC;
  attribute RTL_KEEP of \syncstages_ff[1]\ : signal is "true";
  attribute async_reg of \syncstages_ff[1]\ : signal is "true";
  attribute xpm_cdc of \syncstages_ff[1]\ : signal is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean : boolean;
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][0]\ : label is std.standard.true;
  attribute KEEP : string;
  attribute KEEP of \syncstages_ff_reg[0][0]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][0]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][0]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][0]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][0]\ : label is "ARRAY_SINGLE";
begin
  async_path_bit <= src_in(0);
  dest_out(0) <= \syncstages_ff[1]\;
\syncstages_ff_reg[0][0]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit,
      Q => \syncstages_ff[0]\,
      R => '0'
    );
\syncstages_ff_reg[1][0]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\,
      Q => \syncstages_ff[1]\,
      R => '0'
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__9\ is
  port (
    src_clk : in STD_LOGIC;
    src_in : in STD_LOGIC_VECTOR ( 0 to 0 );
    dest_clk : in STD_LOGIC;
    dest_out : out STD_LOGIC_VECTOR ( 0 to 0 )
  );
  attribute DEST_SYNC_FF : integer;
  attribute DEST_SYNC_FF of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__9\ : entity is 2;
  attribute INIT_SYNC_FF : integer;
  attribute INIT_SYNC_FF of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__9\ : entity is 0;
  attribute ORIG_REF_NAME : string;
  attribute ORIG_REF_NAME of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__9\ : entity is "xpm_cdc_array_single";
  attribute SIM_ASSERT_CHK : integer;
  attribute SIM_ASSERT_CHK of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__9\ : entity is 0;
  attribute SRC_INPUT_REG : integer;
  attribute SRC_INPUT_REG of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__9\ : entity is 0;
  attribute VERSION : integer;
  attribute VERSION of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__9\ : entity is 0;
  attribute WIDTH : integer;
  attribute WIDTH of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__9\ : entity is 1;
  attribute XPM_MODULE : string;
  attribute XPM_MODULE of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__9\ : entity is "TRUE";
  attribute keep_hierarchy : string;
  attribute keep_hierarchy of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__9\ : entity is "true";
  attribute xpm_cdc : string;
  attribute xpm_cdc of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__9\ : entity is "ARRAY_SINGLE";
end \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__9\;

architecture STRUCTURE of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__9\ is
  signal async_path_bit : STD_LOGIC;
  signal \syncstages_ff[0]\ : STD_LOGIC;
  attribute RTL_KEEP : string;
  attribute RTL_KEEP of \syncstages_ff[0]\ : signal is "true";
  attribute async_reg : string;
  attribute async_reg of \syncstages_ff[0]\ : signal is "true";
  attribute xpm_cdc of \syncstages_ff[0]\ : signal is "ARRAY_SINGLE";
  signal \syncstages_ff[1]\ : STD_LOGIC;
  attribute RTL_KEEP of \syncstages_ff[1]\ : signal is "true";
  attribute async_reg of \syncstages_ff[1]\ : signal is "true";
  attribute xpm_cdc of \syncstages_ff[1]\ : signal is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean : boolean;
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][0]\ : label is std.standard.true;
  attribute KEEP : string;
  attribute KEEP of \syncstages_ff_reg[0][0]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][0]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][0]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][0]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][0]\ : label is "ARRAY_SINGLE";
begin
  async_path_bit <= src_in(0);
  dest_out(0) <= \syncstages_ff[1]\;
\syncstages_ff_reg[0][0]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit,
      Q => \syncstages_ff[0]\,
      R => '0'
    );
\syncstages_ff_reg[1][0]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\,
      Q => \syncstages_ff[1]\,
      R => '0'
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0\ is
  port (
    src_clk : in STD_LOGIC;
    src_in : in STD_LOGIC_VECTOR ( 31 downto 0 );
    dest_clk : in STD_LOGIC;
    dest_out : out STD_LOGIC_VECTOR ( 31 downto 0 )
  );
  attribute DEST_SYNC_FF : integer;
  attribute DEST_SYNC_FF of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0\ : entity is 2;
  attribute INIT_SYNC_FF : integer;
  attribute INIT_SYNC_FF of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0\ : entity is 0;
  attribute ORIG_REF_NAME : string;
  attribute ORIG_REF_NAME of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0\ : entity is "xpm_cdc_array_single";
  attribute SIM_ASSERT_CHK : integer;
  attribute SIM_ASSERT_CHK of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0\ : entity is 0;
  attribute SRC_INPUT_REG : integer;
  attribute SRC_INPUT_REG of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0\ : entity is 0;
  attribute VERSION : integer;
  attribute VERSION of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0\ : entity is 0;
  attribute WIDTH : integer;
  attribute WIDTH of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0\ : entity is 32;
  attribute XPM_MODULE : string;
  attribute XPM_MODULE of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0\ : entity is "TRUE";
  attribute keep_hierarchy : string;
  attribute keep_hierarchy of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0\ : entity is "true";
  attribute xpm_cdc : string;
  attribute xpm_cdc of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0\ : entity is "ARRAY_SINGLE";
end \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0\;

architecture STRUCTURE of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0\ is
  signal async_path_bit : STD_LOGIC_VECTOR ( 31 downto 0 );
  signal \syncstages_ff[0]\ : STD_LOGIC_VECTOR ( 31 downto 0 );
  attribute RTL_KEEP : string;
  attribute RTL_KEEP of \syncstages_ff[0]\ : signal is "true";
  attribute async_reg : string;
  attribute async_reg of \syncstages_ff[0]\ : signal is "true";
  attribute xpm_cdc of \syncstages_ff[0]\ : signal is "ARRAY_SINGLE";
  signal \syncstages_ff[1]\ : STD_LOGIC_VECTOR ( 31 downto 0 );
  attribute RTL_KEEP of \syncstages_ff[1]\ : signal is "true";
  attribute async_reg of \syncstages_ff[1]\ : signal is "true";
  attribute xpm_cdc of \syncstages_ff[1]\ : signal is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean : boolean;
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][0]\ : label is std.standard.true;
  attribute KEEP : string;
  attribute KEEP of \syncstages_ff_reg[0][0]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][0]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][10]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][10]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][10]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][11]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][11]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][11]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][12]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][12]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][12]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][13]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][13]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][13]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][14]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][14]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][14]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][15]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][15]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][15]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][16]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][16]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][16]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][17]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][17]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][17]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][18]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][18]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][18]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][19]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][19]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][19]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][1]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][1]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][1]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][20]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][20]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][20]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][21]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][21]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][21]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][22]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][22]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][22]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][23]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][23]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][23]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][24]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][24]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][24]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][25]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][25]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][25]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][26]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][26]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][26]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][27]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][27]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][27]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][28]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][28]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][28]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][29]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][29]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][29]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][2]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][2]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][2]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][30]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][30]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][30]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][31]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][31]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][31]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][3]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][3]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][3]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][4]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][4]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][4]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][5]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][5]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][5]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][6]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][6]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][6]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][7]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][7]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][7]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][8]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][8]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][8]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][9]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][9]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][9]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][0]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][0]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][0]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][10]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][10]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][10]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][11]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][11]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][11]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][12]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][12]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][12]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][13]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][13]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][13]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][14]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][14]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][14]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][15]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][15]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][15]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][16]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][16]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][16]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][17]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][17]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][17]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][18]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][18]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][18]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][19]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][19]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][19]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][1]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][1]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][1]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][20]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][20]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][20]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][21]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][21]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][21]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][22]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][22]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][22]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][23]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][23]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][23]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][24]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][24]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][24]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][25]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][25]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][25]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][26]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][26]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][26]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][27]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][27]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][27]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][28]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][28]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][28]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][29]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][29]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][29]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][2]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][2]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][2]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][30]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][30]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][30]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][31]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][31]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][31]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][3]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][3]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][3]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][4]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][4]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][4]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][5]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][5]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][5]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][6]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][6]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][6]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][7]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][7]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][7]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][8]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][8]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][8]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][9]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][9]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][9]\ : label is "ARRAY_SINGLE";
begin
  async_path_bit(31 downto 0) <= src_in(31 downto 0);
  dest_out(31 downto 0) <= \syncstages_ff[1]\(31 downto 0);
\syncstages_ff_reg[0][0]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(0),
      Q => \syncstages_ff[0]\(0),
      R => '0'
    );
\syncstages_ff_reg[0][10]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(10),
      Q => \syncstages_ff[0]\(10),
      R => '0'
    );
\syncstages_ff_reg[0][11]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(11),
      Q => \syncstages_ff[0]\(11),
      R => '0'
    );
\syncstages_ff_reg[0][12]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(12),
      Q => \syncstages_ff[0]\(12),
      R => '0'
    );
\syncstages_ff_reg[0][13]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(13),
      Q => \syncstages_ff[0]\(13),
      R => '0'
    );
\syncstages_ff_reg[0][14]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(14),
      Q => \syncstages_ff[0]\(14),
      R => '0'
    );
\syncstages_ff_reg[0][15]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(15),
      Q => \syncstages_ff[0]\(15),
      R => '0'
    );
\syncstages_ff_reg[0][16]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(16),
      Q => \syncstages_ff[0]\(16),
      R => '0'
    );
\syncstages_ff_reg[0][17]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(17),
      Q => \syncstages_ff[0]\(17),
      R => '0'
    );
\syncstages_ff_reg[0][18]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(18),
      Q => \syncstages_ff[0]\(18),
      R => '0'
    );
\syncstages_ff_reg[0][19]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(19),
      Q => \syncstages_ff[0]\(19),
      R => '0'
    );
\syncstages_ff_reg[0][1]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(1),
      Q => \syncstages_ff[0]\(1),
      R => '0'
    );
\syncstages_ff_reg[0][20]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(20),
      Q => \syncstages_ff[0]\(20),
      R => '0'
    );
\syncstages_ff_reg[0][21]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(21),
      Q => \syncstages_ff[0]\(21),
      R => '0'
    );
\syncstages_ff_reg[0][22]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(22),
      Q => \syncstages_ff[0]\(22),
      R => '0'
    );
\syncstages_ff_reg[0][23]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(23),
      Q => \syncstages_ff[0]\(23),
      R => '0'
    );
\syncstages_ff_reg[0][24]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(24),
      Q => \syncstages_ff[0]\(24),
      R => '0'
    );
\syncstages_ff_reg[0][25]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(25),
      Q => \syncstages_ff[0]\(25),
      R => '0'
    );
\syncstages_ff_reg[0][26]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(26),
      Q => \syncstages_ff[0]\(26),
      R => '0'
    );
\syncstages_ff_reg[0][27]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(27),
      Q => \syncstages_ff[0]\(27),
      R => '0'
    );
\syncstages_ff_reg[0][28]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(28),
      Q => \syncstages_ff[0]\(28),
      R => '0'
    );
\syncstages_ff_reg[0][29]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(29),
      Q => \syncstages_ff[0]\(29),
      R => '0'
    );
\syncstages_ff_reg[0][2]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(2),
      Q => \syncstages_ff[0]\(2),
      R => '0'
    );
\syncstages_ff_reg[0][30]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(30),
      Q => \syncstages_ff[0]\(30),
      R => '0'
    );
\syncstages_ff_reg[0][31]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(31),
      Q => \syncstages_ff[0]\(31),
      R => '0'
    );
\syncstages_ff_reg[0][3]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(3),
      Q => \syncstages_ff[0]\(3),
      R => '0'
    );
\syncstages_ff_reg[0][4]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(4),
      Q => \syncstages_ff[0]\(4),
      R => '0'
    );
\syncstages_ff_reg[0][5]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(5),
      Q => \syncstages_ff[0]\(5),
      R => '0'
    );
\syncstages_ff_reg[0][6]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(6),
      Q => \syncstages_ff[0]\(6),
      R => '0'
    );
\syncstages_ff_reg[0][7]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(7),
      Q => \syncstages_ff[0]\(7),
      R => '0'
    );
\syncstages_ff_reg[0][8]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(8),
      Q => \syncstages_ff[0]\(8),
      R => '0'
    );
\syncstages_ff_reg[0][9]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(9),
      Q => \syncstages_ff[0]\(9),
      R => '0'
    );
\syncstages_ff_reg[1][0]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(0),
      Q => \syncstages_ff[1]\(0),
      R => '0'
    );
\syncstages_ff_reg[1][10]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(10),
      Q => \syncstages_ff[1]\(10),
      R => '0'
    );
\syncstages_ff_reg[1][11]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(11),
      Q => \syncstages_ff[1]\(11),
      R => '0'
    );
\syncstages_ff_reg[1][12]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(12),
      Q => \syncstages_ff[1]\(12),
      R => '0'
    );
\syncstages_ff_reg[1][13]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(13),
      Q => \syncstages_ff[1]\(13),
      R => '0'
    );
\syncstages_ff_reg[1][14]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(14),
      Q => \syncstages_ff[1]\(14),
      R => '0'
    );
\syncstages_ff_reg[1][15]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(15),
      Q => \syncstages_ff[1]\(15),
      R => '0'
    );
\syncstages_ff_reg[1][16]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(16),
      Q => \syncstages_ff[1]\(16),
      R => '0'
    );
\syncstages_ff_reg[1][17]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(17),
      Q => \syncstages_ff[1]\(17),
      R => '0'
    );
\syncstages_ff_reg[1][18]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(18),
      Q => \syncstages_ff[1]\(18),
      R => '0'
    );
\syncstages_ff_reg[1][19]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(19),
      Q => \syncstages_ff[1]\(19),
      R => '0'
    );
\syncstages_ff_reg[1][1]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(1),
      Q => \syncstages_ff[1]\(1),
      R => '0'
    );
\syncstages_ff_reg[1][20]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(20),
      Q => \syncstages_ff[1]\(20),
      R => '0'
    );
\syncstages_ff_reg[1][21]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(21),
      Q => \syncstages_ff[1]\(21),
      R => '0'
    );
\syncstages_ff_reg[1][22]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(22),
      Q => \syncstages_ff[1]\(22),
      R => '0'
    );
\syncstages_ff_reg[1][23]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(23),
      Q => \syncstages_ff[1]\(23),
      R => '0'
    );
\syncstages_ff_reg[1][24]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(24),
      Q => \syncstages_ff[1]\(24),
      R => '0'
    );
\syncstages_ff_reg[1][25]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(25),
      Q => \syncstages_ff[1]\(25),
      R => '0'
    );
\syncstages_ff_reg[1][26]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(26),
      Q => \syncstages_ff[1]\(26),
      R => '0'
    );
\syncstages_ff_reg[1][27]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(27),
      Q => \syncstages_ff[1]\(27),
      R => '0'
    );
\syncstages_ff_reg[1][28]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(28),
      Q => \syncstages_ff[1]\(28),
      R => '0'
    );
\syncstages_ff_reg[1][29]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(29),
      Q => \syncstages_ff[1]\(29),
      R => '0'
    );
\syncstages_ff_reg[1][2]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(2),
      Q => \syncstages_ff[1]\(2),
      R => '0'
    );
\syncstages_ff_reg[1][30]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(30),
      Q => \syncstages_ff[1]\(30),
      R => '0'
    );
\syncstages_ff_reg[1][31]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(31),
      Q => \syncstages_ff[1]\(31),
      R => '0'
    );
\syncstages_ff_reg[1][3]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(3),
      Q => \syncstages_ff[1]\(3),
      R => '0'
    );
\syncstages_ff_reg[1][4]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(4),
      Q => \syncstages_ff[1]\(4),
      R => '0'
    );
\syncstages_ff_reg[1][5]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(5),
      Q => \syncstages_ff[1]\(5),
      R => '0'
    );
\syncstages_ff_reg[1][6]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(6),
      Q => \syncstages_ff[1]\(6),
      R => '0'
    );
\syncstages_ff_reg[1][7]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(7),
      Q => \syncstages_ff[1]\(7),
      R => '0'
    );
\syncstages_ff_reg[1][8]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(8),
      Q => \syncstages_ff[1]\(8),
      R => '0'
    );
\syncstages_ff_reg[1][9]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(9),
      Q => \syncstages_ff[1]\(9),
      R => '0'
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0__2\ is
  port (
    src_clk : in STD_LOGIC;
    src_in : in STD_LOGIC_VECTOR ( 31 downto 0 );
    dest_clk : in STD_LOGIC;
    dest_out : out STD_LOGIC_VECTOR ( 31 downto 0 )
  );
  attribute DEST_SYNC_FF : integer;
  attribute DEST_SYNC_FF of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0__2\ : entity is 2;
  attribute INIT_SYNC_FF : integer;
  attribute INIT_SYNC_FF of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0__2\ : entity is 0;
  attribute ORIG_REF_NAME : string;
  attribute ORIG_REF_NAME of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0__2\ : entity is "xpm_cdc_array_single";
  attribute SIM_ASSERT_CHK : integer;
  attribute SIM_ASSERT_CHK of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0__2\ : entity is 0;
  attribute SRC_INPUT_REG : integer;
  attribute SRC_INPUT_REG of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0__2\ : entity is 0;
  attribute VERSION : integer;
  attribute VERSION of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0__2\ : entity is 0;
  attribute WIDTH : integer;
  attribute WIDTH of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0__2\ : entity is 32;
  attribute XPM_MODULE : string;
  attribute XPM_MODULE of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0__2\ : entity is "TRUE";
  attribute keep_hierarchy : string;
  attribute keep_hierarchy of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0__2\ : entity is "true";
  attribute xpm_cdc : string;
  attribute xpm_cdc of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0__2\ : entity is "ARRAY_SINGLE";
end \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0__2\;

architecture STRUCTURE of \decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0__2\ is
  signal async_path_bit : STD_LOGIC_VECTOR ( 31 downto 0 );
  signal \syncstages_ff[0]\ : STD_LOGIC_VECTOR ( 31 downto 0 );
  attribute RTL_KEEP : string;
  attribute RTL_KEEP of \syncstages_ff[0]\ : signal is "true";
  attribute async_reg : string;
  attribute async_reg of \syncstages_ff[0]\ : signal is "true";
  attribute xpm_cdc of \syncstages_ff[0]\ : signal is "ARRAY_SINGLE";
  signal \syncstages_ff[1]\ : STD_LOGIC_VECTOR ( 31 downto 0 );
  attribute RTL_KEEP of \syncstages_ff[1]\ : signal is "true";
  attribute async_reg of \syncstages_ff[1]\ : signal is "true";
  attribute xpm_cdc of \syncstages_ff[1]\ : signal is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean : boolean;
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][0]\ : label is std.standard.true;
  attribute KEEP : string;
  attribute KEEP of \syncstages_ff_reg[0][0]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][0]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][10]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][10]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][10]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][11]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][11]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][11]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][12]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][12]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][12]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][13]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][13]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][13]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][14]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][14]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][14]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][15]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][15]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][15]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][16]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][16]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][16]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][17]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][17]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][17]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][18]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][18]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][18]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][19]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][19]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][19]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][1]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][1]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][1]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][20]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][20]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][20]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][21]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][21]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][21]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][22]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][22]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][22]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][23]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][23]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][23]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][24]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][24]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][24]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][25]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][25]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][25]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][26]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][26]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][26]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][27]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][27]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][27]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][28]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][28]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][28]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][29]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][29]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][29]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][2]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][2]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][2]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][30]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][30]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][30]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][31]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][31]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][31]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][3]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][3]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][3]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][4]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][4]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][4]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][5]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][5]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][5]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][6]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][6]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][6]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][7]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][7]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][7]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][8]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][8]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][8]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[0][9]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[0][9]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[0][9]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][0]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][0]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][0]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][10]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][10]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][10]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][11]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][11]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][11]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][12]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][12]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][12]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][13]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][13]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][13]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][14]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][14]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][14]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][15]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][15]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][15]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][16]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][16]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][16]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][17]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][17]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][17]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][18]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][18]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][18]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][19]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][19]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][19]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][1]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][1]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][1]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][20]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][20]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][20]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][21]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][21]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][21]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][22]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][22]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][22]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][23]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][23]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][23]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][24]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][24]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][24]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][25]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][25]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][25]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][26]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][26]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][26]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][27]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][27]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][27]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][28]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][28]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][28]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][29]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][29]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][29]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][2]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][2]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][2]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][30]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][30]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][30]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][31]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][31]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][31]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][3]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][3]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][3]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][4]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][4]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][4]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][5]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][5]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][5]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][6]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][6]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][6]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][7]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][7]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][7]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][8]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][8]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][8]\ : label is "ARRAY_SINGLE";
  attribute ASYNC_REG_boolean of \syncstages_ff_reg[1][9]\ : label is std.standard.true;
  attribute KEEP of \syncstages_ff_reg[1][9]\ : label is "true";
  attribute XPM_CDC of \syncstages_ff_reg[1][9]\ : label is "ARRAY_SINGLE";
begin
  async_path_bit(31 downto 0) <= src_in(31 downto 0);
  dest_out(31 downto 0) <= \syncstages_ff[1]\(31 downto 0);
\syncstages_ff_reg[0][0]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(0),
      Q => \syncstages_ff[0]\(0),
      R => '0'
    );
\syncstages_ff_reg[0][10]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(10),
      Q => \syncstages_ff[0]\(10),
      R => '0'
    );
\syncstages_ff_reg[0][11]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(11),
      Q => \syncstages_ff[0]\(11),
      R => '0'
    );
\syncstages_ff_reg[0][12]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(12),
      Q => \syncstages_ff[0]\(12),
      R => '0'
    );
\syncstages_ff_reg[0][13]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(13),
      Q => \syncstages_ff[0]\(13),
      R => '0'
    );
\syncstages_ff_reg[0][14]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(14),
      Q => \syncstages_ff[0]\(14),
      R => '0'
    );
\syncstages_ff_reg[0][15]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(15),
      Q => \syncstages_ff[0]\(15),
      R => '0'
    );
\syncstages_ff_reg[0][16]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(16),
      Q => \syncstages_ff[0]\(16),
      R => '0'
    );
\syncstages_ff_reg[0][17]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(17),
      Q => \syncstages_ff[0]\(17),
      R => '0'
    );
\syncstages_ff_reg[0][18]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(18),
      Q => \syncstages_ff[0]\(18),
      R => '0'
    );
\syncstages_ff_reg[0][19]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(19),
      Q => \syncstages_ff[0]\(19),
      R => '0'
    );
\syncstages_ff_reg[0][1]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(1),
      Q => \syncstages_ff[0]\(1),
      R => '0'
    );
\syncstages_ff_reg[0][20]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(20),
      Q => \syncstages_ff[0]\(20),
      R => '0'
    );
\syncstages_ff_reg[0][21]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(21),
      Q => \syncstages_ff[0]\(21),
      R => '0'
    );
\syncstages_ff_reg[0][22]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(22),
      Q => \syncstages_ff[0]\(22),
      R => '0'
    );
\syncstages_ff_reg[0][23]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(23),
      Q => \syncstages_ff[0]\(23),
      R => '0'
    );
\syncstages_ff_reg[0][24]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(24),
      Q => \syncstages_ff[0]\(24),
      R => '0'
    );
\syncstages_ff_reg[0][25]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(25),
      Q => \syncstages_ff[0]\(25),
      R => '0'
    );
\syncstages_ff_reg[0][26]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(26),
      Q => \syncstages_ff[0]\(26),
      R => '0'
    );
\syncstages_ff_reg[0][27]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(27),
      Q => \syncstages_ff[0]\(27),
      R => '0'
    );
\syncstages_ff_reg[0][28]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(28),
      Q => \syncstages_ff[0]\(28),
      R => '0'
    );
\syncstages_ff_reg[0][29]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(29),
      Q => \syncstages_ff[0]\(29),
      R => '0'
    );
\syncstages_ff_reg[0][2]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(2),
      Q => \syncstages_ff[0]\(2),
      R => '0'
    );
\syncstages_ff_reg[0][30]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(30),
      Q => \syncstages_ff[0]\(30),
      R => '0'
    );
\syncstages_ff_reg[0][31]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(31),
      Q => \syncstages_ff[0]\(31),
      R => '0'
    );
\syncstages_ff_reg[0][3]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(3),
      Q => \syncstages_ff[0]\(3),
      R => '0'
    );
\syncstages_ff_reg[0][4]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(4),
      Q => \syncstages_ff[0]\(4),
      R => '0'
    );
\syncstages_ff_reg[0][5]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(5),
      Q => \syncstages_ff[0]\(5),
      R => '0'
    );
\syncstages_ff_reg[0][6]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(6),
      Q => \syncstages_ff[0]\(6),
      R => '0'
    );
\syncstages_ff_reg[0][7]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(7),
      Q => \syncstages_ff[0]\(7),
      R => '0'
    );
\syncstages_ff_reg[0][8]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(8),
      Q => \syncstages_ff[0]\(8),
      R => '0'
    );
\syncstages_ff_reg[0][9]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => async_path_bit(9),
      Q => \syncstages_ff[0]\(9),
      R => '0'
    );
\syncstages_ff_reg[1][0]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(0),
      Q => \syncstages_ff[1]\(0),
      R => '0'
    );
\syncstages_ff_reg[1][10]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(10),
      Q => \syncstages_ff[1]\(10),
      R => '0'
    );
\syncstages_ff_reg[1][11]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(11),
      Q => \syncstages_ff[1]\(11),
      R => '0'
    );
\syncstages_ff_reg[1][12]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(12),
      Q => \syncstages_ff[1]\(12),
      R => '0'
    );
\syncstages_ff_reg[1][13]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(13),
      Q => \syncstages_ff[1]\(13),
      R => '0'
    );
\syncstages_ff_reg[1][14]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(14),
      Q => \syncstages_ff[1]\(14),
      R => '0'
    );
\syncstages_ff_reg[1][15]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(15),
      Q => \syncstages_ff[1]\(15),
      R => '0'
    );
\syncstages_ff_reg[1][16]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(16),
      Q => \syncstages_ff[1]\(16),
      R => '0'
    );
\syncstages_ff_reg[1][17]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(17),
      Q => \syncstages_ff[1]\(17),
      R => '0'
    );
\syncstages_ff_reg[1][18]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(18),
      Q => \syncstages_ff[1]\(18),
      R => '0'
    );
\syncstages_ff_reg[1][19]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(19),
      Q => \syncstages_ff[1]\(19),
      R => '0'
    );
\syncstages_ff_reg[1][1]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(1),
      Q => \syncstages_ff[1]\(1),
      R => '0'
    );
\syncstages_ff_reg[1][20]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(20),
      Q => \syncstages_ff[1]\(20),
      R => '0'
    );
\syncstages_ff_reg[1][21]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(21),
      Q => \syncstages_ff[1]\(21),
      R => '0'
    );
\syncstages_ff_reg[1][22]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(22),
      Q => \syncstages_ff[1]\(22),
      R => '0'
    );
\syncstages_ff_reg[1][23]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(23),
      Q => \syncstages_ff[1]\(23),
      R => '0'
    );
\syncstages_ff_reg[1][24]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(24),
      Q => \syncstages_ff[1]\(24),
      R => '0'
    );
\syncstages_ff_reg[1][25]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(25),
      Q => \syncstages_ff[1]\(25),
      R => '0'
    );
\syncstages_ff_reg[1][26]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(26),
      Q => \syncstages_ff[1]\(26),
      R => '0'
    );
\syncstages_ff_reg[1][27]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(27),
      Q => \syncstages_ff[1]\(27),
      R => '0'
    );
\syncstages_ff_reg[1][28]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(28),
      Q => \syncstages_ff[1]\(28),
      R => '0'
    );
\syncstages_ff_reg[1][29]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(29),
      Q => \syncstages_ff[1]\(29),
      R => '0'
    );
\syncstages_ff_reg[1][2]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(2),
      Q => \syncstages_ff[1]\(2),
      R => '0'
    );
\syncstages_ff_reg[1][30]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(30),
      Q => \syncstages_ff[1]\(30),
      R => '0'
    );
\syncstages_ff_reg[1][31]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(31),
      Q => \syncstages_ff[1]\(31),
      R => '0'
    );
\syncstages_ff_reg[1][3]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(3),
      Q => \syncstages_ff[1]\(3),
      R => '0'
    );
\syncstages_ff_reg[1][4]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(4),
      Q => \syncstages_ff[1]\(4),
      R => '0'
    );
\syncstages_ff_reg[1][5]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(5),
      Q => \syncstages_ff[1]\(5),
      R => '0'
    );
\syncstages_ff_reg[1][6]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(6),
      Q => \syncstages_ff[1]\(6),
      R => '0'
    );
\syncstages_ff_reg[1][7]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(7),
      Q => \syncstages_ff[1]\(7),
      R => '0'
    );
\syncstages_ff_reg[1][8]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(8),
      Q => \syncstages_ff[1]\(8),
      R => '0'
    );
\syncstages_ff_reg[1][9]\: unisim.vcomponents.FDRE
     port map (
      C => dest_clk,
      CE => '1',
      D => \syncstages_ff[0]\(9),
      Q => \syncstages_ff[1]\(9),
      R => '0'
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_freq_counter is
  port (
    axil_rdata : out STD_LOGIC_VECTOR ( 31 downto 0 );
    axil_awready : out STD_LOGIC;
    axil_wready : out STD_LOGIC;
    axil_bvalid : out STD_LOGIC;
    axil_arready : out STD_LOGIC;
    axil_rvalid : out STD_LOGIC;
    test_clk0 : in STD_LOGIC;
    test_clk1 : in STD_LOGIC;
    clk : in STD_LOGIC;
    axil_arvalid : in STD_LOGIC;
    axil_araddr : in STD_LOGIC_VECTOR ( 3 downto 0 );
    axil_bready : in STD_LOGIC;
    axil_wvalid : in STD_LOGIC;
    axil_awvalid : in STD_LOGIC;
    reset_n : in STD_LOGIC;
    axil_rready : in STD_LOGIC;
    axil_wdata : in STD_LOGIC_VECTOR ( 30 downto 0 );
    axil_awaddr : in STD_LOGIC_VECTOR ( 3 downto 0 )
  );
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_freq_counter;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_freq_counter is
  signal \^axil_arready\ : STD_LOGIC;
  signal axil_arready_i_1_n_0 : STD_LOGIC;
  signal \^axil_awready\ : STD_LOGIC;
  signal axil_awready_i_1_n_0 : STD_LOGIC;
  signal \^axil_bvalid\ : STD_LOGIC;
  signal axil_bvalid_i_1_n_0 : STD_LOGIC;
  signal \axil_rdata[0]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[0]_i_3_n_0\ : STD_LOGIC;
  signal \axil_rdata[10]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[11]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[12]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[13]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[14]_i_1_n_0\ : STD_LOGIC;
  signal \axil_rdata[14]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[15]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[16]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[17]_i_1_n_0\ : STD_LOGIC;
  signal \axil_rdata[17]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[18]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[19]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[1]_i_10_n_0\ : STD_LOGIC;
  signal \axil_rdata[1]_i_11_n_0\ : STD_LOGIC;
  signal \axil_rdata[1]_i_12_n_0\ : STD_LOGIC;
  signal \axil_rdata[1]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[1]_i_3_n_0\ : STD_LOGIC;
  signal \axil_rdata[1]_i_4_n_0\ : STD_LOGIC;
  signal \axil_rdata[1]_i_5_n_0\ : STD_LOGIC;
  signal \axil_rdata[1]_i_6_n_0\ : STD_LOGIC;
  signal \axil_rdata[1]_i_7_n_0\ : STD_LOGIC;
  signal \axil_rdata[1]_i_8_n_0\ : STD_LOGIC;
  signal \axil_rdata[1]_i_9_n_0\ : STD_LOGIC;
  signal \axil_rdata[20]_i_1_n_0\ : STD_LOGIC;
  signal \axil_rdata[20]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[21]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[22]_i_1_n_0\ : STD_LOGIC;
  signal \axil_rdata[22]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[23]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[24]_i_1_n_0\ : STD_LOGIC;
  signal \axil_rdata[24]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[25]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[26]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[27]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[28]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[29]_i_1_n_0\ : STD_LOGIC;
  signal \axil_rdata[29]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[29]_i_3_n_0\ : STD_LOGIC;
  signal \axil_rdata[2]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[30]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[31]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[31]_i_3_n_0\ : STD_LOGIC;
  signal \axil_rdata[3]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[4]_i_1_n_0\ : STD_LOGIC;
  signal \axil_rdata[4]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[5]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[6]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[7]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[8]_i_1_n_0\ : STD_LOGIC;
  signal \axil_rdata[8]_i_2_n_0\ : STD_LOGIC;
  signal \axil_rdata[9]_i_2_n_0\ : STD_LOGIC;
  signal axil_rdata_0 : STD_LOGIC_VECTOR ( 31 downto 0 );
  signal \^axil_rvalid\ : STD_LOGIC;
  signal axil_rvalid_i_1_n_0 : STD_LOGIC;
  signal \^axil_wready\ : STD_LOGIC;
  signal axil_wready4_out : STD_LOGIC;
  signal axil_wready_i_1_n_0 : STD_LOGIC;
  signal \clear_user_rst_reg_n_0_[0]\ : STD_LOGIC;
  signal done : STD_LOGIC;
  signal done0_synced : STD_LOGIC;
  signal done1_synced : STD_LOGIC;
  signal p_1_in : STD_LOGIC;
  signal \p_1_in__0\ : STD_LOGIC_VECTOR ( 1 to 1 );
  signal \p_1_in__1\ : STD_LOGIC_VECTOR ( 1 to 1 );
  signal p_1_out : STD_LOGIC_VECTOR ( 0 to 0 );
  signal \ref_clk_cntr[0]_i_3_n_0\ : STD_LOGIC;
  signal ref_clk_cntr_reg : STD_LOGIC_VECTOR ( 31 downto 0 );
  signal \ref_clk_cntr_reg[0]_i_2_n_0\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[0]_i_2_n_1\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[0]_i_2_n_10\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[0]_i_2_n_11\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[0]_i_2_n_12\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[0]_i_2_n_13\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[0]_i_2_n_14\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[0]_i_2_n_15\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[0]_i_2_n_2\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[0]_i_2_n_3\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[0]_i_2_n_4\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[0]_i_2_n_5\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[0]_i_2_n_6\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[0]_i_2_n_7\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[0]_i_2_n_8\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[0]_i_2_n_9\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[16]_i_1_n_0\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[16]_i_1_n_1\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[16]_i_1_n_10\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[16]_i_1_n_11\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[16]_i_1_n_12\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[16]_i_1_n_13\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[16]_i_1_n_14\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[16]_i_1_n_15\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[16]_i_1_n_2\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[16]_i_1_n_3\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[16]_i_1_n_4\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[16]_i_1_n_5\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[16]_i_1_n_6\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[16]_i_1_n_7\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[16]_i_1_n_8\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[16]_i_1_n_9\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[24]_i_1_n_1\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[24]_i_1_n_10\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[24]_i_1_n_11\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[24]_i_1_n_12\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[24]_i_1_n_13\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[24]_i_1_n_14\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[24]_i_1_n_15\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[24]_i_1_n_2\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[24]_i_1_n_3\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[24]_i_1_n_4\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[24]_i_1_n_5\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[24]_i_1_n_6\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[24]_i_1_n_7\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[24]_i_1_n_8\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[24]_i_1_n_9\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[8]_i_1_n_0\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[8]_i_1_n_1\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[8]_i_1_n_10\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[8]_i_1_n_11\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[8]_i_1_n_12\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[8]_i_1_n_13\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[8]_i_1_n_14\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[8]_i_1_n_15\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[8]_i_1_n_2\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[8]_i_1_n_3\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[8]_i_1_n_4\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[8]_i_1_n_5\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[8]_i_1_n_6\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[8]_i_1_n_7\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[8]_i_1_n_8\ : STD_LOGIC;
  signal \ref_clk_cntr_reg[8]_i_1_n_9\ : STD_LOGIC;
  signal rst0_synced : STD_LOGIC;
  signal rst1_synced : STD_LOGIC;
  signal sel : STD_LOGIC;
  signal src_in0 : STD_LOGIC;
  signal src_in1 : STD_LOGIC;
  signal state_read : STD_LOGIC_VECTOR ( 1 downto 0 );
  signal \state_read[1]_i_1_n_0\ : STD_LOGIC;
  signal state_write : STD_LOGIC_VECTOR ( 2 downto 0 );
  signal \state_write[2]_i_2_n_0\ : STD_LOGIC;
  signal \test_clk0_cntr[0]_i_1_n_0\ : STD_LOGIC;
  signal \test_clk0_cntr[0]_i_3_n_0\ : STD_LOGIC;
  signal test_clk0_cntr_reg : STD_LOGIC_VECTOR ( 31 downto 0 );
  signal \test_clk0_cntr_reg[0]_i_2_n_0\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[0]_i_2_n_1\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[0]_i_2_n_10\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[0]_i_2_n_11\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[0]_i_2_n_12\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[0]_i_2_n_13\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[0]_i_2_n_14\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[0]_i_2_n_15\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[0]_i_2_n_2\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[0]_i_2_n_3\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[0]_i_2_n_4\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[0]_i_2_n_5\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[0]_i_2_n_6\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[0]_i_2_n_7\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[0]_i_2_n_8\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[0]_i_2_n_9\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[16]_i_1_n_0\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[16]_i_1_n_1\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[16]_i_1_n_10\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[16]_i_1_n_11\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[16]_i_1_n_12\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[16]_i_1_n_13\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[16]_i_1_n_14\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[16]_i_1_n_15\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[16]_i_1_n_2\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[16]_i_1_n_3\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[16]_i_1_n_4\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[16]_i_1_n_5\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[16]_i_1_n_6\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[16]_i_1_n_7\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[16]_i_1_n_8\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[16]_i_1_n_9\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[24]_i_1_n_1\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[24]_i_1_n_10\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[24]_i_1_n_11\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[24]_i_1_n_12\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[24]_i_1_n_13\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[24]_i_1_n_14\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[24]_i_1_n_15\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[24]_i_1_n_2\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[24]_i_1_n_3\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[24]_i_1_n_4\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[24]_i_1_n_5\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[24]_i_1_n_6\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[24]_i_1_n_7\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[24]_i_1_n_8\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[24]_i_1_n_9\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[8]_i_1_n_0\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[8]_i_1_n_1\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[8]_i_1_n_10\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[8]_i_1_n_11\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[8]_i_1_n_12\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[8]_i_1_n_13\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[8]_i_1_n_14\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[8]_i_1_n_15\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[8]_i_1_n_2\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[8]_i_1_n_3\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[8]_i_1_n_4\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[8]_i_1_n_5\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[8]_i_1_n_6\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[8]_i_1_n_7\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[8]_i_1_n_8\ : STD_LOGIC;
  signal \test_clk0_cntr_reg[8]_i_1_n_9\ : STD_LOGIC;
  signal test_clk0_cntr_synced : STD_LOGIC_VECTOR ( 31 downto 0 );
  signal \test_clk1_cntr[0]_i_1_n_0\ : STD_LOGIC;
  signal \test_clk1_cntr[0]_i_3_n_0\ : STD_LOGIC;
  signal test_clk1_cntr_reg : STD_LOGIC_VECTOR ( 31 downto 0 );
  signal \test_clk1_cntr_reg[0]_i_2_n_0\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[0]_i_2_n_1\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[0]_i_2_n_10\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[0]_i_2_n_11\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[0]_i_2_n_12\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[0]_i_2_n_13\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[0]_i_2_n_14\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[0]_i_2_n_15\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[0]_i_2_n_2\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[0]_i_2_n_3\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[0]_i_2_n_4\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[0]_i_2_n_5\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[0]_i_2_n_6\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[0]_i_2_n_7\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[0]_i_2_n_8\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[0]_i_2_n_9\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[16]_i_1_n_0\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[16]_i_1_n_1\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[16]_i_1_n_10\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[16]_i_1_n_11\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[16]_i_1_n_12\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[16]_i_1_n_13\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[16]_i_1_n_14\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[16]_i_1_n_15\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[16]_i_1_n_2\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[16]_i_1_n_3\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[16]_i_1_n_4\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[16]_i_1_n_5\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[16]_i_1_n_6\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[16]_i_1_n_7\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[16]_i_1_n_8\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[16]_i_1_n_9\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[24]_i_1_n_1\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[24]_i_1_n_10\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[24]_i_1_n_11\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[24]_i_1_n_12\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[24]_i_1_n_13\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[24]_i_1_n_14\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[24]_i_1_n_15\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[24]_i_1_n_2\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[24]_i_1_n_3\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[24]_i_1_n_4\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[24]_i_1_n_5\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[24]_i_1_n_6\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[24]_i_1_n_7\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[24]_i_1_n_8\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[24]_i_1_n_9\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[8]_i_1_n_0\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[8]_i_1_n_1\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[8]_i_1_n_10\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[8]_i_1_n_11\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[8]_i_1_n_12\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[8]_i_1_n_13\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[8]_i_1_n_14\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[8]_i_1_n_15\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[8]_i_1_n_2\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[8]_i_1_n_3\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[8]_i_1_n_4\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[8]_i_1_n_5\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[8]_i_1_n_6\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[8]_i_1_n_7\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[8]_i_1_n_8\ : STD_LOGIC;
  signal \test_clk1_cntr_reg[8]_i_1_n_9\ : STD_LOGIC;
  signal test_clk1_cntr_synced : STD_LOGIC_VECTOR ( 31 downto 0 );
  signal user_rst0_ack : STD_LOGIC;
  signal user_rst1_ack : STD_LOGIC;
  signal user_rst_i_1_n_0 : STD_LOGIC;
  signal user_rst_reg_n_0 : STD_LOGIC;
  signal xpm_cdc_array_single_inst2_i_2_n_0 : STD_LOGIC;
  signal xpm_cdc_array_single_inst2_i_3_n_0 : STD_LOGIC;
  signal \NLW_ref_clk_cntr_reg[24]_i_1_CO_UNCONNECTED\ : STD_LOGIC_VECTOR ( 7 to 7 );
  signal \NLW_test_clk0_cntr_reg[24]_i_1_CO_UNCONNECTED\ : STD_LOGIC_VECTOR ( 7 to 7 );
  signal \NLW_test_clk1_cntr_reg[24]_i_1_CO_UNCONNECTED\ : STD_LOGIC_VECTOR ( 7 to 7 );
  attribute SOFT_HLUTNM : string;
  attribute SOFT_HLUTNM of axil_bvalid_i_2 : label is "soft_lutpair0";
  attribute SOFT_HLUTNM of \axil_rdata[1]_i_10\ : label is "soft_lutpair1";
  attribute SOFT_HLUTNM of \axil_rdata[1]_i_8\ : label is "soft_lutpair2";
  attribute ADDER_THRESHOLD : integer;
  attribute ADDER_THRESHOLD of \ref_clk_cntr_reg[0]_i_2\ : label is 16;
  attribute ADDER_THRESHOLD of \ref_clk_cntr_reg[16]_i_1\ : label is 16;
  attribute ADDER_THRESHOLD of \ref_clk_cntr_reg[24]_i_1\ : label is 16;
  attribute ADDER_THRESHOLD of \ref_clk_cntr_reg[8]_i_1\ : label is 16;
  attribute FSM_ENCODED_STATES : string;
  attribute FSM_ENCODED_STATES of \state_read_reg[0]\ : label is "IDLE_READ:01,WAIT_FOR_RVALID_READ:10,";
  attribute FSM_ENCODED_STATES of \state_read_reg[1]\ : label is "IDLE_READ:01,WAIT_FOR_RVALID_READ:10,";
  attribute SOFT_HLUTNM of \state_write[1]_i_1\ : label is "soft_lutpair0";
  attribute FSM_ENCODED_STATES of \state_write_reg[0]\ : label is "IDLE_WRITE:001,WAIT_FOR_WVALID_WRITE:010,WAIT_FOR_BREADY_WRITE:100,";
  attribute FSM_ENCODED_STATES of \state_write_reg[1]\ : label is "IDLE_WRITE:001,WAIT_FOR_WVALID_WRITE:010,WAIT_FOR_BREADY_WRITE:100,";
  attribute FSM_ENCODED_STATES of \state_write_reg[2]\ : label is "IDLE_WRITE:001,WAIT_FOR_WVALID_WRITE:010,WAIT_FOR_BREADY_WRITE:100,";
  attribute ADDER_THRESHOLD of \test_clk0_cntr_reg[0]_i_2\ : label is 16;
  attribute ADDER_THRESHOLD of \test_clk0_cntr_reg[16]_i_1\ : label is 16;
  attribute ADDER_THRESHOLD of \test_clk0_cntr_reg[24]_i_1\ : label is 16;
  attribute ADDER_THRESHOLD of \test_clk0_cntr_reg[8]_i_1\ : label is 16;
  attribute ADDER_THRESHOLD of \test_clk1_cntr_reg[0]_i_2\ : label is 16;
  attribute ADDER_THRESHOLD of \test_clk1_cntr_reg[16]_i_1\ : label is 16;
  attribute ADDER_THRESHOLD of \test_clk1_cntr_reg[24]_i_1\ : label is 16;
  attribute ADDER_THRESHOLD of \test_clk1_cntr_reg[8]_i_1\ : label is 16;
  attribute DEST_SYNC_FF : integer;
  attribute DEST_SYNC_FF of xpm_cdc_array_single_inst : label is 2;
  attribute INIT_SYNC_FF : integer;
  attribute INIT_SYNC_FF of xpm_cdc_array_single_inst : label is 0;
  attribute SIM_ASSERT_CHK : integer;
  attribute SIM_ASSERT_CHK of xpm_cdc_array_single_inst : label is 0;
  attribute SRC_INPUT_REG : integer;
  attribute SRC_INPUT_REG of xpm_cdc_array_single_inst : label is 0;
  attribute VERSION : integer;
  attribute VERSION of xpm_cdc_array_single_inst : label is 0;
  attribute WIDTH : integer;
  attribute WIDTH of xpm_cdc_array_single_inst : label is 1;
  attribute XPM_CDC : string;
  attribute XPM_CDC of xpm_cdc_array_single_inst : label is "ARRAY_SINGLE";
  attribute XPM_MODULE : string;
  attribute XPM_MODULE of xpm_cdc_array_single_inst : label is "TRUE";
  attribute DEST_SYNC_FF of xpm_cdc_array_single_inst1 : label is 2;
  attribute INIT_SYNC_FF of xpm_cdc_array_single_inst1 : label is 0;
  attribute SIM_ASSERT_CHK of xpm_cdc_array_single_inst1 : label is 0;
  attribute SRC_INPUT_REG of xpm_cdc_array_single_inst1 : label is 0;
  attribute VERSION of xpm_cdc_array_single_inst1 : label is 0;
  attribute WIDTH of xpm_cdc_array_single_inst1 : label is 1;
  attribute XPM_CDC of xpm_cdc_array_single_inst1 : label is "ARRAY_SINGLE";
  attribute XPM_MODULE of xpm_cdc_array_single_inst1 : label is "TRUE";
  attribute DEST_SYNC_FF of xpm_cdc_array_single_inst2 : label is 2;
  attribute INIT_SYNC_FF of xpm_cdc_array_single_inst2 : label is 0;
  attribute SIM_ASSERT_CHK of xpm_cdc_array_single_inst2 : label is 0;
  attribute SRC_INPUT_REG of xpm_cdc_array_single_inst2 : label is 0;
  attribute VERSION of xpm_cdc_array_single_inst2 : label is 0;
  attribute WIDTH of xpm_cdc_array_single_inst2 : label is 1;
  attribute XPM_CDC of xpm_cdc_array_single_inst2 : label is "ARRAY_SINGLE";
  attribute XPM_MODULE of xpm_cdc_array_single_inst2 : label is "TRUE";
  attribute SOFT_HLUTNM of xpm_cdc_array_single_inst2_i_2 : label is "soft_lutpair1";
  attribute SOFT_HLUTNM of xpm_cdc_array_single_inst2_i_3 : label is "soft_lutpair2";
  attribute DEST_SYNC_FF of xpm_cdc_array_single_inst3 : label is 2;
  attribute INIT_SYNC_FF of xpm_cdc_array_single_inst3 : label is 0;
  attribute SIM_ASSERT_CHK of xpm_cdc_array_single_inst3 : label is 0;
  attribute SRC_INPUT_REG of xpm_cdc_array_single_inst3 : label is 0;
  attribute VERSION of xpm_cdc_array_single_inst3 : label is 0;
  attribute WIDTH of xpm_cdc_array_single_inst3 : label is 32;
  attribute XPM_CDC of xpm_cdc_array_single_inst3 : label is "ARRAY_SINGLE";
  attribute XPM_MODULE of xpm_cdc_array_single_inst3 : label is "TRUE";
  attribute DEST_SYNC_FF of xpm_cdc_array_single_inst4 : label is 2;
  attribute INIT_SYNC_FF of xpm_cdc_array_single_inst4 : label is 0;
  attribute SIM_ASSERT_CHK of xpm_cdc_array_single_inst4 : label is 0;
  attribute SRC_INPUT_REG of xpm_cdc_array_single_inst4 : label is 0;
  attribute VERSION of xpm_cdc_array_single_inst4 : label is 0;
  attribute WIDTH of xpm_cdc_array_single_inst4 : label is 32;
  attribute XPM_CDC of xpm_cdc_array_single_inst4 : label is "ARRAY_SINGLE";
  attribute XPM_MODULE of xpm_cdc_array_single_inst4 : label is "TRUE";
  attribute DEST_SYNC_FF of xpm_cdc_array_single_inst5 : label is 2;
  attribute INIT_SYNC_FF of xpm_cdc_array_single_inst5 : label is 0;
  attribute SIM_ASSERT_CHK of xpm_cdc_array_single_inst5 : label is 0;
  attribute SRC_INPUT_REG of xpm_cdc_array_single_inst5 : label is 0;
  attribute VERSION of xpm_cdc_array_single_inst5 : label is 0;
  attribute WIDTH of xpm_cdc_array_single_inst5 : label is 1;
  attribute XPM_CDC of xpm_cdc_array_single_inst5 : label is "ARRAY_SINGLE";
  attribute XPM_MODULE of xpm_cdc_array_single_inst5 : label is "TRUE";
  attribute DEST_SYNC_FF of xpm_cdc_array_single_inst6 : label is 2;
  attribute INIT_SYNC_FF of xpm_cdc_array_single_inst6 : label is 0;
  attribute SIM_ASSERT_CHK of xpm_cdc_array_single_inst6 : label is 0;
  attribute SRC_INPUT_REG of xpm_cdc_array_single_inst6 : label is 0;
  attribute VERSION of xpm_cdc_array_single_inst6 : label is 0;
  attribute WIDTH of xpm_cdc_array_single_inst6 : label is 1;
  attribute XPM_CDC of xpm_cdc_array_single_inst6 : label is "ARRAY_SINGLE";
  attribute XPM_MODULE of xpm_cdc_array_single_inst6 : label is "TRUE";
  attribute DEST_SYNC_FF of xpm_cdc_array_single_inst7 : label is 2;
  attribute INIT_SYNC_FF of xpm_cdc_array_single_inst7 : label is 0;
  attribute SIM_ASSERT_CHK of xpm_cdc_array_single_inst7 : label is 0;
  attribute SRC_INPUT_REG of xpm_cdc_array_single_inst7 : label is 0;
  attribute VERSION of xpm_cdc_array_single_inst7 : label is 0;
  attribute WIDTH of xpm_cdc_array_single_inst7 : label is 1;
  attribute XPM_CDC of xpm_cdc_array_single_inst7 : label is "ARRAY_SINGLE";
  attribute XPM_MODULE of xpm_cdc_array_single_inst7 : label is "TRUE";
begin
  axil_arready <= \^axil_arready\;
  axil_awready <= \^axil_awready\;
  axil_bvalid <= \^axil_bvalid\;
  axil_rvalid <= \^axil_rvalid\;
  axil_wready <= \^axil_wready\;
axil_arready_i_1: unisim.vcomponents.LUT5
    generic map(
      INIT => X"F7F72000"
    )
        port map (
      I0 => reset_n,
      I1 => state_read(1),
      I2 => state_read(0),
      I3 => axil_arvalid,
      I4 => \^axil_arready\,
      O => axil_arready_i_1_n_0
    );
axil_arready_reg: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => '1',
      D => axil_arready_i_1_n_0,
      Q => \^axil_arready\,
      R => '0'
    );
axil_awready_i_1: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFDDFF00004000"
    )
        port map (
      I0 => state_write(1),
      I1 => state_write(0),
      I2 => axil_awvalid,
      I3 => reset_n,
      I4 => state_write(2),
      I5 => \^axil_awready\,
      O => axil_awready_i_1_n_0
    );
axil_awready_reg: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => '1',
      D => axil_awready_i_1_n_0,
      Q => \^axil_awready\,
      R => '0'
    );
axil_bvalid_i_1: unisim.vcomponents.LUT6
    generic map(
      INIT => X"2F2F2FFF20202000"
    )
        port map (
      I0 => axil_wvalid,
      I1 => state_write(2),
      I2 => axil_wready4_out,
      I3 => axil_bready,
      I4 => state_write(1),
      I5 => \^axil_bvalid\,
      O => axil_bvalid_i_1_n_0
    );
axil_bvalid_i_2: unisim.vcomponents.LUT5
    generic map(
      INIT => X"00002088"
    )
        port map (
      I0 => reset_n,
      I1 => state_write(2),
      I2 => axil_wvalid,
      I3 => state_write(1),
      I4 => state_write(0),
      O => axil_wready4_out
    );
axil_bvalid_reg: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => '1',
      D => axil_bvalid_i_1_n_0,
      Q => \^axil_bvalid\,
      R => '0'
    );
\axil_rdata[0]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFFFEAEAAAA"
    )
        port map (
      I0 => \axil_rdata[0]_i_2_n_0\,
      I1 => test_clk0_cntr_synced(0),
      I2 => axil_araddr(2),
      I3 => test_clk1_cntr_synced(0),
      I4 => axil_araddr(3),
      I5 => \axil_rdata[0]_i_3_n_0\,
      O => axil_rdata_0(0)
    );
\axil_rdata[0]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"00000000FFE200E2"
    )
        port map (
      I0 => axil_wdata(0),
      I1 => \axil_rdata[31]_i_2_n_0\,
      I2 => user_rst_reg_n_0,
      I3 => axil_araddr(2),
      I4 => ref_clk_cntr_reg(0),
      I5 => axil_araddr(3),
      O => \axil_rdata[0]_i_2_n_0\
    );
\axil_rdata[0]_i_3\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"E"
    )
        port map (
      I0 => axil_araddr(0),
      I1 => axil_araddr(1),
      O => \axil_rdata[0]_i_3_n_0\
    );
\axil_rdata[10]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF40404540"
    )
        port map (
      I0 => axil_araddr(3),
      I1 => ref_clk_cntr_reg(10),
      I2 => axil_araddr(2),
      I3 => axil_wdata(9),
      I4 => \axil_rdata[31]_i_2_n_0\,
      I5 => \axil_rdata[10]_i_2_n_0\,
      O => axil_rdata_0(10)
    );
\axil_rdata[10]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FEEEFEFEFEEEEEEE"
    )
        port map (
      I0 => axil_araddr(1),
      I1 => axil_araddr(0),
      I2 => axil_araddr(3),
      I3 => test_clk1_cntr_synced(10),
      I4 => axil_araddr(2),
      I5 => test_clk0_cntr_synced(10),
      O => \axil_rdata[10]_i_2_n_0\
    );
\axil_rdata[11]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF40404540"
    )
        port map (
      I0 => axil_araddr(3),
      I1 => ref_clk_cntr_reg(11),
      I2 => axil_araddr(2),
      I3 => axil_wdata(10),
      I4 => \axil_rdata[31]_i_2_n_0\,
      I5 => \axil_rdata[11]_i_2_n_0\,
      O => axil_rdata_0(11)
    );
\axil_rdata[11]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FEEEFEFEFEEEEEEE"
    )
        port map (
      I0 => axil_araddr(1),
      I1 => axil_araddr(0),
      I2 => axil_araddr(3),
      I3 => test_clk1_cntr_synced(11),
      I4 => axil_araddr(2),
      I5 => test_clk0_cntr_synced(11),
      O => \axil_rdata[11]_i_2_n_0\
    );
\axil_rdata[12]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF40404540"
    )
        port map (
      I0 => axil_araddr(3),
      I1 => ref_clk_cntr_reg(12),
      I2 => axil_araddr(2),
      I3 => axil_wdata(11),
      I4 => \axil_rdata[31]_i_2_n_0\,
      I5 => \axil_rdata[12]_i_2_n_0\,
      O => axil_rdata_0(12)
    );
\axil_rdata[12]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FEEEFEFEFEEEEEEE"
    )
        port map (
      I0 => axil_araddr(1),
      I1 => axil_araddr(0),
      I2 => axil_araddr(3),
      I3 => test_clk1_cntr_synced(12),
      I4 => axil_araddr(2),
      I5 => test_clk0_cntr_synced(12),
      O => \axil_rdata[12]_i_2_n_0\
    );
\axil_rdata[13]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF40404540"
    )
        port map (
      I0 => axil_araddr(3),
      I1 => ref_clk_cntr_reg(13),
      I2 => axil_araddr(2),
      I3 => axil_wdata(12),
      I4 => \axil_rdata[31]_i_2_n_0\,
      I5 => \axil_rdata[13]_i_2_n_0\,
      O => axil_rdata_0(13)
    );
\axil_rdata[13]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FEEEFEFEFEEEEEEE"
    )
        port map (
      I0 => axil_araddr(1),
      I1 => axil_araddr(0),
      I2 => axil_araddr(3),
      I3 => test_clk1_cntr_synced(13),
      I4 => axil_araddr(2),
      I5 => test_clk0_cntr_synced(13),
      O => \axil_rdata[13]_i_2_n_0\
    );
\axil_rdata[14]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"AFA0CFCFAFA0C0C0"
    )
        port map (
      I0 => test_clk1_cntr_synced(14),
      I1 => test_clk0_cntr_synced(14),
      I2 => axil_araddr(3),
      I3 => ref_clk_cntr_reg(14),
      I4 => axil_araddr(2),
      I5 => \axil_rdata[14]_i_2_n_0\,
      O => \axil_rdata[14]_i_1_n_0\
    );
\axil_rdata[14]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"0000000000000008"
    )
        port map (
      I0 => axil_wdata(13),
      I1 => axil_wvalid,
      I2 => axil_awaddr(0),
      I3 => axil_awaddr(1),
      I4 => axil_awaddr(2),
      I5 => axil_awaddr(3),
      O => \axil_rdata[14]_i_2_n_0\
    );
\axil_rdata[15]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF40404540"
    )
        port map (
      I0 => axil_araddr(3),
      I1 => ref_clk_cntr_reg(15),
      I2 => axil_araddr(2),
      I3 => axil_wdata(14),
      I4 => \axil_rdata[31]_i_2_n_0\,
      I5 => \axil_rdata[15]_i_2_n_0\,
      O => axil_rdata_0(15)
    );
\axil_rdata[15]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FEEEFEFEFEEEEEEE"
    )
        port map (
      I0 => axil_araddr(1),
      I1 => axil_araddr(0),
      I2 => axil_araddr(3),
      I3 => test_clk1_cntr_synced(15),
      I4 => axil_araddr(2),
      I5 => test_clk0_cntr_synced(15),
      O => \axil_rdata[15]_i_2_n_0\
    );
\axil_rdata[16]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF40404540"
    )
        port map (
      I0 => axil_araddr(3),
      I1 => ref_clk_cntr_reg(16),
      I2 => axil_araddr(2),
      I3 => axil_wdata(15),
      I4 => \axil_rdata[31]_i_2_n_0\,
      I5 => \axil_rdata[16]_i_2_n_0\,
      O => axil_rdata_0(16)
    );
\axil_rdata[16]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FEEEFEFEFEEEEEEE"
    )
        port map (
      I0 => axil_araddr(1),
      I1 => axil_araddr(0),
      I2 => axil_araddr(3),
      I3 => test_clk1_cntr_synced(16),
      I4 => axil_araddr(2),
      I5 => test_clk0_cntr_synced(16),
      O => \axil_rdata[16]_i_2_n_0\
    );
\axil_rdata[17]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"AFA0CFCFAFA0C0C0"
    )
        port map (
      I0 => test_clk1_cntr_synced(17),
      I1 => test_clk0_cntr_synced(17),
      I2 => axil_araddr(3),
      I3 => ref_clk_cntr_reg(17),
      I4 => axil_araddr(2),
      I5 => \axil_rdata[17]_i_2_n_0\,
      O => \axil_rdata[17]_i_1_n_0\
    );
\axil_rdata[17]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"0000000000000008"
    )
        port map (
      I0 => axil_wdata(16),
      I1 => axil_wvalid,
      I2 => axil_awaddr(0),
      I3 => axil_awaddr(1),
      I4 => axil_awaddr(2),
      I5 => axil_awaddr(3),
      O => \axil_rdata[17]_i_2_n_0\
    );
\axil_rdata[18]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF40404540"
    )
        port map (
      I0 => axil_araddr(3),
      I1 => ref_clk_cntr_reg(18),
      I2 => axil_araddr(2),
      I3 => axil_wdata(17),
      I4 => \axil_rdata[31]_i_2_n_0\,
      I5 => \axil_rdata[18]_i_2_n_0\,
      O => axil_rdata_0(18)
    );
\axil_rdata[18]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FEEEFEFEFEEEEEEE"
    )
        port map (
      I0 => axil_araddr(1),
      I1 => axil_araddr(0),
      I2 => axil_araddr(3),
      I3 => test_clk1_cntr_synced(18),
      I4 => axil_araddr(2),
      I5 => test_clk0_cntr_synced(18),
      O => \axil_rdata[18]_i_2_n_0\
    );
\axil_rdata[19]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF40404540"
    )
        port map (
      I0 => axil_araddr(3),
      I1 => ref_clk_cntr_reg(19),
      I2 => axil_araddr(2),
      I3 => axil_wdata(18),
      I4 => \axil_rdata[31]_i_2_n_0\,
      I5 => \axil_rdata[19]_i_2_n_0\,
      O => axil_rdata_0(19)
    );
\axil_rdata[19]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FEEEFEFEFEEEEEEE"
    )
        port map (
      I0 => axil_araddr(1),
      I1 => axil_araddr(0),
      I2 => axil_araddr(3),
      I3 => test_clk1_cntr_synced(19),
      I4 => axil_araddr(2),
      I5 => test_clk0_cntr_synced(19),
      O => \axil_rdata[19]_i_2_n_0\
    );
\axil_rdata[1]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF44444544"
    )
        port map (
      I0 => \axil_rdata[1]_i_2_n_0\,
      I1 => axil_araddr(2),
      I2 => \axil_rdata[1]_i_3_n_0\,
      I3 => \axil_rdata[1]_i_4_n_0\,
      I4 => \axil_rdata[1]_i_5_n_0\,
      I5 => \axil_rdata[1]_i_6_n_0\,
      O => axil_rdata_0(1)
    );
\axil_rdata[1]_i_10\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"FFFE"
    )
        port map (
      I0 => ref_clk_cntr_reg(12),
      I1 => ref_clk_cntr_reg(11),
      I2 => ref_clk_cntr_reg(13),
      I3 => ref_clk_cntr_reg(10),
      O => \axil_rdata[1]_i_10_n_0\
    );
\axil_rdata[1]_i_11\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"FFFE"
    )
        port map (
      I0 => ref_clk_cntr_reg(24),
      I1 => ref_clk_cntr_reg(27),
      I2 => ref_clk_cntr_reg(25),
      I3 => ref_clk_cntr_reg(26),
      O => \axil_rdata[1]_i_11_n_0\
    );
\axil_rdata[1]_i_12\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"FFDF"
    )
        port map (
      I0 => ref_clk_cntr_reg(14),
      I1 => ref_clk_cntr_reg(1),
      I2 => ref_clk_cntr_reg(15),
      I3 => ref_clk_cntr_reg(0),
      O => \axil_rdata[1]_i_12_n_0\
    );
\axil_rdata[1]_i_2\: unisim.vcomponents.LUT3
    generic map(
      INIT => X"BA"
    )
        port map (
      I0 => axil_araddr(3),
      I1 => ref_clk_cntr_reg(1),
      I2 => axil_araddr(2),
      O => \axil_rdata[1]_i_2_n_0\
    );
\axil_rdata[1]_i_3\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"FFFE"
    )
        port map (
      I0 => \axil_rdata[1]_i_7_n_0\,
      I1 => \axil_rdata[1]_i_8_n_0\,
      I2 => \axil_rdata[1]_i_9_n_0\,
      I3 => \axil_rdata[1]_i_10_n_0\,
      O => \axil_rdata[1]_i_3_n_0\
    );
\axil_rdata[1]_i_4\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"00008000"
    )
        port map (
      I0 => ref_clk_cntr_reg(8),
      I1 => ref_clk_cntr_reg(6),
      I2 => ref_clk_cntr_reg(9),
      I3 => ref_clk_cntr_reg(4),
      I4 => \axil_rdata[1]_i_11_n_0\,
      O => \axil_rdata[1]_i_4_n_0\
    );
\axil_rdata[1]_i_5\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"FFFFFFFE"
    )
        port map (
      I0 => ref_clk_cntr_reg(28),
      I1 => ref_clk_cntr_reg(30),
      I2 => ref_clk_cntr_reg(29),
      I3 => ref_clk_cntr_reg(31),
      I4 => \axil_rdata[1]_i_12_n_0\,
      O => \axil_rdata[1]_i_5_n_0\
    );
\axil_rdata[1]_i_6\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FEEEFEFEFEEEEEEE"
    )
        port map (
      I0 => axil_araddr(1),
      I1 => axil_araddr(0),
      I2 => axil_araddr(3),
      I3 => test_clk1_cntr_synced(1),
      I4 => axil_araddr(2),
      I5 => test_clk0_cntr_synced(1),
      O => \axil_rdata[1]_i_6_n_0\
    );
\axil_rdata[1]_i_7\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"FFFE"
    )
        port map (
      I0 => ref_clk_cntr_reg(2),
      I1 => ref_clk_cntr_reg(7),
      I2 => ref_clk_cntr_reg(3),
      I3 => ref_clk_cntr_reg(5),
      O => \axil_rdata[1]_i_7_n_0\
    );
\axil_rdata[1]_i_8\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"FFFE"
    )
        port map (
      I0 => ref_clk_cntr_reg(22),
      I1 => ref_clk_cntr_reg(21),
      I2 => ref_clk_cntr_reg(23),
      I3 => ref_clk_cntr_reg(20),
      O => \axil_rdata[1]_i_8_n_0\
    );
\axil_rdata[1]_i_9\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"FFFE"
    )
        port map (
      I0 => ref_clk_cntr_reg(18),
      I1 => ref_clk_cntr_reg(17),
      I2 => ref_clk_cntr_reg(19),
      I3 => ref_clk_cntr_reg(16),
      O => \axil_rdata[1]_i_9_n_0\
    );
\axil_rdata[20]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"AFA0CFCFAFA0C0C0"
    )
        port map (
      I0 => test_clk1_cntr_synced(20),
      I1 => test_clk0_cntr_synced(20),
      I2 => axil_araddr(3),
      I3 => ref_clk_cntr_reg(20),
      I4 => axil_araddr(2),
      I5 => \axil_rdata[20]_i_2_n_0\,
      O => \axil_rdata[20]_i_1_n_0\
    );
\axil_rdata[20]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"0000000000000008"
    )
        port map (
      I0 => axil_wdata(19),
      I1 => axil_wvalid,
      I2 => axil_awaddr(0),
      I3 => axil_awaddr(1),
      I4 => axil_awaddr(2),
      I5 => axil_awaddr(3),
      O => \axil_rdata[20]_i_2_n_0\
    );
\axil_rdata[21]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF40404540"
    )
        port map (
      I0 => axil_araddr(3),
      I1 => ref_clk_cntr_reg(21),
      I2 => axil_araddr(2),
      I3 => axil_wdata(20),
      I4 => \axil_rdata[31]_i_2_n_0\,
      I5 => \axil_rdata[21]_i_2_n_0\,
      O => axil_rdata_0(21)
    );
\axil_rdata[21]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FEEEFEFEFEEEEEEE"
    )
        port map (
      I0 => axil_araddr(1),
      I1 => axil_araddr(0),
      I2 => axil_araddr(3),
      I3 => test_clk1_cntr_synced(21),
      I4 => axil_araddr(2),
      I5 => test_clk0_cntr_synced(21),
      O => \axil_rdata[21]_i_2_n_0\
    );
\axil_rdata[22]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"AFA0CFCFAFA0C0C0"
    )
        port map (
      I0 => test_clk1_cntr_synced(22),
      I1 => test_clk0_cntr_synced(22),
      I2 => axil_araddr(3),
      I3 => ref_clk_cntr_reg(22),
      I4 => axil_araddr(2),
      I5 => \axil_rdata[22]_i_2_n_0\,
      O => \axil_rdata[22]_i_1_n_0\
    );
\axil_rdata[22]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"0000000000000008"
    )
        port map (
      I0 => axil_wdata(21),
      I1 => axil_wvalid,
      I2 => axil_awaddr(0),
      I3 => axil_awaddr(1),
      I4 => axil_awaddr(2),
      I5 => axil_awaddr(3),
      O => \axil_rdata[22]_i_2_n_0\
    );
\axil_rdata[23]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF40404540"
    )
        port map (
      I0 => axil_araddr(3),
      I1 => ref_clk_cntr_reg(23),
      I2 => axil_araddr(2),
      I3 => axil_wdata(22),
      I4 => \axil_rdata[31]_i_2_n_0\,
      I5 => \axil_rdata[23]_i_2_n_0\,
      O => axil_rdata_0(23)
    );
\axil_rdata[23]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FEEEFEFEFEEEEEEE"
    )
        port map (
      I0 => axil_araddr(1),
      I1 => axil_araddr(0),
      I2 => axil_araddr(3),
      I3 => test_clk1_cntr_synced(23),
      I4 => axil_araddr(2),
      I5 => test_clk0_cntr_synced(23),
      O => \axil_rdata[23]_i_2_n_0\
    );
\axil_rdata[24]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"AFA0CFCFAFA0C0C0"
    )
        port map (
      I0 => test_clk1_cntr_synced(24),
      I1 => test_clk0_cntr_synced(24),
      I2 => axil_araddr(3),
      I3 => ref_clk_cntr_reg(24),
      I4 => axil_araddr(2),
      I5 => \axil_rdata[24]_i_2_n_0\,
      O => \axil_rdata[24]_i_1_n_0\
    );
\axil_rdata[24]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"0000000000000008"
    )
        port map (
      I0 => axil_wdata(23),
      I1 => axil_wvalid,
      I2 => axil_awaddr(0),
      I3 => axil_awaddr(1),
      I4 => axil_awaddr(2),
      I5 => axil_awaddr(3),
      O => \axil_rdata[24]_i_2_n_0\
    );
\axil_rdata[25]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF40404540"
    )
        port map (
      I0 => axil_araddr(3),
      I1 => ref_clk_cntr_reg(25),
      I2 => axil_araddr(2),
      I3 => axil_wdata(24),
      I4 => \axil_rdata[31]_i_2_n_0\,
      I5 => \axil_rdata[25]_i_2_n_0\,
      O => axil_rdata_0(25)
    );
\axil_rdata[25]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FEEEFEFEFEEEEEEE"
    )
        port map (
      I0 => axil_araddr(1),
      I1 => axil_araddr(0),
      I2 => axil_araddr(3),
      I3 => test_clk1_cntr_synced(25),
      I4 => axil_araddr(2),
      I5 => test_clk0_cntr_synced(25),
      O => \axil_rdata[25]_i_2_n_0\
    );
\axil_rdata[26]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF40404540"
    )
        port map (
      I0 => axil_araddr(3),
      I1 => ref_clk_cntr_reg(26),
      I2 => axil_araddr(2),
      I3 => axil_wdata(25),
      I4 => \axil_rdata[31]_i_2_n_0\,
      I5 => \axil_rdata[26]_i_2_n_0\,
      O => axil_rdata_0(26)
    );
\axil_rdata[26]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FEEEFEFEFEEEEEEE"
    )
        port map (
      I0 => axil_araddr(1),
      I1 => axil_araddr(0),
      I2 => axil_araddr(3),
      I3 => test_clk1_cntr_synced(26),
      I4 => axil_araddr(2),
      I5 => test_clk0_cntr_synced(26),
      O => \axil_rdata[26]_i_2_n_0\
    );
\axil_rdata[27]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF40404540"
    )
        port map (
      I0 => axil_araddr(3),
      I1 => ref_clk_cntr_reg(27),
      I2 => axil_araddr(2),
      I3 => axil_wdata(26),
      I4 => \axil_rdata[31]_i_2_n_0\,
      I5 => \axil_rdata[27]_i_2_n_0\,
      O => axil_rdata_0(27)
    );
\axil_rdata[27]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FEEEFEFEFEEEEEEE"
    )
        port map (
      I0 => axil_araddr(1),
      I1 => axil_araddr(0),
      I2 => axil_araddr(3),
      I3 => test_clk1_cntr_synced(27),
      I4 => axil_araddr(2),
      I5 => test_clk0_cntr_synced(27),
      O => \axil_rdata[27]_i_2_n_0\
    );
\axil_rdata[28]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF40404540"
    )
        port map (
      I0 => axil_araddr(3),
      I1 => ref_clk_cntr_reg(28),
      I2 => axil_araddr(2),
      I3 => axil_wdata(27),
      I4 => \axil_rdata[31]_i_2_n_0\,
      I5 => \axil_rdata[28]_i_2_n_0\,
      O => axil_rdata_0(28)
    );
\axil_rdata[28]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FEEEFEFEFEEEEEEE"
    )
        port map (
      I0 => axil_araddr(1),
      I1 => axil_araddr(0),
      I2 => axil_araddr(3),
      I3 => test_clk1_cntr_synced(28),
      I4 => axil_araddr(2),
      I5 => test_clk0_cntr_synced(28),
      O => \axil_rdata[28]_i_2_n_0\
    );
\axil_rdata[29]_i_1\: unisim.vcomponents.LUT3
    generic map(
      INIT => X"E0"
    )
        port map (
      I0 => axil_araddr(1),
      I1 => axil_araddr(0),
      I2 => axil_arvalid,
      O => \axil_rdata[29]_i_1_n_0\
    );
\axil_rdata[29]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"AFA0CFCFAFA0C0C0"
    )
        port map (
      I0 => test_clk1_cntr_synced(29),
      I1 => test_clk0_cntr_synced(29),
      I2 => axil_araddr(3),
      I3 => ref_clk_cntr_reg(29),
      I4 => axil_araddr(2),
      I5 => \axil_rdata[29]_i_3_n_0\,
      O => \axil_rdata[29]_i_2_n_0\
    );
\axil_rdata[29]_i_3\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"0000000000000008"
    )
        port map (
      I0 => axil_wdata(28),
      I1 => axil_wvalid,
      I2 => axil_awaddr(0),
      I3 => axil_awaddr(1),
      I4 => axil_awaddr(2),
      I5 => axil_awaddr(3),
      O => \axil_rdata[29]_i_3_n_0\
    );
\axil_rdata[2]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF40404540"
    )
        port map (
      I0 => axil_araddr(3),
      I1 => ref_clk_cntr_reg(2),
      I2 => axil_araddr(2),
      I3 => axil_wdata(1),
      I4 => \axil_rdata[31]_i_2_n_0\,
      I5 => \axil_rdata[2]_i_2_n_0\,
      O => axil_rdata_0(2)
    );
\axil_rdata[2]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FEEEFEFEFEEEEEEE"
    )
        port map (
      I0 => axil_araddr(1),
      I1 => axil_araddr(0),
      I2 => axil_araddr(3),
      I3 => test_clk1_cntr_synced(2),
      I4 => axil_araddr(2),
      I5 => test_clk0_cntr_synced(2),
      O => \axil_rdata[2]_i_2_n_0\
    );
\axil_rdata[30]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF40404540"
    )
        port map (
      I0 => axil_araddr(3),
      I1 => ref_clk_cntr_reg(30),
      I2 => axil_araddr(2),
      I3 => axil_wdata(29),
      I4 => \axil_rdata[31]_i_2_n_0\,
      I5 => \axil_rdata[30]_i_2_n_0\,
      O => axil_rdata_0(30)
    );
\axil_rdata[30]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FEEEFEFEFEEEEEEE"
    )
        port map (
      I0 => axil_araddr(1),
      I1 => axil_araddr(0),
      I2 => axil_araddr(3),
      I3 => test_clk1_cntr_synced(30),
      I4 => axil_araddr(2),
      I5 => test_clk0_cntr_synced(30),
      O => \axil_rdata[30]_i_2_n_0\
    );
\axil_rdata[31]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF40404540"
    )
        port map (
      I0 => axil_araddr(3),
      I1 => ref_clk_cntr_reg(31),
      I2 => axil_araddr(2),
      I3 => axil_wdata(30),
      I4 => \axil_rdata[31]_i_2_n_0\,
      I5 => \axil_rdata[31]_i_3_n_0\,
      O => axil_rdata_0(31)
    );
\axil_rdata[31]_i_2\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"FFFEFFFF"
    )
        port map (
      I0 => axil_awaddr(3),
      I1 => axil_awaddr(2),
      I2 => axil_awaddr(1),
      I3 => axil_awaddr(0),
      I4 => axil_wvalid,
      O => \axil_rdata[31]_i_2_n_0\
    );
\axil_rdata[31]_i_3\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFFFFFF8A80"
    )
        port map (
      I0 => axil_araddr(3),
      I1 => test_clk1_cntr_synced(31),
      I2 => axil_araddr(2),
      I3 => test_clk0_cntr_synced(31),
      I4 => axil_araddr(1),
      I5 => axil_araddr(0),
      O => \axil_rdata[31]_i_3_n_0\
    );
\axil_rdata[3]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF40404540"
    )
        port map (
      I0 => axil_araddr(3),
      I1 => ref_clk_cntr_reg(3),
      I2 => axil_araddr(2),
      I3 => axil_wdata(2),
      I4 => \axil_rdata[31]_i_2_n_0\,
      I5 => \axil_rdata[3]_i_2_n_0\,
      O => axil_rdata_0(3)
    );
\axil_rdata[3]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FEEEFEFEFEEEEEEE"
    )
        port map (
      I0 => axil_araddr(1),
      I1 => axil_araddr(0),
      I2 => axil_araddr(3),
      I3 => test_clk1_cntr_synced(3),
      I4 => axil_araddr(2),
      I5 => test_clk0_cntr_synced(3),
      O => \axil_rdata[3]_i_2_n_0\
    );
\axil_rdata[4]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"AFA0CFCFAFA0C0C0"
    )
        port map (
      I0 => test_clk1_cntr_synced(4),
      I1 => test_clk0_cntr_synced(4),
      I2 => axil_araddr(3),
      I3 => ref_clk_cntr_reg(4),
      I4 => axil_araddr(2),
      I5 => \axil_rdata[4]_i_2_n_0\,
      O => \axil_rdata[4]_i_1_n_0\
    );
\axil_rdata[4]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"0000000000000008"
    )
        port map (
      I0 => axil_wdata(3),
      I1 => axil_wvalid,
      I2 => axil_awaddr(0),
      I3 => axil_awaddr(1),
      I4 => axil_awaddr(2),
      I5 => axil_awaddr(3),
      O => \axil_rdata[4]_i_2_n_0\
    );
\axil_rdata[5]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF40404540"
    )
        port map (
      I0 => axil_araddr(3),
      I1 => ref_clk_cntr_reg(5),
      I2 => axil_araddr(2),
      I3 => axil_wdata(4),
      I4 => \axil_rdata[31]_i_2_n_0\,
      I5 => \axil_rdata[5]_i_2_n_0\,
      O => axil_rdata_0(5)
    );
\axil_rdata[5]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FEEEFEFEFEEEEEEE"
    )
        port map (
      I0 => axil_araddr(1),
      I1 => axil_araddr(0),
      I2 => axil_araddr(3),
      I3 => test_clk1_cntr_synced(5),
      I4 => axil_araddr(2),
      I5 => test_clk0_cntr_synced(5),
      O => \axil_rdata[5]_i_2_n_0\
    );
\axil_rdata[6]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF40404540"
    )
        port map (
      I0 => axil_araddr(3),
      I1 => ref_clk_cntr_reg(6),
      I2 => axil_araddr(2),
      I3 => axil_wdata(5),
      I4 => \axil_rdata[31]_i_2_n_0\,
      I5 => \axil_rdata[6]_i_2_n_0\,
      O => axil_rdata_0(6)
    );
\axil_rdata[6]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FEEEFEFEFEEEEEEE"
    )
        port map (
      I0 => axil_araddr(1),
      I1 => axil_araddr(0),
      I2 => axil_araddr(3),
      I3 => test_clk1_cntr_synced(6),
      I4 => axil_araddr(2),
      I5 => test_clk0_cntr_synced(6),
      O => \axil_rdata[6]_i_2_n_0\
    );
\axil_rdata[7]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF40404540"
    )
        port map (
      I0 => axil_araddr(3),
      I1 => ref_clk_cntr_reg(7),
      I2 => axil_araddr(2),
      I3 => axil_wdata(6),
      I4 => \axil_rdata[31]_i_2_n_0\,
      I5 => \axil_rdata[7]_i_2_n_0\,
      O => axil_rdata_0(7)
    );
\axil_rdata[7]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FEEEFEFEFEEEEEEE"
    )
        port map (
      I0 => axil_araddr(1),
      I1 => axil_araddr(0),
      I2 => axil_araddr(3),
      I3 => test_clk1_cntr_synced(7),
      I4 => axil_araddr(2),
      I5 => test_clk0_cntr_synced(7),
      O => \axil_rdata[7]_i_2_n_0\
    );
\axil_rdata[8]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"AFA0CFCFAFA0C0C0"
    )
        port map (
      I0 => test_clk1_cntr_synced(8),
      I1 => test_clk0_cntr_synced(8),
      I2 => axil_araddr(3),
      I3 => ref_clk_cntr_reg(8),
      I4 => axil_araddr(2),
      I5 => \axil_rdata[8]_i_2_n_0\,
      O => \axil_rdata[8]_i_1_n_0\
    );
\axil_rdata[8]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"0000000000000008"
    )
        port map (
      I0 => axil_wdata(7),
      I1 => axil_wvalid,
      I2 => axil_awaddr(0),
      I3 => axil_awaddr(1),
      I4 => axil_awaddr(2),
      I5 => axil_awaddr(3),
      O => \axil_rdata[8]_i_2_n_0\
    );
\axil_rdata[9]_i_1\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFF40404540"
    )
        port map (
      I0 => axil_araddr(3),
      I1 => ref_clk_cntr_reg(9),
      I2 => axil_araddr(2),
      I3 => axil_wdata(8),
      I4 => \axil_rdata[31]_i_2_n_0\,
      I5 => \axil_rdata[9]_i_2_n_0\,
      O => axil_rdata_0(9)
    );
\axil_rdata[9]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FEEEFEFEFEEEEEEE"
    )
        port map (
      I0 => axil_araddr(1),
      I1 => axil_araddr(0),
      I2 => axil_araddr(3),
      I3 => test_clk1_cntr_synced(9),
      I4 => axil_araddr(2),
      I5 => test_clk0_cntr_synced(9),
      O => \axil_rdata[9]_i_2_n_0\
    );
\axil_rdata_reg[0]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => axil_rdata_0(0),
      Q => axil_rdata(0),
      R => '0'
    );
\axil_rdata_reg[10]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => axil_rdata_0(10),
      Q => axil_rdata(10),
      R => '0'
    );
\axil_rdata_reg[11]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => axil_rdata_0(11),
      Q => axil_rdata(11),
      R => '0'
    );
\axil_rdata_reg[12]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => axil_rdata_0(12),
      Q => axil_rdata(12),
      R => '0'
    );
\axil_rdata_reg[13]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => axil_rdata_0(13),
      Q => axil_rdata(13),
      R => '0'
    );
\axil_rdata_reg[14]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => \axil_rdata[14]_i_1_n_0\,
      Q => axil_rdata(14),
      R => \axil_rdata[29]_i_1_n_0\
    );
\axil_rdata_reg[15]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => axil_rdata_0(15),
      Q => axil_rdata(15),
      R => '0'
    );
\axil_rdata_reg[16]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => axil_rdata_0(16),
      Q => axil_rdata(16),
      R => '0'
    );
\axil_rdata_reg[17]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => \axil_rdata[17]_i_1_n_0\,
      Q => axil_rdata(17),
      R => \axil_rdata[29]_i_1_n_0\
    );
\axil_rdata_reg[18]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => axil_rdata_0(18),
      Q => axil_rdata(18),
      R => '0'
    );
\axil_rdata_reg[19]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => axil_rdata_0(19),
      Q => axil_rdata(19),
      R => '0'
    );
\axil_rdata_reg[1]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => axil_rdata_0(1),
      Q => axil_rdata(1),
      R => '0'
    );
\axil_rdata_reg[20]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => \axil_rdata[20]_i_1_n_0\,
      Q => axil_rdata(20),
      R => \axil_rdata[29]_i_1_n_0\
    );
\axil_rdata_reg[21]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => axil_rdata_0(21),
      Q => axil_rdata(21),
      R => '0'
    );
\axil_rdata_reg[22]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => \axil_rdata[22]_i_1_n_0\,
      Q => axil_rdata(22),
      R => \axil_rdata[29]_i_1_n_0\
    );
\axil_rdata_reg[23]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => axil_rdata_0(23),
      Q => axil_rdata(23),
      R => '0'
    );
\axil_rdata_reg[24]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => \axil_rdata[24]_i_1_n_0\,
      Q => axil_rdata(24),
      R => \axil_rdata[29]_i_1_n_0\
    );
\axil_rdata_reg[25]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => axil_rdata_0(25),
      Q => axil_rdata(25),
      R => '0'
    );
\axil_rdata_reg[26]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => axil_rdata_0(26),
      Q => axil_rdata(26),
      R => '0'
    );
\axil_rdata_reg[27]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => axil_rdata_0(27),
      Q => axil_rdata(27),
      R => '0'
    );
\axil_rdata_reg[28]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => axil_rdata_0(28),
      Q => axil_rdata(28),
      R => '0'
    );
\axil_rdata_reg[29]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => \axil_rdata[29]_i_2_n_0\,
      Q => axil_rdata(29),
      R => \axil_rdata[29]_i_1_n_0\
    );
\axil_rdata_reg[2]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => axil_rdata_0(2),
      Q => axil_rdata(2),
      R => '0'
    );
\axil_rdata_reg[30]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => axil_rdata_0(30),
      Q => axil_rdata(30),
      R => '0'
    );
\axil_rdata_reg[31]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => axil_rdata_0(31),
      Q => axil_rdata(31),
      R => '0'
    );
\axil_rdata_reg[3]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => axil_rdata_0(3),
      Q => axil_rdata(3),
      R => '0'
    );
\axil_rdata_reg[4]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => \axil_rdata[4]_i_1_n_0\,
      Q => axil_rdata(4),
      R => \axil_rdata[29]_i_1_n_0\
    );
\axil_rdata_reg[5]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => axil_rdata_0(5),
      Q => axil_rdata(5),
      R => '0'
    );
\axil_rdata_reg[6]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => axil_rdata_0(6),
      Q => axil_rdata(6),
      R => '0'
    );
\axil_rdata_reg[7]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => axil_rdata_0(7),
      Q => axil_rdata(7),
      R => '0'
    );
\axil_rdata_reg[8]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => \axil_rdata[8]_i_1_n_0\,
      Q => axil_rdata(8),
      R => \axil_rdata[29]_i_1_n_0\
    );
\axil_rdata_reg[9]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => axil_arvalid,
      D => axil_rdata_0(9),
      Q => axil_rdata(9),
      R => '0'
    );
axil_rvalid_i_1: unisim.vcomponents.LUT6
    generic map(
      INIT => X"DDFFFFFF00800080"
    )
        port map (
      I0 => reset_n,
      I1 => state_read(0),
      I2 => axil_arvalid,
      I3 => state_read(1),
      I4 => axil_rready,
      I5 => \^axil_rvalid\,
      O => axil_rvalid_i_1_n_0
    );
axil_rvalid_reg: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => '1',
      D => axil_rvalid_i_1_n_0,
      Q => \^axil_rvalid\,
      R => '0'
    );
axil_wready_i_1: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFF7700002000"
    )
        port map (
      I0 => reset_n,
      I1 => state_write(2),
      I2 => axil_wvalid,
      I3 => state_write(1),
      I4 => state_write(0),
      I5 => \^axil_wready\,
      O => axil_wready_i_1_n_0
    );
axil_wready_reg: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => '1',
      D => axil_wready_i_1_n_0,
      Q => \^axil_wready\,
      R => '0'
    );
\clear_user_rst[0]_i_1\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"8"
    )
        port map (
      I0 => user_rst1_ack,
      I1 => user_rst0_ack,
      O => p_1_out(0)
    );
\clear_user_rst_reg[0]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => '1',
      D => p_1_out(0),
      Q => \clear_user_rst_reg_n_0_[0]\,
      R => '0'
    );
\clear_user_rst_reg[1]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => '1',
      D => \clear_user_rst_reg_n_0_[0]\,
      Q => p_1_in,
      R => '0'
    );
\ref_clk_cntr[0]_i_1\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"FFEF"
    )
        port map (
      I0 => xpm_cdc_array_single_inst2_i_3_n_0,
      I1 => xpm_cdc_array_single_inst2_i_2_n_0,
      I2 => \axil_rdata[1]_i_4_n_0\,
      I3 => \axil_rdata[1]_i_5_n_0\,
      O => sel
    );
\ref_clk_cntr[0]_i_3\: unisim.vcomponents.LUT1
    generic map(
      INIT => X"1"
    )
        port map (
      I0 => ref_clk_cntr_reg(0),
      O => \ref_clk_cntr[0]_i_3_n_0\
    );
\ref_clk_cntr_reg[0]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[0]_i_2_n_15\,
      Q => ref_clk_cntr_reg(0),
      R => src_in0
    );
\ref_clk_cntr_reg[0]_i_2\: unisim.vcomponents.CARRY8
     port map (
      CI => '0',
      CI_TOP => '0',
      CO(7) => \ref_clk_cntr_reg[0]_i_2_n_0\,
      CO(6) => \ref_clk_cntr_reg[0]_i_2_n_1\,
      CO(5) => \ref_clk_cntr_reg[0]_i_2_n_2\,
      CO(4) => \ref_clk_cntr_reg[0]_i_2_n_3\,
      CO(3) => \ref_clk_cntr_reg[0]_i_2_n_4\,
      CO(2) => \ref_clk_cntr_reg[0]_i_2_n_5\,
      CO(1) => \ref_clk_cntr_reg[0]_i_2_n_6\,
      CO(0) => \ref_clk_cntr_reg[0]_i_2_n_7\,
      DI(7 downto 0) => B"00000001",
      O(7) => \ref_clk_cntr_reg[0]_i_2_n_8\,
      O(6) => \ref_clk_cntr_reg[0]_i_2_n_9\,
      O(5) => \ref_clk_cntr_reg[0]_i_2_n_10\,
      O(4) => \ref_clk_cntr_reg[0]_i_2_n_11\,
      O(3) => \ref_clk_cntr_reg[0]_i_2_n_12\,
      O(2) => \ref_clk_cntr_reg[0]_i_2_n_13\,
      O(1) => \ref_clk_cntr_reg[0]_i_2_n_14\,
      O(0) => \ref_clk_cntr_reg[0]_i_2_n_15\,
      S(7 downto 1) => ref_clk_cntr_reg(7 downto 1),
      S(0) => \ref_clk_cntr[0]_i_3_n_0\
    );
\ref_clk_cntr_reg[10]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[8]_i_1_n_13\,
      Q => ref_clk_cntr_reg(10),
      R => src_in0
    );
\ref_clk_cntr_reg[11]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[8]_i_1_n_12\,
      Q => ref_clk_cntr_reg(11),
      R => src_in0
    );
\ref_clk_cntr_reg[12]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[8]_i_1_n_11\,
      Q => ref_clk_cntr_reg(12),
      R => src_in0
    );
\ref_clk_cntr_reg[13]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[8]_i_1_n_10\,
      Q => ref_clk_cntr_reg(13),
      R => src_in0
    );
\ref_clk_cntr_reg[14]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[8]_i_1_n_9\,
      Q => ref_clk_cntr_reg(14),
      R => src_in0
    );
\ref_clk_cntr_reg[15]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[8]_i_1_n_8\,
      Q => ref_clk_cntr_reg(15),
      R => src_in0
    );
\ref_clk_cntr_reg[16]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[16]_i_1_n_15\,
      Q => ref_clk_cntr_reg(16),
      R => src_in0
    );
\ref_clk_cntr_reg[16]_i_1\: unisim.vcomponents.CARRY8
     port map (
      CI => \ref_clk_cntr_reg[8]_i_1_n_0\,
      CI_TOP => '0',
      CO(7) => \ref_clk_cntr_reg[16]_i_1_n_0\,
      CO(6) => \ref_clk_cntr_reg[16]_i_1_n_1\,
      CO(5) => \ref_clk_cntr_reg[16]_i_1_n_2\,
      CO(4) => \ref_clk_cntr_reg[16]_i_1_n_3\,
      CO(3) => \ref_clk_cntr_reg[16]_i_1_n_4\,
      CO(2) => \ref_clk_cntr_reg[16]_i_1_n_5\,
      CO(1) => \ref_clk_cntr_reg[16]_i_1_n_6\,
      CO(0) => \ref_clk_cntr_reg[16]_i_1_n_7\,
      DI(7 downto 0) => B"00000000",
      O(7) => \ref_clk_cntr_reg[16]_i_1_n_8\,
      O(6) => \ref_clk_cntr_reg[16]_i_1_n_9\,
      O(5) => \ref_clk_cntr_reg[16]_i_1_n_10\,
      O(4) => \ref_clk_cntr_reg[16]_i_1_n_11\,
      O(3) => \ref_clk_cntr_reg[16]_i_1_n_12\,
      O(2) => \ref_clk_cntr_reg[16]_i_1_n_13\,
      O(1) => \ref_clk_cntr_reg[16]_i_1_n_14\,
      O(0) => \ref_clk_cntr_reg[16]_i_1_n_15\,
      S(7 downto 0) => ref_clk_cntr_reg(23 downto 16)
    );
\ref_clk_cntr_reg[17]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[16]_i_1_n_14\,
      Q => ref_clk_cntr_reg(17),
      R => src_in0
    );
\ref_clk_cntr_reg[18]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[16]_i_1_n_13\,
      Q => ref_clk_cntr_reg(18),
      R => src_in0
    );
\ref_clk_cntr_reg[19]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[16]_i_1_n_12\,
      Q => ref_clk_cntr_reg(19),
      R => src_in0
    );
\ref_clk_cntr_reg[1]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[0]_i_2_n_14\,
      Q => ref_clk_cntr_reg(1),
      R => src_in0
    );
\ref_clk_cntr_reg[20]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[16]_i_1_n_11\,
      Q => ref_clk_cntr_reg(20),
      R => src_in0
    );
\ref_clk_cntr_reg[21]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[16]_i_1_n_10\,
      Q => ref_clk_cntr_reg(21),
      R => src_in0
    );
\ref_clk_cntr_reg[22]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[16]_i_1_n_9\,
      Q => ref_clk_cntr_reg(22),
      R => src_in0
    );
\ref_clk_cntr_reg[23]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[16]_i_1_n_8\,
      Q => ref_clk_cntr_reg(23),
      R => src_in0
    );
\ref_clk_cntr_reg[24]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[24]_i_1_n_15\,
      Q => ref_clk_cntr_reg(24),
      R => src_in0
    );
\ref_clk_cntr_reg[24]_i_1\: unisim.vcomponents.CARRY8
     port map (
      CI => \ref_clk_cntr_reg[16]_i_1_n_0\,
      CI_TOP => '0',
      CO(7) => \NLW_ref_clk_cntr_reg[24]_i_1_CO_UNCONNECTED\(7),
      CO(6) => \ref_clk_cntr_reg[24]_i_1_n_1\,
      CO(5) => \ref_clk_cntr_reg[24]_i_1_n_2\,
      CO(4) => \ref_clk_cntr_reg[24]_i_1_n_3\,
      CO(3) => \ref_clk_cntr_reg[24]_i_1_n_4\,
      CO(2) => \ref_clk_cntr_reg[24]_i_1_n_5\,
      CO(1) => \ref_clk_cntr_reg[24]_i_1_n_6\,
      CO(0) => \ref_clk_cntr_reg[24]_i_1_n_7\,
      DI(7 downto 0) => B"00000000",
      O(7) => \ref_clk_cntr_reg[24]_i_1_n_8\,
      O(6) => \ref_clk_cntr_reg[24]_i_1_n_9\,
      O(5) => \ref_clk_cntr_reg[24]_i_1_n_10\,
      O(4) => \ref_clk_cntr_reg[24]_i_1_n_11\,
      O(3) => \ref_clk_cntr_reg[24]_i_1_n_12\,
      O(2) => \ref_clk_cntr_reg[24]_i_1_n_13\,
      O(1) => \ref_clk_cntr_reg[24]_i_1_n_14\,
      O(0) => \ref_clk_cntr_reg[24]_i_1_n_15\,
      S(7 downto 0) => ref_clk_cntr_reg(31 downto 24)
    );
\ref_clk_cntr_reg[25]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[24]_i_1_n_14\,
      Q => ref_clk_cntr_reg(25),
      R => src_in0
    );
\ref_clk_cntr_reg[26]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[24]_i_1_n_13\,
      Q => ref_clk_cntr_reg(26),
      R => src_in0
    );
\ref_clk_cntr_reg[27]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[24]_i_1_n_12\,
      Q => ref_clk_cntr_reg(27),
      R => src_in0
    );
\ref_clk_cntr_reg[28]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[24]_i_1_n_11\,
      Q => ref_clk_cntr_reg(28),
      R => src_in0
    );
\ref_clk_cntr_reg[29]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[24]_i_1_n_10\,
      Q => ref_clk_cntr_reg(29),
      R => src_in0
    );
\ref_clk_cntr_reg[2]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[0]_i_2_n_13\,
      Q => ref_clk_cntr_reg(2),
      R => src_in0
    );
\ref_clk_cntr_reg[30]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[24]_i_1_n_9\,
      Q => ref_clk_cntr_reg(30),
      R => src_in0
    );
\ref_clk_cntr_reg[31]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[24]_i_1_n_8\,
      Q => ref_clk_cntr_reg(31),
      R => src_in0
    );
\ref_clk_cntr_reg[3]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[0]_i_2_n_12\,
      Q => ref_clk_cntr_reg(3),
      R => src_in0
    );
\ref_clk_cntr_reg[4]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[0]_i_2_n_11\,
      Q => ref_clk_cntr_reg(4),
      R => src_in0
    );
\ref_clk_cntr_reg[5]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[0]_i_2_n_10\,
      Q => ref_clk_cntr_reg(5),
      R => src_in0
    );
\ref_clk_cntr_reg[6]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[0]_i_2_n_9\,
      Q => ref_clk_cntr_reg(6),
      R => src_in0
    );
\ref_clk_cntr_reg[7]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[0]_i_2_n_8\,
      Q => ref_clk_cntr_reg(7),
      R => src_in0
    );
\ref_clk_cntr_reg[8]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[8]_i_1_n_15\,
      Q => ref_clk_cntr_reg(8),
      R => src_in0
    );
\ref_clk_cntr_reg[8]_i_1\: unisim.vcomponents.CARRY8
     port map (
      CI => \ref_clk_cntr_reg[0]_i_2_n_0\,
      CI_TOP => '0',
      CO(7) => \ref_clk_cntr_reg[8]_i_1_n_0\,
      CO(6) => \ref_clk_cntr_reg[8]_i_1_n_1\,
      CO(5) => \ref_clk_cntr_reg[8]_i_1_n_2\,
      CO(4) => \ref_clk_cntr_reg[8]_i_1_n_3\,
      CO(3) => \ref_clk_cntr_reg[8]_i_1_n_4\,
      CO(2) => \ref_clk_cntr_reg[8]_i_1_n_5\,
      CO(1) => \ref_clk_cntr_reg[8]_i_1_n_6\,
      CO(0) => \ref_clk_cntr_reg[8]_i_1_n_7\,
      DI(7 downto 0) => B"00000000",
      O(7) => \ref_clk_cntr_reg[8]_i_1_n_8\,
      O(6) => \ref_clk_cntr_reg[8]_i_1_n_9\,
      O(5) => \ref_clk_cntr_reg[8]_i_1_n_10\,
      O(4) => \ref_clk_cntr_reg[8]_i_1_n_11\,
      O(3) => \ref_clk_cntr_reg[8]_i_1_n_12\,
      O(2) => \ref_clk_cntr_reg[8]_i_1_n_13\,
      O(1) => \ref_clk_cntr_reg[8]_i_1_n_14\,
      O(0) => \ref_clk_cntr_reg[8]_i_1_n_15\,
      S(7 downto 0) => ref_clk_cntr_reg(15 downto 8)
    );
\ref_clk_cntr_reg[9]\: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => sel,
      D => \ref_clk_cntr_reg[8]_i_1_n_14\,
      Q => ref_clk_cntr_reg(9),
      R => src_in0
    );
\state_read[1]_i_1\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"5808"
    )
        port map (
      I0 => state_read(0),
      I1 => axil_arvalid,
      I2 => state_read(1),
      I3 => axil_rready,
      O => \state_read[1]_i_1_n_0\
    );
\state_read[1]_i_2\: unisim.vcomponents.LUT1
    generic map(
      INIT => X"1"
    )
        port map (
      I0 => state_read(1),
      O => \p_1_in__1\(1)
    );
\state_read_reg[0]\: unisim.vcomponents.FDSE
    generic map(
      INIT => '1'
    )
        port map (
      C => clk,
      CE => \state_read[1]_i_1_n_0\,
      D => state_read(1),
      Q => state_read(0),
      S => src_in1
    );
\state_read_reg[1]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => \state_read[1]_i_1_n_0\,
      D => \p_1_in__1\(1),
      Q => state_read(1),
      R => src_in1
    );
\state_write[1]_i_1\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"1"
    )
        port map (
      I0 => state_write(2),
      I1 => state_write(1),
      O => \p_1_in__0\(1)
    );
\state_write[2]_i_1\: unisim.vcomponents.LUT1
    generic map(
      INIT => X"1"
    )
        port map (
      I0 => reset_n,
      O => src_in1
    );
\state_write[2]_i_2\: unisim.vcomponents.LUT6
    generic map(
      INIT => X"000A000A0CF00C00"
    )
        port map (
      I0 => axil_bready,
      I1 => axil_wvalid,
      I2 => state_write(0),
      I3 => state_write(1),
      I4 => axil_awvalid,
      I5 => state_write(2),
      O => \state_write[2]_i_2_n_0\
    );
\state_write_reg[0]\: unisim.vcomponents.FDSE
    generic map(
      INIT => '1'
    )
        port map (
      C => clk,
      CE => \state_write[2]_i_2_n_0\,
      D => state_write(2),
      Q => state_write(0),
      S => src_in1
    );
\state_write_reg[1]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => \state_write[2]_i_2_n_0\,
      D => \p_1_in__0\(1),
      Q => state_write(1),
      R => src_in1
    );
\state_write_reg[2]\: unisim.vcomponents.FDRE
    generic map(
      INIT => '0'
    )
        port map (
      C => clk,
      CE => \state_write[2]_i_2_n_0\,
      D => state_write(1),
      Q => state_write(2),
      R => src_in1
    );
\test_clk0_cntr[0]_i_1\: unisim.vcomponents.LUT1
    generic map(
      INIT => X"1"
    )
        port map (
      I0 => done0_synced,
      O => \test_clk0_cntr[0]_i_1_n_0\
    );
\test_clk0_cntr[0]_i_3\: unisim.vcomponents.LUT1
    generic map(
      INIT => X"1"
    )
        port map (
      I0 => test_clk0_cntr_reg(0),
      O => \test_clk0_cntr[0]_i_3_n_0\
    );
\test_clk0_cntr_reg[0]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[0]_i_2_n_15\,
      Q => test_clk0_cntr_reg(0),
      R => rst0_synced
    );
\test_clk0_cntr_reg[0]_i_2\: unisim.vcomponents.CARRY8
     port map (
      CI => '0',
      CI_TOP => '0',
      CO(7) => \test_clk0_cntr_reg[0]_i_2_n_0\,
      CO(6) => \test_clk0_cntr_reg[0]_i_2_n_1\,
      CO(5) => \test_clk0_cntr_reg[0]_i_2_n_2\,
      CO(4) => \test_clk0_cntr_reg[0]_i_2_n_3\,
      CO(3) => \test_clk0_cntr_reg[0]_i_2_n_4\,
      CO(2) => \test_clk0_cntr_reg[0]_i_2_n_5\,
      CO(1) => \test_clk0_cntr_reg[0]_i_2_n_6\,
      CO(0) => \test_clk0_cntr_reg[0]_i_2_n_7\,
      DI(7 downto 0) => B"00000001",
      O(7) => \test_clk0_cntr_reg[0]_i_2_n_8\,
      O(6) => \test_clk0_cntr_reg[0]_i_2_n_9\,
      O(5) => \test_clk0_cntr_reg[0]_i_2_n_10\,
      O(4) => \test_clk0_cntr_reg[0]_i_2_n_11\,
      O(3) => \test_clk0_cntr_reg[0]_i_2_n_12\,
      O(2) => \test_clk0_cntr_reg[0]_i_2_n_13\,
      O(1) => \test_clk0_cntr_reg[0]_i_2_n_14\,
      O(0) => \test_clk0_cntr_reg[0]_i_2_n_15\,
      S(7 downto 1) => test_clk0_cntr_reg(7 downto 1),
      S(0) => \test_clk0_cntr[0]_i_3_n_0\
    );
\test_clk0_cntr_reg[10]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[8]_i_1_n_13\,
      Q => test_clk0_cntr_reg(10),
      R => rst0_synced
    );
\test_clk0_cntr_reg[11]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[8]_i_1_n_12\,
      Q => test_clk0_cntr_reg(11),
      R => rst0_synced
    );
\test_clk0_cntr_reg[12]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[8]_i_1_n_11\,
      Q => test_clk0_cntr_reg(12),
      R => rst0_synced
    );
\test_clk0_cntr_reg[13]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[8]_i_1_n_10\,
      Q => test_clk0_cntr_reg(13),
      R => rst0_synced
    );
\test_clk0_cntr_reg[14]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[8]_i_1_n_9\,
      Q => test_clk0_cntr_reg(14),
      R => rst0_synced
    );
\test_clk0_cntr_reg[15]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[8]_i_1_n_8\,
      Q => test_clk0_cntr_reg(15),
      R => rst0_synced
    );
\test_clk0_cntr_reg[16]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[16]_i_1_n_15\,
      Q => test_clk0_cntr_reg(16),
      R => rst0_synced
    );
\test_clk0_cntr_reg[16]_i_1\: unisim.vcomponents.CARRY8
     port map (
      CI => \test_clk0_cntr_reg[8]_i_1_n_0\,
      CI_TOP => '0',
      CO(7) => \test_clk0_cntr_reg[16]_i_1_n_0\,
      CO(6) => \test_clk0_cntr_reg[16]_i_1_n_1\,
      CO(5) => \test_clk0_cntr_reg[16]_i_1_n_2\,
      CO(4) => \test_clk0_cntr_reg[16]_i_1_n_3\,
      CO(3) => \test_clk0_cntr_reg[16]_i_1_n_4\,
      CO(2) => \test_clk0_cntr_reg[16]_i_1_n_5\,
      CO(1) => \test_clk0_cntr_reg[16]_i_1_n_6\,
      CO(0) => \test_clk0_cntr_reg[16]_i_1_n_7\,
      DI(7 downto 0) => B"00000000",
      O(7) => \test_clk0_cntr_reg[16]_i_1_n_8\,
      O(6) => \test_clk0_cntr_reg[16]_i_1_n_9\,
      O(5) => \test_clk0_cntr_reg[16]_i_1_n_10\,
      O(4) => \test_clk0_cntr_reg[16]_i_1_n_11\,
      O(3) => \test_clk0_cntr_reg[16]_i_1_n_12\,
      O(2) => \test_clk0_cntr_reg[16]_i_1_n_13\,
      O(1) => \test_clk0_cntr_reg[16]_i_1_n_14\,
      O(0) => \test_clk0_cntr_reg[16]_i_1_n_15\,
      S(7 downto 0) => test_clk0_cntr_reg(23 downto 16)
    );
\test_clk0_cntr_reg[17]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[16]_i_1_n_14\,
      Q => test_clk0_cntr_reg(17),
      R => rst0_synced
    );
\test_clk0_cntr_reg[18]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[16]_i_1_n_13\,
      Q => test_clk0_cntr_reg(18),
      R => rst0_synced
    );
\test_clk0_cntr_reg[19]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[16]_i_1_n_12\,
      Q => test_clk0_cntr_reg(19),
      R => rst0_synced
    );
\test_clk0_cntr_reg[1]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[0]_i_2_n_14\,
      Q => test_clk0_cntr_reg(1),
      R => rst0_synced
    );
\test_clk0_cntr_reg[20]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[16]_i_1_n_11\,
      Q => test_clk0_cntr_reg(20),
      R => rst0_synced
    );
\test_clk0_cntr_reg[21]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[16]_i_1_n_10\,
      Q => test_clk0_cntr_reg(21),
      R => rst0_synced
    );
\test_clk0_cntr_reg[22]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[16]_i_1_n_9\,
      Q => test_clk0_cntr_reg(22),
      R => rst0_synced
    );
\test_clk0_cntr_reg[23]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[16]_i_1_n_8\,
      Q => test_clk0_cntr_reg(23),
      R => rst0_synced
    );
\test_clk0_cntr_reg[24]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[24]_i_1_n_15\,
      Q => test_clk0_cntr_reg(24),
      R => rst0_synced
    );
\test_clk0_cntr_reg[24]_i_1\: unisim.vcomponents.CARRY8
     port map (
      CI => \test_clk0_cntr_reg[16]_i_1_n_0\,
      CI_TOP => '0',
      CO(7) => \NLW_test_clk0_cntr_reg[24]_i_1_CO_UNCONNECTED\(7),
      CO(6) => \test_clk0_cntr_reg[24]_i_1_n_1\,
      CO(5) => \test_clk0_cntr_reg[24]_i_1_n_2\,
      CO(4) => \test_clk0_cntr_reg[24]_i_1_n_3\,
      CO(3) => \test_clk0_cntr_reg[24]_i_1_n_4\,
      CO(2) => \test_clk0_cntr_reg[24]_i_1_n_5\,
      CO(1) => \test_clk0_cntr_reg[24]_i_1_n_6\,
      CO(0) => \test_clk0_cntr_reg[24]_i_1_n_7\,
      DI(7 downto 0) => B"00000000",
      O(7) => \test_clk0_cntr_reg[24]_i_1_n_8\,
      O(6) => \test_clk0_cntr_reg[24]_i_1_n_9\,
      O(5) => \test_clk0_cntr_reg[24]_i_1_n_10\,
      O(4) => \test_clk0_cntr_reg[24]_i_1_n_11\,
      O(3) => \test_clk0_cntr_reg[24]_i_1_n_12\,
      O(2) => \test_clk0_cntr_reg[24]_i_1_n_13\,
      O(1) => \test_clk0_cntr_reg[24]_i_1_n_14\,
      O(0) => \test_clk0_cntr_reg[24]_i_1_n_15\,
      S(7 downto 0) => test_clk0_cntr_reg(31 downto 24)
    );
\test_clk0_cntr_reg[25]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[24]_i_1_n_14\,
      Q => test_clk0_cntr_reg(25),
      R => rst0_synced
    );
\test_clk0_cntr_reg[26]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[24]_i_1_n_13\,
      Q => test_clk0_cntr_reg(26),
      R => rst0_synced
    );
\test_clk0_cntr_reg[27]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[24]_i_1_n_12\,
      Q => test_clk0_cntr_reg(27),
      R => rst0_synced
    );
\test_clk0_cntr_reg[28]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[24]_i_1_n_11\,
      Q => test_clk0_cntr_reg(28),
      R => rst0_synced
    );
\test_clk0_cntr_reg[29]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[24]_i_1_n_10\,
      Q => test_clk0_cntr_reg(29),
      R => rst0_synced
    );
\test_clk0_cntr_reg[2]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[0]_i_2_n_13\,
      Q => test_clk0_cntr_reg(2),
      R => rst0_synced
    );
\test_clk0_cntr_reg[30]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[24]_i_1_n_9\,
      Q => test_clk0_cntr_reg(30),
      R => rst0_synced
    );
\test_clk0_cntr_reg[31]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[24]_i_1_n_8\,
      Q => test_clk0_cntr_reg(31),
      R => rst0_synced
    );
\test_clk0_cntr_reg[3]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[0]_i_2_n_12\,
      Q => test_clk0_cntr_reg(3),
      R => rst0_synced
    );
\test_clk0_cntr_reg[4]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[0]_i_2_n_11\,
      Q => test_clk0_cntr_reg(4),
      R => rst0_synced
    );
\test_clk0_cntr_reg[5]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[0]_i_2_n_10\,
      Q => test_clk0_cntr_reg(5),
      R => rst0_synced
    );
\test_clk0_cntr_reg[6]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[0]_i_2_n_9\,
      Q => test_clk0_cntr_reg(6),
      R => rst0_synced
    );
\test_clk0_cntr_reg[7]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[0]_i_2_n_8\,
      Q => test_clk0_cntr_reg(7),
      R => rst0_synced
    );
\test_clk0_cntr_reg[8]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[8]_i_1_n_15\,
      Q => test_clk0_cntr_reg(8),
      R => rst0_synced
    );
\test_clk0_cntr_reg[8]_i_1\: unisim.vcomponents.CARRY8
     port map (
      CI => \test_clk0_cntr_reg[0]_i_2_n_0\,
      CI_TOP => '0',
      CO(7) => \test_clk0_cntr_reg[8]_i_1_n_0\,
      CO(6) => \test_clk0_cntr_reg[8]_i_1_n_1\,
      CO(5) => \test_clk0_cntr_reg[8]_i_1_n_2\,
      CO(4) => \test_clk0_cntr_reg[8]_i_1_n_3\,
      CO(3) => \test_clk0_cntr_reg[8]_i_1_n_4\,
      CO(2) => \test_clk0_cntr_reg[8]_i_1_n_5\,
      CO(1) => \test_clk0_cntr_reg[8]_i_1_n_6\,
      CO(0) => \test_clk0_cntr_reg[8]_i_1_n_7\,
      DI(7 downto 0) => B"00000000",
      O(7) => \test_clk0_cntr_reg[8]_i_1_n_8\,
      O(6) => \test_clk0_cntr_reg[8]_i_1_n_9\,
      O(5) => \test_clk0_cntr_reg[8]_i_1_n_10\,
      O(4) => \test_clk0_cntr_reg[8]_i_1_n_11\,
      O(3) => \test_clk0_cntr_reg[8]_i_1_n_12\,
      O(2) => \test_clk0_cntr_reg[8]_i_1_n_13\,
      O(1) => \test_clk0_cntr_reg[8]_i_1_n_14\,
      O(0) => \test_clk0_cntr_reg[8]_i_1_n_15\,
      S(7 downto 0) => test_clk0_cntr_reg(15 downto 8)
    );
\test_clk0_cntr_reg[9]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk0,
      CE => \test_clk0_cntr[0]_i_1_n_0\,
      D => \test_clk0_cntr_reg[8]_i_1_n_14\,
      Q => test_clk0_cntr_reg(9),
      R => rst0_synced
    );
\test_clk1_cntr[0]_i_1\: unisim.vcomponents.LUT1
    generic map(
      INIT => X"1"
    )
        port map (
      I0 => done1_synced,
      O => \test_clk1_cntr[0]_i_1_n_0\
    );
\test_clk1_cntr[0]_i_3\: unisim.vcomponents.LUT1
    generic map(
      INIT => X"1"
    )
        port map (
      I0 => test_clk1_cntr_reg(0),
      O => \test_clk1_cntr[0]_i_3_n_0\
    );
\test_clk1_cntr_reg[0]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[0]_i_2_n_15\,
      Q => test_clk1_cntr_reg(0),
      R => rst1_synced
    );
\test_clk1_cntr_reg[0]_i_2\: unisim.vcomponents.CARRY8
     port map (
      CI => '0',
      CI_TOP => '0',
      CO(7) => \test_clk1_cntr_reg[0]_i_2_n_0\,
      CO(6) => \test_clk1_cntr_reg[0]_i_2_n_1\,
      CO(5) => \test_clk1_cntr_reg[0]_i_2_n_2\,
      CO(4) => \test_clk1_cntr_reg[0]_i_2_n_3\,
      CO(3) => \test_clk1_cntr_reg[0]_i_2_n_4\,
      CO(2) => \test_clk1_cntr_reg[0]_i_2_n_5\,
      CO(1) => \test_clk1_cntr_reg[0]_i_2_n_6\,
      CO(0) => \test_clk1_cntr_reg[0]_i_2_n_7\,
      DI(7 downto 0) => B"00000001",
      O(7) => \test_clk1_cntr_reg[0]_i_2_n_8\,
      O(6) => \test_clk1_cntr_reg[0]_i_2_n_9\,
      O(5) => \test_clk1_cntr_reg[0]_i_2_n_10\,
      O(4) => \test_clk1_cntr_reg[0]_i_2_n_11\,
      O(3) => \test_clk1_cntr_reg[0]_i_2_n_12\,
      O(2) => \test_clk1_cntr_reg[0]_i_2_n_13\,
      O(1) => \test_clk1_cntr_reg[0]_i_2_n_14\,
      O(0) => \test_clk1_cntr_reg[0]_i_2_n_15\,
      S(7 downto 1) => test_clk1_cntr_reg(7 downto 1),
      S(0) => \test_clk1_cntr[0]_i_3_n_0\
    );
\test_clk1_cntr_reg[10]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[8]_i_1_n_13\,
      Q => test_clk1_cntr_reg(10),
      R => rst1_synced
    );
\test_clk1_cntr_reg[11]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[8]_i_1_n_12\,
      Q => test_clk1_cntr_reg(11),
      R => rst1_synced
    );
\test_clk1_cntr_reg[12]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[8]_i_1_n_11\,
      Q => test_clk1_cntr_reg(12),
      R => rst1_synced
    );
\test_clk1_cntr_reg[13]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[8]_i_1_n_10\,
      Q => test_clk1_cntr_reg(13),
      R => rst1_synced
    );
\test_clk1_cntr_reg[14]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[8]_i_1_n_9\,
      Q => test_clk1_cntr_reg(14),
      R => rst1_synced
    );
\test_clk1_cntr_reg[15]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[8]_i_1_n_8\,
      Q => test_clk1_cntr_reg(15),
      R => rst1_synced
    );
\test_clk1_cntr_reg[16]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[16]_i_1_n_15\,
      Q => test_clk1_cntr_reg(16),
      R => rst1_synced
    );
\test_clk1_cntr_reg[16]_i_1\: unisim.vcomponents.CARRY8
     port map (
      CI => \test_clk1_cntr_reg[8]_i_1_n_0\,
      CI_TOP => '0',
      CO(7) => \test_clk1_cntr_reg[16]_i_1_n_0\,
      CO(6) => \test_clk1_cntr_reg[16]_i_1_n_1\,
      CO(5) => \test_clk1_cntr_reg[16]_i_1_n_2\,
      CO(4) => \test_clk1_cntr_reg[16]_i_1_n_3\,
      CO(3) => \test_clk1_cntr_reg[16]_i_1_n_4\,
      CO(2) => \test_clk1_cntr_reg[16]_i_1_n_5\,
      CO(1) => \test_clk1_cntr_reg[16]_i_1_n_6\,
      CO(0) => \test_clk1_cntr_reg[16]_i_1_n_7\,
      DI(7 downto 0) => B"00000000",
      O(7) => \test_clk1_cntr_reg[16]_i_1_n_8\,
      O(6) => \test_clk1_cntr_reg[16]_i_1_n_9\,
      O(5) => \test_clk1_cntr_reg[16]_i_1_n_10\,
      O(4) => \test_clk1_cntr_reg[16]_i_1_n_11\,
      O(3) => \test_clk1_cntr_reg[16]_i_1_n_12\,
      O(2) => \test_clk1_cntr_reg[16]_i_1_n_13\,
      O(1) => \test_clk1_cntr_reg[16]_i_1_n_14\,
      O(0) => \test_clk1_cntr_reg[16]_i_1_n_15\,
      S(7 downto 0) => test_clk1_cntr_reg(23 downto 16)
    );
\test_clk1_cntr_reg[17]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[16]_i_1_n_14\,
      Q => test_clk1_cntr_reg(17),
      R => rst1_synced
    );
\test_clk1_cntr_reg[18]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[16]_i_1_n_13\,
      Q => test_clk1_cntr_reg(18),
      R => rst1_synced
    );
\test_clk1_cntr_reg[19]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[16]_i_1_n_12\,
      Q => test_clk1_cntr_reg(19),
      R => rst1_synced
    );
\test_clk1_cntr_reg[1]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[0]_i_2_n_14\,
      Q => test_clk1_cntr_reg(1),
      R => rst1_synced
    );
\test_clk1_cntr_reg[20]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[16]_i_1_n_11\,
      Q => test_clk1_cntr_reg(20),
      R => rst1_synced
    );
\test_clk1_cntr_reg[21]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[16]_i_1_n_10\,
      Q => test_clk1_cntr_reg(21),
      R => rst1_synced
    );
\test_clk1_cntr_reg[22]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[16]_i_1_n_9\,
      Q => test_clk1_cntr_reg(22),
      R => rst1_synced
    );
\test_clk1_cntr_reg[23]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[16]_i_1_n_8\,
      Q => test_clk1_cntr_reg(23),
      R => rst1_synced
    );
\test_clk1_cntr_reg[24]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[24]_i_1_n_15\,
      Q => test_clk1_cntr_reg(24),
      R => rst1_synced
    );
\test_clk1_cntr_reg[24]_i_1\: unisim.vcomponents.CARRY8
     port map (
      CI => \test_clk1_cntr_reg[16]_i_1_n_0\,
      CI_TOP => '0',
      CO(7) => \NLW_test_clk1_cntr_reg[24]_i_1_CO_UNCONNECTED\(7),
      CO(6) => \test_clk1_cntr_reg[24]_i_1_n_1\,
      CO(5) => \test_clk1_cntr_reg[24]_i_1_n_2\,
      CO(4) => \test_clk1_cntr_reg[24]_i_1_n_3\,
      CO(3) => \test_clk1_cntr_reg[24]_i_1_n_4\,
      CO(2) => \test_clk1_cntr_reg[24]_i_1_n_5\,
      CO(1) => \test_clk1_cntr_reg[24]_i_1_n_6\,
      CO(0) => \test_clk1_cntr_reg[24]_i_1_n_7\,
      DI(7 downto 0) => B"00000000",
      O(7) => \test_clk1_cntr_reg[24]_i_1_n_8\,
      O(6) => \test_clk1_cntr_reg[24]_i_1_n_9\,
      O(5) => \test_clk1_cntr_reg[24]_i_1_n_10\,
      O(4) => \test_clk1_cntr_reg[24]_i_1_n_11\,
      O(3) => \test_clk1_cntr_reg[24]_i_1_n_12\,
      O(2) => \test_clk1_cntr_reg[24]_i_1_n_13\,
      O(1) => \test_clk1_cntr_reg[24]_i_1_n_14\,
      O(0) => \test_clk1_cntr_reg[24]_i_1_n_15\,
      S(7 downto 0) => test_clk1_cntr_reg(31 downto 24)
    );
\test_clk1_cntr_reg[25]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[24]_i_1_n_14\,
      Q => test_clk1_cntr_reg(25),
      R => rst1_synced
    );
\test_clk1_cntr_reg[26]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[24]_i_1_n_13\,
      Q => test_clk1_cntr_reg(26),
      R => rst1_synced
    );
\test_clk1_cntr_reg[27]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[24]_i_1_n_12\,
      Q => test_clk1_cntr_reg(27),
      R => rst1_synced
    );
\test_clk1_cntr_reg[28]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[24]_i_1_n_11\,
      Q => test_clk1_cntr_reg(28),
      R => rst1_synced
    );
\test_clk1_cntr_reg[29]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[24]_i_1_n_10\,
      Q => test_clk1_cntr_reg(29),
      R => rst1_synced
    );
\test_clk1_cntr_reg[2]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[0]_i_2_n_13\,
      Q => test_clk1_cntr_reg(2),
      R => rst1_synced
    );
\test_clk1_cntr_reg[30]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[24]_i_1_n_9\,
      Q => test_clk1_cntr_reg(30),
      R => rst1_synced
    );
\test_clk1_cntr_reg[31]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[24]_i_1_n_8\,
      Q => test_clk1_cntr_reg(31),
      R => rst1_synced
    );
\test_clk1_cntr_reg[3]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[0]_i_2_n_12\,
      Q => test_clk1_cntr_reg(3),
      R => rst1_synced
    );
\test_clk1_cntr_reg[4]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[0]_i_2_n_11\,
      Q => test_clk1_cntr_reg(4),
      R => rst1_synced
    );
\test_clk1_cntr_reg[5]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[0]_i_2_n_10\,
      Q => test_clk1_cntr_reg(5),
      R => rst1_synced
    );
\test_clk1_cntr_reg[6]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[0]_i_2_n_9\,
      Q => test_clk1_cntr_reg(6),
      R => rst1_synced
    );
\test_clk1_cntr_reg[7]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[0]_i_2_n_8\,
      Q => test_clk1_cntr_reg(7),
      R => rst1_synced
    );
\test_clk1_cntr_reg[8]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[8]_i_1_n_15\,
      Q => test_clk1_cntr_reg(8),
      R => rst1_synced
    );
\test_clk1_cntr_reg[8]_i_1\: unisim.vcomponents.CARRY8
     port map (
      CI => \test_clk1_cntr_reg[0]_i_2_n_0\,
      CI_TOP => '0',
      CO(7) => \test_clk1_cntr_reg[8]_i_1_n_0\,
      CO(6) => \test_clk1_cntr_reg[8]_i_1_n_1\,
      CO(5) => \test_clk1_cntr_reg[8]_i_1_n_2\,
      CO(4) => \test_clk1_cntr_reg[8]_i_1_n_3\,
      CO(3) => \test_clk1_cntr_reg[8]_i_1_n_4\,
      CO(2) => \test_clk1_cntr_reg[8]_i_1_n_5\,
      CO(1) => \test_clk1_cntr_reg[8]_i_1_n_6\,
      CO(0) => \test_clk1_cntr_reg[8]_i_1_n_7\,
      DI(7 downto 0) => B"00000000",
      O(7) => \test_clk1_cntr_reg[8]_i_1_n_8\,
      O(6) => \test_clk1_cntr_reg[8]_i_1_n_9\,
      O(5) => \test_clk1_cntr_reg[8]_i_1_n_10\,
      O(4) => \test_clk1_cntr_reg[8]_i_1_n_11\,
      O(3) => \test_clk1_cntr_reg[8]_i_1_n_12\,
      O(2) => \test_clk1_cntr_reg[8]_i_1_n_13\,
      O(1) => \test_clk1_cntr_reg[8]_i_1_n_14\,
      O(0) => \test_clk1_cntr_reg[8]_i_1_n_15\,
      S(7 downto 0) => test_clk1_cntr_reg(15 downto 8)
    );
\test_clk1_cntr_reg[9]\: unisim.vcomponents.FDRE
     port map (
      C => test_clk1,
      CE => \test_clk1_cntr[0]_i_1_n_0\,
      D => \test_clk1_cntr_reg[8]_i_1_n_14\,
      Q => test_clk1_cntr_reg(9),
      R => rst1_synced
    );
user_rst_i_1: unisim.vcomponents.LUT5
    generic map(
      INIT => X"3A330A00"
    )
        port map (
      I0 => axil_wdata(0),
      I1 => p_1_in,
      I2 => \axil_rdata[31]_i_2_n_0\,
      I3 => \^axil_wready\,
      I4 => user_rst_reg_n_0,
      O => user_rst_i_1_n_0
    );
user_rst_reg: unisim.vcomponents.FDRE
     port map (
      C => clk,
      CE => '1',
      D => user_rst_i_1_n_0,
      Q => user_rst_reg_n_0,
      R => '0'
    );
xpm_cdc_array_single_inst: entity work.\decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__6\
     port map (
      dest_clk => test_clk0,
      dest_out(0) => rst0_synced,
      src_clk => '0',
      src_in(0) => src_in0
    );
xpm_cdc_array_single_inst1: entity work.\decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__8\
     port map (
      dest_clk => clk,
      dest_out(0) => user_rst0_ack,
      src_clk => '0',
      src_in(0) => rst0_synced
    );
xpm_cdc_array_single_inst2: entity work.\decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__10\
     port map (
      dest_clk => test_clk0,
      dest_out(0) => done0_synced,
      src_clk => '0',
      src_in(0) => done
    );
xpm_cdc_array_single_inst2_i_1: unisim.vcomponents.LUT4
    generic map(
      INIT => X"0004"
    )
        port map (
      I0 => \axil_rdata[1]_i_5_n_0\,
      I1 => \axil_rdata[1]_i_4_n_0\,
      I2 => xpm_cdc_array_single_inst2_i_2_n_0,
      I3 => xpm_cdc_array_single_inst2_i_3_n_0,
      O => done
    );
xpm_cdc_array_single_inst2_i_2: unisim.vcomponents.LUT5
    generic map(
      INIT => X"FFFFFFFE"
    )
        port map (
      I0 => ref_clk_cntr_reg(10),
      I1 => ref_clk_cntr_reg(13),
      I2 => ref_clk_cntr_reg(11),
      I3 => ref_clk_cntr_reg(12),
      I4 => \axil_rdata[1]_i_9_n_0\,
      O => xpm_cdc_array_single_inst2_i_2_n_0
    );
xpm_cdc_array_single_inst2_i_3: unisim.vcomponents.LUT5
    generic map(
      INIT => X"FFFFFFFE"
    )
        port map (
      I0 => ref_clk_cntr_reg(20),
      I1 => ref_clk_cntr_reg(23),
      I2 => ref_clk_cntr_reg(21),
      I3 => ref_clk_cntr_reg(22),
      I4 => \axil_rdata[1]_i_7_n_0\,
      O => xpm_cdc_array_single_inst2_i_3_n_0
    );
xpm_cdc_array_single_inst3: entity work.\decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0__2\
     port map (
      dest_clk => clk,
      dest_out(31 downto 0) => test_clk0_cntr_synced(31 downto 0),
      src_clk => '0',
      src_in(31 downto 0) => test_clk0_cntr_reg(31 downto 0)
    );
xpm_cdc_array_single_inst4: entity work.\decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__parameterized0\
     port map (
      dest_clk => clk,
      dest_out(31 downto 0) => test_clk1_cntr_synced(31 downto 0),
      src_clk => '0',
      src_in(31 downto 0) => test_clk1_cntr_reg(31 downto 0)
    );
xpm_cdc_array_single_inst5: entity work.\decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__7\
     port map (
      dest_clk => test_clk1,
      dest_out(0) => rst1_synced,
      src_clk => '0',
      src_in(0) => src_in0
    );
xpm_cdc_array_single_inst6: entity work.\decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single__9\
     port map (
      dest_clk => clk,
      dest_out(0) => user_rst1_ack,
      src_clk => '0',
      src_in(0) => rst1_synced
    );
xpm_cdc_array_single_inst7: entity work.decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_array_single
     port map (
      dest_clk => test_clk1,
      dest_out(0) => done1_synced,
      src_clk => '0',
      src_in(0) => done
    );
xpm_cdc_array_single_inst_i_1: unisim.vcomponents.LUT2
    generic map(
      INIT => X"B"
    )
        port map (
      I0 => user_rst_reg_n_0,
      I1 => reset_n,
      O => src_in0
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix is
  port (
    clk : in STD_LOGIC;
    reset_n : in STD_LOGIC;
    axil_awaddr : in STD_LOGIC_VECTOR ( 31 downto 0 );
    axil_awprot : in STD_LOGIC_VECTOR ( 2 downto 0 );
    axil_awvalid : in STD_LOGIC;
    axil_awready : out STD_LOGIC;
    axil_wdata : in STD_LOGIC_VECTOR ( 31 downto 0 );
    axil_wstrb : in STD_LOGIC_VECTOR ( 3 downto 0 );
    axil_wvalid : in STD_LOGIC;
    axil_wready : out STD_LOGIC;
    axil_bvalid : out STD_LOGIC;
    axil_bresp : out STD_LOGIC_VECTOR ( 1 downto 0 );
    axil_bready : in STD_LOGIC;
    axil_araddr : in STD_LOGIC_VECTOR ( 31 downto 0 );
    axil_arprot : in STD_LOGIC_VECTOR ( 2 downto 0 );
    axil_arvalid : in STD_LOGIC;
    axil_arready : out STD_LOGIC;
    axil_rdata : out STD_LOGIC_VECTOR ( 31 downto 0 );
    axil_rresp : out STD_LOGIC_VECTOR ( 1 downto 0 );
    axil_rvalid : out STD_LOGIC;
    axil_rready : in STD_LOGIC;
    test_clk0 : in STD_LOGIC;
    test_clk1 : in STD_LOGIC
  );
  attribute NotValidForBitStream : boolean;
  attribute NotValidForBitStream of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix : entity is true;
  attribute CHECK_LICENSE_TYPE : string;
  attribute CHECK_LICENSE_TYPE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix : entity is "pfm_dynamic_freq_counter_0_0,freq_counter,{}";
  attribute DowngradeIPIdentifiedWarnings : string;
  attribute DowngradeIPIdentifiedWarnings of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix : entity is "yes";
  attribute IP_DEFINITION_SOURCE : string;
  attribute IP_DEFINITION_SOURCE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix : entity is "package_project";
  attribute X_CORE_INFO : string;
  attribute X_CORE_INFO of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix : entity is "freq_counter,Vivado 2020.2";
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix is
  signal \<const0>\ : STD_LOGIC;
  attribute X_INTERFACE_INFO : string;
  attribute X_INTERFACE_INFO of axil_arready : signal is "xilinx.com:interface:aximm:1.0 axil ARREADY";
  attribute X_INTERFACE_INFO of axil_arvalid : signal is "xilinx.com:interface:aximm:1.0 axil ARVALID";
  attribute X_INTERFACE_INFO of axil_awready : signal is "xilinx.com:interface:aximm:1.0 axil AWREADY";
  attribute X_INTERFACE_INFO of axil_awvalid : signal is "xilinx.com:interface:aximm:1.0 axil AWVALID";
  attribute X_INTERFACE_INFO of axil_bready : signal is "xilinx.com:interface:aximm:1.0 axil BREADY";
  attribute X_INTERFACE_INFO of axil_bvalid : signal is "xilinx.com:interface:aximm:1.0 axil BVALID";
  attribute X_INTERFACE_INFO of axil_rready : signal is "xilinx.com:interface:aximm:1.0 axil RREADY";
  attribute X_INTERFACE_PARAMETER : string;
  attribute X_INTERFACE_PARAMETER of axil_rready : signal is "XIL_INTERFACENAME axil, DATA_WIDTH 32, PROTOCOL AXI4LITE, FREQ_HZ 50000000, ID_WIDTH 0, ADDR_WIDTH 32, AWUSER_WIDTH 0, ARUSER_WIDTH 0, WUSER_WIDTH 0, RUSER_WIDTH 0, BUSER_WIDTH 0, READ_WRITE_MODE READ_WRITE, HAS_BURST 0, HAS_LOCK 0, HAS_PROT 1, HAS_CACHE 0, HAS_QOS 0, HAS_REGION 0, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 1, SUPPORTS_NARROW_BURST 0, NUM_READ_OUTSTANDING 1, NUM_WRITE_OUTSTANDING 1, MAX_BURST_LENGTH 1, PHASE 0.000, CLK_DOMAIN pfm_dynamic_s_axi_aclk, NUM_READ_THREADS 1, NUM_WRITE_THREADS 1, RUSER_BITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0";
  attribute X_INTERFACE_INFO of axil_rvalid : signal is "xilinx.com:interface:aximm:1.0 axil RVALID";
  attribute X_INTERFACE_INFO of axil_wready : signal is "xilinx.com:interface:aximm:1.0 axil WREADY";
  attribute X_INTERFACE_INFO of axil_wvalid : signal is "xilinx.com:interface:aximm:1.0 axil WVALID";
  attribute X_INTERFACE_INFO of clk : signal is "xilinx.com:signal:clock:1.0 clk CLK";
  attribute X_INTERFACE_PARAMETER of clk : signal is "XIL_INTERFACENAME clk, ASSOCIATED_BUSIF axil, FREQ_HZ 50000000, PHASE 0.000, CLK_DOMAIN pfm_dynamic_s_axi_aclk, INSERT_VIP 0";
  attribute X_INTERFACE_INFO of reset_n : signal is "xilinx.com:signal:reset:1.0 reset_n RST";
  attribute X_INTERFACE_PARAMETER of reset_n : signal is "XIL_INTERFACENAME reset_n, POLARITY ACTIVE_LOW, INSERT_VIP 0";
  attribute X_INTERFACE_INFO of axil_araddr : signal is "xilinx.com:interface:aximm:1.0 axil ARADDR";
  attribute X_INTERFACE_INFO of axil_arprot : signal is "xilinx.com:interface:aximm:1.0 axil ARPROT";
  attribute X_INTERFACE_INFO of axil_awaddr : signal is "xilinx.com:interface:aximm:1.0 axil AWADDR";
  attribute X_INTERFACE_INFO of axil_awprot : signal is "xilinx.com:interface:aximm:1.0 axil AWPROT";
  attribute X_INTERFACE_INFO of axil_bresp : signal is "xilinx.com:interface:aximm:1.0 axil BRESP";
  attribute X_INTERFACE_INFO of axil_rdata : signal is "xilinx.com:interface:aximm:1.0 axil RDATA";
  attribute X_INTERFACE_INFO of axil_rresp : signal is "xilinx.com:interface:aximm:1.0 axil RRESP";
  attribute X_INTERFACE_INFO of axil_wdata : signal is "xilinx.com:interface:aximm:1.0 axil WDATA";
  attribute X_INTERFACE_INFO of axil_wstrb : signal is "xilinx.com:interface:aximm:1.0 axil WSTRB";
begin
  axil_bresp(1) <= \<const0>\;
  axil_bresp(0) <= \<const0>\;
  axil_rresp(1) <= \<const0>\;
  axil_rresp(0) <= \<const0>\;
GND: unisim.vcomponents.GND
     port map (
      G => \<const0>\
    );
inst: entity work.decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_freq_counter
     port map (
      axil_araddr(3 downto 0) => axil_araddr(3 downto 0),
      axil_arready => axil_arready,
      axil_arvalid => axil_arvalid,
      axil_awaddr(3 downto 0) => axil_awaddr(3 downto 0),
      axil_awready => axil_awready,
      axil_awvalid => axil_awvalid,
      axil_bready => axil_bready,
      axil_bvalid => axil_bvalid,
      axil_rdata(31 downto 0) => axil_rdata(31 downto 0),
      axil_rready => axil_rready,
      axil_rvalid => axil_rvalid,
      axil_wdata(30 downto 1) => axil_wdata(31 downto 2),
      axil_wdata(0) => axil_wdata(0),
      axil_wready => axil_wready,
      axil_wvalid => axil_wvalid,
      clk => clk,
      reset_n => reset_n,
      test_clk0 => test_clk0,
      test_clk1 => test_clk1
    );
end STRUCTURE;

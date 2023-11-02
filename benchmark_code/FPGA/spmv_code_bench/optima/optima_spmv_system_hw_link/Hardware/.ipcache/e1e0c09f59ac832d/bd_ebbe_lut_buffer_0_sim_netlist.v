// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2020.2 (lin64) Build 3064766 Wed Nov 18 09:12:47 MST 2020
// Date        : Fri Oct 20 18:51:13 2023
// Host        : gold1 running 64-bit Ubuntu 18.04.6 LTS
// Command     : write_verilog -force -mode funcsim -rename_top decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix -prefix
//               decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_ bd_ebbe_lut_buffer_0_sim_netlist.v
// Design      : bd_ebbe_lut_buffer_0
// Purpose     : This verilog netlist is a functional simulation representation of the design and should not be modified
//               or synthesized. This netlist cannot be used for SDF annotated simulation.
// Device      : xcu280-fsvh2892-2L-e
// --------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

(* CHECK_LICENSE_TYPE = "bd_ebbe_lut_buffer_0,lut_buffer_v2_0_0_lut_buffer,{}" *) (* DowngradeIPIdentifiedWarnings = "yes" *) (* X_CORE_INFO = "lut_buffer_v2_0_0_lut_buffer,Vivado 2020.2" *) 
(* NotValidForBitStream *)
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix
   (tdi_i,
    tms_i,
    tck_i,
    drck_i,
    sel_i,
    shift_i,
    update_i,
    capture_i,
    runtest_i,
    reset_i,
    bscanid_en_i,
    tdo_o,
    tdi_o,
    tms_o,
    tck_o,
    drck_o,
    sel_o,
    shift_o,
    update_o,
    capture_o,
    runtest_o,
    reset_o,
    bscanid_en_o,
    tdo_i);
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 s_bscan TDI" *) input tdi_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 s_bscan TMS" *) input tms_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 s_bscan TCK" *) input tck_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 s_bscan DRCK" *) input drck_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 s_bscan SEL" *) input sel_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 s_bscan SHIFT" *) input shift_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 s_bscan UPDATE" *) input update_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 s_bscan CAPTURE" *) input capture_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 s_bscan RUNTEST" *) input runtest_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 s_bscan RESET" *) input reset_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 s_bscan BSCANID_EN" *) input bscanid_en_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 s_bscan TDO" *) output tdo_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 m_bscan TDI" *) output tdi_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 m_bscan TMS" *) output tms_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 m_bscan TCK" *) output tck_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 m_bscan DRCK" *) output drck_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 m_bscan SEL" *) output sel_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 m_bscan SHIFT" *) output shift_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 m_bscan UPDATE" *) output update_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 m_bscan CAPTURE" *) output capture_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 m_bscan RUNTEST" *) output runtest_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 m_bscan RESET" *) output reset_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 m_bscan BSCANID_EN" *) output bscanid_en_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 m_bscan TDO" *) input tdo_i;

  wire bscanid_en_i;
  wire bscanid_en_o;
  wire capture_i;
  wire capture_o;
  wire drck_i;
  wire drck_o;
  wire reset_i;
  wire reset_o;
  wire runtest_i;
  wire runtest_o;
  wire sel_i;
  wire sel_o;
  wire shift_i;
  wire shift_o;
  wire tck_i;
  wire tck_o;
  wire tdi_i;
  wire tdi_o;
  wire tdo_i;
  wire tdo_o;
  wire tms_i;
  wire tms_o;
  wire update_i;
  wire update_o;
  wire [31:0]NLW_inst_bscanid_o_UNCONNECTED;

  (* C_EN_BSCANID_VEC = "0" *) 
  (* DONT_TOUCH *) 
  (* KEEP_HIERARCHY = "soft" *) 
  (* is_du_within_envelope = "true" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_lut_buffer_v2_0_0_lut_buffer inst
       (.bscanid_en_i(bscanid_en_i),
        .bscanid_en_o(bscanid_en_o),
        .bscanid_i({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .bscanid_o(NLW_inst_bscanid_o_UNCONNECTED[31:0]),
        .capture_i(capture_i),
        .capture_o(capture_o),
        .drck_i(drck_i),
        .drck_o(drck_o),
        .reset_i(reset_i),
        .reset_o(reset_o),
        .runtest_i(runtest_i),
        .runtest_o(runtest_o),
        .sel_i(sel_i),
        .sel_o(sel_o),
        .shift_i(shift_i),
        .shift_o(shift_o),
        .tck_i(tck_i),
        .tck_o(tck_o),
        .tdi_i(tdi_i),
        .tdi_o(tdi_o),
        .tdo_i(tdo_i),
        .tdo_o(tdo_o),
        .tms_i(tms_i),
        .tms_o(tms_o),
        .update_i(update_i),
        .update_o(update_o));
endmodule
`pragma protect begin_protected
`pragma protect version = 1
`pragma protect encrypt_agent = "XILINX"
`pragma protect encrypt_agent_info = "Xilinx Encryption Tool 2020.2"
`pragma protect key_keyowner="Cadence Design Systems.", key_keyname="cds_rsa_key", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=64)
`pragma protect key_block
geSCzw9gYjFCv0Dn0YxOXxhH+GZFMePCQPK3AjT+zbjt1urPphGbRmSIP212qcXhU3u6qBayOOuP
zGTUOznyYQ==

`pragma protect key_keyowner="Synopsys", key_keyname="SNPS-VCS-RSA-2", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=128)
`pragma protect key_block
OnCSRn8bnLy+eSxgkIEXKk5zY3JDppSX+6N3lQVX9PeSypgnQ/2z4GTpmoL+rdMoco6U9R4G1u4m
E0xhKuM4ba9nEk7cLfAxOQqKqsWQrZaIEmzIr1ET+cp4jOMvYA/MsN4jh93kbuKcSDuJ8zN13DFX
RemIkmekhFjPkyUS5qM=

`pragma protect key_keyowner="Aldec", key_keyname="ALDEC15_001", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
MSBAO7tnsBVh2XpVImbQvPkv1Ik+6Bw1D0e9n6H/Bw1mXnRXzm0RzPaEYAIFuluPbWglTrw4pQSr
JI/DSdCg6087Xmb+Q5zKawFvuZahx4HgmrKxTL15lZwamiIpmu3LGyxaEH/VbYGM9Ky0jp5PyDKU
Jeskyx64XVUPlRklhMjIKCtYITsgROzqjs+d1jIc494zqnDADEz0msJP38WdzHgwLDQ0NamfpodX
BUqMR71hgPx1Rvdt7HagUbkfyaG3/12LxFvpAblT7W0W6RKBFEFgFrxWRFaDw+jzj4jgl9g+sjY0
cveJYAA4UpZJwPSDIWehjmS+mOinzlnl8UP7jw==

`pragma protect key_keyowner="ATRENTA", key_keyname="ATR-SG-2015-RSA-3", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
UtIiSU3lZ1iUKAuaJuT083jLC5QokBuxbJC/zVsWXf8ozOCIDAvtpSufF02lDCCaNNheB40dXQFS
I8VBcTtdWzNr2vj/HmW17e10D6T6mqn/8t0HnWx9c3modRuXup0Too1mNTU5gTH+v3utogTO5ztm
HbJZ/+5ov0tPkaeJufJl5L/RZAfLmRnRYybtx5bbc7XiGyWaVk6KunsaWtX5zJtVnMeUOkg0N8oL
RBeyFp3tFqTN7ecNUp7zom6BjZ3fR6euRy36u1XviJsqBjcxzASI1k+bn2lDs3oEdXuMHoRvcuWL
mmMddzjwWr43L7YoB/WBz/Tw7t6iYI5B6imPUA==

`pragma protect key_keyowner="Mentor Graphics Corporation", key_keyname="MGC-VELOCE-RSA", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=128)
`pragma protect key_block
AMZ17uwJyVzW4KyzjD/YjCrX2GlLIDwW9HSuEat97pn8ZQ7QpDPhFLNx09klp1fHQ8yb1KlxCqpm
IjAljp4A5oQHWcBw/s+Xhtpin6GMDGjsmd5KmAD2J5DQmzqPazc0M8vNO+pGpCJogvWarX5XrP6U
56l9vH5mfyPmbT/09Kw=

`pragma protect key_keyowner="Mentor Graphics Corporation", key_keyname="MGC-VERIF-SIM-RSA-2", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
oAGTk8IFmmwAT5eT+h5xMK7MsqYCJnsMll3rq17njNu6wbVX2TAoOlVC5DzNg9T5ce7gVnuLuFG/
FgNSnTJx8TlbP73KxlDubmAVofR56G/yHzaJfJ0fwNhrfXm5AFgmFaKFPTKNkrG/qjdNuwUeA8p4
iHoj1zvPx50myVHXSpHLQ8n92DLWgMUX/57aPLbMHmYu/gsD0kHOuQ8Fr2Mi2DxufAvq1gzT0kc+
lxSntoseL+X1HLSvmKpEkR/sjaz6P9omIzqKlmOhvLeTZVEZcUtukVN1HTrlol+4/pTFDztcz0tZ
XqYZKVNB/igvn1iP/Fej8fpkaeJOrk1YgJZ6xw==

`pragma protect key_keyowner="Real Intent", key_keyname="RI-RSA-KEY-1", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
HM0OWTHTT9qkiqicwldwQlaXCXhAavOkOLtzdsXybVEqcdGTuInS8Xvu2i7fjfGdnZjc+o4ZayFd
adCXUGMQZ+7u79Rm71DMtV7WL4PMhXZmItLJgXQmNzajU211AuWse/CyD7Am9ZDJuQcIK0fcqZQI
XVJU6sMESVWiSWdCuqkcQLSuSoBY7TVLmCDoTF9n7MlYfcxCkkK6d+2Xs/gjaWO59GZ3TbWhAQLc
9hHL9YUJUTzZ8yPC8tX+DLS/YrniD3lBpquxXGcl1FxHKFTSpMG/6pTH+7Y5u1s29iqS/KYCCOfR
Pqg3ikxxR4ywBL+umX+Ijv+Hqgol6tBnCkWh/g==

`pragma protect key_keyowner="Metrics Technologies Inc.", key_keyname="DSim", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
DQuK/YuairmUNmnTZFN/Tbbjjk202ciTMNZWiES7z/c8BYrmlnBses/x7XzAVxFxOTns+S6gcbxv
lho3MoYQI3b1wxR93ymMpbFY2AAKqfTaYrt9nuBi+J66NUkNb4mO5Ysrmk/FyxUuVMw2JeKhCxVf
1Lkw2weXEA2RSHrWxd8764IFSbBqKoKUTMuqLxHovRaQHDy/mOdyefGG7/6ywGbKjVTlE8lXVH8E
8QodSYZ7p8uod81sVFzJL26a9Tqu+u2tOgD/WqMuxrio7zRkYYC5P+/FtxLC4GaIZ6LivaJuTLOF
bkAMneaa5dlfamLnRyzDXUCJu/DFpJtH5s1eLQ==

`pragma protect key_keyowner="Xilinx", key_keyname="xilinxt_2020_08", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
DL1JbuaDxIhP8ir0jAdx6nea42/rMQ2VXG8PZEGqMkgF/yLmK8+UcPNvmkEnUbVxq6WNxCUUYBOO
eroUXvKd3hB7aza+lE1PkczDRQpe4dZWQ8yHCUSbqj/KnUKU7sMHOSk5MiYcbBazdC+B/zdSxJsr
sUmnOLgp/SqygmZW7/oDYMIYyOExEOrIPD4CH/xXZGlvuNs4OjdmaSus7kQp/iaUxQz03NGaMv3/
EuIfORb3j+mQPwXwEBQhecy81p8ky3bmOS0LK+CPuz0LF3VVvrDnnXUSBCWa2WW6t7burmoHvgPV
oB2jrvwkS6dNjRJ7CoyGvV0N6d60kiD0LjZg6Q==

`pragma protect data_method = "AES128-CBC"
`pragma protect encoding = (enctype = "BASE64", line_length = 76, bytes = 2960)
`pragma protect data_block
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
`pragma protect end_protected
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

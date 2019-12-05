// Created by altera_lib_lpm.pl from fp_add_sub.v
// Created by altera_lib_lpm.pl from fp_add_sub.v
// megafunction wizard: %ALTFP_ADD_SUB%
// GENERATION: STANDARD
// VERSION: WM1.0
// MODULE: altfp_add_sub

// ============================================================
// File Name: fp_add_sub.v
// Megafunction Name(s):
// 			altfp_add_sub
//
// Simulation Library Files(s):
// 			lpm
// ============================================================
// ************************************************************
// THIS IS A WIZARD-GENERATED FILE. DO NOT EDIT THIS FILE!
//
// 18.1.0 Build 625 09/12/2018 SJ Lite Edition
// ************************************************************


//Copyright (C) 2018  Intel Corporation. All rights reserved.
//Your use of Intel Corporation's design tools, logic functions
//and other software and tools, and its AMPP partner logic
//functions, and any output files from any of the foregoing
//(including device programming or simulation files), and any
//associated documentation or information are expressly subject
//to the terms and conditions of the Intel Program License
//Subscription Agreement, the Intel Quartus Prime License Agreement,
//the Intel FPGA IP License Agreement, or other applicable license
//agreement, including, without limitation, that your use is for
//the sole purpose of programming logic devices manufactured by
//Intel and sold by Intel or its authorized distributors.  Please
//refer to the applicable agreement for further details.


//altfp_add_sub CBX_AUTO_BLACKBOX="ALL" DENORMAL_SUPPORT="NO" DEVICE_FAMILY="Cyclone V" DIRECTION="VARIABLE" OPTIMIZE="SPEED" PIPELINE=14 REDUCED_FUNCTIONALITY="NO" WIDTH_EXP=8 WIDTH_MAN=23 add_sub clk_en clock dataa datab result
//VERSION_BEGIN 18.1 cbx_altbarrel_shift 2018:09:12:13:04:09:SJ cbx_altfp_add_sub 2018:09:12:13:04:09:SJ cbx_altpriority_encoder 2018:09:12:13:04:09:SJ cbx_cycloneii 2018:09:12:13:04:09:SJ cbx_lpm_add_sub 2018:09:12:13:04:09:SJ cbx_lpm_compare 2018:09:12:13:04:09:SJ cbx_mgl 2018:09:12:14:15:07:SJ cbx_nadder 2018:09:12:13:04:09:SJ cbx_stratix 2018:09:12:13:04:09:SJ cbx_stratixii 2018:09:12:13:04:09:SJ  VERSION_END
// synthesis VERILOG_INPUT_VERSION VERILOG_2001
// altera message_off 10463



//altbarrel_shift CBX_AUTO_BLACKBOX="ALL" DEVICE_FAMILY="Cyclone V" PIPELINE=1 SHIFTDIR="LEFT" WIDTH=26 WIDTHDIST=5 aclr clk_en clock data distance result
//VERSION_BEGIN 18.1 cbx_altbarrel_shift 2018:09:12:13:04:09:SJ cbx_mgl 2018:09:12:14:15:07:SJ  VERSION_END

//synthesis_resources = reg 27
//synopsys translate_off
`timescale 1 ps / 1 ps
//synopsys translate_on
/*verilator lint_off CASEX*/
/*verilator lint_off COMBDLY*/
/*verilator lint_off INITIALDLY*/
/*verilator lint_off LITENDIAN*/
/*verilator lint_off MULTIDRIVEN*/
/*verilator lint_off UNOPTFLAT*/
/*verilator lint_off BLKANDNBLK*/
module  fp_add_sub_altbarrel_shift_ltd
	(
	aclr,
	clk_en,
	clock,
	data,
	distance,
	result) ;
	input   aclr;
	input   clk_en;
	input   clock;
	input   [25:0]  data;
	input   [4:0]  distance;
	output   [25:0]  result;
`ifndef ALTERA_RESERVED_QIS
// synopsys translate_off
`endif
	logic   aclr; // -- converted tristate to logic
	logic   clk_en; // -- converted tristate to logic
	logic   clock; // -- converted tristate to logic
`ifndef ALTERA_RESERVED_QIS
// synopsys translate_on
`endif

	reg	[0:0]	dir_pipe;
	reg	[25:0]	sbit_piper1d;
	wire  [5:0]  dir_w;
	wire  direction_w;
	wire  [15:0]  pad_w;
	wire  [155:0]  sbit_w;
	wire  [4:0]  sel_w;
	wire  [129:0]  smux_w;

	// synopsys translate_off
	initial
		dir_pipe = 0;
	// synopsys translate_on
	always @ ( posedge clock or  posedge aclr)
		if (aclr == 1'b1) dir_pipe <= 1'b0;
		else if  (clk_en == 1'b1)   dir_pipe <= {dir_w[4]};
	// synopsys translate_off
	initial
		sbit_piper1d = 0;
	// synopsys translate_on
	always @ ( posedge clock or  posedge aclr)
		if (aclr == 1'b1) sbit_piper1d <= 26'b0;
		else if  (clk_en == 1'b1)   sbit_piper1d <= smux_w[129:104];
	assign
		dir_w = {dir_pipe[0], dir_w[3:0], direction_w},
		direction_w = 1'b0,
		pad_w = {16{1'b0}},
		result = sbit_w[155:130],
		sbit_w = {sbit_piper1d, smux_w[103:0], data},
		sel_w = {distance[4:0]},
		smux_w = {((({26{(sel_w[4] & (~ dir_w[4]))}} & {sbit_w[113:104], pad_w[15:0]}) | ({26{(sel_w[4] & dir_w[4])}} & {pad_w[15:0], sbit_w[129:120]})) | ({26{(~ sel_w[4])}} & sbit_w[129:104])), ((({26{(sel_w[3] & (~ dir_w[3]))}} & {sbit_w[95:78], pad_w[7:0]}) | ({26{(sel_w[3] & dir_w[3])}} & {pad_w[7:0], sbit_w[103:86]})) | ({26{(~ sel_w[3])}} & sbit_w[103:78])), ((({26{(sel_w[2] & (~ dir_w[2]))}} & {sbit_w[73:52], pad_w[3:0]}) | ({26{(sel_w[2] & dir_w[2])}} & {pad_w[3:0], sbit_w[77:56]})) | ({26{(~ sel_w[2])}} & sbit_w[77:52])), ((({26{(sel_w[1] & (~ dir_w[1]))}} & {sbit_w[49:26], pad_w[1:0]}) | ({26{(sel_w[1] & dir_w[1])}} & {pad_w[1:0], sbit_w[51:28]})) | ({26{(~ sel_w[1])}} & sbit_w[51:26])), ((({26{(sel_w[0] & (~ dir_w[0]))}} & {sbit_w[24:0], pad_w[0]}) | ({26{(sel_w[0] & dir_w[0])}} & {pad_w[0], sbit_w[25:1]})) | ({26{(~ sel_w[0])}} & sbit_w[25:0]))};
endmodule //fp_add_sub_altbarrel_shift_ltd


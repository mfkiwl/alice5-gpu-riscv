// Created by altera_lib_lpm.pl from fp_add_sub.v

//synthesis_resources =
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
module  fp_add_sub_altpriority_encoder_uv8
	(
	data,
	q) ;
	input   [15:0]  data;
	output   [3:0]  q;

	wire  [2:0]   wire_altpriority_encoder10_q;
	wire  wire_altpriority_encoder10_zero;
	wire  [2:0]   wire_altpriority_encoder9_q;

	fp_add_sub_altpriority_encoder_be8   altpriority_encoder10
	(
	.data(data[15:8]),
	.q(wire_altpriority_encoder10_q),
	.zero(wire_altpriority_encoder10_zero));
	fp_add_sub_altpriority_encoder_bv7   altpriority_encoder9
	(
	.data(data[7:0]),
	.q(wire_altpriority_encoder9_q));
	assign
		q = {(~ wire_altpriority_encoder10_zero), (({3{wire_altpriority_encoder10_zero}} & wire_altpriority_encoder9_q) | ({3{(~ wire_altpriority_encoder10_zero)}} & wire_altpriority_encoder10_q))};
endmodule //fp_add_sub_altpriority_encoder_uv8


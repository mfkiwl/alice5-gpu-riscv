// Created by altera_lib_lpm.pl from fp_add_sub.v


//altpriority_encoder CBX_AUTO_BLACKBOX="ALL" LSB_PRIORITY="YES" PIPELINE=2 WIDTH=32 WIDTHAD=5 aclr clk_en clock data q
//VERSION_BEGIN 18.1 cbx_altpriority_encoder 2018:09:12:13:04:09:SJ cbx_mgl 2018:09:12:14:15:07:SJ  VERSION_END


//altpriority_encoder CBX_AUTO_BLACKBOX="ALL" LSB_PRIORITY="YES" PIPELINE=1 WIDTH=16 WIDTHAD=4 aclr clk_en clock data q zero
//VERSION_BEGIN 18.1 cbx_altpriority_encoder 2018:09:12:13:04:09:SJ cbx_mgl 2018:09:12:14:15:07:SJ  VERSION_END


//altpriority_encoder CBX_AUTO_BLACKBOX="ALL" LSB_PRIORITY="YES" PIPELINE=0 WIDTH=8 WIDTHAD=3 data q zero
//VERSION_BEGIN 18.1 cbx_altpriority_encoder 2018:09:12:13:04:09:SJ cbx_mgl 2018:09:12:14:15:07:SJ  VERSION_END


//altpriority_encoder CBX_AUTO_BLACKBOX="ALL" LSB_PRIORITY="YES" WIDTH=4 WIDTHAD=2 data q zero
//VERSION_BEGIN 18.1 cbx_altpriority_encoder 2018:09:12:13:04:09:SJ cbx_mgl 2018:09:12:14:15:07:SJ  VERSION_END


//altpriority_encoder CBX_AUTO_BLACKBOX="ALL" LSB_PRIORITY="YES" WIDTH=2 WIDTHAD=1 data q zero
//VERSION_BEGIN 18.1 cbx_altpriority_encoder 2018:09:12:13:04:09:SJ cbx_mgl 2018:09:12:14:15:07:SJ  VERSION_END

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
module  fp_add_sub_altpriority_encoder_nh8
	(
	data,
	q,
	zero) ;
	input   [1:0]  data;
	output   [0:0]  q;
	output   zero;


	assign
		q = {(~ data[0])},
		zero = (~ (data[0] | data[1]));
endmodule //fp_add_sub_altpriority_encoder_nh8


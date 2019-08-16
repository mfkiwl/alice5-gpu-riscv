// RISC-V IMF decoder
module RISCVDecode
    #(parameter INSN_WIDTH=32) 
(
    input wire clock,
    input wire [INSN_WIDTH-1:0] inst,

    output reg opcode_is_branch,
    output reg opcode_is_ALU_reg_imm,
    output reg opcode_is_ALU_reg_reg,
    output reg opcode_is_jal,
    output reg opcode_is_jalr,
    output reg opcode_is_lui,
    output reg opcode_is_auipc,
    output reg opcode_is_load,
    output reg opcode_is_store,
    output reg opcode_is_system,
    output reg opcode_is_fadd,
    output reg opcode_is_fsub,
    output reg opcode_is_fmul,
    output reg opcode_is_fdiv,
    output reg opcode_is_fsgnj,
    output reg opcode_is_fminmax,
    output reg opcode_is_fsqrt,
    output reg opcode_is_fcmp,
    output reg opcode_is_fcvt_f2i,
    output reg opcode_is_fmv_f2i,
    output reg opcode_is_fcvt_i2f,
    output reg opcode_is_fmv_i2f,
    output reg opcode_is_flw,
    output reg opcode_is_fsw,
    output reg opcode_is_fmadd,
    output reg opcode_is_fmsub,
    output reg opcode_is_fnmsub,
    output reg opcode_is_fnmadd,

    output reg [4:0] rs1,
    output reg [4:0] rs2,
    output reg [4:0] rs3,
    output reg [4:0] rd,
    output reg [1:0] fmt,
    output reg [2:0] funct3_rm,
    output reg [6:0] funct7,
    output reg [4:0] funct5,
    output reg [4:0] shamt_ftype,

    output reg [31:0] imm_alu_load,
    output reg [31:0] imm_store,
    output reg [31:0] imm_branch,
    output reg [31:0] imm_upper,
    output reg [31:0] imm_jump
);

    wire [6:0] opcode;
    wire [4:0] ffunct;

    assign opcode = {inst[6:2],inst[1:0]};
    assign ffunct = {inst[31:27]};

    always @(posedge clock) begin

        opcode_is_branch <= opcode == {5'h18,2'h3};
        opcode_is_jal <= opcode == {5'h1b,2'h3};
        opcode_is_jalr <= opcode == {5'h19,2'h3};
        opcode_is_lui <= opcode == {5'h0d,2'h3};
        opcode_is_auipc <= opcode == {5'h05,2'h3};
        opcode_is_ALU_reg_imm <= opcode == {5'h04,2'h3};
        opcode_is_ALU_reg_reg <= opcode == {5'h0c,2'h3};
        opcode_is_load <= opcode == {5'h00,2'h3};
        opcode_is_store <= opcode == {5'h08,2'h3};
        opcode_is_system <= opcode == {5'h1c,2'h3};

        rs1 <= inst[19:15];
        rs2 <= inst[24:20];
        rs3 <= inst[31:27];
        rd <= inst[11:7];
        fmt <= inst[26:25];
        funct3_rm <= inst[14:12];
        funct7 <= inst[31:25];
        funct5 <= inst[31:27];
        shamt_ftype <= inst[24:20];

        opcode_is_fadd <= ((opcode == {5'h14,2'h3}) && (ffunct == {5'h0}));
        opcode_is_fsub <= ((opcode == {5'h14,2'h3}) && (ffunct == {5'h1}));
        opcode_is_fmul <= ((opcode == {5'h14,2'h3}) && (ffunct == {5'h2}));
        opcode_is_fdiv <= ((opcode == {5'h14,2'h3}) && (ffunct == {5'h3}));
        opcode_is_fsgnj <= ((opcode == {5'h14,2'h3}) && (ffunct == {5'h4}));
        opcode_is_fminmax <= ((opcode == {5'h14,2'h3}) && (ffunct == {5'h5}));
        opcode_is_fsqrt <= ((opcode == {5'h14,2'h3}) && (ffunct == {5'hb}));
        opcode_is_fcmp <= ((opcode == {5'h14,2'h3}) && (ffunct == {5'h14}));
        opcode_is_fcvt_f2i <= ((opcode == {5'h14,2'h3}) && (ffunct == {5'h18}));
        opcode_is_fmv_f2i <= ((opcode == {5'h14,2'h3}) && (ffunct == {5'h1c}));
        opcode_is_fcvt_i2f <= ((opcode == {5'h14,2'h3}) && (ffunct == {5'h1a}));
        opcode_is_fmv_i2f <= ((opcode == {5'h14,2'h3}) && (ffunct == {5'h1e}));
        opcode_is_flw <= opcode == {5'h01,2'h3};
        opcode_is_fsw <= opcode == {5'h09,2'h3};
        opcode_is_fmadd <= opcode == {5'h10,2'h3};
        opcode_is_fmsub <= opcode == {5'h11,2'h3};
        opcode_is_fnmsub <= opcode == {5'h12,2'h3};
        opcode_is_fnmadd <= opcode == {5'h13,2'h3};

        // Replications of bit 31 in following are sign extensions
        imm_alu_load <= {{20{inst[31]}}, inst[31:20]};
        imm_store <= {{20{inst[31]}}, inst[31:25], inst[11:7]};
        imm_branch <= {{19{inst[31]}}, inst[31], inst[7], inst[30:25], inst[11:8], 1'b0};
        imm_upper <= {inst[31:12], 12'b0};
        imm_jump <= {{11{inst[31]}}, inst[31], inst[19:12], inst[20], inst[30:21], 1'b0};
    end
endmodule
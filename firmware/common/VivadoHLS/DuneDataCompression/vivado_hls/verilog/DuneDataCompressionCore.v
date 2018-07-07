// ==============================================================
// RTL generated by Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC
// Version: 2018.1
// Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
// 
// ===========================================================

`timescale 1 ns / 1 ps 

(* CORE_GENERATION_INFO="DuneDataCompressionCore,hls_ip_2018_1,{HLS_INPUT_TYPE=cxx,HLS_INPUT_FLOAT=0,HLS_INPUT_FIXED=1,HLS_INPUT_PART=xc7z045ffg900-2,HLS_INPUT_CLOCK=8.000000,HLS_INPUT_ARCH=dataflow,HLS_SYN_CLOCK=9.459500,HLS_SYN_LAT=95492,HLS_SYN_TPT=54320,HLS_SYN_MEM=143,HLS_SYN_DSP=8,HLS_SYN_FF=29216,HLS_SYN_LUT=56245}" *)

module DuneDataCompressionCore (
        s_axi_BUS_A_AWVALID,
        s_axi_BUS_A_AWREADY,
        s_axi_BUS_A_AWADDR,
        s_axi_BUS_A_WVALID,
        s_axi_BUS_A_WREADY,
        s_axi_BUS_A_WDATA,
        s_axi_BUS_A_WSTRB,
        s_axi_BUS_A_ARVALID,
        s_axi_BUS_A_ARREADY,
        s_axi_BUS_A_ARADDR,
        s_axi_BUS_A_RVALID,
        s_axi_BUS_A_RREADY,
        s_axi_BUS_A_RDATA,
        s_axi_BUS_A_RRESP,
        s_axi_BUS_A_BVALID,
        s_axi_BUS_A_BREADY,
        s_axi_BUS_A_BRESP,
        ap_clk,
        ap_rst_n,
        interrupt,
        sAxis_TDATA,
        sAxis_TKEEP,
        sAxis_TSTRB,
        sAxis_TUSER,
        sAxis_TLAST,
        sAxis_TID,
        sAxis_TDEST,
        mAxis_TDATA,
        mAxis_TKEEP,
        mAxis_TSTRB,
        mAxis_TUSER,
        mAxis_TLAST,
        mAxis_TID,
        mAxis_TDEST,
        moduleIdx_V,
        mAxis_TVALID,
        mAxis_TREADY,
        sAxis_TVALID,
        sAxis_TREADY
);

parameter    C_S_AXI_BUS_A_DATA_WIDTH = 32;
parameter    C_S_AXI_BUS_A_ADDR_WIDTH = 10;
parameter    C_S_AXI_DATA_WIDTH = 32;
parameter    C_S_AXI_ADDR_WIDTH = 32;

parameter C_S_AXI_BUS_A_WSTRB_WIDTH = (32 / 8);
parameter C_S_AXI_WSTRB_WIDTH = (32 / 8);

input   s_axi_BUS_A_AWVALID;
output   s_axi_BUS_A_AWREADY;
input  [C_S_AXI_BUS_A_ADDR_WIDTH - 1:0] s_axi_BUS_A_AWADDR;
input   s_axi_BUS_A_WVALID;
output   s_axi_BUS_A_WREADY;
input  [C_S_AXI_BUS_A_DATA_WIDTH - 1:0] s_axi_BUS_A_WDATA;
input  [C_S_AXI_BUS_A_WSTRB_WIDTH - 1:0] s_axi_BUS_A_WSTRB;
input   s_axi_BUS_A_ARVALID;
output   s_axi_BUS_A_ARREADY;
input  [C_S_AXI_BUS_A_ADDR_WIDTH - 1:0] s_axi_BUS_A_ARADDR;
output   s_axi_BUS_A_RVALID;
input   s_axi_BUS_A_RREADY;
output  [C_S_AXI_BUS_A_DATA_WIDTH - 1:0] s_axi_BUS_A_RDATA;
output  [1:0] s_axi_BUS_A_RRESP;
output   s_axi_BUS_A_BVALID;
input   s_axi_BUS_A_BREADY;
output  [1:0] s_axi_BUS_A_BRESP;
input   ap_clk;
input   ap_rst_n;
output   interrupt;
input  [63:0] sAxis_TDATA;
input  [7:0] sAxis_TKEEP;
input  [7:0] sAxis_TSTRB;
input  [3:0] sAxis_TUSER;
input  [0:0] sAxis_TLAST;
input  [0:0] sAxis_TID;
input  [0:0] sAxis_TDEST;
output  [63:0] mAxis_TDATA;
output  [7:0] mAxis_TKEEP;
output  [7:0] mAxis_TSTRB;
output  [3:0] mAxis_TUSER;
output  [0:0] mAxis_TLAST;
output  [0:0] mAxis_TID;
output  [0:0] mAxis_TDEST;
input  [0:0] moduleIdx_V;
output   mAxis_TVALID;
input   mAxis_TREADY;
input   sAxis_TVALID;
output   sAxis_TREADY;

reg    ap_rst_n_inv;
wire    ap_start;
wire    ap_ready;
wire    ap_done;
wire    ap_idle;
wire   [31:0] config_init;
wire   [31:0] config_mode;
wire   [31:0] config_limit;
wire   [31:0] monitor_common_pattern;
wire   [31:0] monitor_cfg_m_mode;
wire   [31:0] monitor_cfg_m_ncfgs;
wire   [31:0] monitor_read_summary_mask_V;
wire   [31:0] monitor_read_summary_nframes;
wire   [31:0] monitor_write_nbytes;
wire   [31:0] monitor_write_npromoted;
wire   [31:0] monitor_write_ndropped;
wire   [31:0] monitor_write_npackets;
wire   [63:0] handle_packet_U0_mAxis_TDATA;
wire   [7:0] handle_packet_U0_mAxis_TKEEP;
wire   [7:0] handle_packet_U0_mAxis_TSTRB;
wire   [3:0] handle_packet_U0_mAxis_TUSER;
wire   [0:0] handle_packet_U0_mAxis_TLAST;
wire   [0:0] handle_packet_U0_mAxis_TID;
wire   [0:0] handle_packet_U0_mAxis_TDEST;
wire    handle_packet_U0_sAxis_TREADY;
wire    handle_packet_U0_mAxis_TVALID;
wire    handle_packet_U0_ap_done;
wire    handle_packet_U0_ap_start;
wire    handle_packet_U0_ap_ready;
wire    handle_packet_U0_ap_idle;
wire    handle_packet_U0_ap_continue;
wire    ap_sync_continue;
wire    ap_sync_done;
wire    ap_sync_ready;
wire    handle_packet_U0_start_full_n;
wire    handle_packet_U0_start_write;

DuneDataCompressionCore_BUS_A_s_axi #(
    .C_S_AXI_ADDR_WIDTH( C_S_AXI_BUS_A_ADDR_WIDTH ),
    .C_S_AXI_DATA_WIDTH( C_S_AXI_BUS_A_DATA_WIDTH ))
DuneDataCompressionCore_BUS_A_s_axi_U(
    .AWVALID(s_axi_BUS_A_AWVALID),
    .AWREADY(s_axi_BUS_A_AWREADY),
    .AWADDR(s_axi_BUS_A_AWADDR),
    .WVALID(s_axi_BUS_A_WVALID),
    .WREADY(s_axi_BUS_A_WREADY),
    .WDATA(s_axi_BUS_A_WDATA),
    .WSTRB(s_axi_BUS_A_WSTRB),
    .ARVALID(s_axi_BUS_A_ARVALID),
    .ARREADY(s_axi_BUS_A_ARREADY),
    .ARADDR(s_axi_BUS_A_ARADDR),
    .RVALID(s_axi_BUS_A_RVALID),
    .RREADY(s_axi_BUS_A_RREADY),
    .RDATA(s_axi_BUS_A_RDATA),
    .RRESP(s_axi_BUS_A_RRESP),
    .BVALID(s_axi_BUS_A_BVALID),
    .BREADY(s_axi_BUS_A_BREADY),
    .BRESP(s_axi_BUS_A_BRESP),
    .ACLK(ap_clk),
    .ARESET(ap_rst_n_inv),
    .ACLK_EN(1'b1),
    .ap_start(ap_start),
    .interrupt(interrupt),
    .ap_ready(ap_ready),
    .ap_done(ap_done),
    .ap_idle(ap_idle),
    .config_init(config_init),
    .config_mode(config_mode),
    .config_limit(config_limit),
    .monitor_common_pattern(monitor_common_pattern),
    .monitor_cfg_m_mode(monitor_cfg_m_mode),
    .monitor_cfg_m_ncfgs(monitor_cfg_m_ncfgs),
    .monitor_read_summary_mask_V(monitor_read_summary_mask_V),
    .monitor_read_summary_nframes(monitor_read_summary_nframes),
    .monitor_write_nbytes(monitor_write_nbytes),
    .monitor_write_npromoted(monitor_write_npromoted),
    .monitor_write_ndropped(monitor_write_ndropped),
    .monitor_write_npackets(monitor_write_npackets)
);

handle_packet handle_packet_U0(
    .ap_clk(ap_clk),
    .ap_rst(ap_rst_n_inv),
    .mAxis_TDATA(handle_packet_U0_mAxis_TDATA),
    .mAxis_TKEEP(handle_packet_U0_mAxis_TKEEP),
    .mAxis_TSTRB(handle_packet_U0_mAxis_TSTRB),
    .mAxis_TUSER(handle_packet_U0_mAxis_TUSER),
    .mAxis_TLAST(handle_packet_U0_mAxis_TLAST),
    .mAxis_TID(handle_packet_U0_mAxis_TID),
    .mAxis_TDEST(handle_packet_U0_mAxis_TDEST),
    .sAxis_TDATA(sAxis_TDATA),
    .sAxis_TKEEP(sAxis_TKEEP),
    .sAxis_TSTRB(sAxis_TSTRB),
    .sAxis_TUSER(sAxis_TUSER),
    .sAxis_TLAST(sAxis_TLAST),
    .sAxis_TID(sAxis_TID),
    .sAxis_TDEST(sAxis_TDEST),
    .sAxis_TVALID(sAxis_TVALID),
    .sAxis_TREADY(handle_packet_U0_sAxis_TREADY),
    .mAxis_TVALID(handle_packet_U0_mAxis_TVALID),
    .mAxis_TREADY(mAxis_TREADY),
    .ap_done(handle_packet_U0_ap_done),
    .ap_start(handle_packet_U0_ap_start),
    .ap_ready(handle_packet_U0_ap_ready),
    .ap_idle(handle_packet_U0_ap_idle),
    .ap_continue(handle_packet_U0_ap_continue)
);

assign ap_done = handle_packet_U0_ap_done;

assign ap_idle = handle_packet_U0_ap_idle;

assign ap_ready = handle_packet_U0_ap_ready;

always @ (*) begin
    ap_rst_n_inv = ~ap_rst_n;
end

assign ap_sync_continue = 1'b1;

assign ap_sync_done = handle_packet_U0_ap_done;

assign ap_sync_ready = handle_packet_U0_ap_ready;

assign handle_packet_U0_ap_continue = 1'b1;

assign handle_packet_U0_ap_start = ap_start;

assign handle_packet_U0_start_full_n = 1'b1;

assign handle_packet_U0_start_write = 1'b0;

assign mAxis_TDATA = handle_packet_U0_mAxis_TDATA;

assign mAxis_TDEST = handle_packet_U0_mAxis_TDEST;

assign mAxis_TID = handle_packet_U0_mAxis_TID;

assign mAxis_TKEEP = handle_packet_U0_mAxis_TKEEP;

assign mAxis_TLAST = handle_packet_U0_mAxis_TLAST;

assign mAxis_TSTRB = handle_packet_U0_mAxis_TSTRB;

assign mAxis_TUSER = handle_packet_U0_mAxis_TUSER;

assign mAxis_TVALID = handle_packet_U0_mAxis_TVALID;

assign sAxis_TREADY = handle_packet_U0_sAxis_TREADY;

endmodule //DuneDataCompressionCore
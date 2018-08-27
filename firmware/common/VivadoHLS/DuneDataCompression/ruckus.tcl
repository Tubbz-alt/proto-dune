# Load RUCKUS library
source -quiet $::env(RUCKUS_DIR)/vivado_proc.tcl

# Load Source Code
loadSource -dir  "$::DIR_PATH/rtl/"
loadSource -path "$::DIR_PATH/vivado_hls/ip/DuneDataCompressionCore.dcp"

loadSource -sim_only -dir "$::DIR_PATH/vivado_hls/verilog/"

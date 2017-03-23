-------------------------------------------------------------------------------
-- File       : ProtoDuneDpmTimingReg.vhd
-- Company    : SLAC National Accelerator Laboratory
-- Created    : 2016-09-26
-- Last update: 2017-03-22
-------------------------------------------------------------------------------
-- Description: 
-------------------------------------------------------------------------------
-- This file is part of 'DUNE Development Firmware'.
-- It is subject to the license terms in the LICENSE.txt file found in the 
-- top-level directory of this distribution and at: 
--    https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
-- No part of 'DUNE Development Firmware', including this file, 
-- may be copied, modified, propagated, or distributed except according to 
-- the terms contained in the LICENSE.txt file.
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;

use work.StdRtlPkg.all;
use work.AxiLitePkg.all;

entity ProtoDuneDpmTimingReg is
   generic (
      TPD_G            : time            := 1 ns;
      AXI_ERROR_RESP_G : slv(1 downto 0) := AXI_RESP_DECERR_C);
   port (
      -- WIB Interface (wibClk domain)
      wibClk          : in  sl;
      wibRst          : in  sl;
      runEnable       : in  sl;
      swFlush         : out sl;
      -- Timing RX Interface (cdrClk domain)
      cdrClk          : in  sl;
      cdrRst          : in  sl;
      cdrEdgeSel      : out sl;
      cdrDataInv      : out sl;
      timingLinkUp    : in  sl;
      cdrLocked       : in  sl;
      timingStat      : in  slv(3 downto 0);
      freqMeasured    : in  slv(31 downto 0);
      -- AXI-Lite Interface (axilClk domain)
      axilClk         : in  sl;
      axilRst         : in  sl;
      axilReadMaster  : in  AxiLiteReadMasterType;
      axilReadSlave   : out AxiLiteReadSlaveType;
      axilWriteMaster : in  AxiLiteWriteMasterType;
      axilWriteSlave  : out AxiLiteWriteSlaveType);
end ProtoDuneDpmTimingReg;

architecture rtl of ProtoDuneDpmTimingReg is

   constant STATUS_SIZE_C : positive := 2;

   type RegType is record
      cdrEdgeSel     : sl;
      cdrDataInv     : sl;
      swFlush        : sl;
      cntRst         : sl;
      rollOverEn     : slv(STATUS_SIZE_C-1 downto 0);
      hardRst        : sl;
      axilReadSlave  : AxiLiteReadSlaveType;
      axilWriteSlave : AxiLiteWriteSlaveType;
   end record;

   constant REG_INIT_C : RegType := (
      cdrEdgeSel     => '0',
      cdrDataInv     => '0',
      swFlush        => '1',
      cntRst         => '1',
      rollOverEn     => (others => '0'),
      hardRst        => '0',
      axilReadSlave  => AXI_LITE_READ_SLAVE_INIT_C,
      axilWriteSlave => AXI_LITE_WRITE_SLAVE_INIT_C);

   signal r   : RegType := REG_INIT_C;
   signal rin : RegType;

   signal statusOut : slv(STATUS_SIZE_C-1 downto 0);
   signal statusCnt : SlVectorArray(STATUS_SIZE_C-1 downto 0, 31 downto 0);

   signal linkUp : sl;   
   
   -- attribute dont_touch               : string;
   -- attribute dont_touch of r          : signal is "TRUE";

begin

   comb : process (axilReadMaster, axilRst, axilWriteMaster, freqMeasured, r,
                   statusCnt, statusOut, timingStat) is
      variable v      : RegType;
      variable regCon : AxiLiteEndPointType;
   begin
      -- Latch the current value
      v := r;

      -- Reset the strobes
      v.hardRst := '0';
      v.cntRst  := '0';

      -- Check for hard reset
      if (r.hardRst = '1') then
         -- Reset the register
         v := REG_INIT_C;
      end if;

      -- Determine the transaction type
      axiSlaveWaitTxn(regCon, axilWriteMaster, axilReadMaster, v.axilWriteSlave, v.axilReadSlave);

      -- Map the read registers
      for i in STATUS_SIZE_C-1 downto 0 loop
         axiSlaveRegisterR(regCon, toSlv((4*i), 12), 0, muxSlVectorArray(statusCnt, i));
      end loop;
      axiSlaveRegisterR(regCon, x"400", 0, statusOut);
      axiSlaveRegisterR(regCon, x"404", 0, freqMeasured);
      axiSlaveRegisterR(regCon, x"408", 0, timingStat);

      -- Map the write registers
      axiSlaveRegister(regCon, x"800", 0, v.swFlush);
      axiSlaveRegister(regCon, x"804", 0, v.cdrEdgeSel);
      axiSlaveRegister(regCon, x"808", 0, v.cdrDataInv);

      axiSlaveRegister(regCon, x"FF0", 0, v.rollOverEn);
      axiSlaveRegister(regCon, x"FF4", 0, v.cntRst);
      axiSlaveRegister(regCon, x"FFC", 0, v.hardRst);

      -- Closeout the transaction
      axiSlaveDefault(regCon, v.axilWriteSlave, v.axilReadSlave, AXI_ERROR_RESP_G);

      -- Synchronous Reset
      if (axilRst = '1') then
         -- Reset the register
         v := REG_INIT_C;
      end if;

      -- Register the variable for next clock cycle
      rin <= v;

      -- Outputs
      axilWriteSlave <= r.axilWriteSlave;
      axilReadSlave  <= r.axilReadSlave;

   end process comb;

   seq : process (axilClk) is
   begin
      if (rising_edge(axilClk)) then
         r <= rin after TPD_G;
      end if;
   end process seq;

   U_SyncVec : entity work.SynchronizerVector
      generic map (
         TPD_G   => TPD_G,
         WIDTH_G => 2)
      port map (
         clk        => wibClk,
         dataIn(0)  => r.swFlush,
         dataIn(1)  => timingLinkUp,
         dataOut(0) => swFlush,
         dataOut(1) => linkUp);

   U_CdrSync : entity work.SynchronizerVector
      generic map (
         TPD_G   => TPD_G,
         WIDTH_G => 2)
      port map (
         clk        => cdrClk,
         dataIn(0)  => r.cdrEdgeSel,
         dataIn(1)  => r.cdrDataInv,
         dataOut(0) => cdrEdgeSel,
         dataOut(1) => cdrDataInv);

   U_SyncStatusVector : entity work.SyncStatusVector
      generic map (
         TPD_G          => TPD_G,
         OUT_POLARITY_G => '1',
         CNT_RST_EDGE_G => true,
         CNT_WIDTH_G    => 32,
         WIDTH_G        => STATUS_SIZE_C)
      port map (
         -- Input Status bit Signals (wrClk domain)
         statusIn(1)  => linkUp,
         statusIn(0)  => runEnable,
         -- Output Status bit Signals (rdClk domain)  
         statusOut    => statusOut,
         -- Status Bit Counters Signals (rdClk domain) 
         cntRstIn     => r.cntRst,
         rollOverEnIn => r.rollOverEn,
         cntOut       => statusCnt,
         -- Clocks and Reset Ports
         wrClk        => wibClk,
         rdClk        => axilClk);

end rtl;

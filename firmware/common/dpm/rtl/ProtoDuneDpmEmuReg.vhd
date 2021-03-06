-------------------------------------------------------------------------------
-- File       : ProtoDuneDpmEmuReg.vhd
-- Company    : SLAC National Accelerator Laboratory
-- Created    : 2016-08-08
-- Last update: 2018-08-10
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
use work.ProtoDuneDpmPkg.all;

entity ProtoDuneDpmEmuReg is
   generic (
      TPD_G         : time                  := 1 ns;
      FIBER_ID_G    : Slv3Array(1 downto 0) := (0 => "111", 1 => "111");
      CRATE_ID_G    : Slv5Array(1 downto 0) := (0 => "11001", 1 => "11001");
      SLOT_ID_G     : Slv3Array(1 downto 0) := (0 => "101", 1 => "110");
      SIM_START_G   : sl                    := '0';
      DEFAULT_CNT_G : sl                    := '0');
   port (
      -- Status/Configuration Interface (clk domain)
      clk             : in  sl;
      rst             : in  sl;
      enableTx        : out sl;
      enableTrig      : out sl;
      sendCnt         : out sl;
      loopback        : out sl;
      cmNoiseCgf      : out slv(3 downto 0);
      chNoiseCgf      : out Slv4Array(127 downto 0);
      chDlyCfg        : out EmuDlyCfg;
      txPreCursor     : out slv(4 downto 0);
      txPostCursor    : out slv(4 downto 0);
      txDiffCtrl      : out slv(3 downto 0);
      fiberId         : out Slv3Array(1 downto 0);
      crateId         : out Slv5Array(1 downto 0);
      slotId          : out Slv3Array(1 downto 0);
      -- AXI-Lite Interface (axilClk domain)
      axilClk         : in  sl;
      axilRst         : in  sl;
      axilReadMaster  : in  AxiLiteReadMasterType;
      axilReadSlave   : out AxiLiteReadSlaveType;
      axilWriteMaster : in  AxiLiteWriteMasterType;
      axilWriteSlave  : out AxiLiteWriteSlaveType);
end ProtoDuneDpmEmuReg;

architecture rtl of ProtoDuneDpmEmuReg is

   -- constant STATUS_SIZE_C : positive := 10;

   type RegType is record
      enableTx       : sl;
      enableTrig     : sl;
      sendCnt        : sl;
      loopback       : sl;
      cmNoiseCgf     : slv(3 downto 0);
      chNoiseCgf     : Slv4Array(127 downto 0);
      chDlyCfg       : EmuDlyCfg;
      txPreCursor    : slv(4 downto 0);
      txPostCursor   : slv(4 downto 0);
      txDiffCtrl     : slv(3 downto 0);
      fiberId        : Slv3Array(1 downto 0);
      crateId        : Slv5Array(1 downto 0);
      slotId         : Slv3Array(1 downto 0);
      -- cntRst         : sl;
      -- rollOverEn     : slv(STATUS_SIZE_C-1 downto 0);
      hardRst        : sl;
      axilReadSlave  : AxiLiteReadSlaveType;
      axilWriteSlave : AxiLiteWriteSlaveType;
   end record;

   constant REG_INIT_C : RegType := (
      enableTx       => SIM_START_G,
      enableTrig     => '0',
      sendCnt        => DEFAULT_CNT_G,
      loopback       => '0',
      cmNoiseCgf     => "0000",
      chNoiseCgf     => (others => (others => '0')),
      chDlyCfg       => (others => (others => '0')),
      txPreCursor    => "00000",
      txPostCursor   => "01111",
      txDiffCtrl     => "1111",
      fiberId        => FIBER_ID_G,
      crateId        => CRATE_ID_G,
      slotId         => SLOT_ID_G,
      -- cntRst         => '1',
      -- rollOverEn     => (others => '0'),
      hardRst        => '0',
      axilReadSlave  => AXI_LITE_READ_SLAVE_INIT_C,
      axilWriteSlave => AXI_LITE_WRITE_SLAVE_INIT_C);

   signal r   : RegType := REG_INIT_C;
   signal rin : RegType;

   -- signal statusOut : slv(STATUS_SIZE_C-1 downto 0);
   -- signal statusCnt : SlVectorArray(STATUS_SIZE_C-1 downto 0, 31 downto 0);

   -- attribute dont_touch               : string;
   -- attribute dont_touch of r          : signal is "TRUE";

begin

   comb : process (axilReadMaster, axilRst, axilWriteMaster, r) is
      variable v      : RegType;
      variable regCon : AxiLiteEndPointType;
   begin
      -- Latch the current value
      v := r;

      -- Reset the strobes
      v.hardRst := '0';
      -- v.cntRst := '0';

      -- Check for hard reset
      if (r.hardRst = '1') then
         -- Reset the register
         v := REG_INIT_C;
      end if;

      -- Determine the transaction type
      axiSlaveWaitTxn(regCon, axilWriteMaster, axilReadMaster, v.axilWriteSlave, v.axilReadSlave);

      -- -- Map the read registers
      -- for i in STATUS_SIZE_C-1 downto 0 loop
      -- axiSlaveRegisterR(regCon, toSlv((4*i), 12), 0, muxSlVectorArray(statusCnt, i));
      -- end loop;
      -- axiSlaveRegisterR(regCon, x"400", 0, statusOut);

      -- Map the write registers
      for i in 127 downto 0 loop
         axiSlaveRegister(regCon, toSlv(2048+(4*i), 12), 0, v.chDlyCfg(i));  -- [0x800:0x9FF]
         axiSlaveRegister(regCon, toSlv(2048+(4*i), 12), 16, v.chNoiseCgf(i));  -- [0x800:0x9FF]
      end loop;
      axiSlaveRegister(regCon, x"C00", 0, v.enableTx);
      axiSlaveRegister(regCon, x"C04", 0, v.enableTrig);
      axiSlaveRegister(regCon, x"C08", 0, v.loopback);
      axiSlaveRegister(regCon, x"C0C", 0, v.cmNoiseCgf);
      -- axiSlaveRegister(regCon, x"C10", 0, v.trigRate);
      axiSlaveRegister(regCon, x"C14", 0, v.txPreCursor);
      axiSlaveRegister(regCon, x"C18", 0, v.txPostCursor);
      axiSlaveRegister(regCon, x"C1C", 0, v.txDiffCtrl);
      axiSlaveRegister(regCon, x"C20", 0, v.sendCnt);

      axiSlaveRegister(regCon, x"C30", 0, v.fiberId(0));
      axiSlaveRegister(regCon, x"C34", 0, v.crateId(0));
      axiSlaveRegister(regCon, x"C38", 0, v.slotId(0));

      axiSlaveRegister(regCon, x"C40", 0, v.fiberId(1));
      axiSlaveRegister(regCon, x"C44", 0, v.crateId(1));
      axiSlaveRegister(regCon, x"C48", 0, v.slotId(1));

      -- axiSlaveRegister(regCon, x"FF0", 0, v.rollOverEn);
      -- axiSlaveRegister(regCon, x"FF4", 0, v.cntRst);
      axiSlaveRegister(regCon, x"FFC", 0, v.hardRst);

      -- Closeout the transaction
      axiSlaveDefault(regCon, v.axilWriteSlave, v.axilReadSlave, AXI_RESP_DECERR_C);

      -- Check prevent invalid configurations
      if (v.cmNoiseCgf > 12) then
         v.cmNoiseCgf := r.cmNoiseCgf;
      end if;
      for i in 127 downto 0 loop
         if (v.chNoiseCgf(i) > 12) then
            v.chNoiseCgf(i) := r.chNoiseCgf(i);
         end if;
      end loop;

      -- Synchronous Reset
      if (axilRst = '1') then
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

   U_SyncOutVec : entity work.SynchronizerVector
      generic map (
         TPD_G   => TPD_G,
         WIDTH_G => 4)
      port map (
         clk        => clk,
         dataIn(0)  => r.enableTx,
         dataIn(1)  => r.enableTrig,
         dataIn(2)  => r.sendCnt,
         dataIn(3)  => r.loopback,
         dataOut(0) => enableTx,
         dataOut(1) => enableTrig,
         dataOut(2) => sendCnt,
         dataOut(3) => loopback);

   -- Since these configurations won't change 
   -- during a run,not using a SYNC module to 
   -- save resources and tagging AXI-Lite 
   -- and EMU as ASYNC in .XDC
   chDlyCfg     <= r.chDlyCfg;
   cmNoiseCgf   <= r.cmNoiseCgf;
   chNoiseCgf   <= r.chNoiseCgf;
   txPreCursor  <= r.txPreCursor;
   txPostCursor <= r.txPostCursor;
   txDiffCtrl   <= r.txDiffCtrl;
   fiberId      <= r.fiberId;
   crateId      <= r.crateId;
   slotId       <= r.slotId;

end rtl;

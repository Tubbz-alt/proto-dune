//-----------------------------------------------------------------------------
// File          : AxiStreamDmaRing.h
// Author        : Ben Reese <bareese@slac.stanford.edu>
// Created       : 11/12/2013
// Project       : 
//-----------------------------------------------------------------------------
// Description :
//    DAQ Device Driver for the StdLib/axi/rtl/AxiStreamDmaRing.vhd
//-----------------------------------------------------------------------------
// This file is part of 'SLAC Generic DAQ Software'.
// It is subject to the license terms in the LICENSE.txt file found in the 
// top-level directory of this distribution and at: 
//    https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
// No part of 'SLAC Generic DAQ Software', including this file, 
// may be copied, modified, propagated, or distributed except according to 
// the terms contained in the LICENSE.txt file.
// Proprietary and confidential to SLAC.
//-----------------------------------------------------------------------------
// Modification history :
// 11/12/2013: created
//-----------------------------------------------------------------------------
#ifndef __AXI_STREAM_DMA_RING_H__
#define __AXI_STREAM_DMA_RING_H__

#include <Device.h>
#include <stdint.h>
using namespace std;

//! Class to contain AxiStreamDmaRing
class AxiStreamDmaRing : public Device {

   public:

      //! Constructor
      /*! 
       * \param linkConfig Device linkConfig
       * \param baseAddress Device base address
       * \param index       Device index
       * \param parent      Parent device
      */
      AxiStreamDmaRing ( uint32_t linkConfig, uint32_t baseAddress, uint32_t index,
                         Device *parent, uint32_t numBuffers );

      //! Deconstructor
      ~AxiStreamDmaRing ( );

      //! Method to process a command
      /*!
       * \param name     Command name
       * \param arg      Optional arg
      */
      void command ( string name, string arg );

      //! Method to read status registers and update variables
      //void readStatus ( );

      //! Method to read configuration registers and update variables
      /*!
       * Throws string error.
       */
      //void readConfig ( );

      //! Method to write configuration registers
      /*! 
       * Throws string on error.
       * \param force Write all registers if true, only stale if false
      */
      //void writeConfig ( bool force );
    private:
      enum BufferVars { START=0, END, NEXT, TRIG, MODE, STATUS };
      uint32_t getBufferAddr(uint32_t var, uint32_t buf);
    
};

#endif

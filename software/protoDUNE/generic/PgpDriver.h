/**
 *-----------------------------------------------------------------------------
 * Title      : PGP Card Driver, Shared Header
 * ----------------------------------------------------------------------------
 * File       : PgpDriver.h
 * Author     : Ryan Herbst, rherbst@slac.stanford.edu
 * Created    : 2016-08-08
 * Last update: 2016-08-08
 * ----------------------------------------------------------------------------
 * Description:
 * Defintions and inline functions for interacting with PGP driver.
 * ----------------------------------------------------------------------------
 * This file is part of the PGP card driver. It is subject to 
 * the license terms in the LICENSE.txt file found in the top-level directory 
 * of this distribution and at: 
 *    https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
 * No part of the PGP card driver, including this file, may be 
 * copied, modified, propagated, or distributed except according to the terms 
 * contained in the LICENSE.txt file.
 * ----------------------------------------------------------------------------
**/
#ifndef __PGP_DRIVER_H__
#define __PGP_DRIVER_H__

#ifdef PGP_IN_KERNEL
#include <linux/types.h>
#else
#include <stdint.h>
#endif

// Card Info
struct PgpInfo {
   uint64_t serial;
   uint32_t type;
   uint32_t version;
   uint32_t laneMask;
   uint32_t vcPerMask;
   uint32_t pgpRate;
   uint32_t promPrgEn;
   uint32_t evrSupport;
   uint32_t pad;
   char     buildStamp[256];
};


// PCI Info
struct PciStatus {
   uint32_t pciCommand;
   uint32_t pciStatus;
   uint32_t pciDCommand;
   uint32_t pciDStatus;
   uint32_t pciLCommand;
   uint32_t pciLStatus;
   uint32_t pciLinkState;
   uint32_t pciFunction;
   uint32_t pciDevice;
   uint32_t pciBus;
   uint32_t pciLanes;
};


// Lane status
struct PgpStatus {
   uint32_t lane;
   uint32_t loopBack;
   uint32_t locLinkReady;
   uint32_t remLinkReady;
   uint32_t rxReady;
   uint32_t txReady;
   uint32_t rxCount;
   uint32_t cellErrCnt;
   uint32_t linkDownCnt;
   uint32_t linkErrCnt;
   uint32_t fifoErr;
   uint32_t remData;
   uint32_t remBuffStatus;
};


// EVR Control, per lane
struct PgpEvrControl {
   uint32_t  lane;
   uint32_t  evrEnable;     // Global flag
   uint32_t  laneRunMask;   // 1 = Run trigger enable
   uint32_t  evrSyncEn;     // 1 = Start, 0 = Stop
   uint32_t  evrSyncSel;    // 0 = async, 1 = sync for start/stop
   uint32_t  headerMask;    // 1 = Enable header data checking, one bit per VC (4 bits)
   uint32_t  evrSyncWord;   // fiducial to transition start stop
   uint32_t  runCode;       // Run code
   uint32_t  runDelay;      // Run delay
   uint32_t  acceptCode;    // Accept code
   uint32_t  acceptDelay;   // Accept delay
};


// EVR Status, per lane
struct PgpEvrStatus {
   uint32_t  lane;
   uint32_t  linkErrors;
   uint32_t  linkUp;
   uint32_t  runStatus;    // 1 = Running, 0 = Stopped
   uint32_t  evrSeconds;
   uint32_t  runCounter;
   uint32_t  acceptCounter;
};


// Card Types
#define PGP_NONE     0x00
#define PGP_GEN1     0x01
#define PGP_GEN2     0x02
#define PGP_GEN2_VCI 0x12
#define PGP_GEN3     0x03
#define PGP_GEN3_VCI 0x13

// Error values
#define PGP_ERR_EOFE 0x01
#define PGP_ERR_FIFO 0x02
#define PGP_ERR_LEN  0x04
#define PGP_ERR_MAX  0x08

// Send Frame
// Returns transmit size
// inline ssize_t pgpWrite(int32_t fd, void * buf, size_t size, uint8_t lane, uint8_t vc, uint8_t cont);

// Receive Frame
// Returns receive size
// inline ssize_t pgpRead(int32_t fd, void * buf, size_t maxSize, uint8_t * lane, uint8_t * vc, uint8_t * error, uint8_t * cont);

// Receive Frame, access memory mapped buffer
// Returns receive size
// inline ssize_t pgpReadIndex(int32_t fd, uint32_t * index, uint8_t * lane, uint8_t * vc, uint8_t * error, uint8_t * cont);

// Return Index
// inline ssize_t pgpRetIndex(int32_t fd, uint32_t index);

// Return user space mapping to dma buffers
// inline void ** pgpMapDma(int32_t fd, uint32_t *count, uint32_t *size);

// Free space mapping to dma buffers
// inline ssize_t pgpUnMapDma(int2_t fd, void ** buffer);

// Read Card Info
// inline ssize_t pgpGetInfo(int32_t fd, struct PgpInfo * info);

// Read PCI Status
// inline ssize_t pgpGetPci(int32_t fd, struct PciStatus * status);

// Read Lane Status
// inline ssize_t pgpGetStatus(int32_t fd, uint8_t lane, struct PgpStatus * status);

// Set debug
// inline ssize_t pgpSetDebug(int32_t fd, uint8_t level);

// Set Loopback State For Lane
// inline ssize_t pgpSetLoop(int32_t fd, uint8_t lane, uint8_t state);

// Reset counters
// inline ssize_t pgpCountReset(int32_t fd);

// Set Sideband Data
// inline ssize_t pgpSetData(int32_t fd, uint32_t lane, uint8_t data);

// Send OpCode
// inline ssize_t pgpSendOpCode(int32_t fd, uint32_t code);

// set lane/vc rx mask, one bit per vc
// inline ssize_t pgpSetMask(int32_t fd, uint32_t mask);

// Set EVR Control
// inline ssize_t pgpSetEvrControl(int32_t fd, uint32_t lane, struct PgpEvrControl * control);

// Get EVR Control
// inline ssize_t pgpGetEvrControl(int32_t fd, uint32_t lane, struct PgpEvrControl * control);

// Get EVR Status
// inline ssize_t pgpGetEvrStatus(int32_t fd, uint32_t lane, struct PgpEvrStatus * status);

// Reset EVR Counters
// inline ssize_t pgpResetEvrCount(int32_t fd, uint32_t lane);

// Write to PROM
// inline ssize_t pgpWriteProm(int32_t fd, uint32_t address, uint32_t cmd, uint32_t data);

// Read from PROM
// inline ssize_t pgpReadProm(int32_t fd, uint32_t address, uint32_t cmd, uint32_t *data);

// Assign interrupt handler
// inline void pgpAssignHandler (int32_t fd, void (*handler)(int32_t))

// Commands
#define PGP_Get_Rx_Count   0x1001
#define PGP_Get_Rx_Size    0x1002
#define PGP_Read_Info      0x1003
#define PGP_Read_Pci       0x1004
#define PGP_Read_Status    0x1005
#define PGP_Set_Debug      0x1006
#define PGP_Set_Loop       0x1007
#define PGP_Count_Reset    0x1008
#define PGP_Send_OpCode    0x1009
#define PGP_Set_Data       0x100A
#define PGP_Set_Mask       0x100B
#define PGP_Set_Evr_Cntrl  0x100C
#define PGP_Get_Evr_Cntrl  0x100D
#define PGP_Get_Evr_Status 0x100E
#define PGP_Rst_Evr_Count  0x100F
#define PGP_Write_Prom     0x1010
#define PGP_Read_Prom      0x1011

// TX Structure
// Size = 0 for return index
struct PgpWriteData {
   uint64_t  data;
   uint32_t  lane;
   uint32_t  vc;
   uint32_t  cont;
   uint32_t  index;
   uint32_t  is32;
   uint32_t  size;
};

// RX Structure
// Data = 0 for read index
struct PgpReadData {
   uint64_t   data;
   uint32_t   lane;
   uint32_t   vc;
   uint32_t   cont;
   uint32_t   index;
   uint32_t   size;
   uint32_t   error;
   uint32_t   is32;
   uint32_t   pad;
};

// Prom Programming 
struct PgpPromData {
   uint32_t address;
   uint32_t cmd;
   uint32_t data;
   uint32_t pad;
};

// Everything below is hidden during kernel module compile
#ifndef PGP_IN_KERNEL
#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/signal.h>
#include <sys/fcntl.h>

// Write Frame
inline ssize_t pgpWrite(int32_t fd, void * buf, size_t size, uint32_t lane, uint32_t vc, uint32_t cont=0) {
   struct PgpWriteData w;

   memset(&w,0,sizeof(struct PgpWriteData));
   w.lane    = lane;
   w.vc      = vc;
   w.cont    = cont;
   w.size    = size;
   w.is32    = (sizeof(void *)==4);
   w.data    = (uint64_t)buf;

   return(write(fd,&w,sizeof(struct PgpWriteData)));
}

// Receive Frame
inline ssize_t pgpRead(int32_t fd, void * buf, size_t maxSize, uint32_t * lane, uint32_t * vc, uint32_t * error, uint32_t * cont=NULL) {
   struct PgpReadData r;
   ssize_t ret;

   memset(&r,0,sizeof(struct PgpReadData));
   r.size = maxSize;
   r.is32 = (sizeof(void *)==4);
   r.data = (uint64_t)buf;

   ret = read(fd,&r,sizeof(struct PgpReadData));

   if ( lane  != NULL ) *lane  = r.lane;
   if ( vc    != NULL ) *vc    = r.vc;
   if ( error != NULL ) *error = r.error;
   if ( cont  != NULL ) *cont  = r.cont;

   return(ret);
}

// Receive Frame, access memory mapped buffer
// Returns receive size
inline ssize_t pgpReadIndex(int32_t fd, uint32_t * index, uint32_t * lane, uint32_t * vc, uint32_t * error, uint32_t * cont=NULL) {
   struct PgpReadData r;
   size_t ret;

   memset(&r,0,sizeof(struct PgpReadData));

   ret = read(fd,&r,sizeof(struct PgpReadData));

   if ( lane  != NULL ) *lane  = r.lane;
   if ( vc    != NULL ) *vc    = r.vc;
   if ( error != NULL ) *error = r.error;
   if ( cont  != NULL ) *cont  = r.cont;
   if ( index != NULL ) *index = r.index;

   return(ret);
}

// Post Index
inline ssize_t pgpRetIndex(int32_t fd, uint32_t index) {
   struct PgpWriteData w;

   memset(&w,0,sizeof(struct PgpWriteData));
   w.index = index;

   return(write(fd,&w,sizeof(struct PgpWriteData)));
}

// Return user space mapping to dma buffers
inline void ** pgpMapDma(int32_t fd, uint32_t *count, uint32_t *size) {
   void *   temp;
   void **  ret;
   uint32_t bCount;
   uint32_t bSize;
   uint32_t x;

   bSize  = ioctl(fd,PGP_Get_Rx_Size,0);
   bCount = ioctl(fd,PGP_Get_Rx_Count,0);

   if ( count != NULL ) *count = bCount;
   if ( size  != NULL ) *size  = bSize;

   if ( (ret = (void **)malloc(sizeof(void *) * bCount)) == 0 ) return(NULL);

   for (x=0; x < bCount; x++) {

      if ( (temp = mmap (0, bSize, PROT_READ, MAP_PRIVATE, fd, (bSize*x))) == MAP_FAILED) {
         free(ret);
         return(NULL);
      }

      ret[x] = temp;
   }

   return(ret);
}

// Free space mapping to dma buffers
inline ssize_t pgpUnMapDma(int32_t fd, void ** buffer) {
   uint32_t  bCount;
   uint32_t  bSize;
   uint32_t  x;;

   bCount = ioctl(fd,PGP_Get_Rx_Count,0);
   bSize  = ioctl(fd,PGP_Get_Rx_Size,0);

   // I don't think this is correct.....
   for (x=0; x < bCount; x++) munmap (buffer, bSize);

   free(buffer);
   return(0);
}

// Read Card Info
inline ssize_t pgpGetInfo(int32_t fd, struct PgpInfo * info) {
   return(ioctl(fd,PGP_Read_Info,info));
}

// Read PCI Status
inline ssize_t pgpGetPci(int32_t fd, struct PciStatus * status) {
   return(ioctl(fd,PGP_Read_Pci,status));
}

// Read Lane Status
inline ssize_t pgpGetStatus(int32_t fd, uint32_t lane, struct PgpStatus * status) {
   status->lane = lane;
   return(ioctl(fd,PGP_Read_Status,status));
}

// Set debug
inline ssize_t pgpSetDebug(int32_t fd, uint32_t level) {
   return(ioctl(fd,PGP_Set_Debug,level));
}


// Set Loopback State For Lane
inline ssize_t pgpSetLoop(int32_t fd, uint32_t lane, uint32_t state) {
   uint32_t temp;

   temp = lane & 0xFF;
   temp |= ((state << 8) & 0x100);

   return(ioctl(fd,PGP_Set_Loop,temp));
}


// Reset counters
inline ssize_t pgpCountReset(int32_t fd) {
   return(ioctl(fd,PGP_Count_Reset,0));
}


// Set Sideband Data
inline ssize_t pgpSetData(int32_t fd, uint32_t lane, uint32_t data) {
   uint32_t temp;

   temp = lane & 0xFF;
   temp |= ((data << 8) & 0xFF00);

   return(ioctl(fd,PGP_Set_Data,temp));
}

// Send OpCode
inline ssize_t pgpSendOpCode(int32_t fd, uint32_t code) {
   return(ioctl(fd,PGP_Send_OpCode,code));
}

// set lane/vc rx mask, one bit per vc
inline ssize_t pgpSetMask(int32_t fd, uint32_t mask) {
   return(ioctl(fd,PGP_Set_Mask,mask));
}


// Set EVR Control
inline ssize_t pgpSetEvrControl(int32_t fd, uint32_t lane, struct PgpEvrControl * control) {
   control->lane = lane;
   return(ioctl(fd,PGP_Set_Evr_Cntrl,control));
}


// Get EVR Control
inline ssize_t pgpGetEvrControl(int32_t fd, uint32_t lane, struct PgpEvrControl * control) {
   control->lane = lane;
   return(ioctl(fd,PGP_Get_Evr_Cntrl,control));
}


// Get EVR Status
inline ssize_t pgpGetEvrStatus(int32_t fd, uint32_t lane, struct PgpEvrStatus * status) {
   status->lane = lane;
   return(ioctl(fd,PGP_Get_Evr_Status,status));
}

// Reset EVR Counters
inline ssize_t pgpResetEvrCount(int32_t fd, uint32_t lane) {
   return(ioctl(fd,PGP_Rst_Evr_Count,lane));
}

// Write to PROM
inline ssize_t pgpWriteProm(int32_t fd, uint32_t address, uint32_t cmd, uint32_t data) {
   struct PgpPromData prom;

   prom.address = address;
   prom.cmd     = cmd;
   prom.data    = data;
   return(ioctl(fd,PGP_Write_Prom,&prom));
}

// Read from PROM
inline ssize_t pgpReadProm(int32_t fd, uint32_t address, uint32_t cmd, uint32_t *data) {
   struct PgpPromData prom;
   ssize_t res;

   prom.address = address;
   prom.cmd     = cmd;
   prom.data    = 0;
   res = ioctl(fd,PGP_Read_Prom,&prom);

   if ( data != NULL ) *data = prom.data;

   return(res);
}

// Assign interrupt handler
inline void pgpAssignHandler (int32_t fd, void (*handler)(int32_t)) {
   struct sigaction act;
   int32_t oflags;

   act.sa_handler = handler;
   sigemptyset(&act.sa_mask);
   act.sa_flags = 0;

   sigaction(SIGIO, &act, NULL);
   fcntl(fd, F_SETOWN, getpid());
   oflags = fcntl(fd, F_GETFL);
   fcntl(fd, F_SETFL, oflags | FASYNC);
}

#endif
#endif


//-----------------------------------------------------------------------------
// File          : JesdRx.cpp
// Author        : Uros legat <ulegat@slac.stanford.edu>
//                            <uros.legat@cosylab.com>
// Created       : 27/04/2015
// Project       : 
//-----------------------------------------------------------------------------
// Description :
//    Device container for Jesd204b
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
// 27/04/2015: created
//-----------------------------------------------------------------------------
#include <JesdRx.h>
#include <Register.h>
#include <RegisterLink.h>
#include <Variable.h>
#include <Command.h>
#include <sstream>
#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

// Constructor
JesdRx::JesdRx ( uint32_t linkConfig, uint32_t baseAddress, uint32_t index, Device *parent, uint32_t addrSize ) : 
                        Device(linkConfig,baseAddress,"JesdRx",index,parent) {
   Command      *c;
   
   // Description
   desc_ = "Common JESD interface object.";

   // Create Registers: name, address
   RegisterLink *rl;
   
   addRegisterLink(rl = new RegisterLink("Enable",           baseAddress_ + (0x00*addrSize), Variable::Configuration));
   rl->getVariable()->setDescription("Enables the RX modules: 0x3 - enables both modules at a time");
   
   
   addRegisterLink(rl = new RegisterLink("SysrefDelay",      baseAddress_ + (0x01*addrSize), Variable::Configuration));
   rl->getVariable()->setDescription("Sets the synchronisation delay in clock cycles");

   //addRegisterLink(rl = new RegisterLink("AXISTrigger",      baseAddress_ + (0x02*addrSize), Variable::Configuration));
   //rl->getVariable()->setDescription("Triggers the AXI stream transfer: 0x3 - triggers both modules at a time");
   
   
   //addRegisterLink(rl = new RegisterLink("AXISpacketSize",   baseAddress_ + (0x03*addrSize), Variable::Configuration));
   //rl->getVariable()->setDescription("Data packet size (when enabled packets are being sent continuously)"); 

   addRegisterLink(rl = new RegisterLink("CommonControl",    baseAddress_ + (0x04*addrSize), 1, 6,
                                "SubClass",              Variable::Configuration, 0, 0x1,
                                "ReplaceEnable",         Variable::Configuration, 1, 0x1,
                                "ResetGTs",              Variable::Configuration, 2, 0x1,
                                "ClearErrors",           Variable::Configuration, 3, 0x1,
                                "InvertSync",            Variable::Configuration, 4, 0x1,
                                "ScrambleEnable",        Variable::Configuration, 5, 0x1));   
     
   //addRegisterLink(rl = new RegisterLink("L1_Test",      baseAddress_ + (0x20*addrSize), 1, 2,
   //                             "L1_Align",         Variable::Configuration, 0, 0xf,
   //                             "L1_Delay",         Variable::Configuration, 8, 0xf));
                                
   // addRegisterLink(rl = new RegisterLink("L2_Test",      baseAddress_ + (0x21*addrSize), 1, 2,
                                // "L2_Align",         Variable::Configuration, 0, 0xf,
                                // "L2_Delay",         Variable::Configuration, 8, 0xf));
                              
   addRegisterLink(rl = new RegisterLink("L1_Test_thr",      baseAddress_ + (0x30*addrSize), 1, 2,
                                "L1_Threshold_Low",         Variable::Configuration, 0,  0xffff,
                                "L1_Threshold_High",        Variable::Configuration, 16, 0xffff));
                                
   addRegisterLink(rl = new RegisterLink("L1_Test_thr",      baseAddress_ + (0x31*addrSize), 1, 2,
                                "L2_Threshold_Low",         Variable::Configuration, 0,  0xffff,
                                "L2_Threshold_High",        Variable::Configuration, 16, 0xffff));

   addRegisterLink(rl = new RegisterLink("L1_Status",    baseAddress_ + (0x10*addrSize), 1, 13,
                                "L1_GTXReady",       Variable::Status, 0, 0x1,
                                "L1_DataValid",     Variable::Status, 1, 0x1, 
                                "L1_AlignErr",      Variable::Status, 2, 0x1,
                                "L1_nSync",         Variable::Status, 3, 0x1,                                 
                                "L1_RxBuffUfl",     Variable::Status, 4, 0x1,
                                "L1_RxBuffOfl",     Variable::Status, 5, 0x1,                                 
                                "L1_PositionErr",   Variable::Status, 6, 0x1,
                                "L1_RxEnabled",     Variable::Status, 7, 0x1,
                                "L1_SysRefDetected",Variable::Status, 8, 0x1,
                                "L1_CommaDetected", Variable::Status, 9, 0x1,                               
                                "L1_DisparityErr",  Variable::Status, 10,0xF,
                                "L1_DecErr",        Variable::Status, 14,0xF,
                                "L1_ElBuffLatency", Variable::Status, 18,0xFF));                                                      
                                
   addRegisterLink(rl = new RegisterLink("L2_Status",     baseAddress_ + (0x11*addrSize), 1, 13,
                                "L2_GTXRdy",        Variable::Status, 0, 0x1,
                                "L2_DataValid",     Variable::Status, 1, 0x1, 
                                "L2_AlignErr",      Variable::Status, 2, 0x1,
                                "L2_nSync",         Variable::Status, 3, 0x1,                                 
                                "L2_RxBuffUfl",     Variable::Status, 4, 0x1,
                                "L2_RxBuffOfl",     Variable::Status, 5, 0x1,                                 
                                "L2_PositionErr",   Variable::Status, 6, 0x1,
                                "L2_RxEnabled",     Variable::Status, 7, 0x1,
                                "L2_SysRefDetected",Variable::Status, 8, 0x1,
                                "L2_CommaDetected", Variable::Status, 9, 0x1,                               
                                "L2_DisparityErr",  Variable::Status, 10,0xF,
                                "L2_DecErr",        Variable::Status, 14,0xF,
                                "L2_ElBuffLatency", Variable::Status, 18,0xFF));
   addRegisterLink(rl = new RegisterLink("L3_Status",    baseAddress_ + (0x12*addrSize), 1, 13,
                                "L3_GTXReady",       Variable::Status, 0, 0x1,
                                "L3_DataValid",     Variable::Status, 1, 0x1, 
                                "L3_AlignErr",      Variable::Status, 2, 0x1,
                                "L3_nSync",         Variable::Status, 3, 0x1,                                 
                                "L3_RxBuffUfl",     Variable::Status, 4, 0x1,
                                "L3_RxBuffOfl",     Variable::Status, 5, 0x1,                                 
                                "L3_PositionErr",   Variable::Status, 6, 0x1,
                                "L3_RxEnabled",     Variable::Status, 7, 0x1,
                                "L3_SysRefDetected",Variable::Status, 8, 0x1,
                                "L3_CommaDetected", Variable::Status, 9, 0x1,                               
                                "L3_DisparityErr",  Variable::Status, 10,0xF,
                                "L3_DecErr",        Variable::Status, 14,0xF,
                                "L3_ElBuffLatency", Variable::Status, 18,0xFF));                                                      
                                
   addRegisterLink(rl = new RegisterLink("L4_Status",     baseAddress_ + (0x13*addrSize), 1, 13,
                                "L4_GTXRdy",        Variable::Status, 0, 0x1,
                                "L4_DataValid",     Variable::Status, 1, 0x1, 
                                "L4_AlignErr",      Variable::Status, 2, 0x1,
                                "L4_nSync",         Variable::Status, 3, 0x1,                                 
                                "L4_RxBuffUfl",     Variable::Status, 4, 0x1,
                                "L4_RxBuffOfl",     Variable::Status, 5, 0x1,                                 
                                "L4_PositionErr",   Variable::Status, 6, 0x1,
                                "L4_RxEnabled",     Variable::Status, 7, 0x1,
                                "L4_SysRefDetected",Variable::Status, 8, 0x1,
                                "L4_CommaDetected", Variable::Status, 9, 0x1,                               
                                "L4_DisparityErr",  Variable::Status, 10,0xF,
                                "L4_DecErr",        Variable::Status, 14,0xF,
                                "L4_ElBuffLatency", Variable::Status, 18,0xFF)); 
   addRegisterLink(rl = new RegisterLink("L5_Status",    baseAddress_ + (0x14*addrSize), 1, 13,
                                "L5_GTXReady",       Variable::Status, 0, 0x1,
                                "L5_DataValid",     Variable::Status, 1, 0x1, 
                                "L5_AlignErr",      Variable::Status, 2, 0x1,
                                "L5_nSync",         Variable::Status, 3, 0x1,                                 
                                "L5_RxBuffUfl",     Variable::Status, 4, 0x1,
                                "L5_RxBuffOfl",     Variable::Status, 5, 0x1,                                 
                                "L5_PositionErr",   Variable::Status, 6, 0x1,
                                "L5_RxEnabled",     Variable::Status, 7, 0x1,
                                "L5_SysRefDetected",Variable::Status, 8, 0x1,
                                "L5_CommaDetected", Variable::Status, 9, 0x1,                               
                                "L5_DisparityErr",  Variable::Status, 10,0xF,
                                "L5_DecErr",        Variable::Status, 14,0xF,
                                "L5_ElBuffLatency", Variable::Status, 18,0xFF));                                                     
                                
   addRegisterLink(rl = new RegisterLink("L6_Status",     baseAddress_ + (0x15*addrSize), 1, 13,
                                "L6_GTXRdy",        Variable::Status, 0, 0x1,
                                "L6_DataValid",     Variable::Status, 1, 0x1, 
                                "L6_AlignErr",      Variable::Status, 2, 0x1,
                                "L6_nSync",         Variable::Status, 3, 0x1,                                 
                                "L6_RxBuffUfl",     Variable::Status, 4, 0x1,
                                "L6_RxBuffOfl",     Variable::Status, 5, 0x1,                                 
                                "L6_PositionErr",   Variable::Status, 6, 0x1,
                                "L6_RxEnabled",     Variable::Status, 7, 0x1,
                                "L6_SysRefDetected",Variable::Status, 8, 0x1,
                                "L6_CommaDetected", Variable::Status, 9, 0x1,                               
                                "L6_DisparityErr",  Variable::Status, 10,0xF,
                                "L6_DecErr",        Variable::Status, 14,0xF,
                                "L6_ElBuffLatency", Variable::Status, 18,0xFF));
   
   addRegisterLink(rl = new RegisterLink("L1_StatusValidCnt",      baseAddress_ + (0x40*addrSize), Variable::Status));
   rl->getVariable()->setDescription("Shows stability of JESD lanes. Counts number of JESD re-syncronisations.");
   addRegisterLink(rl = new RegisterLink("L2_StatusValidCnt",      baseAddress_ + (0x41*addrSize), Variable::Status));
   rl->getVariable()->setDescription("Shows stability of JESD lanes. Counts number of JESD re-syncronisations.");
   addRegisterLink(rl = new RegisterLink("L3_StatusValidCnt",      baseAddress_ + (0x42*addrSize), Variable::Status));
   rl->getVariable()->setDescription("Shows stability of JESD lanes. Counts number of JESD re-syncronisations.");
   addRegisterLink(rl = new RegisterLink("L4_StatusValidCnt",      baseAddress_ + (0x43*addrSize), Variable::Status));
   rl->getVariable()->setDescription("Shows stability of JESD lanes. Counts number of JESD re-syncronisations.");
   addRegisterLink(rl = new RegisterLink("L5_StatusValidCnt",      baseAddress_ + (0x44*addrSize), Variable::Status));
   rl->getVariable()->setDescription("Shows stability of JESD lanes. Counts number of JESD re-syncronisations.");
   addRegisterLink(rl = new RegisterLink("L6_StatusValidCnt",      baseAddress_ + (0x45*addrSize), Variable::Status));
   rl->getVariable()->setDescription("Shows stability of JESD lanes. Counts number of JESD re-syncronisations.");   
                                
   // Variables
   getVariable("Enabled")->setHidden(true);
   
   //Commands
   addCommand(c = new Command("ClearErrors"));
   c->setDescription("Clear the registered errors of all RX lanes.");
   
   addCommand(c = new Command("RestartGTs"));
   c->setDescription("Toggle the reset of all RX MGTs.");

}

// Deconstructor
JesdRx::~JesdRx ( ) { }

// Process Commands
void JesdRx::command(string name, string arg) {
   if (name == "ClearErrors") clrErrors();
   else if (name == "RestartGTs") rstGts();
   else Device::command(name,arg);
}

//! Clear errors
void JesdRx::clrErrors () {

   Register *r;
   REGISTER_LOCK
   r = getRegister("CommonControl");
   r->set(0x1,3,0x1);
   writeRegister(r, true);
   r->set(0x0,3,0x1);
   writeRegister(r, true);
   REGISTER_UNLOCK

}

//! Reset GTs
void JesdRx::rstGts () {

   Register *r;
   REGISTER_LOCK
   r = getRegister("CommonControl");
   r->set(0x1,2,0x1);
   writeRegister(r, true);
   r->set(0x0,2,0x1);
   writeRegister(r, true);
   REGISTER_UNLOCK

}




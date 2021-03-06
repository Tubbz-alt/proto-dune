//////////////////////////////////////////////////////////////////////////////
// This file is part of 'AXI Stream DMA Core'.
// It is subject to the license terms in the LICENSE.txt file found in the 
// top-level directory of this distribution and at: 
//    https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
// No part of 'AXI Stream DMA Core', including this file, 
// may be copied, modified, propagated, or distributed except according to 
// the terms contained in the LICENSE.txt file.
//////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>
#include <iostream>
#include <iomanip>
#include <signal.h>
#include "../driverV3/AxiStreamDma.h"
using namespace std;
 
int main(int argc, char **argv) {
   uint              buffSize;
   unsigned char   * rxData;
   uint              lastSize;
   uint              rxSize;
   time_t            lastTme;
   time_t            currTme;
   uint              count;
   uint              total;
   int               maxFd;
   struct timeval    timeout;
   fd_set            fds;
   int               fd;
   uint              loop;
   uint              errors;

   fd = open("/dev/axi_stream_dma_2",O_RDWR|O_NONBLOCK);

   if ( fd < 0 ) {
      printf("Error opening device\n");
      return(1);
   }

   buffSize  = axisGetReadBufferSize(fd);
   rxData = (unsigned char *)malloc(buffSize);

   time(&lastTme);
   currTme = lastTme;
   count = 0;
   loop  = 0;
   total = 0;
   lastSize = 0;
   errors   = 0;

   maxFd = fd;

   while (1) {
      timeout.tv_sec  = 0;
      timeout.tv_usec = 10000;
      FD_ZERO(&fds);
      FD_SET(fd,&fds); 
      loop = 0;

      if ( select(maxFd+1,&fds,NULL, NULL, &timeout) > 0 ) {
         if ( FD_ISSET(fd,&fds) ) {
            do {
               rxSize = axisRead(fd,rxData,buffSize,0,0,0);
               if (rxSize > 0 ) {
                  lastSize = rxSize;
                  count++;
                  total++;
               }
               else if ( rxSize < 0 ) errors++;
               loop++;
            } while((rxSize > 0) & (loop < 100));
         }
      }

      time(&currTme);
      if ( currTme != lastTme ) {
         printf("Total %i, Rate %i hz, Size %i, %i MBps, %i Mbps, %i Errors\n",
            total, count, lastSize, (count*lastSize)/(1024*1024), ((count*lastSize)/(1024*1024))*8, errors);

         if ( count == 0 ) total = 0;
         count = 0;
         lastTme = currTme;
      }
   }
}


// -*-Mode: C;-*-

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <inttypes.h>
#include <getopt.h>




/* ---------------------------------------------------------------------- *//*!

  \struct _Prms
  \brief  The configuration parameters
                                                                          *//*!
  \typedef Prms
  \brief   Typedef for struct _Prms
                                                                          */
/* ---------------------------------------------------------------------- */
struct _Prms
{
   int portNumber;   /*!< The port to connect on the RCE side            */
   int    rcvSize;   /*!< Size of the receiver buffer in bytes           */
   int       data;   /*!< The number of data words to print              */
   int    nodelay;   /*!< Value of the TCP_NODELAY parameter             */
   int  nfailures;  /*!< Maximum number of failure messages              */
   char   chkData;   /*!< Perform the data check                         */
};
/* ---------------------------------------------------------------------- */
typedef struct _Prms Prms;
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Retrieve of set defaults for the governing parameters
                                                                          */
/* ---------------------------------------------------------------------- */
static void getPrms (Prms *prms, int argc, char *const argv[])
{
    int c;
    int  portNumber =       8991;
    int  rcvSize    = 128 * 1024;
    int  data       =          0;
    char chkData    =          0;
    int  nodelay    =          0;
    int  nfailures  =         25;


    while ( (c = getopt (argc, argv, "f:p:r:xd:n:")) != EOF)
    {
       if       (c == 'f') nfailures = strtoul (optarg, NULL, 0);
       else if  (c == 'p') portNumber = strtoul (optarg, NULL, 0);
       else if  (c == 'r') rcvSize    = strtoul (optarg, NULL, 0);
       else if  (c == 'd') data       = strtoul (optarg, NULL, 0);
       else if  (c == 'n') nodelay    = strtoul (optarg, NULL, 0);
       else if  (c == 'x') chkData    = 1;
    }


    prms->portNumber = portNumber;
    prms->rcvSize    = rcvSize;
    prms->data       = data;
    prms->chkData    = chkData;
    prms->nfailures  = nfailures;
    prms->nodelay    = nodelay != 0;

    return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
typedef struct RcvProfile_s
{
   unsigned int       cnt;  /*!< The number of reads                      */
   unsigned int       tot;  /*!< Total number of bytes read               */
   unsigned int hist[256];  /*!< The size, in bytes of each read          */
}
RcvProfile;
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Prints the rcv read history

  \param[in] reads   The read history
  \param[in] title   String to label the history
                                                                          */
/* ---------------------------------------------------------------------- */
static void rcvProfile_print (RcvProfile const *profile, char const *title)
{
   int          idx;
   int          tot = 0;
   unsigned int cnt = profile->cnt;


   /* Limit the number of history items to print to no more than the max */
   if (cnt > sizeof (profile->hist) / sizeof (profile->hist[0]))
   {
      cnt = sizeof (profile->hist) / sizeof (profile->hist[0]);
   }

   
   /* 
    | Calculate the total received, this must match profile->tot 
    | unless there was an error of the total number of reads exceeded
    | the number of history entries/
   */
   for (idx = 0; idx < cnt; idx++) tot += profile->hist[idx];
   printf ("%s  nreads = %u %u/%u\n", title, cnt, tot, profile->tot);
   

   /* Print the read size history */
   int col = 0;
   for (idx = 0;  idx < cnt; idx++)
   {
      col = idx % 10;
      if (col == 0) printf (" %3u:", idx);
      printf (" %6.6x", profile->hist[idx]);
      if (col == 9) putchar ('\n');
   }

   if (col != 9) putchar ('\n');
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

   \enum  ERR_M
   \brief  Enumerates error bits used when checking the header and data
           for integrity
                                                                          */
/* ---------------------------------------------------------------------- */
enum ERR_M
{
   ERR_M_SEQNUM = (1 << 0),  /*!< Error bit for bad sequence number       */
   ERR_M_DATSIZ = (1 << 1),  /*!< Error bit for bad data size             */
   ERR_M_DATVAL = (1 << 2),  /*!< Error in data value                     */
};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \struct _Statistics 
  \brief   Keeps track of the statistics
                                                                          *//*!
  \typedef Statistics 
  \brief   Typedef for struct _Statistics
                                                                          */
/* ---------------------------------------------------------------------- */
struct _Statistics 
{
   uint32_t    hdrCnt;  /*!< The number of header packets seen            */
   uint32_t    hdrSiz;  /*!< The number of header bytes   received        */
   uint32_t    datCnt;  /*!< The number of data   packets seen            */
   uint32_t    datSiz;  /*!< The number of data   bytes   received        */
   uint32_t    rcvSiz;  /*!< Number of bytes received                     */
   uint32_t    lstSiz;  /*!< Size, in bytes of the last packet received   */
};
/* ---------------------------------------------------------------------- */
typedef struct _Statistics Statistics;
/* ---------------------------------------------------------------------- */
   


/* ---------------------------------------------------------------------- *//*!

  \struct _Errors 
  \brief   Keeps track of the errors
                                                                          *//*!
  \typedef Errors
  \brief   Typedef for struct _Errors
                                                                          */
/* ---------------------------------------------------------------------- */
struct _Errors 
{
   uint32_t seqNum;  /*!< Number of sequence errors                       */
   uint32_t datSiz;  /*!< Number of bad sizes                             */
   uint32_t datVal;  /*!< Number of times data packet had a bad data value*/
};
/* ---------------------------------------------------------------------- */
typedef struct _Errors Errors;
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \struct _Ctx
  \brief   The context used to track the data
                                                                          *//*!
  \typedef Ctx
  \brief   Typedef for struct _Ctx
                                                                          */
/* ---------------------------------------------------------------------- */
struct _Ctx
{
   uint32_t       seqNum;  /*!< The packet sequence number                */
   uint32_t       datMax;  /*!< The maximum size of a data packet         */
   time_t      timestamp;  /*!< The time of the last read                 */
   Statistics      stats;  /*!< The receive statistics                    */
   Errors           errs;  /*!< The error statistics                      */
   int         nfailures;  /*!< Number of failures                        */
   uint32_t history[256];  /*!< History of sequence number by index       */
};
/* ---------------------------------------------------------------------- */
typedef struct _Ctx Ctx;
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- *//*!

  \brief Invalidates the checking. This is typically done as part of the
         initialization or after an error.

  \param[in] ctx The check context to reset
                                                                          */
/* ---------------------------------------------------------------------- */
static inline void invalidate_ctx (Ctx *ctx)
{
   ctx->seqNum = 0xffffffff;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Clears the statistics counters

  \param[in] statistics The statistics to clear
                                                                          */
/* ---------------------------------------------------------------------- */
static inline void clear_statistics (Statistics *statistics)
{
   memset (statistics, 0, sizeof (*statistics));
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Clears the error counters

  \param[in] errors The errors to clear
                                                                          */
/* ---------------------------------------------------------------------- */
static inline void clear_errors (Errors *errors)
{
   memset (errors, 0, sizeof (*errors));
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Clears the statistics and error counters

  \param[in] ctx The check context to clear
                                                                          */
/* ---------------------------------------------------------------------- */
static inline void clear_ctx (Ctx *ctx)
{
   clear_statistics (&ctx->stats);
   clear_errors     (&ctx->errs);
   return;
}
/* ---------------------------------------------------------------------- */
           


/* ---------------------------------------------------------------------- *//*!

  \brief Initializes the data tracking context

  \param[in] ctx The context to initialize
                                                                          */
/* ---------------------------------------------------------------------- */
static inline void init_ctx (Ctx *ctx)
{
   ctx->datMax    = 0x4075c;
   ctx->nfailures =       0;
   invalidate_ctx     (ctx);
   clear_ctx          (ctx);
   time   (&ctx->timestamp);

   memset (ctx->history, 0xff, sizeof (ctx->history));
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Extracts one 32-bit word from the \a data stream
  \return The extract 32-bit word

  \param[in] data  The data stream
                                                                          */
/* ---------------------------------------------------------------------- */
static inline uint64_t get_w64 (uint8_t const *data)
{
   uint64_t w = *(uint64_t const *)data;
   return w;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Checks the integrity of a header packet
  \retval == 0, all is okay
  \retval != 0, a bit map of the errors

  \param[in]   ctx  The check context
  \param[in]  data  The data to check
  \param[in] ndata  The number of bytes in the header
                                                                          */
/* ---------------------------------------------------------------------- */
static inline unsigned int checkHeader (Ctx            *ctx,
                                        uint8_t const *data,
                                        int           ndata)
{
   unsigned int reason = 0;

   uint64_t header = get_w64 (data);

   printf ("Headerx= %16.16" PRIx64 "\n", header);
   
   ctx->stats.hdrCnt += 1;
   ctx->stats.hdrSiz += ndata;


   return reason;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Checks the integrity of a data packet
  \retval == 0, all is okay
  \retval != 0, a bit map of the errors

  \param[in]   ctx  The check context
  \param[in]  data  The data to check
  \param[in] ndata  The number of bytes in the data
                                                                          */
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!
 *
 *  \brief Performs basic integrity checks on the frame.
 *
 *  \param[in]  bytes The frame data
 *  \param[in] nbytes The number bytes in the data
 *  \param[in]   dest The originating module, 0 or 1
 *
\* ---------------------------------------------------------------------- */
static inline unsigned int checkData (Ctx             *ctx,
                                      uint8_t  const *bytes,
                                      int            nbytes)
{
#  define N64_PER_FRAME 30
#  define NBYTES (unsigned int)(((1 + N64_PER_FRAME * 1024) + 1) * sizeof (uint64_t))


   struct History_s
   {
      uint64_t  timestamp;
      uint16_t convert[2];
   };

   static struct History_s History[2] = { {0, {0, 0}}, {0, {0,0}} };
   static unsigned int        Counter = 0;
   uint64_t const               *data = (uint64_t const *)bytes;
   int                            n64 = nbytes / sizeof (*data) - 1;
   int                           dest = 0;  


   // ------------------------------------
   // Check for the correct amount of data
   // ------------------------------------
   if (nbytes != NBYTES)
   {
      printf ("Aborting @ %2u.%6u %u != %u incorrect amount of data received\n",
              dest, Counter, nbytes, NBYTES);
      return 1;
   }

   // SKip FPGA header
   data += 1;
   n64  -= 1;

   // -------------------------------------------------------------
   // Seed predicted sequence number with either
   //   1) The GPS timestamp of the previous packet
   //   2) The GPS timestamp of the first frame
   // -------------------------------------------------------------
   uint16_t   predicted_cvt_0;
   uint16_t   predicted_cvt_1;
   uint64_t   predicted_ts = History[dest].timestamp;
   if (predicted_ts)
   {
      predicted_ts    = data[ 1];
      predicted_cvt_0 = data[ 2] >> 48;
      predicted_cvt_1 = data[16] >> 48;
   }
   else
   {
      predicted_cvt_0 = History[dest].convert[0];
      predicted_cvt_1 = History[dest].convert[1];
   }


   // --------------------
   // Loop over each frame
   // --------------------
   unsigned int    frame = 0;
   for (int idx = 0; idx < n64; idx += N64_PER_FRAME)
   {
      uint64_t d = data[idx];

      // -------------------------
      // Check the comma character
      // -------------------------
      if ((d & 0xff) != 0xbc)
      {
         printf ("Error frame @ %2u.%6u.%4u: %16.16" PRIx64 "\n",
                 dest, Counter, frame, d);
      }

      // -----------------------------------
      // Form and check the sequence counter
      // ----------------------------------
      uint64_t timestamp = data[idx+1];
      uint16_t     cvt_0 = data[idx +  2] >> 48;
      uint16_t     cvt_1 = data[idx + 16] >> 48;

      int error = ((timestamp != predicted_ts   ) << 0)
                | ((cvt_0     != predicted_cvt_0) << 1)
                | ((cvt_1     != predicted_cvt_1) << 2);

      if (error)
      {
         printf ("Error  seq @ %2u.%6u.%4u: "
                 "ts: %16.16" PRIx64 " %c= %16.16" PRIx64 " "
                 "cvt: %4.4" PRIx16 " %c= %4.4" PRIx16 " "
                 "cvt: %4.4" PRIx16 " %c= %4.4" PRIx16 "\n",
                 dest, Counter, frame, 
                 timestamp, (error&1) ? '!' : '=', predicted_ts,
                 cvt_0    , (error&2) ? '!' : '=', predicted_cvt_0,
                 cvt_1    , (error&4) ? '!' : '=', predicted_cvt_1);

         // -----------------------------------------------------
         // In case of error, resynch the predicted to the actual
         // -----------------------------------------------------
         predicted_ts    = timestamp;
         predicted_cvt_0 = cvt_0;
         predicted_cvt_1 = cvt_1;
      }
      else if (1 && ((Counter % (1024)) == 0) && ((frame % 256) == 0))
      {
         // --------------------------------------
         // Print reassuring message at about 2 Hz
         // --------------------------------------
         printf ("Spot check @ %2u.%6u.%4u: "
                 "ts: %16.16" PRIx64 " == %16.16" PRIx64 " "
                 "cvt: %4.4" PRIx16 " == %4.4" PRIx16 " "
                 "cvt: %4.4" PRIx16 " == %4.4" PRIx16 "\n",
                 dest, Counter, frame, 
                 timestamp, predicted_ts,
                 cvt_0    , predicted_cvt_0,
                 cvt_1    , predicted_cvt_1);
      }


      // -------------------------------------
      // Advance the predicted sequence number
      // Advance the frame counter
      // -------------------------------------
      predicted_ts     = timestamp + 500;
      predicted_cvt_0 += 1;
      predicted_cvt_1 += 1;
      frame           += 1;
   }

   // -----------------------------------------------
   // Keep track of the number of time called and
   // the expected sequence number of the next packet
   // for this destination.
   // -----------------------------------------------
   Counter                 += 1;
   History[dest].timestamp  = predicted_ts    +  500;
   History[dest].convert[0] = predicted_cvt_0 +    1;
   History[dest].convert[1] = predicted_cvt_1 +    1;

   return 0;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Puts of the statistics/status title line
                                                                          */
/* ---------------------------------------------------------------------- */
static inline void print_statistics_title ()
{
   puts (
   "   Rate        Bps SeqErrs DatErrs BadData Nevents   Total      Bytes\n"
    " ------ ---------- ------- --------------- ------- ------- ----------");

    return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Prints the periodic statistics

  \param[in] cur  The current statistics
  \param[in] prv  The previous statistics; used to form differences and
                  rates
  \param[in] eol  Either a '\n' or '\r'
                                                                          */
/* ---------------------------------------------------------------------- */
static void print_statistics (Ctx const *cur, Ctx const *prv, char eol)
{
   uint32_t curSiz = cur->stats.hdrSiz + cur->stats.datSiz;
   uint32_t prvSiz = prv->stats.hdrSiz + prv->stats.datSiz;
   
   printf (" %6"PRIu32" %10"PRIu32" %7"PRIu32" %7"PRIu32" %7"PRIu32
           " %7"PRIu32" %7"PRIu32" %10"PRIu32"%c",
           cur->stats.hdrCnt - prv->stats.hdrCnt,
           (curSiz - prvSiz) * 8,
           cur->errs.seqNum,
           cur->errs.datSiz,
           cur->errs.datVal,
           cur->stats.datCnt,
           cur->stats.hdrCnt,
           curSiz,
           eol);

   fflush (stdout);

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
/* LOCAL PROTOTYPES                                                       */
/* ---------------------------------------------------------------------- */
static int     open_client       (int         portno, 
                                  int        rcvSize, 
                                  int        nodelay, 
                                  int        *servFd);

static int     connect_client    (int       listenFd, 
                                  int         portno, 
                                  int        rcvSize,
                                  int        nodelay);

static int     reconnect_client  (ssize_t      nread,
                                  int       clientFd,
                                  int        serverFd,
                                  Prms const   *prms,
                                  char const    *msg);

static void    print_socketopts  (int             fd);

static void            print_hdr (uint64_t        data);
static void            print_id  (uint64_t const *data);

static ssize_t read_data        (int              fd,
                                 uint8_t       *data,
                                 int           ndata,
                                 RcvProfile *profile);

static char    if_newline       (char        need_lf);
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- *//*!

  \brief  Code to test TCP/IP reception

  \param[in]  argc   Command line argument count
  \param[in]  argv   Vecto of command line parameters
                                                                          */
/* ---------------------------------------------------------------------- */
int main(int argc, char *const argv[])
{
    uint32_t              headerSize = sizeof (uint64_t);
    uint32_t		    maxBytes = 8*1024*1024;


    uint8_t  *rxData = malloc (maxBytes); 
    uint32_t       eject = 60;
    char         need_lf =  0;

    Prms      prms;
    getPrms (&prms, argc, argv);


    bzero (rxData, maxBytes);
    Ctx           ctx;
    Ctx           prv;
    init_ctx   (&ctx);
    init_ctx   (&prv);


    unsigned int retries[128];
    memset (retries, 0, sizeof (retries));


    int         srvFd;
    int         rcvFd  = open_client (prms.portNumber, 
                                      prms.rcvSize, 
                                      prms.nodelay,
                                      &srvFd);

    print_socketopts  (rcvFd);
    print_statistics_title ();

    while (1)
    {
       RcvProfile hdrRcv;
       RcvProfile datRcv;



       /* 
        | Obtain the header, the hdrRcv keeps track of how many
        | separate rcv calls where made and the number of bytes
        | each read.  One will see a similar thing when reading
        | the data.  Of course there it is more interesting, since
        | the typical header reader will complete in one call to
        | rcv, while the data read will take many, depending on the
        | size of the TCP receive buffer
       */
       ssize_t nread = read_data (rcvFd, rxData, headerSize, &hdrRcv);
   

       /* Check if had error or a disconnect */
       if (nread <= 0)
       {
          need_lf = if_newline (need_lf);
          puts ("Error");
          rcvProfile_print (&hdrRcv, "Hdr");
          rcvProfile_print (&datRcv, "Dat");
          print_statistics (&ctx, &prv, '\n');          
          rcvFd   = reconnect_client (nread, 
                                      rcvFd,
                                      srvFd,
                                      &prms, 
                                      "reading header");
          invalidate_ctx (&ctx);
          continue;
       }
       else if (nread != headerSize)
       {
          printf ("Did not read enough data %zu\n", nread);
          rcvProfile_print (&hdrRcv, "Hdr");
          exit (-1);
       }

       uint32_t failures = checkHeader (&ctx, rxData, headerSize);
       print_hdr (get_w64 (rxData));


       if (failures)
       {
          int nfailures = ctx.nfailures++;
          if (nfailures  < prms.nfailures)
          {
             need_lf = if_newline (need_lf);
             printf ("Failure = %8.8"PRIx32"\n", failures);
          }
       }
      
       /* If the header is OK, read the 'data' associated with it */
       if (failures == 0)
       {
          ssize_t  received = nread;
          uint64_t header   = get_w64 (rxData);
          uint32_t dataSize = (header >> 8) & 0xffffff;


          /* Ensure that the buffer can handle the data volume */
          if (dataSize > maxBytes)
          {
             printf ("Packet too large  %x > %" PRIx32 "\n", dataSize, maxBytes);
          }
          
          if (received < dataSize)
          {
             static int Count = 0;
             uint8_t  *data =   rxData + headerSize;
             uint32_t ndata = dataSize - headerSize;
             nread          = read_data (rcvFd, data, ndata, &datRcv);

             if (received != headerSize)
             {
                Count += 1;
             }

             /* 
              | Make a cheap histogram of the number of rcv calls
              | in an attempt to see if this at all correlates 
              | with the failures.
             */
             retries[datRcv.cnt] += 1;

             /* Check if had error or disconnect */
             if (nread <= 0)
             {
                need_lf = if_newline (need_lf);
                printf ("Error reading %u bytes\n", ndata);
                printf ("\n");
                print_hdr (get_w64 (rxData));
                printf ("\n");
                rcvProfile_print (&hdrRcv, "Hdr");
                rcvProfile_print (&datRcv, "Dat");
                print_statistics (&ctx, &prv, '\n'); 
                rcvFd = reconnect_client (nread,
                                          rcvFd,
                                          srvFd,
                                          &prms, 
                                          "reading data");
                invalidate_ctx (&ctx);
                continue;
             }

             print_id ((uint64_t const *)data);
             putchar ('\n');

             printf ("prms.chkData = %d\n", prms.chkData);

             if (prms.chkData) 
             {
                unsigned int err = checkData (&ctx, data, ndata);
                if (err)
                {
                   rcvProfile_print (&hdrRcv, "Hdr");
                   rcvProfile_print (&datRcv, "Dat");
                }
             }

             ctx.stats.datCnt += 1;
             ctx.stats.datSiz += ndata;

          }
       }


       /* 
        | Periodically (once a second) output the accumuated statistics
        | and status 
       */
       time_t prvTime = ctx.timestamp;
       time (&ctx.timestamp);
       if (0 && ctx.timestamp != prvTime)
       {
          /*
          printf ("Cur:Prv %u:%u\n", 
                  (unsigned int)ctx.timestamp,
                  (unsigned int)prvTime);
          */


          char c;
          if (eject-- == 0)
          {
             // Reset the failure count
             ctx.nfailures = 0;

             c       = '\n';
             need_lf = 0;
             eject   = 60;
          }
          else
          {
             c       = '\r';
             need_lf = 1;
          }

          print_statistics (&ctx, &prv, c);

          need_lf = 1;
          prv     = ctx;
       }
    }
   
    close (srvFd);
    close (rcvFd);

    return 0;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Opens a socket on the specified port 
  \return  The opened socket

  \param[ in]   portno  The port number
  \param[ in]  rcvSize  Size, in bytes of the TCP receive buffer
  \param[out]    srvFd  Returned as the server socket

   This is a very cheap implementation.  It expects that the other side
   has is already setup.  As such, this executable must be started after
   the client side has been started.
                                                                          */
/* ---------------------------------------------------------------------- */
static int open_client (int portno, int rcvSize, int nodelay, int *srvFd)
{
    int     listenFd;

    struct sockaddr_in srvAdr;
    printf  ("Open socket\n");


    // Init structures
    listenFd = socket (AF_INET,  SOCK_STREAM,  0);
    memset (&srvAdr, 0, sizeof (srvAdr));

    printf ("Got listening socket %d\n", listenFd);

    srvAdr.sin_family      = AF_INET;
    srvAdr.sin_addr.s_addr = INADDR_ANY;
    srvAdr.sin_port        = htons(portno);


    if (bind (listenFd, (struct sockaddr *) &srvAdr, sizeof(srvAdr)) < 0) 
    {
       printf ("ControlServer::startListen -> Failed to bind socket %5d\n",
               portno);
        exit(-1);
    }
   
    int cliFd;
    cliFd  = connect_client (listenFd, portno, rcvSize, nodelay);
   *srvFd = listenFd;
    return cliFd;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static int connect_client (int listenFd, 
                           int   portno, 
                           int  rcvSize,
                           int  nodelay)
{
    socklen_t cliLen;
    int        cliFd;
    struct sockaddr_in  cliAddr;


    // Start listen
    printf ("Listening for a connection on port %5d\n", portno);
    listen (listenFd, 5);
    
   
    cliLen = sizeof (cliAddr);
    cliFd  = accept (listenFd, (struct sockaddr *)&cliAddr, &cliLen);
    if(cliFd < 0)
    {
       puts ("Error on Accept");
    }    
    else
    {
       puts ("Accepted connection from client");
       setsockopt (cliFd, SOL_SOCKET, SO_RCVBUF, &rcvSize, sizeof (int));


       int iss;
       iss = setsockopt (cliFd, SOL_TCP, TCP_NODELAY, &nodelay, sizeof (int));
       if (iss < 0)
       {
          printf ("Error setting NODELAY flag %d\n", h_errno);
       }

    }



    return cliFd;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Tries to reconnect to the client after a disconnect
  \return The reconnected socket fd

  \parampin]    nread  The last return value from rcv
  \param[in] clientFd  The old client socket fd (to be closed)
  \param[in] serverFd  The socket fd to accept the new connection
  \param[in]     prms  The connection parameters
  \param[in]      msg  A string indicating the reason for disconnecting
                                                                          */
/* ---------------------------------------------------------------------- */
static int reconnect_client (ssize_t     nread,
                             int      clientFd,
                             int      serverFd,
                             Prms const  *prms,
                             char  const  *msg)
{
   if (nread == 0)
   {
      printf ("Disconnect while %s\n"
              "Try reconnecting\n",  msg);

      close (clientFd);
      clientFd = connect_client (serverFd, 
                                 prms->portNumber, 
                                 prms->rcvSize,
                                 prms->nodelay);
   }
   else if (nread < 0)
   {
      printf ("Receive error while %s, quitting\n", msg);
      exit (-1);
   }

   
   return clientFd;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Provides a newline if one is needed
  \retval == 0, always

  \param[in] need_lf  If non-zero, then a newline is output to stdout
                                                                          */
/* ---------------------------------------------------------------------- */
static inline char if_newline (char need_lf)
{
   if (need_lf) putchar ('\n');
   return 0;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Print a couple of the critical options for the specified socket

  \param[in] fd  The socket's fd
                                                                          */
/* ---------------------------------------------------------------------- */
static void print_socketopts (int fd)
{
    int       rcvSize;
    int       nodelay;
    socklen_t optSize;

    /* Retrieve the socket's receive buffer size */
    optSize = sizeof (rcvSize);
    getsockopt (fd, SOL_SOCKET, SO_RCVBUF, &rcvSize, &optSize);


    /* Retrieve whether the ACK's are delayed (Nagle Algorithm) or not */
    optSize = sizeof (nodelay);
    getsockopt (fd, SOL_TCP, TCP_NODELAY, &nodelay, &optSize);


    printf ("SO_RCVBUF   = %8.8x\n", rcvSize);
    printf ("TCP_NODELAY = %8d\n",   nodelay);

    return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Prints the frame header

  \param[in]  data   The header data
                                                                          */
/* ---------------------------------------------------------------------- */
static void print_hdr (uint64_t header)
{
   unsigned format    = (header >>  0) & 0xf;
   unsigned type      = (header >>  4) & 0xf;
   unsigned length    = (header >>  8) & 0xffffff;
   unsigned specific0 = (header >> 32) & 0xf;
   unsigned naux64    = (header >> 36) & 0xf;
   unsigned specific1 = (header >> 40) & 0xffffff;


   printf ("Header0    format = %1.1x  type   = %1.1x length = %6.6x\n"
           "           spec0  = %1.1x  naux64 = %1.1x  spec1 = %6.6x\n",
            format,
            type,
            length,
            specific0,
            naux64,
            specific1);

   return;
}
/* ---------------------------------------------------------------------- */



static void  print_id (uint64_t const *data)
{
   uint64_t       w64 = data[0];
   uint64_t timestamp = data[1];
   printf ("Identifier: %16.16" PRIx64 " %16.16" PRIx64 "\n",
           w64, timestamp);

   unsigned format   = (w64 >>  0) & 0xf;
   unsigned src0     = (w64 >>  4) & 0xfff;
   unsigned type     = (w64 >> 16) & 0xf;
   unsigned src1     = (w64 >> 20) & 0xfff;
   unsigned sequence = (w64 >> 32) & 0xffffffff;

   unsigned c0 = (src0 >> 6) & 0x1f;
   unsigned s0 = (src0 >> 3) & 0x7;
   unsigned f0 = (src0 >> 0) & 0x7;

   unsigned c1 = (src1 >> 6) & 0x1f;
   unsigned s1 = (src1 >> 3) & 0x7;
   unsigned f1 = (src1 >> 0) & 0x7;


   printf ("          Format.Type = %1.1x.%1.1x Srcs = %1x.%1x.%1x : %1x.%1x.%1x\n"
           "          Timestamp   = %16.16" PRIx64 " Sequence = %8.8" PRIx32 "\n",
           format, type, c0, s0, f0, c1, s1, f1,
           timestamp, sequence);

   return;
}
           
   


/* ---------------------------------------------------------------------- */
static ssize_t read_data (int              fd, 
                          uint8_t       *data,
                          int           ndata,
                          RcvProfile *profile)
{
   int       cnt = 1;
   ssize_t nread = recv (fd, data, ndata, 0);
   ssize_t left  = ndata;

   profile->hist[0] = nread;

   while ( (left -= nread) > 0)
   {
      unsigned int idx;
      data     += nread;

      nread = recv (fd, data , left, 0);

      idx = (cnt >= sizeof (profile->hist) / sizeof (profile->hist[0]))
         ? (        sizeof (profile->hist) / sizeof (profile->hist[0]) - 1)
          : cnt;


      profile->hist[idx] = nread;

      cnt += 1;
      if (nread <= 0) goto EXIT;
   }

   nread = ndata;

EXIT:
   profile->cnt = cnt;
   profile->tot = ndata - left;

   return nread;
}
/* ---------------------------------------------------------------------- */

   


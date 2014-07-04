/*
 * netapp.h - CC31xx/CC32xx Host Driver Implementation
 *
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

#include "simplelink.h"

#ifndef __NETAPP_H__
#define    __NETAPP_H__

#ifdef    __cplusplus
extern "C" {
#endif

/*************************************************************************************************************/
/*ERROR code*/
#define NETAPP_RX_BUFFER_LENGTH_ERROR -230
/*************************************************************************************************************/

typedef struct
{
    unsigned long   PacketsSent;
    unsigned long   PacketsReceived;
    unsigned short  MinRoundTime;
    unsigned short  MaxRoundTime;
    unsigned short  AvgRoundTime;
    unsigned long   TestTime;
}SlPingReport_t;


typedef struct
{
    unsigned long  PingIntervalTime;       /* delay between pings, in miliseconds */
    unsigned short PingSize;               /* ping packet size in bytes           */
    unsigned short PingRequestTimeout;     /* timeout time for every ping in miliseconds  */
    unsigned long  TotalNumberOfAttempts;  /* max number of ping requests. 0 - forever    */
    unsigned long  Flags;                  /* flag - 0 report only when finished, 1 - return response for every ping, 2 - stop after 1 successful ping.  */
    unsigned long  Ip;                     /* IPv4 address or IPv6 first 4 bytes  */
    unsigned long  Ip1OrPaadding;
    unsigned long  Ip2OrPaadding;
    unsigned long  Ip3OrPaadding;
}SlPingStartCommand_t;


/*  Http Server interface */

#define MAX_INPUT_STRING          64 /*  because of WPA */

#define MAX_AUTH_NAME_LEN        20
#define MAX_AUTH_PASSWORD_LEN    20
#define MAX_AUTH_REALM_LEN        20

#define MAX_DEVICE_URN_LEN (15+1)
#define MAX_DOMAIN_NAME_LEN    (24+1)

#define MAX_ACTION_LEN            30
#define MAX_TOKEN_NAME_LEN        20  /* Important: in case the max len is changed, make sure the struct sl_NetAppHttpServerSendToken_t in protocol.h is padded correctly!! */
#define MAX_TOKEN_VALUE_LEN        MAX_INPUT_STRING

/* Server Events */
#define SL_NETAPP_HTTPGETTOKENVALUE        1
#define SL_NETAPP_HTTPPOSTTOKENVALUE    2



#define NETAPP_MAX_SERVICE_TEXT_SIZE           256
#define NETAPP_MAX_SERVICE_NAME_SIZE           60
#define NETAPP_MAX_SERVICE_HOST_NAME_SIZE      64


/* Server Responses */
#define SL_NETAPP_RESPONSE_NONE            0
#define SL_NETAPP_HTTPSETTOKENVALUE        1

#define SL_NETAPP_FAMILY_MASK  0x80

typedef struct _slHttpServerString_t
{
    unsigned char len;
    unsigned char *data;
} slHttpServerString_t;

typedef struct _slHttpServerData_t
{
    unsigned char value_len;
    unsigned char name_len;
    unsigned char *token_value;
    unsigned char *token_name;
} slHttpServerData_t;

typedef struct _slHttpServerPostData_t
{
    slHttpServerString_t action;
    slHttpServerString_t  token_name;
    slHttpServerString_t token_value;
}slHttpServerPostData_t;

typedef union
{
  slHttpServerString_t  httpTokenName; /* SL_NETAPP_HTTPGETTOKENVALUE */
  slHttpServerPostData_t   httpPostData;  /* SL_NETAPP_HTTPPOSTTOKENVALUE */
} SlHttpServerEventData_u;

typedef union
{
  slHttpServerString_t token_value;
} SlHttpServerResponsedata_u;

typedef struct
{
   unsigned long            Event;
   SlHttpServerEventData_u EventData;
}SlHttpServerEvent_t;

typedef struct
{
   unsigned long Response;
   SlHttpServerResponsedata_u ResponseData;
}SlHttpServerResponse_t;


typedef struct
{
    unsigned long  lease_time;
    unsigned long  ipv4_addr_start;
    unsigned long  ipv4_addr_last;
}SlNetAppDhcpServerBasicOpt_t; 

/********************************************************************************************************/
/*mDNS parameters*/


typedef enum
{
    SL_NET_APP_FULL_SERVICE_WITH_TEXT_IPV4_TYPE = 1,
    SL_NET_APP_FULL_SERVICE_IPV4_TYPE,
    SL_NET_APP_SHORT_SERVICE_IPV4_TYPE
 
} SlNetAppGetServiceListType_e;

typedef struct
{
    unsigned long   service_ipv4;
    unsigned short  service_port;
    unsigned short  Reserved;
}SlNetAppGetShortServiceIpv4List_t;



typedef struct
{
    unsigned long   service_ipv4;
    unsigned short  service_port;
    unsigned short  Reserved;
    unsigned char   service_name[NETAPP_MAX_SERVICE_NAME_SIZE];
    unsigned char   service_host[NETAPP_MAX_SERVICE_HOST_NAME_SIZE];
}SlNetAppGetFullServiceIpv4List_t;

typedef struct
{
    unsigned long   service_ipv4;
    unsigned short  service_port;
    unsigned short  Reserved;
    unsigned char   service_name[NETAPP_MAX_SERVICE_NAME_SIZE];
    unsigned char   service_host[NETAPP_MAX_SERVICE_HOST_NAME_SIZE];
    unsigned char   service_text[NETAPP_MAX_SERVICE_TEXT_SIZE];
}SlNetAppGetFullServiceWithTextIpv4List_t;

typedef struct
{
    /*The below parameters are used to configure the advertise times and interval
    For example:
        If:
        Period is set to T
        Repetitions are set to P
        Telescopic factor is K=2
        The transmission shall be:
        advertise P times
        wait T
        advertise P times
        wait 4 * T
        advertise P time
        wait 16 * T  ... (till max time reached / configuration changed / query issued)
    */
    unsigned long    t;              /* Number of ticks for the initial period. Default is 100 ticks for 1 second. */
    unsigned long    p;              /* Number of repetitions. Default value is 1                                  */
    unsigned long    k;              /* Telescopic factor. Default value is 2.                                     */
    unsigned long    RetransInterval;/* Announcing retransmission interval                                         */
    unsigned long   Maxinterval;     /* Announcing max period interval                                             */
    unsigned long    max_time;       /* Announcing max time                                                        */
}SlNetAppServiceAdvertiseTimingParameters_t;

#define SL_NET_APP_MASK_IPP_TYPE_OF_SERVICE    			0x00000001
#define SL_NET_APP_MASK_DEVICE_INFO_TYPE_OF_SERVICE		0x00000002
#define SL_NET_APP_MASK_HTTP_TYPE_OF_SERVICE			0x00000004
#define SL_NET_APP_MASK_HTTPS_TYPE_OF_SERVICE			0x00000008
#define SL_NET_APP_MASK_WORKSATION_TYPE_OF_SERVICE		0x00000010
#define SL_NET_APP_MASK_GUID_TYPE_OF_SERVICE			0x00000020
#define SL_NET_APP_MASK_H323_TYPE_OF_SERVICE			0x00000040
#define SL_NET_APP_MASK_NTP_TYPE_OF_SERVICE				0x00000080
#define SL_NET_APP_MASK_OBJECITVE_TYPE_OF_SERVICE		0x00000100
#define SL_NET_APP_MASK_RDP_TYPE_OF_SERVICE				0x00000200
#define SL_NET_APP_MASK_REMOTE_TYPE_OF_SERVICE			0x00000400
#define SL_NET_APP_MASK_RTSP_TYPE_OF_SERVICE			0x00000800
#define SL_NET_APP_MASK_SIP_TYPE_OF_SERVICE				0x00001000
#define SL_NET_APP_MASK_SMB_TYPE_OF_SERVICE				0x00002000
#define SL_NET_APP_MASK_SOAP_TYPE_OF_SERVICE			0x00004000
#define SL_NET_APP_MASK_SSH_TYPE_OF_SERVICE				0x00008000
#define SL_NET_APP_MASK_TELNET_TYPE_OF_SERVICE			0x00010000
#define SL_NET_APP_MASK_TFTP_TYPE_OF_SERVICE			0x00020000
#define SL_NET_APP_MASK_XMPP_CLIENT_TYPE_OF_SERVICE		0x00040000
#define SL_NET_APP_MASK_RAOP_TYPE_OF_SERVICE			0x00080000
#define SL_NET_APP_MASK_ALL_TYPE_OF_SERVICE				0xFFFFFFFF

/********************************************************************************************************/
/* sl_NetAppDnsGetHostByName error codes     */

#define SL_NET_APP_DNS_QUERY_NO_RESPONSE         (-159)        /* DNS query failed, no response                        */ 
#define SL_NET_APP_DNS_NO_SERVER                 (-161)        /* No DNS server was specified                          */ 
#define SL_NET_APP_DNS_QUERY_FAILED              (-163)        /* DNS query failed; no DNS server sent an 'answer'     */ 
#define SL_NET_APP_DNS_MALFORMED_PACKET          (-166)        /* Improperly formed or corrupted DNS packet received   */ 
#define SL_NET_APP_DNS_MISMATCHED_RESPONSE       (-174)        /* Server response type does not match the query request*/

/********************************************************************************************************/


/* NetApp application IDs */
#define SL_NET_APP_HTTP_SERVER_ID                (1)
#define SL_NET_APP_DHCP_SERVER_ID                (2)
#define SL_NET_APP_MDNS_ID                       (4)
#define SL_NET_APP_DNS_SERVER_ID	             (8)
#define SL_NET_APP_DEVICE_CONFIG_ID              (16)

/* NetApp application set/get options */
#define NETAPP_SET_DHCP_SRV_BASIC_OPT            (0)

/* HTTP server set/get options */
#define NETAPP_SET_GET_HTTP_OPT_PORT_NUMBER      (0)
#define NETAPP_SET_GET_HTTP_OPT_AUTH_CHECK       (1)
#define NETAPP_SET_GET_HTTP_OPT_AUTH_NAME        (2)
#define NETAPP_SET_GET_HTTP_OPT_AUTH_PASSWORD    (3)
#define NETAPP_SET_GET_HTTP_OPT_AUTH_REALM       (4)
#define NETAPP_SET_GET_HTTP_OPT_ROM_PAGES_ACCESS (5)
 
#define NETAPP_SET_GET_MDNS_CONT_QUERY_OPT       (1)
#define NETAPP_SET_GET_MDNS_QEVETN_MASK_OPT      (2)
#define NETAPP_SET_GET_MDNS_TIMING_PARAMS_OPT    (3)

/* DNS server set/get options */
#define NETAPP_SET_GET_DNS_OPT_DOMAIN_NAME	     (0)

/* Device Config set/get options */
#define NETAPP_SET_GET_DEV_CONF_OPT_DEVICE_URN   (0)
#define NETAPP_SET_GET_DEV_CONF_OPT_DOMAIN_NAME  (1)

typedef void (*P_SL_DEV_PING_CALLBACK)(SlPingReport_t*);

/*****************************************************************************

    API Prototypes

 *****************************************************************************/

/*!

    \addtogroup netapp
    @{

*/
/*!
    \brief Starts a network application

    Gets and starts network application for the current WLAN mode

    \param[in] AppBitMap      application bitmap, could be one or combination of the following: \n
                              - SL_NET_APP_HTTP_SERVER_ID (1)
                              - SL_NET_APP_DHCP_SERVER_ID (2)
                              - SL_NET_APP_MDNS_ID        (4)

    \return                   On error, negative number is returned

    \sa                       Stop one or more the above started applications using sl_NetAppStop
    \note                     This command activates the application for the current WLAN mode (AP or STA)
    \warning
    \par                 Example:  
    \code                
    For example: Starting internal HTTP server + DHCP server: 
    sl_NetAppStart(SL_NET_APP_HTTP_SERVER_ID | SL_NET_APP_DHCP_SERVER_ID)
    
    \endcode
*/
#if _SL_INCLUDE_FUNC(sl_NetAppStart)
int sl_NetAppStart(unsigned long AppBitMap);
#endif
/*!
    \brief Stops a network application

    Gets and stops network application for the current WLAN mode

    \param[in] AppBitMap    application id, could be one of the following: \n
                            - SL_NET_APP_HTTP_SERVER_ID (1)
                            - SL_NET_APP_DHCP_SERVER_ID (2)
                            - SL_NET_APP_MDNS_ID (4)

    \return                 On error, negative number is returned

    \sa
    \note                This command disables the application for the current active WLAN mode (AP or STA)
    \warning
    \par                 Example:
    \code                
    
    For example: Stopping internal HTTP server: 
                         sl_NetAppStop(SL_NET_APP_HTTP_SERVER_ID); 
    
    \endcode
*/
#if _SL_INCLUDE_FUNC(sl_NetAppStop)
int sl_NetAppStop(unsigned long AppBitMap);
#endif


/*!
    \brief Get host IP by name

    Obtain the IP Address of machine on network, by machine name.

    \param[in]  hostname        host name
    \param[in]  usNameLen       name length
    \param[out] out_ip_addr     This parameter is filled in with
                                host IP address. In case that host name is not
                                resolved, out_ip_addr is zero.
    \param[in]  family          protocol family

    \return                     On success, positive is returned.
                                On error, negative is returned
                                SL_POOL_IS_EMPTY may be return in case there are no resources in the system
                                In this case try again later or increase MAX_CONCURRENT_ACTIONS
                                Possible DNS error codes:
                                - SL_NET_APP_DNS_QUERY_NO_RESPONSE       
                                - SL_NET_APP_DNS_NO_SERVER               
                                - SL_NET_APP_DNS_QUERY_FAILED            
                                - SL_NET_APP_DNS_MALFORMED_PACKET        
                                - SL_NET_APP_DNS_MISMATCHED_RESPONSE     

    \sa
    \note   Only one sl_NetAppDnsGetHostByName can be handled at a time.
            Calling this API while the same command is called from another thread, may result
            in one of the two scenarios:
            1. The command will wait (internal) until the previous command finish, and then be executed.
            2. There are not enough resources and POOL_IS_EMPTY error will return. 
            In this case, MAX_CONCURRENT_ACTIONS can be increased (result in memory increase) or try
            again later to issue the command.
    \warning
    \par  Example:
    \code
    unsigned long DestinationIP;
    sl_NetAppDnsGetHostByName("www.google.com", strlen("www.google.com"), &DestinationIP,SL_AF_INET);

    Addr.sin_family = SL_AF_INET;
    Addr.sin_port = sl_Htons(80);
    Addr.sin_addr.s_addr = sl_Htonl(DestinationIP);
    AddrSize = sizeof(SlSockAddrIn_t);
    SockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
    \endcode
*/
#if _SL_INCLUDE_FUNC(sl_NetAppDnsGetHostByName)
int sl_NetAppDnsGetHostByName(char * hostname, unsigned short usNameLen, unsigned long* out_ip_addr,unsigned char family );
#endif


/*!
        \brief Return service attributes like IP address, port and text according to service name
        \par
        The user sets a service name Full/Part (see example below), and should get:
        - IP of service
        - The port of service
        - The text of service

        Hence it can make a connection to the specific service and use it.
        It is similar to get host by name method.

        It is done by a single shot query with PTR type on the service name.

                  The command that is sent is from constant parameters and variables parameters.


        \param[in]     pService                   Service name can be full or partial. \n
                                                  Example for full service name:
                                                  1. PC1._ipp._tcp.local
                                                  2. PC2_server._ftp._tcp.local \n
                                                  .
                                                  Example for partial service name:
                                                  1. _ipp._tcp.local
                                                  2. _ftp._tcp.local

        \param[in]    ServiceLen                  The length of the service name (in_pService).
        \param[in]    Family                      IPv4 or IPv6 (SL_AF_INET , SL_AF_INET6).
        \param[out]    pAddr                      Contains the IP address of the service.
        \param[out]    pPort                      Contains the port of the service.
        \param[out]    pTextLen                   Has 2 options. One as Input field and the other one as output:
                                                  - Input: \n
                                                  Contains the max length of the text that the user wants to get.\n
                                                  It means that if the text len of service is bigger that its value than
                                                  the text is cut to inout_TextLen value.
                                                  - Output: \n
                                                   Contain the length of the text that is returned. Can be full text or part of the text (see above).

        \param[out]   pOut_pText     Contains the text of the service full or partial

        \return       On success, zero is returned
                      SL_POOL_IS_EMPTY may be return in case there are no resources in the system
                      In this case try again later or increase MAX_CONCURRENT_ACTIONS
                      In case No service is found error -177 will be returned

        \note         The returns attributes belongs to the first service found.
                      There may be other services with the same service name that will response to the query.
                      The results of these responses are saved in the peer cache of the Device and should be read by another API.
                          
                      Only one sl_NetAppDnsGetHostByService can be handled at a time.
                      Calling this API while the same command is called from another thread, may result
                      in one of the two scenarios:
                      1. The command will wait (internal) until the previous command finish, and then be executed.
                      2. There are not enough resources and SL_POOL_IS_EMPTY error will return. 
                      In this case, MAX_CONCURRENT_ACTIONS can be increased (result in memory increase) or try
                      again later to issue the command.

        \warning      Text length can be 120 bytes only
*/
#if _SL_INCLUDE_FUNC(sl_NetAppDnsGetHostByService)
long sl_NetAppDnsGetHostByService(char           *pServiceName,  /*  string containing all (or only part): name + subtype + service */
                                  unsigned char  ServiceLen,
                                  unsigned char  Family,        /*  4-IPv4 , 16-IPv6  */
                                  unsigned long  pAddr[], 
                                  unsigned long  *pPort,
                                  unsigned short *pTextLen, /*  in: max len , out: actual len */
                                  char           *pText
                                 );

#endif

/*!

 

        \brief Get service List
        Insert into out pBuffer a list of peer's services that are the NWP.
        The list is in a form of service struct. The user should chose the type
        of the service struct like:
            - Full service parameters with text.
            - Full service parameters.
            - Short service parameters (port and IP only) especially for tiny hosts.

        The different types of struct are made to give the 
        Possibility to save memory in the host 

        
        The user also chose how many max services to get and start point index
        NWP peer cache.
        For example:
            1.    Get max of 3 full services from index 0 � means up to 3 full services
            from index 0 are inserted into pBuffer (services that are in indexes 0,1,2).
            2.    Get max of 4 full services from index 3 � means up to 4 full services
            from index 3 are inserted into pBuffer (services that are in indexes 3,4,5,6).
            3.    Get max of 2 short services from index 6 � means up to 2 short services
            from index 6 are inserted into pBuffer (services that are in indexes 6,7).

        See below - command parameters.
                    


 
        \param[in] 1. indexOffset - The start index in the peer cache that from it the first service is returned.
        \param[in] 2. MaxServiceCount - The Max services that can be returned if existed or if not exceed the max index 
                      in the peer cache
        \param[in] 3. Flags - an ENUM number that means which service struct to use (means which types of service to fill)                                            
                        - use SlNetAppGetFullServiceWithTextIpv4List_t
                        - use SlNetAppGetFullServiceIpv4List_t
                        - use SlNetAppGetShortServiceIpv4List_t

       \param[out]4. Buffer - The Services are inserted into this buffer. In the struct form according to the bit that is set in the Flags 
                      input parameter.
 
        \return    ServiceFoundCount - The number of the services that were inserted into the buffer. zero means no service is found 
                   negative number means ERROR:
                   - (-208)    Illegal value of flags parameters in API get service list
                   - (-230)    Returned list buffer is bigger than the user allocated buffer
        \sa           sl_NetAppMDNSRegisterService
        \note        
        \warning 
                    if the out pBuffer size is bigger than an RX packet(1480), than
                    an error is returned because there
                    is no place in the RX packet.
                    The size is a multiply of MaxServiceCount and size of service struct(that is set 
                    according to flag value).
*/

#if _SL_INCLUDE_FUNC(sl_NetAppGetServiceList)
int sl_NetAppGetServiceList(unsigned char  IndexOffest,
                            unsigned char  MaxServiceCount,
                            unsigned char  Flags,
                            char           *pBuffer,
                            unsigned long  RxBufferLength
                            );


#endif


/*!
        \brief Unregister mDNS service
        This function deletes the mDNS service from the mDNS package and the database.

        The mDNS service that is to be unregistered is a service that the application no longer wishes to provide. \n
        The service name should be the full service name according to RFC
        of the DNS-SD - meaning the value in name field in the SRV answer.
                    
        Examples for service names:
        1. PC1._ipp._tcp.local
        2. PC2_server._ftp._tcp.local

        \param[in]    pServiceName            Full service name. \n
                                                Example for service name:
                                                1. PC1._ipp._tcp.local
                                                2. PC2_server._ftp._tcp.local
        \param[in]    ServiceLen              The length of the service. 
        \return    On success, zero is returned 
        \sa          sl_NetAppMDNSRegisterService

        \note        
        \warning 
        The size of the service length should be smaller than 255.


*/
#if _SL_INCLUDE_FUNC(sl_NetAppMDNSUnRegisterService)
int sl_NetAppMDNSUnRegisterService(    const char        *pServiceName, 
                                    unsigned char   ServiceNameLen);


#endif

/*!
        \brief Register a new mDNS service
        \par
        This function registers a new mDNS service to the mDNS package and the DB.
                    
        This registered service is a service offered by the application.
        The service name should be full service name according to RFC
        of the DNS-SD - meaning the value in name field in the SRV answer.
        Example for service name:
        1. PC1._ipp._tcp.local
        2. PC2_server._ftp._tcp.local

        If the option is_unique is set, mDNS probes the service name to make sure
        it is unique before starting to announce the service on the network.
        Instance is the instance portion of the service name.

        \param[in]  ServiceLen         The length of the service.
        \param[in]  TextLen            The length of the service should be smaller than 64.
        \param[in]  port               The port on this target host port.
        \param[in]  TTL                The TTL of the service
        \param[in]  Options            bitwise parameters: \n
                                       - bit 0  - service is unique (means that the service needs to be unique)
                                       - bit 31  - for internal use if the service should be added or deleted (set means ADD).
                                       - bit 1-30 for future.

        \param[in]    pServiceName              The service name.
                                       Example for service name: \n
                                                1. PC1._ipp._tcp.local
                                                2. PC2_server._ftp._tcp.local

        \param[in] pText                     The description of the service.
                                                should be as mentioned in the RFC
                                                (according to type of the service IPP,FTP...)

        \return     On success, zero is returned
                    Possible error codes:
                    -200        Maximum advertise services are already configured. 
                                Delete another existed service that is registered and then register again the new service
                    -201        Trying to register a service that is already exists
                    -203        Trying to delete service that does not existed
                    -204/-179    Illegal service name according to the RFC
                    -205        Retry request
                    -207        Illegal length of one of the mDNS Set functions
                    -161        mDNS is not operational as the device has no IP.Connect the device to an AP to get an IP address.
                    -162        mDNS parameters error
                    -163/-182    mDNS internal cache error
                    -164 to -176  mDNS internal error
                    -178        Adding a service is not allowed as it is already exist (duplicate service)
                    -180        mDNS is not running                    
                    -181        Host name error. Host name format is not allowed according to RFC 1033,1034,1035, 6763
                    
                    -206        List size buffer is bigger than internally allowed in the NWP (API get service list)    Change the APIs� parameters to decrease the size of the list
                                        

        \sa              sl_NetAppMDNSUnRegisterService

        \warning      1) Temporary -  there is an allocation on stack of internal buffer.
                    Its size is NETAPP_MDNS_MAX_SERVICE_NAME_AND_TEXT_LENGTH. \n
                    It means that the sum of the text length and service name length cannot be bigger than
                    NETAPP_MDNS_MAX_SERVICE_NAME_AND_TEXT_LENGTH.\n
                    If it is - An error is returned. \n
                    2) According to now from certain constraints the variables parameters are set in the
                    attribute part (contain constant parameters)
*/
#if _SL_INCLUDE_FUNC(sl_NetAppMDNSRegisterService)
int sl_NetAppMDNSRegisterService(    const char*        pServiceName, 
                                    unsigned char   ServiceNameLen,
                                    const char*        pText,
                                    unsigned char   TextLen,
                                    unsigned short  Port,
                                    unsigned long    TTL,
                                    unsigned long    Options);

#endif


/*!
    \brief send ICMP ECHO_REQUEST to network hosts

    Ping uses the ICMP protocol's mandatory ECHO_REQUEST


    \param[in]   pPingParams     Pointer to the ping request structure: \n
                                 - if flags parameter is set to 0, ping will report back once all requested pings are done (as defined by TotalNumberOfAttempts). \n
                                 - if flags parameter is set to 1, ping will report back after every ping, for TotalNumberOfAttempts.
                                 - if flags parameter is set to 2, ping will stop after the first successful ping, and report back for the successful ping, as well as any preceding failed ones. 
								 
							    For stopping an ongoing ping activity, set parameters IP address to 0

    \param[in]   family          SL_AF_INET or  SL_AF_INET6
    \param[out]  pReport         Ping pReport
    \param[out]  pCallback       Callback function upon completion.
                                 If callback is NULL, the API is blocked until data arrives


    \return    On success, zero is returned. On error, -1 is returned
               SL_POOL_IS_EMPTY may be return in case there are no resources in the system
               In this case try again later or increase MAX_CONCURRENT_ACTIONS

    \sa       sl_NetAppPingReport
    \note      Only one sl_NetAppPingStart can be handled at a time.
              Calling this API while the same command is called from another thread, may result
                  in one of the two scenarios:
              1. The command will wait (internal) until the previous command finish, and then be executed.
              2. There are not enough resources and SL_POOL_IS_EMPTY error will return. 
              In this case, MAX_CONCURRENT_ACTIONS can be increased (result in memory increase) or try
              again later to issue the command.
    \warning  
    \par      Example:
    \code     
    
    An example of sending 20 ping requests and reporting results to a callback routine when 
              all requests are sent:

              // callback routine
              void pingRes(SlPingReport_t* pReport)
              {
               // handle ping results 
              }
              
              // ping activation
              void PingTest()
              {
                 SlPingReport_t report;
                 SlPingStartCommand_t pingCommand;
                 
                 pingCommand.Ip = SL_IPV4_VAL(10,1,1,200);     // destination IP address is 10.1.1.200
                 pingCommand.PingSize = 150;                   // size of ping, in bytes 
                 pingCommand.PingIntervalTime = 100;           // delay between pings, in milliseconds
                 pingCommand.PingRequestTimeout = 1000;        // timeout for every ping in milliseconds
                 pingCommand.TotalNumberOfAttempts = 20;       // max number of ping requests. 0 - forever 
                 pingCommand.Flags = 0;                        // report only when finished
  
                 sl_NetAppPingStart( &pingCommand, SL_AF_INET, &report, pingRes ) ;
             }

    \endcode
*/
#if _SL_INCLUDE_FUNC(sl_NetAppPingStart)
int sl_NetAppPingStart(SlPingStartCommand_t* pPingParams,unsigned char family,SlPingReport_t *pReport,const P_SL_DEV_PING_CALLBACK pPingCallback);
#endif

/*!
    \brief     Internal function for setting network application configurations

    \return    On success, zero is returned. On error, -1 is
               returned

    \param[in] AppId          Application id, could be one of the following: \n
                              - SL_NET_APP_HTTP_SERVER_ID
                              - SL_NET_APP_DHCP_SERVER_ID

    \param[in] SetOptions     set option, could be one of the following: \n
                              NETAPP_SET_BASIC_OPT

    \param[in] OptionLen       option structure length

    \param[in] pOptionValues   pointer to the option structure
    \sa
    \note
    \warning
    \par
    \code
        Set DHCP Server (AP mode) parameters example:
                          
        SlNetAppDhcpServerBasicOpt_t dhcpParams; 
        unsigned char outLen = sizeof(SlNetAppDhcpServerBasicOpt_t); 
        dhcpParams.lease_time      = 4096;                         // lease time (in seconds) of the IP Address
        dhcpParams.ipv4_addr_start =  SL_IPV4_VAL(192,168,1,10);   // first IP Address for allocation. IP Address should be set as Hex number - i.e. 0A0B0C01 for (10.11.12.1)
        dhcpParams.ipv4_addr_last  =  SL_IPV4_VAL(192,168,1,16);   // last IP Address for allocation. IP Address should be set as Hex number - i.e. 0A0B0C01 for (10.11.12.1)
        sl_NetAppStop(SL_NET_APP_DHCP_SERVER_ID);                  // Stop DHCP server before settings
        sl_NetAppSet(SL_NET_APP_DHCP_SERVER_ID, NETAPP_SET_DHCP_SRV_BASIC_OPT, outLen, (unsigned char*)&dhcpParams);  // set parameters
        sl_NetAppStart(SL_NET_APP_DHCP_SERVER_ID);                 // Start DHCP server with new settings
    \endcode
    \code
        Set Device URN name example:
 
        Device name, maximum length of 33 characters 
        Device name affects URN name, own SSID name in AP mode, and WPS file "device name" in WPS I.E (STA-WPS / P2P)
        In case no device URN name set, the default name is "mysimplelink" 
        Allowed characters in device name are: 'a - z' , 'A - Z' , '0-9' and '-'

        unsigned char *my_device = "MY-SIMPLELINK-DEV";
        sl_NetAppSet (SL_NET_APP_DEVICE_CONFIG_ID, NETAPP_SET_GET_DEV_CONF_OPT_DEVICE_URN, strlen(my_device), (unsigned char *) my_device);
    \endcode

*/
#if _SL_INCLUDE_FUNC(sl_NetAppSet)
long sl_NetAppSet(unsigned char AppId ,unsigned char Option,unsigned char OptionLen, unsigned char *pOptionValue);
#endif

/*!
    \brief     Internal function for getting network applications configurations

    \return    On success, zero is returned. On error, -1 is
               returned

    \param[in] AppId          Application id, could be one of the following: \n
                              - SL_NET_APP_HTTP_SERVER_ID
                              - SL_NET_APP_DHCP_SERVER_ID

    \param[in] Options        Get option, could be one of the following: \n
                              NETAPP_SET_BASIC_OPT

    \param[in] OptionLen     The length of the allocated memory as input, when the
                                        function complete, the value of this parameter would be
                                        the len that actually read from the device.
                                        If the device return length that is longer from the input
                                        value, the function will cut the end of the returned structure
                                        and will return ESMALLBUF

    \param[out] pValues      pointer to the option structure which will be filled with the response from the device

    \sa
    \note
    \warning
    \par
    \code
         Get DHCP Server parameters example:

         SlNetAppDhcpServerBasicOpt_t dhcpParams;
         unsigned char outLen = sizeof(SlNetAppDhcpServerBasicOpt_t);
         sl_NetAppGet(SL_NET_APP_DHCP_SERVER_ID, NETAPP_SET_DHCP_SRV_BASIC_OPT, &outLen, (unsigned char*)&dhcpParams);
 
         printf("DHCP Start IP %d.%d.%d.%d End IP %d.%d.%d.%d Lease time seconds %d\n",                                                           
            SL_IPV4_BYTE(dhcpParams.ipv4_addr_start,3),SL_IPV4_BYTE(dhcpParams.ipv4_addr_start,2),
            SL_IPV4_BYTE(dhcpParams.ipv4_addr_start,1),SL_IPV4_BYTE(dhcpParams.ipv4_addr_start,0), 
            SL_IPV4_BYTE(dhcpParams.ipv4_addr_last,3),SL_IPV4_BYTE(dhcpParams.ipv4_addr_last,2),
            SL_IPV4_BYTE(dhcpParams.ipv4_addr_last,1),SL_IPV4_BYTE(dhcpParams.ipv4_addr_last,0),         
            dhcpParams.lease_time);    
    \endcode
    \code
         Get Device URN name example:
         Maximum length of 33 characters of device name. 
         Device name affects URN name, own SSID name in AP mode, and WPS file "device name" in WPS I.E (STA-WPS / P2P)
         in case no device URN name set, the default name is "mysimplelink" 

         unsigned char my_device_name[35];
         sl_NetAppGet (SL_NET_APP_DEVICE_CONFIG_ID, NETAPP_SET_GET_DEV_CONF_OPT_DEVICE_URN, strlen(my_device_name), (unsigned char *)my_device_name); 
    \endcode
*/
#if _SL_INCLUDE_FUNC(sl_NetAppGet)
long sl_NetAppGet(unsigned char AppId, unsigned char Option,unsigned char *pOptionLen, unsigned char *pOptionValue);
#endif



/*!

 Close the Doxygen group.
 @}

 */


#ifdef  __cplusplus
}
#endif /*  __cplusplus */

#endif    /*  __NETAPP_H__ */


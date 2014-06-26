/*
 * socket.c - CC31xx/CC32xx Host Driver Implementation
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
    

/*****************************************************************************

    API Prototypes

 *****************************************************************************/

#include "datatypes.h"
#include "simplelink.h"
#include "protocol.h"
#include "driver.h"


void   _sl_BuildAddress(const SlSockAddr_t *addr, int addrlen, _SocketAddrCommand_u    *pCmd);
void   _sl_ParseAddress(_SocketAddrResponse_u *pRsp, SlSockAddr_t *addr, SlSocklen_t *addrlen);
void   _sl_HandleAsync_Connect(void *pVoidBuf);
void   _sl_HandleAsync_Accept(void *pVoidBuf);
void   _sl_HandleAsync_Select(void *pVoidBuf);
unsigned int _sl_TruncatePayloadByProtocol(const UINT8 pSd,const unsigned int length);  



/* ******************************************************************************/
/*  _sl_BuildAddress */
/* ******************************************************************************/
void _sl_BuildAddress(const SlSockAddr_t *addr, int addrlen, _SocketAddrCommand_u    *pCmd)
{

    /*  Note: parsing of family and port in the generic way for all IPV4, IPV6 and EUI48 */
    /*  is possible as long as these parameters are in the same offset and size for these */
    /*  three families. */
    pCmd->IpV4.FamilyAndFlags = (addr->sa_family << 4) & 0xF0;
    pCmd->IpV4.port = ((SlSockAddrIn_t *)addr)->sin_port;

    if(SL_AF_INET == addr->sa_family)
    {
        pCmd->IpV4.address  = ((SlSockAddrIn_t *)addr)->sin_addr.s_addr;
    }
    else if (SL_AF_INET6_EUI_48 == addr->sa_family )
    {
        sl_Memcpy( pCmd->IpV6EUI48.address,((SlSockAddrIn6_t *)addr)->sin6_addr._S6_un._S6_u8, 6);
    }
#ifdef SL_SUPPORT_IPV6
    else
    {
        sl_Memcpy(pCmd->IpV6.address, ((sockaddr_in6 *)addr)->sin6_addr._S6_un._S6_u32, 16 );
    }
#endif
}

/* ******************************************************************************/
/*  _sl_TruncatePayloadByProtocol */
/* ******************************************************************************/
unsigned int _sl_TruncatePayloadByProtocol(const _SlSd_t sd,const unsigned int length)
{
   unsigned int maxLength; 
   
   switch(sd & SL_SOCKET_PAYLOAD_TYPE_MASK)
   {
      case SL_SOCKET_PAYLOAD_TYPE_UDP_IPV4:
         maxLength = 1472;
      break;
      
      case SL_SOCKET_PAYLOAD_TYPE_TCP_IPV4:
         maxLength = 1460;
      break;
      
      case SL_SOCKET_PAYLOAD_TYPE_UDP_IPV6:
         maxLength = 1452;
      break;
      
      case SL_SOCKET_PAYLOAD_TYPE_TCP_IPV6:
		  maxLength = 1440;
	  break;
      case SL_SOCKET_PAYLOAD_TYPE_TCP_IPV4_SECURE:
      case SL_SOCKET_PAYLOAD_TYPE_UDP_IPV4_SECURE:
		  maxLength = 1402;
	  break;
      case SL_SOCKET_PAYLOAD_TYPE_UDP_IPV6_SECURE:
      case SL_SOCKET_PAYLOAD_TYPE_TCP_IPV6_SECURE:
         maxLength = 1396;
      break;
      case SL_SOCKET_PAYLOAD_TYPE_RAW_TRANCEIVER:
           maxLength = 1476;
      break;
     case SL_SOCKET_PAYLOAD_TYPE_RAW_PACKET:
           maxLength = 1514;
      break;
	 case SL_SOCKET_PAYLOAD_TYPE_RAW_IP4:
           maxLength = 1480;
      break;
      default:
           maxLength = 1440;
      break;
   }
   
   if( length > maxLength )
   {
      return maxLength;
   }
   else
   {
      return length;
   }
}

/*******************************************************************************/
/*  _sl_ParseAddress */
/*******************************************************************************/
void _sl_ParseAddress(_SocketAddrResponse_u    *pRsp, SlSockAddr_t *addr, SlSocklen_t *addrlen)
{
    /*  Note: parsing of family and port in the generic way for all IPV4, IPV6 and EUI48 */
    /*  is possible as long as these parameters are in the same offset and size for these */
    /*  three families. */
    addr->sa_family                 = pRsp->IpV4.family;
    ((SlSockAddrIn_t *)addr)->sin_port = pRsp->IpV4.port;

    *addrlen = (SL_AF_INET == addr->sa_family) ? sizeof(SlSockAddrIn_t) : sizeof(SlSockAddrIn6_t);

    if(SL_AF_INET == addr->sa_family)
    {
        ((SlSockAddrIn_t *)addr)->sin_addr.s_addr  = pRsp->IpV4.address;
    }
    else if (SL_AF_INET6_EUI_48 == addr->sa_family )
    {
        sl_Memcpy(((SlSockAddrIn6_t *)addr)->sin6_addr._S6_un._S6_u8, pRsp->IpV6EUI48.address, 6);
    }
#ifdef SL_SUPPORT_IPV6
    else
    {
        sl_Memcpy(((sockaddr_in6 *)addr)->sin6_addr._S6_un._S6_u32, pRsp->IpV6.address, 16);
    }
#endif
}

/*******************************************************************************/
/* sl_Socket */
/*******************************************************************************/
typedef union
{
        UINT32                  Dummy;
	_SocketCommand_t 	Cmd;
	_SocketResponse_t	Rsp;
}_SlSockSocketMsg_u;

const _SlCmdCtrl_t _SlSockSocketCmdCtrl =
{
    SL_OPCODE_SOCKET_SOCKET,
    sizeof(_SocketCommand_t),
    sizeof(_SocketResponse_t)
};

#if _SL_INCLUDE_FUNC(sl_Socket)
int sl_Socket(int Domain, int Type, int Protocol)
{
    _SlSockSocketMsg_u  Msg;

    Msg.Cmd.Domain	    = (UINT8)Domain;
    Msg.Cmd.Type     	= (UINT8)Type;
    Msg.Cmd.Protocol 	= (UINT8)Protocol;

    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlSockSocketCmdCtrl, &Msg, NULL));

    if( Msg.Rsp.statusOrLen < 0 )
	{
		return( Msg.Rsp.statusOrLen );
	}
	else
	{
    return (int)((UINT8)Msg.Rsp.sd);
}
}
#endif

/*******************************************************************************/
/*  sl_Close  */
/*******************************************************************************/
typedef union
{
	_CloseCommand_t	    Cmd;
	_SocketResponse_t	Rsp;
}_SlSockCloseMsg_u;

const _SlCmdCtrl_t _SlSockCloseCmdCtrl =
{
	SL_OPCODE_SOCKET_CLOSE,
    sizeof(_CloseCommand_t),
    sizeof(_SocketResponse_t)
};

#if _SL_INCLUDE_FUNC(sl_Close)
int sl_Close(int sd)
{
	_SlSockCloseMsg_u   Msg;

    Msg.Cmd.sd = (UINT8)sd;

    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlSockCloseCmdCtrl, &Msg, NULL));

    return Msg.Rsp.statusOrLen;
}
#endif

/*******************************************************************************/
/*  sl_Bind */
/*******************************************************************************/
typedef union
{
	_SocketAddrCommand_u    Cmd;
	_SocketResponse_t	    Rsp;
}_SlSockBindMsg_u;

#if _SL_INCLUDE_FUNC(sl_Bind)
int sl_Bind(int sd, const SlSockAddr_t *addr, int addrlen)
{
	_SlSockBindMsg_u    Msg;
    _SlCmdCtrl_t         CmdCtrl = {0, 0, sizeof(_SocketResponse_t)};

    switch(addr->sa_family)
    {
        case SL_AF_INET :
            CmdCtrl.Opcode = SL_OPCODE_SOCKET_BIND;
            CmdCtrl.TxDescLen = sizeof(_SocketAddrIPv4Command_t);
            break;
        case SL_AF_INET6_EUI_48:
            CmdCtrl.Opcode = SL_OPCODE_SOCKET_BIND_V6;
            CmdCtrl.TxDescLen = sizeof(_SocketAddrIPv6EUI48Command_t);
        	break;
#ifdef SL_SUPPORT_IPV6
        case AF_INET6:
            CmdCtrl.Opcode = SL_OPCODE_SOCKET_BIND_V6;
            CmdCtrl.TxDescLen = sizeof(_SocketAddrIPv6Command_t);
            break;
#endif
        case SL_AF_RF   :
        default:
            return SL_RET_CODE_INVALID_INPUT;
    }

    Msg.Cmd.IpV4.lenOrPadding = 0;
    Msg.Cmd.IpV4.sd = (UINT8)sd;

    _sl_BuildAddress(addr, addrlen, &Msg.Cmd);

    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&CmdCtrl, &Msg, NULL));

    return Msg.Rsp.statusOrLen;
}
#endif

/*******************************************************************************/
/*  sl_Sendto */
/*******************************************************************************/
typedef union
{
    _SocketAddrCommand_u    Cmd;
    /*  no response for 'sendto' commands*/
}_SlSendtoMsg_u;

#if _SL_INCLUDE_FUNC(sl_SendTo)
int sl_SendTo(int sd, const void *pBuf, int Len, int flags, const SlSockAddr_t *to, SlSocklen_t tolen)
{
    _SlSendtoMsg_u   Msg;
    _SlCmdCtrl_t     CmdCtrl = {0, 0, 0};
    _SlCmdExt_t      CmdExt;
    UINT16           ChunkLen;
    int              RetVal;

    CmdExt.TxPayloadLen = (UINT16)Len;
    CmdExt.RxPayloadLen = 0;
    CmdExt.pTxPayload = (UINT8 *)pBuf;
    CmdExt.pRxPayload = NULL;


    switch(to->sa_family)
    {
        case SL_AF_INET:
            CmdCtrl.Opcode = SL_OPCODE_SOCKET_SENDTO;
            CmdCtrl.TxDescLen = sizeof(_SocketAddrIPv4Command_t);
            break;
        case SL_AF_INET6_EUI_48:
            CmdCtrl.Opcode = SL_OPCODE_SOCKET_BIND_V6;
            CmdCtrl.TxDescLen = sizeof(_SocketAddrIPv6EUI48Command_t);
        	break;
#ifdef SL_SUPPORT_IPV6
        case AF_INET6:
            CmdCtrl.Opcode = SL_OPCODE_SOCKET_SENDTO_V6;
            CmdCtrl.TxDescLen = sizeof(_SocketAddrIPv6Command_t);
            break;
#endif
        case SL_AF_RF:
        default:
            return SL_RET_CODE_INVALID_INPUT;
    }

    ChunkLen = _sl_TruncatePayloadByProtocol(sd,Len);
    Msg.Cmd.IpV4.lenOrPadding = ChunkLen;
    CmdExt.TxPayloadLen = ChunkLen;

    Msg.Cmd.IpV4.sd = (UINT8)sd;

    _sl_BuildAddress(to, tolen, &Msg.Cmd);

    Msg.Cmd.IpV4.FamilyAndFlags |= flags & 0x0F;

    do
    {
        RetVal = _SlDrvDataWriteOp((_SlSd_t)sd, &CmdCtrl, &Msg, &CmdExt);

        if(SL_OS_RET_CODE_OK == RetVal)
        {
            CmdExt.pTxPayload += ChunkLen;
            ChunkLen = (UINT16)((UINT8 *)pBuf + Len - CmdExt.pTxPayload);
            ChunkLen = _sl_TruncatePayloadByProtocol(sd,ChunkLen);
            CmdExt.TxPayloadLen = ChunkLen;
            Msg.Cmd.IpV4.lenOrPadding = ChunkLen;
        }
        else
        {
            return RetVal;
        }
    }while(ChunkLen > 0);

    return (int)Len;
}
#endif

/*******************************************************************************/
/*  sl_Recvfrom */
/*******************************************************************************/
typedef union
{
    _sendRecvCommand_t	    Cmd;
    _SocketAddrResponse_u	Rsp;
}_SlRecvfromMsg_u;

const _SlCmdCtrl_t _SlRecvfomCmdCtrl =
{
	SL_OPCODE_SOCKET_RECVFROM,
    sizeof(_sendRecvCommand_t),
    sizeof(_SocketAddrResponse_u)
};

#if _SL_INCLUDE_FUNC(sl_RecvFrom)
int sl_RecvFrom(int sd, void *buf, int Len, int flags, SlSockAddr_t *from, SlSocklen_t *fromlen)
{
    _SlRecvfromMsg_u    Msg;
    _SlCmdExt_t         CmdExt;
    int                 RetVal;

    CmdExt.TxPayloadLen = 0;
    CmdExt.RxPayloadLen = Len;
    CmdExt.pTxPayload = NULL;
    CmdExt.pRxPayload = (UINT8 *)buf;


    Msg.Cmd.sd = sd;
    Msg.Cmd.StatusOrLen = Len;
    /*  no size truncation in recv path */
    CmdExt.RxPayloadLen = Msg.Cmd.StatusOrLen;

    if(sizeof(SlSockAddrIn_t) == *fromlen)
    {
        Msg.Cmd.FamilyAndFlags = SL_AF_INET;
    }
    else if (sizeof(SlSockAddrIn6_t) == *fromlen)
    {
        Msg.Cmd.FamilyAndFlags = SL_AF_INET6;
    }
    else
    {
        return SL_RET_CODE_INVALID_INPUT;
    }

    Msg.Cmd.FamilyAndFlags = (Msg.Cmd.FamilyAndFlags << 4) & 0xF0;
    Msg.Cmd.FamilyAndFlags |= flags & 0x0F;

    RetVal = _SlDrvDataReadOp((_SlSd_t)sd, (_SlCmdCtrl_t *)&_SlRecvfomCmdCtrl, &Msg, &CmdExt);
    if( RetVal != SL_OS_RET_CODE_OK )
    {
	return RetVal;
    }

    RetVal = Msg.Rsp.IpV4.statusOrLen;

    if(RetVal >= 0)
    {
        VERIFY_PROTOCOL(sd == Msg.Rsp.IpV4.sd);
#if 0
        _sl_ParseAddress(&Msg.Rsp, from, fromlen);
#else
        from->sa_family = Msg.Rsp.IpV4.family;
        if(SL_AF_INET == from->sa_family)
        {
            ((SlSockAddrIn_t *)from)->sin_port = Msg.Rsp.IpV4.port;
            ((SlSockAddrIn_t *)from)->sin_addr.s_addr = Msg.Rsp.IpV4.address;
            *fromlen = sizeof(SlSockAddrIn_t);
        }
        else if (SL_AF_INET6_EUI_48 == from->sa_family )
         {
            ((SlSockAddrIn6_t *)from)->sin6_port  = Msg.Rsp.IpV6EUI48.port;
            sl_Memcpy(((SlSockAddrIn6_t *)from)->sin6_addr._S6_un._S6_u8, Msg.Rsp.IpV6EUI48.address, 6);
         }
#ifdef SL_SUPPORT_IPV6
        else if(AF_INET6 == from->sa_family)
        {
            VERIFY_PROTOCOL(*fromlen >= sizeof(sockaddr_in6));

            ((sockaddr_in6 *)from)->sin6_port = Msg.Rsp.IpV6.port;
            sl_Memcpy(((sockaddr_in6 *)from)->sin6_addr._S6_un._S6_u32, Msg.Rsp.IpV6.address, 16);
            *fromlen = sizeof(sockaddr_in6);
        }
#endif
#endif
    }

    return (int)RetVal;
}
#endif

/*******************************************************************************/
/*  sl_Connect */
/*******************************************************************************/
typedef union
{
	_SocketAddrCommand_u    Cmd;
	_SocketResponse_t	    Rsp;
}_SlSockConnectMsg_u;

#if _SL_INCLUDE_FUNC(sl_Connect)
int sl_Connect(int sd, const SlSockAddr_t *addr, int addrlen)
{
	_SlSockConnectMsg_u  Msg;
	_SlReturnVal_t       RetVal;
    _SlCmdCtrl_t         CmdCtrl = {0, 0, sizeof(_SocketResponse_t)};
    _SocketResponse_t    AsyncRsp;
	UINT8 pObjIdx = MAX_CONCURRENT_ACTIONS;


    switch(addr->sa_family)
    {
        case SL_AF_INET :
            CmdCtrl.Opcode = SL_OPCODE_SOCKET_CONNECT;
            CmdCtrl.TxDescLen = sizeof(_SocketAddrIPv4Command_t);
            break;
        case  SL_AF_INET6_EUI_48:
            CmdCtrl.Opcode = SL_OPCODE_SOCKET_CONNECT_V6;
            CmdCtrl.TxDescLen = sizeof(_SocketAddrIPv6EUI48Command_t);
        	break;
#ifdef SL_SUPPORT_IPV6
        case AF_INET6:
            CmdCtrl.Opcode = SL_OPCODE_SOCKET_CONNECT_V6;
            CmdCtrl.TxDescLen = sizeof(_SocketAddrIPv6Command_t);
            break;
#endif
        case SL_AF_RF   :
        default:
            return SL_RET_CODE_INVALID_INPUT;
    }

    Msg.Cmd.IpV4.lenOrPadding = 0;
    Msg.Cmd.IpV4.sd = sd;

    _sl_BuildAddress(addr, addrlen, &Msg.Cmd);

	/* Use Obj to issue the command, if not available try later */
	pObjIdx = _SlDrvWaitForPoolObj(CONNECT_ID, sd  & BSD_SOCKET_ID_MASK);

	if (MAX_CONCURRENT_ACTIONS == pObjIdx)
	{
		return SL_POOL_IS_EMPTY;
	}
	OSI_RET_OK_CHECK(sl_LockObjLock(&g_pCB->ProtectionLockObj, SL_OS_WAIT_FOREVER));

	g_pCB->ObjPool[pObjIdx].pRespArgs =  (UINT8 *)&AsyncRsp;

    OSI_RET_OK_CHECK(sl_LockObjUnlock(&g_pCB->ProtectionLockObj));
	
	/* send the command */
    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&CmdCtrl, &Msg, NULL));
    VERIFY_PROTOCOL(Msg.Rsp.sd == sd)

	RetVal = Msg.Rsp.statusOrLen;

    if(SL_RET_CODE_OK == RetVal)
    {
		/*  wait for async and get Data Read parameters */
        OSI_RET_OK_CHECK(sl_SyncObjWait(&g_pCB->ObjPool[pObjIdx].SyncObj, SL_OS_WAIT_FOREVER));

        VERIFY_PROTOCOL(AsyncRsp.sd == sd);

        RetVal = AsyncRsp.statusOrLen;
    }
    _SlDrvReleasePoolObj(pObjIdx);
    return RetVal;
}
#endif

/*******************************************************************************/
/*   _sl_HandleAsync_Connect */
/*******************************************************************************/
void _sl_HandleAsync_Connect(void *pVoidBuf)
{
    _SocketResponse_t          *pMsgArgs   = (_SocketResponse_t *)_SL_RESP_ARGS_START(pVoidBuf);

    OSI_RET_OK_CHECK(sl_LockObjLock(&g_pCB->ProtectionLockObj, SL_OS_WAIT_FOREVER));

    VERIFY_PROTOCOL((pMsgArgs->sd & BSD_SOCKET_ID_MASK) <= SL_MAX_SOCKETS);
    VERIFY_SOCKET_CB(NULL != g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs);

    ((_SocketResponse_t *)(g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs))->sd = pMsgArgs->sd;
    ((_SocketResponse_t *)(g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs))->statusOrLen = pMsgArgs->statusOrLen;

    OSI_RET_OK_CHECK(sl_SyncObjSignal(&g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].SyncObj));
    OSI_RET_OK_CHECK(sl_LockObjUnlock(&g_pCB->ProtectionLockObj));
    return;
}

/*******************************************************************************/
/*  sl_Sendto */
/*******************************************************************************/
typedef union
{
	_sendRecvCommand_t    Cmd;
    /*  no response for 'sendto' commands*/
}_SlSendMsg_u;

const _SlCmdCtrl_t _SlSendCmdCtrl =
{
    SL_OPCODE_SOCKET_SEND,
    sizeof(_sendRecvCommand_t),
    0
};

#if _SL_INCLUDE_FUNC(sl_Send)
int sl_Send(int sd, const void *pBuf, int Len, int flags)
{
    _SlSendMsg_u   Msg;
    _SlCmdExt_t    CmdExt;
    UINT16         ChunkLen;
    int            RetVal;
	UINT32         tempVal;
	unsigned char  runSingleChunk = FALSE; 

    CmdExt.TxPayloadLen = Len;
    CmdExt.RxPayloadLen = 0;
    CmdExt.pTxPayload = (UINT8 *)pBuf;

    /* Only for RAW transceiver type socket, relay the flags parameter in the 2 bytes (4 byte aligned) before the actual payload */
    if ((sd & SL_SOCKET_PAYLOAD_TYPE_MASK) == SL_SOCKET_PAYLOAD_TYPE_RAW_TRANCEIVER)
    {
		tempVal = flags;
        CmdExt.pRxPayload = (UINT8 *)&tempVal;
		CmdExt.RxPayloadLen = 4;
        g_pCB->RelayFlagsViaRxPayload = TRUE;
		runSingleChunk = TRUE;
    }
    else
    {
        CmdExt.pRxPayload = NULL;
    }

    ChunkLen = _sl_TruncatePayloadByProtocol(sd,Len);
    CmdExt.TxPayloadLen = ChunkLen;

    Msg.Cmd.StatusOrLen = ChunkLen;
    Msg.Cmd.sd = sd;
    Msg.Cmd.FamilyAndFlags |= flags & 0x0F;

    do
    {
        RetVal = _SlDrvDataWriteOp(sd, (_SlCmdCtrl_t *)&_SlSendCmdCtrl, &Msg, &CmdExt);
        if(SL_OS_RET_CODE_OK == RetVal)
        {
            CmdExt.pTxPayload += ChunkLen;
            ChunkLen = (UINT8 *)pBuf + Len - CmdExt.pTxPayload;
            ChunkLen = _sl_TruncatePayloadByProtocol(sd,ChunkLen);
            CmdExt.TxPayloadLen = ChunkLen;
            Msg.Cmd.StatusOrLen = ChunkLen;
        }
        else
        {
            return RetVal;
        }
    }while((ChunkLen > 0) && (runSingleChunk==FALSE));
    
    return (int)Len;
}
#endif

/*******************************************************************************/
/*  sl_Listen */
/*******************************************************************************/
typedef union
{
	_ListenCommand_t    Cmd;
    _BasicResponse_t    Rsp;
}_SlListenMsg_u;

const _SlCmdCtrl_t _SlListenCmdCtrl =
{
    SL_OPCODE_SOCKET_LISTEN,
    sizeof(_ListenCommand_t),
    sizeof(_BasicResponse_t),
};

#if _SL_INCLUDE_FUNC(sl_Listen)
int sl_Listen(int sd, int backlog)
{
    _SlListenMsg_u  Msg;

    Msg.Cmd.sd = sd;
    Msg.Cmd.backlog = backlog;

    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlListenCmdCtrl, &Msg, NULL));

    return (int)Msg.Rsp.status;
}
#endif

/*******************************************************************************/
/*  sl_Accept */
/*******************************************************************************/
typedef union
{
	_AcceptCommand_t    Cmd;
	_SocketResponse_t   Rsp;
}_SlSockAcceptMsg_u;

const _SlCmdCtrl_t _SlAcceptCmdCtrl =
{
    SL_OPCODE_SOCKET_ACCEPT,
    sizeof(_AcceptCommand_t),
    sizeof(_BasicResponse_t),
};

#if _SL_INCLUDE_FUNC(sl_Accept)
int sl_Accept(int sd, SlSockAddr_t *addr, SlSocklen_t *addrlen)
{
	_SlSockAcceptMsg_u      Msg;
    _SlReturnVal_t          RetVal;
    _SocketAddrResponse_u   AsyncRsp;

	UINT8 pObjIdx = MAX_CONCURRENT_ACTIONS;


    Msg.Cmd.sd = sd;
    Msg.Cmd.family = (sizeof(SlSockAddrIn_t) == *addrlen) ? SL_AF_INET : SL_AF_INET6;

	/* Use Obj to issue the command, if not available try later */
	pObjIdx = _SlDrvWaitForPoolObj(ACCEPT_ID, sd  & BSD_SOCKET_ID_MASK);
	
	if (MAX_CONCURRENT_ACTIONS == pObjIdx)
	{
		return SL_POOL_IS_EMPTY;
	}
	
    OSI_RET_OK_CHECK(sl_LockObjLock(&g_pCB->ProtectionLockObj, SL_OS_WAIT_FOREVER));

	g_pCB->ObjPool[pObjIdx].pRespArgs = (UINT8 *)&AsyncRsp;

    OSI_RET_OK_CHECK(sl_LockObjUnlock(&g_pCB->ProtectionLockObj));
	/* send the command */
    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlAcceptCmdCtrl, &Msg, NULL));
    VERIFY_PROTOCOL(Msg.Rsp.sd == sd);

    RetVal = Msg.Rsp.statusOrLen;

    if(SL_OS_RET_CODE_OK == RetVal)
    {
        /*  wait for async and get Data Read parameters */
		OSI_RET_OK_CHECK(sl_SyncObjWait(&g_pCB->ObjPool[pObjIdx].SyncObj, SL_OS_WAIT_FOREVER));

        VERIFY_PROTOCOL(AsyncRsp.IpV4.sd == sd);

        RetVal = AsyncRsp.IpV4.statusOrLen;
        if( (NULL != addr) && (NULL != addrlen) )  
        {
#if 0 /*  Kept for backup */
            _sl_ParseAddress(&AsyncRsp, addr, addrlen);
#else
    	   addr->sa_family = AsyncRsp.IpV4.family;

    	    if(SL_AF_INET == addr->sa_family)
    	    {
              if( *addrlen == sizeof( SlSockAddrIn_t ) )
              {
                ((SlSockAddrIn_t *)addr)->sin_port         = AsyncRsp.IpV4.port;
                ((SlSockAddrIn_t *)addr)->sin_addr.s_addr  = AsyncRsp.IpV4.address;
              }
              else
              {
                *addrlen = 0;
              }
    	    }
    	    else if (SL_AF_INET6_EUI_48 == addr->sa_family )
    	    {
                if( *addrlen == sizeof( SlSockAddrIn6_t ) )
              {
                ((SlSockAddrIn6_t *)addr)->sin6_port                   = AsyncRsp.IpV6EUI48.port    ;
                /*  will be called from here and from _sl_BuildAddress*/
                sl_Memcpy(((SlSockAddrIn6_t *)addr)->sin6_addr._S6_un._S6_u8, AsyncRsp.IpV6EUI48.address, 6);
              }
              else
              {
                *addrlen = 0;
              }
    	    }
#ifdef SL_SUPPORT_IPV6
    	    else
    	    {
              if( *addrlen == sizeof( sockaddr_in6 ) )
              {
    	        ((sockaddr_in6 *)addr)->sin6_port                   = AsyncRsp.IpV6.port    ;
    	        sl_Memcpy(((sockaddr_in6 *)addr)->sin6_addr._S6_un._S6_u32, AsyncRsp.IpV6.address, 16);
              }
              else
              {
                *addrlen = 0;
              }
    	    }
#endif
#endif			
        }
    }

    _SlDrvReleasePoolObj(pObjIdx);
    return (int)RetVal;
}
#endif


/*******************************************************************************/
/*  sl_Htonl */
/*******************************************************************************/
unsigned long sl_Htonl( unsigned long val )
{
  unsigned long i = 1; 
  char *p = (char *)&i;  
  if (p[0] == 1) /* little endian */
  {
    p[0] = ((char*)&val)[3];
    p[1] = ((char*)&val)[2];
    p[2] = ((char*)&val)[1];
    p[3] = ((char*)&val)[0];
    return i;
  }
  else /* big endian */
  {
    return val; 
  }
}

/*******************************************************************************/
/*  sl_Htonl */
/*******************************************************************************/
unsigned short sl_Htons( unsigned short val )
{
  short i = 1; 
  char *p = (char *)&i;  
  if (p[0] == 1) /* little endian */
  {
    p[0] = ((char*)&val)[1];
    p[1] = ((char*)&val)[0];
    return i;
  }
  else /* big endian */
  {
    return val; 
  }
}

/*******************************************************************************/
/*   _sl_HandleAsync_Accept */
/*******************************************************************************/
void _sl_HandleAsync_Accept(void *pVoidBuf)
{
    _SocketAddrResponse_u      *pMsgArgs   = (_SocketAddrResponse_u *)_SL_RESP_ARGS_START(pVoidBuf);

    OSI_RET_OK_CHECK(sl_LockObjLock(&g_pCB->ProtectionLockObj, SL_OS_WAIT_FOREVER));

    VERIFY_PROTOCOL(( pMsgArgs->IpV4.sd & BSD_SOCKET_ID_MASK) <= SL_MAX_SOCKETS);
    VERIFY_SOCKET_CB(NULL != g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs);

	memcpy(g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs, pMsgArgs,sizeof(_SocketAddrResponse_u));
	OSI_RET_OK_CHECK(sl_SyncObjSignal(&g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].SyncObj));

    OSI_RET_OK_CHECK(sl_LockObjUnlock(&g_pCB->ProtectionLockObj));
    return;
}

/*******************************************************************************/
/*  sl_Recv */
/*******************************************************************************/
typedef union
{
	_sendRecvCommand_t  Cmd;
	_SocketResponse_t   Rsp;    
}_SlRecvMsg_u;

const _SlCmdCtrl_t _SlRecvCmdCtrl =
{
    SL_OPCODE_SOCKET_RECV,
    sizeof(_sendRecvCommand_t),
    sizeof(_SocketResponse_t)
};

#if _SL_INCLUDE_FUNC(sl_Recv)
int sl_Recv(int sd, void *pBuf, int Len, int flags)
{
    _SlRecvMsg_u    Msg;
    _SlCmdExt_t     CmdExt;
    _SlReturnVal_t status;

    CmdExt.TxPayloadLen = 0;
    CmdExt.RxPayloadLen = Len;
    CmdExt.pTxPayload = NULL;
    CmdExt.pRxPayload = (UINT8 *)pBuf;

    Msg.Cmd.sd = sd;
    Msg.Cmd.StatusOrLen = Len;

    /*  no size truncation in recv path */
    CmdExt.RxPayloadLen = Msg.Cmd.StatusOrLen;

    Msg.Cmd.FamilyAndFlags = flags & 0x0F;

    status = _SlDrvDataReadOp((_SlSd_t)sd, (_SlCmdCtrl_t *)&_SlRecvCmdCtrl, &Msg, &CmdExt);
    if( status != SL_OS_RET_CODE_OK )
    {
	return status;
    }
     
    /*  if the Device side sends less than expected it is not the Driver's role */
    /*  the returned value could be smaller than the requested size */
    return (int)Msg.Rsp.statusOrLen;
}
#endif

/*******************************************************************************/
/*  sl_SetSockOpt */
/*******************************************************************************/
typedef union
{
	_setSockOptCommand_t    Cmd;
	_SocketResponse_t       Rsp;    
}_SlSetSockOptMsg_u;

const _SlCmdCtrl_t _SlSetSockOptCmdCtrl =
{
    SL_OPCODE_SOCKET_SETSOCKOPT,
    sizeof(_setSockOptCommand_t),
    sizeof(_SocketResponse_t)
};

#if _SL_INCLUDE_FUNC(sl_SetSockOpt)
int sl_SetSockOpt(int sd, int level, int optname, const void *optval, SlSocklen_t optlen)
{
    _SlSetSockOptMsg_u    Msg;
    _SlCmdExt_t           CmdExt;

    CmdExt.TxPayloadLen = optlen;
    CmdExt.RxPayloadLen = 0;
    CmdExt.pTxPayload = (UINT8 *)optval;
    CmdExt.pRxPayload = NULL;

    Msg.Cmd.sd = sd;
    Msg.Cmd.level = level;
    Msg.Cmd.optionLen = optlen;
    Msg.Cmd.optionName = optname;

    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlSetSockOptCmdCtrl, &Msg, &CmdExt));

    return (int)Msg.Rsp.statusOrLen;
}
#endif

/*******************************************************************************/
/*  sl_GetSockOpt */
/*******************************************************************************/
typedef union
{
	_getSockOptCommand_t    Cmd;
	_getSockOptResponse_t   Rsp;    
}_SlGetSockOptMsg_u;

const _SlCmdCtrl_t _SlGetSockOptCmdCtrl =
{
    SL_OPCODE_SOCKET_GETSOCKOPT,
    sizeof(_getSockOptCommand_t),
    sizeof(_getSockOptResponse_t)
};

#if _SL_INCLUDE_FUNC(sl_GetSockOpt)
int sl_GetSockOpt(int sd, int level, int optname, void *optval, SlSocklen_t *optlen)
{
    _SlGetSockOptMsg_u    Msg;
    _SlCmdExt_t           CmdExt;

	if (*optlen == 0)
	{
		return SL_EZEROLEN;
	}
    CmdExt.TxPayloadLen = 0;
    CmdExt.RxPayloadLen = *optlen;
    CmdExt.pTxPayload = NULL;
    CmdExt.pRxPayload = optval;
	CmdExt.ActualRxPayloadLen = 0;

    Msg.Cmd.sd = sd;
    Msg.Cmd.level = level;
    Msg.Cmd.optionLen = *optlen;
    Msg.Cmd.optionName = optname;

    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlGetSockOptCmdCtrl, &Msg, &CmdExt));

	if (CmdExt.RxPayloadLen < CmdExt.ActualRxPayloadLen) 
	{
	    *optlen = Msg.Rsp.optionLen;
	   return SL_ESMALLBUF;
	}
	else
	{
		*optlen = (UINT8)CmdExt.ActualRxPayloadLen;
	}
    return (int)Msg.Rsp.status;
}
#endif

/*******************************************************************************/
/*  sl_Select */
/* ******************************************************************************/
typedef union
{
	_SelectCommand_t   Cmd;
	_BasicResponse_t   Rsp;    
}_SlSelectMsg_u;

const _SlCmdCtrl_t _SlSelectCmdCtrl =
{
    SL_OPCODE_SOCKET_SELECT,
    sizeof(_SelectCommand_t),
    sizeof(_BasicResponse_t)
};

#if _SL_INCLUDE_FUNC(sl_Select)
int sl_Select(int nfds, SlFdSet_t *readsds, SlFdSet_t *writesds, SlFdSet_t *exceptsds, struct SlTimeval_t *timeout)
{
    _SlSelectMsg_u          Msg;
    _SelectAsyncResponse_t  AsyncRsp;
	UINT8 pObjIdx = MAX_CONCURRENT_ACTIONS;

    Msg.Cmd.nfds          = nfds;
    Msg.Cmd.readFdsCount  = 0;
    Msg.Cmd.writeFdsCount = 0;
    
    Msg.Cmd.readFds = 0;
    Msg.Cmd.writeFds = 0; 
    
    if( readsds )
    {
       Msg.Cmd.readFds       = (UINT16)readsds->fd_array[0];  
    }
    if( writesds )
    {
       Msg.Cmd.writeFds      = (UINT16)writesds->fd_array[0]; 
    }
	if( NULL == timeout )
	{
		Msg.Cmd.tv_sec = 0xffff;
		Msg.Cmd.tv_usec = 0xffff;
	}
	else
	{
		if( 0xffff <= timeout->tv_sec )	
		{
			Msg.Cmd.tv_sec = 0xffff;
		}
		else
		{
			Msg.Cmd.tv_sec = (UINT16)timeout->tv_sec;
		}
		timeout->tv_usec = timeout->tv_usec >> 10;  /*  convert to milliseconds */
		if( 0xffff <= timeout->tv_usec )	
		{
			Msg.Cmd.tv_usec = 0xffff;
		}
		else
		{
			Msg.Cmd.tv_usec = (UINT16)timeout->tv_usec;
		}
	}

	/* Use Obj to issue the command, if not available try later */
	pObjIdx = _SlDrvWaitForPoolObj(SELECT_ID, SL_MAX_SOCKETS);

	if (MAX_CONCURRENT_ACTIONS == pObjIdx)
	{
		return SL_POOL_IS_EMPTY;
	}
	OSI_RET_OK_CHECK(sl_LockObjLock(&g_pCB->ProtectionLockObj, SL_OS_WAIT_FOREVER));

	g_pCB->ObjPool[pObjIdx].pRespArgs =  (UINT8 *)&AsyncRsp;

    OSI_RET_OK_CHECK(sl_LockObjUnlock(&g_pCB->ProtectionLockObj));
	/* send the command */
    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlSelectCmdCtrl, &Msg, NULL));

    if(SL_OS_RET_CODE_OK == (int)Msg.Rsp.status)
    {
        OSI_RET_OK_CHECK(sl_SyncObjWait(&g_pCB->ObjPool[pObjIdx].SyncObj, SL_OS_WAIT_FOREVER));
        Msg.Rsp.status = AsyncRsp.status;

        if(  ((int)Msg.Rsp.status) >= 0 )
        {
            if( readsds )
            {
               readsds->fd_array[0]  = AsyncRsp.readFds;
            }
            if( writesds )
            {
               writesds->fd_array[0] = AsyncRsp.writeFds;      
            }
        }
    }

    _SlDrvReleasePoolObj(pObjIdx);
    return (int)Msg.Rsp.status;
}

/*  Select helper functions */
/*******************************************************************************/
/*  SL_FD_SET */
/* ******************************************************************************/
void SL_FD_SET(int fd, SlFdSet_t *fdset)
{
   fdset->fd_array[0] |=  (1<< (fd & BSD_SOCKET_ID_MASK));
}
/*******************************************************************************/
/*  SL_FD_CLR */
/*******************************************************************************/
void SL_FD_CLR(int fd, SlFdSet_t *fdset)
{
  fdset->fd_array[0] &=  ~(1<< (fd & BSD_SOCKET_ID_MASK));
}
/*******************************************************************************/
/*  SL_FD_ISSET */
/*******************************************************************************/
int  SL_FD_ISSET(int fd, SlFdSet_t *fdset)
{
  if( fdset->fd_array[0] & (1<< (fd & BSD_SOCKET_ID_MASK)) )
  {
    return 1;
  }
  return 0;
}
/*******************************************************************************/
/*  SL_FD_ZERO */
/*******************************************************************************/  
void SL_FD_ZERO(SlFdSet_t *fdset)
{
  fdset->fd_array[0] = 0;
}

#endif

/*******************************************************************************/
/*   _sl_HandleAsync_Select */
/*******************************************************************************/
void _sl_HandleAsync_Select(void *pVoidBuf)
{
    _SelectAsyncResponse_t     *pMsgArgs   = (_SelectAsyncResponse_t *)_SL_RESP_ARGS_START(pVoidBuf);

    OSI_RET_OK_CHECK(sl_LockObjLock(&g_pCB->ProtectionLockObj, SL_OS_WAIT_FOREVER));

    VERIFY_SOCKET_CB(NULL != g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs);

    memcpy(g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs, pMsgArgs, sizeof(_SelectAsyncResponse_t));
    OSI_RET_OK_CHECK(sl_SyncObjSignal(&g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].SyncObj));

    OSI_RET_OK_CHECK(sl_LockObjUnlock(&g_pCB->ProtectionLockObj));
    return;
}


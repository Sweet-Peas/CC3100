#include "simplelink.h"

P_EVENT_HANDLER        pIrqEventHandler = 0;

void SimpleLinkWlanEventHandler(SlWlanEvent_t *pSlWlanEvent)
{

}

void sl_GeneralEvtHdlr(SlDeviceEvent_t *pSlDeviceEvent)
{

}

void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pSlSockEvent)
{

}

void sl_SockEvtHdlr(SlSockEvent_t *pSlSockEvent)
{

}

void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pSlHttpServerEvent,
    SlHttpServerResponse_t *pSlHttpServerResponse)
{

}

int registerInterruptHandler(P_EVENT_HANDLER InterruptHdl , void* pValue)
{
    pIrqEventHandler = InterruptHdl;

    return 0;
}

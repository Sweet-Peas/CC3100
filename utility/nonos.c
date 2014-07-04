/*
 * nonos.c - CC31xx/CC32xx Host Driver Implementation
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

#include "datatypes.h"
#include "simplelink.h"
#include "protocol.h"
#include "driver.h"

#ifndef SL_PLATFORM_MULTI_THREADED

#include "nonos.h"

#define NONOS_MAX_SPAWN_ENTRIES		5

typedef struct
{
	_SlSpawnEntryFunc_t 		pEntry;
	void* 						pValue;
}_SlNonOsSpawnEntry_t;

typedef struct
{
	_SlNonOsSpawnEntry_t	SpawnEntries[NONOS_MAX_SPAWN_ENTRIES];
}_SlNonOsCB_t;

_SlNonOsCB_t g__SlNonOsCB;


_SlNonOsRetVal_t _SlNonOsSemSet(_SlNonOsSemObj_t* pSemObj , _SlNonOsSemObj_t Value)
{
	*pSemObj = Value;
	return NONOS_RET_OK;
}

_SlNonOsRetVal_t _SlNonOsSemGet(_SlNonOsSemObj_t* pSyncObj, _SlNonOsSemObj_t WaitValue, _SlNonOsSemObj_t SetValue, _SlNonOsTime_t Timeout)
{
	while (Timeout>0)
	{
		if (WaitValue == *pSyncObj)
		{
			*pSyncObj = SetValue;
			break;
		}
		if (Timeout != NONOS_WAIT_FOREVER)
		{		
			Timeout--;
		}
		_SlNonOsMainLoopTask();
	}
	
	if (0 == Timeout)
	{
		return NONOS_RET_ERR;
	}
	else
	{
		return NONOS_RET_OK;
	}
}


_SlNonOsRetVal_t _SlNonOsSpawn(_SlSpawnEntryFunc_t pEntry , void* pValue , unsigned long flags)
{
	int i;
	
	for (i=0 ; i<NONOS_MAX_SPAWN_ENTRIES ; i++)
	{
		_SlNonOsSpawnEntry_t* pE = &g__SlNonOsCB.SpawnEntries[i];
	
		if (NULL == pE->pEntry)
		{
			pE->pValue = pValue;
			pE->pEntry = pEntry;
			break;
		}
	}
        
        
        return NONOS_RET_OK;
}


_SlNonOsRetVal_t _SlNonOsMainLoopTask(void)
{
	int i;
	
	for (i=0 ; i<NONOS_MAX_SPAWN_ENTRIES ; i++)
	{
		_SlNonOsSpawnEntry_t* pE = &g__SlNonOsCB.SpawnEntries[i];
		_SlSpawnEntryFunc_t 		pF = pE->pEntry;
		
		if (NULL != pF)
		{
                        if((g_pCB)->RxIrqCnt != (g_pCB)->RxDoneCnt)
            {
				pF(0);/*(pValue);*/
		}
			pE->pEntry = NULL;
			pE->pValue = NULL;
		}
	}
        
        return NONOS_RET_OK;
}
    
#endif /*(SL_PLATFORM != SL_PLATFORM_NON_OS)*/

/*-------------------------------- Arctic Core ------------------------------
 * Copyright (C) 2013, ArcCore AB, Sweden, www.arccore.com.
 * Contact: <contact@arccore.com>
 * 
 * You may ONLY use this file:
 * 1)if you have a valid commercial ArcCore license and then in accordance with  
 * the terms contained in the written license agreement between you and ArcCore, 
 * or alternatively
 * 2)if you follow the terms found in GNU General Public License version 2 as 
 * published by the Free Software Foundation and appearing in the file 
 * LICENSE.GPL included in the packaging of this file or here 
 * <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>
 *-------------------------------- Arctic Core -----------------------------*/



/*
 *  General requirements
 */
/** @req DCM600 */ // Generated by BSW builder


#include <string.h>
#include "Dcm.h"
#include "Dcm_Internal.h"
#include "MemMap.h"

typedef struct {
	const PduInfoType 				*pduRxData;
	PduInfoType 					*pduTxData;
	const Dcm_DsdServiceTableType	*serviceTable;
	Dcm_ProtocolAddrTypeType		addrType;
	PduIdType 						pdurTxPduId;
	PduIdType 						rxPduId;
	Dcm_ProtocolTransTypeType       txType;
	boolean                         internalRequest;
	boolean                         sendRespPendOnTransToBoot;
} MsgDataType;

// In "queue" to DSD
static boolean	dsdDslDataIndication = FALSE;
static MsgDataType msgData;

static uint8	currentSid;
static boolean	suppressPosRspMsg;

#if (DCM_MANUFACTURER_NOTIFICATION == STD_ON)
static uint16   Dcm_Dsd_sourceAddress;
static Dcm_ProtocolAddrTypeType Dcm_Dsd_requestType;
#endif
/*
 * Local functions
 */


#if (DCM_MANUFACTURER_NOTIFICATION == STD_ON)   /** @req DCM218 */
static Std_ReturnType ServiceIndication(uint8 sid, uint8 *requestData, uint16 dataSize, Dcm_ProtocolAddrTypeType reqType,
                                        uint16 sourceAddress, Dcm_NegativeResponseCodeType* errorCode)
{
    Std_ReturnType returnCode = E_OK;
    const Dcm_DslServiceRequestNotificationType *serviceRequestNotification = Dcm_ConfigPtr->Dsl->DslServiceRequestNotification;

    /* If any indcation returns E_REQUEST_NOT_ACCEPTED it shall be returned.
     * If all indcations returns E_OK it shall be returned
     * Otherwise, E_NOT_OK shall be returned.
     */

    while ((!serviceRequestNotification->Arc_EOL) && (returnCode != E_REQUEST_NOT_ACCEPTED)) {
        if (serviceRequestNotification->Indication != NULL) {
            Std_ReturnType result = serviceRequestNotification->Indication(sid, requestData + 1, dataSize - 1, reqType, sourceAddress, errorCode);
            if ((result == E_REQUEST_NOT_ACCEPTED) || (result == E_NOT_OK)) {
                returnCode = result;
            }
            else if (result != E_OK) {
                DCM_DET_REPORTERROR(DCM_GLOBAL_ID, DCM_E_UNEXPECTED_RESPONSE);
            }
            else {
                /* E_OK */
            }
        }
        serviceRequestNotification++;
    }

    return returnCode;
}


static void ServiceConfirmation(uint8 sid, Dcm_ProtocolAddrTypeType reqType, uint16 sourceAddress, NotifResultType result)
{
    Std_ReturnType returnCode = E_OK;
    const Dcm_DslServiceRequestNotificationType *serviceRequestNotification= Dcm_ConfigPtr->Dsl->DslServiceRequestNotification;

    /* The values of Dcm_ConfirmationStatusType are not specified, using the following */
    Dcm_ConfirmationStatusType status;
    if (msgData.pduTxData->SduDataPtr[0] == SID_NEGATIVE_RESPONSE) {
        status = (result == NTFRSLT_OK) ? DCM_RES_NEG_OK: DCM_RES_NEG_NOT_OK;
    }
    else {
        status = (result == NTFRSLT_OK) ? DCM_RES_POS_OK: DCM_RES_POS_NOT_OK;
    }

    /* If all indcations returns E_OK it shall be returned
     * Otherwise, E_NOT_OK shall be returned.
     */

    /* @req DCM770 */ /* @req DCM741 */
    while ((!serviceRequestNotification->Arc_EOL) && (returnCode != E_NOT_OK)) {
        if (serviceRequestNotification->Confirmation != NULL) {
            /* No way to terminate response like since it is already done */
            (void)serviceRequestNotification->Confirmation(sid, reqType, sourceAddress, status);
        }
        serviceRequestNotification++;
    }

}

#endif




#define IS_SUPRESSED_NRC_ON_FUNC_ADDR(_x) ((DCM_E_SERVICENOTSUPPORTED == (_x)) || (DCM_E_SUBFUNCTIONNOTSUPPORTED == (_x)) || (DCM_E_REQUESTOUTOFRANGE == (_x)))
static void createAndSendNcr(Dcm_NegativeResponseCodeType responseCode)
{
    PduIdType dummyId = 0;
    /* Suppress reponse if DCM001 is fulfilled or if it is a type2 response and a NRC */
    if( ((msgData.addrType == DCM_PROTOCOL_FUNCTIONAL_ADDR_TYPE) && IS_SUPRESSED_NRC_ON_FUNC_ADDR(responseCode)) ||/** @req DCM001 */
            ((DCM_E_POSITIVERESPONSE != responseCode) && DslPduRPduUsedForType2Tx(msgData.pdurTxPduId, &dummyId)) ) {
        DslDsdProcessingDone(msgData.rxPduId, DSD_TX_RESPONSE_SUPPRESSED);
    } else {
        msgData.pduTxData->SduDataPtr[0] = SID_NEGATIVE_RESPONSE;
        msgData.pduTxData->SduDataPtr[1] = currentSid;
        msgData.pduTxData->SduDataPtr[2] = responseCode;
        msgData.pduTxData->SduLength = 3;
        DslDsdProcessingDone(msgData.rxPduId, DSD_TX_RESPONSE_READY);   /** @req DCM114 */ /** @req DCM232.Ncr */
    }
}


static void selectServiceFunction(const Dcm_DsdServiceType *sidConfPtr)
{
    /** @req DCM442.Partially */
    switch (sidConfPtr->DsdSidTabServiceId)	 /** @req DCM221 */
    {
#ifdef DCM_USE_SERVICE_DIAGNOSTICSESSIONCONTROL
    case SID_DIAGNOSTIC_SESSION_CONTROL:
        DspUdsDiagnosticSessionControl(msgData.pduRxData, msgData.pdurTxPduId, msgData.pduTxData, msgData.sendRespPendOnTransToBoot, msgData.internalRequest);
        break;
#endif

#ifdef DCM_USE_SERVICE_ECURESET
    case SID_ECU_RESET:
		DspUdsEcuReset(sidConfPtr, msgData.pduRxData, msgData.pdurTxPduId, msgData.pduTxData);
        break;
#endif

#if defined(USE_DEM) && defined(DCM_USE_SERVICE_CLEARDIAGNOSTICINFORMATION)
    case SID_CLEAR_DIAGNOSTIC_INFORMATION:
        DspUdsClearDiagnosticInformation(msgData.pduRxData, msgData.pduTxData);
        break;
#endif

#if defined(USE_DEM) && defined(DCM_USE_SERVICE_READDTCINFORMATION)
    case SID_READ_DTC_INFORMATION:
        DspUdsReadDtcInformation(msgData.pduRxData, msgData.pduTxData);
        break;
#endif

#ifdef DCM_USE_SERVICE_READDATABYIDENTIFIER
    case SID_READ_DATA_BY_IDENTIFIER:
        DspUdsReadDataByIdentifier(msgData.pduRxData, msgData.pduTxData);
        break;
#endif

#ifdef DCM_USE_SERVICE_READMEMORYBYADDRESS
    case SID_READ_MEMORY_BY_ADDRESS:
        DspUdsReadMemoryByAddress(msgData.pduRxData, msgData.pduTxData);
        break;
#endif

#ifdef DCM_USE_SERVICE_WRITEMEMORYBYADDRESS
    case SID_WRITE_MEMORY_BY_ADDRESS:
        DspUdsWriteMemoryByAddress(msgData.pduRxData, msgData.pduTxData);
        break;
#endif

#ifdef DCM_USE_SERVICE_READSCALINGDATABYIDENTIFIER
    case SID_READ_SCALING_DATA_BY_IDENTIFIER:
        DspUdsReadScalingDataByIdentifier(msgData.pduRxData, msgData.pduTxData);
        break;
#endif

#ifdef DCM_USE_SERVICE_SECURITYACCESS
    case SID_SECURITY_ACCESS:
        DspUdsSecurityAccess(msgData.pduRxData, msgData.pduTxData);
        break;
#endif

#ifdef DCM_USE_SERVICE_WRITEDATABYIDENTIFIER
    case SID_WRITE_DATA_BY_IDENTIFIER:
        DspUdsWriteDataByIdentifier(msgData.pduRxData, msgData.pduTxData);
        break;
#endif

#ifdef DCM_USE_SERVICE_ROUTINECONTROL
    case SID_ROUTINE_CONTROL:
        DspUdsRoutineControl(msgData.pduRxData, msgData.pduTxData);
        break;
#endif

#ifdef DCM_USE_SERVICE_TESTERPRESENT
    case SID_TESTER_PRESENT:
        DspUdsTesterPresent(msgData.pduRxData, msgData.pduTxData);
        break;
#endif

#if defined(USE_DEM) && defined(DCM_USE_SERVICE_CONTROLDTCSETTING)
    case SID_CONTROL_DTC_SETTING:
        DspUdsControlDtcSetting(msgData.pduRxData, msgData.pduTxData);
        break;
#endif

#ifdef DCM_USE_SERVICE_RESPONSEONEVENT
    case SID_RESPONSE_ON_EVENT:
        DspResponseOnEvent(msgData.pduRxData, msgData.rxPduId, msgData.pduTxData);
        break;
#endif

#ifdef DCM_USE_SERVICE_READDATABYPERIODICIDENTIFIER
    case SID_READ_DATA_BY_PERIODIC_IDENTIFIER:
		DspReadDataByPeriodicIdentifier(msgData.pduRxData, msgData.pduTxData, msgData.rxPduId, msgData.txType, msgData.internalRequest);
        break;
#endif

#ifdef DCM_USE_SERVICE_DYNAMICALLYDEFINEDATAIDENTIFIER
    case SID_DYNAMICALLY_DEFINE_DATA_IDENTIFIER:
        DspDynamicallyDefineDataIdentifier(msgData.pduRxData, msgData.pduTxData);
        break;
#endif

#ifdef DCM_USE_SERVICE_INPUTOUTPUTCONTROLBYIDENTIFIER
    case SID_INPUT_OUTPUT_CONTROL_BY_IDENTIFIER:
        DspIOControlByDataIdentifier(msgData.pduRxData, msgData.pduTxData);
        break;
#endif

#ifdef DCM_USE_SERVICE_COMMUNICATIONCONTROL
    case SID_COMMUNICATION_CONTROL:
        DspCommunicationControl(msgData.pduRxData, msgData.pduTxData);
        break;
#endif
#ifdef DCM_USE_SERVICE_REQUESTDOWNLOAD
	case SID_REQUEST_DOWNLOAD:
		DspUdsRequestDownload(msgData.pduRxData, msgData.pduTxData);
		break;
#endif

#ifdef DCM_USE_SERVICE_REQUESTUPLOAD
	case SID_REQUEST_UPLOAD:
		DspUdsRequestUpload(msgData.pduRxData, msgData.pduTxData);
		break;
#endif

#ifdef DCM_USE_SERVICE_TRANSFERDATA
	case SID_TRANSFER_DATA:
		DspUdsTransferData(msgData.pduRxData, msgData.pduTxData);
		break;
#endif

#ifdef DCM_USE_SERVICE_REQUESTTRANSFEREXIT
	case SID_REQUEST_TRANSFER_EXIT:
		DspUdsRequestTransferExit(msgData.pduRxData, msgData.pduTxData);
		break;
#endif

    /* OBD */
#ifdef DCM_USE_SERVICE_REQUESTCURRENTPOWERTRAINDIAGNOSTICDATA
    case SID_REQUEST_CURRENT_POWERTRAIN_DIAGNOSTIC_DATA:
        DspObdRequestCurrentPowertrainDiagnosticData(msgData.pduRxData, msgData.pduTxData);
        break;
#endif

#if defined(USE_DEM) && defined(DCM_USE_SERVICE_REQUESTPOWERTRAINFREEZEFRAMEDATA)
    case SID_REQUEST_POWERTRAIN_FREEZE_FRAME_DATA:
        DspObdRequestPowertrainFreezeFrameData(msgData.pduRxData, msgData.pduTxData);
        break;
#endif

#if defined(USE_DEM) && defined(DCM_USE_SERVICE_CLEAREMISSIONRELATEDDIAGNOSTICDATA)
    case SID_CLEAR_EMISSION_RELATED_DIAGNOSTIC_INFORMATION:
        DspObdClearEmissionRelatedDiagnosticData(msgData.pduRxData, msgData.pduTxData);
        break;
#endif

#if defined(USE_DEM) && defined(DCM_USE_SERVICE_REQUESTEMISSIONRELATEDDIAGNOSTICTROUBLECODES)
    case SID_REQUEST_EMISSION_RELATED_DIAGNOSTIC_TROUBLE_CODES:
        DspObdRequestEmissionRelatedDiagnosticTroubleCodes(msgData.pduRxData, msgData.pduTxData);
        break;
#endif

#if defined(USE_DEM) && defined(DCM_USE_SERVICE_REQUESTEMISSIONRELATEDDTCSDETECTEDDURINGCURRENTORLASTCOMPLETEDDRIVINGCYCLE)
    case SID_REQUEST_EMISSION_RELATED_DIAGNOSTIC_TROUBLE_CODES_DETECTED_DURING_CURRENT_OR_LAST_COMPLETED_DRIVING_CYCLE:
        DspObdRequestEmissionRelatedDiagnosticTroubleCodesService07(msgData.pduRxData, msgData.pduTxData);
        break;
#endif

#ifdef DCM_USE_SERVICE_REQUESTVEHICLEINFORMATION
    case SID_REQUEST_VEHICLE_INFORMATION:
        DspObdRequestVehicleInformation(msgData.pduRxData, msgData.pduTxData);
        break;
#endif
    /* OBD */

    default:
        /* Non implemented service */
        createAndSendNcr(DCM_E_SERVICENOTSUPPORTED);
        break;
    }
}


static boolean lookupSid(uint8 sid, const Dcm_DsdServiceType **sidPtr)
{
    boolean returnStatus = FALSE;
    *sidPtr = NULL;
    const Dcm_DsdServiceType *service;

    if(NULL != msgData.serviceTable) {
        service = msgData.serviceTable->DsdService;
        while ((service->DsdSidTabServiceId != sid) && (!service->Arc_EOL)) {
            service++;
        }

        if (!service->Arc_EOL) {
            *sidPtr = service;
            returnStatus = TRUE;
        }
    }

    return returnStatus;
}


/*
 * Exported functions
 */

void DsdInit(void)
{

}


void DsdMain(void)
{
	if (dsdDslDataIndication) {
		dsdDslDataIndication = FALSE;
		DsdHandleRequest();
	}
}

boolean DsdLookupSubService(const Dcm_DsdServiceType *sidConfPtr, uint8 subService, const Dcm_DsdSubServiceType **subServicePtr, Dcm_NegativeResponseCodeType *respCode)
{
    boolean found = FALSE;
    *respCode = DCM_E_POSITIVERESPONSE;
    if( (NULL != sidConfPtr) && (sidConfPtr->DsdSidTabSubfuncAvail) && (NULL != sidConfPtr->DsdSubServiceList) ) {
        uint8 indx = 0;
        while( !sidConfPtr->DsdSubServiceList[indx].Arc_EOL ) {
            if( subService == sidConfPtr->DsdSubServiceList[indx].DsdSubServiceId) {
                *subServicePtr = &sidConfPtr->DsdSubServiceList[indx];
                /* IMPROVEMENT: Check if subservice is supported in the current session and security level */
                /*
                if(DspCheckSessionLevel(sidConfPtr->DsdSubServiceList[indx]->SessionRef))) {
                    if(!DspCheckSecurityLevel(sidConfPtr->DsdSubServiceList[indx]->SecurityRef)) {
                        *respCode = DCM_E_SECURITYACCESSDENIED;
                    }
                } else {
                    *respCode = DCM_E_SUBFUNCTIONNOTSUPPORTEDINACTIVESESSION;
                }
                */
                found = TRUE;
            }
            indx++;
        }
    }
    return found;
}

void DsdHandleRequest(void)
{
    Std_ReturnType result = E_OK;
    const Dcm_DsdServiceType *sidConfPtr = NULL;
    Dcm_NegativeResponseCodeType errorCode = DCM_E_POSITIVERESPONSE;

    if( msgData.pduRxData->SduLength > 0 ) {
        currentSid = msgData.pduRxData->SduDataPtr[0];	/** @req DCM198 */

#if (DCM_MANUFACTURER_NOTIFICATION == STD_ON)  /** @req DCM218 */
        Arc_DslGetRxConnectionParams(msgData.rxPduId, &Dcm_Dsd_sourceAddress, &Dcm_Dsd_requestType);
        result = ServiceIndication(currentSid, msgData.pduRxData->SduDataPtr, msgData.pduRxData->SduLength,
                               Dcm_Dsd_requestType, Dcm_Dsd_sourceAddress, &errorCode);
#endif
    }

    if (( 0 == msgData.pduRxData->SduLength) || (E_REQUEST_NOT_ACCEPTED == result)) {
        /* Special case with sdu length 0, No service id so we cannot send response. */
        /* @req DCM677 */ /* @req DCM462 */
        // Do not send any response     /** @req DCM462 */ /* @req DCM677*/
        DslDsdProcessingDone(msgData.rxPduId, DSD_TX_RESPONSE_SUPPRESSED);
    } else if (result != E_OK) {
        /* @req DCM678 */ /* @req DCM463 */
        createAndSendNcr(errorCode);   /** @req DCM463 */
    }
    /** @req DCM178 */
    //lint --e(506, 774)	PC-Lint exception Misra 13.7, 14.1 Allow configuration variables in boolean expression
    else if ((DCM_RESPOND_ALL_REQUEST == STD_ON) || ((currentSid & 0x7Fu) < 0x40)) {		/** @req DCM084 */
        if (lookupSid(currentSid, &sidConfPtr)) {		/** @req DCM192 */ /** @req DCM193 */ /** @req DCM196 */
            // SID found!
            if (DspCheckSessionLevel(sidConfPtr->DsdSidTabSessionLevelRef)) {		 /** @req DCM211 */
                if (DspCheckSecurityLevel(sidConfPtr->DsdSidTabSecurityLevelRef)) {	 /** @req DCM217 */
                    //lint --e(506, 774)	PC-Lint exception Misra 13.7, 14.1 Allow configuration variables in boolean expression
                    //lint --e(506, 774)	PC-Lint exception Misra 13.7, 14.1 Allow configuration variables in boolean expression
                    if ( (sidConfPtr->DsdSidTabSubfuncAvail) && (msgData.pduRxData->SduDataPtr[1] & SUPPRESS_POS_RESP_BIT) ) {	/** @req DCM204 */
                        suppressPosRspMsg = TRUE;	/** @req DCM202 */
                        msgData.pduRxData->SduDataPtr[1] &= ~SUPPRESS_POS_RESP_BIT;	/** @req DCM201 */
                    } else {
                        suppressPosRspMsg = FALSE;	/** @req DCM202 */
                    }
                    selectServiceFunction(sidConfPtr);
                }
                else {
                    createAndSendNcr(DCM_E_SECURITYACCESSDENIED);	/** @req DCM217 */
                }
            } else {
                createAndSendNcr(DCM_E_SERVICENOTSUPPORTEDINACTIVESESSION);	/** @req DCM211 */
            }
        } else {
            createAndSendNcr(DCM_E_SERVICENOTSUPPORTED);	/** @req DCM197 */
        }
    } else {
        // Inform DSL that message has been discard
        DslDsdProcessingDone(msgData.rxPduId, DSD_TX_RESPONSE_SUPPRESSED);
    }
}


void DsdDspProcessingDone(Dcm_NegativeResponseCodeType responseCode)
{
    PduIdType dummyId = 0;
    if (responseCode == DCM_E_POSITIVERESPONSE) {
        if (!suppressPosRspMsg) {	/** @req DCM200 */ /** @req DCM231 */
            /** @req DCM222 */
            if(!DslPduRPduUsedForType2Tx(msgData.pdurTxPduId, &dummyId)) {
                msgData.pduTxData->SduDataPtr[0] = currentSid | SID_RESPONSE_BIT;	/** @req DCM223 */ /** @req DCM224 */
            }
            DslDsdProcessingDone(msgData.rxPduId, DSD_TX_RESPONSE_READY);	/** @req DCM114 */ /** @req DCM225 */ /** @req DCM232.Ok */
        } else {
            DspDcmConfirmation(msgData.pdurTxPduId);	/** @req DCM236 */ /** @req DCM240 */
            DslDsdProcessingDone(msgData.rxPduId, DSD_TX_RESPONSE_SUPPRESSED);
        }
    } else {
        createAndSendNcr(responseCode);	/** @req DCM228 */
    }

}
#if (DCM_USE_JUMP_TO_BOOT == STD_ON)
/**
 * Gets the tester source address of the current request
 * @return Tester source address
 */
uint16 DsdDspGetTesterSourceAddress(void)
{
    uint16 testerSrcAddr;
    Dcm_ProtocolAddrTypeType reqType;
    Arc_DslGetRxConnectionParams(msgData.rxPduId, &testerSrcAddr, &reqType);
    return testerSrcAddr;
}

/**
 * Used to determine if a response is required to the current request
 * @return TRUE: Response required, FALSE: Response not required
 */
boolean DsdDspGetResponseRequired(void)
{
    return (FALSE == suppressPosRspMsg);
}
#endif

/**
 * Forces out a negative response 0x78 (requestCorrectlyReceivedResponsePending)
 */
void DsdDspForceResponsePending(void) {
    DslDsdSendResponsePending(msgData.rxPduId);
}

void DsdDspProcessingDone_ReadDataByPeriodicIdentifier(Dcm_NegativeResponseCodeType responseCode, boolean supressNRC) {
    if(supressNRC && (DCM_E_POSITIVERESPONSE != responseCode)) {
        DslDsdProcessingDone(msgData.rxPduId, DSD_TX_RESPONSE_SUPPRESSED);
    } else {
        DsdDspProcessingDone(responseCode);
    }
}

void DsdDataConfirmation(PduIdType confirmPduId, NotifResultType result)
{
	DspDcmConfirmation(confirmPduId);	/** @req DCM236 */

#if (DCM_MANUFACTURER_NOTIFICATION == STD_ON)  /** @req DCM742  */
	ServiceConfirmation(currentSid, Dcm_Dsd_requestType, Dcm_Dsd_sourceAddress, result);
#else
	(void)result;
#endif
}

#if (DCM_USE_JUMP_TO_BOOT == STD_ON)
/**
 * Forwards a tx confirmation of NRC 0x78 to Dsp
 * @param confirmPduId
 * @param result
 */
void DsdResponsePendingConfirmed(PduIdType confirmPduId, NotifResultType result)
{
    if(NTFRSLT_OK == result) {
        DspResponsePendingConfirmed(confirmPduId);
    }
}
#endif

void DsdDslDataIndication(const PduInfoType *pduRxData,
        const Dcm_DsdServiceTableType *protocolSIDTable,
        Dcm_ProtocolAddrTypeType addrType,
        PduIdType txPduId,
        PduInfoType *pduTxData,
        PduIdType rxContextPduId,
        Dcm_ProtocolTransTypeType txType,
        boolean internalRequest,
        boolean sendRespPendOnTransToBoot)
{
	msgData.rxPduId = rxContextPduId;
	msgData.pdurTxPduId = txPduId;
	msgData.pduRxData = pduRxData;
	msgData.pduTxData = pduTxData;
	msgData.addrType = addrType;
	msgData.serviceTable = protocolSIDTable;
	msgData.txType = txType;
	msgData.internalRequest = internalRequest;
	msgData.sendRespPendOnTransToBoot = sendRespPendOnTransToBoot;
	dsdDslDataIndication = TRUE;
}

//OBD: called by DSL to get the current Tx PduId
//IMPROVEMENT: Perhaps remove this?
PduIdType DsdDslGetCurrentTxPduId(void)
{
    return msgData.pdurTxPduId;
}

#if defined(DCM_USE_SERVICE_READDATABYPERIODICIDENTIFIER) || defined(DCM_USE_CONTROL_DIDS)
boolean DsdDspCheckServiceSupportedInActiveSessionAndSecurity(uint8 sid)
{
    boolean ret = FALSE;
    const Dcm_DsdServiceType *sidConfPtr = NULL;
    if( lookupSid(sid, &sidConfPtr) &&
        DspCheckSessionLevel(sidConfPtr->DsdSidTabSessionLevelRef) &&
        DspCheckSecurityLevel(sidConfPtr->DsdSidTabSecurityLevelRef) ) {
        /* Service is supported for the active protocol in the
         * current session and security level */
        ret = TRUE;

    }
    return ret;
}
#endif
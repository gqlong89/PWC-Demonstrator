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


#if !(((ECUM_SW_MAJOR_VERSION == 1) && (ECUM_SW_MINOR_VERSION == 0)) )
#error EcuM: Configuration file expected BSW module version to be 1.0.*
#endif


#ifndef _ECUM_GENERATED_TYPES_H_
#define _ECUM_GENERATED_TYPES_H_


#include "PreCompiledDataHash.h"

/* @req EcuM2992 */ /* @req EcuMf2992 */

#include "EcuM_Types.h"

#if defined(USE_MCU)
#include "Mcu.h"
#endif
#if defined(USE_PORT)
#include "Port.h"
#endif
#if defined(USE_CAN)
#include "Can.h"
#endif
#if defined(USE_DIO)
#include "Dio.h"
#endif
#if defined(USE_LIN)
#include "Lin.h"
#endif
#if defined(USE_CANIF)
#include "CanIf.h"
#endif
#if defined(USE_LINIF)
#include "LinIf.h"
#endif
#if defined(USE_PWM)
#include "Pwm.h"
#endif
#if defined (USE_OCU)
#include "Ocu.h"
#endif
#if defined (USE_ICU)
#include "Icu.h"
#endif
#if defined(USE_COM)
#include "Com.h"
#endif
#if defined(USE_PDUR)
#include "PduR.h"
#endif
#if defined(USE_DMA)
#include "Dma.h"
#endif
#if defined(USE_ADC)
#include "Adc.h"
#endif
#if defined(USE_GPT)
#include "Gpt.h"
#endif
#if defined(USE_COMM)
#include "ComM.h"
#endif
#if defined(USE_NM)
#include "Nm.h"
#endif
#if defined(USE_CANNM)
#include "CanNm.h"
#endif
#if defined(USE_CANSM)
#include "CanSM.h"
#endif
#if defined(USE_LINSM)
#include "LinSM.h"
#endif
#if defined(USE_J1939TP)
#include "J1939Tp.h"
#endif
#if defined(USE_UDPNM)
#include "UdpNm.h"
#endif
#if defined(USE_FLS)
#include "Fls.h"
#endif
#if defined(USE_EEP)
#include "Eep.h"
#endif
#if defined(USE_SPI)
#include "Spi.h"
#endif
#if defined(USE_WDG)
#include "Wdg.h"
#endif
#if defined(USE_WDGM)
#include "WdgM.h"
#endif
#if defined(USE_WDGIF)
#include "WdgIf.h"
#endif

#if defined(USE_BSWM)
#include "BswM.h"
#endif

#if defined(USE_PDUR) || defined(USE_COM) || defined(USE_CANIF) || defined(USE_CANTP)
#include "EcuM_PBTypes.h"
#endif

#if defined(USE_DCM)
#include "Dcm.h"
#endif
#if defined(USE_DEM)
#include "Dem.h"
#endif
#if defined(USE_XCP)
#include "Xcp.h"
#endif

typedef struct EcuM_ConfigS
{
    uint32 EcuMPostBuildVariant;            /* @req EcuM2794 */ /* @req EcuMf2794 */
    uint64 EcuMConfigConsistencyHashLow;    /* @req EcuM2795 Hash set when compiling the whole software for the */ /* @req EcuMf2795 */
    uint64 EcuMConfigConsistencyHashHigh;   /* PB module. It is compared to the PB hash at startup. */
	EcuM_StateType EcuMDefaultShutdownTarget;
	uint8 EcuMDefaultSleepMode;
	AppModeType EcuMDefaultAppMode;
	uint32 EcuMRunMinimumDuration;
	uint32 EcuMNvramReadAllTimeout;
	uint32 EcuMNvramWriteAllTimeout;
	Mcu_ModeType EcuMNormalMcuMode;
#if defined(USE_DEM)
	Dem_EventIdType EcuMDemInconsistencyEventId;
	Dem_EventIdType EcuMDemRamCheckFailedEventId;
	Dem_EventIdType EcuMDemAllRunRequestsKilledEventId;
#endif
    const EcuM_SleepModeType *EcuMSleepModeConfig;
    const EcuM_WakeupSourceConfigType *EcuMWakeupSourceConfig;
#if defined(USE_ECUM_FLEXIBLE)
    const EcuM_User *EcuMGoDownAllowedUsers;
#endif
#if defined (USE_COMM)
    const EcuM_ComMConfigType *EcuMComMConfig;
#endif   
#if defined(USE_MCU)
    const Mcu_ConfigType* McuConfig;
#endif
#if defined(USE_PORT)
    const Port_ConfigType* PortConfig;
#endif
#if defined(USE_CAN)
    const Can_ConfigType* CanConfig;
#endif
#if defined(USE_DIO)
    const Dio_ConfigType* DioCfg;
#endif
#if defined(USE_CANSM)
    const CanSM_ConfigType* CanSMConfig;
#endif
#if defined(USE_LIN)
    const Lin_ConfigType* LinConfig;
#endif
#if defined(USE_LINIF)
    const LinIf_ConfigType* LinIfConfig;
#endif
#if defined(USE_LINSM)
    const LinSM_ConfigType* LinSMConfig;
#endif
#if defined(USE_NM)
    const Nm_ConfigType* NmConfig;
#endif
#if defined(USE_UDPNM)
    const UdpNm_ConfigType* UdpNmConfig;
#endif
#if defined(USE_COMM)
    const ComM_ConfigType* ComMConfig;
#endif
#if defined(USE_BSWM)
    const BswM_ConfigType* BswMConfig;
#endif
#if defined(USE_J1939TP)
    const J1939Tp_ConfigType* J1939TpConfig;
#endif
#if defined(USE_PWM)
    const Pwm_ConfigType* PwmConfig;
#endif
#if defined(USE_OCU)
    const Ocu_ConfigType* OcuConfig;
#endif
#if defined(USE_ICU)
    const Icu_ConfigType* IcuConfig;
#endif
#if defined(USE_DMA)
    const Dma_ConfigType* DmaConfig;
#endif
#if defined(USE_ADC)
    const Adc_ConfigType* AdcConfig;
#endif
#if defined(USE_GPT)
    const Gpt_ConfigType* GptConfig;
#endif
#if defined(USE_FLS)
	const Fls_ConfigType* FlashConfig;
#endif
#if defined(USE_EEP)
	const Eep_ConfigType* EepConfig;
#endif
#if defined(USE_SPI)
	const Spi_ConfigType* SpiConfig;
#endif
#if defined(USE_WDG)
    const Wdg_ConfigType* WdgConfig;
#endif
#if defined(USE_WDGIF)
    const WdgIf_ConfigType* WdgIfConfig;
#endif
#if defined(USE_WDGM)
    const WdgM_ConfigType* WdgMConfig;
#endif
#if defined(USE_DCM)
	const Dcm_ConfigType* DcmConfig;
#endif
#if defined(USE_DEM)
	const Dem_ConfigType* DemConfig;
#endif
#if defined(USE_XCP)
	const Xcp_ConfigType* XcpConfig;
#endif
#if defined(USE_PDUR) || defined(USE_COM) || defined(USE_CANIF) || defined(USE_CANTP) || defined(USE_CANNM)
	const PostbuildConfigType* PostBuildConfig;
#endif
} EcuM_ConfigType;

#endif /*_ECUM_GENERATED_TYPES_H_*/

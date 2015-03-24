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


/* The post build area has the following structure.
 *
 * PostbuildConfig_Type - contains general data about the post
 * build data and contains pointers to the actual configurations
 * that are stored afterwards.
 *
 * ComM - ComM_Config
 * Com  - ComConfiguration
 * PduR  - PduR_Config
 * CanIf - CanIf_Config
 * CanTp Not supported yet
 * LinIf (N/A)
 * LinTp (N/A)
 * FrIf (N/A)
 * FrTp (N/A)
 *
 */


#include "EcuM_PBTypes.h"
#include "PreCompiledDataHash.h"
#include "MemMap.h"

#if defined(USE_CANTP)
#include "CanTp.h"
extern const CanTp_ConfigType CanTpConfig;
#endif

#if defined(USE_COM)
#include "Com.h"
extern const Com_ConfigType ComConfiguration;
#endif

#if defined(USE_PDUR)
#include "PduR.h"
extern const PduR_PBConfigType PduR_Config;
#endif

#if defined(USE_CANIF)
#include "CanIf.h"
extern const CanIf_ConfigType CanIf_Config;
#endif

#if defined(USE_CANNM)
#include "CanNm.h"
extern const CanNm_ConfigType CanNm_Config;
#endif


SECTION_POSTBUILD_HEADER const PostbuildConfigType Postbuild_Config = {
	.startPattern = 0x5A5A5A5A,
	.postBuildVariant = 1,
	.preCompileHashLow = PRE_COMPILED_DATA_HASH_LOW,
	.preCompileHashHigh = PRE_COMPILED_DATA_HASH_HIGH,
#if defined(USE_CANTP)
	.CanTp_ConfigPtr = &CanTpConfig,
#endif
#if defined(USE_COM)
	.ComConfigurationPtr = &ComConfiguration,
#endif
#if defined(USE_PDUR)
	.PduR_ConfigPtr = &PduR_Config,
#endif
#if defined(USE_CANIF)
	.CanIf_ConfigPtr = &CanIf_Config,
#endif
#if defined (USE_CANNM)
	.CanNm_ConfigPtr = &CanNm_Config,
#endif
};

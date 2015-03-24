/**
 * Application Header File
 *
 * @req SWS_Rte_01003
 */

/** === HEADER ====================================================================================
 */

/** --- C++ guard ---------------------------------------------------------------------------------
 * @req SWS_Rte_03709
 */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** --- Normal include guard ----------------------------------------------------------------------
 */
#ifndef RTE_MOTORDRIVERTYPE_H_
#define RTE_MOTORDRIVERTYPE_H_

/** --- Duplicate application include guard -------------------------------------------------------
 * @req SWS_Rte_01006
 */
#ifdef RTE_APPLICATION_HEADER_FILE
#error Multiple application header files included.
#endif
#define RTE_APPLICATION_HEADER_FILE

/** --- Single runnable API -----------------------------------------------------------------------
 * @req SWS_Rte_02751
 */
#if defined(RTE_RUNNABLEAPI_MotorDriverRunnable)
#define RTE_RUNNABLEAPI
#endif

/** --- Includes ----------------------------------------------------------------------------------
 * @req SWS_Rte_02751
 * @req SWS_Rte_07131
 */
#include <Rte_DataHandleType.h>
#include <Rte_MotorDriverType_Type.h>

/** --- Application Errors ------------------------------------------------------------------------
 * @req SWS_Rte_02575
 * @req SWS_Rte_02576
 * @req SWS_Rte_07143
 */

/** --- Initial Values ----------------------------------------------------------------------------
 * @SWS_Rte_05078
 */

/** --- PIM DATA TYPES ------------------------------------------------------------------------------ */

/** === BODY ======================================================================================
 */

/** @req SWS_Rte_03731
 *  @req SWS_Rte_07137
 *  @req SWS_Rte_07138
 *  !req SWS_Rte_06523
 *  @req SWS_Rte_03730
 *  @req SWS_Rte_07677
 *  @req SWS_Rte_02620
 *  @req SWS_Rte_02621
 *  @req SWS_Rte_01055
 *  @req SWS_Rte_03726 */

/** @req SWS_Rte_01343
 *  @req SWS_Rte_01342
 *  !req SWS_Rte_06524
 *  @req SWS_Rte_01053
 */

extern Std_ReturnType Rte_Call_MotorDriverType_MotorDriver_RunMotor_1_Write(/*IN*/DigitalLevel Level);
extern Std_ReturnType Rte_Call_MotorDriverType_MotorDriver_RunMotor_2_Write(/*IN*/DigitalLevel Level);

/** @req SWS_Rte_07132
 *  @req SWS_Rte_03714 
 *  @req SWS_Rte_03725 
 *	@req SWS_Rte_03752
 *	@req SWS_Rte_02623
 */
typedef struct {
    Rte_DE_commandType * const MotorDriverRunnable_cmd_command;
} Rte_CDS_MotorDriverType;

/** --- Instance handle type ---------------------------------------------------------------------- */
typedef Rte_CDS_MotorDriverType const * const Rte_Instance;

/** --- Singleton instance handle -----------------------------------------------------------------
 *  @req SWS_Rte_03793
 */
extern const Rte_Instance Rte_Inst_MotorDriverType;
#define self (Rte_Inst_MotorDriverType)

/** --- Calibration API --------------------------------------------------------------------------- */

/** --- Per Instance Memory API ------------------------------------------------------------------- */

/** --- Indirect port API ------------------------------------------------------------------------- */

/** --- Single Runnable APIs ---------------------------------------------------------------------- */
#if defined(RTE_RUNNABLEAPI)
/** --- MotorDriverRunnable */
#if defined(RTE_RUNNABLEAPI_MotorDriverRunnable)

void MotorDriverRunnable(void);

static inline commandType Rte_IRead_MotorDriverRunnable_cmd_command(void) {
    return self->MotorDriverRunnable_cmd_command->value;
}

static inline Std_ReturnType Rte_Call_RunMotor_1_Write(/*IN*/DigitalLevel Level) {
    return Rte_Call_MotorDriverType_MotorDriver_RunMotor_1_Write(Level);
}

static inline Std_ReturnType Rte_Call_RunMotor_2_Write(/*IN*/DigitalLevel Level) {
    return Rte_Call_MotorDriverType_MotorDriver_RunMotor_2_Write(Level);
}

#endif
#endif

/** --- All Runnable APIs ------------------------------------------------------------------------- */
#if !defined(RTE_RUNNABLEAPI)
void MotorDriverRunnable(void);

static inline commandType Rte_IRead_MotorDriverRunnable_cmd_command(void) {
    return self->MotorDriverRunnable_cmd_command->value;
}

static inline Std_ReturnType Rte_Call_RunMotor_1_Write(/*IN*/DigitalLevel Level) {
    return Rte_Call_MotorDriverType_MotorDriver_RunMotor_1_Write(Level);
}

static inline Std_ReturnType Rte_Call_RunMotor_2_Write(/*IN*/DigitalLevel Level) {
    return Rte_Call_MotorDriverType_MotorDriver_RunMotor_2_Write(Level);
}
#endif

/** === FOOTER ====================================================================================
 */

#endif /* RTE_MOTORDRIVERTYPE_H_ */

/** @req SWS_Rte_03710 */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

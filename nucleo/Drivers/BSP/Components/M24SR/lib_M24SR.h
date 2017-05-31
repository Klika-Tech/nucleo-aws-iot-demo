/**
  ******************************************************************************
  * @file    lib_M24SR.h
  * @author  MMY Application Team
  * @version V1.0.0
  * @date    20-November-2013
  * @brief   This file help to manage Data inside M24SR.
   ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MMY-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LIB_M24SR_H
#define __LIB_M24SR_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "m24sr.h"	 

/** @addtogroup M24SR_Driver
  * @{
  */


/** @addtogroup lib_M24SR
  * @{
  */
	 
/* Exported constants --------------------------------------------------------*/
	 
/** @defgroup lib_M24SR_Exported_Constants
  * @{
  */
	 
/** @defgroup lib_M24SR_M24SR_EEPROM_Size_Version
  * @{
  */	 
/* ---------------------- M24SR properties -----------------------------------*/
#define M24SR04_NDEF_MAX_SIZE								0x200
#define M24SR16_NDEF_MAX_SIZE								0x800
#define M24SR64_NDEF_MAX_SIZE								0x2000	 
/**
  * @}
  */

/** @defgroup lib_M24SR_Flag_to_select_open_session_command
  * @{
  */	 
#define ASK_FOR_SESSION											0x0000
#define TAKE_SESSION												0xFFFF	 
/**
  * @}
  */

/* M24SR buffer size is 0xF6 can be retrieve dynamicaly in CC file */
#define M24SR_READ_MAX_NBBYTE								0xF6 
#define M24SR_WRITE_MAX_NBBYTE							0xF6


#define NDEF_SESSION_CLOSED									0xDEADBEEF

/**
  * @}
  */


/* Exported macro ------------------------------------------------------------*/

/** @brief Check return statement of function
  * @param  fCall function result to check 
  * @retval M24SR_ACTION_COMPLETED or jump to dedicated Error management
  */ 
#ifndef errorchk
#define errorchk(fCall) if (status = (fCall), status != M24SR_ACTION_COMPLETED) \
	{goto Error;} else
#endif
	
/**
  * @}
  */

/**
  * @}
  */	

uint16_t M24SR_Initialization (uint8_t* pCCBuffer, uint8_t size );
	
uint16_t M24SR_GetNDEFFileId ( uint16_t *NDEF_fileID );
uint16_t M24SR_OpenNDEFSession ( uint16_t NDEF_fileID, uint16_t Priority );
uint16_t M24SR_ReadData ( uint16_t Offset , uint16_t DataSize , uint8_t* pData);
uint16_t M24SR_ForceReadData ( uint16_t Offset , uint16_t DataSize , uint8_t* pData);
uint16_t M24SR_WriteData ( uint16_t Offset , uint16_t DataSize , uint8_t* pData);
uint16_t M24SR_CloseNDEFSession ( uint16_t NDEF_fileID );	

uint16_t M24SR_EnableReadPassword( uint8_t* pCurrentWritePassword, uint8_t* pNewPassword);	
uint16_t M24SR_DisableReadPassword( uint8_t* pCurrentWritePassword );	
uint16_t M24SR_EnableWritePassword( uint8_t* pCurrentWritePassword, uint8_t* pNewPassword);	
uint16_t M24SR_DisableWritePassword( uint8_t* pCurrentWritePassword );
uint16_t M24SR_DisableAllPassword( uint8_t* pSuperUserPassword);

uint16_t M24SR_EnableReadOnly( uint8_t* pCurrentWritePassword);	
uint16_t M24SR_DisableReadOnly( uint8_t* pCurrentWritePassword);	
uint16_t M24SR_EnableWriteOnly( uint8_t* pCurrentWritePassword);	
uint16_t M24SR_DisableWriteOnly( uint8_t* pCurrentWritePassword);

uint16_t M24SR_ManageGPO( uc8 GPO_config, uc8 mode);	

#ifdef __cplusplus
}
#endif
	
	
#endif /* __LIB_M24SR_H */
 
/******************* (C) COPYRIGHT 2013 STMicroelectronics *****END OF FILE****/

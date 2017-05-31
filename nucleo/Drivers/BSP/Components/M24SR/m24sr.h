/**
  ******************************************************************************
  * @file    m24sr.h
  * @author  MMY Application Team
  * @version V1.2.0
  * @date    20-October-2014
  * @brief   This file provides a set of functions needed to manage M24SR
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
#ifndef __DRV_M24SR_H
#define __DRV_M24SR_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "drv_I2C_M24SR.h"

/** @addtogroup M24SR_Driver
  * @{
  */


/** @addtogroup drv_M24SR
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/**
  * @brief  APDU-Header command structure
  */
typedef struct
{
  uint8_t CLA;  /* Command class */
  uint8_t INS;  /* Operation code */
  uint8_t P1;   /* Selection Mode */
  uint8_t P2;   /* Selection Option */
} C_APDU_Header;


/**
  * @brief  APDU-Body command structure
  */
typedef struct 
{
  uint8_t LC;         						/* Data field length */	
  uint8_t *pData ;  						/* Command parameters */ 
  uint8_t LE;          						/* Expected length of data to be returned */
} C_APDU_Body;

/**
  * @brief  APDU Command structure 
  */
typedef struct
{
  C_APDU_Header Header;
  C_APDU_Body Body;
} C_APDU;

/**
  * @brief  SC response structure
  */
typedef struct
{
  uint8_t *pData ;  /* Data returned from the card */ // pointer on the transceiver buffer = ReaderRecBuf[CR95HF_DATA_OFFSET ];
  uint8_t SW1;         						 /* Command Processing status */
  uint8_t SW2;          						/* Command Processing qualification */
} R_APDU;

/**
  * @brief  GPO mode structure 
  */
typedef enum{
	RF_GPO= 0,
	I2C_GPO
}M24SR_GPO_MODE;

/**
  * @brief  GPO state structure 
  */
typedef enum{
	HIGH_IMPEDANCE= 0,
	SESSION_OPENED,
	WIP,
	I2C_ANSWER_READY,
	INTERRUPT,
	STATE_CONTROL
}M24SR_GPO_MGMT;

/* Exported constants --------------------------------------------------------*/

/** @defgroup lib_M24SR_Exported_Constants
  * @{
  */

/* ---------------------- status code ----------------------------------------*/
#define M24SR_ACTION_COMPLETED							0x9000
#define UB_STATUS_OFFSET										4
#define LB_STATUS_OFFSET  									3

#define M24SR_NBBYTE_INVALID								0xFFFE

/** @defgroup drv_M24SR_File_Identifier
  * @{
  */	 
#define SYSTEM_FILE_ID					0xE101	 
#define CC_FILE_ID							0xE103
#define NDEF_FILE_ID						0x0001	 
/**
  * @}
  */
	
/** @defgroup drv_M24SR_Password_Management
  * @{
  */
#define READ_PWD								0x0001
#define WRITE_PWD								0x0002
#define I2C_PWD									0x0003

/*-------------------------- Verify command answer ----------------------------*/
#define M24SR_PWD_NOT_NEEDED								0x9000
#define M24SR_PWD_NEEDED										0x6300
#define M24SR_PWD_CORRECT										0x9000
/**
  * @}
  */

	 
/** @defgroup drv_M24SR_Command_Management
  * @{
  */	
	
/* special M24SR command ----------------------------------------------------------------------*/	 
#define M24SR_OPENSESSION													0x26
#define M24SR_KILLSESSION													0x52

/* APDU Command: class list -------------------------------------------*/
#define C_APDU_CLA_DEFAULT	  0x00
#define C_APDU_CLA_ST					0xA2

/*------------------------ Data Area Management Commands ---------------------*/
#define C_APDU_SELECT_FILE     0xA4
#define C_APDU_GET_RESPONCE    0xC0
#define C_APDU_STATUS          0xF2
#define C_APDU_UPDATE_BINARY   0xD6
#define C_APDU_READ_BINARY     0xB0
#define C_APDU_WRITE_BINARY    0xD0
#define C_APDU_UPDATE_RECORD   0xDC
#define C_APDU_READ_RECORD     0xB2

/*-------------------------- Safety Management Commands ----------------------*/
#define C_APDU_VERIFY          0x20
#define C_APDU_CHANGE          0x24
#define C_APDU_DISABLE         0x26
#define C_APDU_ENABLE          0x28

/*-------------------------- Gpio Management Commands ------------------------*/
#define C_APDU_INTERRUPT       0xD6

/*  Length	----------------------------------------------------------------------------------*/
#define M24SR_STATUS_NBBYTE												2
#define M24SR_CRC_NBBYTE													2
#define M24SR_STATUSRESPONSE_NBBYTE								5
#define M24SR_DESELECTREQUEST_NBBYTE							3
#define M24SR_DESELECTRESPONSE_NBBYTE							3
#define M24SR_WATINGTIMEEXTRESPONSE_NBBYTE				4
#define M24SR_PASSWORD_NBBYTE											0x10

/*  Command structure	------------------------------------------------------------------------*/
#define M24SR_CMDSTRUCT_SELECTAPPLICATION					0x01FF
#define M24SR_CMDSTRUCT_SELECTCCFILE							0x017F
#define M24SR_CMDSTRUCT_SELECTNDEFFILE						0x017F
#define M24SR_CMDSTRUCT_READBINARY								0x019F
#define M24SR_CMDSTRUCT_UPDATEBINARY							0x017F
#define M24SR_CMDSTRUCT_VERIFYBINARYWOPWD					0x013F
#define M24SR_CMDSTRUCT_VERIFYBINARYWITHPWD				0x017F
#define M24SR_CMDSTRUCT_CHANGEREFDATA							0x017F
#define M24SR_CMDSTRUCT_ENABLEVERIFREQ						0x011F
#define M24SR_CMDSTRUCT_DISABLEVERIFREQ						0x011F
#define M24SR_CMDSTRUCT_SENDINTERRUPT							0x013F
#define M24SR_CMDSTRUCT_GPOSTATE									0x017F

/*  Command structure Mask -------------------------------------------------------------------*/
#define M24SR_PCB_NEEDED				0x0001		/* PCB byte present or not */
#define M24SR_CLA_NEEDED				0x0002 		/* CLA byte present or not */
#define M24SR_INS_NEEDED				0x0004 		/* Operation code present or not*/ 
#define M24SR_P1_NEEDED					0x0008		/* Selection Mode  present or not*/
#define M24SR_P2_NEEDED					0x0010		/* Selection Option present or not*/
#define M24SR_LC_NEEDED					0x0020		/* Data field length byte present or not */
#define M24SR_DATA_NEEDED				0x0040		/* Data present or not */
#define M24SR_LE_NEEDED					0x0080		/* Expected length present or not */
#define M24SR_CRC_NEEDED				0x0100		/* 2 CRC bytes present	or not */

#define M24SR_DID_NEEDED				0x08			/* DID byte present or not */

/**
  * @}
  */
	
	

/*  Offset	----------------------------------------------------------------------------------*/
#define M24SR_OFFSET_PCB													0
#define M24SR_OFFSET_CLASS												1
#define M24SR_OFFSET_INS													2
#define M24SR_OFFSET_P1														3


/*  mask	------------------------------------------------------------------------------------*/
#define M24SR_MASK_BLOCK													0xC0
#define M24SR_MASK_IBLOCK													0x00
#define M24SR_MASK_RBLOCK													0x80
#define M24SR_MASK_SBLOCK													0xC0

/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

/** @brief Get Most Significant Byte
  * @param  val: number where MSB must be extracted
  * @retval MSB
  */ 
#define GETMSB(val) 		( (uint8_t) ((val & 0xFF00 )>>8) ) 

/** @brief Get Least Significant Byte
  * @param  val: number where LSB must be extracted
  * @retval LSB
  */ 
#define GETLSB(val) 		( (uint8_t) (val & 0x00FF )) 

/** @brief Used to toggle the block number by adding 0 or 1 to default block number value
  * @param  val: number to know if incrementation is needed
  * @retval  0 or 1 if incrementation needed
  */
#define TOGGLE(val) 		((val != 0x00)? 0x00 : 0x01)


/*  public function	--------------------------------------------------------------------------*/

void   	 M24SR_Init														( void );
uint16_t M24SR_GetSession 										( void );
uint16_t M24SR_KillSession 										( void );
uint16_t M24SR_Deselect 											( void );
uint16_t M24SR_SelectApplication 							( void );
uint16_t M24SR_SelectCCfile 									( void );
uint16_t M24SR_SelectNDEFfile 								( uc16 NDEFfileId );
uint16_t M24SR_SelectSystemfile 							( void );
uint16_t M24SR_ReadBinary 										( uc16 Offset, uc8 NbByteToRead, uint8_t *pBufferRead );
uint16_t M24SR_STReadBinary			 							( uc16 Offset, uc8 NbByteToRead, uint8_t *pBufferRead );
uint16_t M24SR_UpdateBinary 									( uc16 Offset, uc8 NbByteToWrite, uc8 *pDataToWrite );
uint16_t M24SR_Verify 												( uc16 uPwdId, uc8 NbPwdByte, uc8 *pPwd );
uint16_t M24SR_ChangeReferenceData 						( uc16 uPwdId, uc8 *pPwd );
uint16_t M24SR_EnableVerificationRequirement 	( uc16 uReadOrWrite );
uint16_t M24SR_DisableVerificationRequirement ( uc16 uReadOrWrite );
uint16_t M24SR_EnablePermanentState   				( uc16 uReadOrWrite  );
uint16_t M24SR_DisablePermanentState 					( uc16 uReadOrWrite  );
uint16_t M24SR_SendInterrupt 									( void  );
uint16_t M24SR_StateControl 									( uc8 uSetOrReset );
uint16_t M24SR_ManageI2CGPO										( uc8 GPO_I2Cconfig);
uint16_t M24SR_ManageRFGPO										( uc8 GPO_RFconfig);
void 		 M24SR_RFConfig 											( uc8 OnOffChoice);

/**
  * @}
  */
	
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __DRV_M24SR_H */




/******************* (C) COPYRIGHT 2013 STMicroelectronics *****END OF FILE****/

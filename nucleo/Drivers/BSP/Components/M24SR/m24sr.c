/**
  ******************************************************************************
  * @file    m24sr.c
  * @author  MMY Application Team
  * @version V1.2.0
  * @date    20-October-2014
  * @brief   This file provides a set of functions to interface with the M24SR
  *          device.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "m24sr.h"

/** @addtogroup M24SR_Driver
  * @{
  *	@brief  <b>This folder contains the driver layer of M24SR family (M24SR64, M24SR16, M24SR04, M24SR02)</b> 
  */


/** @defgroup drv_M24SR
  * @{
	*	@brief  This file contains the driver which implements all the M24SR commands.
  */

static C_APDU	 						Command;
//static R_APDU 						Response;
static uint8_t					 	DataBuffer[0xFF];
uint8_t 									uM24SRbuffer [0xFF];
static uint8_t 						uDIDbyte =0x00;


static uint16_t M24SR_UpdateCrc							( uint8_t ch, uint16_t *lpwCrc);
static uint16_t M24SR_ComputeCrc						( uc8 *Data, uint8_t Length);
static uint16_t M24SR_IsCorrectCRC16Residue ( uc8 *DataIn,uc8 Length);
static void M24SR_InitStructure 						( void );
static void M24SR_BuildIBlockCommand 				( uc16 CommandStructure, C_APDU Command, uint16_t *NbByte , uint8_t *pCommand);
static int8_t IsSBlock 											(uc8 *pBuffer);
#if 0
static int8_t IsRBlock 											(uc8 *pBuffer);
static int8_t IsIBlock 											(uc8 *pBuffer);
#endif
static uint16_t M24SR_FWTExtension 					( uint8_t FWTbyte );

/** @defgroup drvM24SR_Private_Functions
  * @{
  */ 
	
	/**
  * @brief  This function updates the CRC 
  * @param  None
  * @retval None
  */
static uint16_t M24SR_UpdateCrc (uint8_t ch, uint16_t *lpwCrc)
{
  ch = (ch^(uint8_t)((*lpwCrc) & 0x00FF));
  ch = (ch^(ch<<4));
  *lpwCrc = (*lpwCrc >> 8)^((uint16_t)ch << 8)^((uint16_t)ch<<3)^((uint16_t)ch>>4);
	
  return(*lpwCrc);
}

/**
  * @brief  This function returns the CRC 16 
  * @param  Data : pointer on the data used to compute the CRC16
  * @param  Length : number of byte of the data
  * @retval CRC16 
  */
static uint16_t M24SR_ComputeCrc(uc8 *Data, uint8_t Length)
{
  uint8_t chBlock;
  uint16_t wCrc;
  
  wCrc = 0x6363; // ITU-V.41
  
  do {
    chBlock = *Data++;
    M24SR_UpdateCrc(chBlock, &wCrc);
  } while (--Length);
  
  return wCrc ;
}


/**  
* @brief  	This function computes the CRC16 residue as defined by CRC ISO/IEC 13239
* @param  	DataIn		:	input to data 
* @param		Length 		: 	Number of bits of DataIn
* @retval 	Status (SW1&SW2)  	:   CRC16 residue is correct	
* @retval 	M24SR_ERROR_CRC  	:  CRC16 residue is false
*/
static uint16_t M24SR_IsCorrectCRC16Residue (uc8 *DataIn,uc8 Length)
{
  uint16_t ResCRC=0;
  
	/* check the CRC16 Residue */
	if (Length !=0)
		ResCRC= M24SR_ComputeCrc (DataIn, Length);
	
	if ( ResCRC == 0x0000)
	{
		/* Good CRC, but error status from M24SR */
    return( ((DataIn[Length-UB_STATUS_OFFSET]<<8) & 0xFF00) | (DataIn[Length-LB_STATUS_OFFSET] & 0x00FF) ); 
	}
	else
	{
		ResCRC=0;
		ResCRC= M24SR_ComputeCrc (DataIn, 5);
		if ( ResCRC != 0x0000)
		{
			/* Bad CRC */
			return M24SR_ERROR_CRC;
		}
		else
		{
			/* Good CRC, but error status from M24SR */
			return( ((DataIn[1]<<8) & 0xFF00) | (DataIn[2] & 0x00FF) ); 
		}
	}	
}

/**
  * @brief  Initialize the command and response structure 
  * @param  None
  * @retval None
  */
static void M24SR_InitStructure ( void )
{
	/* build the command */
	Command.Header.CLA = 0x00;
	Command.Header.INS = 0x00;
	/* copy the offset */
	Command.Header.P1 = 0x00 ;
	Command.Header.P2 = 0x00 ;
	/* copy the number of byte of the data field */
	Command.Body.LC = 0x00 ;
	/* copy the number of byte to read */
	Command.Body.LE = 0x00 ;
	Command.Body.pData = DataBuffer; 
	
  // 	/* initializes the response structure*/
  // 	Response.pData = DataBuffer; 
  // 	Response.SW1 = 0x00;
  // 	Response.SW2 = 0x00;
}

/**
  * @brief 		This functions creates an I block command according to the structures CommandStructure and Command. 
	* @param	 	Command : structue which contains the field of the different parameter
	* @param	 	CommandStructure : structure that contain the structure of the command (if the different field are presnet or not 
	* @param	 	NbByte : number of byte of the command
	* @param	 	pCommand : pointer of the command created
  */
static void M24SR_BuildIBlockCommand ( uc16 CommandStructure, C_APDU Command, uint16_t *NbByte , uint8_t *pCommand)
{
	uint16_t	uCRC16; 
	static uint8_t BlockNumber = 0x01;
	
	(*NbByte) = 0;
	
	/* add the PCD byte */
	if ((CommandStructure & M24SR_PCB_NEEDED) !=0)
	{
		/* toggle the block number */
		BlockNumber = TOGGLE ( BlockNumber );
		/* Add the I block byte */
		pCommand[(*NbByte)++] = 0x02 |  BlockNumber; 
	}
	
	/* add the DID byte */
	if ((BlockNumber & M24SR_DID_NEEDED) !=0)
	{
		/* Add the I block byte */
		pCommand[(*NbByte)++] = uDIDbyte; 
	}
	
	/* add the Class byte */
	if ((CommandStructure & M24SR_CLA_NEEDED) !=0)
	{
		pCommand[(*NbByte)++] = Command.Header.CLA ;
	}
	/* add the instruction byte byte */
	if ( (CommandStructure & M24SR_INS_NEEDED) !=0)
	{
		pCommand[(*NbByte)++] = Command.Header.INS ;
	}
	/* add the Selection Mode byte */
	if ((CommandStructure & M24SR_P1_NEEDED) !=0)
	{
		pCommand[(*NbByte)++] = Command.Header.P1 ;
	}
	/* add the Selection Mode byte */
	if ((CommandStructure & M24SR_P2_NEEDED) !=0)
	{
		pCommand[(*NbByte)++] = Command.Header.P2 ;
	}
	/* add Data field lengthbyte */
	if ((CommandStructure & M24SR_LC_NEEDED) !=0)
	{
		pCommand[(*NbByte)++] = Command.Body.LC ;
	}
	/* add Data field  */
	if ((CommandStructure & M24SR_DATA_NEEDED) !=0)
	{
		memcpy(&(pCommand[(*NbByte)]) ,Command.Body.pData,Command.Body.LC ) ;
		(*NbByte) += Command.Body.LC ;
	}
	/* add Le field  */
	if ((CommandStructure & M24SR_LE_NEEDED) !=0)
	{
		pCommand[(*NbByte)++] = Command.Body.LE ;
	}
	/* add CRC field  */
	if ((CommandStructure & M24SR_CRC_NEEDED) !=0)
	{
		uCRC16 = M24SR_ComputeCrc (pCommand,(uint8_t) (*NbByte));
		/* append the CRC16 */
		pCommand [(*NbByte)++] = GETLSB	(uCRC16 ) ;
		pCommand [(*NbByte)++] = GETMSB	(uCRC16 ) ;	
	}
  
}

#if 0
/**  
* @brief  	This function return M24SR_STATUS_SUCCESS if the pBuffer is an I-block
* @param  	pBuffer		:	pointer of the data
* @retval 	M24SR_STATUS_SUCCESS  :  the data is a I-Block
* @retval 	M24SR_ERROR_DEFAULT  	:  the data is not a I-Block
*/
static int8_t IsIBlock (uc8 *pBuffer)
{
  
	if ((pBuffer[M24SR_OFFSET_PCB] & M24SR_MASK_BLOCK) == M24SR_MASK_IBLOCK)
	{
		return M24SR_STATUS_SUCCESS;
	}
	else 
	{	
		return M24SR_ERROR_DEFAULT;
	}
	
}

/**  
* @brief  	This function return M24SR_STATUS_SUCCESS if the pBuffer is an R-block
* @param  	pBuffer		:	pointer of the data
* @retval 	M24SR_STATUS_SUCCESS  :  the data is a R-Block
* @retval 	M24SR_ERROR_DEFAULT  	:  the data is not a R-Block
*/
static int8_t IsRBlock (uc8 *pBuffer)
{
  
	if ((pBuffer[M24SR_OFFSET_PCB] & M24SR_MASK_BLOCK) == M24SR_MASK_RBLOCK)
	{
		return M24SR_STATUS_SUCCESS;
	}
	else 
	{	
		return M24SR_ERROR_DEFAULT;
	}
	
}
#endif	
	
/**  
* @brief  	This function return M24SR_STATUS_SUCCESS if the pBuffer is an s-block
* @param  	pBuffer		:	pointer of the data
* @retval 	M24SR_STATUS_SUCCESS  :  the data is a S-Block
* @retval 	M24SR_ERROR_DEFAULT  	:  the data is not a S-Block
*/
static int8_t IsSBlock (uc8 *pBuffer)
{
  
	if ((pBuffer[M24SR_OFFSET_PCB] & M24SR_MASK_BLOCK) == M24SR_MASK_SBLOCK)
	{
		return M24SR_STATUS_SUCCESS;
	}
	else 
	{	
		return M24SR_ERROR_DEFAULT;
	}
	
}

/**
  * @brief  This function sends the FWT extension command (S-Block format)
	* @param	FWTbyte : FWT value
  * @retval Status (SW1&SW2) : Status of the operation to complete.
	* @retval M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
  */
static uint16_t M24SR_FWTExtension ( uint8_t FWTbyte )
{
  uint8_t 	pBuffer[M24SR_STATUSRESPONSE_NBBYTE];
  uint16_t		status ; 
  uint16_t	NthByte = 0,
  uCRC16;
  
	/* create the response */
	pBuffer[NthByte++] = 0xF2 ;	
	pBuffer[NthByte++] = FWTbyte ;
	/* compute the CRC */
	uCRC16 = M24SR_ComputeCrc (pBuffer,0x02);
	/* append the CRC16 */
	pBuffer [NthByte++] = GETLSB	(uCRC16 ) ;
	pBuffer [NthByte++]= 	GETMSB	(uCRC16 ) ;	
	
	/* send the request */ 
	errchk( M24SR_SendI2Ccommand ( NthByte , pBuffer ));
	errchk( M24SR_IsAnswerReady ( ));
	/* read the response */ 
	errchk( M24SR_ReceiveI2Cresponse ( M24SR_STATUSRESPONSE_NBBYTE , pBuffer ));
	
	status = M24SR_IsCorrectCRC16Residue (pBuffer, M24SR_STATUSRESPONSE_NBBYTE); 
	return status;
	
Error :
	return M24SR_ERROR_I2CTIMEOUT;
}

/**
  * @}
  */


/** @defgroup drvM24SR_Public_Functions
  * @{
  */ 

/**
  * @brief  This function initialize the M24SR device
  * @retval None 
  */
void M24SR_Init( void )
{		
	M24SR_I2CInit();
	M24SR_GPOInit();
	
	M24SR_InitStructure();
  
#if defined (I2C_GPO_SYNCHRO_ALLOWED) || defined (I2C_GPO_INTERRUPT_ALLOWED)
	if( M24SR_KillSession() == M24SR_ACTION_COMPLETED)
	{
    M24SR_ManageI2CGPO(I2C_ANSWER_READY);
	  M24SR_Deselect ();
	} 
#endif /* I2C_GPO_SYNCHRO_ALLOWED */
}

/**
  * @brief  This function sends the GetSession command to the M24SR device
	* @retval M24SR_ACTION_COMPLETED : the function is succesful.
  * @retval Status (SW1&SW2) : if operation does not complete.
  */
uint16_t M24SR_GetSession ( void )
{
	uint8_t Buffer = M24SR_OPENSESSION;
	int16_t 	status;
  
	errchk(M24SR_SendI2Ccommand ( 0x01 , &Buffer ));
	
	/* Insure no access will be done just after open session */	
	/* The only way here is to poll I2C to know when M24SR is ready */
	/* GPO can not be use with GetSession command */
	errchk(M24SR_PollI2C ( )); 
  
	return M24SR_ACTION_COMPLETED;
Error :
	return M24SR_ERROR_I2CTIMEOUT;	
}



/**
  * @brief  This function sends the KillSession command to the M24SR device
  * @param  None
	* @retval M24SR_ACTION_COMPLETED : the function is succesful. 
	* @retval M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_KillSession ( void )
{
	uint8_t pBuffer[] = {M24SR_KILLSESSION};
	int8_t 	status;
  
	errchk(M24SR_SendI2Ccommand ( 0x01 , pBuffer ));
  
	/* Insure no access will be done just after open session */	
	/* The only way here is to poll I2C to know when M24SR is ready */
	/* GPO can not be use with KillSession command */
	errchk(M24SR_PollI2C ( )); 	
	
	return M24SR_ACTION_COMPLETED;
Error :
	return M24SR_ERROR_I2CTIMEOUT;
}


/**
  * @brief  This function sends the Deselect command (S-Block format)
	* @retval M24SR_ACTION_COMPLETED : the function is succesful. 
	* @retval M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured. 
  */
uint16_t M24SR_Deselect ( void )
{
  uint8_t 	pBuffer[] = {0xC2,0xE0,0xB4} ;
	int8_t		status ; 
	
	/* send the request */ 
	errchk( M24SR_SendI2Ccommand ( M24SR_DESELECTREQUEST_NBBYTE , pBuffer ));
	
	errchk( M24SR_IsAnswerReady ( ));	
	/* flush the M24SR buffer */ 
	errchk( M24SR_ReceiveI2Cresponse ( M24SR_DESELECTREQUEST_NBBYTE , pBuffer ));
	
	return M24SR_ACTION_COMPLETED;
  
Error :
	return M24SR_ERROR_I2CTIMEOUT;
}




/**
  * @brief  This function sends the SelectApplication command
	* @retval M24SR_ACTION_COMPLETED : the function is succesful. 
	* @retval M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_SelectApplication ( void )
{
  uint8_t 	*pBuffer = uM24SRbuffer ,
  NbByteToRead = M24SR_STATUSRESPONSE_NBBYTE;
  uint8_t		uLc = 0x07,
  pData[] = {0xD2,0x76,0x00,0x00,0x85,0x01,0x01},
  uLe = 0x00;
  uint16_t	status ; 
  uint16_t	uP1P2 =0x0400,
  NbByte;
	
	/* build the command */
	Command.Header.CLA = C_APDU_CLA_DEFAULT;
	Command.Header.INS = C_APDU_SELECT_FILE;
	/* copy the offset */
	Command.Header.P1 = GETMSB	(uP1P2 ) ;
	Command.Header.P2 = GETLSB	(uP1P2 ) ;
	/* copy the number of byte of the data field */
	Command.Body.LC = uLc ;
	/* copy the data */
	memcpy(Command.Body.pData, pData, uLc);
	/* copy the number of byte to read */
	Command.Body.LE = uLe ;
	/* build the I²C command */
	M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_SELECTAPPLICATION,  Command, &NbByte , pBuffer);
	
	/* send the request */ 
	errchk( M24SR_SendI2Ccommand ( NbByte , pBuffer ));
	errchk( M24SR_IsAnswerReady ( ));	
	/* read the response */ 
	errchk( M24SR_ReceiveI2Cresponse ( NbByteToRead , pBuffer ));
	
	status = M24SR_IsCorrectCRC16Residue (pBuffer,NbByteToRead); 
	return status;
	
Error :
	return M24SR_ERROR_I2CTIMEOUT;
}

/**
  * @brief  This function sends the SelectCCFile command
	* @retval M24SR_ACTION_COMPLETED : the function is succesful. 
	* @retval M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
  * @retval Status (SW1&SW2) : if operation does not complete for another reason.
  */
uint16_t M24SR_SelectCCfile ( void )
{
  uint8_t 	*pBuffer = uM24SRbuffer ,
  NbByteToRead = M24SR_STATUSRESPONSE_NBBYTE;
  uint8_t		uLc = 0x02;
  uint16_t	status ; 
  uint16_t	uP1P2 =0x000C,
  uNbFileId =CC_FILE_ID,
  NbByte;
	
	/* build the command */
	Command.Header.CLA = C_APDU_CLA_DEFAULT;
	Command.Header.INS = C_APDU_SELECT_FILE;
	/* copy the offset */
	Command.Header.P1 = GETMSB	(uP1P2 ) ;
	Command.Header.P2 = GETLSB	(uP1P2 ) ;
	/* copy the number of byte of the data field */
	Command.Body.LC = uLc ;
	/* copy the File Id */
	Command.Body.pData[0] = GETMSB	(uNbFileId ) ;
	Command.Body.pData[1] = GETLSB	(uNbFileId ) ;
	/* build the I²C command */
	M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_SELECTCCFILE,  Command, &NbByte , pBuffer);
	
	/* send the request */ 
	errchk( M24SR_SendI2Ccommand ( NbByte , pBuffer ));
	errchk( M24SR_IsAnswerReady ( ));
	/* read the response */ 
	errchk( M24SR_ReceiveI2Cresponse ( NbByteToRead , pBuffer ));
	
	status = M24SR_IsCorrectCRC16Residue (pBuffer,NbByteToRead); 
	return status; 
	
Error :
	return M24SR_ERROR_I2CTIMEOUT;
}


/**
  * @brief  This function sends the SelectSystemFile command
  * @retval Status (SW1&SW2) : Status of the operation to complete.
	* @retval M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_SelectSystemfile ( void )
{
  uint8_t 	*pBuffer = uM24SRbuffer ,
  NbByteToRead = M24SR_STATUSRESPONSE_NBBYTE;
  uint8_t		uLc = 0x02;
  uint16_t	status ; 
  uint16_t	uP1P2 =0x000C,
  uNbFileId =SYSTEM_FILE_ID,
  NbByte;
	
	/* build the command */
	Command.Header.CLA = C_APDU_CLA_DEFAULT;
	Command.Header.INS = C_APDU_SELECT_FILE;
	/* copy the offset */
	Command.Header.P1 = GETMSB	(uP1P2 ) ;
	Command.Header.P2 = GETLSB	(uP1P2 ) ;
	/* copy the number of byte of the data field */
	Command.Body.LC = uLc ;
	/* copy the File Id */
	Command.Body.pData[0] = GETMSB	(uNbFileId ) ;
	Command.Body.pData[1] = GETLSB	(uNbFileId ) ;
	/* build the I²C command */
	M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_SELECTCCFILE,  Command, &NbByte , pBuffer);
	
	/* send the request */ 
	errchk( M24SR_SendI2Ccommand ( NbByte , pBuffer ));
	errchk( M24SR_IsAnswerReady ( ));
	/* read the response */ 
	errchk( M24SR_ReceiveI2Cresponse ( NbByteToRead , pBuffer ));
	
	status = M24SR_IsCorrectCRC16Residue (pBuffer,NbByteToRead); 
	return status;
  
Error :
	return M24SR_ERROR_I2CTIMEOUT;
}

/**
  * @brief  This function sends the SelectNDEFfile command
  * @retval Status (SW1&SW2) : Status of the operation to complete.
	* @retval M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_SelectNDEFfile ( uc16 NDEFfileId )
{
  uint8_t 	*pBuffer = uM24SRbuffer ,
  NbByteToRead = M24SR_STATUSRESPONSE_NBBYTE;
  uint8_t		uLc = 0x02;
  uint16_t	status ; 
  uint16_t	uP1P2 =0x000C,
  NbByte;
	
	/* build the command */
	Command.Header.CLA = C_APDU_CLA_DEFAULT;
	Command.Header.INS = C_APDU_SELECT_FILE;
	/* copy the offset */
	Command.Header.P1 = GETMSB	(uP1P2 ) ;
	Command.Header.P2 = GETLSB	(uP1P2 ) ;
	/* copy the number of byte of the data field */
	Command.Body.LC = uLc ;
	/* copy the offset */
	Command.Body.pData[0] = GETMSB	(NDEFfileId ) ;
	Command.Body.pData[1] = GETLSB	(NDEFfileId ) ;
	/* build the I²C command */
	M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_SELECTNDEFFILE,  Command, &NbByte , pBuffer);
	
	/* send the request */ 
	errchk( M24SR_SendI2Ccommand ( NbByte , pBuffer ));
	errchk( M24SR_IsAnswerReady ( ));
	/* read the response */ 
	errchk( M24SR_ReceiveI2Cresponse ( NbByteToRead , pBuffer ));
	
	status = M24SR_IsCorrectCRC16Residue (pBuffer,NbByteToRead); 
	return status;
	
Error :
	return M24SR_ERROR_I2CTIMEOUT;
	
}

/**
  * @brief  This function sends a read binary command
	* @param	Offset : first byte to read
	* @param	NbByteToRead : number of byte to read
	* @param	pBufferRead : pointer of the buffer read from the M24SR device
  * @retval Status (SW1&SW2) : Status of the operation to complete.
	* @retval M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_ReadBinary ( uc16 Offset ,uc8 NbByteToRead , uint8_t *pBufferRead )
{
	uint8_t 	*pBuffer = uM24SRbuffer ;
	uint16_t	status ; 
	uint16_t	NbByte;
	
	/* build the command */
	Command.Header.CLA = C_APDU_CLA_DEFAULT;
	Command.Header.INS = C_APDU_READ_BINARY;
	/* copy the offset */
	Command.Header.P1 = GETMSB	(Offset ) ;
	Command.Header.P2 = GETLSB	(Offset ) ;
	/* copy the number of byte to read */
	Command.Body.LE = NbByteToRead ;
  
	M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_READBINARY,  Command, &NbByte , pBuffer);
  
	errchk( M24SR_SendI2Ccommand ( NbByte , pBuffer ));
	errchk( M24SR_IsAnswerReady ( ));
	errchk( M24SR_ReceiveI2Cresponse ( NbByteToRead + M24SR_STATUSRESPONSE_NBBYTE , pBuffer ));
	
	status = M24SR_IsCorrectCRC16Residue (pBuffer,NbByteToRead+ M24SR_STATUSRESPONSE_NBBYTE); 
	/* retrieve the data without SW1 & SW2 as provided as return value of the function */
	memcpy(pBufferRead ,&pBuffer[1],NbByteToRead);
	return status;
	
Error :
	return M24SR_ERROR_I2CTIMEOUT;
	
}



/**
  * @brief  This function sends a ST read binary command (no error if access is not inside NDEF file)
	* @param	Offset : first byte to read
	* @param	NbByteToRead : number of byte to read
	* @param	pBufferRead : pointer of the buffer read from the M24SR device
  * @retval Status (SW1&SW2) : Status of the operation to complete.
	* @retval M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured. 
  */
uint16_t M24SR_STReadBinary ( uc16 Offset, uc8 NbByteToRead, uint8_t *pBufferRead )
{
  uint8_t 	*pBuffer = uM24SRbuffer ;
  uint16_t	status ; 
  uint16_t	NbByte;
	
	/* build the command */
	Command.Header.CLA = C_APDU_CLA_ST;
	Command.Header.INS = C_APDU_READ_BINARY;
	/* copy the offset */
	Command.Header.P1 = GETMSB	(Offset ) ;
	Command.Header.P2 = GETLSB	(Offset ) ;
	/* copy the number of byte to read */
	Command.Body.LE = NbByteToRead ;
  
	M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_READBINARY,  Command, &NbByte , pBuffer);
  
	errchk( M24SR_SendI2Ccommand ( NbByte , pBuffer ));
	errchk( M24SR_IsAnswerReady ( ));
	errchk( M24SR_ReceiveI2Cresponse ( NbByteToRead + M24SR_STATUSRESPONSE_NBBYTE , pBuffer ));
	
	status = M24SR_IsCorrectCRC16Residue (pBuffer,NbByteToRead+ M24SR_STATUSRESPONSE_NBBYTE); 
	/* retrieve the data without SW1 & SW2 as provided as return value of the function */
	memcpy(pBufferRead ,&pBuffer[1],NbByteToRead);
	return status;
	
Error :
	return M24SR_ERROR_I2CTIMEOUT;
	
}

/**
  * @brief  This function sends a Update binary command
	* @param	Offset : first byte to read
	* @param	NbByteToWrite : number of byte to write
	* @param	pBufferRead : pointer of the buffer read from the M24SR device
  * @retval Status (SW1&SW2) : Status of the operation to complete.
	* @retval M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_UpdateBinary ( uc16 Offset ,uc8 NbByteToWrite,uc8 *pDataToWrite )
{
  uint8_t 	*pBuffer = uM24SRbuffer ;
  uint16_t	status ; 
  uint16_t	NbByte;
	
	/* build the command */
	Command.Header.CLA = C_APDU_CLA_DEFAULT;
	Command.Header.INS = C_APDU_UPDATE_BINARY;
	/* copy the offset */
	Command.Header.P1 = GETMSB	(Offset ) ;
	Command.Header.P2 = GETLSB	(Offset ) ;
	/* copy the number of byte of the data field */
	Command.Body.LC = NbByteToWrite ;
	/* copy the File Id */
	memcpy(Command.Body.pData ,pDataToWrite, NbByteToWrite );
  
	M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_UPDATEBINARY,  Command, &NbByte , pBuffer);
  
	errchk( M24SR_SendI2Ccommand ( NbByte , pBuffer ));
	errchk( M24SR_IsAnswerReady ( ));
	errchk( M24SR_ReceiveI2Cresponse ( M24SR_STATUSRESPONSE_NBBYTE , pBuffer ));
	/* if the response is a Watiting frame extenstion request */ 
	if (IsSBlock (pBuffer) == M24SR_STATUS_SUCCESS)
	{
		/*check the CRC */ 
		if (M24SR_IsCorrectCRC16Residue (pBuffer , M24SR_WATINGTIMEEXTRESPONSE_NBBYTE) != M24SR_ERROR_CRC)
		{
			/* send the FrameExension response*/ 
			status = M24SR_FWTExtension (  pBuffer [M24SR_OFFSET_PCB+1] );
		}
	}
	else
	{	
		status = M24SR_IsCorrectCRC16Residue (pBuffer, M24SR_STATUSRESPONSE_NBBYTE); 
	}
	
	return status;
  
	
Error :
	return M24SR_ERROR_I2CTIMEOUT;
}


/**
  * @brief  This function sends the Verify command
	* @param	uPwdId : PasswordId ( 0x0001 : Read NDEF pwd or 0x0002 : Write NDEF pwd or 0x0003 : I2C pwd)
	* @param	NbPwdByte : Number of byte ( 0x00 or 0x10)
	* @param	pPwd : pointer on the passwaord
  * @retval Status (SW1&SW2) : Status of the operation to complete.
	* @retval M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_Verify ( uc16 uPwdId,uc8 NbPwdByte ,uc8 *pPwd )
{
  uint8_t 	*pBuffer = uM24SRbuffer ;
  uint16_t	status = 0x0000 ; 
  uint16_t	NbByte;
  
	/*check the parameters */
	if (uPwdId > 0x0003)
	{	
		return M24SR_ERROR_PARAMETER;
	}
	if ( (NbPwdByte != 0x00) && (NbPwdByte != 0x10))
	{	
		return M24SR_ERROR_PARAMETER;
	}
  
	/* build the command */
	Command.Header.CLA = C_APDU_CLA_DEFAULT;
	Command.Header.INS = C_APDU_VERIFY;
	/* copy the Password Id */
	Command.Header.P1 = GETMSB	(uPwdId ) ;
	Command.Header.P2 = GETLSB	(uPwdId ) ;
	/* copy the number of byte of the data field */
	Command.Body.LC = NbPwdByte ;
  
	if (NbPwdByte == 0x10) 
	{
		/* copy the password */
		memcpy(Command.Body.pData, pPwd, NbPwdByte);
		/* build the I²C command */
		M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_VERIFYBINARYWITHPWD,  Command, &NbByte , pBuffer);
	}
	else 
	{
		/* build the I²C command */
		M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_VERIFYBINARYWOPWD,  Command, &NbByte , pBuffer);
	}
	
	/* send the request */ 
	errchk( M24SR_SendI2Ccommand ( NbByte , pBuffer ));
	/* wait for answer ready */
	errchk( M24SR_IsAnswerReady ( ));
	/* read the response */ 
	errchk( M24SR_ReceiveI2Cresponse ( M24SR_STATUSRESPONSE_NBBYTE , pBuffer ));
	
	status = M24SR_IsCorrectCRC16Residue (pBuffer, M24SR_STATUSRESPONSE_NBBYTE); 
	return status;
	
Error :
	return M24SR_ERROR_I2CTIMEOUT;
}


/**
  * @brief  This function sends the ChangeReferenceData command
	* @param	uPwdId : PasswordId ( 0x0001 : Read NDEF pwd or 0x0002 : Write NDEF pwd or 0x0003 : I2C pwd)
	* @param	pPwd : pointer on the passwaord
  * @retval Status (SW1&SW2) : Status of the operation to complete.
	* @retval M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_ChangeReferenceData ( uc16 uPwdId,uc8 *pPwd )
{
  uint8_t 	*pBuffer = uM24SRbuffer;
  uint16_t	status ; 
  uint16_t	NbByte;
  
	/*check the parameters */
	if (uPwdId > 0x0003)
	{	
		return M24SR_ERROR_PARAMETER;
	}
  
	/* build the command */
	Command.Header.CLA = C_APDU_CLA_DEFAULT;
	Command.Header.INS = C_APDU_CHANGE;
	/* copy the Password Id */
	Command.Header.P1 = GETMSB	(uPwdId ) ;
	Command.Header.P2 = GETLSB	(uPwdId ) ;
	/* copy the number of byte of the data field */
	Command.Body.LC = M24SR_PASSWORD_NBBYTE ;
	/* copy the password */
	memcpy(Command.Body.pData, pPwd, M24SR_PASSWORD_NBBYTE);
	/* build the I²C command */
	M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_CHANGEREFDATA,  Command, &NbByte , pBuffer);
  
	
	/* send the request */ 
	errchk( M24SR_SendI2Ccommand ( NbByte , pBuffer ));
	errchk( M24SR_IsAnswerReady ( ));
	/* read the response */ 
	errchk( M24SR_ReceiveI2Cresponse ( M24SR_STATUSRESPONSE_NBBYTE , pBuffer ));
	
	status = M24SR_IsCorrectCRC16Residue (pBuffer, M24SR_STATUSRESPONSE_NBBYTE); 
	return status;
  
Error :
	return M24SR_ERROR_I2CTIMEOUT;
}


/**
  * @brief  This function sends the EnableVerificationRequirement command
	* @param	uReadOrWrite : enable the read or write protection ( 0x0001 : Read or 0x0002 : Write  )
  * @retval Status (SW1&SW2) : Status of the operation to complete.
	* @retval M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_EnableVerificationRequirement ( uc16 uReadOrWrite  )
{
  uint8_t 	*pBuffer = uM24SRbuffer;
  uint16_t	status ; 
  uint16_t	NbByte;
  
	/*check the parameters */
	if ( (uReadOrWrite != 0x0001) && (uReadOrWrite != 0x0002))
	{	
		return M24SR_ERROR_PARAMETER;
	}
  
	/* build the command */
	Command.Header.CLA = C_APDU_CLA_DEFAULT;
	Command.Header.INS = C_APDU_ENABLE;
	/* copy the Password Id */
	Command.Header.P1 = GETMSB	(uReadOrWrite ) ;
	Command.Header.P2 = GETLSB	(uReadOrWrite ) ;
	/* build the I²C command */
	M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_ENABLEVERIFREQ,  Command, &NbByte , pBuffer);
  
	/* send the request */ 
	errchk( M24SR_SendI2Ccommand ( NbByte , pBuffer ));
	/* The right access to be updated in EEPROM need at least 6ms */	
	errchk( M24SR_IsAnswerReady ( ));
	/* read the response */ 
	errchk( M24SR_ReceiveI2Cresponse ( M24SR_STATUSRESPONSE_NBBYTE , pBuffer ));
  
	status = M24SR_IsCorrectCRC16Residue (pBuffer, M24SR_STATUSRESPONSE_NBBYTE); 
	return status;
	
Error :
	return M24SR_ERROR_I2CTIMEOUT;
}

/**
  * @brief  This function sends the DisableVerificationRequirement command
	* @param	uReadOrWrite : enable the read or write protection ( 0x0001 : Read or 0x0002 : Write  )
  * @retval Status (SW1&SW2) : Status of the operation to complete.
	* @retval M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_DisableVerificationRequirement ( uc16 uReadOrWrite  )
{
  uint8_t 	*pBuffer = uM24SRbuffer;
  uint16_t	status ; 
  uint16_t	NbByte;
  
	/*check the parameters */
	if ( (uReadOrWrite != 0x0001) && (uReadOrWrite != 0x0002))
	{	
		return M24SR_ERROR_PARAMETER;
	}
  
	/* build the command */
	Command.Header.CLA = C_APDU_CLA_DEFAULT;
	Command.Header.INS = C_APDU_DISABLE;
	/* copy the Password Id */
	Command.Header.P1 = GETMSB	(uReadOrWrite ) ;
	Command.Header.P2 = GETLSB	(uReadOrWrite ) ;
	/* build the I²C command */
	M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_DISABLEVERIFREQ,  Command, &NbByte , pBuffer);
  
	/* send the request */ 
	errchk( M24SR_SendI2Ccommand ( NbByte , pBuffer ));
	/* The right access to be updated in EEPROM need at least 6ms */		
	errchk( M24SR_IsAnswerReady ( ));
	/* read the response */ 
	errchk( M24SR_ReceiveI2Cresponse ( M24SR_STATUSRESPONSE_NBBYTE , pBuffer ));
  
	status = M24SR_IsCorrectCRC16Residue (pBuffer, M24SR_STATUSRESPONSE_NBBYTE); 
	return status;
	
Error :
	return M24SR_ERROR_I2CTIMEOUT;
}

/**
  * @brief  This function sends the EnablePermananentState command
	* @param	uReadOrWrite : enable the read or write protection ( 0x0001 : Read or 0x0002 : Write  )
  * @retval Status (SW1&SW2) : Status of the operation to complete.
	* @retval M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_EnablePermanentState ( uc16 uReadOrWrite  )
{
  uint8_t 	*pBuffer = uM24SRbuffer;
  uint16_t	status ; 
  uint16_t	NbByte;
  
	/*check the parameters */
	if ( (uReadOrWrite != 0x0001) && (uReadOrWrite != 0x0002))
	{	
		return M24SR_ERROR_PARAMETER;
	}
  
	/* build the command */
	Command.Header.CLA = C_APDU_CLA_ST;
	Command.Header.INS = C_APDU_ENABLE;
	/* copy the Password Id */
	Command.Header.P1 = GETMSB	(uReadOrWrite ) ;
	Command.Header.P2 = GETLSB	(uReadOrWrite ) ;
	/* build the I²C command */
	M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_ENABLEVERIFREQ,  Command, &NbByte , pBuffer);
  
	/* send the request */ 
	errchk( M24SR_SendI2Ccommand ( NbByte , pBuffer ));
	errchk( M24SR_IsAnswerReady ( ));
	/* read the response */ 
	errchk( M24SR_ReceiveI2Cresponse ( M24SR_STATUSRESPONSE_NBBYTE , pBuffer ));
  
	status = M24SR_IsCorrectCRC16Residue (pBuffer, M24SR_STATUSRESPONSE_NBBYTE); 
	return status;
	
Error :
	return M24SR_ERROR_I2CTIMEOUT;
}

/**
  * @brief  This function sends the DisablePermanentState command
	* @param	uReadOrWrite : enable the read or write protection ( 0x0001 : Read or 0x0002 : Write  )
  * @retval Status (SW1&SW2) : Status of the operation to complete.
	* @retval M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_DisablePermanentState ( uc16 uReadOrWrite  )
{
  uint8_t 	*pBuffer = uM24SRbuffer;
  uint16_t	status ; 
  uint16_t	NbByte;
  
	/*check the parameters */
	if ( (uReadOrWrite != 0x0001) && (uReadOrWrite != 0x0002))
	{	
		return M24SR_ERROR_PARAMETER;
	}
  
	/* build the command */
	Command.Header.CLA = C_APDU_CLA_ST;
	Command.Header.INS = C_APDU_DISABLE;
	/* copy the Password Id */
	Command.Header.P1 = GETMSB	(uReadOrWrite ) ;
	Command.Header.P2 = GETLSB	(uReadOrWrite ) ;
	/* build the I²C command */
	M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_DISABLEVERIFREQ,  Command, &NbByte , pBuffer);
  
	/* send the request */ 
	errchk( M24SR_SendI2Ccommand ( NbByte , pBuffer ));
	errchk( M24SR_IsAnswerReady ( ));
	/* read the response */ 
	errchk( M24SR_ReceiveI2Cresponse ( M24SR_STATUSRESPONSE_NBBYTE , pBuffer ));
  
	status = M24SR_IsCorrectCRC16Residue (pBuffer, M24SR_STATUSRESPONSE_NBBYTE); 
	return status;
	
Error :
	return M24SR_ERROR_I2CTIMEOUT;
}

/**
  * @brief  This function generates a interrupt on GPO pin
	* @param	None
  * @retval Status (SW1&SW2) : Status of the operation to complete.
	* @retval M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_SendInterrupt ( void  )
{
	uint8_t 	*pBuffer = uM24SRbuffer;
	uint16_t	uP1P2 =0x001E;
	uint16_t	status ; 
	uint16_t	NbByte;
	
	status = M24SR_ManageI2CGPO( INTERRUPT);
	
	/* build the command */
	Command.Header.CLA = C_APDU_CLA_ST;
	Command.Header.INS = C_APDU_INTERRUPT;
	/* copy the Password Id */
	Command.Header.P1 = GETMSB	(uP1P2 ) ;
	Command.Header.P2 = GETLSB	(uP1P2 ) ;
	Command.Body.LC = 0x00 ;
	/* build the I²C command */
	M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_SENDINTERRUPT,  Command, &NbByte , pBuffer);
  
	/* send the request */ 
	errchk( M24SR_SendI2Ccommand ( NbByte , pBuffer ));
	errchk( M24SR_IsAnswerReady ( ));
	/* read the response */ 
	errchk( M24SR_ReceiveI2Cresponse ( M24SR_STATUSRESPONSE_NBBYTE , pBuffer ));
  
	status = M24SR_IsCorrectCRC16Residue (pBuffer, M24SR_STATUSRESPONSE_NBBYTE); 
	return status;
	
Error :
	return M24SR_ERROR_I2CTIMEOUT;
}

/**
  * @brief  This function force GPO pin to low state or high Z
	* @param	uSetOrReset : select if GPO must be low (reset) or HiZ
  * @retval Status (SW1&SW2) : Status of the operation to complete.
	* @retval M24SR_ERROR_I2CTIMEOUT : The I2C timeout occured.
  */
uint16_t M24SR_StateControl ( uc8 uSetOrReset )
{
	uint8_t 	*pBuffer = uM24SRbuffer;
	uint16_t	uP1P2 =0x001F;
	uint16_t	status ; 
	uint16_t	NbByte;
	
	/*check the parameters */
	if ( (uSetOrReset != 0x01) && (uSetOrReset != 0x00))
	{	
		return M24SR_ERROR_PARAMETER;
	}
	
	status = M24SR_ManageI2CGPO( STATE_CONTROL);
	
	/* build the command */
	Command.Header.CLA = C_APDU_CLA_ST;
	Command.Header.INS = C_APDU_INTERRUPT;
	/* copy the Password Id */
	Command.Header.P1 = GETMSB	(uP1P2 ) ;
	Command.Header.P2 = GETLSB	(uP1P2 ) ;
	Command.Body.LC = 0x01 ;
	/* copy the data */
	memcpy(Command.Body.pData , &uSetOrReset, 0x01 );
	//Command.Body.LE = 0x00 ;
	/* build the I²C command */
	M24SR_BuildIBlockCommand ( M24SR_CMDSTRUCT_GPOSTATE,  Command, &NbByte , pBuffer);
  
	/* send the request */ 
	errchk( M24SR_SendI2Ccommand ( NbByte , pBuffer ));
	errchk( M24SR_IsAnswerReady ( ));
	/* read the response */ 
	errchk( M24SR_ReceiveI2Cresponse ( M24SR_STATUSRESPONSE_NBBYTE , pBuffer ));
  
	status = M24SR_IsCorrectCRC16Residue (pBuffer, M24SR_STATUSRESPONSE_NBBYTE); 
	return status;
	
Error :
	return M24SR_ERROR_I2CTIMEOUT;
}

/**
  * @brief  This function configure GPO purpose for I2C session
	* @param	GPO_I2Cconfig: GPO configuration to set
  * @retval Status (SW1&SW2) : Status of the operation to complete.
  */
uint16_t M24SR_ManageI2CGPO( uc8 GPO_I2Cconfig)
{
	uint16_t status;
	uint8_t GPO_config;
	uint8_t DefaultPassword[16]={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	
	if( GPO_I2Cconfig > STATE_CONTROL)
	{	
		return M24SR_ERROR_PARAMETER;
	}
	
	/* we must not be in interrupt mode for I2C synchro as we will change GPO purpose */
	M24SR_SetI2CSynchroMode(M24SR_WAITINGTIME_POLLING);
	
	M24SR_SelectApplication();
	M24SR_SelectSystemfile();
	M24SR_ReadBinary ( 0x0004 , 0x01 , &GPO_config );
  
  /* Update only GPO purpose for I2C */	
	GPO_config = (GPO_config & 0xF0) | GPO_I2Cconfig;
	M24SR_SelectSystemfile();
	M24SR_Verify( I2C_PWD ,0x10 ,DefaultPassword );
	status = M24SR_UpdateBinary ( 0x0004 ,0x01, &(GPO_config) );
	
	/* if we have set interrupt mode for I2C synchro we can enable interrupt mode */
	if ( GPO_I2Cconfig == I2C_ANSWER_READY && status == M24SR_ACTION_COMPLETED)
#ifdef I2C_GPO_SYNCHRO_ALLOWED
		M24SR_SetI2CSynchroMode(M24SR_WAITINGTIME_GPO);
#else
  M24SR_SetI2CSynchroMode(M24SR_INTERRUPT_GPO);
#endif
	
	return status;
}

/**
  * @brief  This function configure GPO purpose for RF session
	* @param	GPO_RFconfig: GPO configuration to set
  * @retval Status (SW1&SW2) : Status of the operation to complete.
  */
uint16_t M24SR_ManageRFGPO( uc8 GPO_RFconfig)
{
	uint16_t status;
	uint8_t GPO_config;
	uint8_t DefaultPassword[16]={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	
	if( GPO_RFconfig > STATE_CONTROL)
	{	
		return M24SR_ERROR_PARAMETER;
	}
	
	M24SR_SelectApplication();
	M24SR_SelectSystemfile();
	M24SR_ReadBinary ( 0x0004 , 0x01 , &GPO_config );
  
  /* Update only GPO purpose for I2C */	
	GPO_config = (GPO_config & 0x0F) | (GPO_RFconfig<<4);
	M24SR_SelectSystemfile();
	M24SR_Verify( I2C_PWD ,0x10 ,DefaultPassword );
	status = M24SR_UpdateBinary ( 0x0004 ,0x01, &(GPO_config) );
	
	return status;
}


/**
  * @brief  This function enable or disable RF communication
	* @param	OnOffChoice: GPO configuration to set
  * @retval Status (SW1&SW2) : Status of the operation to complete.
  */
void M24SR_RFConfig( uc8 OnOffChoice)
{
	M24SR_RFConfig_Hard(OnOffChoice);
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2013 STMicroelectronics *****END OF FILE****/

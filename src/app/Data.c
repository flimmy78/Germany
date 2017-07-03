//#include <includes.h>
#include <stdint.h>
#include "TypeMeter.h"
#include "TypeE2p.h"
#include "Type.h"
#include "Data.h"
#include "Display.h"
#include "RsComm.h"
#include "Measure.h"
#include "Power.h"
#include "Port.h"
#include "sml.h"
#include "smlfunc.h"

//#include <Dl645_Globals.h>

//*****************************************************************
// define global data
//*****************************************************************

PARA		Para;				// 电表参数
CLK			Clk;
COMM		Comm;
COMM8213	Comm8213;
ADJ 		Adj;
DISKEY		Disk;

SRAM 		SM;
FLAG		Flag;




//#pragma memory=default
/*
const unsigned char TimeMaxTab[7] =
{
//	0x99,0x12,0x31,0x06,0x23,0x59,0x59
	0x59,0x59,0x23,0x06,0x31,0x12,0x99
};
*/


const unsigned char TimeMaxTab[3] =
{
	0x59,0x59,0x23
};

const unsigned char DateMaxTab[4] =
{
	0x07,0x31,0x12,0x99
};

const char MonTab[13] =
{
	0,31,28,31,30,31,30,31,31,30,31,30,31
};
const unsigned char BCDMonTab[13]=
{	0x31,0x31,0x28,0x31,0x30,0x31,0x30,
	0x31,0x31,0x30,0x31,0x30,0x31
};

const unsigned char WeekTab[] = {//èò?ê??D??ú±í  
(3 << 5) + 31,//1?? 
(6 << 5) + 29,//2?? 
(0 << 5) + 31,//3?? 
(3 << 5) + 30,//4?? 
(5 << 5) + 31,//5??   
(1 << 5) + 30,//6?? 
(3 << 5) + 31,//7?? 
(6 << 5) + 31,//8?? 
//(1 << 5) + 30,//9?? 
(2 << 5) + 30,//9?? 
(4 << 5) + 31,//10?? 
(0 << 5) + 30,//11?? 
(2 << 5) + 31 //12??
};
/*
const int LoadDayTab[13] =						//用于年负荷数据表
{
	0,0,31,60,91,121,152,182,213,244,274,305,335
};
*/

//const unsigned char LCDRollOrderTab[21] =
//{
//0x14,0x12,0x13,0x10,0x0F,0x11,0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE   	
//};

const LASTSUNDAY LastSunTab[]=
{
	0x26,0x29,	//; YEAR 00	
	0x25,0x28,	//; YEAR 01	
	0x31,0x27,	//; YEAR 02	
	0x30,0x26,	//; YEAR 03	
	0x28,0x31,	//; YEAR 04	
	0x27,0x30,	//; YEAR 05	
	0x26,0x29,	//; YEAR 06	
	0x25,0x28,	//; YEAR 07	
	0x30,0x26,	//; YEAR 08	
	0x29,0x25,	//; YEAR 09	
	0x28,0x31,	//; YEAR 10	
	0x27,0x30,	//; YEAR 11	
	0x25,0x28,	//; YEAR 12
	0x31,0x27,	//; YEAR 13
	0x30,0x26,	//; YEAR 14	
	0x29,0x25,	//; YEAR 15	
	0x27,0x30,	//; YEAR 16	
	0x26,0x29,	//; YEAR 17	
	0x25,0x28,	//; YEAR 18	
	0x31,0x27,	//; YEAR 19	
	0x29,0x25,	//; YEAR 20	
	0x28,0x31,	//; YEAR 21	
	0x27,0x30,	//; YEAR 22	
	0x26,0x29,	//; YEAR 23	
	0x31,0x27,	//; YEAR 24	
	0x30,0x26,	//; YEAR 25	
	0x29,0x25,	//; YEAR 26	
	0x28,0x31,	//; YEAR 27	
	0x26,0x29,	//; YEAR 28	
	0x25,0x28,	//; YEAR 29	
	0x31,0x27,	//; YEAR 30	
	0x30,0x26,	//; YEAR 31	
	0x28,0x31,	//; YEAR 32	
	0x27,0x30,	//; YEAR 33	
	0x26,0x29,	//; YEAR 34	
	0x25,0x28	//; YEAR 35			
};

const E2Para E2PTab[4] =
{
	
#if ( SinglePhase == YesCheck )
	ECDataEAds,	0x3000,	0x2,	0,	//24C512	
	FMAds,		0x3FF,	0x2,	0,	//FM24CL16	3V
	AdjEAds,		0x80,	0x2,	0,	//ATMEL24C04	
#endif	

#if ( Threephase == YesCheck ) //三相		
	AdjEAds,		0x80,	0x2,	0,	//ATMEL24C04
	DataEAds,	0xFFF,	0x2,	0,	//24C64
	FMAds,		0x3FF,	0x2,	0,	//FM24CL16	3V	
#endif	
	ProfEAds,	0x00,	0x1,	0	//24C512
};

#define	READ	0x8000
#define	WRITE	0x4000
#define MM1		0x0100
#define MM2		0x0200
#define MM3		0x0300
#define MM4		0x0400
#define MM5		0x0500


const unsigned char CByteBit[8] = 
{ 
//	0x01, 0x02, 0x04, 0x08, 
//	0x10, 0x20, 0x40, 0x80 
	0x80, 0x40, 0x20, 0x10, 
	0x08, 0x04, 0x02, 0x01
};
const unsigned short ByteBit[16] = 
{ 
	0x0001, 0x0002, 0x0004, 0x0008, 
	0x0010, 0x0020, 0x0040, 0x0080,
	0x0100, 0x0200, 0x0400, 0x0800, 
	0x1000, 0x2000, 0x4000, 0x8000 	 
};

//CRC16的表格//
const unsigned short Crc16tab[256] = {
0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

#ifndef NULL
#define NULL 0
#endif
const uint8_t DeviceClass[8]={0x81,0x81,0xc7,0x82,0x60,0xff};
const uint8_t Manufacture[4]={"LOG"};
const uint8_t firmware_version[]="STC868IRANDR01VS1";
uint8_t Z1_M[16];
const SMLOBISTAB	SMLOBISTab[]=
{
	0x01,0x00,0x5E,0x31,0x00,0x01, Len4,0x65,(uint32_t)&Comm.SecPulseCnt,	RAM_INTER_TYPE+READ,			1,NULL,NULL,
	0x01,0x00,0x5E,0x31,0x01,0x01, Len1,0x42,E2P_DisPActive,							0x80+READ+WRITE,1,NULL,NULL,
	0x01,0x00,0x5E,0x31,0x01,0x02, Len1,0x42,E2P_DisDataSetFlag,  				0x80+READ+WRITE,1,NULL,NULL,
	0x01,0x00,0x5E,0x31,0x01,0x03, Len2,0x63,MOnCnt,  										0x80+READ,			1,NULL,NULL,
	0x01,0x00,0x5E,0x31,0x01,0x04, Len1,0x42,E2P_ManiDetectFlag,  				0x80+READ+WRITE,1,NULL,NULL,
#if 0	
	0x01,0x00,0x5E,0x31,0x01,0x05, Len6,0x07,E2P_DeviceClass,  						0x80+READ,			1,NULL,NULL,
#else
	0x01,0x00,0x5E,0x31,0x01,0x05, Len6,0x07,(uint32_t)DeviceClass,  			RAM_TYPE+READ,			1,NULL,NULL,
#endif
	0x01,0x00,0x5E,0x31,0x01,0x09, Len1,0x42,E2P_Mechanical,  						0x80+READ+WRITE,1,NULL,NULL,
	0x01,0x00,0x5E,0x31,0x01,0x0A, Len2,0x63,OpCoverCnt,  						    0x80+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x31,0x01,0x01, Len6,0x07,Server_ID,  									0x80+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x01,0x00,0xff, 14,	0x0f,Server_ID,  									0x80+READ,			1,NULL,NULL,
	
	0x01,0x00,0x60,0x02,0x00,0x00, 20,	0x81,(uint32_t)firmware_version,  					RAM_TYPE+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x01,0x04, 6,		0x07,E2P_HardwareVer,  						0x80+READ,			1,NULL,NULL,
	0x01,0x00,0x5E,0x31,0x00,0x02, 0,		0x01,PublicKey_Y,									0x90+READ,			1,NULL,Get_PublicKey,
	0x01,0x00,0x5E,0x31,0x00,0x03, 0,	  0x01,PublicKey_Y,								  0x90+READ,			1,NULL,Get_Certificate,
/*	0x01,0x00,0x5E,0x31,0x00,0x04, 12,	0x0d,E2P_PrivateKey,						  0x80+WRITE,			1,NULL,NULL,*/
	0x01,0x00,0x5E,0x31,0x00,0x04, 255,	0x82,E2P_LMN_Certi,						  0x80+WRITE,			1,NULL,SetPrivateKey,
	0x01,0x00,0x5E,0x31,0x00,0x05, 16,	0x81,(uint32_t)Z1_M,				  		RAM_TYPE+WRITE,	0x1,NULL,SetMKey,
	0x01,0x00,0x5E,0x31,0x00,0x05, 16,	0x81,E2P_SymmetricalKey,				  0x80+WRITE,			0x100,NULL,NULL,
	0x01,0x00,0x5E,0x31,0x00,0x06, 16,	0x81,E2P_SymmetricalKey,				  0x80+READ+WRITE+XPERMIT,0x100,NULL,NULL,
	0x01,0x00,0x60,0x32,0x01,0x03, Len2,0x03,E2P_FirmWareCheck,				  	0x80+READ,			1,NULL,NULL,
	0x01,0x00,0x5e,0x31,0x00,0x07, Len1,0x42,E2P_ResetCrypoto,						0x80+WRITE,			1,Judge_ResteCry,NULL,
#if 0	
	0x01,0x00,0x01,0x08,0x00,0xFF, Len8,0x69,CMon_EC_Pp0,									0x80+READ,			32770/*32768*/,NULL,NULL,
	
	0x01,0x00,0x02,0x08,0x00,0xFF, Len8,0x69,CMon_EC_Pn0,									0x80+READ,			32770/*32768*/,NULL,NULL,
#else
	0x01,0x00,0x01,0x08,0x00,0xFF, Len8,0x69,CMon_EC_Pp0,									0x90+READ,			32770/*32768*/,GetEC_Pp0,GetP_back,
	
	0x01,0x00,0x02,0x08,0x00,0xFF, Len8,0x69,CMon_EC_Pn0,									0x90+READ,			32770/*32768*/,GetEC_Pn0,GetP_back,
#endif
#if 0
	0x01,0x00,0x20,0x07,0x00,0xFF, Len8,0x69,(uint32_t)&Para.Ua[0],				0x70+READ,			3,NULL,NULL,
	0x01,0x00,0x34,0x07,0x00,0xFF, Len8,0x69,(uint32_t)&Para.Ub[0],				0x70+READ,			3,NULL,NULL,
	0x01,0x00,0x48,0x07,0x00,0xFF, Len8,0x69,(uint32_t)&Para.Uc[0],				0x70+READ,			3,NULL,NULL,
#else
	0x01,0x00,0x20,0x07,0x00,0xFF, Len8,0x69,(uint32_t)&Para.Ua,				0x90+READ,			3,GetU_L1,GetU_back,
	0x01,0x00,0x34,0x07,0x00,0xFF, Len8,0x69,(uint32_t)&Para.Ub,				0x90+READ,			3,GetU_L2,GetU_back,
	0x01,0x00,0x48,0x07,0x00,0xFF, Len8,0x69,(uint32_t)&Para.Uc,				0x90+READ,			3,GetU_L3,GetU_back,
#endif
#if 0
	0x01,0x00,0x10,0x07,0x00,0xFF, Len8,0x69,(uint32_t)&Para.PtABS[0],		0x70+READ,			3,NULL,NULL,
#else
	0x01,0x00,0x10,0x07,0x00,0xFF, Len8,0x69,(uint32_t)&Para.Pt,				0x90+READ,			3,GetS,GetS_back,
#endif
	0x01,0x00,0x01,0x08,0x00,0xFF, Len1,0x42,E2P_OrderPlusA,							0x80+WRITE,			1,Judge_P,NULL,  //正向签名
	0x01,0x00,0x02,0x08,0x00,0xFF, Len1,0x42,E2P_OrderDecA,								0x80+WRITE,			1,Judge_N,NULL,  //反向签名
	//0x01,0x00,0x60,0x05,0x00,0xFF, Len4,0x65,E2P_SATEWORD,  							0x80+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x05,0x00,0xFF, Len4,0x65,(uint32_t)&Para.meter_sts,  		RAM_INTER_TYPE+READ,			1,NULL,NULL,
	/*0x01,0x00,0x5E,0x31,0x00,0x09, Len1,0x42,E2P_MaxFragLen,  						0x80+READ+WRITE,NULL,*/
	0x01,0x00,0x5E,0x31,0x00,0x09, Len2,0x63,E2P_MaxFragLen,  						0x80+READ+WRITE,			1,Judge_MaxLen,NULL,
	0x01,0x00,0x5E,0x31,0x01,0x06, Len1,0x42,E2P_ProtectionPINCode,				0x80+READ+WRITE,1,NULL,NULL,
	
	0x01,0x00,0x60,0x32,0x01,0x01, 3,  		0x04,(uint32_t)Manufacture,								RAM_TYPE+READ,1,NULL,NULL,
	0x01,0x00,0x00,0x02,0x00,0x00, 17,		0x81,(uint32_t)firmware_version,  				RAM_TYPE+READ,			1,NULL,NULL,
	0x01,0x00,0x00,0x02,0x00,0x02, 17,		0x81,(uint32_t)firmware_version,  				RAM_TYPE+READ+XPERMIT,			1,NULL,NULL,
	0x01,0x00,0x00,0x02,0x00,0x03, 17,		0x81,(uint32_t)firmware_version,  				RAM_TYPE+READ+XPERMIT,			1,NULL,NULL,
	0x01,0x00,0x00,0x02,0x00,0x04, 17,		0x81,(uint32_t)firmware_version,  				RAM_TYPE+READ+XPERMIT,			1,NULL,NULL,
	0x01,0x00,0x00,0x02,0x00,0x05, 17,		0x81,(uint32_t)firmware_version,  				RAM_TYPE+READ+XPERMIT,			1,NULL,NULL,
	0x01,0x00,0x00,0x02,0x00,0x06, 17,		0x81,(uint32_t)firmware_version,  				RAM_TYPE+READ+XPERMIT,			1,NULL,NULL,
	0x01,0x00,0x00,0x02,0x00,0x07, 17,		0x81,(uint32_t)firmware_version,  				RAM_TYPE+READ+XPERMIT,			1,NULL,NULL,
	0x01,0x00,0x00,0x02,0x00,0x08, 17,		0x81,(uint32_t)firmware_version,  				RAM_TYPE+READ+XPERMIT,			1,NULL,NULL,
	0x01,0x00,0x00,0x02,0x00,0x09, 17,		0x81,(uint32_t)firmware_version,  				RAM_TYPE+READ+XPERMIT,			1,NULL,NULL,
	
	0x01,0x00,0x00,0x02,0x00,0x0a, 17,		0x81,(uint32_t)firmware_version,  				RAM_TYPE+READ+XPERMIT,			1,NULL,NULL,
	0x01,0x00,0x00,0x02,0x00,0x0b, 17,		0x81,(uint32_t)firmware_version,  				RAM_TYPE+READ+XPERMIT,			1,NULL,NULL,
	0x01,0x00,0x00,0x02,0x00,0x0c, 17,		0x81,(uint32_t)firmware_version,  				RAM_TYPE+READ+XPERMIT,			1,NULL,NULL,
	0x01,0x00,0x00,0x02,0x00,0x0d, 17,		0x81,(uint32_t)firmware_version,  				RAM_TYPE+READ+XPERMIT,			1,NULL,NULL,
	0x01,0x00,0x00,0x02,0x00,0x0e, 17,		0x81,(uint32_t)firmware_version,  				RAM_TYPE+READ+XPERMIT,			1,NULL,NULL,
	0x01,0x00,0x00,0x02,0x00,0x0f, 17,		0x81,(uint32_t)firmware_version,  				RAM_TYPE+READ+XPERMIT,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x02,0x04, 6,		0x07,E2P_HardwareVer,  						0x80+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x03,0x04, 6,		0x07,E2P_HardwareVer,  						0x80+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x04,0x04, 6,		0x07,E2P_HardwareVer,  						0x80+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x05,0x04, 6,		0x07,E2P_HardwareVer,  						0x80+READ,			1,NULL,NULL,
	
	0x01,0x00,0x60,0x32,0x06,0x04, 6,		0x07,E2P_HardwareVer,  						0x80+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x07,0x04, 6,		0x07,E2P_HardwareVer,  						0x80+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x08,0x04, 6,		0x07,E2P_HardwareVer,  						0x80+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x09,0x04, 6,		0x07,E2P_HardwareVer,  						0x80+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x0a,0x04, 6,		0x07,E2P_HardwareVer,  						0x80+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x0b,0x04, 6,		0x07,E2P_HardwareVer,  						0x80+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x0c,0x04, 6,		0x07,E2P_HardwareVer,  						0x80+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x0d,0x04, 6,		0x07,E2P_HardwareVer,  						0x80+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x0e,0x04, 6,		0x07,E2P_HardwareVer,  						0x80+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x0f,0x04, 6,		0x07,E2P_HardwareVer,  						0x80+READ,			1,NULL,NULL,
	
	0x01,0x00,0x60,0x32,0x02,0x03, 2,		0x03,E2P_FirmWareCheck,  					0x80+READ+XPERMIT,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x03,0x03, 2,		0x03,E2P_FirmWareCheck,  					0x80+READ+XPERMIT,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x04,0x03, 2,		0x03,E2P_FirmWareCheck,  					0x80+READ+XPERMIT,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x05,0x03, 2,		0x03,E2P_FirmWareCheck,  					0x80+READ+XPERMIT,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x06,0x03, 2,		0x03,E2P_FirmWareCheck,  					0x80+READ+XPERMIT,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x07,0x03, 2,		0x03,E2P_FirmWareCheck,  					0x80+READ+XPERMIT,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x08,0x03, 2,		0x03,E2P_FirmWareCheck,  					0x80+READ+XPERMIT,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x09,0x03, 2,		0x03,E2P_FirmWareCheck,  					0x80+READ+XPERMIT,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x0A,0x03, 2,		0x03,E2P_FirmWareCheck,  					0x80+READ+XPERMIT,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x0B,0x03, 2,		0x03,E2P_FirmWareCheck,  					0x80+READ+XPERMIT,			1,NULL,NULL,
	
	0x01,0x00,0x60,0x32,0x0C,0x03, 2,		0x03,E2P_FirmWareCheck,  					0x80+READ+XPERMIT,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x0D,0x03, 2,		0x03,E2P_FirmWareCheck,  					0x80+READ+XPERMIT,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x0E,0x03, 2,		0x03,E2P_FirmWareCheck,  					0x80+READ+XPERMIT,			1,NULL,NULL,
	0x01,0x00,0x60,0x32,0x0F,0x03, 2,		0x03,E2P_FirmWareCheck,  					0x80+READ+XPERMIT,			1,NULL,NULL,
	

	0x01,0x00,0x01,0x08,0x00,0xFF, Len8,0x69,CMon_EC_Pp0,									0x90+READ,							32768,Judge_P,GetP_Signature,
	0x01,0x00,0x01,0x08,0x00,0xFF, Len8,0x69,CMon_EC_Pp0,									0x90+READ,							32769,Judge_P,GetP_Signature,
	0x01,0x00,0x02,0x08,0x00,0xFF, Len8,0x69,CMon_EC_Pn0,									0x90+READ,							32768,Judge_N,GetN_Signature,
	0x01,0x00,0x02,0x08,0x00,0xFF, Len8,0x69,CMon_EC_Pn0,									0x90+READ,							32770,Judge_N,GetN_Signature,
	0x01,0x00,0x01,0x08,0x00,0x60, Len8,0x69,(uint32_t)&Para.P0_day,	    RAM_INTER_TYPE+READ+WRITE+PERMIT,3,Judge_Day,ReSetDay,
	0x01,0x00,0x01,0x08,0x00,0x61, Len8,0x69,(uint32_t)&Para.P0_week,	    RAM_INTER_TYPE+READ+WRITE+PERMIT,3,Judge_Week,NULL,
	0x01,0x00,0x01,0x08,0x00,0x62, Len8,0x69,(uint32_t)&Para.P0_month,	  RAM_INTER_TYPE+READ+WRITE+PERMIT,3,Judge_Month,NULL,
	0x01,0x00,0x01,0x08,0x00,0x63, Len8,0x69,(uint32_t)&Para.P0_year,	    RAM_INTER_TYPE+READ+WRITE+PERMIT,3,Judge_Year,NULL,
	0x01,0x00,0x01,0x08,0x00,0x64, Len8,0x69,(uint32_t)&Para.P0_last,	    RAM_INTER_TYPE+READ+WRITE+PERMIT,3,NULL,ReSetLast,
	0x01,0x00,0x1F,0x07,0x00,0xFF, Len8,0x69,(uint32_t)&Para.Ia,	    		RAM_INTER_TYPE+READ,			3,NULL,GetI_back,
	0x01,0x00,0x33,0x07,0x00,0xFF, Len8,0x69,(uint32_t)&Para.Ib,	    		RAM_INTER_TYPE+READ,			3,NULL,GetI_back,
	0x01,0x00,0x47,0x07,0x00,0xFF, Len8,0x69,(uint32_t)&Para.Ic,	    		RAM_INTER_TYPE+READ,			3,NULL,GetI_back,
	0x01,0x00,0x5E,0x31,0x01,0x07, Len4,0x05,DisPINAdd,										E2P_TYPE+WRITE,1,Judge_PINCODE,NULL,
	0x01,0x00,0x5E,0x31,0x01,0x08, Len1,0x42,E2P_DisHistorical,						E2P_TYPE+READ+WRITE,1,NULL,NULL,
	0x01,0x00,0x60,0x5A,0x02,0x01, 2,		0x03,E2P_FirmWareCheck,  					E2P_TYPE+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x5A,0x02,0x02, 2,		0x03,E2P_FirmWareCheck,  					E2P_TYPE+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x5A,0x02,0x03, 2,		0x03,E2P_FirmWareCheck,  					E2P_TYPE+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x5A,0x02,0x04, 2,		0x03,E2P_FirmWareCheck,  					E2P_TYPE+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x5A,0x02,0x05, 2,		0x03,E2P_FirmWareCheck,  					E2P_TYPE+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x5A,0x02,0x06, 2,		0x03,E2P_FirmWareCheck,  					E2P_TYPE+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x5A,0x02,0x07, 2,		0x03,E2P_FirmWareCheck,  					E2P_TYPE+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x5A,0x02,0x08, 2,		0x03,E2P_FirmWareCheck,  					E2P_TYPE+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x5A,0x02,0x09, 2,		0x03,E2P_FirmWareCheck,  					E2P_TYPE+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x5A,0x02,0x0a, 2,		0x03,E2P_FirmWareCheck,  					E2P_TYPE+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x5A,0x02,0x0b, 2,		0x03,E2P_FirmWareCheck,  					E2P_TYPE+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x5A,0x02,0x0c, 2,		0x03,E2P_FirmWareCheck,  					E2P_TYPE+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x5A,0x02,0x0d, 2,		0x03,E2P_FirmWareCheck,  					E2P_TYPE+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x5A,0x02,0x0e, 2,		0x03,E2P_FirmWareCheck,  					E2P_TYPE+READ,			1,NULL,NULL,
	0x01,0x00,0x60,0x5A,0x02,0x0f, 2,		0x03,E2P_FirmWareCheck,  					E2P_TYPE+READ,			1,NULL,NULL,
	0x01,0x00,0x5E,0x31,0x01,0x0d, Len1,0x62,E2P_ChangeTariff,						E2P_TYPE+WRITE+XPERMIT,1,NULL,NULL,	
	0x01,0x00,0x5E,0x31,0x01,0x0c, Len1,0x62,E2P_ChangeTariff,						E2P_TYPE+WRITE+XPERMIT,1,NULL,NULL,
	0x01,0x00,0x01,0x08,0x01,0xFF, Len8,0x69,CMon_EC_Pp1,									E2P_TYPE+READ+XPERMIT,	32770,NULL,NULL,
	0x01,0x00,0x01,0x08,0x02,0xFF, Len8,0x69,CMon_EC_Pp2,									E2P_TYPE+READ+XPERMIT,	32770,NULL,NULL,
	0x01,0x00,0x02,0x08,0x01,0xFF, Len8,0x69,CMon_EC_Pn1,									E2P_TYPE+READ+XPERMIT,	32770,NULL,NULL,
	0x01,0x00,0x02,0x08,0x02,0xFF, Len8,0x69,CMon_EC_Pn2,									E2P_TYPE+READ+XPERMIT,	32770,NULL,NULL,
	/*0x01,0x00,0x01,0x08,0x00,0x60, Len8,0x69,E2P_HisDAY1,									0x80+READ+WRITE,NULL,
	0x01,0x00,0x01,0x08,0x00,0x61, Len8,0x69,E2P_HisDAY7,									0x80+READ+WRITE,NULL,
	0x01,0x00,0x01,0x08,0x00,0x62, Len8,0x69,E2P_HisDAY30,								0x80+READ+WRITE,NULL,
	0x01,0x00,0x01,0x08,0x00,0x63, Len8,0x69,E2P_HisDAY365,								0x80+READ,			NULL,
	0x01,0x00,0x01,0x08,0x00,0x64, Len8,0x69,E2P_LastReset,								0x80+READ,			NULL,
	
	0x81,0x81,0xc7,0x8c,0x01,0xFF, Len1,0x42,E2P_INF0,				0x01+READ+WRITE,NULL,
	0x81,0x81,0xc7,0x8c,0x02,0xFF, Len1,0x42,E2P_ModeEDL40,		0x06+READ+WRITE,NULL,
	0x81,0x81,0xc7,0x8c,0x03,0xFF, Len1,0x42,E2P_ResHistorical,	0x00+READ+WRITE,NULL,
	0x81,0x81,0xc7,0x8c,0x04,0xFF, Len1,0x42,E2P_DisHistorical,	0x00+READ+WRITE,NULL,
	0x81,0x81,0xc7,0x8c,0x08,0xFF, Len1,0x42,E2P_SetMSB,		0x00+READ+WRITE,NULL,
	
	0x81,0x81,0xc7,0x8c,0x09,0xFF, Len1,0x42,E2P_OResHisButton,	0x00+READ+WRITE,NULL,


	
	0x01,0x00,0x01,0x08,0x00,0x63, Len5,0x56,E2P_HisDAY365,		0x00+READ,NULL,
	0x01,0x00,0x00,0x09,0x0B,0x00, Len4,0x65,E2P_SMLTime,		0x10+READ+WRITE,NULL,
	0x81,0x81,0xc7,0x8c,0x0C,0xFF, Len4,0x65,E2P_ConManipulation,	0x00+READ,NULL,
	0x81,0x81,0xc7,0x8c,0x07,0xFF, Len4,0x65,E2P_DisTariff,		0x02+READ+WRITE,NULL,
	0x00,0x00,0x60,0x0E,0x00,0xFF, Len1,0x62,E2P_ChangeTariff,	0x03+READ+WRITE,NULL,	//unsigned8
	
	0x01,0x00,0x01,0x08,0x00,0xFF, Len8,0x59,CMon_EC_Pp0,		0x00+READ,NULL,
	0x01,0x00,0x01,0x08,0x01,0xFF, Len8,0x59,CMon_EC_Pp1,		0x00+READ,NULL,
	0x01,0x00,0x01,0x08,0x02,0xFF, Len8,0x59,CMon_EC_Pp2,		0x00+READ,NULL,
	0x01,0x00,0x60,0x05,0x05,0xFF, Len2,0x63,E2P_SATEWORD,		0x00+READ+WRITE,NULL,
	0x01,0x00,0x01,0x11,0x00,0xFF, Len8,0x59,Last_EC_Pp0,		0x04+READ,NULL,
	
	0x81,0x81,0xc7,0x8c,0x0B,0xFF, Len2,0x05,DisPINAdd,		0x0C+READ+WRITE,NULL,
	0x81,0x81,0xc7,0x8c,0x0D,0xFF, Len1,0x42,E2P_ProtectionPINCode,	0x00+READ+WRITE,NULL,
	0x01,0x00,0x00,0x02,0x02,0xFF, Len6,0x07,E2P_NumSwitchProgramm,	0x00+READ+WRITE,NULL,
	0x81,0x81,0xc7,0x8c,0x06,0xFF, Len9,0x0A,E2P_TextAusgabe,	0x0B+READ+WRITE,NULL,
	0x01,0x00,0x29,0x07,0x00,0xFF, Len4,0x55,0x01,			0x00+READ+WRITE,NULL,
	
	0x01,0x00,0x15,0x07,0x00,0xFF, Len4,0x55,0x00,			0x00+READ+WRITE,NULL,
	0x01,0x00,0x3D,0x07,0x00,0xFF, Len4,0x55,0x02,			0x00+READ+WRITE,NULL,
	0x01,0x00,0x01,0x07,0x00,0xFF, Len4,0x55,0x03,			0x00+READ+WRITE,NULL,
	0x81,0x81,0xC7,0x8C,0xE2,0xFF, Len4,0x55,0x00,			0x80+READ+WRITE,NULL,
	0x01,0x02,0x03,0x04,0x05,0x06, Len6,0x57,EYBase,		0x00+READ+WRITE,NULL,
	
	0x01,0x02,0x05,0x05,0x05,0x07, Len10,0x0B,Server_ID,		0x60+READ+WRITE,NULL,
	0x81,0x81,0xC7,0x83,0x82,0x02, Len10,0x0B,PublicKey_Y,		0x07+READ+WRITE,NULL,
	0x81,0x81,0xc7,0x82,0x03,0xFF, Len3,0x04,Server_ID,		0x05+READ,NULL,	//It includes information about the manufacturer
	0x81,0x81,0xc7,0x82,0x02,0xFF, Len6,0x07,Server_ID,		0x08+READ,NULL,
	0x81,0x81,0xc7,0x82,0x01,0xFF, Len6,0x07,Server_ID,		0x09+READ,NULL,
	
	0x81,0x81,0xc7,0x8c,0x10,0xFF, Len1,0x42,E2P_DisPActive,	0x00+READ+WRITE,NULL,
	0x81,0x81,0xc7,0x8c,0x0E,0xFF, Len1,0x42,Magnetic_Bit,		0x0A+WRITE,NULL,
	
	
//	0x01,0x00,0x5E,0x31,0x01,0x09, Len4,0x42,E2P_DisPActive,  0x80+READ+WRITE,
	
	0x01,0x00,0x05,0x08,0x00,0xFF, Len8,0x69,E2P_DisPActive,  0x80+READ+WRITE,NULL,
	0x01,0x00,0x06,0x08,0x00,0xFF, Len8,0x69,E2P_DisPActive,  0x80+READ+WRITE,NULL,
	0x01,0x00,0x07,0x08,0x00,0xFF, Len8,0x69,E2P_DisPActive,  0x80+READ+WRITE,NULL,
	0x01,0x00,0x08,0x08,0x00,0xFF, Len8,0x69,E2P_DisPActive,  0x80+READ+WRITE,NULL,*/
};
const SMLATTENTIONNUMBER SMLAttentionNumber[]=
{
	0x81,0x81,0xC7,0xC7,0xE0,0x00,		//beginning of application-specific error numbers
	0x81,0x81,0xC7,0xC7,0xFC,0xFF,		//end of application-specific error numbers
	0x81,0x81,0xC7,0xC7,0xFD,0x00,		//OK, positive acknowledgement.
	0x81,0x81,0xC7,0xC7,0xFD,0x01,		//Order will be executed later and result passed to server address per Response-without-Request.
	0x81,0x81,0xC7,0xC7,0xFD,0xFF,		//reserved	
	
	0x81,0x81,0xC7,0xC7,0xFE,0x00,		//error message that cannot be assigned to any of the meanings defined below
	0x81,0x81,0xC7,0xC7,0xFE,0x01,		//unknown SML designator
	0x81,0x81,0xC7,0xC7,0xFE,0x02,		//inadequate authentication,user/password combination impermissible
	0x81,0x81,0xC7,0xC7,0xFE,0x03,		//destination address('serverld')not available
	0x81,0x81,0xC7,0xC7,0xFE,0x04,		//order('reqfileld')not aveilable
	
	0x81,0x81,0xC7,0xC7,0xFE,0x05,		//one or more destination attribute(s) cannot be written
	0x81,0x81,0xC7,0xC7,0xFE,0x06,		//one or more destination attribute(s) cannot be read
	0x81,0x81,0xC7,0xC7,0xFE,0x07,		//communication with measuring point disturbed
	0x81,0x81,0xC7,0xC7,0xFE,0x08,		//raw data cannot be interpreted
	0x81,0x81,0xC7,0xC7,0xFE,0x09,		//value supplied is outside the permissible value range
	
	0x81,0x81,0xC7,0xC7,0xFE,0x0A,		//order not executed(for example ,because the 'parameterTreePath' supplies indicates an absennt element)
	0x81,0x81,0xC7,0xC7,0xFE,0x0B,		//checksum faulty
	0x81,0x81,0xC7,0xC7,0xFE,0x0C,		//broadcast not supported
	0x81,0x81,0xC7,0xC7,0xFE,0x0D,		//unexpected SML message (e.g. an SML file without an open request)
	0x81,0x81,0xC7,0xC7,0xFE,0x0E,		//unknown object in the load profile (the OBIS code in the request for a load profile refers to a data source not recorded in the load profile)
	
	0x81,0x81,0xC7,0xC7,0xFE,0x0F,		//data type not supported(e.g. the data type in a setProcPar.Req message does not corresspond to the expected data type)
	0x81,0x81,0xC7,0xC7,0xFE,0x10,		//Optional element not supported (an element defined in SML as OPTIONAL, which is not supported by the application, has been received as OPTIONAL.)
	0x81,0x81,0xC7,0xC7,0xFE,0x15,
	0x81,0x81,0xC7,0xC7,0xE0,0x01,
};


//SML命令对应的数据项//
const SMLORDER SMLOrder[15] =
{
	0x0100,0x07,
	0x0101,0x06,
	0x0200,0x01,
	0x0201,0x01,
	0x0300,0x09,
	0x0301,0x08,
	0x0400,0x09,
	0x0401,0x09,
	0x0500,0x05,
	0x0501,0x03,
	0x0600,0x05,
	0x0601,0x04,
	0x0700,0x05,
	0x0701,0x07,
	0xff01,0x04,

};

#include	"CDdata.c"

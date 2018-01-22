
#ifndef _DL645CLOCK_H
#define _DL645CLOCK_H

//
//*******************************************************************************
//**  Function define
//*******************************************************************************
//
#define SYS_CLK_3M	1
#define SYS_CLK_7M	2
#define SYS_CLK_14M 3
#define SYS_CLK_29M	4
#define SYS_CLK			SYS_CLK_29M
extern void fnDl645MainClock_Init( void );
extern void fnDl645SubClock_Init( void );
extern	u32 fnDl645MainClock_Get( void );
extern void fnDl645RCClock_Init( void );
#endif

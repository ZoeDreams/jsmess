#ifndef __I8008_H__
#define __I8008_H__



/***************************************************************************
    CONSTANTS
***************************************************************************/

enum
{
	I8008_PC,
	I8008_A,I8008_B,I8008_C,I8008_D,I8008_E,I8008_H,I8008_L,
	I8008_ADDR1,I8008_ADDR2,I8008_ADDR3,I8008_ADDR4,I8008_ADDR5,I8008_ADDR6,I8008_ADDR7,I8008_ADDR8,
	I8008_GENPC = STATE_GENPC,
	I8008_GENSP = STATE_GENSP,
	I8008_GENPCBASE = STATE_GENPCBASE
};

/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

/***************************************************************************
    FUNCTION PROTOTYPES
***************************************************************************/

DECLARE_LEGACY_CPU_DEVICE(I8008, i8008);

CPU_DISASSEMBLE( i8008 );
#endif

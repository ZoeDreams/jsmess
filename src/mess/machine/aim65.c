/******************************************************************************

 AIM65

******************************************************************************/


#include "driver.h"
#include "includes/aim65.h"

/* Peripheral chips */
#include "machine/6821pia.h"
#include "machine/6522via.h"
#include "machine/6532riot.h"

/* DL1416A display chip */
#include "video/dl1416.h"

/* M6502 main CPU */
#include "cpu/m6502/m6502.h"



/******************************************************************************
 Function Prototypes
******************************************************************************/

void aim65_update_ds1(int digit, int data);
void aim65_update_ds2(int digit, int data);
void aim65_update_ds3(int digit, int data);
void aim65_update_ds4(int digit, int data);
void aim65_update_ds5(int digit, int data);



/******************************************************************************
 Global variables
******************************************************************************/


static UINT8 pia_a, pia_b;
static UINT8 riot_port_a;

/* Display driver interfaces */
static const dl1416_interface dl1416_ds1 = { DL1416T, aim65_update_ds1 };
static const dl1416_interface dl1416_ds2 = { DL1416T, aim65_update_ds2 };
static const dl1416_interface dl1416_ds3 = { DL1416T, aim65_update_ds3 };
static const dl1416_interface dl1416_ds4 = { DL1416T, aim65_update_ds4 };
static const dl1416_interface dl1416_ds5 = { DL1416T, aim65_update_ds5 };



/******************************************************************************
 Interrupt handling
******************************************************************************/


static void aim65_via_irq_func(int state)
{
	cpunum_set_input_line(0, M6502_IRQ_LINE, state ? HOLD_LINE : CLEAR_LINE);
}

/* STEP/RUN
 *
 * Switch S2 (STEP/RUN) causes AIM 65 to operate either in the RUN mode or the
 * single STEP mode. In the STEP mode, the NMI interrupt line is driven low
 * when SYNC and O2 go high during instruction execution if the address lines
 * are outside the A000-FFFF range. The NMI interrupt occurs on the high to
 * low transition of the NMI line. The Monitor software will trace instructions
 * and register, outside the Monitor instruction address range if the trace
 * modes are selected and the NMI Interrupt Routine is not bypassed.
 */



/******************************************************************************
 6821 PIA
******************************************************************************/

/* PA0: A0 (Address)
 * PA1: A1 (Address)
 * PA2: CE1 (Chip enable)
 * PA3: CE2 (Chip enable)
 * PA4: CE3 (Chip enable)
 * PA5: CE4 (Chip enable)
 * PA6: CE5 (Chip enable)
 * PA7: W (Write enable)
 * PB0-6: D0-D6 (Data)
 * PB7: CU (Cursor)
 */

static void aim65_pia(void)
{
	int which;

	for (which = 0; which < 5; which++)
	{
		dl1416_set_input_ce(which, pia_a & (0x04 << which));
		dl1416_set_input_w(which, pia_a & 0x80);
		dl1416_set_input_cu(which, pia_b & 0x80);
		dl1416_write(which, pia_a & 0x03, pia_b & 0x7f);
	}
}


static WRITE8_HANDLER( aim65_pia_a_w )
{
	pia_a = data;
	aim65_pia();
}


static WRITE8_HANDLER( aim65_pia_b_w )
{
	pia_b = data;
	aim65_pia();
}


static const pia6821_interface pia =
{
	NULL, // read8_handler in_a_func,
	NULL, // read8_handler in_b_func,
	NULL, // read8_handler in_ca1_func,
	NULL, // read8_handler in_cb1_func,
	NULL, // read8_handler in_ca2_func,
	NULL, // read8_handler in_cb2_func,
	aim65_pia_a_w,
	aim65_pia_b_w,
	NULL, // write8_handler out_ca2_func,
	NULL, // write8_handler out_cb2_func,
	NULL, // void (*irq_a_func)(int state),
	NULL, // void (*irq_b_func)(int state),
};


void aim65_update_ds1(int digit, int data) { output_set_digit_value( 0 + (digit ^ 3), data); }
void aim65_update_ds2(int digit, int data) { output_set_digit_value( 4 + (digit ^ 3), data); }
void aim65_update_ds3(int digit, int data) { output_set_digit_value( 8 + (digit ^ 3), data); }
void aim65_update_ds4(int digit, int data) { output_set_digit_value(12 + (digit ^ 3), data); }
void aim65_update_ds5(int digit, int data) { output_set_digit_value(16 + (digit ^ 3), data); }



/******************************************************************************
 6532 RIOT
******************************************************************************/


static READ8_HANDLER( aim65_riot_b_r )
{
	int data = 0xff;

	if (!(riot_port_a & 0x01)) {
		if (KEY_SPACE) data &= ~0x01;
		//right?
		if (KEY_POINT) data &= ~0x04;
		if (KEY_M) data &= ~0x08;
		if (KEY_B) data &= ~0x10;
		if (KEY_C) data &= ~0x20;
		if (KEY_Z) data &= ~0x40;
		//right?
	}

	if (!(riot_port_a & 0x02)) {
		if (KEY_DEL) data &= ~0x01; //backspace 0x08
		if (KEY_LF) data &= ~0x02; //0x60
		if (KEY_L) data &= ~0x04;
		if (KEY_J) data &= ~0x08;
		if (KEY_G) data &= ~0x10;
		if (KEY_D) data &= ~0x20;
		if (KEY_A) data &= ~0x40;
		//right?
	}

	if (!(riot_port_a & 0x04)) {
		//right?
		if (KEY_PRINT) data &= ~0x02; // backslash
		if (KEY_P) data &= ~0x04;
		if (KEY_I) data &= ~0x08;
		if (KEY_Y) data &= ~0x10;
		if (KEY_R) data &= ~0x20;
		if (KEY_W) data &= ~0x40;
		if (KEY_ESC) data &= ~0x80; //0x1b
	}

	if (!(riot_port_a & 0x08)) {
		if (KEY_RETURN) data &= ~0x01;
		//right?
		if (KEY_MINUS) data &= ~0x04;
		if (KEY_O) data &= ~0x08;
		if (KEY_U) data &= ~0x10;
		if (KEY_T) data &= ~0x20;
		if (KEY_E) data &= ~0x40;
		if (KEY_Q) data &= ~0x80;
	}

	if (!(riot_port_a & 0x10)) {
		if (KEY_CRTL) data &= ~0x01;
		//right?
		if (KEY_COLON) data &= ~0x04;
		if (KEY_9) data &= ~0x08;
		if (KEY_7) data &= ~0x10;
		if (KEY_5) data &= ~0x20;
		if (KEY_3) data &= ~0x40;
		if (KEY_1) data &= ~0x80;
	}

	if (!(riot_port_a & 0x20)) {
		if (KEY_LEFT_SHIFT) data &= ~0x01;
		//right?
		if (KEY_0) data &= ~0x04;
		if (KEY_8) data &= ~0x08;
		if (KEY_6) data &= ~0x10;
		if (KEY_4) data &= ~0x20;
		if (KEY_2) data &= ~0x40;
		if (KEY_F3) data &= ~0x80; //^ 0x5e
	}

	if (!(riot_port_a & 0x40)) {
		if (KEY_RIGHT_SHIFT) data &= ~0x01;
		if (KEY_DEL) data &= ~0x02; //backspace 0x08
		if (KEY_SEMICOLON) data &= ~0x04;
		if (KEY_K) data &= ~0x08;
		if (KEY_H) data &= ~0x10;
		if (KEY_F) data &= ~0x20;
		if (KEY_S) data &= ~0x40;
		if (KEY_F2) data &= ~0x80;
	}

	if (!(riot_port_a & 0x80)) {
		if (KEY_SLASH) data &=~0x04;
		if (KEY_COMMA) data &=~0x08;
		if (KEY_N) data &= ~0x10;
		if (KEY_V) data &= ~0x20;
		if (KEY_X) data &= ~0x40;
		if (KEY_F1) data &= ~0x80;
	}

	return data;
}


static WRITE8_HANDLER(aim65_riot_a_w)
{
	riot_port_a = data;
}


static const struct riot6532_interface r6532_interface =
{
	NULL,
	aim65_riot_b_r,
	aim65_riot_a_w,
	NULL
};



/******************************************************************************
 6522 VIA
******************************************************************************/


static WRITE8_HANDLER( aim65_via0_a_w )
{
	 aim65_printer_data_a(data);
}


static WRITE8_HANDLER( aim65_via0_b_w )
{
	aim65_printer_data_b(data);
}


static READ8_HANDLER( aim65_via0_b_r )
{
	return readinputport(4);
}


static const struct via6522_interface via0 =
{
	0, // read8_handler in_a_func;
	aim65_via0_b_r, // read8_handler in_b_func;
	0, // read8_handler in_ca1_func;
	0, // read8_handler in_cb1_func;
	0, // read8_handler in_ca2_func;
	0, // read8_handler in_cb2_func;
	aim65_via0_a_w,	// write8_handler out_a_func;
	aim65_via0_b_w, // write8_handler out_b_func;
	0, // write8_handler out_ca1_func;
	0, // write8_handler out_cb1_func;
	0, // write8_handler out_ca2_func;
	aim65_printer_on, // write8_handler out_cb2_func;
	aim65_via_irq_func // void (*irq_func)(int state);
};

static const struct via6522_interface user_via =
{
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};



/******************************************************************************
 Driver init
******************************************************************************/


DRIVER_INIT( aim65 )
{
	/* Init RAM */
	memory_install_read8_handler (0, ADDRESS_SPACE_PROGRAM, 0, mess_ram_size - 1, 0, 0, MRA8_RAM);
	memory_install_write8_handler(0, ADDRESS_SPACE_PROGRAM, 0, mess_ram_size - 1, 0, 0, MWA8_RAM);

	if (mess_ram_size < 4 * 1024)
	{
		memory_install_read8_handler (0, ADDRESS_SPACE_PROGRAM, mess_ram_size, 0x0fff, 0, 0, MRA8_NOP);
		memory_install_write8_handler(0, ADDRESS_SPACE_PROGRAM, mess_ram_size, 0x0fff, 0, 0, MWA8_NOP);
	}

	/* Init display driver */
	dl1416_config(0, &dl1416_ds1);
	dl1416_config(1, &dl1416_ds2);
	dl1416_config(2, &dl1416_ds3);
	dl1416_config(3, &dl1416_ds4);
	dl1416_config(4, &dl1416_ds5);

	dl1416_reset(0);
	dl1416_reset(1);
	dl1416_reset(2);
	dl1416_reset(3);
	dl1416_reset(4);

	pia_config(0, &pia);

	r6532_config(0, &r6532_interface);
	r6532_set_clock(0, 1000000);
	r6532_reset(0);

	via_config(0, &via0);
	via_0_cb1_w(1, 1);
	via_0_ca1_w(1, 0);

	via_config(1, &user_via);
	via_reset();
}


/* RESET
 *
 * Pushbutton switch S1 initiates RESET of the AIM65 hardware and software.
 * Timer Z4 holds the RES low for at least 15 ms from the time the pushbutton
 * is released. RES is routed to the R6502 CPU, the Monitor R6522 (Z32), the
 * Monitor R6532 RIOT (Z33), the user R6522 VIA (Z1), and the display R6520 PIA
 * (U1). To initiate the device RESET function is also routed to the expansion
 * connector for off-board RESET functions. The Monitor performs a software
 * reset when the RES line goes high.
 */

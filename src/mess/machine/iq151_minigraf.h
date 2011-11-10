#pragma once

#ifndef __IQ151_MINIGRAF_H__
#define __IQ151_MINIGRAF_H__

#include "emu.h"
#include "machine/iq151cart.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> iq151_minigraf_device

class iq151_minigraf_device :
		public device_t,
		public device_iq151cart_interface
{
public:
	// construction/destruction
	iq151_minigraf_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual const rom_entry *device_rom_region() const;


protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_config_complete() { m_shortname = "iq151_minigraf"; }

	// iq151cart_interface overrides
	virtual void read(offs_t offset, UINT8 &data);
	virtual void io_write(offs_t offset, UINT8 data);

private:

	UINT8 *		m_rom;
};


// device type definition
extern const device_type IQ151_MINIGRAF;

#endif  /* __IQ151_MINIGRAF_H__ */

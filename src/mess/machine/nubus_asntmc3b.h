#pragma once

#ifndef __NUBUS_ASNTMC3B_H__
#define __NUBUS_ASNTMC3B_H__

#include "emu.h"
#include "machine/nubus.h"
#include "machine/dp8390.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> nubus_asntm3b_device

class nubus_asntm3b_device :
		public device_t,
		public device_nubus_card_interface,
		public device_slot_card_interface
{
public:
        // construction/destruction
        nubus_asntm3b_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
		nubus_asntm3b_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock);

		// optional information overrides
		virtual machine_config_constructor device_mconfig_additions() const;
		virtual const rom_entry *device_rom_region() const;

        void dp_irq_w(int state);
        DECLARE_READ8_MEMBER(dp_mem_read);
        DECLARE_WRITE8_MEMBER(dp_mem_write);

protected:
        // device-level overrides
        virtual void device_start();
        virtual void device_reset();

        DECLARE_READ8_MEMBER(asntm3b_ram_r);
        DECLARE_WRITE8_MEMBER(asntm3b_ram_w);
        DECLARE_READ32_MEMBER(en_r);
        DECLARE_WRITE32_MEMBER(en_w);

        required_device<dp8390_device> m_dp83902;

private:
        UINT8 m_ram[0x20000];
        UINT8 m_prom[16];
};


// device type definition
extern const device_type NUBUS_ASNTMC3B;

#endif  /* __NUBUS_ASNTMC3B_H__ */

#pragma once

#ifndef __ISA_COM_H__
#define __ISA_COM_H__

#include "emu.h"
#include "machine/isa.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> isa8_com_device

class isa8_com_device :
		public device_t,
		public device_isa8_card_interface,
		public device_slot_card_interface
{
public:
        // construction/destruction
        isa8_com_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
		isa8_com_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock);

		// optional information overrides
		virtual machine_config_constructor device_mconfig_additions() const;
		virtual ioport_constructor device_input_ports() const;
protected:
        // device-level overrides
        virtual void device_start();
        virtual void device_reset();

private:
        // internal state
};


// device type definition
extern const device_type ISA8_COM;

// ======================> isa8_com_at_device

class isa8_com_at_device :
		public isa8_com_device
{
public:
        // construction/destruction
        isa8_com_at_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

		// optional information overrides
		virtual machine_config_constructor device_mconfig_additions() const;
};


// device type definition
extern const device_type ISA8_COM_AT;

#endif  /* __ISA_COM_H__ */

/***************************************************************************

    Hitachi B(asic Master?) 16

    31/05/2011 Skeleton driver.

****************************************************************************/
#define ADDRESS_MAP_MODERN

#include "emu.h"
#include "cpu/i86/i86.h"
#include "video/mc6845.h"

#define VIDEO_START_MEMBER(name) void name::video_start()
#define SCREEN_UPDATE_MEMBER(name) bool name::screen_update(screen_device &screen, bitmap_t &bitmap, const rectangle &cliprect)


class b16_state : public driver_device
{
public:
	b16_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) { }

	UINT8 *m_char_rom;
	UINT16 *m_vram;
	UINT8 m_crtc_vreg[0x100],m_crtc_index;

	DECLARE_READ16_MEMBER(vblank_r);
	DECLARE_WRITE8_MEMBER(b16_pcg_w);
	DECLARE_WRITE8_MEMBER(b16_6845_address_w);
	DECLARE_WRITE8_MEMBER(b16_6845_data_w);

	virtual void video_start();
	virtual bool screen_update(screen_device &screen, bitmap_t &bitmap, const rectangle &cliprect);

	mc6845_device *m_mc6845;
};

#define mc6845_h_char_total 	(m_crtc_vreg[0])
#define mc6845_h_display		(m_crtc_vreg[1])
#define mc6845_h_sync_pos		(m_crtc_vreg[2])
#define mc6845_sync_width		(m_crtc_vreg[3])
#define mc6845_v_char_total		(m_crtc_vreg[4])
#define mc6845_v_total_adj		(m_crtc_vreg[5])
#define mc6845_v_display		(m_crtc_vreg[6])
#define mc6845_v_sync_pos		(m_crtc_vreg[7])
#define mc6845_mode_ctrl		(m_crtc_vreg[8])
#define mc6845_tile_height		(m_crtc_vreg[9]+1)
#define mc6845_cursor_y_start	(m_crtc_vreg[0x0a])
#define mc6845_cursor_y_end 	(m_crtc_vreg[0x0b])
#define mc6845_start_addr		(((m_crtc_vreg[0x0c]<<8) & 0x3f00) | (m_crtc_vreg[0x0d] & 0xff))
#define mc6845_cursor_addr  	(((m_crtc_vreg[0x0e]<<8) & 0x3f00) | (m_crtc_vreg[0x0f] & 0xff))
#define mc6845_light_pen_addr	(((m_crtc_vreg[0x10]<<8) & 0x3f00) | (m_crtc_vreg[0x11] & 0xff))
#define mc6845_update_addr  	(((m_crtc_vreg[0x12]<<8) & 0x3f00) | (m_crtc_vreg[0x13] & 0xff))


VIDEO_START_MEMBER( b16_state )
{
	// find memory regions
	m_char_rom = machine().region("pcg")->base();

	VIDEO_START_NAME(generic_bitmapped)(machine());
}


SCREEN_UPDATE_MEMBER( b16_state )
{
	b16_state *state = machine().driver_data<b16_state>();
	int x,y;
	int xi,yi;
	UINT8 *gfx_rom = machine().region("pcg")->base();

	for(y=0;y<mc6845_v_display;y++)
	{
		for(x=0;x<mc6845_h_display;x++)
		{
			int tile = state->m_vram[x+y*mc6845_h_display] & 0xff;
			int color = (state->m_vram[x+y*mc6845_h_display] & 0x700) >> 8;
			int pen;

			for(yi=0;yi<mc6845_tile_height;yi++)
			{
				for(xi=0;xi<8;xi++)
				{
					pen = (gfx_rom[tile*16+yi] >> (7-xi) & 1) ? color : 0;

					if(y*mc6845_tile_height < 400 && x*8+xi < 640) /* TODO: safety check */
						*BITMAP_ADDR16(&bitmap, y*mc6845_tile_height+yi, x*8+xi) = machine().pens[pen];
				}
			}
		}
	}

	return 0;
}

WRITE8_MEMBER( b16_state::b16_pcg_w )
{
	m_char_rom[offset] = data;

	gfx_element_mark_dirty(machine().gfx[0], offset >> 3);
}

static ADDRESS_MAP_START( b16_map, AS_PROGRAM, 16, b16_state)
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE( 0xa0000, 0xaffff ) AM_RAM // bitmap?
	AM_RANGE( 0xb0000, 0xb7fff ) AM_RAM AM_BASE(m_vram) // tvram
	AM_RANGE( 0xb8000, 0xbbfff ) AM_WRITE8(b16_pcg_w,0x00ff) // pcg
	AM_RANGE( 0xfc000, 0xfffff ) AM_ROM AM_REGION("ipl",0)
ADDRESS_MAP_END

READ16_MEMBER( b16_state::vblank_r )
{
	return input_port_read(machine(), "SYSTEM");
}

WRITE8_MEMBER( b16_state::b16_6845_address_w )
{
	m_crtc_index = data;
	m_mc6845->address_w(space,offset, data);
}

WRITE8_MEMBER( b16_state::b16_6845_data_w )
{
	m_crtc_vreg[m_crtc_index] = data;
	m_mc6845->register_w(space, offset, data);
}


static ADDRESS_MAP_START( b16_io, AS_IO, 16, b16_state)
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE( 0x20, 0x21 ) AM_WRITE8(b16_6845_address_w,0x00ff)
	AM_RANGE( 0x22, 0x23 ) AM_WRITE8(b16_6845_data_w,0x00ff)
	AM_RANGE( 0x80, 0x81 ) AM_READ(vblank_r) // TODO
ADDRESS_MAP_END


/* Input ports */
static INPUT_PORTS_START( b16 )
	PORT_START("SYSTEM")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_VBLANK )
INPUT_PORTS_END




/* F4 Character Displayer */
static const gfx_layout b16_charlayout =
{
	8, 16,					/* 8 x 16 characters */
	RGN_FRAC(1,1),					/* 128 characters */
	1,					/* 1 bits per pixel */
	{ 0 },					/* no bitplanes */
	/* x offsets */
	{ STEP8(0,1) },
	/* y offsets */
	{ STEP16(0,8) },
	8*16					/* every char takes 2 x 8 bytes */
};

static MACHINE_START(b16)
{
	b16_state *state = machine.driver_data<b16_state>();
	state->m_mc6845 = machine.device<mc6845_device>("crtc");
}

static MACHINE_RESET(b16)
{
}

static GFXDECODE_START( b16 )
	GFXDECODE_ENTRY( "pcg", 0x0000, b16_charlayout, 0, 1 )
GFXDECODE_END

static const mc6845_interface mc6845_intf =
{
	"screen",	/* screen we are acting on */
	8,			/* number of pixels per video memory address */
	NULL,		/* before pixel update callback */
	NULL,		/* row update callback */
	NULL,		/* after pixel update callback */
	DEVCB_NULL,	/* callback for display state changes */
	DEVCB_NULL,	/* callback for cursor state changes */
	DEVCB_NULL,	/* HSYNC callback */
	DEVCB_NULL,	/* VSYNC callback */
	NULL		/* update address callback */
};


static MACHINE_CONFIG_START( b16, b16_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu",I8086, XTAL_4MHz) //unknown xtal
	MCFG_CPU_PROGRAM_MAP(b16_map)
	MCFG_CPU_IO_MAP(b16_io)

	MCFG_MACHINE_START(b16)
	MCFG_MACHINE_RESET(b16)

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500)) /* not accurate */
	MCFG_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MCFG_SCREEN_SIZE(640, 400)
	MCFG_SCREEN_VISIBLE_AREA(0, 640-1, 0, 400-1)

	MCFG_MC6845_ADD("crtc", H46505, XTAL_14_31818MHz/5, mc6845_intf)	/* unknown clock, hand tuned to get ~60 fps */

	MCFG_GFXDECODE(b16)
	MCFG_PALETTE_LENGTH(8)
//	MCFG_PALETTE_INIT(black_and_white) // TODO

MACHINE_CONFIG_END

/* ROM definition */
ROM_START( b16 )
	ROM_REGION( 0x4000, "ipl", ROMREGION_ERASEFF )
	ROM_LOAD( "ipl.rom", 0x0000, 0x4000, CRC(7c1c93d5) SHA1(2a1e63a689c316ff836f21646166b38714a18e03) )

	ROM_REGION( 0x4000/2, "pcg", ROMREGION_ERASE00 )
ROM_END

/* Driver */

/*    YEAR  NAME    PARENT  COMPAT   MACHINE     INPUT    INIT    COMPANY           FULLNAME       FLAGS */
COMP( 1983, b16,  0,      0,       b16,      b16,   0,      "Hitachi",   "B16", GAME_NOT_WORKING | GAME_NO_SOUND)

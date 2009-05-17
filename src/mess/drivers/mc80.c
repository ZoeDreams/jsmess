/***************************************************************************

        MC-80.xx driver by Miodrag Milanovic

        15/05/2009 Initial implementation
        12/05/2009 Skeleton driver.

****************************************************************************/

#include "driver.h"
#include "cpu/z80/z80.h"
#include "includes/mc80.h"

static ADDRESS_MAP_START(mc8020_mem, ADDRESS_SPACE_PROGRAM, 8)
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x0000, 0x0bff) AM_ROM
	AM_RANGE(0x0c00, 0x0fff) AM_RAM AM_BASE(&mc8020_video_ram)// 1KB RAM ZRE
	AM_RANGE(0x2000, 0x5fff) AM_ROM	
	AM_RANGE(0x6000, 0xffff) AM_RAM
ADDRESS_MAP_END

static ADDRESS_MAP_START( mc8020_io , ADDRESS_SPACE_IO, 8)
	ADDRESS_MAP_UNMAP_HIGH
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0xf0, 0xf3) AM_DEVREADWRITE("z80ctc", z80ctc_r, z80ctc_w)	
	AM_RANGE(0xf4, 0xf7) AM_DEVREADWRITE("z80pio", z80pio_r, z80pio_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START(mc8030_mem, ADDRESS_SPACE_PROGRAM, 8)
	ADDRESS_MAP_UNMAP_HIGH
	//  ZRE 4 * 2KB 
	AM_RANGE(0x0000, 0x1fff) AM_ROM // ZRE ROM's 4 * 2716
	AM_RANGE(0x2000, 0x27ff) AM_ROM // SPE ROM's 2 * 2708
	AM_RANGE(0x2800, 0x3fff) AM_ROM // For extension
	AM_RANGE(0x4000, 0xbfff) AM_RAM // SPE RAM
	AM_RANGE(0xc000, 0xffff) AM_RAM // ZRE RAM
ADDRESS_MAP_END

static ADDRESS_MAP_START( mc8030_io , ADDRESS_SPACE_IO, 8)
	ADDRESS_MAP_UNMAP_HIGH
	//ADDRESS_MAP_GLOBAL_MASK(0xff)
	//AM_RANGE(0x80, 0x84) AM_MIRROR(0xff00) AM_DEVREADWRITE("zve_ctc", z80ctc_r, z80ctc_w)
	//AM_RANGE(0x84, 0x87) AM_MIRROR(0xff00) AM_DEVREADWRITE("zve_pio", z80pio_r, z80pio_w)
	AM_RANGE(0x8c, 0x8c) AM_MIRROR(0xff00) AM_WRITE(zve_write_protect_w)
	AM_RANGE(0xc0, 0xcf) AM_MIRROR(0xff00) AM_WRITE(vis_w) AM_MASK(0xffff)
	//AM_RANGE(0xd0, 0xd3) AM_MIRROR(0xff00) AM_DEVREADWRITE("asp_sio", z80ctc_r, z80ctc_w)
	AM_RANGE(0xd4, 0xd7) AM_MIRROR(0xff00) AM_DEVREADWRITE("asp_ctc", z80ctc_r, z80ctc_w)
	AM_RANGE(0xd8, 0xdb) AM_MIRROR(0xff00) AM_DEVREADWRITE("asp_pio", z80pio_r, z80pio_w)
	AM_RANGE(0xe0, 0xef) AM_MIRROR(0xff00) AM_WRITE(eprom_prog_w)
ADDRESS_MAP_END

/* Input ports */
INPUT_PORTS_START( mc80 )
INPUT_PORTS_END


static MACHINE_DRIVER_START( mc8020 )
    /* basic machine hardware */
    MDRV_CPU_ADD("maincpu",Z80, XTAL_2_4576MHz)
    MDRV_CPU_PROGRAM_MAP(mc8020_mem)
    MDRV_CPU_IO_MAP(mc8020_io)
    
    MDRV_MACHINE_RESET(mc8020)
    
    /* video hardware */
    MDRV_SCREEN_ADD("screen", RASTER)
    MDRV_SCREEN_REFRESH_RATE(50)
    MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500)) /* not accurate */
    MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
    MDRV_SCREEN_SIZE(32*6, 16*8)
    MDRV_SCREEN_VISIBLE_AREA(0, 32*6-1, 0, 16*8-1)
    MDRV_PALETTE_LENGTH(2)
    MDRV_PALETTE_INIT(black_and_white)

    MDRV_VIDEO_START(mc8020)
    MDRV_VIDEO_UPDATE(mc8020)
    
	MDRV_Z80PIO_ADD( "z80pio", mc8020_z80pio_intf )    
	MDRV_Z80CTC_ADD( "z80ctc", XTAL_2_4576MHz / 100, mc8020_ctc_intf )    
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( mc8030 )
    /* basic machine hardware */
    MDRV_CPU_ADD("maincpu",Z80, XTAL_2_4576MHz)
    MDRV_CPU_PROGRAM_MAP(mc8030_mem)
    MDRV_CPU_IO_MAP(mc8030_io)

    MDRV_MACHINE_RESET(mc8030)
    
    /* video hardware */
    MDRV_SCREEN_ADD("screen", RASTER)
    MDRV_SCREEN_REFRESH_RATE(50)
    MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500)) /* not accurate */
    MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
    MDRV_SCREEN_SIZE(512, 256)
    MDRV_SCREEN_VISIBLE_AREA(0, 512-1, 0, 256-1)
    MDRV_PALETTE_LENGTH(2)
    MDRV_PALETTE_INIT(black_and_white)

    MDRV_VIDEO_START(mc8030)
    MDRV_VIDEO_UPDATE(mc8030)
    
    MDRV_Z80PIO_ADD( "zve_pio", zve_z80pio_intf )	
    MDRV_Z80CTC_ADD( "zve_ctc", XTAL_2_4576MHz, zve_z80ctc_intf )
    
	MDRV_Z80PIO_ADD( "asp_pio", asp_z80pio_intf )    
	MDRV_Z80CTC_ADD( "asp_ctc", XTAL_2_4576MHz, asp_z80ctc_intf )
    MDRV_Z80SIO_ADD( "asp_sio", 4800, asp_z80sio_intf )    
MACHINE_DRIVER_END

static SYSTEM_CONFIG_START(mc80)
SYSTEM_CONFIG_END

/* ROM definition */
ROM_START( mc8020 )
   ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
   ROM_SYSTEM_BIOS(0, "ver1", "Version 1")   
   ROMX_LOAD( "s01.rom",     0x0000, 0x0400, CRC(0f1c1a62) SHA1(270c0a9e8e165658f3b09d40a3e8bb3dc1b88184), ROM_BIOS(1))
   ROMX_LOAD( "s02.rom",     0x0400, 0x0400, CRC(93b5811c) SHA1(8559d24072c9b5908a2627ff986d818308f51d59), ROM_BIOS(1))    
   ROMX_LOAD( "s03.rom",     0x0800, 0x0400, CRC(3d32c334) SHA1(56d3012595540d03054ad3c6795ed5d929581a04), ROM_BIOS(1))   
   ROMX_LOAD( "mo01_v2.rom", 0x2000, 0x0400, CRC(7e47201c) SHA1(db49afdc5c1fe4065a979c56cbdbd3c58f5d942f), ROM_BIOS(1))

   ROM_SYSTEM_BIOS(1, "ver2", "Version 2")   
   ROMX_LOAD( "s01.rom",    0x0000, 0x0400, CRC(0f1c1a62) SHA1(270c0a9e8e165658f3b09d40a3e8bb3dc1b88184), ROM_BIOS(2))
   ROMX_LOAD( "s02_v2.rom", 0x0400, 0x0400, CRC(dd26c90a) SHA1(1108c11362fa63d21110a3b17868c1854a318c09), ROM_BIOS(2))    
   ROMX_LOAD( "s03_v2.rom", 0x0800, 0x0400, CRC(5b64ee7b) SHA1(3b4cbfcb8e2dedcfd4a3680c81fe6ceb2211b275), ROM_BIOS(2))   
   ROMX_LOAD( "mo01.rom",   0x2000, 0x0400, CRC(c65a470f) SHA1(71325fed1a342149b5efc2234ecfc8adfff0a42d), ROM_BIOS(2))

   ROM_SYSTEM_BIOS(2, "ver3", "Version 3")   
   ROMX_LOAD( "s01.rom",    0x0000, 0x0400, CRC(0f1c1a62) SHA1(270c0a9e8e165658f3b09d40a3e8bb3dc1b88184), ROM_BIOS(3))
   ROMX_LOAD( "s02_v3.rom", 0x0400, 0x0400, CRC(40c7a694) SHA1(bcdf382e8dad9bb6e06d23ec018e9df55c8d8d0c), ROM_BIOS(3))    
   ROMX_LOAD( "s03.rom",    0x0800, 0x0400, CRC(3d32c334) SHA1(56d3012595540d03054ad3c6795ed5d929581a04), ROM_BIOS(3))   
   ROMX_LOAD( "mo01_v2.rom",0x2000, 0x0400, CRC(7e47201c) SHA1(db49afdc5c1fe4065a979c56cbdbd3c58f5d942f), ROM_BIOS(3))    
   
   // m02 doesn't exist on board
   ROM_LOAD( "mo03.rom", 0x2800, 0x0400, CRC(29685056) SHA1(39e77658fb7af5a28112341f0893e007d73c1b7a))
   ROM_LOAD( "mo04.rom", 0x2c00, 0x0400, CRC(fd315b73) SHA1(cfb943ec8c884a9b92562d05f92faf06fe42ad68))
   ROM_LOAD( "mo05.rom", 0x3000, 0x0400, CRC(453d6370) SHA1(d96f0849a2da958d7e92a31667178ad140719477))
   ROM_LOAD( "mo06.rom", 0x3400, 0x0400, CRC(6357aba5) SHA1(a4867766f6e14e9fe66f22a6839f17c01058c8af))
   ROM_LOAD( "mo07.rom", 0x3800, 0x0400, CRC(a1eb6021) SHA1(b05b63f02de89f065f337bbe54c5b48244e3a4ba))
   ROM_LOAD( "mo08.rom", 0x3c00, 0x0400, CRC(8221cc32) SHA1(65e0ee4241d39d138205c88374b3bcd127e21511))
   ROM_LOAD( "mo09.rom", 0x4000, 0x0400, CRC(7ad5944d) SHA1(ef2781b114277a09ce0cf2e7decfdb7c48a693e3))
   ROM_LOAD( "mo10.rom", 0x4400, 0x0400, CRC(11de8c76) SHA1(b384d22506ff7e3e28bd2dcc33b7a69617eeb52a))
   ROM_LOAD( "mo11.rom", 0x4800, 0x0400, CRC(370cc672) SHA1(133f6e8bfd4e1ca2e9e0a8e2342084419f895e3c))
   ROM_LOAD( "mo12.rom", 0x4c00, 0x0400, CRC(a3838f2b) SHA1(e3602943700bf5068117946bf86f052f5c587169))
   ROM_LOAD( "mo13.rom", 0x5000, 0x0400, CRC(88b61d12) SHA1(00dd4452b4d4191e589ab58ba924ed21b10f323b))
   ROM_LOAD( "mo14.rom", 0x5400, 0x0400, CRC(2168da19) SHA1(c1ce1263167067d8be0a90604d9c29b7379a0545))
   ROM_LOAD( "mo15.rom", 0x5800, 0x0400, CRC(e32f54c4) SHA1(c3d9ca2204e7adbc625cf96031acb8c1df0447c7))
   ROM_LOAD( "mo16.rom", 0x5c00, 0x0400, CRC(403be935) SHA1(4e74355a78ab090ce180437156fed8e4a1d1c787))
ROM_END

ROM_START( mc8030 )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "zve_1.rom", 0x0000, 0x0800, CRC(31ec0159) SHA1(a97ea9eb733c462e77d625a7942134e45d911c0a))
	ROM_LOAD( "zve_2.rom", 0x0800, 0x0800, CRC(5104983d) SHA1(7516274904042f4fc6813aa8b2a75c0a64f9b937))
	ROM_LOAD( "zve_3.rom", 0x1000, 0x0800, CRC(4bcfd727) SHA1(d296e587098e70270ad60db8edaa685af368b849))
	ROM_LOAD( "zve_4.rom", 0x1800, 0x0800, CRC(f949ae43) SHA1(68c324cf5578497db7ae65da5695fcb30493f612))
	ROM_LOAD( "spe_1.rom", 0x2000, 0x0400, CRC(826F609C) SHA1(e77ff6c180f5a6d7756d076173ae264a0e26f066))
	ROM_LOAD( "spe_2.rom", 0x2400, 0x0400, CRC(98320040) SHA1(6baf87e196f1ccdf44912deafa6042becbfb0679))
ROM_END

/* Driver */

/*    YEAR  NAME    PARENT  COMPAT   MACHINE    INPUT    INIT    CONFIG COMPANY   FULLNAME       FLAGS */
COMP( ????, mc8020, 0,       0, 		mc8020, mc80, 	 0,  	  mc80,  	 "VEB Elektronik Gera",   "MC-80.21/22",		GAME_NOT_WORKING)
COMP( ????, mc8030, mc8020,  0, 		mc8030,	mc80, 	 0,  	  mc80,  	 "VEB Elektronik Gera",   "MC-80.30/31",		GAME_NOT_WORKING)


#ifndef PS2AUTOTESTS_GS_GSREGS_H
#define PS2AUTOTESTS_GS_GSREGS_H

#include <common-ee.h>

namespace GS
{
	enum RegCSRBits {
		CSR_SIGNAL = 1 << 0,
		CSR_RESET  = 1 << 9,
	};
	
	enum REGS {
		REG_PRIM       = 0x00,
		REG_RGBAQ      = 0x01,
		REG_XYZ2       = 0x05,
		REG_XYOFFSET_1 = 0x18,
		REG_XYOFFSET_2 = 0x19,
		REG_PRMODECONT = 0x1A,
		REG_SCISSOR_1  = 0x40,
		REG_SCISSOR_2  = 0x41,
		REG_FRAME_1    = 0x4C,
		REG_FRAME_2    = 0x4D,
		REG_ZBUF_1     = 0x4E,
		REG_ZBUF_2     = 0x4F,
		REG_BITBLTBUF  = 0x50,
		REG_TRXPOS     = 0x51,
		REG_TRXREG     = 0x52,
		REG_TRXDIR     = 0x53,
		REG_SIGNAL     = 0x60,
		REG_LABEL      = 0x62,
	};

	enum PRIMTYPE {
		PRIM_POINT         = 0,
		PRIM_LINE          = 1,
		PRIM_LINESTRIP     = 2,
		PRIM_TRIANGLE      = 3,
		PRIM_TRIANGLESTRIP = 4,
		PRIM_TRIANGLEFAN   = 5,
		PRIM_SPRITE        = 6,
		PRIM_INVALID       = 7
	};
	
	enum PSM {
		PSMCT32  = 0x00,
		PSMCT24  = 0x01,
		PSMCT16  = 0x02,
		PSMCT16S = 0x0A,
		PSMT8    = 0x13,
		PSMT4    = 0x14,
		PSMT8H   = 0x1B,
		PSMT4HL  = 0x24,
		PSMT4HH  = 0x2C,
		PSMZ32   = 0x30,
		PSMZ24   = 0x31,
		PSMZ16   = 0x32,
		PSMZ16S  = 0x3A
	};
	
	enum TRXDIR_XDIR {
		TRXDIR_XDIR_HOST_TO_LOCAL  = 0,
		TRXDIR_XDIR_LOCAL_TO_HOST  = 1,
		TRXDIR_XDIR_LOCAL_TO_LOCAL = 2,
		TRXDIR_XDIR_INACTIVE       = 3
	};
	
	u64 PRIM(u8 prim, u8 iip, u8 tme, u8 fge, u8 abe, u8 aa1, u8 fst, u8 ctxt, u8 fix);
	u64 RGBAQ(u8 r, u8 g, u8 b, u8 a, float q);
	u64 XYZ2(u16 x, u16 y, u32 z);
	u64 XYOFFSET(u16 ofsx, u16 ofsy);
	u64 PRMODECONT(u8 ac);
	u64 SCISSOR(u16 scax0, u16 scax1, u16 scay0, u16 scay1);
	u64 FRAME(u16 fbp, u8 fbw, u8 psm, u32 fbmsk);
	u64 ZBUF(u16 zbp, u8 psm, u8 zmsk);
	u64 BITBLTBUF(u16 sbp, u8 sbw, u8 spsm, u16 dbp, u8 dbw, u8 dpsm);
	u64 TRXPOS(u16 ssax, u16 ssay, u16 dsax, u16 dsay, u8 dir);
	u64 TRXREG(u16 rrw, u16 rrh);
	u64 TRXDIR(u8 xdir);
	u64 SIGNAL(u32 id, u32 idmsk);
	u64 LABEL(u32 id, u32 idmsk);
	
	u64 PMODE(u8 en1, u8 en2, u8 crtmd, u8 mmod, u8 amod, u8 slbg, u8 alp);
	u64 DISPFB(u16 fbp, u8 fbw, u8 psm, u16 dbx, u16 dby);
	u64 DISPLAY(u16 dx, u16 dy, u8 magh, u8 magv, u16 dw, u16 dh);
	
	static volatile u64 *const R_PMODE = (volatile u64 *)0x12000000;
	static volatile u64 *const R_DISPFB1 = (volatile u64 *)0x12000070;
	static volatile u64 *const R_DISPLAY1 = (volatile u64 *)0x12000080;
	static volatile u64 *const R_DISPFB2 = (volatile u64 *)0x12000090;
	static volatile u64 *const R_DISPLAY2 = (volatile u64 *)0x120000A0;
	static volatile u64 *const R_CSR = (volatile u64 *)0x12001000;
	static volatile u64 *const R_BUSDIR = (volatile u64 *)0x12001040;
	static volatile u64 *const R_SIGLBLID = (volatile u64 *)0x12001080;
}

#endif

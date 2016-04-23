#include "gsregs.h"

namespace GS {
	u64 PRIM(u8 prim, u8 iip, u8 tme, u8 fge, u8 abe, u8 aa1, u8 fst, u8 ctxt, u8 fix) {
		return ((u64)prim) | ((u64)iip << 3) | ((u64)tme << 4) | ((u64)fge << 5) | ((u64)abe << 6) | ((u64)aa1 << 7) | 
		       ((u64)fst << 8) | ((u64)ctxt << 9) | ((u64)fix << 10);
	}
	
	u64 RGBAQ(u8 r, u8 g, u8 b, u8 a, float q) {
		u32 qint = *(u32*)&q;
		return ((u64)r) | ((u64)g << 8) | ((u64)b << 16) | ((u64)a << 24) | ((u64)qint << 32);
	}
	
	u64 RGBAQ(u32 rgba, float q) {
		u32 qint = *(u32*)&q;
		return ((u64)rgba) | ((u64)qint << 32);
	}
	
	u64 XYZ2(u16 x, u16 y, u32 z) {
		return ((u64)x) | ((u64)y << 16) | ((u64)z << 32);
	}
	
	u64 XYOFFSET(u16 ofsx, u16 ofsy) {
		return ((u64)ofsx) | ((u64)ofsy << 32);
	}
	
	u64 PRMODECONT(u8 ac) {
		return ((u64)ac);
	}
	
	u64 SCISSOR(u16 scax0, u16 scax1, u16 scay0, u16 scay1) {
		return ((u64)scax0) | ((u64)scax1 << 16) | ((u64)scay0 << 32) | ((u64)scay1 << 48);
	}
	
	u64 ALPHA(u8 a, u8 b, u8 c, u8 d, u8 fix) {
		return ((u64)a) | ((u64)b << 2) | ((u64)c << 4) | ((u64)d << 6) | ((u64)fix << 32);
	}
	
	u64 COLCLAMP(u8 clamp) {
		return ((u64)clamp);
	}
	
	u64 FRAME(u16 fbp, u8 fbw, u8 psm, u32 fbmsk) {
		return ((u64)fbp) | ((u64)fbw << 16) | ((u64)psm << 24) | ((u64)fbmsk << 32);
	}
	
	u64 ZBUF(u16 zbp, u8 psm, u8 zmsk) {
		return ((u64)zbp) | ((u64)psm << 24) | ((u64)zmsk << 32);
	}
	
	u64 BITBLTBUF(u16 sbp, u8 sbw, u8 spsm, u16 dbp, u8 dbw, u8 dpsm) {
		return ((u64)sbp) | ((u64)sbw << 16) | ((u64)spsm << 24) | ((u64)dbp << 32) | ((u64)dbw << 48) | ((u64)dpsm << 56);
	}

	u64 TRXPOS(u16 ssax, u16 ssay, u16 dsax, u16 dsay, u8 dir) {
		return ((u64)ssax) | ((u64)ssay << 16) | ((u64)dsax << 32) | ((u64)dsay << 48) | ((u64)dir << 59);
	}
	
	u64 TRXREG(u16 rrw, u16 rrh) {
		return ((u64)rrw) | ((u64)rrh << 32);
	}
	
	u64 TRXDIR(u8 xdir) {
		return ((u64)xdir);
	}
	
	u64 SIGNAL(u32 id, u32 idmsk) {
		return ((u64)id) | ((u64)idmsk << 32);
	}
	
	u64 LABEL(u32 id, u32 idmsk) {
		return ((u64)id) | ((u64)idmsk << 32);
	}
	
	u64 PMODE(u8 en1, u8 en2, u8 crtmd, u8 mmod, u8 amod, u8 slbg, u8 alp) {
		return ((u64)en1) | ((u64)en2 << 1) | ((u64)crtmd << 2) | ((u64)mmod << 5) | ((u64)amod << 6) | ((u64)slbg << 7) | ((u64)alp << 8);
	}
	
	u64 DISPFB(u16 fbp, u8 fbw, u8 psm, u16 dbx, u16 dby) {
		return ((u64)fbp) | ((u64)fbw << 9) | ((u64)psm << 15) | ((u64)dbx << 32) | ((u64)dby << 43);
	}
	
	u64 DISPLAY(u16 dx, u16 dy, u8 magh, u8 magv, u16 dw, u16 dh) {
		return ((u64)dx) | ((u64)dy << 12) | ((u64)magh << 23) | ((u64)magv << 27) | ((u64)dw << 32) | ((u64)dh << 44);
	}
}

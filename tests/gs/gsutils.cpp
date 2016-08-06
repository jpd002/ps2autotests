#include <common-ee.h>
#include <assert.h>
#include <string.h>
#include "../dma/dmaregs.h"
#include "../dma/dmasend.h"
#include "../dma/vif/vifregs.h"
#include "gsregs.h"
#include "gsutils.h"
#include "emit_giftag.h"

void initGs(u32 frameAddress, u32 frameWidth, u32 frameHeight, u32 framePsm) {
	assert((frameAddress & (2048 - 1)) == 0);
	assert((frameWidth & (64 - 1)) == 0);
	
	*GS::R_CSR = GS::CSR_RESET;
	*GS::R_CSR = 0;
	SetGsCrt(1, 2, 0);

	static const u32 videoModeX = 636;
	static const u32 videoModeY = 63;
	static const u32 videoModeWidth = 2560;
	static const u32 videoModeHeight = 224;
	
	u32 dx = videoModeX;
	u32 dy = videoModeY;
	u32 dw = videoModeWidth;
	u32 dh = videoModeHeight;
	u32 magh = dw / frameWidth;
	u32 magv = dh / frameHeight;
	if(magh < 1) magh = 1;
	if(magv < 1) magv = 1;
	
	*GS::R_DISPFB1 = GS::DISPFB(frameAddress / 2048, frameWidth / 64, framePsm, 0, 0);
	*GS::R_DISPLAY1 = GS::DISPLAY(dx, dy, magh - 1, magv - 1, dw - 1, frameHeight - 1);
	*GS::R_PMODE = GS::PMODE(1, 0, 1, 0, 0, 0, 0);
}

u32 getColor(u32 frameAddress, u32 frameWidth, u32 framePsm) {
	assert((frameAddress & (64 - 1)) == 0);
	assert((frameWidth & (64 - 1)) == 0);
	
	const u32 width = 32;
	const u32 height = 32;
	
	GIF::Tag tag;
	tag.SetLoops(4);
	tag.SetEop();
	tag.SetRegDescs(GIF::REG_AD);
	
	GIF::PackedPacket packet(5 * 0x10);
	packet.WriteTag(tag);
	packet.AD(GS::REG_BITBLTBUF, GS::BITBLTBUF(frameAddress / 256, frameWidth / 64, framePsm, 0, 0, 0));
	packet.AD(GS::REG_TRXPOS,    GS::TRXPOS(0, 0, 0, 0, 0));
	packet.AD(GS::REG_TRXREG,    GS::TRXREG(width, height));
	packet.AD(GS::REG_TRXDIR,    GS::TRXDIR(GS::TRXDIR_XDIR_LOCAL_TO_HOST));
	DMA::SendSimple(DMA::D2, packet.Raw(), packet.Size());
	
	*GS::R_BUSDIR = 1;
	VIF::VIF1->stat = VIF::STAT_FDR;
	u32 size = width * height * 4;
	u32* imageData = (u32 *)memalign(128, size);
	memset(imageData, 0, size);
	DMA::ReceiveSimple(DMA::D1, imageData, size);
	*GS::R_BUSDIR = 0;
	VIF::VIF1->stat = (VIF::RegSTATBits)0;
	
	u32 result = imageData[0];
	free(imageData);
	return result;
}

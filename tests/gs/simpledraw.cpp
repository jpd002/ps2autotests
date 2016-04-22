#include <common-ee.h>
#include <stdio.h>
#include <string.h>
#include <kernel.h>
#include "../dma/dmaregs.h"
#include "../dma/dmasend.h"
#include "../dma/vif/vifregs.h"
#include "gsregs.h"
#include "emit_giftag.h"

#define FRAME_ADDRESS 0
#define FRAME_WIDTH   640
#define FRAME_HEIGHT  448
#define FRAME_PSM     GS::PSMCT32

void initGs() {
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
	u32 magh = dw / FRAME_WIDTH;
	u32 magv = dh / FRAME_HEIGHT;
	if(magh < 1) magh = 1;
	if(magv < 1) magv = 1;
	
	*GS::R_DISPFB1 = GS::DISPFB(FRAME_ADDRESS / 2048, FRAME_WIDTH / 64, FRAME_PSM, 0, 0);
	*GS::R_DISPLAY1 = GS::DISPLAY(dx, dy, magh - 1, magv - 1, dw - 1, FRAME_HEIGHT - 1);
	*GS::R_PMODE = GS::PMODE(1, 0, 1, 0, 0, 0, 0);
}

u32 getColor() {
	const u32 width = 32;
	const u32 height = 32;
	
	GIF::Tag tag;
	tag.SetLoops(4);
	tag.SetEop();
	tag.SetRegDescs(GIF::REG_AD);
	
	GIF::PackedPacket packet(64);
	packet.WriteTag(tag);
	packet.AD(GS::REG_BITBLTBUF, GS::BITBLTBUF(FRAME_ADDRESS / 64, FRAME_WIDTH / 64, FRAME_PSM, 0, 0, 0));
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

void doTest() {
	GIF::Tag tag;
	tag.SetLoops(13);
	tag.SetEop();
	tag.SetRegDescs(GIF::REG_AD);
	
	GIF::PackedPacket packet(64);
	packet.WriteTag(tag);
	
	//Setup render environment
	packet.AD(GS::REG_XYOFFSET_1, GS::XYOFFSET(0, 0));
	packet.AD(GS::REG_SCISSOR_1,  GS::SCISSOR(0, FRAME_WIDTH - 1, 0, FRAME_HEIGHT - 1));
	packet.AD(GS::REG_FRAME_1,    GS::FRAME(FRAME_ADDRESS / 2048, FRAME_WIDTH / 64, FRAME_PSM, 0));
	packet.AD(GS::REG_ZBUF_1,     GS::ZBUF(0, 0, 1));
	packet.AD(GS::REG_PRMODECONT, GS::PRMODECONT(1));
	
	//Clear background
	packet.AD(GS::REG_PRIM,       GS::PRIM(GS::PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));
	packet.AD(GS::REG_RGBAQ,      GS::RGBAQ(0xFF, 0x00, 0xFF, 0xFF, 0));
	packet.AD(GS::REG_XYZ2,       GS::XYZ2(0, 0, 0));
	packet.AD(GS::REG_XYZ2,       GS::XYZ2(FRAME_WIDTH * 16, FRAME_HEIGHT * 16, 0));
	
	//Draw test sprite
	packet.AD(GS::REG_PRIM,       GS::PRIM(GS::PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));
	packet.AD(GS::REG_RGBAQ,      GS::RGBAQ(0xCC, 0xDD, 0xEE, 0xAA, 0));
	packet.AD(GS::REG_XYZ2,       GS::XYZ2(0  * 16, 0  * 16, 0));
	packet.AD(GS::REG_XYZ2,       GS::XYZ2(32 * 16, 32 * 16, 0));
	
	DMA::SendSimple(DMA::D2, packet.Raw(), packet.Size());
	
	u32 result = getColor();
	printf("color: %08x\n", result);
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");
	initGs();
	doTest();
	printf("-- TEST END\n");
	return 0;
}

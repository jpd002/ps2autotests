#include <common-ee.h>
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

void render(u32 color1, u32 color2, u32 blendColor1, u32 blendColor2, u32 blendAlpha, u32 blendColor3) {
	GIF::Tag tag;
	tag.SetLoops(19);
	tag.SetEop();
	tag.SetRegDescs(GIF::REG_AD);
	
	GIF::PackedPacket packet(64);
	packet.WriteTag(tag);
	
	//Setup render environment
	packet.AD(GS::REG_XYOFFSET_1, GS::XYOFFSET(0, 0));
	packet.AD(GS::REG_SCISSOR_1,  GS::SCISSOR(0, FRAME_WIDTH - 1, 0, FRAME_HEIGHT - 1));
	packet.AD(GS::REG_FRAME_1,    GS::FRAME(FRAME_ADDRESS / 2048, FRAME_WIDTH / 64, FRAME_PSM, 0));
	packet.AD(GS::REG_ZBUF_1,     GS::ZBUF(0, 0, 1));
	packet.AD(GS::REG_ALPHA_1,    GS::ALPHA(blendColor1, blendColor2, blendAlpha, blendColor3, 0));
	packet.AD(GS::REG_COLCLAMP,   GS::COLCLAMP(1));
	packet.AD(GS::REG_PRMODECONT, GS::PRMODECONT(1));
	
	//Clear background
	packet.AD(GS::REG_PRIM,       GS::PRIM(GS::PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));
	packet.AD(GS::REG_RGBAQ,      GS::RGBAQ(0x00, 0x00, 0x00, 0x00, 0));
	packet.AD(GS::REG_XYZ2,       GS::XYZ2(0, 0, 0));
	packet.AD(GS::REG_XYZ2,       GS::XYZ2(FRAME_WIDTH * 16, FRAME_HEIGHT * 16, 0));
	
	//Draw first sprite
	packet.AD(GS::REG_PRIM,       GS::PRIM(GS::PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));
	packet.AD(GS::REG_RGBAQ,      GS::RGBAQ(color1, 0));
	packet.AD(GS::REG_XYZ2,       GS::XYZ2(0  * 16, 0  * 16, 0));
	packet.AD(GS::REG_XYZ2,       GS::XYZ2(32 * 16, 32 * 16, 0));
	
	//Draw second sprite (blending enabled)
	packet.AD(GS::REG_PRIM,       GS::PRIM(GS::PRIM_SPRITE, 0, 0, 0, 1, 0, 0, 0, 0));
	packet.AD(GS::REG_RGBAQ,      GS::RGBAQ(color2, 0));
	packet.AD(GS::REG_XYZ2,       GS::XYZ2(0  * 16, 0  * 16, 0));
	packet.AD(GS::REG_XYZ2,       GS::XYZ2(32 * 16, 32 * 16, 0));
	
	DMA::SendSimple(DMA::D2, packet.Raw(), packet.Size());
}

void doTest(u32 blendColor1, u32 blendColor2, u32 blendAlpha, u32 blendColor3) {
	static const u32 colorCombos[] = {
		0xFF00FF00, 0xFFFF00FF,
		0x8000FF00, 0x80FF00FF,
		0x4000FF00, 0x40FF00FF,

		0xFF204020, 0xFF402040,
		0x80204020, 0x80402040,
		0x40204020, 0x40402040
	};
	
	//TODO: Use ARRAY_SIZE
	static const u32 comboCount = sizeof(colorCombos) / sizeof(u32);
	for(u32 i = 0; i < comboCount; i += 2) {
		u32 color1 = colorCombos[i + 0];
		u32 color2 = colorCombos[i + 1];
		render(color1, color2, blendColor1, blendColor2, blendAlpha, blendColor3);
		u32 result = getColor();
		printf("  src: %08x, dst: %08x => %08x\n", color2, color1, result & 0x00FFFFFF);
	}
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");
	initGs();
	
	static const char* abdParamName[3] = {
		"Cs",
		"Cd",
		"0"
	};
	
	static const char* cParamName[3] = {
		"As",
		"Ad",
		"FIX"
	};
	
	for(u32 a = 0; a < 3; a++) {
		for(u32 b = 0; b < 3; b++) {
			for(u32 c = 0; c < 2; c++) {
				for(u32 d = 0; d < 3; d++) {
					printf("%d%d%d%d: (%s - %s) * %s + %s\n", a, b, c, d, 
						abdParamName[a], abdParamName[b], cParamName[c], abdParamName[d]);
					doTest(a, b, c, d);
				}
			}
		}
	}
	
	printf("-- TEST END\n");
	return 0;
}

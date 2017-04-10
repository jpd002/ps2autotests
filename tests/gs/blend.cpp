#include <common-ee.h>
#include <stdio.h>
#include "../dma/dmaregs.h"
#include "../dma/dmasend.h"
#include "gsregs.h"
#include "gsutils.h"
#include "emit_giftag.h"

#define FRAME_ADDRESS 0
#define FRAME_WIDTH   640
#define FRAME_HEIGHT  448
#define FRAME_PSM     GS::PSMCT32

void render(u32 color1, u32 color2, u32 blendColor1, u32 blendColor2, u32 blendAlpha, u32 blendColor3, u8 fix) {
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
	packet.AD(GS::REG_ALPHA_1,    GS::ALPHA(blendColor1, blendColor2, blendAlpha, blendColor3, fix));
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
		0xFF00FF00, 0xFFFF00FF, 0xFF,
		0x8000FF00, 0x80FF00FF, 0x80,
		0x4000FF00, 0x40FF00FF, 0x40,

		0xFF204020, 0xFF402040, 0xFF,
		0x80204020, 0x80402040, 0x80,
		0x40204020, 0x40402040, 0x40
	};
	
	//TODO: Use ARRAY_SIZE
	static const u32 comboCount = sizeof(colorCombos) / sizeof(u32);
	for(u32 i = 0; i < comboCount; i += 3) {
		u32 color1 = colorCombos[i + 0];
		u32 color2 = colorCombos[i + 1];
		u8 fix = static_cast<u8>(colorCombos[i + 2]);
		render(color1, color2, blendColor1, blendColor2, blendAlpha, blendColor3, fix);
		u32 result = getColor(FRAME_ADDRESS, FRAME_WIDTH, FRAME_PSM);
		printf("  src: %08x, dst: %08x, fix: %02x => %08x\n", 
			color2, color1, fix, result & 0x00FFFFFF);
	}
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");
	initGs(FRAME_ADDRESS, FRAME_WIDTH, FRAME_HEIGHT, FRAME_PSM);
	
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
			for(u32 c = 0; c < 3; c++) {
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

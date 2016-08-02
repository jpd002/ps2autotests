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
	
	u32 result = getColor(FRAME_ADDRESS, FRAME_WIDTH, FRAME_PSM);
	printf("color: %08x\n", result);
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");
	initGs(FRAME_ADDRESS, FRAME_WIDTH, FRAME_HEIGHT, FRAME_PSM);
	doTest();
	printf("-- TEST END\n");
	return 0;
}

#include <common-ee.h>
#include <stdio.h>
#include <assert.h>
#include "../dma/dmaregs.h"
#include "../dma/dmasend.h"
#include "gsregs.h"
#include "gsutils.h"
#include "emit_giftag.h"

#define FRAME_ADDRESS 0
#define FRAME_WIDTH   640
#define FRAME_HEIGHT  448
#define FRAME_PSM     GS::PSMCT32

static u16 ftoi4(float value) {
	assert(value >= 0);
	value *= 16;
	return static_cast<u16>(value);
}

void doTest() {
	GIF::Tag tag;
	tag.SetLoops(19);
	tag.SetEop();
	tag.SetRegDescs(GIF::REG_AD);
	
	GIF::PackedPacket packet((19 + 1) * 0x10);
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
	packet.AD(GS::REG_XYZ2,       GS::XYZ2(ftoi4(0), ftoi4(0), 0));
	packet.AD(GS::REG_XYZ2,       GS::XYZ2(ftoi4(FRAME_WIDTH), ftoi4(FRAME_HEIGHT), 0));
	
	//Draw test sprite (Flat shaded)
	packet.AD(GS::REG_PRIM,       GS::PRIM(GS::PRIM_SPRITE, 0, 0, 0, 0, 0, 0, 0, 0));
	packet.AD(GS::REG_RGBAQ,      GS::RGBAQ(0xCC, 0xDD, 0xEE, 0xAA, 0));
	packet.AD(GS::REG_XYZ2,       GS::XYZ2(ftoi4( 0), ftoi4( 0), 0));
	packet.AD(GS::REG_RGBAQ,      GS::RGBAQ(0x11, 0x22, 0x33, 0xAA, 0));
	packet.AD(GS::REG_XYZ2,       GS::XYZ2(ftoi4(32), ftoi4(32), 0));
	
	//Draw test sprite (Gouraud shaded)
	packet.AD(GS::REG_PRIM,       GS::PRIM(GS::PRIM_SPRITE, 1, 0, 0, 0, 0, 0, 0, 0));
	packet.AD(GS::REG_RGBAQ,      GS::RGBAQ(0xCC, 0xDD, 0xEE, 0xAA, 0));
	packet.AD(GS::REG_XYZ2,       GS::XYZ2(ftoi4(64), ftoi4( 0), 0));
	packet.AD(GS::REG_RGBAQ,      GS::RGBAQ(0x11, 0x22, 0x33, 0xAA, 0));
	packet.AD(GS::REG_XYZ2,       GS::XYZ2(ftoi4(96), ftoi4(32), 0));

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

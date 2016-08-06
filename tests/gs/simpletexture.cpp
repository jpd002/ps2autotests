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

#define TEXTURE_ADDRESS (FRAME_WIDTH * FRAME_HEIGHT * 4)
#define TEXTURE_WIDTH   256
#define TEXTURE_HEIGHT  256
#define TEXTURE_PSM     GS::PSMCT32

void uploadTexture() {
	u32 textureSize = (TEXTURE_WIDTH * TEXTURE_HEIGHT * 4);
	u32 textureQwordSize = textureSize / 0x10;
	
	u32* textureData = reinterpret_cast<u32*>(memalign(16, textureSize));
	for(u32 y = 0; y < TEXTURE_HEIGHT; y++) {
		for(u32 x = 0; x < TEXTURE_WIDTH; x++) {
			u32 color = (((x & 16) ^ (y & 16)) != 0) ? 0xFFFFFFFF : 0xFF808080;
			textureData[x + (y * TEXTURE_WIDTH)] = color;
		}
	}
	
	//Prepare transfer
	{
		GIF::Tag tag;
		tag.SetLoops(4);
		tag.SetEop();
		tag.SetRegDescs(GIF::REG_AD);
		
		GIF::PackedPacket packet(5 * 0x10);
		packet.WriteTag(tag);
		packet.AD(GS::REG_BITBLTBUF, GS::BITBLTBUF(0, 0, 0, TEXTURE_ADDRESS / 256, TEXTURE_WIDTH / 64, TEXTURE_PSM));
		packet.AD(GS::REG_TRXPOS,    GS::TRXPOS(0, 0, 0, 0, 0));
		packet.AD(GS::REG_TRXREG,    GS::TRXREG(TEXTURE_WIDTH, TEXTURE_HEIGHT));
		packet.AD(GS::REG_TRXDIR,    GS::TRXDIR(GS::TRXDIR_XDIR_HOST_TO_LOCAL));
		
		DMA::SendSimple(DMA::D2, packet.Raw(), packet.Size());
	}
	
	//Send texture
	{
		GIF::Tag tag;
		tag.SetLoops(textureQwordSize);
		tag.SetEop();
		tag.SetFormat(GIF::FORMAT_IMAGE);
		
		GIF::Packet packet(1 * 0x10);
		packet.WriteTag(tag);
		
		DMA::SendSimple(DMA::D2, packet.Raw(), packet.Size());
		DMA::SendSimple(DMA::D2, textureData, textureSize);
	}
	
	free(textureData);
}

void doTest() {
	GIF::Tag tag;
	tag.SetLoops(12);
	tag.SetEop();
	tag.SetRegDescs(GIF::REG_AD);
	
	GIF::PackedPacket packet(12 * 0x10);
	packet.WriteTag(tag);
	
	//Setup render environment
	packet.AD(GS::REG_XYOFFSET_1, GS::XYOFFSET(0, 0));
	packet.AD(GS::REG_SCISSOR_1,  GS::SCISSOR(0, FRAME_WIDTH - 1, 0, FRAME_HEIGHT - 1));
	packet.AD(GS::REG_FRAME_1,    GS::FRAME(FRAME_ADDRESS / 2048, FRAME_WIDTH / 64, FRAME_PSM, 0));
	packet.AD(GS::REG_ZBUF_1,     GS::ZBUF(0, 0, 1));
	packet.AD(GS::REG_PRMODECONT, GS::PRMODECONT(1));
	
	//Setup texture
	packet.AD(GS::REG_TEX0_1,     GS::TEX0(TEXTURE_ADDRESS / 256, TEXTURE_WIDTH / 64, TEXTURE_PSM, 8, 8, 0, 0, 0, 0, 0, 0, 0));
	
	//Draw test sprite
	packet.AD(GS::REG_PRIM,       GS::PRIM(GS::PRIM_SPRITE, 0, 1, 0, 0, 0, 1, 0, 0));
	packet.AD(GS::REG_RGBAQ,      GS::RGBAQ(0x80, 0x80, 0x80, 0x80, 0));
	packet.AD(GS::REG_UV,         GS::UV(0, 0));
	packet.AD(GS::REG_XYZ2,       GS::XYZ2(0, 0, 0));
	packet.AD(GS::REG_UV,         GS::UV(TEXTURE_WIDTH * 16, TEXTURE_HEIGHT * 16));
	packet.AD(GS::REG_XYZ2,       GS::XYZ2(FRAME_WIDTH * 16, FRAME_HEIGHT * 16, 0));
	
	DMA::SendSimple(DMA::D2, packet.Raw(), packet.Size());
	
	u32 result = getColor(FRAME_ADDRESS, FRAME_WIDTH, FRAME_PSM);
	printf("color: %08x\n", result);
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");
	initGs(FRAME_ADDRESS, FRAME_WIDTH, FRAME_HEIGHT, FRAME_PSM);
	uploadTexture();
	doTest();
	printf("-- TEST END\n");
	return 0;
}

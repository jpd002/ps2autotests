#include <common-ee.h>
#include "../test_runner.h"

class BranchDelayTestRunner : public TestRunner {
public:
	BranchDelayTestRunner(int vu) : TestRunner(vu) {
	}
	
	void Perform1() {
		using namespace VU;

		printf("branch after multiple ops:\n");

		Reset();

		Wr(IADDIU(VI01, VI00, 0x03));
		Wr(IADDIU(VI02, VI00, 0x00));
		
		//Loop
		Label loopStart = L();
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(IADDIU(VI02, VI02, 0x01));
		Wr(IADDIU(VI02, VI02, 0x01));
		Wr(IADDIU(VI02, VI02, 0x01));
		Wr(IADDIU(VI02, VI02, 0x01));
		Wr(IADDIU(VI02, VI02, 0x01));
		Wr(IADDIU(VI02, VI02, 0x01));
		Wr(IADDIU(VI02, VI02, 0x01));
		IBNE(loopStart, VI01, VI02);
		
		Execute();
		
		printf("  VI01: "); PrintRegister(VI01, true);
		printf("  VI02: "); PrintRegister(VI02, true);
	}
	
	void Perform2() {
		using namespace VU;

		printf("branch without delay:\n");

		Reset();

		Wr(IADDIU(VI01, VI00, 0x10));
		Wr(IADDIU(VI02, VI00, 0x00));
		Wr(IADDIU(VI03, VI00, 0x00));
		
		//Loop
		Label loopStart = L();
		Wr(IADDIU(VI03, VI03, 0x01));
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(IADDIU(VI02, VI02, 0x01));
		IBNE(loopStart, VI01, VI02);
		
		Execute();
		
		printf("  VI01: "); PrintRegister(VI01, true);
		printf("  VI02: "); PrintRegister(VI02, true);
		printf("  VI03: "); PrintRegister(VI03, true);
	}

	void Perform3() {
		using namespace VU;

		printf("branch with delay:\n");

		Reset();

		Wr(IADDIU(VI01, VI00, 0x10));
		Wr(IADDIU(VI02, VI00, 0x00));
		Wr(IADDIU(VI03, VI00, 0x00));
		
		//Loop
		Label loopStart = L();
		Wr(IADDIU(VI03, VI03, 0x01));
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(IADDIU(VI02, VI02, 0x01));
		Wr(IADDIU(VI03, VI03, 0x01));
		IBNE(loopStart, VI01, VI02);
		
		Execute();
		
		printf("  VI01: "); PrintRegister(VI01, true);
		printf("  VI02: "); PrintRegister(VI02, true);
		printf("  VI03: "); PrintRegister(VI03, true);
	}
	
	void Perform4() {
		using namespace VU;

		printf("branch with integer op in delay slot:\n");

		Reset();

		Wr(IADDIU(VI01, VI00, 0xFB));
		Wr(IADDIU(VI02, VI00, 0x00));
		
		//Loop
		Label loopStart = L();
		Wr(IADDIU(VI02, VI02, 0x01));
		Wr(IADDIU(VI02, VI02, 0x01));
		Wr(IADDIU(VI02, VI02, 0x01));
		IBNE(loopStart, VI01, VI02,
			NOP(), IADDIU(VI02, VI02, 0x01));
		
		Execute();
		
		printf("  VI01: "); PrintRegister(VI01, true);
		printf("  VI02: "); PrintRegister(VI02, true);
	}
	
	typedef void (BranchDelayTestRunner::*PrepareInstructionPipelineTestFunction)();
	
	void PerformInstructionPipelineTest(const char* name, const PrepareInstructionPipelineTestFunction& prepareTest) {
		using namespace VU;
		
		printf("%s pipeline:\n", name);
		
		Reset();
		
		//Reset VI01
		Wr(IADDIU(VI01, VI00, 0x00));
		
		//Add in some NOPs to prevent chaining two integer instructions together
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		
		//Last instruction must modify VI01
		((*this).*(prepareTest))();
		
		//Will check if VI01 is not equal to zero
		//If branch is taken, it means branch used result of previous instruction
		Label labelNotEqualZero = IBNE(VI01, VI00);
		Label labelEqualZero = B();
		
		L(labelEqualZero);
		Wr(IADDIU(VI02, VI00, 0x00));
		Label labelDone = B();
		
		L(labelNotEqualZero);
		Wr(IADDIU(VI02, VI00, 0x01));
		
		L(labelDone);
		
		Execute();
		
		printf("  Instruction result: "); PrintRegister(VI01, true);
		printf("  Branch used result: "); PrintRegister(VI02, true);
	}
	
	void PrepareFMANDTest() {
		using namespace VU;
		
		Wr(ADD(DEST_XYZW, VF01, VF00, VF00), IADDIU(VI02, VI00, 0x02));
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		
		Wr(FMAND(VI01, VI02));
	}
	
	void PrepareIADDIUTest() {
		using namespace VU;
		Wr(IADDIU(VI01, VI00, 0xFF));
	}
	
	void PrepareILWTest() {
		using namespace VU;
		
		Wr(IADDIU(VI10, VI00, 0x10));
		Wr(ISW(DEST_X, VI10, VI00, 0x00));
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		
		Wr(ILW(DEST_X, VI01, VI00, 0x00));
	}
	
	void PrepareILWRTest() {
		using namespace VU;
		
		Wr(IADDIU(VI10, VI00, 0xF0));
		Wr(ISW(DEST_X, VI10, VI00, 0x00));
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		
		Wr(ILWR(DEST_X, VI01, VI00));
	}
	
	void PrepareISUBIUTest() {
		using namespace VU;
		Wr(ISUBIU(VI01, VI00, 0xFF));
	}
	
	void PrepareLQITest() {
		using namespace VU;
		Wr(LQI(DEST_XYZW, VF00, VI01));
	}
	
	void PrepareLQDTest() {
		using namespace VU;
		Wr(LQD(DEST_XYZW, VF00, VI01));
	}
	
	void PrepareMTIRTest() {
		using namespace VU;
		Wr(IADDIU(VI10, VI00, 0x23));
		Wr(MFIR(DEST_X, VF01, VI10));
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		
		Wr(MTIR(DEST_X, VI01, FIELD_X, VF01));
	}
	
	void PrepareSQITest() {
		using namespace VU;
		Wr(SQI(DEST_XYZW, VF00, VI01));
	}
	
	void PrepareSQDTest() {
		using namespace VU;
		Wr(SQD(DEST_XYZW, VF00, VI01));
	}
};

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	BranchDelayTestRunner runner(0);
	runner.Perform1();
	runner.Perform2();
	runner.Perform3();
	runner.Perform4();
	
	runner.PerformInstructionPipelineTest("FMAND",  &BranchDelayTestRunner::PrepareFMANDTest);
	runner.PerformInstructionPipelineTest("IADDIU", &BranchDelayTestRunner::PrepareIADDIUTest);
	runner.PerformInstructionPipelineTest("ILW",    &BranchDelayTestRunner::PrepareILWTest);
	runner.PerformInstructionPipelineTest("ILWR",   &BranchDelayTestRunner::PrepareILWRTest);
	runner.PerformInstructionPipelineTest("ISUBIU", &BranchDelayTestRunner::PrepareISUBIUTest);
	runner.PerformInstructionPipelineTest("LQI",    &BranchDelayTestRunner::PrepareLQITest);
	runner.PerformInstructionPipelineTest("LQD",    &BranchDelayTestRunner::PrepareLQDTest);
	runner.PerformInstructionPipelineTest("MTIR",   &BranchDelayTestRunner::PrepareMTIRTest);
	runner.PerformInstructionPipelineTest("SQI",    &BranchDelayTestRunner::PrepareSQITest);
	runner.PerformInstructionPipelineTest("SQD",    &BranchDelayTestRunner::PrepareSQDTest);
	
	//Instructions that still need testing:
	//BAL/JALR (probably hard to test and not very interesting)
	//FCAND/FCEQ/FCGET/FCOR
	//FMEQ/FMOR
	//FSAND/FSEQ/FSOR
	//IADDI
	//IAND
	//IOR
	//ISUB
	//XTOP/XITOP
	
	printf("-- TEST END\n");
	return 0;
}

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
	
	void Perform5() {
		using namespace VU;
		
		printf("branch with flag check op:\n");
		
		Reset();
		
		Wr(ADD(DEST_XYZW, VF01, VF00, VF00), IADDIU(VI01, VI00, 0x02));
		Wr(IADDIU(VI02, VI00, 0x00));
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		Wr(NOP());
		
		Wr(FMAND(VI02, VI01));
		Label labelNotZero = IBNE(VI02, VI00);
		Label labelZero = B();
		
		L(labelNotZero);
		Wr(IADDIU(VI01, VI00, 0x01));
		Label labelDone = B();
		
		L(labelZero);
		Wr(IADDIU(VI01, VI00, 0x00));
		
		L(labelDone);
		
		Execute();
		
		printf("  VI01: "); PrintRegister(VI01, true);
		printf("  VI02: "); PrintRegister(VI02, true);
	}
};

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	BranchDelayTestRunner runner(0);
	runner.Perform1();
	runner.Perform2();
	runner.Perform3();
	runner.Perform4();
	runner.Perform5();
	
	printf("-- TEST END\n");
	return 0;
}

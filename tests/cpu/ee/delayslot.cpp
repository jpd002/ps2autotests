#include <common-ee.h>

void __attribute__((noinline)) testBranch(u32 value1, u32 value2) {
	s32 counter1 = 0, counter2 = 0;

	asm volatile (
		".set    noreorder\n"
		"addiu   $t2, $0, 1\n"
		"beq     %2, %3, branch_de\n"
		"branch_fi:\n"
		"addiu   %0, %0, 1\n"
		"branch_de:\n"
		"addiu   %1, %1, 1\n"
		"bne     $t2, $0, branch_fi\n"
		"addiu   $t2, $t2, -1\n"
		: "+r"(counter1), "+r"(counter2) : "r"(value1), "r"(value2) : "t2"
	);

	printf("branch -> %s: %d, %d\n", (value1 == value2) ? "taken" : "not taken",
		counter1, counter2);
}

void __attribute__((noinline)) testBranchLikely(u32 value1, u32 value2) {
	s32 counter1 = 0, counter2 = 0;

	asm volatile (
		".set    noreorder\n"
		"addiu   $t2, $0, 1\n"
		"beql    %2, %3, branch_likely_de\n"
		"branch_likely_fi:\n"
		"addiu   %0, %0, 1\n"
		"branch_likely_de:\n"
		"addiu   %1, %1, 1\n"
		"bne     $t2, $0, branch_likely_fi\n"
		"addiu   $t2, $t2, -1\n"
		: "+r"(counter1), "+r"(counter2) : "r"(value1), "r"(value2) : "t2"
	);

	printf("branch likely -> %s: %d, %d\n", (value1 == value2) ? "taken" : "not taken",
		counter1, counter2);
}

int main(int argc, char *argv[]) {
	printf("-- TEST BEGIN\n");

	testBranch(0, 0);
	testBranch(0, 1);
	testBranchLikely(0, 0);
	testBranchLikely(0, 1);
	
	printf("-- TEST END\n");

	return 0;
}

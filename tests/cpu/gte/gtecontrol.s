.text
.globl    enableGte
.globl    disableGte

enableGte:
	li      $t0, 0x40000000
	mfc0    $t1, $12
	or      $t1, $t1, $t0
	mtc0    $t1, $12
	jr      $ra
	nop

disableGte:
	li      $t0, 0xBFFFFFFF
	mfc0    $t1, $12
	and     $t1, $t1, $t0
	mtc0    $t1, $12
	jr      $ra
	nop

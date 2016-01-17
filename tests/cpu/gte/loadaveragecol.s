.text
.globl  loadAverageCol

loadAverageCol:

arg_10          =  0x10

                lbu     $t0, 0($a0)
                lbu     $t1, 1($a0)
                lbu     $t2, 2($a0)
                mtc2    $a2, $8
                mtc2    $t0, $9
                mtc2    $t1, $10
                mtc2    $t2, $11
                nop
                cop2    0x190003D
                lbu     $t0, 0($a1)
                lbu     $t1, 1($a1)
                lbu     $t2, 2($a1)
                mfc2    $v0, $31
                mtc2    $a3, $8
                mtc2    $t0, $9
                mtc2    $t1, $10
                mtc2    $t2, $11
                li      $t3, 0xC
                cop2    0x1A0003E
                lw      $t5, arg_10($sp)
                mfc2    $t0, $25
                mfc2    $t1, $26
                mfc2    $t2, $27
                sra     $t0, $t3
                sra     $t1, $t3
                sra     $t2, $t3
                sb      $t0, 0($t5)
                sb      $t1, 1($t5)
                sb      $t2, 2($t5)
                jr      $ra
                nop

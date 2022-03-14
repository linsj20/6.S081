.globl write_matrix

.text
# ==============================================================================
# FUNCTION: Writes a matrix of integers into a binary file
# FILE FORMAT:
#   The first 8 bytes of the file will be two 4 byte ints representing the
#   numbers of rows and columns respectively. Every 4 bytes thereafter is an
#   element of the matrix in row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is the pointer to the start of the matrix in memory
#   a2 (int)   is the number of rows in the matrix
#   a3 (int)   is the number of columns in the matrix
# Returns:
#   None
# Exceptions:
#   - If you receive an fopen error or eof,
#     this function terminates the program with error code 27
#   - If you receive an fclose error or eof,
#     this function terminates the program with error code 28
#   - If you receive an fwrite error or eof,
#     this function terminates the program with error code 30
# ==============================================================================
write_matrix:

	# Prologue
	addi sp sp -32
    sw ra 0(sp)
	sw s0 4(sp)
    sw s1 8(sp)
    sw s2 12(sp)
    sw s3 16(sp)
    sw s4 20(sp)
	add s0 a0 x0
    add s1 a1 x0
    add s2 a2 x0
    add s3 a3 x0

	sw a2 24(sp)
	sw a3 28(sp)

	add a0 s0 x0
    addi a1 x0 1
	jal fopen
    add s4 a0 x0
	addi t0 x0 -1
    beq a0 t0 exit1
	
	add a0 s4 x0
    addi a1 sp 24
	addi a2 x0 2
    addi a3 x0 4
    jal ra fwrite
    addi t0 x0 2
    bne a0 t0 exit2
    
    add a0 s4 x0
    add a1 x0 s1
    mul a2 s2 s3
    addi a3 x0 4
    jal ra fwrite
    mul t0 s2 s3
    bne a0 t0 exit2
    
    add a0 s4 x0
    jal fclose
    bne a0 x0 exit3
    
	# Epilogue
    lw ra 0(sp)
	lw s0 4(sp)
    lw s1 8(sp)
    lw s2 12(sp)
    lw s3 16(sp)
    lw s4 20(sp)
	addi sp sp 32
	ret
exit1:
	li a0 27
    j exit
exit2:
	li a0 30
    j exit
exit3:
	li a0 28
    j exit
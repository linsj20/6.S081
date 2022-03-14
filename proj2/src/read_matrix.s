.globl read_matrix

.text
# ==============================================================================
# FUNCTION: Allocates memory and reads in a binary file as a matrix of integers
#
# FILE FORMAT:
#   The first 8 bytes are two 4 byte ints representing the # of rows and columns
#   in the matrix. Every 4 bytes afterwards is an element of the matrix in
#   row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is a pointer to an integer, we will set it to the number of rows
#   a2 (int*)  is a pointer to an integer, we will set it to the number of columns
# Returns:
#   a0 (int*)  is the pointer to the matrix in memory
# Exceptions:
#   - If malloc returns an error,
#     this function terminates the program with error code 26
#   - If you receive an fopen error or eof,
#     this function terminates the program with error code 27
#   - If you receive an fclose error or eof,
#     this function terminates the program with error code 28
#   - If you receive an fread error or eof,
#     this function terminates the program with error code 29
# ==============================================================================
read_matrix:

	# Prologue
	addi sp sp -28
    sw ra 0(sp)
    sw s0 4(sp)
    sw s1 8(sp)
    sw s2 12(sp)
	sw s3 16(sp)
    sw s4 20(sp)
    sw s5 24(sp)
    
	add s0 a0 x0
    add s1 a1 x0
    add s2 a2 x0   
    
    add a0 s0 x0
    add a1 x0 x0
    jal ra fopen
    addi t0 x0 -1
    beq a0 t0 exit2
    add s5 a0 x0
    
    add a1 s1 x0
    addi a2 x0 4
    jal ra fread
    addi t0 x0 4
    bne a0 t0 exit4
    
    add a0 s5 x0
    add a1 s2 x0
    addi a2 x0 4
    jal ra fread
    addi t0 x0 4
    bne a0 t0 exit4
    
    lw t0 0(s1)
    lw t1 0(s2)
    mul s4 t0 t1
    slli s4 s4 2
    
    add a0 s4 x0
    jal ra malloc
    beq a0 x0 exit1
    add s3 a0 x0 
    
    add a0 s5 x0
    add a1 s3 x0
    add a2 x0 s4
    jal ra fread
    bne a0 s4 exit4
    
    add a0 s5 x0
    jal ra fclose
    bne a0 x0 exit3
    
    add a0 s3 x0
	# Epilogue
    lw ra 0(sp)
    lw s0 4(sp)
    lw s1 8(sp)
    lw s2 12(sp)
    lw s3 16(sp)
    lw s4 20(sp)
    lw s5 24(sp)
    addi sp sp 28
	ret
    
exit1:
	li a0 26
    j exit
exit2:
	li a0 27
    j exit
exit3:
	li a0 28
    j exit
exit4:
	li a0 29
    j exit
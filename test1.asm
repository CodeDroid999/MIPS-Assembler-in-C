# Some simple MIPS instructions
# used to test the Assembler

	.globl main 

	.text 		

main:
	li $t2, 25		# Load immediate value (25) 
	lw $t3, value		# Load the word stored in value (see bottom)
	add $t4, $t2, $t3	# Add
	sub $t5, $t2, $t3	# Subtract
	sw $t5, Z		#Store the answer in Z (declared at the bottom)  

	li $v0, 10 # Sets $v0 to "10" to select exit syscall
	syscall # Exit

	# All memory structures are placed after the
	# .data assembler directive
	.data

value:	.word 12
Z:	.word 0
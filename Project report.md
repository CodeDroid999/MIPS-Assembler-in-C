Mips Assembler
==============

The assembler will read and parse the contents of a simple MIPS program.
It then generates an output file consisting of data segment and text segment.

# Authors

Author1:
Author2:

# Program Description

The Assembler consists of fives components:
 1.assembler.c
   -opens Input files containing MIPS Assembly code,
   -Checks that files opened properly,
   -make sure correct number of arguments input,
   -Creates a hash table,
   -Parse in passes,
   -and closes the files.

2.file-parser.c
  -contains code to parse contents of inout file,
  -stores registers and their respective binary reference,
  -parse the tokens within a line,
  -interprets the instructions and parse the tokens within a line,
  -Keeps a reference to which register has been parsed for storage.

3.hash_function.c
  -Contains routines to test the hash,
  -mix 4 32-bit values reversibly,
  -report the result.

4.hash_table.h
 -creates a hash table and returns a pointer to it,
 -inserts a structure into the hash table,
 -finds the entry corresponding to key in the hash table,
 -destroys the hash table and frees all allocated memory after a program cycle.

5.tokenizer.h
 -Thread-safe string tokenization for general purpose parsing,
 -Creates tokens,
 -Bypass leading whitespace delimiters,
 -Create output string .

# Supported instructions

The Assembler supports the following instructions:
    - la
    - lw
    - sw
    - add
    - sub
    - addi
    - or
    - and
    - ori
    - andi
    - slt
    - slti
    - sll
    - srl
    - beq
    - j
    - jr
    - jal
 
# Supported directives

The assembler supports the following directives:
    - .data
    - .text
    - .word

# known issues

    components implemented with errors
    1.Section headers such as .data and .text should be in a line by themselves with no other assembly mnemonic.
    2.Branch targets such as loop: will be on a line by themselves with no other assembly mnemonic.
    3.The input assembly file should only contain one data section and one text section.

# Tests

The Assembler design, architecture and functionality were tested using the four test sample codes attached.
The results of these tests were also attached.

    Scope of tests:
 -Test 1 involved testing basic arithmetics with registers
  Test1 was succesful and result is included in the test1.out file

 -Test 2 involved a mips hello world program
  Test1 was succesful and result is included in the test2.out file

 -Test 3 involved testing functions without stack
  Test1 was succesful and result is included in the test3.out file

  -Test 4 involved testing basic arithmetics with registers
  Test1 was succesful and result is included in the test4.out file
  
# Run

After compiling, run:

    ./assembler test1.asm tets1.txt

/*
*Author:
*Author:
*This file contains code to parse contents of inout file(program.asm)
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <unistd.h>
#include "file_parser.h"
#include "tokenizer.h"

/*
 * The structs below map a character to an integer.
 * They are used in order to map a specific instruciton/register to its binary format in ASCII
 */

// Struct that stores registers and their respective binary reference
struct {
	const char *name;
	char *address;
} registerMap[] = {
		{ "zero", "00000" },
		{ "at", "00001" },
		{ "v0", "00010" },
		{ "v1", "00011" },
		{ "a0", "00100" },
		{ "a1", "00101" },
		{ "a2", "00110" },
		{ "a3", "00111" },
		{ "t0", "01000" },
		{ "t1", "01001" },
		{ "t2", "01010" },
		{ "t3", "01011" },
		{ "t4", "01100" },
		{ "t5", "01101" },
		{ "t6", "01110" },
		{ "t7", "01111" },
		{ "s0", "10000" },
		{ "s1", "10001" },
		{ "s2", "10010" },
		{ "s3", "10011" },
		{ "s4", "10100" },
		{ "s5", "10101" },
		{ "s6", "10110" },
		{ "s7", "10111" },
		{ "t8", "11000" },
		{ "t9", "11001" },
		{ NULL, 0 } };

// Struct for R-Type instructions mapping for the 'function' field in the instruction
struct {
	const char *name;
	char *function;
} rMap[] = {
		{ "add", "100000" },
		{ "sub", "100001" },
		{ "and", "100100" },
		{ "or",  "100101" },
		{ "sll", "000000" },
		{ "slt", "101010" },
		{ "srl", "000010" },
		{ "jr",  "001000" },
		{ NULL, 0 } };

// Struct for I-Type instructions
struct {
	const char *name;
	char *address;
} iMap[] = {
		{ "lw",   "100011" },
		{ "sw",   "101011" },
		{ "andi", "001100" },
		{ "ori",  "001101" },
		{ "lui",  "001111" },
		{ "beq",  "000100" },
		{ "bne", "001010" },
		{ "addi", "001000" },
		{ NULL, 0 } };

// Struct for J-Type instructions
struct {
	const char *name;
	char *address;
} jMap[] = {
		{ "j", "000010" },
		{ "jal", "000011" },
		{ NULL, 0 } };

void parse_file(FILE *fptr, int pass, char *instructions[], size_t inst_len, hash_table_t *hash_table, FILE *Out) {

	char line[MAX_LINE_LENGTH + 1];
	char *tok_ptr, *ret, *token = NULL;
	int32_t line_num = 1;
	int32_t instruction_count = 0x00000000;
	int data_reached = 0;
	//FILE *fptr;

	fptr = fopen(program.asm, "r");
	if (fptr == NULL) {
		fprintf(Out, "unable to open file %s. aborting ...\n", program.asm);
		exit(-1);
	}

	while (1) {
		if ((ret = fgets(line, MAX_LINE_LENGTH, fptr)) == NULL)
			break;
		line[MAX_LINE_LENGTH] = 0;

		tok_ptr = line;
		if (strlen(line) == MAX_LINE_LENGTH) {
			fprintf(Out,
					"line %d: line is too long. ignoring line ...\n", line_num);
			line_num++;
			continue;
		}

		/* parse the tokens within a line */
		while (1) {

			token = parse_token(tok_ptr, " \n\t$,", &tok_ptr, NULL);

			/* blank line or comment begins here. go to the next line */
			if (token == NULL || *token == '#') {
				line_num++;
				free(token);
				break;
			}

			printf("token: %s\n", token);

			/*
			 * If token is "la", increment by 8, otherwise if it exists in instructions[],
			 * increment by 4.
			 */
			int x = search(token);
			//int x = (binarySearch(instructions, 0, inst_len, token));
			if (x >= 0) {
				if (strcmp(token, "la") == 0)
					instruction_count = instruction_count + 8;
				else
					instruction_count = instruction_count + 4;
			}

			// If token is ".data", reset instruction to .data starting address
			else if (strcmp(token, ".data") == 0) {
				instruction_count = 0x00002000;
				data_reached = 1;
			}
			// If first pass, then add labels to hash table
			if (pass == 1) {

				printf("First pass\n");

				// if token has ':', then it is a label so add it to hash table
				if (strstr(token, ":") && data_reached == 0) {

					printf("Label\n");

					// Strip out ':'
					//printf("Label: %s at %d with address %d: \n", token, line_num, instruction_count);
					size_t token_len = strlen(token);
					token[token_len - 1] = '\0';

					// Insert variable to hash table
					uint32_t *inst_count;
					inst_count = (uint32_t *)malloc(sizeof(uint32_t));
					*inst_count = instruction_count;
					int32_t insert = hash_insert(hash_table, token, strlen(token)+1, inst_count);

					if (insert != 1) {
						fprintf(Out, "Error inserting into hash table\n");
						exit(1);
					}
				}

				// If .data has been reached, increment instruction count accordingly
				// and store to hash table
				else {

					char *var_tok = NULL;
					char *var_tok_ptr = tok_ptr;

					// If variable is .word
					if (strstr(tok_ptr, ".word")) {

						printf(".word\n");

						// Variable is array
						if (strstr(var_tok_ptr, ":")) {

							printf("array\n");

							// Store the number in var_tok and the occurance in var_tok_ptr
							var_tok = parse_token(var_tok_ptr, ":", &var_tok_ptr, NULL);

							// Convert char* to int
							int freq = atoi(var_tok_ptr);

							int num;
							sscanf(var_tok, "%*s %d", &num);

							// Increment instruction count by freq
							instruction_count = instruction_count + (freq * 4);

							// Strip out ':' from token
							size_t token_len = strlen(token);
							token[token_len - 1] = '\0';

							//printf("Key: '%s', len: %zd\n", token, strlen(token));

							// Insert variable to hash table
							uint32_t *inst_count;
							inst_count = (uint32_t *)malloc(sizeof(uint32_t));
							*inst_count = instruction_count;
							int32_t insert = hash_insert(hash_table, token, strlen(token)+1, inst_count);

							if (insert == 0) {
								fprintf(Out, "Error in hash table insertion\n");
								exit(1);
							}

							printf("End array\n");
						}

						// Variable is a single variable
						else {

							instruction_count = instruction_count + 4;

							// Strip out ':' from token
							size_t token_len = strlen(token);
							token[token_len - 1] = '\0';

							// Insert variable to hash table
							uint32_t *inst_count;
							inst_count = (uint32_t *)malloc(sizeof(uint32_t));
							*inst_count = instruction_count;
							int32_t insert = hash_insert(hash_table, token, strlen(token)+1, inst_count);

							if (insert == 0) {
								fprintf(Out, "Error in hash table insertion\n");
								exit(1);
							}

							printf("end singe var\n");
						}
					}

					// Variable is a string
					else if (strstr(tok_ptr, ".asciiz")) {

						// Store the ascii in var_tok
						var_tok_ptr+= 8;
						var_tok = parse_token(var_tok_ptr, "\"", &var_tok_ptr, NULL);

						// Increment instruction count by string length
						size_t str_byte_len = strlen(var_tok);
						instruction_count = instruction_count + str_byte_len;

						// Strip out ':' from token
						size_t token_len = strlen(token);
						token[token_len - 1] = '\0';

						// Insert variable to hash table
						uint32_t *inst_count;
						inst_count = (uint32_t *)malloc(sizeof(uint32_t));
						*inst_count = instruction_count;
						int32_t insert = hash_insert(hash_table, token, strlen(token)+1, inst_count);

						if (insert == 0) {
							fprintf(Out, "Error in hash table insertion\n");
							exit(1);
						}
					}
				}
			}

			// If second pass, then interpret
			else if (pass == 2) {

				printf("############    Pass 2   ##############\n");

				// start interpreting here
				// if j loop --> then instruction is: 000010 then immediate is insturction count in 26 bits??

				// If in .text section
				if (data_reached == 0) {

					// Check instruction type
					int instruction_supported = search(token);
					char inst_type;

					// If instruction is supported
					if (instruction_supported != -1) {

						// token contains the instruction
						// tok_ptr points to the rest of the line

						// Determine instruction type
						inst_type = instruction_type(token);

						if (inst_type == 'r') {

							// R-Type with $rd, $rs, $rt format
							if (strcmp(token, "add") == 0 || strcmp(token, "sub") == 0
									|| strcmp(token, "and") == 0
									|| strcmp(token, "or") == 0 || strcmp(token, "slt") == 0) {

								// Parse the instructio - get rd, rs, rt registers
								char *inst_ptr = tok_ptr;
								char *reg = NULL;

								// Create an array of char* that stores rd, rs, rt respectively
								char **reg_store;
								reg_store = malloc(3 * sizeof(char*));
								if (reg_store == NULL) {
									fprintf(Out, "Out of memory\n");
									exit(1);
								}

								for (int i = 0; i < 3; i++) {
									reg_store[i] = malloc(2 * sizeof(char));
									if (reg_store[i] == NULL) {
										fprintf(Out, "Out of memory\n");
										exit(1);
									}
								}

								// Keeps a reference to which register has been parsed for storage
								int count = 0;
								while (1) {

									reg = parse_token(inst_ptr, " $,\n\t", &inst_ptr, NULL);

									if (reg == NULL || *reg == '#') {
										break;
									}

									strcpy(reg_store[count], reg);
									count++;
									free(reg);
								}

								// Send reg_store for output
								// rd is in position 0, rs is in position 1 and rt is in position 2
								rtype_instruction(token, reg_store[1], reg_store[2], reg_store[0], 0, Out);

								// Dealloc reg_store
								for (int i = 0; i < 3; i++) {
									free(reg_store[i]);
								}
								free(reg_store);
							}

							// R-Type with $rd, $rs, shamt format
							else if (strcmp(token, "sll") == 0 || strcmp(token, "srl") == 0) {

								// Parse the instructio - get rd, rs, rt registers
								char *inst_ptr = tok_ptr;
								char *reg = NULL;

								// Create an array of char* that stores rd, rs and shamt
								char **reg_store;
								reg_store = malloc(3 * sizeof(char*));
								if (reg_store == NULL) {
									fprintf(Out, "Out of memory\n");
									exit(1);
								}

								for (int i = 0; i < 3; i++) {
									reg_store[i] = malloc(2 * sizeof(char));
									if (reg_store[i] == NULL) {
										fprintf(Out, "Out of memory\n");
										exit(1);
									}
								}

								// Keeps a reference to which register has been parsed for storage
								int count = 0;
								while (1) {

									reg = parse_token(inst_ptr, " $,\n\t", &inst_ptr, NULL);

									if (reg == NULL || *reg == '#') {
										break;
									}

									strcpy(reg_store[count], reg);
									count++;
									free(reg);
								}

								// Send reg_store for output
								// rd is in position 0, rs is in position 1 and shamt is in position 2
								rtype_instruction(token, "00000", reg_store[1], reg_store[0], atoi(reg_store[2]), Out);

								// Dealloc reg_store
								for (int i = 0; i < 3; i++) {
									free(reg_store[i]);
								}
								free(reg_store);
							}

							else if (strcmp(token, "jr") == 0) {

								// Parse the instruction - rs is in tok_ptr
								char *inst_ptr = tok_ptr;
								char *reg = NULL;
								reg = parse_token(inst_ptr, " $,\n\t", &inst_ptr, NULL);

								rtype_instruction(token, reg, "00000", "00000", 0, Out);
							}
						}

						// I-Type
						else if (inst_type == 'i') {

							// la is pseudo instruction for lui and ori
							// Convert to lui and ori and pass those instructions
							if (strcmp(token, "la") == 0) {

								// Parse the instruction - get register & immediate
								char *inst_ptr = tok_ptr;
								char *reg = NULL;

								// Create an array of char* that stores rd, rs and shamt
								char **reg_store;
								reg_store = malloc(2 * sizeof(char*));
								if (reg_store == NULL) {
									fprintf(Out, "Out of memory\n");
									exit(1);
								}

								for (int i = 0; i < 2; i++) {
									reg_store[i] = malloc(2 * sizeof(char));
									if (reg_store[i] == NULL) {
										fprintf(Out, "Out of memory\n");
										exit(1);
									}
								}

								// Keeps a reference to which register has been parsed for storage
								int count = 0;
								while (1) {

									reg = parse_token(inst_ptr, " $,\n\t", &inst_ptr, NULL);

									if (reg == NULL || *reg == '#') {
										break;
									}

									strcpy(reg_store[count], reg);
									count++;
									free(reg);
								}

								// Interpret la instruction.
								// The register is at reg_store[0] and the variable is at reg_store[1]

								// Find address of label in hash table
								int *address = hash_find(hash_table, reg_store[1], strlen(reg_store[1])+1);

								// Convert address to binary in char*
								char addressBinary[33];
								getBin(*address, addressBinary, 32);

								// Get upper and lower bits of address
								char upperBits[16];
								char lowerBits[16];

								for (int i = 0; i < 32; i++) {
									if (i < 16)
										lowerBits[i] = addressBinary[i];
									else
										upperBits[i-16] = addressBinary[i];
								}

								// Call the lui instruction with: lui $reg, upperBits
								// Convert upperBits binary to int
								int immediate = getDec(upperBits);
								itype_instruction("lui", "00000", reg_store[0], immediate, Out);

								// Call the ori instruction with: ori $reg, $reg, lowerBits
								// Convert lowerBits binary to int
								immediate = getDec(lowerBits);
								itype_instruction("ori", reg_store[0], reg_store[0], immediate, Out);

								// Dealloc reg_store
								for (int i = 0; i < 2; i++) {
									free(reg_store[i]);
								}
								free(reg_store);
							}

							// I-Type $rt, i($rs)
							else if (strcmp(token, "lw") == 0 || strcmp(token, "sw") == 0) {

								// Parse the instructio - rt, immediate and rs
								char *inst_ptr = tok_ptr;
								char *reg = NULL;
								//
								// Create an array of char* that stores rd, rs, rt respectively
								char **reg_store;
								reg_store = malloc(3 * sizeof(char*));
								if (reg_store == NULL) {
									fprintf(Out, "Out of memory\n");
									exit(1);
								}

								for (int i = 0; i < 3; i++) {
									reg_store[i] = malloc(2 * sizeof(char));
									if (reg_store[i] == NULL) {
										fprintf(Out, "Out of memory\n");
										exit(1);
									}
								}

								// Keeps a reference to which register has been parsed for storage
								int count = 0;
								while (1) {

									reg = parse_token(inst_ptr, " $,\n\t()", &inst_ptr, NULL);

									if (reg == NULL || *reg == '#') {
										break;
									}

									strcpy(reg_store[count], reg);
									count++;
									free(reg);
								}

								// rt in position 0, immediate in position 1 and rs in position2
								int immediate = atoi(reg_store[1]);
								itype_instruction(token, reg_store[2], reg_store[0], immediate, Out);

								// Dealloc reg_store
								for (int i = 0; i < 3; i++) {
									free(reg_store[i]);
								}
								free(reg_store);
							}

							// I-Type rt, rs, im
							else if (strcmp(token, "andi") == 0 || strcmp( token, "ori") == 0
									|| strcmp(token, "slti") == 0 || strcmp(token, "addi") == 0) {

								// Parse the instruction - rt, rs, immediate
								char *inst_ptr = tok_ptr;
								char *reg = NULL;

								// Create an array of char* that stores rt, rs
								char **reg_store;
								reg_store = malloc(3 * sizeof(char*));
								if (reg_store == NULL) {
									fprintf(Out, "Out of memory\n");
									exit(1);
								}

								for (int i = 0; i < 3; i++) {
									reg_store[i] = malloc(2 * sizeof(char));
									if (reg_store[i] == NULL) {
										fprintf(Out, "Out of memory\n");
										exit(1);
									}
								}

								// Keeps a reference to which register has been parsed for storage
								int count = 0;
								while (1) {

									reg = parse_token(inst_ptr, " $,\n\t", &inst_ptr, NULL);

									if (reg == NULL || *reg == '#') {
										break;
									}

									strcpy(reg_store[count], reg);
									count++;
									free(reg);
								}

								// rt in position 0, rs in position 1 and immediate in position 2
								int immediate = atoi(reg_store[2]);
								itype_instruction(token, reg_store[1], reg_store[0], immediate, Out);

								// Dealloc reg_store
								for (int i = 0; i < 3; i++) {
									free(reg_store[i]);
								}
								free(reg_store);
							}

							// I-Type $rt, immediate
							else if (strcmp(token, "lui") == 0) {

								// Parse the insturction,  rt - immediate
								char *inst_ptr = tok_ptr;
								char *reg = NULL;

								// Create an array of char* that stores rs, rt
								char **reg_store;
								reg_store = malloc(2 * sizeof(char*));
								if (reg_store == NULL) {
									fprintf(Out, "Out of memory\n");
									exit(1);
								}

								for (int i = 0; i < 2; i++) {
									reg_store[i] = malloc(2 * sizeof(char));
									if (reg_store[i] == NULL) {
										fprintf(Out, "Out of memory\n");
										exit(1);
									}
								}

								// Keeps a reference to which register has been parsed for storage
								int count = 0;
								while (1) {

									reg = parse_token(inst_ptr, " $,\n\t", &inst_ptr, NULL);

									if (reg == NULL || *reg == '#') {
										break;
									}

									strcpy(reg_store[count], reg);
									count++;
									free(reg);
								}


								// rt in position 0, immediate in position 1
								int immediate = atoi(reg_store[1]);
								itype_instruction(token, "00000", reg_store[0], immediate, Out);

								// Dealloc reg_store
								for (int i = 0; i < 3; i++) {
									free(reg_store[i]);
								}
								free(reg_store);
							}

							// I-Type $rs, $rt, label
							else if (strcmp(token, "beq") == 0) {

								// Parse the instruction - rs, rt
								char *inst_ptr = tok_ptr;
								char *reg = NULL;

								// Create an array of char* that stores rs, rt
								char **reg_store;
								reg_store = malloc(2 * sizeof(char*));
								if (reg_store == NULL) {
									fprintf(Out, "Out of memory\n");
									exit(1);
								}

								for (int i = 0; i < 2; i++) {
									reg_store[i] = malloc(2 * sizeof(char));
									if (reg_store[i] == NULL) {
										fprintf(Out, "Out of memory\n");
										exit(1);
									}
								}

								// Keeps a reference to which register has been parsed for storage
								int count = 0;
								while (1) {

									reg = parse_token(inst_ptr, " $,\n\t", &inst_ptr, NULL);

									if (reg == NULL || *reg == '#') {
										break;
									}

									strcpy(reg_store[count], reg);
									count++;
									free(reg);

									if (count == 2)
										break;
								}

								reg = parse_token(inst_ptr, " $,\n\t", &inst_ptr, NULL);

								// Find hash address for a register and put in an immediate
								int *address = hash_find(hash_table, reg, strlen(reg)+1);
								
								int immediate = *address + instruction_count;

								// Send instruction to itype function
								itype_instruction(token, reg_store[0], reg_store[1], immediate, Out);

								// Dealloc reg_store
								for (int i = 0; i < 2; i++) {
									free(reg_store[i]);
								}
								free(reg_store);
							}
						}

						// J-Type
						else if (inst_type == 'j') {

							// Parse the instruction - get label
							char *inst_ptr = tok_ptr;

							// If comment, extract the label alone
							char *comment = strchr(inst_ptr, '#');
							if (comment != NULL) {							

								int str_len_count = 0;
								for (int i = 0; i < strlen(inst_ptr); i++) {
									if (inst_ptr[i] != ' ')
										str_len_count++;
									else
										break;
								}

								char new_label[str_len_count+1];
								for (int i = 0; i < str_len_count; i++)
									new_label[i] = inst_ptr[i];
								new_label[str_len_count] = '\0';

								strcpy(inst_ptr, new_label);
							}

							else { printf("NO COMMENT\n");
								inst_ptr[strlen(inst_ptr)-1] = '\0'; 
							}

							// Find hash address for a label and put in an immediate
							int *address = hash_find(hash_table, inst_ptr, strlen(inst_ptr)+1);
							
							// Send to jtype function
							jtype_instruction(token, *address, Out);
						}
					}

					if (strcmp(token, "nop") == 0) {
						fprintf(Out, "00000000000000000000000000000000\n");
					}
				}
			}

			free(token);
		}
	}
}


// Determine Instruction Type
char instruction_type(char *instruction) {

	if (strcmp(instruction, "add") == 0 || strcmp(instruction, "sub") == 0
			|| strcmp(instruction, "and") == 0 || strcmp(instruction, "or")
			== 0 || strcmp(instruction, "sll") == 0 || strcmp(instruction,
			"slt") == 0 || strcmp(instruction, "srl") == 0 || strcmp(
			instruction, "jr") == 0) {

		return 'r';
	}

	else if (strcmp(instruction, "lw") == 0 || strcmp(instruction, "sw") == 0
			|| strcmp(instruction, "andi") == 0 || strcmp(instruction, "ori")
			== 0 || strcmp(instruction, "lui") == 0 || strcmp(instruction,
			"beq") == 0 || strcmp(instruction, "slti") == 0 || strcmp(
			instruction, "addi") == 0 || strcmp(instruction, "la") == 0) {

		return 'i';
	}

	else if (strcmp(instruction, "j") == 0 || strcmp(instruction, "jal") == 0) {
		return 'j';
	}
	return 0;
}

// Write out the R-Type instruction
void rtype_instruction(char *instruction, char *rs, char *rt, char *rd, int shamt, FILE *Out) {

	// Set the instruction bits
	char *opcode = "000000";

	char *rdBin = "00000";
	if (strcmp(rd, "00000") != 0)
		rdBin = register_address(rd);

	char *rsBin = "00000";
	if (strcmp(rs, "00000") != 0)
		rsBin = register_address(rs);

	char *rtBin = "00000";
	if (strcmp(rt, "00000") != 0)
		rtBin = register_address(rt);

	char *func = NULL;
	char shamtBin[6];
	}

	// Print out the instruction to the file
	fprintf(Out, "%s%s%s%s%s%s\n", opcode, rsBin, rtBin, rdBin, shamtBin, func);
}

// Write out the I-Type instruction
void itype_instruction(char *instruction, char *rs, char *rt, int immediateNum, FILE *Out) {

	// Set the instruction bits
	char *rsBin = "00000";
	if (strcmp(rs, "00000") != 0)
		rsBin = register_address(rs);

	char *rtBin = "00000";
		if (strcmp(rt, "00000") != 0)
			rtBin = register_address(rt);

	char *opcode = NULL;
	char immediate[17];

	size_t i;
	for (i = 0; iMap[i].name != NULL; i++) {
		if (strcmp(instruction, iMap[i].name) == 0) {
			opcode = iMap[i].address;
		}
	}

	// Convert immediate to binary
	getBin(immediateNum, immediate, 16);

	// Print out the instruction to the file
	fprintf(Out, "%s%s%s%s\n", opcode, rsBin, rtBin, immediate);
}

// Write out the J-Type instruction
void jtype_instruction(char *instruction, int immediate, FILE *Out) {

	// Set the instruction bits
	char *opcode = NULL;

	// Get opcode bits
	size_t i;
	for (i = 0; jMap[i].name != NULL; i++) {
		if (strcmp(instruction, jMap[i].name) == 0) {
			opcode = jMap[i].address;
		}
	}

	// Convert immediate to binary
	char immediateStr[27];
	getBin(immediate, immediateStr, 26);

	// Print out instruction to file
	fprintf(Out, "%s%s\n", opcode, immediateStr);
}


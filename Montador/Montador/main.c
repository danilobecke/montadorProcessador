//
//  main.c
//  Montador
//
//  Created by Danilo Becke on 29/06/16.
//  Copyright © 2016 Danilo Becke. All rights reserved.
//
//  As instruções devem seguir o padrao:
//  instrucao, rx, ry
//
//  As constantes devem ser precedidas por #
//
//  Os enderecos devem (mas nao precisam) estar entre []
//  Exemplo: ld R0, [R6]
//
//  Código disponível em: https://github.com/danilobecke/montadorProcessador
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 Enum to define the state of conversion
 */
typedef enum State {
    Instruction,
    FirstParam,
    LastParam
} State;

/**
 Enum to define the type of the next parameter
 */
typedef enum ParamType {
    Type,
    Reg,
    Numb,
    Addr
} ParamType;

/**
 *  List with decoded instructions
 */
typedef struct Instructions {
    char instruction[25];
    struct Instructions *next;
}Instructions;

//Global vars
Instructions *instructions;
State currentState = Instruction;
ParamType lastParam;
int count = 0;
int isNumber = 0;
char constantNumber[25];

/**
 *  Insert one instruction in the end of the list
 *
 *  @param instruction Instruction to be added
 */
void insertInstruction(char* instruction) {
    Instructions *aux = instructions;
    while (aux->next != NULL) {
        aux = aux->next;
    }
    aux->next = malloc(sizeof(Instructions));
    aux = aux->next;
    strcpy(aux->instruction, instruction);
    aux->next = NULL;
}

/**
 *  Replaces part of one string for other
 *
 *  @param orig Original string
 *  @param rep  Substring to be removed
 *  @param with Substring to be replaced
 *
 *  @return New string
 */
char *str_replace(char *orig, char *rep, char *with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    unsigned long len_rep;  // length of rep
    unsigned long len_with; // length of with
    unsigned long len_front; // distance between rep and end of last rep
    int count;    // number of replacements
    
    if (!orig)
        return NULL;
    if (!rep)
        rep = "";
    len_rep = strlen(rep);
    if (!with)
        with = "";
    len_with = strlen(with);
    
    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }
    
    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);
    
    if (!result)
        return NULL;
    
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}

/**
 *  Converts a decimal number to its binary representation
 *
 *  @param n    Decimal number
 *
 *  @param size Number of binary digits
 *
 *  @return Binary representation
 */
char *decimal_to_binary(int n, int size)
{
    int c, d, count;
    char *pointer;
    
    count = 0;
    pointer = (char*)malloc(size);
    
    if ( pointer == NULL )
        exit(EXIT_FAILURE);
    
    for ( c = size-1 ; c >= 0 ; c-- )
    {
        d = n >> c;
        
        if ( d & 1 )
            *(pointer+count) = 1 + '0';
        else
            *(pointer+count) = 0 + '0';
        
        count++;
    }
    
    return  pointer;
}


/**
 *  Fix the path of the file
 *
 *  @param path Original path
 *
 *  @return Path ready to be read
 */
char* formatPath(char* path) {
    char *newPath = str_replace(path, "/", "//");
    char *intermediate = str_replace(newPath, " ", "");
    char *finalPath = str_replace(intermediate, "\\", " ");
    return finalPath;

}

/**
 *  Decode the instruction
 *
 *  @param token Instruction to be decoded
 *
 *  @return Instruction decoded
 */
char* decodeInstruction(char* token) {
    char *instruction = NULL;
    if(strcmp(token, "mv") == 0) {
        instruction = "000";
        lastParam = Reg;
    } else {
        if(strcmp(token, "mvi") == 0) {
            instruction = "001";
            lastParam = Numb;
        } else {
            if(strcmp(token, "add") == 0) {
                instruction = "010";
                lastParam = Reg;
            } else {
                if(strcmp(token, "sub") == 0) {
                    instruction = "011";
                    lastParam = Reg;
                } else {
                    if(strcmp(token, "ld") == 0) {
                        instruction = "100";
                        lastParam = Addr;
                    } else {
                        if(strcmp(token, "st") == 0) {
                            instruction = "101";
                            lastParam = Addr;
                        } else {
                            if(strcmp(token, "mvnz") == 0) {
                                instruction = "110";
                                lastParam = Reg;
                            }
                        }
                    }
                }
            }
        }
    }
    return instruction;
}

/**
 *  Decodes one registrator
 *
 *  @param token String to be decoded
 *
 *  @return Decoded string
 */
char* decodeReg(char* token) {
    char *decoded = NULL;
    char *number;
    number = str_replace(token, "R", "");
    decoded = decimal_to_binary(atoi(number),3);
    return decoded;
}

/**
 *  Decodes one number and stores the formated string at constantNumber
 *
 *  @param token Number to be decoded
 */
void decodeConstant(char* token) {
    char *decoded = NULL;
    char *number;
    number = str_replace(token, "#", "");
    decoded = decimal_to_binary(atoi(number), 16);
    count++;
    sprintf(constantNumber, "%03d : %s;\n",count,decoded);
}

/**
 *  Decodes one address
 *
 *  @param token String to be decoded
 *
 *  @return Decoded string
 */
char* decodeAddr(char* token) {
    char *decoded = NULL;
    char *partial, *last;
    partial = str_replace(token, "[", "");
    last = str_replace(partial, "]", "");
    decoded = decodeReg(last);
    return decoded;
}

/**
 *  Decodes one string
 *
 *  @param token String to be decoded
 *
 *  @return Decoded string
 */
char* decodeToken(char* token) {
    char *decoded = NULL;
    switch (currentState) {
        case Instruction:
            decoded = decodeInstruction(token);
            currentState = FirstParam;
            break;
        case FirstParam:
            decoded = decodeReg(token);
            currentState = LastParam;
            break;
        case LastParam:
            if (lastParam == Reg) {
                decoded = decodeReg(token);
            } else if(lastParam == Addr) {
                decoded = decodeAddr(token);
                } else {
                    decoded = "000";
                    decodeConstant(token);
                    isNumber = 1;
                }
            currentState = Instruction;
            break;
        default:
            break;
    }
    
    return decoded;
}

/**
 *  Prints the converted instructions to one file
 *
 *  @param file File that will be written
 */
void printInstructions(FILE* file) {
    char *header = "WIDTH=16;\nDEPTH=128;\n\nADDRESS_RADIX=UNS;\nDATA_RADIX=BIN;\n\nCONTENT BEGIN\n";
    fprintf(file, "%s",header);
    Instructions *aux = instructions->next;
    while (aux != NULL) {
        fprintf(file,"%s",aux->instruction);
        aux = aux->next;
    }
    fprintf(file, "END;\n");
}

int main(int argc, const char * argv[]) {
    
//Variables
    //file path
    char filePath[500];
    //pointer to file
    FILE *file;
    
    
//UI
    //getting file path
    printf("Arraste o arquivo de entrada\n");
    scanf("%[^\n]c",filePath);
    
    //initializing the list
    instructions = malloc(sizeof(Instructions));
    strcpy(instructions->instruction, "HEAD");
    instructions->next = NULL;

    
//File operations
    char *newPath = formatPath(filePath);
    file = fopen(newPath, "r");
    if (file != NULL) {
        char *lineBuffer = NULL;
        char decodedInstruction[25] = "000 : ";
        ssize_t read;
        size_t len = 0;
        //reading lines
        while ((read = getline(&lineBuffer, &len, file)) != -1) {
            //removing \n
            char *preparing = str_replace(lineBuffer, "\n", "");
            //removing ,
            char *line = str_replace(preparing, ",", "");
            //separating line by spaces
            char *token = strtok(line, " ");
            while (token != NULL) {
                //decoding token
                char *decodedToken = decodeToken(token);
                if(decodedToken == NULL) {
                    printf("Código inválido\n");
                    return 1;
                }
                //formating the instruction
                sprintf(decodedInstruction, "%s%s",decodedInstruction,decodedToken);
                token = strtok(NULL, " ");
            }
            sprintf(decodedInstruction, "%s0000000;\n",decodedInstruction);
            insertInstruction(decodedInstruction);
            count++;
            //checks if the instruction needs a constant number
            if(isNumber == 1) {
                insertInstruction(constantNumber);
                isNumber = 0;
            }
            sprintf(decodedInstruction, "%03d : ",count);
        }
    }
    fclose(file);
    file = NULL;
    file = fopen("output.mif", "w");
    printInstructions(file);
    fclose(file);
    printf("Arquivo criado com o nome 'output.mif'\n");
    return 0;
}

//
//  main.c
//  Montador
//
//  Created by Danilo Becke on 29/06/16.
//  Copyright © 2016 Danilo Becke. All rights reserved.
//

#include <stdio.h>
#include <regex.h>
#include <stdlib.h>
#include <string.h>

char *str_replace(char *orig, char *rep, char *with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep
    int len_with; // length of with
    int len_front; // distance between rep and end of last rep
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
    for (count = 0; tmp = strstr(ins, rep); ++count) {
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



char* formatPath(char* path) {
    
    char *newPath = str_replace(path, "/", "//");
    char *finalPath = str_replace(newPath, "\\ ", " ");
    return finalPath;

}

int main(int argc, const char * argv[]) {
    
//MARK: Variables
    //file path
    char filePath[500];
    //pointer to file
    FILE *file;
    
    
//MARK: UI
    //getting file path
    printf("Arraste o arquivo de entrada\n");
    scanf("%[^\n]c",filePath);
    
//MARK: File operations
    char *newPath = formatPath(filePath);
    file = fopen(newPath, "r");
    if (file != NULL) {
        char teste[10];
        fscanf(file, "%s", teste);
        printf("%s\n",teste);
    }
    return 0;
}

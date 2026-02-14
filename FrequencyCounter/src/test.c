#include <stdio.h>
#include <string.h>

typedef struct Word {
    char word[21];
    int count;
}Word;

void swapEmSort(Word arr[]) {
    int pointer = 1;
        while (arr[pointer].word[0] != '\0' && pointer != 200) {

        // If LHS < RHS, then swap them
        if (arr[pointer-1].count < arr[pointer].count) {
            Word temp = arr[pointer-1];
            arr[pointer-1] = arr[pointer];
            arr[pointer] = temp;
            pointer = 1;
            continue;
        }

        // If same frequencey AND not already in order
        if (arr[pointer-1].count == arr[pointer].count && strcmp(arr[pointer-1].word, arr[pointer].word) > 0) {

            Word temp = arr[pointer-1];
            arr[pointer-1] = arr[pointer];
            arr[pointer] = temp;
            pointer = 1;
            continue;
        }
        pointer++;

    }
}

int main(void) {

    // Use aggregate initializer to zero out all indices
    Word input[200] = { {"", 0} };

    char buffer[21];
    int pointer = 0;
    while (scanf("%s", buffer) == 1) {
        if (strcmp(buffer, "zzzzz") == 0) {
            break;
        } 
        do {
            // If the first char is \0, string is empty meaning it found an empty index
            if (input[pointer].word[0] == '\0') {
                // Create a new word at that point
                strcpy(input[pointer].word, buffer);
                input[pointer].count = 1; 
                pointer = 0;
                break;
            }
            // If the word has already been added increment its count
            else if (strcmp(input[pointer].word, buffer) == 0) {
                input[pointer].count++;
                pointer = 0;
                break;
            }
        } while(input[pointer++].word[0] != '\0' && pointer != 200);
    }


    swapEmSort(input);

    printf("======\n");
    // Print the 5 most frequent with their count
    for(int i=0; i<5; i++) {
        printf("%d %s\n", input[i].count, input[i].word);
    }

    return 0;
}
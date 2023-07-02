/*
A C program that uses doubly linked list to evaluate postfix expressions from a file, and writes the results in an output file.

Roxanne Ysabel P. Resuello
May 23, 2022 1:30pm
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

//node structure
typedef struct node_tag{
    int data;
    struct node_tag *next;
    struct node_tag *prev;
}node;

//Function for printing an evaluation step
void printStep (FILE *outFile, struct node_tag *head, char expression[]){
    struct node_tag *temp = head;
    fprintf(outFile, "= ");

    //points the temp to the last node
    while(temp->next != NULL){
        temp= temp->next;
    }

    //prints all the data in the node and the unevaluated expression
    while(temp != NULL){
        fprintf(outFile, "%d ", temp->data);
        temp = temp->prev;
    }
    
    if (expression[0] == ' '){
        fprintf(outFile, "%s\n", expression+1);
    } else{
        fprintf(outFile, "%s\n", expression);
    }
}

//Function for adding a node
void addNode(struct node_tag **head, int temp){
    struct node_tag *new = (struct node_tag *) malloc (sizeof(struct node_tag));
    new->data = temp;
    new->next = NULL;
    new ->prev = NULL;
    if(*head == NULL){      //adds a new node if linked list is empty
        *head = new;
    } else{                 //add a new node at head
        new->next = (*head);
        (*head)->prev = new;
        (*head) = new;
    }    
}

//Evaluates the operator and first two operand in the linked list
void computeExp(struct node_tag **head, char operator){
    //variable declaration
    struct node_tag *temp = (*head);
    int operand1 = (*head)->next->data;
    int operand2 = (*head)->data;
    int result;

    //checks the given operator, then applies it to the operands11
    switch(operator){
        case '-':
            result = operand1 - operand2;
            break;
        case '/':
            result = operand1 / operand2;
            break;
        case '+':
            result = operand1 + operand2;
            break;
        case '*':
            result = operand1 * operand2;
            break;
        case '%':
            result = operand1 % operand2;
            break;
    }

    //inputs the result in the linked list and deletes one node containing the used operand 
    (*head) = (*head)->next;
    (*head)->data = result;
    (*head)->prev = NULL;
    free(temp);
}

//Function for deleting all the nodes of linked list
void deleteAll(struct node_tag **head){
    while((*head) != NULL){
        struct node_tag *temp = (*head);
        (*head) = (*head)->next;
        free(temp);
    }
}

/*
Function for scanning the expression in input file and prints it in the output file
Counts the length of expression, number of operator, and number of operands
*/
void scanExpression(FILE *inFile, FILE *outFile, int *expLength, int *digitCount, int *operatorCount, int number){
    char currentChar, temp = ' ';

    fprintf(outFile, "Expression %d:\n", number);

    //gets all the character in the expression and prints it
    while(currentChar != '\n' && currentChar !=EOF){
        currentChar = fgetc(inFile);
        if (currentChar !=EOF){
            if ( currentChar != '\n'){
                fputc(currentChar, outFile);
                *expLength += 1;
            } 
        }

        if (currentChar == ' ' || currentChar == '\n' || feof(inFile)){
            if(isdigit(temp)){
                *digitCount += 1;               //increments the number of operand
            } else{
                *operatorCount += 1;            //increments the number of operator
            }
        }
        temp = currentChar;
    }
    fputc('\n', outFile);
}

int main(){
    //declaration of file pointer for the input file
    FILE *inFile = fopen("expressions.in", "r");
    FILE *inFile2 = fopen("expressions.in", "r");

    //Checks if the input file exists, if yes performs the evaluation
    if (inFile != NULL){
        //variable declaration
        int numOfExpressions, temp, expLength, digitCount, operatorCount;
        struct node_tag *head = NULL;
        
        //declaration of file pointer for the output file
        FILE *outFile = fopen("evaluations.out", "w");

        //scans the number of expressions in the file
        fscanf(inFile, "%d\n", &numOfExpressions);
        fscanf(inFile2, "%d\n", &temp);
        
        
        //traverses through all the expressions in written in the file
        for (int i = 0; i<numOfExpressions; i++){
            expLength = 0;
            digitCount = 0;
            operatorCount = 0;

            //prints the given expression in the output file
            scanExpression(inFile, outFile, &expLength, &digitCount, &operatorCount, i+1);   
            
            char currentChar, tempOperator = ' ', expression[expLength+1];
            int prevDigit = 0;
            int validDigit = 0;             //1 if the prevDigit(previous digit) is valid, else 0

            //scans the given equation
            fscanf(inFile2, " %[^\n]s", expression);

            int length = strlen(expression);
            
            //checks if the number of operator or operand are enough to evaluate the expression
            if (operatorCount+1 != digitCount || operatorCount <= 0 || digitCount <= 0){
                if (operatorCount < digitCount - 1){
                    fprintf(outFile, "= Invalid! Insufficient Operators!\n");
                } else if( digitCount <= operatorCount){
                    fprintf(outFile, "= Invalid! Insufficient Operands!\n");
                } else {
                    fprintf(outFile, "= Invalid! Insufficient Operators and Operands!\n");
                }
            } else{                                 //if the number of operator and operands are valid, traverses through each character and then evaluates
                for(int s = 0; s<length; s++){
                    currentChar = expression[0];
                    memmove(expression, expression+1, strlen(expression));      //removes the first character in the expression


                    if(currentChar == ' '){                     //checks if the current character is a ' '
                        if (tempOperator == '-'){               //if the temp operator is a '-', use it to the last two operand in the linked list
                            computeExp(&head, tempOperator);
                            printStep(outFile, head, expression);
                            tempOperator = ' ';
                        } else if(validDigit == 1) {            //adds a new node containing the previous digit
                            addNode(&head, prevDigit);
                            prevDigit = 0;
                            validDigit = 0;
                        }
                    } else if (isdigit(currentChar)){           //if the current character is a digit, stores it to the prevDigit variable
                        if (tempOperator == '-'){               //stores the negative value of the previous digit
                            prevDigit = -1 * (currentChar - '0');
                            tempOperator = ' ';
                        } else if (validDigit == 1) {           //adds the current char to prevDigit
                            if (prevDigit < 0){
                                prevDigit = (prevDigit*10) - (currentChar-'0');
                            } else{
                                prevDigit = (prevDigit*10) + (currentChar-'0');
                            }
                        }else {                                 //stores the current character to prevDigit
                            prevDigit = currentChar-'0';
                        }
                        validDigit = 1;
                    } else if(currentChar == '-' && s != length-1){
                        tempOperator = currentChar;
                    }else{                                      //if the current character is an operator, use it in the last two operand in the linked list
                        computeExp(&head, currentChar);
                        validDigit = 0;
                        prevDigit = 0;
                        tempOperator = ' ';
                        printStep(outFile, head, expression);
                    }
                }
            }
            deleteAll(&head);   //deles all the nodes
        }
        //Close file pointers
        fclose(outFile);
        fclose(inFile);
        fclose(inFile2);
    } else {
        printf("\nNo file to be opened!\n");
    }
    return 0;
}
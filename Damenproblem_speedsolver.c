/*
    Informatik & Programmieren 2 (Informatics & Programming 2)
    ECE 2016

    Name:       Felix Mark
    Exercise:   Ladys Problem (Queens Problem)
    Date:       02.03.2017
    Version:    1.1
    Comment:    This is a possible solution for solving the Queens-problem.
                For single solution an algorithm described in the following Wikipedia Article is used.
                https://de.wikipedia.org/wiki/Damenproblem
*/

//includes
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//defines
#define DAME            "\n       ()\n    .-:--:-.\n     \\____/\n     {====}\n      )__(\n     /____\\\n      |  |\n      |  |\n      |  |\n      |  |\n     /____\\\n    (======)\n    }======{\n   (________)\n\n"
#define MAX_BOARD_SIZE  50
#define TRUE            1
#define FALSE           0
#define SINGLE_BOARD    6

//global variables
unsigned long int       found_solutions = 0;
unsigned char           posX = 0, posY = 0;
unsigned char           lady_count = 0;

short int           	ladys			[MAX_BOARD_SIZE] = {};
short int           	row_block		[MAX_BOARD_SIZE] = {};
short int           	diagonal_m	    [2*MAX_BOARD_SIZE-1] = {};
short int           	diagonal_s	    [2*MAX_BOARD_SIZE-1] = {};

double                  output_data[MAX_BOARD_SIZE] = {};
double                  time_diff, time_start;
unsigned int            n_cnt_collisiontests = 0;
unsigned int            n_cnt_removes = 0;
unsigned char           all_solutions = 0, debug_mode = 0, output = 1, board_size = 30, time_mode = 0;

//function-prototypes
void printMenu();       // ... print Menu
void solveLadysProblem(char solveSilent, char meassureTime);
void setLady();         // ... set Queen on Position posX / posY
char hasConflict();     // ... checks if posX posY has a conflict with another Queen
void removeLady();      // ... removes last Queen
void printChessBoard(); // ... prints chessboard
void findLady();        // ... searches Queen in row posX
void initGlobals();     // ... initializes all Variables
void stepBack();        // ... step back
void saveFile(char x);  // ... saves File to Output.csv

int main() {
    //char array for user input
    char input[100] = {};

    while (!(input[0] == 'x' && input[1] == 0)) {
        //clear screen first
        system("clear");

        //Menu output and userinput
        printMenu();
        scanf("%s",input);
        printf("\n");

        //RUN selected
        if (input[0] == 'r' && input[1] == 0) {
            //clear screen first
            system("cls");
            initGlobals();
            if (all_solutions) {
                //if all solutions was selected, find all solutions
                solveLadysProblem(FALSE, TRUE);
            } else {
                //if only one solutions is selected, find one solution
                if (board_size > SINGLE_BOARD) {
                    //if board_size is greater than the start board for single solutions...

                    //store board size, the user wanted in board_size_save (i.e. 40)
                    char board_size_save = board_size;
                    //use previous solution for next solution algorithm
                    for (char i = SINGLE_BOARD; i <= board_size_save; ++i) {
                        board_size = i;
                        solveLadysProblem(i != board_size_save, i == SINGLE_BOARD);
                        if (time_mode) {
                            time_diff = (clock() - time_start); // calculating time difference
                            output_data[i-SINGLE_BOARD] = time_diff;
                            if (i != board_size_save) printf ("%dx%d: %f seconds\n", board_size, board_size, time_diff/CLOCKS_PER_SEC);  // needed time in seconds
                        }
                    }
                    if (time_mode) {
                        printf("\n");
                        saveFile(board_size_save-SINGLE_BOARD);
                    }
                    for (int i = 0; i < MAX_BOARD_SIZE; ++i) {
                        output_data[i] = 0.0;
                    }
                } else {
                    //...board_size is less or equal to the start board for single solutions...
                    solveLadysProblem(FALSE, TRUE);
                }
            }
            getchar(); //wait for key
        } else if (input[0] == 'a' && input[1] == 0) {
            //toggle the all_solutions mode
            all_solutions = !all_solutions;
            printf("ALL_SOLUTIONS turned %s", all_solutions?"ON":"OFF");
            getchar(); //wait for key
        } else if (input[0] == 'd' && input[1] == 0) {
            //toggle debug_mode
            debug_mode = !debug_mode;
            printf("DEBUG_MODE turned %s", debug_mode?"ON":"OFF");
            getchar(); //wait for key
        } else if (input[0] == 'o' && input[1] == 0) {
            //toggle output_mode
            output = !output;
            printf("OUTPUT turned %s", output?"ON":"OFF");
            getchar(); //wait for key
        } else if ((!all_solutions) && input[0] == 't' && input[1] == 0) {
            //toggle output_mode
            time_mode = !time_mode;
            printf("TIME_MODE turned %s", time_mode?"ON":"OFF");
            getchar(); //wait for key
        } else if (input[0] == 's' && input[1] == 0) {
            //set size
            getchar();
            printf("New board-size (1-%d): ",MAX_BOARD_SIZE);
            char bs[100] = {};
            fgets(bs, 100, stdin);
            char board_size_new = atoi(bs);
            if (board_size_new > 0 && board_size_new <= MAX_BOARD_SIZE) {
                board_size = board_size_new;
                printf("\nBoard-size changed.");
            } else {
                printf("Incorrect input.");
            }
        } else if (input[0] == 'x' && input[1] == 0) {
            //exit program by continuing. Outer loop will break since input is equal to 'x'
            continue;
        } else {
            printf("Incorrect input.");
            getchar();
        }
        printf("\n\nPress [ENTER]"); //wait for enter (or any other key) after command is executed
        getchar();
    }
} //main()

void printMenu() {
    printf(DAME);
    printf("BOARD_SIZE\t%d (%dx%d)\n",board_size,board_size,board_size);
    printf("ALL_SOLUTIONS\t%s\n",all_solutions?"ON":"OFF");
    printf("DEBUG_MODE\t%s\n",debug_mode?"ON":"OFF");
    if (!all_solutions) printf("TIME_MODE    \t%s\n",time_mode?"ON":"OFF");
    printf("OUTPUT    \t%s\n\n",output?"ON":"OFF");
    //printf("Menu:\n");
    printf("[r] run solver algorithm\n");
    printf("[s] change boardsize\n");
    printf("[a] toggle ALL_SOLUTIONS\n");
    printf("[d] toggle DEBUG_MODE\n");
    if (!all_solutions) printf("[t] toggle TIME_MODE\n");
    printf("[o] toggle OUTPUT\n");
    printf("[x] exit\n\n> ");
} //printMenu()

void initGlobals() {
    found_solutions = 0;
    posX = 0;
    posY = 0;
	for (int i = 0; i < 2*MAX_BOARD_SIZE; ++i) {
		if (i < MAX_BOARD_SIZE){
			ladys[i] = -1;
			row_block[i] = -1;
		}
	    diagonal_s[i] = -1;
	    diagonal_m[i] = -1;
	}
    n_cnt_collisiontests = 0;
    n_cnt_removes = 0;
} //initGlobals()

void saveFile(char x) {
	FILE *output_file;
	output_file = fopen("Output.csv","w");
	for (short int i = 0; i < x; ++i) {
		fprintf(output_file,"%f;\n", output_data[i]);
	}
	fclose(output_file);
	printf("File Output.csv saved.\n");
} //saveFile()







void solveLadysProblem(char solveSilent, char meassureTime) {
	char locked = FALSE, stopSolving = FALSE; //locked and stopSolving for program flow control
	if (meassureTime) {
        time_t t;
        time(&t);
        printf ("Start: %s\n\n", ctime(&t));
        time_start = clock(); //getting start time (after printing it, so we don't have the print time in there)
	}
    if (debug_mode) getchar();

	while(!stopSolving) {
        if (debug_mode) {
            printChessBoard();
            printf("\n");
            printf("\nPress [ENTER]");
            getchar();
            printf("\n");
        }

        if (!locked && !hasConflict()) { //No conflict on [posX][posY]
            setLady();
            posY = posY > 1 ? 0 : posY == 0 && board_size > 2 ? 2 : board_size > 3 ? 3 : board_size-1;
            ++posX;
            if (posX >= board_size) { //found a Solution
                if (!solveSilent) {
                    ++found_solutions;
                    if (output) {
                        printf("Solution #%lu:\n",found_solutions);
                        printChessBoard();
                        printf("\n");
                    }
                }
                if (!all_solutions) {
                	for(int i = board_size*2-1; i > 0; --i) {
                		diagonal_m[i] = diagonal_m[i-1];
                	}
                	diagonal_m[0] = -1;

                	break;
                }
                stepBack();
                locked = TRUE;
            }
        } else { //There was a conflict on posX posY
            locked = FALSE;
            char isOkay = FALSE;
            while (!isOkay) {
                ++posY;
                if (posY >= board_size) {
                    if (posX == 0) {
                        if (output && !solveSilent) printf("End.\n");
                        stopSolving = TRUE;
                        break;
                    }
                    stepBack();
                } else {
                    isOkay = TRUE;
                }
            }
        }
	} //solving Loop


    if (!solveSilent) {
        time_diff = (clock() - time_start); //calculating time difference
        printf ("\n");
        printf ("END TIME:               %f seconds\n", time_diff/CLOCKS_PER_SEC);
        printf ("found solutions:        %lu\n", found_solutions);
        printf ("collisionchecks:        %u\n", n_cnt_collisiontests);
        printf ("lady removes:           %u\n", n_cnt_removes);
    }
}

void setLady() {
    ++lady_count;
    ladys[posX] = posY;
    row_block[posY] = 1;
    diagonal_s[posX+posY] = 1;
    diagonal_m[posX-posY+board_size-1] = 1;
} //setLady()

void removeLady() {
    n_cnt_removes++;
    --lady_count;
    row_block[posY] = -1;
    diagonal_s[posX+posY] = -1;
    diagonal_m[posX-posY+board_size-1] = -1;
} //removeLady()

char hasConflict() {
    ++n_cnt_collisiontests;
    if (
        row_block[posY] > 0 ||
        diagonal_s[posX+posY] > 0 ||
        diagonal_m[posX-posY+board_size-1] > 0
    ) return TRUE;  //CONFLICT found
    return FALSE;   //NO CONFLICT found
} //hasConflict()

void printChessBoard() {
    for (unsigned char y = 0; y < board_size; ++y) {
        for (unsigned char x = 0; x < board_size; ++x) {
            printf("%c ", (ladys[x] != y || x >= lady_count ? ((x == posX && y == posY) ? 'x' : '.') : 'D'));
        }
        printf("\n");
    }
    if (debug_mode) {
        printf("  ");
		for (unsigned char x = 0; x < board_size*2-1; ++x) {
			printf("%c ", (diagonal_m[x] == -1 ? '0' : '1'));
		}
    }
} //printChessBoard()

void findLady() {
    posY = ladys[posX];
} //findLady()

void stepBack() {
    --posX;
    findLady();
    removeLady();
} //stepBack()


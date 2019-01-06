#include <stdio.h>
#include <stdlib.h>

int main()
{
    //printf(); with modifiers
    /*
    int x=52;
    float y=27.3392;
    double z=134000;  //large numbers need to be a double in order to work correctly
    printf("x=%d\n", x); //same as bucky
    printf("y=%.4f\n", y); //same as bucky
    printf("x=%d\ny=%.4f\n", x, y);
    printf("z=%e\n", z); //this is scientific notation

    //scanf();
    int x;
    float y;
    printf("Enter an integer value:");
    scanf("%d", &x); //accepts input from a user
    printf("Enter a float value:");
    scanf("%f", &y);  //don't put any friggin formats on this beast
    printf("You entered %d, %.2f.", x, y);
    scanf("%d%f", x, y;); //this checks for a space, interprets everything before as an integer, and everything after as float


    //pointers
    int i=27, int j=-32;
    int *pointer1;
    int *pointer2;
    pointer1=&i;
    pointer2=&j;
    *pointer1 = *pointer1 + 2;  //uses pointer as alias for the variable it points to, star is necessary
    *pointer2 = *pointer2 * *pointer2;
    */

    //Example of swap using pass-by-value
    /*
void swap (int x, int y);  //Function prototype

void swap (int x, int y)
{
    int temp = x;
    x=y;
    y=temp;
    //returns with values that are not actually swapped

}
int main (int argc, char *argv[])
    {
	int i=27, j=-32;
	swap (i,j);  //prevents from passing three arguments when it only takes two
	printf("i=%d\nj=%d\n", i, j);


    return 0;
    }

    //Example of swap using pass-by-example
void swap (int *x, int *y); //again, function prototype, but needs to match real function

void swap (int *x, int *y)
{
    int temp = *x;
    *x = *y;
    *y=temp;

}
int main (int argc, char *argv[])
{
    int i=27, j=-32;
    swap (&i, &j);
    printf("i=%d\nj=%d\n", i, j);

    return 0;
}
//this swaps the values, the other method does not, could be useful to alter data without changing values
*/

//}

//Arrays
/*
int myArray [100];
float myArray2[] = {32.2, 68.95, -103.667}; Empty brackets create array size of input, a designated number creates a pre-made array

//note that int [100] myArrays; int []myArray = new int[100]; are illegal
//size of the array must be a literal, not a constant, is original C.
#define N 100 //defines N as 100, as a text substitution
#define M 1000 //defines M as 1000
float myArray3[N][M];
for (i=0; i<N; i++){
    for (j=0, j<M; j++){
        myArray3[i][j]=...
    }
}

int myArray [N];
    for(i=0; i<N; i++) //semi-colon separates three parts, commas separate sections within a part
        myArray[i]="whatever goes here";


//Proof that arrays are not part of pass-by-value
#define N 40

void add2(int b[]) //passes by value
{
    int j;
    for(j=0;j<N;j++)
        b[j]+=2;
}

int main (int argc, char *argv[])
{
    int i, b[N];
    for (i=0; i<N; i++)
        b[i]=11*i;
    add2(b);
    for (i=0; i<N; i++)
        printf("b[%d]=%d\n", i, b[i]);


    char myTable[10] = "Hello\0"; //allows to kill the string before the end of the array (k*\erning). All strings need this.
    printf("%s \n", myTable);

    char myTable2[] = {'t', 'w', 'o', '\0'}; //cumbersome
    printf("%s \n", myTable2);

    char *myTable3 = "three"; //references another array that is not placed on the stack
    printf("%s \n", myTable3);

    char *myTable4; //this doesn't really do anything as space has not been allocated.

    //copy to another array
    i=0
    while(myTable2[i]!='\0'){
        myTable[i]=myTable2[i];
        i++;
    }
    myTable[i]='\0';


    /*gets(s) - read characters from stdin until a new line is reached and store those characters (minus the new line) in the
    string with '\0' at the end.*/
    //puts(s) - writes the string to stdout and adds a new line.
    /*fgets(<string>,<size>, <file descriptor>) - reads characters from the file until <size> characters are read or a new line is reached
    and stores those in the <string> (including newline) with '\0' at the end. */
    //fputs(<string>, <file descriptor>) - writes the string to the file and does NOT add a new line.
/*
    char s[BUFSIZ];
    fgets(s,BUFSIZ-1, stdin);
    sscanf(s, "%f%s", &x, &s2);
    s[strlen(s)-1]='\0'; //this grabs the string without the new line
*/





    return 0;
}


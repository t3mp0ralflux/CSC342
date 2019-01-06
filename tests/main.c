#include <stdio.h>
#include <stdlib.h>
#define PAGE_SIZE 512


int main()
{
    int logic_addr = 5178;
    int pageNumber;
    float offset;
    pageNumber = (logic_addr/PAGE_SIZE);
    printf("The page number is %d\n", pageNumber);
    offset = (logic_addr % PAGE_SIZE);
    printf("The offset is %f\n", offset);

}

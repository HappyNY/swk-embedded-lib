#include <swk-embedded-c/fslist.h>

typedef struct data
{
    char name[12];
    int age;
} data_t;

int main( void )
{
    FreeSpaceList list;
    data_t initD = { {"團熱", 15 }, {"繹熱", 21 }, {"唳熱", 33}, {"陴熱", 5}, {"鷓熱", 12}, {"惜熱", 44} };
    int i;

    fslistInit( &list, 100, sizeof (data_t) );
    
    for(i = 0; i < sizeof(initD)/sizeof(*initD); ++i)
    {
        fslistInsertNew( &list, -1, initD + i );
    }
}
#include <swk-embedded-c/fslist.h>

typedef struct data
{
    char name[12];
    int age;
} data_t;

int main( void )
{
    FreeSpaceList list;
    data_t initD = { {"�μ�", 15 }, {"ö��", 21 }, {"���", 33}, {"����", 5}, {"�¼�", 12}, {"����", 44} };
    int i;

    fslistInit( &list, 100, sizeof (data_t) );
    
    for(i = 0; i < sizeof(initD)/sizeof(*initD); ++i)
    {
        fslistInsertNew( &list, -1, initD + i );
    }
}
#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/*! \brief
    A function callback that returns \n
        l > r -> val > 0 \n
        l = r -> val = 0 \n
        l < r -> val < 0
 */
typedef int (*btree_compare_func_t)(void const *l, void const *r);

/*! \brief      Memory pool implementation of binary tree.
 */
struct btree
{
    //! \brief      Root node.
    struct btree_node *root;
    
    //! \brief      Memory pool
    struct btree_node *pool;

    //! \brief      Where the data begins
    char *data;
    
    //! \brief      Allocated memory chunk
    void *buff;
    
    //! \brief      Size of each element
    size_t elemSize;
    
    //! \brief      Count of active nodes.
    size_t size;
    
    //! \brief      Number of maximum elements
    size_t capacity; 

    //! \brief      Comparator function
    btree_compare_func_t compare;
};

struct btree_node
{
    /* data */
    //! \brief      Left side child node
    struct btree_node *left;
    //! \brief      Right side child node
    struct btree_node *right;
};

typedef struct btree btree_t;

/*! \brief              Initializes new binary tree
    \param s           
    \param buff         Allocated memory buffer. This should be released when finalizing the tree.
    \param buffSize     Size of the 'buff'
    \param elemSize     Size of each elements. This will not be changed after initialization.
    \return             Number of available nodes.
 */
size_t btree_init(btree_t *s, void *buff, size_t buffSize, size_t elemSize, btree_compare_func_t compare);

/*! \brief              Inserts data into binary tree.
    \param s            
    \param data         Data to insert.
    \return             'true' if insertion was successful. 'false' when the value is duplicated.
 */
bool btree_insert(btree_t *s, void const *data);

/*! \brief              Replacing version of insert(). This function always replace value 
    \param s            
    \param data 
    \return 
 */
void btree_substitue(btree_t *s, void const *data);

/*! \brief              Lookup for data. 
    \param s 
    \param data 
    \return             Actual data address that matches with given data. 
    \warning            Return value should not be modified to prevent corrupting sorted state!
 */
void const* btree_lookup(btree_t *s, void const*data);

/*! \brief              Erase data.
    \param s 
    \param data         Data to erase
    \return             'true' if data has successfuly erased.
 */
bool btree_erase(btree_t *s, void const*data);

/*! \brief              Iterate same operation per objects.

    \param s 
    \param obj 
    \param cb 
 */
void btree_forEach(btree_t *s, void* obj, void (*cb)(void*, void const*));
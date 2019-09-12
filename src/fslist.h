//! \file       fslist.h
//! \brief      Decribes free space node type.
#pragma once
#include <stdint.h>
#include <stdbool.h>

//! Unsigned integer type is required to indicate node index.
typedef uint16_t fslistNodeIdx_t;

//! Representation for null node.
enum { fslist_nodeidx_none = (uint16_t)(-1) };

/*! \brief      Node type for free spaced list
    \note       Since each node is represented as index from pre-allocated memory pool,
                there's no need to refer nodes via pointer. */
typedef struct fslistNode
{
    //! Node index of previous linkage
    fslistNodeIdx_t prev;
    //! Node index of next linkage
    fslistNodeIdx_t next;

    //! \brief      Pointer to a data structure.
    //!             The description for the original data type will be inside of fslist type.
    void* pdata;
}fslistNode_t; 

//! \brief Free space list type
//! \warning Any member of this structure must not be modified directly.
typedef struct fslist
{
    //! First available node
    fslistNodeIdx_t head;
    //! Last available node
    fslistNodeIdx_t tail;
    //! The first node of all available nodes.
    fslistNodeIdx_t availableHead;

    //! \brief      Pointer to node pool.
    //! \details    You can access to each nodes via "->pool[node_index];
    fslistNode_t* pool;
    
    //! Pointer to data pool. Detached from the node pool to avoid dynamic allocation for each node creation.
    uint8_t* dataPool;

    //! Length of the allocated pool. Capacity can grow if all of the pool elements are consumed.
    size_t poolCapacity;
    
    //! Currently active node count. 
    size_t count;

    //! Size of allocated element type. 
    size_t elemSize;

    //! Callback for construct data type.
    void ( *ctorCallback )( void*, void const* /*initData*/ );

    //! Callback for destruct data type.
    void ( *dtorCallback )( void* );

} fslist_t;

/*! \brief      Initialize newly generated fslist. 
    \returns    false when allocating memory for dataPool or nodePool has failed. */
bool fslistInit( fslist_t* list, size_t initCapacity, size_t elementSize );

/*! \brief      Deallocate currently allocated list. 
    \details    This function can handle the list object that wasn't correctly allocated on initialization. */
bool fslistDestroy( fslist_t* list );

/*! \brief      Checks if given node is inside of given list.  
    \returns    fslist_nodeidx_none if given node doesn't came from given list. */
fslistNodeIdx_t fslistGetOffset( fslist_t const* list, fslistNode_t const* node );

/*! \brief      Create and insert a new node after a node. 
    \param      list; A list that owns the node on a given index.
    \param      node; A node which will be preceded by the newly generated node. If set this value to none(-1), then the newly created node will replace the head.
    \param      initData; Initial data which will be copeid into newly create node. 
    \returns    pointer to newly created node. */
fslistNode_t* fslistInsertNew( fslist_t* list, fslistNodeIdx_t node, void const* initData );

/*! \brief      Removes a node from the list. */
bool fslistErase( fslist_t* list, fslistNodeIdx_t* node );

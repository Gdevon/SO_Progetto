#pragma once
#include "linked_list.h"
#include "FileHandle.h"
#include "DirHandle.h"
typedef enum{
    FILE_HANDLE,
    DIR_HANDLE
}Handle_Type;

typedef struct{
    ListItem h;
    void* handle;
    Handle_Type type;
}Handle_Item;
void Handle_Item_destroy(ListItem* );
void Handle_Item_print(ListItem*);
void Handle_Item_create(Handle_Item* , void*, Handle_Type );



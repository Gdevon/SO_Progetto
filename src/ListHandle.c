#include "ListHandle.h"
#include <stdlib.h>
ListItemOps handle_ops = {
    .dtor_fn = Handle_Item_destroy,
    .print_fn = Handle_Item_print
};
void Handle_Item_create(Handle_Item* item, void*handle, Handle_Type type){
    ListItem_construct(&item->h,&handle_ops);
    item->handle = handle;
    item->type = type;
}
void Handle_Item_destroy(ListItem* item){
    Handle_Item* h = (Handle_Item*) item;
    if(h->type == FILE_HANDLE){
        FileHandle* fh = (FileHandle*) h->handle;
        fh->open = 0;
    }else{
        DirHandle* dh = (DirHandle*) h->handle;
        dh->open = 0;
        if(dh->entries){
        }
    }
    free(h);
}
void Handle_Item_print(ListItem* l){
    return;
}
#pragma once

#include "stdint.h"

namespace edge::drivers {

typedef struct node_t {

    uint32_t id;
    uint32_t freq;

    //callback
    etl::delegate<void()> callback;
    

    uint32_t timer_value;
    struct node_t* next;


} node_t;


void list_insert_sorted(node_t* node);

node_t* list_get_first();

node_t* list_remove_first();

void list_remove(node_t* node);

} // namespace edge::drivers

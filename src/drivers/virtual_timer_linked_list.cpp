#include "drivers/virtual_timer_linked_list.hpp"

namespace edge::drivers {
static node_t* linked_list = nullptr;

void list_insert_sorted(node_t* node)
{
    if (linked_list == nullptr) {
        node->next = nullptr;
        linked_list = node;
    }
    else {
        if (node->timer_value < linked_list->timer_value) {
            node->next = linked_list;
            linked_list = node;
        }
        else {
            node_t* prev_node = linked_list;
            node_t* curr_node = linked_list->next;
            while (curr_node != nullptr && curr_node->timer_value < node->timer_value) {
                prev_node = curr_node;
                curr_node = curr_node->next;
            }
            prev_node->next = node;
            node->next = curr_node;
        }
    }
}

node_t* list_get_first()
{
    return linked_list;
}

node_t* list_remove_first()
{
    node_t* head = linked_list;
    if (head != nullptr) {
        linked_list = head->next;
    }
    return head;
}

void list_remove(node_t* node)
{
    if (linked_list != nullptr) {
        if (linked_list == node) {
            linked_list = linked_list->next;
        }
        else {
            node_t* prev_node = linked_list;
            node_t* curr_node = linked_list->next;
            while (curr_node != nullptr && curr_node != node) {
                prev_node = curr_node;
                curr_node = curr_node->next;
            }
            if (curr_node != nullptr) {
                prev_node->next = curr_node->next;
            }
        }
    }
}

} // namespace edge::drivers

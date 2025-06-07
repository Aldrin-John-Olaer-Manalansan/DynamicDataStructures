/*
 * @File: SinglyLinkedList.c
 * @Author: Aldrin John O. Manalansan (ajom)
 * @Email: aldrinjohnolaermanalansan@gmail.com
 * @Brief: Singly Linked List library source code
 * @LastUpdate: June 7, 2025
 * 
 * Copyright (C) 2025  Aldrin John O. Manalansan  <aldrinjohnolaermanalansan@gmail.com>
 * 
 * This Source Code is served under Open-Source AJOM License
 * You should have received a copy of License_OS-AJOM
 * along with this source code. If not, see:
 * <https://raw.githubusercontent.com/Aldrin-John-Olaer-Manalansan/AJOM_License/refs/heads/main/LICENSE_AJOM-OS>
 */

#include "SinglyLinkedList.h"

/*
 * Deletes all nodes found on the list
 */
void SinglyLinkedList_DeleteAllNodes(td_SinglyLinkedList_info *info) {
    if (info->first) {
        td_SinglyLinkedList_node *node = info->first;
        do { // traverse
            if (node->data) {
                free(node->data);
            }
            td_SinglyLinkedList_node *deleted = node;
            node = node->next;
            free(deleted);
        } while (node);
        
        info->first = NULL;
        info->indexed = NULL;
        info->last = NULL;
    }
}

/*
 * Creates a new node at the end of the list and returns its pointer
 * Returns NULL if node creation fails
 */
td_SinglyLinkedList_node* SinglyLinkedList_AddNode(td_SinglyLinkedList_info *info, const void *data) {
    td_SinglyLinkedList_node *newNode = malloc(sizeof(td_SinglyLinkedList_node));
    if (newNode) { // allocated successfully
        newNode->data = malloc(info->dataSize);
        if (!newNode->data) { // failed to allocate memory
            free(newNode); // cancel node creation
        } else {
            newNode->next = NULL;
            if (!info->first) { // first node doesn't exist yet
                info->first = newNode;
            }
            if (info->last) { // last node exists
                info->last->next = newNode; // chain the last node to the new node
            }
            info->last = newNode; // update last node to newly created node
            return newNode;
        }
    }
    return NULL; // node creation failed
}

/*
 * When trueOnce = true, Deletes only the first node which the InspectorFunction evaluated as true
 * When trueOnce = false, Deletes all nodes which the InspectorFunction evaluated as true
 * Returns the number of nodes deleted from the list
 */
uint32_t SinglyLinkedList_DeleteNodeByCondition(td_SinglyLinkedList_info *info, bool (*InspectorFunction)(const void*), bool trueOnce) {
    uint32_t deletedCount = 0;
    if (info->first) {
        td_SinglyLinkedList_node *prev = 0;
        td_SinglyLinkedList_node *node = info->first;
        do { // traverse
            td_SinglyLinkedList_node *nextNode = node->next;
            if (!(*InspectorFunction)(node->data)) {
                prev = node;
            } else { // inspected node matches the required condition
                    // delete the node
                if (info->first == node) {
                    info->first = node->next;
                }
                if (info->last == node) {
                    info->last = prev;
                    if (info->indexed == node) {
                        info->indexed = info->first;
                    }
                } else if (info->indexed == node) {
                    info->indexed = node->next;
                }
                if (prev) {
                    prev->next = node->next;
                }
                free(node->data);
                free(node);
                    //

                deletedCount++;
                if (trueOnce) {
                    return deletedCount;
                }
            }
            node = nextNode;
        } while (node);
    }
    return deletedCount;
}

/*
 * Executes the passed function to every nodes on the list
 */
void SinglyLinkedList_ExecuteFunctionForEachNode(td_SinglyLinkedList_info *info, void (*ExecutedFunction)(const void*)) {
    uint32_t deletedCount = 0;
    if (info->first) {
        td_SinglyLinkedList_node *node = info->first;
        do { // traverse
            (*ExecutedFunction)(node->data);
            node = node->next;
        } while (node);
    }
}

void SinglyLinkedList_Reset(td_SinglyLinkedList_info *info, const size_t size) {
    SinglyLinkedList_DeleteAllNodes(info);
    info->dataSize = size;
}
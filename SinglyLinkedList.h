/*
 * @File: SinglyLinkedList.h
 * @Author: Aldrin John O. Manalansan (ajom)
 * @Email: aldrinjohnolaermanalansan@gmail.com
 * @Brief: Attempted Implementation of Singly Linked List
 * @LastUpdate: June 7, 2025
 * 
 * Copyright (C) 2025  Aldrin John O. Manalansan  <aldrinjohnolaermanalansan@gmail.com>
 * 
 * This Source Code is served under Open-Source AJOM License
 * You should have received a copy of License_OS-AJOM
 * along with this source code. If not, see:
 * <https://raw.githubusercontent.com/Aldrin-John-Olaer-Manalansan/AJOM_License/refs/heads/main/LICENSE_AJOM-OS>
 */

#ifndef SINGLYLINKEDLIST_H
#define SINGLYLINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct td_SinglyLinkedList_node {
	void *data;
	struct td_SinglyLinkedList_node *next;
} td_SinglyLinkedList_node;

typedef struct {
	size_t dataSize;
	td_SinglyLinkedList_node *first;
	td_SinglyLinkedList_node *indexed;
	td_SinglyLinkedList_node *last;
} td_SinglyLinkedList_info;

void SinglyLinkedList_DeleteAllNodes(td_SinglyLinkedList_info *info);
td_SinglyLinkedList_node* SinglyLinkedList_AddNode(td_SinglyLinkedList_info *info, const void *data);
uint32_t SinglyLinkedList_DeleteNodeByCondition(td_SinglyLinkedList_info *info, bool (*InspectorFunction)(const void*), bool trueOnce);
void SinglyLinkedList_ExecuteFunctionForEachNode(td_SinglyLinkedList_info *info, void (*ExecutedFunction)(const void*));
void SinglyLinkedList_Reset(td_SinglyLinkedList_info *info, const size_t size);

#endif /* SINGLYLINKEDLIST_H */
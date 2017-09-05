#ifndef __MY_LIST_H__
#define __MY_LIST_H__
/*�Լ�ʵ�ֵ�˫��ѭ������,*/
/*��Ҫ������ bug :
	��������û��ָ��
	struct �в����� ����
	push_back ָ������� 

	ָ���ֵ��ֵ����ֱ�Ӹ�ֵ
*/

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
//#include <iostream>
//using namespace std;

typedef char *   elem_type;
typedef char *  elem_pointer_type;

typedef struct ND
{
	elem_type data;
	struct ND  *front, *next;
}node_type, *list_pointer_type, * node_pointer_type;

typedef struct
{
	int  length;
	node_pointer_type header, trailer;  //ͷ��β�ڱ�
}my_list, * mylist_pointer_type;

// ��������
list_pointer_type creat_list(mylist_pointer_type lst_ptr);

// �ͷ������ڴ�
void delete_list(mylist_pointer_type lst_ptr);

//��position ��λ��֮ǰ����Ԫ��e (1��i�ܱ�+1 )
/* s
	����ֵ��
		�ɹ��� ����ֵ��λ��
		ʧ�ܣ� NULL
*/
node_pointer_type insert(mylist_pointer_type lst_ptr, int position, elem_type data);

node_pointer_type insert_p( node_pointer_type ndptr, elem_type data);

//��������Ŀ�ʼλ��
node_pointer_type begin(my_list lst);

//�����������Ԫ�ص���һ��λ�ã�����������Ԫ��
node_pointer_type end(my_list lst);

// ��ȡ��һ���ڵ������
elem_type front(my_list lst);

// ��ȡ���һ���ڵ������
elem_type back(my_list lst);

//�������Ԫ��
void clear(mylist_pointer_type lst_ptr);

//��������Ĵ�С
int size(my_list lst);

//�Ƿ�Ϊ��,
//	��: 	��0
//	�ǿ�:	0
int empty(my_list lst);

//ǰ��
void push_front(mylist_pointer_type lst_ptr, elem_type data);

//���
void push_back(mylist_pointer_type lst_ptr, elem_type data);

// ǰ���� ɾ����ǰ��Ԫ��
int pop_front(mylist_pointer_type lst_ptr);

// ��  ɾ������Ԫ��
int pop_back(mylist_pointer_type lst_ptr);

//ɾ�� ������ Ԫ��
node_pointer_type erase(mylist_pointer_type lst_ptr, node_pointer_type first, node_pointer_type last);

// ��õ� position ��λ��
node_pointer_type get_pointer(my_list lst, int position);

//��ȡֵ
elem_type get_elem(my_list lst, int position);

//��ȡ��ĳ�ڵ��� ��position���ڵ�  --->  ѭ������
node_pointer_type get_elem_next_pos_nd(my_list lst, node_pointer_type ndptr_prev, int position);

//��ȡ��ĳ�ڵ�ǰ�� ��position���ڵ�  --->  ѭ������
node_pointer_type get_elem_front_pos_nd(my_list lst, node_pointer_type ndptr_prev, int position);

// ��ȡ�ڵ��ǰһ��Ԫ��  ѭ������
node_pointer_type get_front_node(my_list lst, node_pointer_type  ndptr);

// ��ȡ�ڵ�ĺ�һ��Ԫ��  ѭ������
node_pointer_type get_next_node(my_list lst, node_pointer_type  ndptr);

#endif


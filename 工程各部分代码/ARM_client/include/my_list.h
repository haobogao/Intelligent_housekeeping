#ifndef __MY_LIST_H__
#define __MY_LIST_H__
/*自己实现的双向循环链表,*/
/*主要遇到的 bug :
	函数传参没用指针
	struct 中不能有 函数
	push_back 指针误操作 

	指针的值赋值不能直接赋值
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
	node_pointer_type header, trailer;  //头、尾哨兵
}my_list, * mylist_pointer_type;

// 创建链表
list_pointer_type creat_list(mylist_pointer_type lst_ptr);

// 释放链表内存
void delete_list(mylist_pointer_type lst_ptr);

//在position 个位置之前插入元素e (1≤i≤表长+1 )
/* s
	返回值：
		成功： 插入值的位置
		失败： NULL
*/
node_pointer_type insert(mylist_pointer_type lst_ptr, int position, elem_type data);

node_pointer_type insert_p( node_pointer_type ndptr, elem_type data);

//返回链表的开始位置
node_pointer_type begin(my_list lst);

//返回链表最后元素的下一个位置，链表包含标兵元素
node_pointer_type end(my_list lst);

// 获取第一个节点的数据
elem_type front(my_list lst);

// 获取最后一个节点的数据
elem_type back(my_list lst);

//清除所有元素
void clear(mylist_pointer_type lst_ptr);

//返回链表的大小
int size(my_list lst);

//是否为空,
//	空: 	非0
//	非空:	0
int empty(my_list lst);

//前插
void push_front(mylist_pointer_type lst_ptr, elem_type data);

//后插
void push_back(mylist_pointer_type lst_ptr, elem_type data);

// 前弹， 删除最前个元素
int pop_front(mylist_pointer_type lst_ptr);

// 后弹  删除最后个元素
int pop_back(mylist_pointer_type lst_ptr);

//删除 区间内 元素
node_pointer_type erase(mylist_pointer_type lst_ptr, node_pointer_type first, node_pointer_type last);

// 获得第 position 的位置
node_pointer_type get_pointer(my_list lst, int position);

//获取值
elem_type get_elem(my_list lst, int position);

//获取在某节点后的 第position个节点  --->  循环操作
node_pointer_type get_elem_next_pos_nd(my_list lst, node_pointer_type ndptr_prev, int position);

//获取在某节点前的 第position个节点  --->  循环操作
node_pointer_type get_elem_front_pos_nd(my_list lst, node_pointer_type ndptr_prev, int position);

// 获取节点的前一个元素  循环操作
node_pointer_type get_front_node(my_list lst, node_pointer_type  ndptr);

// 获取节点的后一个元素  循环操作
node_pointer_type get_next_node(my_list lst, node_pointer_type  ndptr);

#endif


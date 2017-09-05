#include "my_list.h"

list_pointer_type creat_list(mylist_pointer_type lst_ptr) {
	node_pointer_type  ndptr_nil =  (node_pointer_type)malloc (sizeof(node_type));
	if (ndptr_nil == NULL) return ndptr_nil;

	ndptr_nil->data = NULL;

	ndptr_nil->front = ndptr_nil->next  = ndptr_nil;
	lst_ptr->length = 0;

	lst_ptr->trailer  =  lst_ptr->header  =  ndptr_nil;

	return ndptr_nil;
}

void delete_list(mylist_pointer_type lst_ptr) {
	//释放所有申请的内存空间
	node_pointer_type ndptr =  lst_ptr->header;
	node_pointer_type ndptr_f =  lst_ptr->header;
	while (ndptr->data != NULL) {
		ndptr_f = ndptr;
		free(ndptr_f);
		ndptr = ndptr->next;
	}

	free(ndptr);
	lst_ptr->length = 0;
}

//在position 个位置之前插入元素e (1≤i≤表长+1 )
/*
	返回值：
		成功： 插入值的位置
		失败： NULL
*/
node_pointer_type insert(mylist_pointer_type lst_ptr, int position, elem_type data) {
	node_pointer_type ndptr = lst_ptr->header + position;

	node_pointer_type  ndptr_dt =  (node_pointer_type) malloc (sizeof(node_type));
	elem_pointer_type elem_ptr =  (elem_pointer_type) malloc (strlen(data)+1);

	// ndptr_dt->data = data;
	memset(ndptr_dt, 0, sizeof(node_type));
	memset(elem_ptr, 0, strlen(data)+1);
	memcpy(elem_ptr, data, strlen(data)+1);
	ndptr_dt->data = elem_ptr;

	// 插数据
	ndptr_dt->front = ndptr->front;
	ndptr->front->next = ndptr_dt;
	ndptr_dt->next  = ndptr;
	ndptr->front = ndptr_dt;

	lst_ptr->length++;

	return ndptr_dt;
}

node_pointer_type insert_p(node_pointer_type ndptr, elem_type data){
	node_pointer_type  ndptr_dt = (node_pointer_type) malloc (sizeof(node_type));
	elem_pointer_type elem_ptr =  (elem_pointer_type) malloc (strlen(data)+1);

	// ndptr_dt->data = data;
	memset(ndptr_dt, 0, sizeof(node_type));
	memset(elem_ptr, 0, strlen(data)+1);
	memcpy(elem_ptr, data, strlen(data)+1);
	ndptr_dt->data = elem_ptr;

	// 插数据
	ndptr_dt->front = ndptr->front;
	ndptr->front->next = ndptr_dt;
	ndptr_dt->next  = ndptr;
	ndptr->front = ndptr_dt;

	return ndptr_dt;
}

//返回链表的开始位置
node_pointer_type begin(my_list lst) {
	return lst.header;
}

//返回链表最后元素的下一个位置，链表包含标兵元素
node_pointer_type end(my_list lst) {
	return lst.trailer;
}

// 获取第一个节点的数据
elem_type front(my_list lst) {
	return lst.header->data;
}

// 获取最后一个节点的数据
elem_type back(my_list lst) {
	return lst.trailer->front->data;
}

//清除所有元素
void clear(mylist_pointer_type lst_ptr) {
	//释放所有申请的内存空间
	node_pointer_type ndptr = lst_ptr->header;
	node_pointer_type ndptr_f =  lst_ptr->header;

	while (ndptr->data != NULL) {
		ndptr_f = ndptr;
		free(ndptr_f);
		ndptr = ndptr->next;
	}

	lst_ptr->header = lst_ptr->trailer;
	lst_ptr->trailer->front = lst_ptr->trailer->next  = lst_ptr->trailer;

	//保留尾哨兵
	lst_ptr->length = 0;
}

//返回链表的大小
int size(my_list lst) {
	return lst.length;
}

//是否为空,
//	空: 	非0
//	非空:	0
int empty(my_list lst) {
	return (lst.length == 0);
}

//前插
void push_front(mylist_pointer_type lst_ptr, elem_type data) {
	node_pointer_type  ndptr_dt =  (node_pointer_type) malloc (sizeof(node_type));
	elem_pointer_type elem_ptr =  (elem_pointer_type) malloc (strlen(data)+1);

	// ndptr_dt->data = data;
	memset(ndptr_dt, 0, sizeof(node_type));
	memset(elem_ptr, 0, strlen(data)+1);
	memcpy(elem_ptr, data, strlen(data)+1);
	ndptr_dt->data = elem_ptr;

	//头插数据
	ndptr_dt->front = lst_ptr->header->front;
	ndptr_dt->next  = lst_ptr->header;
	lst_ptr->header->front->next = ndptr_dt;
	lst_ptr->header->front = ndptr_dt;

	lst_ptr->header = ndptr_dt;

	lst_ptr->length++;
}

//后插
void push_back(mylist_pointer_type lst_ptr, elem_type data) {
	node_pointer_type  ndptr_dt =  (node_pointer_type) malloc (sizeof(node_type));
	elem_pointer_type elem_ptr =  (elem_pointer_type) malloc (strlen(data)+1);

	memset(ndptr_dt, 0, sizeof(node_type));
	memset(elem_ptr, 0, strlen(data)+1);
	memcpy(elem_ptr, data, strlen(data)+1);
	ndptr_dt->data = elem_ptr;

	// ndptr_dt->data = data;

	// 注意初始情况
	if( lst_ptr->length == 0) {
		lst_ptr->header = ndptr_dt;
	}

	//尾插数据
	lst_ptr->trailer->front->next = ndptr_dt;
	ndptr_dt->front = lst_ptr->trailer->front;
	ndptr_dt->next  = lst_ptr->trailer;
	lst_ptr->trailer->front = ndptr_dt;

//	lst_ptr->trailer = ndptr_dt;

	lst_ptr->length++;
}

// 前弹， 删除最前个元素
int pop_front(mylist_pointer_type lst_ptr) {
	if (lst_ptr->length <= 0) return -1;

	node_pointer_type  ndptr  = lst_ptr->header;
	node_pointer_type  ndptr_f  = lst_ptr->header;

	lst_ptr->header = ndptr->next;
	lst_ptr->header->front = lst_ptr->trailer;
	lst_ptr->trailer->next = lst_ptr->header;

	free(ndptr_f);
	lst_ptr->length--;

	return 0;

}

// 后弹  删除最后个元素
int pop_back(mylist_pointer_type lst_ptr) {
	if (lst_ptr->length <= 0) return -1;

	node_pointer_type  ndptr  = lst_ptr->trailer->front;

	lst_ptr->trailer->front = ndptr->front;
	ndptr->front->next = lst_ptr->trailer;

	free(ndptr);
	lst_ptr->length--;

	return 0;
}

//删除 区间 [first, last) 内 元素
node_pointer_type erase(mylist_pointer_type lst_ptr, node_pointer_type first, node_pointer_type last) {
	node_pointer_type ndptr_begin = first;
	node_pointer_type ndptr = first;

	while (	ndptr != last ) {
		first = first->next;
		// if (first == lst_ptr->trailer) first = first->next;
		lst_ptr->length--;
		free(ndptr);
		ndptr = first;
	}

	last->front = ndptr_begin;
	ndptr_begin->next = last;

	return ndptr_begin;
}

// 获得第 position 的位置
node_pointer_type get_pointer(my_list lst, int position) {
	node_pointer_type ndptr = lst.header;

	if (position == 0) return  ndptr;
	
	while (position--) {
		if (ndptr->next == NULL) {
			return NULL;
		}
		ndptr = ndptr->next;
	}

	return ndptr;
}

//获取值
elem_type get_elem(my_list lst, int position) {
	node_pointer_type ndptr = lst.header;

	if (position == 0) return  ndptr->data;

	while (position--) {
		if (ndptr->next == NULL) {
			return NULL;
		}
		ndptr = ndptr->next;
	}
	return  ndptr->data;
}

//获取在某节点后的 第position个节点  --->  循环操作
node_pointer_type get_elem_next_pos_nd(my_list lst, node_pointer_type ndptr_prev, int position) {
	
	if (position < 0)	   return NULL;
	if (position == 0) return  ndptr_prev;

	while (position--) {
		ndptr_prev = get_next_node(lst, ndptr_prev);
	}
	return  ndptr_prev;
}

//获取在某节点前的 第position个节点  --->  循环操作
node_pointer_type get_elem_front_pos_nd(my_list lst, node_pointer_type ndptr_prev, int position) {
	
	if (position < 0)	   return NULL;
	if (position == 0) return  ndptr_prev;

	while (position--) {
		ndptr_prev = get_front_node(lst, ndptr_prev);
	}
	return  ndptr_prev;
}

// 获取节点的前一个元素  循环操作
node_pointer_type get_front_node(my_list lst, node_pointer_type  ndptr) {

	if (ndptr->front == lst.header ) {
		return lst.trailer->front;
	}

	return ndptr->front;
}

// 获取节点的后一个元素  循环操作
node_pointer_type get_next_node(my_list lst, node_pointer_type  ndptr) {

	if (ndptr->next == lst.trailer ) {
		return lst.header;
	}

	return ndptr->next;
}
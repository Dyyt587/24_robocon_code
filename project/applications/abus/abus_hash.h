#ifndef ABUS_HASH_H
#define ABUS_HASH_H
#ifdef __cplsusplus
extern "C" {
#endif


#define MaxSize 10
#define NullKey 32768
#define ABUS_HASH_PRINTF printf
// 链表结点
typedef struct LNode
{
    const char* name;
    void* data;
    struct LNode* next;
}LNode, * LinkList; //LNode node; // 声明一个单个结点        LinkList list; // 声明一个链表的指针

// 哈希表
typedef struct hashTable
{
    LinkList list;// 声明一个链表的指针
    int count;
}hashTable;



void HashTableInit(hashTable* H);
void InsertHashKey(hashTable* H, const char* key,void*data);
void* SearchHashKey(hashTable H, const char* key);
void DisplayHashTable(hashTable H);


#ifdef __cplsusplus
}
#endif
#endif

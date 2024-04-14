/*
 * @Author: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @Date: 2024-04-13 14:56:01
 * @LastEditors: Dyyt587 67887002+Dyyt587@users.noreply.github.com
 * @LastEditTime: 2024-04-15 02:01:30
 * @FilePath: \project\applications\abus\abus_hash.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
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



hashTable* HashTableInit(void);
void InsertHashKey(hashTable* H, const char* key,void*data);
void* SearchHashKey(hashTable H, const char* key);
void DisplayHashTable(hashTable H);


#ifdef __cplsusplus
}
#endif
#endif

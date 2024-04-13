/*
 * @Author: Dyyt587 805207319@qq.com
 * @Date: 2024-04-13 09:47:14
 * @LastEditors: Dyyt587 805207319@qq.com
 * @LastEditTime: 2024-04-13 10:18:55
 * @FilePath: \project\applications\abus\abus_hash.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "abus_hash.h"
#include <math.h>


// 初始化哈希表
void HashTableInit(hashTable* H)
{
    // 分配多个链表头结点空间
    H->list = (LNode*)malloc(MaxSize * sizeof(LNode));//分配20个LNode大的元素空间 
    H->count = 0;

    // 初始化链表头结点（头结点存储初始值，链表后序结点存储冲突值）
    int i;
    for (i = 0; i < MaxSize; i++)
    {
        H->list[i].name = (const char*)NullKey;
        H->list[i].next = NULL;
    }
    ABUS_HASH_PRINTF("已初始化哈希表!\n");
}

// 定义哈希函数
unsigned int hashFunction(const char* str) {
    unsigned int hash = 0;

    for (int i = 0; str[i] != '\0'; i++) {
        hash += (int)str[i];
    }

    return hash % MaxSize;
}
// 交换两个节点的数据
void swap(LNode* a, LNode* b) {
    const char* temp;
    temp = a->name;

    a->name = b->name;

    b->name = temp;

}

// 对链表进行冒泡排序
void bubbleSort(LNode* head) {
    if (head == NULL || head->next == NULL) {
        return;
    }

    int swapped;
    LNode* current;
    LNode* last = NULL;

    do {
        swapped = 0;
        current = head;

        while (current->next != last) {
            if (strcmp(current->name, current->next->name) > 0) {
                swap(current, current->next);
                swapped = 1;
            }
            current = current->next;
        }
        last = current;
    } while (swapped);
}

// 插入关键字
void InsertHashKey(hashTable* H, const char* key,void *data)
{
    // 根据哈希函数得到下标地址
    int address = hashFunction(key);
    ABUS_HASH_PRINTF("\n插入关键字 %s, 哈希地址 = %d\n", key, address);

    if (H->list[address].name == (const char*)NullKey)
    {
        // 若未发生冲突，则直接赋值给头结点
        H->list[address].name = key;
        ABUS_HASH_PRINTF("插入下标地址[%d], 插入成功!\n\n", address);
    }
    else
    {
        // 若发生冲突，则在该头结点的链表下进行头插
        // 创建新结点
        LNode* s;
        s = (LNode*)malloc(sizeof(LNode));
        s->name = key;
        s->data = data;
        // 头插法插入
        s->next = H->list[address].next;//将头结点的next赋值给新结点的next，新值在前面
        H->list[address].next = s;

        bubbleSort(H->list[address].next);

        ABUS_HASH_PRINTF("插入下标地址[%d], 发生冲突! 已插入该地址的链表下\n\n", address);
    }
    H->count++;
}

// 查找关键字
void* SearchHashKey(hashTable H, const char* key)
{
    int address = hashFunction(key);
    // 根据哈希地址获取对应链表的头结点
    LNode* p = &H.list[address];
    // 在链表中查找关键字
    while (p != NULL && strcmp(p->name, key)!=0 )
    {
        // 线性探测
        p = p->next;
    }
    return p->data;
}

void DisplayHashTable(hashTable H)
{
    int i;
    ABUS_HASH_PRINTF("哈希表：\n**********************************\n");
    for (i = 0; i < MaxSize; i++)
    {
        ABUS_HASH_PRINTF("%d\t", i);
    }
    ABUS_HASH_PRINTF("\n");

    LNode* p;
    int j, k = 0, flag = 1;

    while (flag)
    {
        flag = 0;
        for (i = 0; i < MaxSize; i++)
        {
            j = 0;
            p = &H.list[i];
            while (j < k && p)
            {
                p = p->next;
                j++;
            }
            if (p && p->name != (const char*)NullKey)
            {
                ABUS_HASH_PRINTF("%s\t", p->name);
                flag = 1;
            }
            else
            {
                ABUS_HASH_PRINTF(" \t");
            }
        }
        k++;
        ABUS_HASH_PRINTF("\n");
    }
    ABUS_HASH_PRINTF("*********************************\n");
}


int main()
{
    int i;
    char key[10];
    hashTable H;
    HashTableInit(&H);
    ABUS_HASH_PRINTF("构建哈希表:");

    const char* key1[6] = { "a8","uart1","uart12","uart2","uart3","uart5"};

    for (i = 0; i < 6; i++)
    {
        // 参考用例: 50 81 191 32 65 75 9 97 88 109
        /*scanf("%s", key[i]);*/
        InsertHashKey(&H, key1[i]);
    }
    DisplayHashTable(H);

    while (1)
    {
        LNode* result;
        ABUS_HASH_PRINTF("查找关键字: ");
        scanf("%s", key);
        // 查找关键字地址
        result = SearchHashKey(H, key);
        if (result != NULL)
        {
            ABUS_HASH_PRINTF("查找成功, 目标地址 = %x\n", result);
        }
        else
        {
            ABUS_HASH_PRINTF("查找失败\n");
        }
    }
    return 0;
}


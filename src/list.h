/**
 * @file list.h
 */

#ifndef __LIST_H__
#define __LIST_H__ 1

/*
 * The offsetof macro is from include/linux/stddef.h,
 * the container_of macro is from include/linux/kernel.h,
 * the list manipulation macros are from include/linux/list.h
 */

struct list_head {
    struct list_head *next, *prev;
};

#undef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) & ((TYPE*)0)->MEMBER)

/**
 * container_of - cast a member of a structure out to the containing structure
 *
 * @param ptr the pointer to the member.
 * @param type the type of the container struct this is embedded in.
 * @param member the name of the member within the struct.
 */
#define container_of(ptr, type, member)                   \
    ({                                                    \
        const typeof(((type*)0)->member)* __mptr = (ptr); \
        (type*)((char*)__mptr - offsetof(type, member));  \
    })

#define LIST_HEAD_INIT(name) \
    {                        \
        &(name), &(name)     \
    }

#define LIST_HEAD(name) struct list_head name = LIST_HEAD_INIT(name)

#define INIT_LIST_HEAD(ptr)  \
    do {                     \
        (ptr)->next = (ptr); \
        (ptr)->prev = (ptr); \
    } while(0)

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_add(struct list_head* new, struct list_head* prev, struct list_head* next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

/**
 * list_add - add a new entry
 * @param new new entry to be added
 * @param head list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void list_add(struct list_head* new, struct list_head* head)
{
    __list_add(new, head, head->next);
}

/**
 * list_add_tail - add a new entry
 * @param new new entry to be added
 * @param head list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void list_add_tail(struct list_head* new, struct list_head* head)
{
    __list_add(new, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_del(struct list_head* prev, struct list_head* next)
{
    next->prev = prev;
    prev->next = next;
}

/**
 * list_del - deletes entry from list.
 * @param entry the element to delete from the list.
 * @note list_empty on entry does not return true after this, the entry is
 * in an undefined state.
 */
static inline void list_del(struct list_head* entry)
{
    __list_del(entry->prev, entry->next);
    INIT_LIST_HEAD(entry);
}

/**
 * list_move - delete from one list and add as another's head
 * @param list the entry to move
 * @param head the head that will precede our entry
 */
static inline void list_move(struct list_head* list, struct list_head* head)
{
    __list_del(list->prev, list->next);
    list_add(list, head);
}

/**
 * list_move_tail - delete from one list and add as another's tail
 * @param list the entry to move
 * @param head the head that will follow our entry
 */
static inline void list_move_tail(struct list_head* list, struct list_head* head)
{
    __list_del(list->prev, list->next);
    list_add_tail(list, head);
}

/**
 * list_empty - tests whether a list is empty
 * @param head the list to test.
 */
static inline int list_empty(const struct list_head* head)
{
    return head->next == head;
}

static inline void __list_splice(struct list_head* list, struct list_head* head)
{
    struct list_head* first = list->next;
    struct list_head* last = list->prev;
    struct list_head* at = head->next;

    first->prev = head;
    head->next = first;

    last->next = at;
    at->prev = last;
}

/**
 * list_splice - join two lists
 * @param list the new list to add.
 * @param head the place to add it in the first list.
 */
static inline void list_splice(struct list_head* list, struct list_head* head)
{
    if(!list_empty(list)) {
        __list_splice(list, head);
        INIT_LIST_HEAD(list);
    }
}

/**
 * list_entry - get the struct for this entry
 * @param ptr	the &struct list_head pointer.
 * @param type	the type of the struct this is embedded in.
 * @param member	the name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member) container_of(ptr, type, member)

/**
 * list_for_each	-	iterate over a list
 * @param pos	the &struct list_head to use as a loop counter.
 * @param head	the head for your list.
 */
#define list_for_each(pos, head) for(pos = (head)->next; pos != (head); pos = pos->next)

/**
 * list_for_each_prev	-	iterate over a list backwards
 * @param pos	the &struct list_head to use as a loop counter.
 * @param head	the head for your list.
 */
#define list_for_each_prev(pos, head) for(pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * list_for_each_safe	-	iterate over a list safe against removal of list entry
 * @param pos	the &struct list_head to use as a loop counter.
 * @param n		another &struct list_head to use as temporary storage
 * @param head	the head for your list.
 */
#define list_for_each_safe(pos, n, head) for(pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)

/**
 * Returns the size of the specified list.
 *
 * @param list the list that we need the size of
 * @return the number of elements present in the list
 */
static inline int list_size(const struct list_head* list)
{
    struct list_head* lp;
    int i = 0;

    list_for_each(lp, list)
    {
        i++;
    }

    return i;
}

#endif

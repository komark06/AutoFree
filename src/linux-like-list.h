#if !defined(__LINUX_LIKE_LIST_H__)
#define __LINUX_LIKE_LIST_H__

#ifdef __cplusplus
extern "C" {
#endif

struct list_head {
    struct list_head *prev;
    struct list_head *next;
};

static inline int list_empty(const struct list_head *head)
{
    return (head->next == head);
}

static inline void init_list_head(struct list_head *head)
{
    head->next = head;
    head->prev = head;
}

static inline void list_add(struct list_head *node, struct list_head *head)
{
    struct list_head *next = head->next;
    next->prev = node;
    node->next = next;
    node->prev = head;
    head->next = node;
}

static inline void list_del(struct list_head *node)
{
    struct list_head *next = node->next;
    struct list_head *prev = node->prev;
    next->prev = prev;
    prev->next = next;
}

/*
    If @node is existed on the list, return 1.
    Else, return 0.
*/
static inline int node_exist(const struct list_head *node,
                             struct list_head *head)
{
    if (node == head)
        return 1;
    struct list_head *tmp = head->next;
    while (tmp != head) {
        if (tmp == node)
            return 1;
        tmp = tmp->next;
    }
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif  // !defined(__LINUX_LIKE_LIST_H__)
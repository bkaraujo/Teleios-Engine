#include "teleios/container.h"
#include "teleios/logger.h"
#include "teleios/memory.h"

static const u64 MSIZE = sizeof(TLMap);
static const u64 ESIZE = sizeof(TLEntry);

TLMap* tl_map_create(void) {
    TLMap* container = tl_memory_alloc(TL_MEMORY_TYPE_CONTAINER_MAP, MSIZE);
    if (container == NULL) {
        TLERROR("tl_map_create: Failed to allocate memory");
        return NULL;
    }

    return container;
}

b8 tl_map_append(TLMap* container, u32 key, const void* payload) {
    if (container == NULL) {
        TLERROR("tl_map_append: container is null");
        return false;
    }

    if (payload == NULL) {
        TLERROR("tl_map_append: payload is null");
        return false;
    }

    if (container->size == 0) {
        TLEntry* entry = tl_memory_alloc(TL_MEMORY_TYPE_CONTAINER_NODE, ESIZE);
        entry->key = key;
        entry->values = tl_list_create();

        if (entry->values == NULL) {
            tl_memory_free(entry, TL_MEMORY_TYPE_CONTAINER_NODE, ESIZE);
            TLERROR("tl_map_append: Failed to create entry");
            return false;
        }

        if (!tl_list_append(entry->values, payload)) {
            tl_list_destroy(entry->values);
            tl_memory_free(entry, TL_MEMORY_TYPE_CONTAINER_NODE, ESIZE);
            TLERROR("tl_map_append: Failed to append at key %llu", key);
            return false;
        }

        container->head = entry;
    }
    else {
        if (container->head->key == key) {
            return tl_list_append(container->head->values, payload);
        }

        TLEntry* entry = container->head;
        while (entry->next != NULL) {
            entry = entry->next;
            if (entry->key != key) {
                continue;
            }

            return tl_list_append(entry->values, payload);
        }

        TLEntry* newentry = tl_memory_alloc(TL_MEMORY_TYPE_CONTAINER_NODE, ESIZE);
        newentry->key = key;
        newentry->values = tl_list_create();

        if (newentry->values == NULL) {
            tl_memory_free(newentry, TL_MEMORY_TYPE_CONTAINER_NODE, ESIZE);
            TLERROR("tl_map_append: Failed to create entry");
            return false;
        }

        if (!tl_list_append(newentry->values, payload)) {
            tl_list_destroy(newentry->values);
            tl_memory_free(newentry, TL_MEMORY_TYPE_CONTAINER_NODE, ESIZE);
            TLERROR("tl_map_append: Failed to append at key %llu", key);
            return false;
        }

        entry->next = newentry;
    }

    return true;
}

//struct nlist { /* table entry: */
//    struct nlist* next; /* next entry in chain */
//    char* name; /* defined name */
//    char* defn; /* replacement text */
//};
//
//#define HASHSIZE 101
//static struct nlist* hashtab[HASHSIZE]; /* pointer table */
//
///* hash: form hash value for string s */
//unsigned hash(char* s)
//{
//    unsigned hashval;
//    for (hashval = 0; *s != '\0'; s++)
//        hashval = *s + 31 * hashval;
//    return hashval % HASHSIZE;
//}
//
///* lookup: look for s in hashtab */
//struct nlist* lookup(char* s)
//{
//    struct nlist* np;
//    for (np = hashtab[hash(s)]; np != NULL; np = np->next)
//        if (strcmp(s, np->name) == 0)
//            return np; /* found */
//    return NULL; /* not found */
//}
//
//char* strdup(char*);
///* install: put (name, defn) in hashtab */
//struct nlist* install(char* name, char* defn)
//{
//    struct nlist* np;
//    unsigned hashval;
//    if ((np = lookup(name)) == NULL) { /* not found */
//        np = (struct nlist*)malloc(sizeof(*np));
//        if (np == NULL || (np->name = strdup(name)) == NULL)
//            return NULL;
//        hashval = hash(name);
//        np->next = hashtab[hashval];
//        hashtab[hashval] = np;
//    }
//    else /* already there */
//        free((void*)np->defn); /*free previous defn */
//    if ((np->defn = strdup(defn)) == NULL)
//        return NULL;
//    return np;
//}

#include "datastructures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------------------- common functions ---------------------------- */

/** Same as malloc except in case of allocation failure, print an error and
 * exit. */
static void *xmalloc(size_t size) {
	void *buffer = malloc(size);
	if (buffer) return buffer;
	printf("Out of memory error\n");
	exit(EXIT_FAILURE);
}

/** Same as realloc except in case of allocation failure, print an error and
 * exit. */
static void *xrealloc(void *ptr, size_t new_size) {
	void *buffer = realloc(ptr, new_size);
	if (buffer) return buffer;
	printf("Out of memory error\n");
	exit(EXIT_FAILURE);
}


/* -------------------------- variable size array -------------------------- */

/* Growth/shrink factor for arraylist */
#define ARRAYLIST_GROWTH_FACTOR 1.5

/* Percent threshold below which physical length of array is shrunk by shrink factor.
   Must be <= 1/ARRAYLIST_GROWTH_FACTOR */
#define ARRAYLIST_SHRINK_THRESHOLD 0.3

   /* Initial physical length of array */
#define ARRAYLIST_INIT_LEN 5

arraylist_t *arraylist_new(size_t elem_size, cmp_func_t cmp_func) {
	arraylist_t *new_arraylist = xmalloc(sizeof(arraylist_t));
	new_arraylist->len = 0;
	new_arraylist->elem_size = elem_size;
	if (new_arraylist->contents = malloc(ARRAYLIST_INIT_LEN * elem_size)) {
		new_arraylist->phys_len = ARRAYLIST_INIT_LEN;
	} else {
		new_arraylist->contents = xmalloc(elem_size);
		new_arraylist->phys_len = 1;
	}
	new_arraylist->end = new_arraylist->contents;
	new_arraylist->cmp_func = cmp_func;
	return new_arraylist;
}

arraylist_t *arraylist_from_array(const void *array, int64_t array_len, size_t elem_size, cmp_func_t cmp_func) {
	if (array_len == 0) return arraylist_new(elem_size, cmp_func);
	arraylist_t *new_arraylist = xmalloc(sizeof(arraylist_t));
	new_arraylist->len = array_len;
	new_arraylist->phys_len = array_len;
	new_arraylist->elem_size = elem_size;
	new_arraylist->contents = xmalloc(array_len * elem_size);
	memcpy(new_arraylist->contents, array, array_len * elem_size);
	new_arraylist->end = new_arraylist->contents + array_len * elem_size;
	new_arraylist->cmp_func = cmp_func;
	return new_arraylist;
}

void arraylist_free(arraylist_t *arraylist) {
	free(arraylist->contents);
	free(arraylist);
}

int64_t arraylist_len(const arraylist_t *arraylist) {
	return arraylist->len;
}

void *arraylist_set(arraylist_t *arraylist, int64_t index, const void *value) {
	if (index < -arraylist->len || index >= arraylist->len) return NULL;
	if (index < 0) index += arraylist->len;
	int8_t *elem_location = arraylist->contents + index * arraylist->elem_size;
	memmove(elem_location, value, arraylist->elem_size);
	return elem_location;
}

void *arraylist_get(const arraylist_t *arraylist, int64_t index) {
	if (index < -arraylist->len || index >= arraylist->len) return NULL;
	if (index < 0) index += arraylist->len;
	return arraylist->contents + index * arraylist->elem_size;
}

void *arraylist_get_copy(const arraylist_t *arraylist, int64_t index, void *dest) {
	void *ptr = arraylist_get(arraylist, index);
	if (ptr != NULL) memmove(dest, ptr, arraylist->elem_size);
	return ptr;
}

/** If the virtual length of `arraylist` is equal to its physical length,
 * increase the physical length of the arraylist by a factor of
 * ARRAYLIST_GROWTH_FACTOR or by 1, whichever results in a larger array. Leave
 * the virtual length unchanged. If memory allocation fails, increase the
 * physical length by 1. */
static void arraylist_grow(arraylist_t *arraylist) {
	if (arraylist->phys_len != arraylist->len) return;
	int64_t phys_len_new = max((int64_t)(ARRAYLIST_GROWTH_FACTOR * arraylist->phys_len), arraylist->phys_len + 1);
	int8_t *contents_new = realloc(arraylist->contents, phys_len_new * arraylist->elem_size);
	if (contents_new) {
		arraylist->phys_len = phys_len_new;
		arraylist->contents = contents_new;
	} else {
		arraylist->contents = xrealloc(arraylist->contents, ++(arraylist->phys_len));
	}
	arraylist->end = arraylist->contents + arraylist->len * arraylist->elem_size;
}

/** If the virtual length of `arraylist` is greater than or equal to
 * ARRAYLIST_INIT_LEN * ARRAYLIST_GROWTH_FACTOR and
 * is at least a factor of ARRAYLIST_GROWTH_FACTOR smaller than its physical
 * length, shrink the physical length by ARRAYLIST_GROWTH_FACTOR. Otherwise, do
 * nothing. */
static void arraylist_shrink(arraylist_t *arraylist) {
	if ((int64_t)(arraylist->phys_len / ARRAYLIST_GROWTH_FACTOR) >= ARRAYLIST_INIT_LEN &&
		arraylist->len <= (int64_t)(arraylist->phys_len * ARRAYLIST_SHRINK_THRESHOLD)) {
		arraylist->phys_len = (int64_t)(arraylist->phys_len / ARRAYLIST_GROWTH_FACTOR);
		arraylist->contents = xrealloc(arraylist->contents, arraylist->phys_len * arraylist->elem_size);
		arraylist->end = arraylist->contents + arraylist->len * arraylist->elem_size;
	}
}

void arraylist_append(arraylist_t *arraylist, const void *value) {
	arraylist_grow(arraylist);
	memmove(arraylist->end, value, arraylist->elem_size);
	arraylist->len++;
	arraylist->end += arraylist->elem_size;
}

void arraylist_extend(arraylist_t *dest, const arraylist_t *source) {
	for (int8_t *value = source->contents; value < source->end; value += source->elem_size) {
		arraylist_append(dest, value);
	}
}

void arraylist_insert(arraylist_t *arraylist, int64_t index, const void *value) {
	if (index < -arraylist->len) index = 0;
	else if (index < 0) index += arraylist->len;
	else if (index > arraylist->len) index = arraylist->len;
	arraylist_grow(arraylist);
	memmove(arraylist->contents + (index + 1) * arraylist->elem_size,
		arraylist->contents + index * arraylist->elem_size,
		(arraylist->len - index) * arraylist->elem_size);
	memmove(arraylist->contents + index * arraylist->elem_size, value, arraylist->elem_size);
	arraylist->len++;
	arraylist->end += arraylist->elem_size;
}

bool arraylist_contains(const arraylist_t *arraylist, const void *value) {
	for (int8_t *current = arraylist->contents; current < arraylist->end; current += arraylist->elem_size) {
		if (!arraylist->cmp_func(current, value)) {
			return true;
		}
	}
	return false;
}

bool arraylist_remove(arraylist_t *arraylist, const void *value) {
	for (int8_t *current = arraylist->contents; current < arraylist->end; current += arraylist->elem_size) {
		if (!arraylist->cmp_func(current, value)) {
			memmove(current, current + arraylist->elem_size,
				arraylist->end - current - arraylist->elem_size);
			arraylist->len--;
			arraylist->end -= arraylist->elem_size;
			arraylist_shrink(arraylist);
			return true;
		}
	}
	return false;
}

bool arraylist_delete(arraylist_t *arraylist, int64_t index) {
	if (index < -arraylist->len || index >= arraylist->len) return false;
	if (index < 0) index += arraylist->len;
	memmove(arraylist->contents + index * arraylist->elem_size,
		arraylist->contents + (index + 1) * arraylist->elem_size,
		(arraylist->len - index - 1) * arraylist->elem_size);
	arraylist->len--;
	arraylist->end -= arraylist->elem_size;
	arraylist_shrink(arraylist);
	return true;
}

arraylist_t *arraylist_slice(arraylist_t *arraylist, int64_t start, int64_t end) {
	if (start < -arraylist->len) start = 0;
	else if (start < 0) start += arraylist->len;
	else if (start >= arraylist->len) start = arraylist->len;
	if (end < -arraylist->len) end = 0;
	else if (end < 0) end += arraylist->len;
	else if (end >= arraylist->len) end = arraylist->len;
	if (start >= end) {
		return arraylist_new(arraylist->elem_size, arraylist->cmp_func);
	}
	return arraylist_from_array(arraylist->contents + start * arraylist->elem_size,
		end - start, arraylist->elem_size, arraylist->cmp_func);
}

bool arraylist_pop(arraylist_t *arraylist, int64_t index, void *dest) {
	if (!arraylist_get_copy(arraylist, index, dest)) return false;
	return arraylist_delete(arraylist, index);
}

void arraylist_clear(arraylist_t *arraylist) {
	arraylist->len = 0;
	int8_t *contents_new = realloc(arraylist->contents, ARRAYLIST_INIT_LEN * arraylist->elem_size);
	if (contents_new) {
		arraylist->phys_len = ARRAYLIST_INIT_LEN;
		arraylist->contents = contents_new;
	} else {
		arraylist->phys_len = 1;
		arraylist->contents = xrealloc(arraylist->contents, arraylist->elem_size);
	}
	arraylist->end = arraylist->contents;
}

int64_t arraylist_find(const arraylist_t *arraylist, const void *value) {
	int64_t i = 0;
	for (int8_t *current = arraylist->contents; current < arraylist->end; current += arraylist->elem_size, i++) {
		if (!arraylist->cmp_func(current, value)) {
			return i;
		}
	}
	return -1;
}

int64_t arraylist_rfind(const arraylist_t *arraylist, const void *value) {
	int64_t i = arraylist->len - 1;
	for (int8_t *current = arraylist->end - arraylist->elem_size; current >= arraylist->contents; current -= arraylist->elem_size, i--) {
		if (!arraylist->cmp_func(current, value)) {
			return i;
		}
	}
	return -1;
}

int64_t arraylist_count(const arraylist_t *arraylist, const void *value) {
	int64_t count = 0;
	for (int8_t *current = arraylist->contents; current < arraylist->end; current += arraylist->elem_size) {
		if (!arraylist->cmp_func(current, value)) {
			count++;
		}
	}
	return count;
}

void arraylist_sort(arraylist_t *arraylist) {
	// minrun should be in the range [32,64] such that the number of minruns
	// in the array is slightly less than or equal to a power of 2
	int64_t minrun = arraylist->len;
	int remainder = 0;
	while (minrun > 64) {
		remainder |= minrun & 1;
		minrun >>= 1;
	}
	minrun += remainder;
	// TODO complete this function
}

void arraylist_reverse(arraylist_t *arraylist) {
	void *temp = xmalloc(arraylist->elem_size);
	int8_t *a = arraylist->contents;
	int8_t *b = arraylist->contents + (arraylist->len - 1) * arraylist->elem_size;
	while (a < b) {
		memcpy(temp, a, arraylist->elem_size);
		memcpy(a, b, arraylist->elem_size);
		memcpy(b, temp, arraylist->elem_size);
		a += arraylist->elem_size;
		b -= arraylist->elem_size;
	}
	free(temp);
}

arraylist_t *arraylist_copy(const arraylist_t *arraylist) {
	arraylist_t *copy = xmalloc(sizeof(arraylist_t));
	copy->len = arraylist->len;
	copy->elem_size = arraylist->elem_size;
	if (copy->contents = malloc(arraylist->phys_len * arraylist->elem_size)) {
		copy->phys_len = arraylist->phys_len;
	} else {
		copy->phys_len = max(arraylist->len, 1);
		copy->contents = xmalloc(copy->phys_len * arraylist->elem_size);
	}
	memcpy(copy->contents, arraylist->contents, arraylist->len * arraylist->elem_size);
	copy->end = copy->contents + copy->len * copy->elem_size;
	copy->cmp_func = arraylist->cmp_func;
	return copy;
}

int64_t arraylist_compare(const arraylist_t *arraylist1, const arraylist_t *arraylist2) {
	int8_t *value1, *value2;
	for (value1 = arraylist1->contents, value2 = arraylist2->contents;
		value1 < arraylist1->end && value2 < arraylist2->end;
		value1 += arraylist1->elem_size, value2 += arraylist2->elem_size) {
		int64_t cmp = arraylist1->cmp_func(value1, value2);
		if (cmp) return cmp;
	}
	if (value1 < arraylist1->end) return 1;
	else if (value2 < arraylist2->end) return -1;
	else return 0;
}

void arraylist_foreach(arraylist_t *arraylist, void(*func)(void*)) {
	for (int8_t *value = arraylist->contents; value < arraylist->end; value += arraylist->elem_size) {
		func(value);
	}
}

arraylist_iter_t *arraylist_iter_new(const arraylist_t *arraylist) {
	arraylist_iter_t *iter = xmalloc(sizeof(arraylist_iter_t));
	iter->arraylist = arraylist;
	iter->next = arraylist->contents;
	return iter;
}

void arraylist_iter_free(arraylist_iter_t *iter) {
	free(iter);
}

void *arraylist_iter_next(arraylist_iter_t *iter) {
	if (iter->next == iter->arraylist->end) return NULL;
	else {
		void *next = iter->next;
		iter->next += iter->arraylist->elem_size;
		return next;
	}
}

void arraylist_iter_reset(arraylist_iter_t *iter) {
	iter->next = iter->arraylist->contents;
}


/* -------------------------- doubly linked list -------------------------- */

linkedlist_t *linkedlist_new(size_t elem_size, cmp_func_t cmp_func) {
	linkedlist_t *linkedlist = xmalloc(sizeof(linkedlist_t));
	linkedlist->head = NULL;
	linkedlist->tail = NULL;
	linkedlist->value_in_node = elem_size <= NODE_VALUE_MAX_SIZE;
	linkedlist->len = 0;
	linkedlist->cmp_func = cmp_func;
	return linkedlist;
}

void linkedlist_free(linkedlist_t *linkedlist) {
	if (linkedlist->head) {
		linkedlistnode_t *node = linkedlist->head;
		if (linkedlist->value_in_node) {
			while (node) {
				linkedlistnode_t *prev_node = node;
				node = node->next;
				free(prev_node);
			}
		} else {
			while (node) {
				free(node->elem.ptr);
				linkedlistnode_t *prev_node = node;
				node = node->next;
				free(prev_node);
			}
		}
	}
	free(linkedlist);
}

int64_t linkedlist_len(linkedlist_t *linkedlist) {
	return linkedlist->len;
}


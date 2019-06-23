#include "datastructures.h"
#include <stdio.h>
#include <string.h>

/* -------------------------- variable size array -------------------------- */

/* Growth/shrink factor for arraylist */
#define ARRAYLIST_GROWTH_FACTOR 2

/* If the virtual length of the arraylist is <= the physical length of the
   arraylist / ARRAYLIST_SHRINK_THRESHOLD, then physical length of the arraylist
   is shrunk by ARRAYLIST_SHRINK_THRESHOLD. Must be >= ARRAYLIST_GROWTH_FACTOR. */
#define ARRAYLIST_SHRINK_THRESHOLD 3

#if ARRAYLIST_SHRINK_THRESHOLD < ARRAYLIST_GROWTH_FACTOR
#error ARRAYLIST_SHRINK_THRESHOLD must be >= ARRAYLIST_GROWTH_FACTOR
#endif

/* Initial physical length of array */
#define ARRAYLIST_INIT_LEN 5

/* Pointer to item at index `index` in `arraylist`. No bounds checking is done.
   Negative indices are not supported. */
#define ARRAYLIST_GET_UNCHECKED(arraylist, index) \
	((arraylist)->contents + (index) * (int64_t)(arraylist)->elem_size)

arraylist_t *arraylist_new(size_t elem_size, cmp_func_t cmp_func) {
	arraylist_t *new_arraylist = malloc(sizeof(arraylist_t));
	if (!new_arraylist) return NULL;
	if ((new_arraylist->contents = malloc(ARRAYLIST_INIT_LEN * elem_size)) != NULL) {
		new_arraylist->phys_len = ARRAYLIST_INIT_LEN;
	} else if ((new_arraylist->contents = malloc(elem_size)) != NULL) {
		new_arraylist->phys_len = 1;
	} else {
		free(new_arraylist);
		return NULL;
	}
	new_arraylist->len = 0;
	new_arraylist->elem_size = elem_size;
	new_arraylist->end = new_arraylist->contents;
	new_arraylist->cmp_func = cmp_func;
	return new_arraylist;
}

arraylist_t *arraylist_from_array(const void *array, int64_t array_len, size_t elem_size, cmp_func_t cmp_func) {
	if (array_len == 0) return arraylist_new(elem_size, cmp_func);
	arraylist_t *new_arraylist = malloc(sizeof(arraylist_t));
	if (!new_arraylist) return NULL;
	if (!(new_arraylist->contents = malloc((size_t)array_len * elem_size))) {
		free(new_arraylist);
		return NULL;
	}
	new_arraylist->len = array_len;
	new_arraylist->phys_len = array_len;
	new_arraylist->elem_size = elem_size;
	memcpy(new_arraylist->contents, array, (size_t)array_len * elem_size);
	new_arraylist->end = ARRAYLIST_GET_UNCHECKED(new_arraylist, array_len);
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
	int8_t *elem_location = ARRAYLIST_GET_UNCHECKED(arraylist, index);
	memmove(elem_location, value, arraylist->elem_size);
	return elem_location;
}

void *arraylist_get(const arraylist_t *arraylist, int64_t index) {
	if (index < -arraylist->len || index >= arraylist->len) return NULL;
	if (index < 0) index += arraylist->len;
	return ARRAYLIST_GET_UNCHECKED(arraylist, index);
}

void *arraylist_get_copy(const arraylist_t *arraylist, int64_t index, void *dest) {
	void *ptr = arraylist_get(arraylist, index);
	if (ptr) memmove(dest, ptr, arraylist->elem_size);
	return ptr;
}

/** If the virtual length of `arraylist` is not equal to its physical length,
 * return true. If the virtual length of `arraylist` is equal to its physical
 * length, increase the physical length of the arraylist by a factor of
 * ARRAYLIST_GROWTH_FACTOR or by 1, whichever results in a larger array, leaving
 * the virtual length unchanged. If memory allocation fails, try to increase the
 * physical length by 1. On success, return true. On failure, return false. */
static bool arraylist_grow(arraylist_t *arraylist) {
	if (arraylist->phys_len != arraylist->len) return true;
	int64_t phys_len_new = MAX(ARRAYLIST_GROWTH_FACTOR * arraylist->phys_len, arraylist->phys_len + 1);
	int8_t *contents_new = realloc(arraylist->contents, (size_t)phys_len_new * arraylist->elem_size);
	if (contents_new) {
		arraylist->phys_len = phys_len_new;
		arraylist->contents = contents_new;
	} else if ((contents_new = realloc(arraylist->contents, (size_t)(++arraylist->phys_len) * arraylist->elem_size)) != NULL) {
		arraylist->contents = contents_new;
	} else {
		return false;
	}
	arraylist->end = ARRAYLIST_GET_UNCHECKED(arraylist, arraylist->len);
	return true;
}

/** If the virtual length of `arraylist` is greater than or equal to
 * ARRAYLIST_INIT_LEN * ARRAYLIST_GROWTH_FACTOR and is at least a factor of
 * ARRAYLIST_GROWTH_FACTOR smaller than its physical length, try to shrink the
 * physical length by ARRAYLIST_GROWTH_FACTOR. Otherwise, do nothing. */
static void arraylist_shrink(arraylist_t *arraylist) {
	if (arraylist->phys_len / ARRAYLIST_GROWTH_FACTOR >= ARRAYLIST_INIT_LEN &&
		arraylist->len <= arraylist->phys_len / ARRAYLIST_SHRINK_THRESHOLD) {
		int64_t new_phys_len = arraylist->phys_len / ARRAYLIST_GROWTH_FACTOR;
		int8_t *new_contents = realloc(arraylist->contents, (size_t)new_phys_len * arraylist->elem_size);
		if (new_contents) {
			arraylist->phys_len = new_phys_len;
			arraylist->contents = new_contents;
			arraylist->end = ARRAYLIST_GET_UNCHECKED(arraylist, arraylist->len);
		}
	}
}

void *arraylist_append(arraylist_t *arraylist, const void *value) {
	if (!arraylist_grow(arraylist)) return NULL;
	memmove(arraylist->end, value, arraylist->elem_size);
	arraylist->len++;
	void *old_end = arraylist->end;
	arraylist->end += arraylist->elem_size;
	return old_end;
}

// todo handle OOM
void arraylist_extend(arraylist_t *dest, const arraylist_t *source) {
	for (int8_t *value = source->contents; value < source->end; value += source->elem_size) {
		arraylist_append(dest, value);
	}
}

// todo handle OOM
void arraylist_insert(arraylist_t *arraylist, int64_t index, const void *value) {
	if (index < -arraylist->len) index = 0;
	else if (index < 0) index += arraylist->len;
	else if (index > arraylist->len) index = arraylist->len;
	arraylist_grow(arraylist);
	memmove(ARRAYLIST_GET_UNCHECKED(arraylist, index + 1),
		ARRAYLIST_GET_UNCHECKED(arraylist, index),
		(size_t)(arraylist->len - index) * arraylist->elem_size);
	memmove(ARRAYLIST_GET_UNCHECKED(arraylist, index), value, arraylist->elem_size);
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
				(size_t)(arraylist->end - current) - arraylist->elem_size);
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
	memmove(ARRAYLIST_GET_UNCHECKED(arraylist, index),
		ARRAYLIST_GET_UNCHECKED(arraylist, index + 1),
		(size_t)(arraylist->len - index - 1) * arraylist->elem_size);
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
	return arraylist_from_array(ARRAYLIST_GET_UNCHECKED(arraylist, start),
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
		arraylist->end = arraylist->contents;
	} else if ((contents_new = realloc(arraylist->contents, arraylist->elem_size)) != NULL) {
		arraylist->phys_len = 1;
		arraylist->contents = contents_new;
		arraylist->end = arraylist->contents;
	}
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

/** Return the index of the inorder successor of `value` in the slice of
 * `arraylist` starting at index `start` and ending just before index `end`.
 * If all elements in the slice are less than `value`, then return the length of
 * `arraylist`.
 * Precondition: Elements `start` up to but not including `end` are sorted from
 *     least to greatest.
 * @param arraylist: the arraylist
 * @param arraylist: index of first element in sorted slice
 * @param end: index of first element to the right of sorted slice
 * @param value: value for which the inorder successor is sought
 * @return: index of inorder successor of `value` in sorted part of `arraylist`,
 *     length of `arraylist` if one doesn't exist */
static int64_t binary_search_right(const arraylist_t *arraylist, int64_t start, int64_t end, const void *value) {
	while (start < end) {
		int64_t m = start + (end - start) / 2;
		if (arraylist->cmp_func(ARRAYLIST_GET_UNCHECKED(arraylist, m), value) <= 0) {
			start = m + 1;
		} else {
			end = m;
		}
	}
	return start;
}

/** Sort the elements of `arraylist` from least to greatest, starting at index
 * `start` and ending just before index `end`.
 * Precondition: `start` <= `end`
 * @param arraylist: the arraylist to sort
 * @param start: pointer to first element to sort, <= end
 * @param end: pointer just past the last element to sort, >= start
 * @param temp: buffer to hold temporary data during sort, must be large enough
 *     to hold a single element */
static void insertion_sort(const arraylist_t *arraylist, int64_t start, int64_t end, void *temp) {
	for (int64_t current = start + 1; current < end; current++) {
		// current is the index of the value we are pushing down to its sorted position
		int8_t *current_value = ARRAYLIST_GET_UNCHECKED(arraylist, current);
		int64_t insert_index = binary_search_right(arraylist, start, end, current_value);
		if (insert_index != current) {
			memcpy(temp, current_value, arraylist->elem_size);
			memmove(ARRAYLIST_GET_UNCHECKED(arraylist, insert_index + 1),
				ARRAYLIST_GET_UNCHECKED(arraylist, insert_index),
				(size_t)end - (size_t)insert_index * arraylist->elem_size);
			memcpy(ARRAYLIST_GET_UNCHECKED(arraylist, insert_index), temp, arraylist->elem_size);
		}
	}
}

void arraylist_sort(arraylist_t *arraylist) {
	// minrun should be in the range [32,64] such that the number of minruns
	// in the array is slightly less than or equal to a power of 2
	int64_t minrun = arraylist->len;
	int remainder = 0;
	while (minrun >= 64) {
		remainder |= (int)(minrun & 1);
		minrun >>= 1;
	}
	minrun += remainder;
	

}

void arraylist_reverse(arraylist_t *arraylist, void *temp) {
	int8_t *a = arraylist->contents;
	int8_t *b = ARRAYLIST_GET_UNCHECKED(arraylist, arraylist->len - 1);
	while (a < b) {
		memcpy(temp, a, arraylist->elem_size);
		memcpy(a, b, arraylist->elem_size);
		memcpy(b, temp, arraylist->elem_size);
		a += arraylist->elem_size;
		b -= arraylist->elem_size;
	}
}

arraylist_t *arraylist_copy(const arraylist_t *arraylist) {
	arraylist_t *copy = malloc(sizeof(arraylist_t));
	if (!copy) return NULL;
	copy->len = arraylist->len;
	copy->elem_size = arraylist->elem_size;
	if ((copy->contents = malloc((size_t)arraylist->phys_len * arraylist->elem_size)) != NULL) {
		copy->phys_len = arraylist->phys_len;
	} else if ((copy->contents = malloc((size_t)MAX(arraylist->len, 1) * arraylist->elem_size)) != NULL) {
		copy->phys_len = MAX(arraylist->len, 1);
	} else {
		free(copy);
		return NULL;
	}
	memcpy(copy->contents, arraylist->contents, (size_t)arraylist->len * arraylist->elem_size);
	copy->end = ARRAYLIST_GET_UNCHECKED(copy, copy->len);
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
	arraylist_iter_t *iter = malloc(sizeof(arraylist_iter_t));
	if (!iter) return NULL;
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
	linkedlist_t *linkedlist = malloc(sizeof(linkedlist_t));
	if (!linkedlist) return NULL;
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


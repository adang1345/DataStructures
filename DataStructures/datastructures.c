#include "datastructures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------- variable size array ---------- */


arraylist_t *arraylist_new(size_t elem_size, cmp_func_t cmp_func) {
	arraylist_t *new_arraylist = malloc(sizeof(arraylist_t));
	new_arraylist->len = 0;
	new_arraylist->phys_len = ARRAYLIST_INIT_LEN;
	new_arraylist->elem_size = elem_size;
	new_arraylist->contents = malloc(ARRAYLIST_INIT_LEN * elem_size);
	new_arraylist->cmp_func = cmp_func;
	return new_arraylist;
}

arraylist_t *arraylist_from_array(const void *array, int64_t array_len, size_t elem_size, cmp_func_t cmp_func) {
	if (array_len == 0) return arraylist_new(elem_size, cmp_func);
	arraylist_t *new_arraylist = malloc(sizeof(arraylist_t));
	new_arraylist->len = array_len;
	new_arraylist->phys_len = array_len;
	new_arraylist->elem_size = elem_size;
	new_arraylist->contents = malloc(array_len * elem_size);
	memcpy(new_arraylist->contents, array, array_len * elem_size);
	new_arraylist->cmp_func = cmp_func;
	return new_arraylist;
}

void arraylist_free(arraylist_t *arraylist) {
	free(arraylist->contents);
	free(arraylist);
}

int64_t arraylist_len(arraylist_t *arraylist) {
	return arraylist->len;
}

void *arraylist_set(arraylist_t *arraylist, int64_t index, const void *value) {
	if (index < -arraylist->len || index >= arraylist->len) return NULL;
	if (index < 0) index += arraylist->len;
	int8_t *elem_location = arraylist->contents + index * arraylist->elem_size;
	memcpy(elem_location, value, arraylist->elem_size);
	return elem_location;
}

void *arraylist_get(arraylist_t *arraylist, int64_t index) {
	if (index < -arraylist->len || index >= arraylist->len) return NULL;
	if (index < 0) index += arraylist->len;
	return arraylist->contents + index * arraylist->elem_size;
}

void *arraylist_get_copy(arraylist_t *arraylist, int64_t index, void *dest) {
	void *ptr = arraylist_get(arraylist, index);
	if (ptr != NULL) memcpy(dest, ptr, arraylist->elem_size);
	return ptr;
}

/** If the virtual length of `arraylist` is equal to its physical length,
 * increase the physical length of an arraylist by a factor of
 * ARRAYLIST_GROWTH_FACTOR, leaving the virtual length unchanged. Otherwise, do
 * nothing. */
static void arraylist_grow(arraylist_t *arraylist) {
	if (arraylist->phys_len != arraylist->len) return;
	arraylist->phys_len *= ARRAYLIST_GROWTH_FACTOR;
	arraylist->contents = realloc(arraylist->contents, arraylist->phys_len * arraylist->elem_size);
}

/** If the virtual length of `arraylist` is greater than or equal to
 * ARRAYLIST_INIT_LEN * ARRAYLIST_GROWTH_FACTOR and
 * is at least a factor of ARRAYLIST_GROWTH_FACTOR smaller than its physical
 * length, shrink the physical length by ARRAYLIST_GROWTH_FACTOR. Otherwise, do
 * nothing. */
static void arraylist_shrink(arraylist_t *arraylist) {
	if (arraylist->len >= ARRAYLIST_INIT_LEN * ARRAYLIST_GROWTH_FACTOR &&
		arraylist->len <= arraylist->phys_len / ARRAYLIST_GROWTH_FACTOR) {
		arraylist->phys_len /= ARRAYLIST_GROWTH_FACTOR;
		arraylist->contents = realloc(arraylist->contents, arraylist->phys_len * arraylist->elem_size);
	}
}

void arraylist_append(arraylist_t *arraylist, const void *value) {
	arraylist_grow(arraylist);
	arraylist->len++;
	arraylist_set(arraylist, -1, value);
}

void arraylist_extend(arraylist_t *dest, arraylist_t *source) {
	for (int64_t i = 0; i < arraylist_len(source); i++) {
		arraylist_append(dest, arraylist_get(source, i));
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
	arraylist->len++;
	arraylist_set(arraylist, index, value);
}

bool arraylist_contains(arraylist_t *arraylist, const void *value) {
	for (int64_t i = 0; i < arraylist->len; i++) {
		if (!arraylist->cmp_func(arraylist_get(arraylist, i), value)) {
			return true;
		}
	}
	return false;
}

bool arraylist_remove(arraylist_t *arraylist, const void *value) {
	for (int64_t i = 0; i < arraylist->len; i++) {
		if (!arraylist->cmp_func(arraylist_get(arraylist, i), value)) {
			memmove(arraylist->contents + i * arraylist->elem_size,
				arraylist->contents + (i + 1) * arraylist->elem_size,
				(arraylist->len - i - 1) * arraylist->elem_size);
			arraylist->len--;
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
	return true;
}

/** Delete the element at index `index` in `arraylist`, copy it to `dest`, and
 * return true. If `index` is not a valid index, then do not modify `dest` and
 * return false. */
bool arraylist_pop(arraylist_t *arraylist, int64_t index, void *dest) {
	if (!arraylist_get_copy(arraylist, index, dest)) return false;
	return arraylist_delete(arraylist, index);
}

/** Remove all items from `arraylist`. */
void arraylist_clear(arraylist_t *arraylist) {
	arraylist->len = 0;
	arraylist->phys_len = ARRAYLIST_INIT_LEN;
	arraylist->contents = realloc(arraylist->contents, ARRAYLIST_INIT_LEN * arraylist->elem_size);
}

/** Return the index of the first occurence of `value` in `arraylist` using the
 * comparison function, -1 if `value` is not in the arraylist. */
int64_t arraylist_find(arraylist_t *arraylist, const void *value) {
	for (int64_t i = 0; i < arraylist->len; i++) {
		if (arraylist->cmp_func(arraylist_get(arraylist, i), value) == 0) {
			return i;
		}
	}
	return -1;
}



#define assert_arraylists_equal(arraylist1, arraylist2, eq_func) \
	(assert_true(assert_arraylists_equal_helper((arraylist1), (arraylist2), (eq_func)))



#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef DATASTRUCTURES_EXPORTS
#define DATASTRUCTURES_API __declspec(dllexport)
#else
#define DATASTRUCTURES_API __declspec(dllimport)
#endif


/* ---------- variable size array ---------- */

#define ARRAYLIST_GROWTH_FACTOR 2
#define ARRAYLIST_INIT_LEN 5

/** Comparison function type */
typedef int64_t (*cmp_func_t)(const void*, const void*);

typedef struct {
	int64_t len;			// number of elements in array
	int64_t phys_len;		// number of elements raw contents can hold, >0
	size_t elem_size;		// size of each element, in bytes
	int8_t *contents;		// raw contents of array, must be able to hold at least 1 element
	cmp_func_t cmp_func;	// comparison function
} arraylist_t;

/** Create and return a new empty arraylist.
 * @param elem_size: size, in bytes, of each element of array.
 * @param cmp_func: comparison function
 * @return: the array created */
DATASTRUCTURES_API arraylist_t *arraylist_new(size_t elem_size, cmp_func_t cmp_func);

/** Create and return a new arraylist from a given array. If `array_len` is 0,
 * an empty arraylist is returned.
 * @param array: the given array
 * @param array_len: length of the given array, >=0
 * @param elem_size: size, in bytes, of an element of the array */
DATASTRUCTURES_API arraylist_t *arraylist_from_array(const void *array, int64_t array_len, size_t elem_size, cmp_func_t cmp_func);

/** Free the memory associated with an arraylist.
 * @param array: the arraylist to free */
DATASTRUCTURES_API void arraylist_free(arraylist_t *arraylist);

/** Return the number of elements in an arraylist. */
DATASTRUCTURES_API int64_t arraylist_len(arraylist_t *arraylist);

/** Assign an element of an arraylist to a value. Return a pointer to the value.
 * Bytes are copied from `value` into the array.
 * If `index` is greater than or equal to the size, return NULL. If `index` is
 * negative, then read backward from the right end of the arraylist. If `index`
 * is less than the negative of the size of the arraylist, return NULL. */
DATASTRUCTURES_API void *arraylist_set(arraylist_t *arraylist, int64_t index, const void *value);

/** Return a pointer to an element of an arraylist. Return NULL if `index` is
 * out of bounds. Negative indices are supported as in array_set. */
DATASTRUCTURES_API void *arraylist_get(arraylist_t *arraylist, int64_t index);

/** Return a pointer to an element of an arraylist and copy the element into
 * `dest`. If `index` is out of bounds, return NULL and do not modify `dest`.
 * Precondition: `dest` must have enough space to hold an element of the
 *   arraylist. */
DATASTRUCTURES_API void *arraylist_get_copy(arraylist_t *arraylist, int64_t index, void *dest);

/** Append `value` to the end of `arraylist` by copying its contents. */
DATASTRUCTURES_API void arraylist_append(arraylist_t *arraylist, const void *value);

/** Extend `dest` by appending all items from `source`.
 * Precondition: `dest` and `source` are arraylists whose values are the same
 *   type. */
DATASTRUCTURES_API void arraylist_extend(arraylist_t *dest, arraylist_t *source);

/** Insert `value` at position `index` by copying. Negative indices are
 * supported. If the index is out of bounds, the value is inserted at the
 * beginning or the end, whichever is closer to the provided index. */
DATASTRUCTURES_API void arraylist_insert(arraylist_t *arraylist, int64_t index, const void *value);

/** Determine whether `arraylist` contains `value` using the comparison function. */
DATASTRUCTURES_API bool arraylist_contains(arraylist_t *arraylist, const void *value);

/** Remove the first occurence of `value` from `arraylist` using its comparison
* function. Return true if the value was successfully removed, false if the
* value was not in the list. */
DATASTRUCTURES_API bool arraylist_remove(arraylist_t *arraylist, const void *value);

/** Delete the element at index `index` in `arraylist`. If deletion was
 * successful, then return true. If `index` is out of bounds, then return
 * false and do not modify the arraylist. */
DATASTRUCTURES_API bool arraylist_delete(arraylist_t *arraylist, int64_t index);

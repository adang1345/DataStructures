#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef DATASTRUCTURES_EXPORTS
#define DATASTRUCTURES_API __declspec(dllexport)
#else
#define DATASTRUCTURES_API __declspec(dllimport)
#endif


/* -------------------------- variable size array -------------------------- */

/** Comparison function type */
typedef int64_t (*cmp_func_t)(const void*, const void*);

/** Arraylist type */
typedef struct {
	int64_t len;			// number of elements in array
	int64_t phys_len;		// number of elements raw contents can hold, >0
	size_t elem_size;		// size of each element, in bytes
	int8_t *contents;		// raw contents of array, must be able to hold at least 1 element
	int8_t *end;			// pointer just past last element of array, = contents + len * elem_size
	cmp_func_t cmp_func;	// comparison function
} arraylist_t;

/** Arraylist iterator type */
typedef struct {
	const arraylist_t *arraylist;	// arraylist over which we are iterating
	int8_t *next;					// pointer to next value, = arraylist->end if we've reached the end
} arraylist_iter_t;

/** Create and return a new empty arraylist.
 * @param elem_size: size, in bytes, of each element of array.
 * @param cmp_func: comparison function
 * @return: the arraylist created */
DATASTRUCTURES_API arraylist_t *arraylist_new(size_t elem_size, cmp_func_t cmp_func);

/** Create and return a new arraylist from a given array by copying its
 * contents. If `array_len` is 0, an empty arraylist is returned.
 * @param array: the given array
 * @param array_len: length of the given array, >=0
 * @param elem_size: size, in bytes, of an element of the array
 * @return: the arraylist created */
DATASTRUCTURES_API arraylist_t *arraylist_from_array(const void *array, int64_t array_len, size_t elem_size, cmp_func_t cmp_func);

/** Free the memory associated with an arraylist.
 * @param array: the arraylist to free */
DATASTRUCTURES_API void arraylist_free(arraylist_t *arraylist);

/** Return the number of elements in an arraylist.
 * @param arraylist: the arraylist */
DATASTRUCTURES_API int64_t arraylist_len(const arraylist_t *arraylist);

/** Assign an element of an arraylist to a value. Return a pointer to the value.
 * Bytes are copied from `value` into the array.
 * If `index` is greater than or equal to the size, return NULL. If `index` is
 * negative, then read backward from the right end of the arraylist. If `index`
 * is less than the negative of the size of the arraylist, return NULL.
 * @param arraylist: the arraylist
 * @param index: the index in the arraylist
 * @param value: the value to assign
 * @return: pointer to the value assigned in the arraylist */
DATASTRUCTURES_API void *arraylist_set(arraylist_t *arraylist, int64_t index, const void *value);

/** Return a pointer to an element of an arraylist. Return NULL if `index` is
 * out of bounds. Negative indices are supported as in array_set.
 * @param arraylist: the arraylist
 * @param index: index of the element to get
 * @return: pointer to the requested element */
DATASTRUCTURES_API void *arraylist_get(const arraylist_t *arraylist, int64_t index);

/** Return a pointer to an element of an arraylist and copy the element into
 * `dest`. If `index` is out of bounds, return NULL and do not modify `dest`.
 * @param arraylist: the arraylist
 * @param index: index of the element to get
 * @param dest: location to copy the element, must have enough space to hold 1
 *   element
 * @return: pointer to the requested element */
DATASTRUCTURES_API void *arraylist_get_copy(const arraylist_t *arraylist, int64_t index, void *dest);

/** Append a value to the end of an arraylist by copying its contents. 
 * @param arraylist: the arraylist
 * @param value: the value to append */
DATASTRUCTURES_API void arraylist_append(arraylist_t *arraylist, const void *value);

/** Extend `dest` by appending all items in `source` to the end in order.
 * `source` is unchanged.
 * @param dest: the arraylist that receives values, must have values that are
 *   the same type as those in `source`
 * @param source: the arraylist that gives values */
DATASTRUCTURES_API void arraylist_extend(arraylist_t *dest, const arraylist_t *source);

/** Insert `value` into the arraylist at position `index` by copying. Negative
 * indices are supported. If the index is out of bounds, the value is inserted
 * at the beginning or the end, whichever is closer to `index`.
 * @param arraylist: the arraylist
 * @param index: index to insert the value
 * @param value: value to insert */
DATASTRUCTURES_API void arraylist_insert(arraylist_t *arraylist, int64_t index, const void *value);

/** Determine whether `arraylist` contains `value` using the comparison function.
 * @param arraylist: the arraylist
 * @param value: value to search for */
DATASTRUCTURES_API bool arraylist_contains(const arraylist_t *arraylist, const void *value);

/** Remove the first occurence of `value` from `arraylist` using its comparison
* function. Return true if the value was successfully removed, false if the
* value was not in the list.
* @param arraylist: the arraylist
* @param value: value to remove
* @return: whether removal was successful */
DATASTRUCTURES_API bool arraylist_remove(arraylist_t *arraylist, const void *value);

/** Delete the element at index `index` in `arraylist`. If deletion was
 * successful, then return true. If `index` is out of bounds, then return
 * false and do not modify the arraylist.
 * @param arraylist: the arraylist
 * @param index: index of element to delete
 * @return: whether deletion was successful */
DATASTRUCTURES_API bool arraylist_delete(arraylist_t *arraylist, int64_t index);

/** Return a new dynamically allocated arraylist containing the elements in
 * `arraylist` starting at index `start` and ending just before index `end`.
 * Negative indices may be used. If `start` or `end` is out of bounds, normalize
 * it to the closest of 0 and the length of `arraylist`. After normalization, if
 * `end` comes before `start` or `start` is not a valid index, return an empty
 * arraylist.
 * @param arraylist: the arraylist
 * @param start: start index
 * @param end: end index
 * @return: the sub-arraylist resulting from slicing */
DATASTRUCTURES_API arraylist_t *arraylist_slice(arraylist_t *arraylist, int64_t start, int64_t end);

/** Delete the element at index `index` in `arraylist`, copy it to `dest`, and
 * return true. If `index` is not a valid index, then do not modify `dest` and
 * return false.
 * @param arraylist: the arraylist
 * @param index: index of element to delete
 * @param dest: location to copy the deleted element, must be large enough to
 *   hold an element
 * @return: whether deletion was successful */
DATASTRUCTURES_API bool arraylist_pop(arraylist_t *arraylist, int64_t index, void *dest);

/** Remove all items from `arraylist`.
 * @param arraylist: the arraylist */
DATASTRUCTURES_API void arraylist_clear(arraylist_t *arraylist);

/** Return the non-negative index of the first occurence of `value` in
 * `arraylist` using the comparison function, -1 if `value` is not in the
 * arraylist.
 * @param arraylist: the arraylist
 * @param value: value to search for
 * @return: index of first occurrence of `value`, -1 if not in `arraylist` */
DATASTRUCTURES_API int64_t arraylist_find(const arraylist_t *arraylist, const void *value);

/** Return the non-negative index of the last occurence of `value` in
 * `arraylist` using the comparison function, -1 if `value` is not in the
 * arraylist.
 * @param arraylist: the arraylist
 * @param value: value to search for 
 * @return: index of first occurrence of `value`, -1 if not in `arraylist` */
DATASTRUCTURES_API int64_t arraylist_rfind(const arraylist_t *arraylist, const void *value);

/** Return the number of times `value` appears in `arraylist`, using the
 * comparison function.
 * @param arraylist: the arraylist
 * @param value: value to search for
 * @return: number of times `value` appears */
DATASTRUCTURES_API int64_t arraylist_count(const arraylist_t *arraylist, const void *value);

/** Sort `arraylist` using its comparison function.
 * @param arraylist: the arraylist */
DATASTRUCTURES_API void arraylist_sort(arraylist_t *arraylist);

/** Reverse the elements of `arraylist`.
 * @param arraylist: the arraylist */
DATASTRUCTURES_API void arraylist_reverse(arraylist_t *arraylist);

/** Return a shallow copy of `arraylist`.
 * @param arraylist: the arraylist
 * @return: a copy of `arraylist` */
DATASTRUCTURES_API arraylist_t *arraylist_copy(const arraylist_t *arraylist);

/** Return a negative number if `arraylist1` is less than `arraylist2`, 0 if
 * `arraylist1` is equal to `arraylist2`, a positive number if `arraylist1` is
 * greater than `arraylist2`. Comparison is done one element at a time using the
 * comparison function. If all first
 * min(length of `arraylist1`, length of `arraylist2`) are equal, then the
 * longer arraylist is greater than the shorter arraylist.
 * @param arraylist1: the first arraylist
 * @param arraylist2: the second arraylist, must have elements of the same type
 *   as `arraylist1`
 * @return: comparison of `arraylist1` and `arraylist2` */
DATASTRUCTURES_API int64_t arraylist_compare(const arraylist_t *arraylist1, const arraylist_t *arraylist2);

/** Call `func` for each value in `arraylist` in order by passing a pointer
 * to the value to `func`.
 * @param arraylist: the arraylist
 * @param func: function to call */
DATASTRUCTURES_API void arraylist_foreach(arraylist_t *arraylist, void(*func)(void*));

/** Create and return a new arraylist iterator.
 * @param arraylist: the arraylist
 * @return: an arraylist iterator */
DATASTRUCTURES_API arraylist_iter_t *arraylist_iter_new(const arraylist_t *arraylist);

/** Free an arraylist iterator. Does not affect the underlying arraylist.
 * @param iter: the arraylist iterator */
DATASTRUCTURES_API void arraylist_iter_free(arraylist_iter_t *iter);

/** Return a pointer to the next value in an arraylist iterator. If there is no
 * next value, return NULL.
 * @param iter: the arraylist iterator */
DATASTRUCTURES_API void *arraylist_iter_next(arraylist_iter_t *iter);

/** Reset an arraylist iterator back to the beginning.
 * @param iter: the arraylist iterator */
DATASTRUCTURES_API void arraylist_iter_reset(arraylist_iter_t *iter);


/* -------------------------- doubly linked list -------------------------- */

#define NODE_VALUE_MAX_SIZE sizeof(void *)

typedef struct linkedlistnode_t {
	union {
		void *ptr;							// pointer to malloced buffer containing value, used when value requires >sizeof(int8_t *) bytes
		int8_t value[NODE_VALUE_MAX_SIZE];	// byte array whose first few elements contain the value, used when value requires <=sizeof(int8_t *) bytes
	} elem;									// element in this node
	size_t elem_size;						// size of element
	struct linkedlistnode_t *prev;			// previous node, NULL if we are the head
	struct linkedlistnode_t *next;			// next node, NULL if we are the tail
} linkedlistnode_t;

typedef struct {
	linkedlistnode_t *head;		// head node, NULL if empty
	linkedlistnode_t *tail;		// tail node, NULL if empty
	bool value_in_node;			// true if value is stored in node directly, false if value is stored in malloced buffer whose pointer is stored in node
	int64_t len;				// number of elements
	cmp_func_t cmp_func;		// comparison function
} linkedlist_t;

/** Create and return a new, empty linkedlist.
 * @param elem_size: size, in bytes, of each element of the linkedlist
 * @param cmp_func: the comparison function
 * @return: the linkedlist created */
DATASTRUCTURES_API linkedlist_t *linkedlist_new(size_t elem_size, cmp_func_t cmp_func);

/** Free the memory associated with a linkedlist.
 * @param linkedlist: the linkedlist */
DATASTRUCTURES_API void linkedlist_free(linkedlist_t *linkedlist);

/** Return the length of a linkedlist.
 * @param linkedlist: the linkedlist
 * @return: length of the linkedlist */
DATASTRUCTURES_API int64_t linkedlist_len(linkedlist_t *linkedlist);

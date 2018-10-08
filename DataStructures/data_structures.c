#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>


/* general purpose assertions for testing */
#define assert_equal(a, b) if ((a) != (b)) {printf("assert_equal fail: line %i\n", __LINE__); exit(EXIT_FAILURE);}
#define assert_true(a) if (!a) {printf("assert_equal fail: line %i\n", __LINE__); exit(EXIT_FAILURE);}
#define assert_not_equal(a, b) if ((a) == (b)) {printf("assert_not_equal fail: line %i\n", __LINE__); exit(EXIT_FAILURE);}
#define assert_equal_sized(a, b, size) \
	if (strncmp((char*)(a), (char*)(b), (size))) {printf("assert_equal_sized fail: line %i\n", __LINE__); exit(EXIT_FAILURE);}
#define array_len(arr) (sizeof(arr) / sizeof(arr[0]))


/* ---------- fixed-size array ---------- */
typedef struct {
	int64_t size;  // number of elements in array
	size_t elem_size;   // size of each element, in bytes
	int8_t *contents;           // raw contents of array
} array_t;

/** Create and return a new array, where all elements are initialized to zero.
 * @param size: number of elements in array, must be non-negative
 * @param elem_size: size, in bytes, of each element of array.
 * @return: the array created */
array_t *array_new(int64_t size, size_t elem_size) {
	array_t *new_array = malloc(sizeof(array_t));
	new_array->size = size;
	new_array->elem_size = elem_size;
	new_array->contents = calloc(size, elem_size);
	return new_array;
}

/** Free the memory associated with an array.
 * @param array: the array to free */
void array_free(array_t *array) {
	free(array->contents);
	free(array);
}

/** Assign an element of an array to a value. Return a pointer to the value.
 * Bytes are copied from `value` into the array.
 * If `index` is greater than or equal to the size, return NULL. If `index` is
 * negative, then read backward from the right end of the array. If `index` is
 * less than the negative of the size of the array, return NULL. */
void *array_set(array_t *array, int64_t index, const void *value) {
	if (index < -array->size || index >= array->size) return NULL;
	if (index < 0) index += array->size;
	int8_t *elem_location = array->contents + index * array->elem_size;
	memcpy(elem_location, value, array->elem_size);
	return elem_location;
}

/** Return a pointer to an element of an array. Return NULL if `index` is out of
 * bounds. Negative indices are supported as in array_set. */
void *array_get(array_t *array, int64_t index) {
	if (index < -array->size || index >= array->size) return NULL;
	if (index < 0) index += array->size;
	return array->contents + index * array->elem_size;
}

/** Return a pointer to an element of an array and copy the element into `dest`.
 * If `index` is out of bounds, return NULL and do not modify `dest`.
 * Precondition: `dest` must have enough space to hold an element of the array. */
void *array_get_copy(array_t *array, int64_t index, void *dest) {
	void *ptr = array_get(array, index);
	if (ptr != NULL) memcpy(dest, ptr, array->elem_size);
	return ptr;
}

/** struct for testing fixed-size array */
struct array_test_struct {
	int a:16;
	double b;
	char *c;
	int d[3];
};

void test_array(void) {
	int int_value = 1;
	array_t *int_array0 = array_new(0, sizeof(int));
	assert_equal(0, int_array0->size);
	assert_equal(sizeof(int), int_array0->elem_size);
	assert_equal(NULL, array_set(int_array0, 0, &int_value));
	assert_equal(NULL, array_set(int_array0, 1, &int_value));
	assert_equal(NULL, array_set(int_array0, -1, &int_value));
	assert_equal(NULL, array_get(int_array0, 0));
	assert_equal(NULL, array_get(int_array0, 1));
	assert_equal(NULL, array_get(int_array0, -1));
	array_free(int_array0);

	double double_values[] = { 1., 2. };
	array_t *double_array1 = array_new(1, sizeof(double));
	assert_equal(1, double_array1->size);
	assert_equal(0., *(double*)array_get(double_array1, 0));
	assert_equal(0., *(double*)array_get(double_array1, -1));
	assert_equal(NULL, array_get(double_array1, 1));
	assert_equal(NULL, array_get(double_array1, -2));
	assert_equal(1., *(double*)array_set(double_array1, 0, &double_values[0]));
	assert_equal(2., *(double*)array_set(double_array1, -1, &double_values[1]));
	assert_equal(2., *(double*)array_get(double_array1, 0));
	assert_equal(2., *(double*)array_get(double_array1, -1));
	double get_value;
	assert_not_equal(NULL, array_get_copy(double_array1, 0, &get_value));
	assert_equal(2., get_value);
	assert_equal(NULL, array_get_copy(double_array1, 1, &get_value));
	assert_equal(2., get_value);
	array_free(double_array1);

	struct array_test_struct array_test_struct_values[3];
	memset(array_test_struct_values, 0, sizeof array_test_struct_values);
	array_test_struct_values[0].a = 5;
	array_test_struct_values[0].b = 2.;
	array_test_struct_values[0].c = NULL;
	array_test_struct_values[0].d[0] = 5;
	array_test_struct_values[0].d[1] = 6;
	array_test_struct_values[0].d[2] = 7;
	array_test_struct_values[1].a = 1;
	array_test_struct_values[1].b = 156.23;
	array_test_struct_values[1].c = NULL;
	array_test_struct_values[1].d[0] = 7;
	array_test_struct_values[1].d[1] = 8;
	array_test_struct_values[1].d[2] = 9;
	array_test_struct_values[2].a = 7;
	array_test_struct_values[2].b = 0.5;
	array_test_struct_values[2].c = NULL;
	array_test_struct_values[2].d[0] = -1;
	array_test_struct_values[2].d[1] = 0;
	array_test_struct_values[2].d[2] = 1;
	array_t *struct_array3 = array_new(3, sizeof(struct array_test_struct));
	assert_equal_sized(&array_test_struct_values[0], array_set(struct_array3, 0, &array_test_struct_values[0]), sizeof(struct array_test_struct));
	array_set(struct_array3, 1, &array_test_struct_values[1]);
	array_set(struct_array3, -1, &array_test_struct_values[2]);
	assert_equal_sized(&array_test_struct_values[1], array_get(struct_array3, 1), sizeof(struct array_test_struct));
	assert_equal_sized(&array_test_struct_values[2], array_get(struct_array3, 2), sizeof(struct array_test_struct));
	array_free(struct_array3);

	printf("test_array passed\n");
}



/* ---------- variable size array ---------- */

#define ARRAYLIST_GROWTH_FACTOR 2
#define ARRAYLIST_INIT_SIZE 5

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
 * @param 
 * @return: the array created */
arraylist_t *arraylist_new(size_t elem_size, cmp_func_t cmp_func) {
	arraylist_t *new_arraylist = malloc(sizeof(arraylist_t));
	new_arraylist->len = 0;
	new_arraylist->phys_len = ARRAYLIST_INIT_SIZE;
	new_arraylist->elem_size = elem_size;
	new_arraylist->contents = malloc(ARRAYLIST_INIT_SIZE * elem_size);
	new_arraylist->cmp_func = cmp_func;
	return new_arraylist;
}

/** Create and return a new arraylist from a given array. If `array_len` is 0,
 * an empty arraylist is returned.
 * @param array: the given array
 * @param array_len: length of the given array, >=0
 * @param elem_size: size, in bytes, of an element of the array */
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

/** Free the memory associated with an arraylist.
 * @param array: the arraylist to free */
void arraylist_free(arraylist_t *arraylist) {
	free(arraylist->contents);
	free(arraylist);
}

/** Return the number of elements in an arraylist. */
int64_t arraylist_len(arraylist_t *arraylist) {
	return arraylist->len;
}

/** Assign an element of an arraylist to a value. Return a pointer to the value.
 * Bytes are copied from `value` into the array.
 * If `index` is greater than or equal to the size, return NULL. If `index` is
 * negative, then read backward from the right end of the arraylist. If `index`
 * is less than the negative of the size of the arraylist, return NULL. */
void *arraylist_set(arraylist_t *arraylist, int64_t index, const void *value) {
	if (index < -arraylist->len || index >= arraylist->len) return NULL;
	if (index < 0) index += arraylist->len;
	int8_t *elem_location = arraylist->contents + index * arraylist->elem_size;
	memcpy(elem_location, value, arraylist->elem_size);
	return elem_location;
}

/** Return a pointer to an element of an arraylist. Return NULL if `index` is
 * out of bounds. Negative indices are supported as in array_set. */
void *arraylist_get(arraylist_t *arraylist, int64_t index) {
	if (index < -arraylist->len || index >= arraylist->len) return NULL;
	if (index < 0) index += arraylist->len;
	return arraylist->contents + index * arraylist->elem_size;
}

/** Return a pointer to an element of an arraylist and copy the element into
 * `dest`. If `index` is out of bounds, return NULL and do not modify `dest`.
 * Precondition: `dest` must have enough space to hold an element of the
 *   arraylist. */
void *arraylist_get_copy(arraylist_t *arraylist, int64_t index, void *dest) {
	void *ptr = arraylist_get(arraylist, index);
	if (ptr != NULL) memcpy(dest, ptr, arraylist->elem_size);
	return ptr;
}

/** If the virtual size of `arraylist` is equal to its physical size, increase
 * the physical size of an arraylist by a factor of ARRAYLIST_GROWTH_FACTOR,
 * leaving the virtual size unchanged. Otherwise, do nothing.*/
static void arraylist_grow(arraylist_t *arraylist) {
	if (arraylist->phys_len != arraylist->len) return;
	arraylist->phys_len *= ARRAYLIST_GROWTH_FACTOR;
	arraylist->contents = realloc(arraylist->contents, arraylist->phys_len * arraylist->elem_size);
}

/** Append `value` to the end of `arraylist` by copying its contents. */
void arraylist_append(arraylist_t *arraylist, const void *value) {
	arraylist_grow(arraylist);
	arraylist->len++;
	arraylist_set(arraylist, -1, value);
}

/** Extend `dest` by appending all items from `source`.
 * Precondition: `dest` and `source` are arraylists whose values are the same
 *   type. */
void arraylist_extend(arraylist_t *dest, arraylist_t *source) {
	for (int64_t i = 0; i < arraylist_len(source); i++) {
		arraylist_append(dest, arraylist_get(source, i));
	}
}

/** Insert `value` at position `index` by copying. Negative indices are
 * supported. If the index is out of bounds, the value is inserted at the
 * beginning or the end, whichever is closer to the provided index. */
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

/** Determine whether `arraylist` contains `value` using the comparison function. */
bool arraylist_contains(arraylist_t *arraylist, const void *value) {
	for (int64_t i = 0; i < arraylist->len; i++) {
		if (!arraylist->cmp_func(arraylist_get(arraylist, i), value)) {
			return true;
		}
	}
	return false;
}

/** Remove the first occurence of `value` from `arraylist` using its comparison
* function. Return true if the value was successfully removed, false if the
* value was not in the list. */
bool arraylist_remove(arraylist_t *arraylist, const void *value) {
	for (int64_t i = 0; i < arraylist->len; i++) {
		if (!arraylist->cmp_func(arraylist_get(arraylist, i), value)) {
			memmove(arraylist->contents + i * arraylist->elem_size,
				arraylist->contents + (i + 1) * arraylist->elem_size,
				(arraylist->len - i - 1) * arraylist->elem_size);
			arraylist->len--;
			return true;
		}
	}
	return false;
}

/** Delete the element at index `index` in `arraylist`. If deletion was
 * successful, then return true. If `index` is out of bounds, then return
 * false. */
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
	free(arraylist->contents);
	arraylist->len = 0;
	arraylist->phys_len = ARRAYLIST_INIT_SIZE;
	arraylist->contents = malloc(ARRAYLIST_INIT_SIZE * arraylist->elem_size);
}



/** Determine if `arraylist1` and `arraylist2` are equal, given a function `equal`
 * that determines whether two corresponding elements are equal. */
static bool assert_arraylists_equal_helper(arraylist_t *arraylist1, arraylist_t *arraylist2, int (*eq_func)(void*, void*)) {
	if (arraylist_len(arraylist1) != arraylist_len(arraylist2)) return false;
	for (int64_t i = 0; i < arraylist_len(arraylist1); i++) {
		if (!(*eq_func)(arraylist_get(arraylist1, i), arraylist_get(arraylist2, i))) {
			return false;
		}
	}
	return true;
}

#define assert_arraylists_equal(arraylist1, arraylist2, eq_func) \
	(assert_true(assert_arraylists_equal_helper((arraylist1), (arraylist2), (eq_func)))

static int64_t int_compare(const int *a, const int *b) {
	return *a - *b;
}

static int64_t double_compare(const double *a, const double *b) {
	if (*a < *b) return -1;
	else if (*a == *b) return 0;
	else return 1;
}

void test_arraylist(void) {
	// empty arraylist
	int int_value = 1;
	arraylist_t *int_arraylist0 = arraylist_new(sizeof(int), int_compare);
	assert_equal(0, arraylist_len(int_arraylist0));
	assert_equal(sizeof(int), int_arraylist0->elem_size);
	assert_equal(NULL, arraylist_set(int_arraylist0, 0, &int_value));
	assert_equal(NULL, arraylist_set(int_arraylist0, 1, &int_value));
	assert_equal(NULL, arraylist_set(int_arraylist0, -1, &int_value));
	assert_equal(NULL, arraylist_get(int_arraylist0, 0));
	assert_equal(NULL, arraylist_get(int_arraylist0, 1));
	assert_equal(NULL, arraylist_get(int_arraylist0, -1));
	arraylist_free(int_arraylist0);

	// arraylist_get, arraylist_set, arraylist_get_copy, arraylist_append
	double double_values[] = { 0., 1., 2., 3., 4., 5., 6.};
	arraylist_t *double_arraylist = arraylist_new(sizeof(double), double_compare);
	arraylist_append(double_arraylist, &double_values[0]);
	assert_equal(1, arraylist_len(double_arraylist));
	assert_equal(0., *(double*)arraylist_get(double_arraylist, 0));
	assert_equal(0., *(double*)arraylist_get(double_arraylist, -1));
	arraylist_append(double_arraylist, &double_values[1]);
	arraylist_append(double_arraylist, &double_values[2]);
	arraylist_append(double_arraylist, &double_values[3]);
	arraylist_append(double_arraylist, &double_values[4]);
	arraylist_append(double_arraylist, &double_values[5]);
	arraylist_append(double_arraylist, &double_values[6]);
	assert_equal(7, arraylist_len(double_arraylist));
	assert_equal(4., *(double*)arraylist_get(double_arraylist, 4));
	assert_equal(5., *(double*)arraylist_get(double_arraylist, 5));
	assert_equal(6., *(double*)arraylist_get(double_arraylist, 6));
	assert_equal(0., *(double*)arraylist_get(double_arraylist, -7));
	assert_equal(NULL, arraylist_get(double_arraylist, 7));
	assert_equal(NULL, arraylist_get(double_arraylist, -8));
	double double_value;
	arraylist_get_copy(double_arraylist, -2, &double_value);
	assert_equal(5., double_value);
	
	// arraylist_extend
	arraylist_t *double_arraylist2 = arraylist_new(sizeof(double), double_compare);
	arraylist_extend(double_arraylist, double_arraylist2);
	assert_equal(6., *(double*)arraylist_get(double_arraylist, -1));
	assert_equal(7, arraylist_len(double_arraylist));
	arraylist_free(double_arraylist2);
	double_arraylist2 = arraylist_from_array(double_values, 1, sizeof(double), double_compare);
	arraylist_extend(double_arraylist, double_arraylist2);
	assert_equal(0., *(double*)arraylist_get(double_arraylist, -1));
	assert_equal(8, arraylist_len(double_arraylist));
	arraylist_free(double_arraylist2);
	double_arraylist2 = arraylist_from_array(double_values, 2, sizeof(double), double_compare);
	arraylist_extend(double_arraylist, double_arraylist2);
	assert_equal(1., *(double*)arraylist_get(double_arraylist, -1));
	assert_equal(10, arraylist_len(double_arraylist));
	arraylist_free(double_arraylist2);
	arraylist_free(double_arraylist);

	// arraylist_insert
	int int_values[] = { 0, 1, 2, 3, 4 };
	for (int i = -2; i <= 2; i++) {
		int_arraylist0 = arraylist_new(sizeof(int), int_compare);
		arraylist_insert(int_arraylist0, i, &int_values[0]);
		assert_equal(0, *(int*)arraylist_get(int_arraylist0, 0));
		assert_equal(1, arraylist_len(int_arraylist0));
		arraylist_free(int_arraylist0);
	}
	for (int i = -2; i <= 0; i++) {
		arraylist_t *int_arraylist1 = arraylist_new(sizeof(int), int_compare);
		arraylist_insert(int_arraylist1, 5, &int_values[0]);
		arraylist_insert(int_arraylist1, i, &int_values[1]);
		assert_equal(1, *(int*)arraylist_get(int_arraylist1, 0));
		assert_equal(0, *(int*)arraylist_get(int_arraylist1, 1));
		arraylist_free(int_arraylist1);
	}
	for (int i = 1; i <= 2; i++) {
		arraylist_t *int_arraylist1 = arraylist_new(sizeof(int), int_compare);
		arraylist_insert(int_arraylist1, 5, &int_values[0]);
		arraylist_insert(int_arraylist1, i, &int_values[1]);
		assert_equal(0, *(int*)arraylist_get(int_arraylist1, 0));
		assert_equal(1, *(int*)arraylist_get(int_arraylist1, 1));
		arraylist_free(int_arraylist1);
	}
	arraylist_t *int_arraylist5 = arraylist_from_array(int_values, array_len(int_values), sizeof(int), int_compare);
	arraylist_insert(int_arraylist5, -2, &int_values[0]);
	assert_equal(0, *(int*)arraylist_get(int_arraylist5, 0));
	assert_equal(1, *(int*)arraylist_get(int_arraylist5, 1));
	assert_equal(2, *(int*)arraylist_get(int_arraylist5, 2));
	assert_equal(0, *(int*)arraylist_get(int_arraylist5, 3));
	assert_equal(3, *(int*)arraylist_get(int_arraylist5, 4));
	assert_equal(4, *(int*)arraylist_get(int_arraylist5, 5));
	arraylist_free(int_arraylist5);
	int_arraylist5 = arraylist_from_array(int_values, array_len(int_values), sizeof(int), int_compare);
	arraylist_insert(int_arraylist5, 0, &int_values[0]);
	assert_equal(0, *(int*)arraylist_get(int_arraylist5, 0));
	assert_equal(0, *(int*)arraylist_get(int_arraylist5, 1));
	assert_equal(1, *(int*)arraylist_get(int_arraylist5, 2));
	assert_equal(2, *(int*)arraylist_get(int_arraylist5, 3));
	assert_equal(3, *(int*)arraylist_get(int_arraylist5, 4));
	assert_equal(4, *(int*)arraylist_get(int_arraylist5, 5));
	arraylist_free(int_arraylist5);
	int_arraylist5 = arraylist_from_array(int_values, array_len(int_values), sizeof(int), int_compare);
	arraylist_insert(int_arraylist5, 1, &int_values[0]);
	assert_equal(0, *(int*)arraylist_get(int_arraylist5, 0));
	assert_equal(0, *(int*)arraylist_get(int_arraylist5, 1));
	assert_equal(1, *(int*)arraylist_get(int_arraylist5, 2));
	assert_equal(2, *(int*)arraylist_get(int_arraylist5, 3));
	assert_equal(3, *(int*)arraylist_get(int_arraylist5, 4));
	assert_equal(4, *(int*)arraylist_get(int_arraylist5, 5));
	arraylist_free(int_arraylist5);
	int_arraylist5 = arraylist_from_array(int_values, array_len(int_values), sizeof(int), int_compare);
	arraylist_insert(int_arraylist5, 5, &int_values[0]);
	assert_equal(0, *(int*)arraylist_get(int_arraylist5, 0));
	assert_equal(1, *(int*)arraylist_get(int_arraylist5, 1));
	assert_equal(2, *(int*)arraylist_get(int_arraylist5, 2));
	assert_equal(3, *(int*)arraylist_get(int_arraylist5, 3));
	assert_equal(4, *(int*)arraylist_get(int_arraylist5, 4));
	assert_equal(0, *(int*)arraylist_get(int_arraylist5, 5));
	arraylist_free(int_arraylist5);
	int_arraylist5 = arraylist_from_array(int_values, array_len(int_values), sizeof(int), int_compare);
	arraylist_insert(int_arraylist5, 6, &int_values[0]);
	assert_equal(0, *(int*)arraylist_get(int_arraylist5, 0));
	assert_equal(1, *(int*)arraylist_get(int_arraylist5, 1));
	assert_equal(2, *(int*)arraylist_get(int_arraylist5, 2));
	assert_equal(3, *(int*)arraylist_get(int_arraylist5, 3));
	assert_equal(4, *(int*)arraylist_get(int_arraylist5, 4));
	assert_equal(0, *(int*)arraylist_get(int_arraylist5, 5));
	arraylist_free(int_arraylist5);
	printf("test_arraylist passed\n");
}



int main(void) {
	test_array();
	test_arraylist();
	return EXIT_SUCCESS;
}
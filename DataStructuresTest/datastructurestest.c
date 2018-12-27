#include "datastructures.h"
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#define assert_equal(a, b) \
	do { \
		if ((a) != (b)) { \
			printf("assert_equal fail: line %i\n", __LINE__); \
			exit(EXIT_FAILURE); \
		} \
	} while (0)
#define assert_true(a) \
	do { \
		if (!(a)) { \
			printf("assert_true fail: line %i\n", __LINE__); \
			exit(EXIT_FAILURE); \
		} \
	} while (0)
#define assert_false(a) \
	do { \
		if (a) { \
			printf("assert_false fail: line %i\n", __LINE__); \
			exit(EXIT_FAILURE); \
		} \
	} while (0)
#define assert_not_equal(a, b) \
	do { \
		if ((a) == (b)) { \
			printf("assert_not_equal fail: line %i\n", __LINE__); \
			exit(EXIT_FAILURE); \
		} \
	} while (0)

#define run_test(test) \
	__try { \
		(test)(); \
		printf(#test " passed\n"); \
	} \
	__except (EXCEPTION_EXECUTE_HANDLER) { \
		printf("Exception Code %d occured during " #test "\n", GetExceptionCode()); \
		exit(EXIT_FAILURE); \
	}

static int64_t int_compare(const int *a, const int *b) {
	return *a - *b;
}

static int64_t double_compare(const double *a, const double *b) {
	if (*a < *b) return -1;
	else if (*a == *b) return 0;
	else return 1;
}

/** Increment `value` by 1. Used to test arraylist_foreach */
void increment(int *value) {
	(*value)++;
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

	// arraylist_get, arraylist_set, arraylist_get_copy, arraylist_append
	double double_values[] = { 0., 1., 2., 3., 4., 5., 6. };
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
	assert_equal(0., *(double*)arraylist_get(double_arraylist2, 0));
	assert_equal(0., *(double*)arraylist_get(double_arraylist2, -1));
	arraylist_extend(double_arraylist, double_arraylist2);
	assert_equal(8, arraylist_len(double_arraylist));
	assert_equal(0., *(double*)arraylist_get(double_arraylist, -1));
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
	arraylist_t *int_arraylist1;
	for (int i = -2; i <= 0; i++) {
		int_arraylist1 = arraylist_new(sizeof(int), int_compare);
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
	arraylist_t *int_arraylist5 = arraylist_from_array(int_values, _countof(int_values), sizeof(int), int_compare);
	arraylist_insert(int_arraylist5, -2, &int_values[0]);
	assert_equal(0, *(int*)arraylist_get(int_arraylist5, 0));
	assert_equal(1, *(int*)arraylist_get(int_arraylist5, 1));
	assert_equal(2, *(int*)arraylist_get(int_arraylist5, 2));
	assert_equal(0, *(int*)arraylist_get(int_arraylist5, 3));
	assert_equal(3, *(int*)arraylist_get(int_arraylist5, 4));
	assert_equal(4, *(int*)arraylist_get(int_arraylist5, 5));
	arraylist_free(int_arraylist5);
	int_arraylist5 = arraylist_from_array(int_values, _countof(int_values), sizeof(int), int_compare);
	arraylist_insert(int_arraylist5, 0, &int_values[0]);
	assert_equal(0, *(int*)arraylist_get(int_arraylist5, 0));
	assert_equal(0, *(int*)arraylist_get(int_arraylist5, 1));
	assert_equal(1, *(int*)arraylist_get(int_arraylist5, 2));
	assert_equal(2, *(int*)arraylist_get(int_arraylist5, 3));
	assert_equal(3, *(int*)arraylist_get(int_arraylist5, 4));
	assert_equal(4, *(int*)arraylist_get(int_arraylist5, 5));
	arraylist_free(int_arraylist5);
	int_arraylist5 = arraylist_from_array(int_values, _countof(int_values), sizeof(int), int_compare);
	arraylist_insert(int_arraylist5, 1, &int_values[0]);
	assert_equal(0, *(int*)arraylist_get(int_arraylist5, 0));
	assert_equal(0, *(int*)arraylist_get(int_arraylist5, 1));
	assert_equal(1, *(int*)arraylist_get(int_arraylist5, 2));
	assert_equal(2, *(int*)arraylist_get(int_arraylist5, 3));
	assert_equal(3, *(int*)arraylist_get(int_arraylist5, 4));
	assert_equal(4, *(int*)arraylist_get(int_arraylist5, 5));
	arraylist_free(int_arraylist5);
	int_arraylist5 = arraylist_from_array(int_values, _countof(int_values), sizeof(int), int_compare);
	arraylist_insert(int_arraylist5, 5, &int_values[0]);
	assert_equal(0, *(int*)arraylist_get(int_arraylist5, 0));
	assert_equal(1, *(int*)arraylist_get(int_arraylist5, 1));
	assert_equal(2, *(int*)arraylist_get(int_arraylist5, 2));
	assert_equal(3, *(int*)arraylist_get(int_arraylist5, 3));
	assert_equal(4, *(int*)arraylist_get(int_arraylist5, 4));
	assert_equal(0, *(int*)arraylist_get(int_arraylist5, 5));
	arraylist_free(int_arraylist5);
	int_arraylist5 = arraylist_from_array(int_values, _countof(int_values), sizeof(int), int_compare);
	arraylist_insert(int_arraylist5, 6, &int_values[0]);
	assert_equal(0, *(int*)arraylist_get(int_arraylist5, 0));
	assert_equal(1, *(int*)arraylist_get(int_arraylist5, 1));
	assert_equal(2, *(int*)arraylist_get(int_arraylist5, 2));
	assert_equal(3, *(int*)arraylist_get(int_arraylist5, 3));
	assert_equal(4, *(int*)arraylist_get(int_arraylist5, 4));
	assert_equal(0, *(int*)arraylist_get(int_arraylist5, 5));
	arraylist_free(int_arraylist5);

	// arraylist_contains
	int_arraylist0 = arraylist_new(sizeof(int), int_compare);
	assert_false(arraylist_contains(int_arraylist0, &int_values[0]));
	arraylist_free(int_arraylist0);
	int_arraylist1 = arraylist_from_array(int_values, 1, sizeof(int), int_compare);
	assert_true(arraylist_contains(int_arraylist1, &int_values[0]));
	assert_false(arraylist_contains(int_arraylist1, &int_values[1]));
	arraylist_free(int_arraylist1);
	arraylist_t *int_arraylist3 = arraylist_from_array(int_values, 3, sizeof(int), int_compare);
	assert_true(arraylist_contains(int_arraylist3, &int_values[0]));
	assert_true(arraylist_contains(int_arraylist3, &int_values[1]));
	assert_true(arraylist_contains(int_arraylist3, &int_values[2]));
	assert_false(arraylist_contains(int_arraylist3, &int_values[3]));
	arraylist_free(int_arraylist3);

	// arraylist_remove
	int_arraylist5 = arraylist_from_array(int_values, _countof(int_values), sizeof(int), int_compare);
	int five = 5;
	assert_false(arraylist_remove(int_arraylist5, &five));
	// {0,1,2,3,4}
	assert_true(arraylist_remove(int_arraylist5, &int_values[0]));
	// {1,2,3,4}
	assert_equal(4, arraylist_len(int_arraylist5));
	assert_equal(1, *(int*)arraylist_get(int_arraylist5, 0));
	assert_true(arraylist_remove(int_arraylist5, &int_values[4]));
	// {1,2,3}
	assert_equal(3, arraylist_len(int_arraylist5));
	assert_equal(3, *(int*)arraylist_get(int_arraylist5, -1));
	assert_true(arraylist_remove(int_arraylist5, &int_values[2]));
	// {1,3}
	assert_equal(1, *(int*)arraylist_get(int_arraylist5, 0));
	assert_equal(3, *(int*)arraylist_get(int_arraylist5, 1));
	assert_equal(NULL, arraylist_get(int_arraylist5, 2));
	assert_true(arraylist_remove(int_arraylist5, &int_values[1]));
	assert_true(arraylist_remove(int_arraylist5, &int_values[3]));
	// {}
	assert_equal(0, arraylist_len(int_arraylist5));
	assert_false(arraylist_remove(int_arraylist5, &int_values[3]));
	arraylist_free(int_arraylist5);

	// arraylist_delete
	int_arraylist5 = arraylist_from_array(int_values, _countof(int_values), sizeof(int), int_compare);
	assert_false(arraylist_delete(int_arraylist5, 5));
	assert_equal(5, arraylist_len(int_arraylist5));
	// {0,1,2,3,4}
	assert_true(arraylist_delete(int_arraylist5, 0));
	// {1,2,3,4}
	assert_equal(1, *(int*)arraylist_get(int_arraylist5, 0));
	assert_true(arraylist_delete(int_arraylist5, -1));
	// {1,2,3}
	assert_equal(3, *(int*)arraylist_get(int_arraylist5, -1));
	assert_true(arraylist_delete(int_arraylist5, 1));
	// {1,3}
	assert_equal(2, arraylist_len(int_arraylist5));
	assert_true(arraylist_delete(int_arraylist5, -2));
	// {3}
	assert_equal(3, *(int*)arraylist_get(int_arraylist5, 0));
	assert_true(arraylist_delete(int_arraylist5, 0));
	// {}
	assert_equal(0, arraylist_len(int_arraylist5));
	arraylist_free(int_arraylist5);

	// arraylist_grow and arraylist_shrink helpers
	arraylist_t *giant_arraylist = arraylist_new(sizeof(int), int_compare);
	for (int i = 0; i < 1000; i++) {
		assert_equal(i, arraylist_len(giant_arraylist));
		arraylist_append(giant_arraylist, &i);
		assert_equal(i, *(int*)arraylist_get(giant_arraylist, -1));
		//printf("%lld\n", giant_arraylist->phys_len);
	}
	for (int i = 999; i >= 0; i--) {
		assert_equal(i, *(int*)arraylist_get(giant_arraylist, -1));
		assert_true(arraylist_delete(giant_arraylist, -1));
		//printf("%lld\n", giant_arraylist->phys_len);
	}
	assert_equal(0, arraylist_len(giant_arraylist));
	arraylist_free(giant_arraylist);

	// arraylist_slice
	int_arraylist5 = arraylist_from_array(int_values, _countof(int_values), sizeof(int), int_compare);
	arraylist_t *sliced = arraylist_slice(int_arraylist5, 1, 1);
	assert_equal(0, arraylist_len(sliced));
	arraylist_free(sliced);
	sliced = arraylist_slice(int_arraylist5, -1, -1);
	assert_equal(0, arraylist_len(sliced));
	arraylist_free(sliced);
	sliced = arraylist_slice(int_arraylist5, 0, 1);
	assert_equal(1, arraylist_len(sliced));
	assert_equal(0, *(int*)arraylist_get(sliced, 0));
	arraylist_free(sliced);
	sliced = arraylist_slice(int_arraylist5, 0, -1);
	assert_equal(4, arraylist_len(sliced));
	for (int i = 0; i <= 3; i++) {
		assert_equal(i, *(int*)arraylist_get(sliced, i));
	}
	arraylist_free(sliced);
	sliced = arraylist_slice(int_arraylist5, -20, 1);
	assert_equal(1, arraylist_len(sliced));
	assert_equal(0, *(int*)arraylist_get(sliced, 0));
	arraylist_free(sliced);
	sliced = arraylist_slice(int_arraylist5, -20, -10);
	assert_equal(0, arraylist_len(sliced));
	arraylist_free(sliced);
	sliced = arraylist_slice(int_arraylist5, 5, -1);
	assert_equal(0, arraylist_len(sliced));
	arraylist_free(sliced);
	sliced = arraylist_slice(int_arraylist5, 3, 2);
	assert_equal(0, arraylist_len(sliced));
	arraylist_free(sliced);
	sliced = arraylist_slice(int_arraylist5, 4, 20);
	assert_equal(1, arraylist_len(sliced));
	assert_equal(4, *(int*)arraylist_get(sliced, 0));
	arraylist_free(sliced);
	sliced = arraylist_slice(int_arraylist5, -1, 20);
	assert_equal(1, arraylist_len(sliced));
	assert_equal(4, *(int*)arraylist_get(sliced, 0));
	arraylist_free(sliced);
	arraylist_free(int_arraylist5);

	// arraylist_pop
	int_arraylist5 = arraylist_from_array(int_values, _countof(int_values), sizeof(int), int_compare);
	int dest = 1;
	// {0,1,2,3,4}
	assert_false(arraylist_pop(int_arraylist5, 5, &dest));
	assert_equal(1, dest);
	assert_true(arraylist_pop(int_arraylist5, 0, &dest));
	// {1,2,3,4}
	assert_equal(0, dest);
	assert_true(arraylist_pop(int_arraylist5, 3, &dest));
	// {1,2,3}
	assert_equal(4, dest);
	assert_equal(3, arraylist_len(int_arraylist5));
	assert_true(arraylist_pop(int_arraylist5, 1, &dest));
	// {1,3}
	assert_equal(2, dest);
	assert_true(arraylist_pop(int_arraylist5, 0, &dest));
	// {3}
	assert_equal(1, dest);
	assert_true(arraylist_pop(int_arraylist5, 0, &dest));
	// {}
	assert_equal(3, dest);
	assert_equal(0, arraylist_len(int_arraylist5));
	arraylist_free(int_arraylist5);

	// arraylist_clear
	int_arraylist5 = arraylist_from_array(int_values, _countof(int_values), sizeof(int), int_compare);
	arraylist_clear(int_arraylist5);
	assert_equal(0, arraylist_len(int_arraylist5));
	arraylist_free(int_arraylist5);

	// arraylist_find, arraylist_rfind and arraylist_count
	int_arraylist5 = arraylist_from_array(int_values, _countof(int_values), sizeof(int), int_compare);
	arraylist_append(int_arraylist5, &int_values[1]);
	// {0,1,2,3,4,1}
	assert_equal(0, arraylist_find(int_arraylist5, &int_values[0]));
	assert_equal(0, arraylist_rfind(int_arraylist5, &int_values[0]));
	assert_equal(1, arraylist_find(int_arraylist5, &int_values[1]));
	assert_equal(5, arraylist_rfind(int_arraylist5, &int_values[1]));
	assert_equal(2, arraylist_find(int_arraylist5, &int_values[2]));
	assert_equal(2, arraylist_rfind(int_arraylist5, &int_values[2]));
	assert_equal(-1, arraylist_find(int_arraylist5, &five));
	assert_equal(-1, arraylist_rfind(int_arraylist5, &five));
	assert_equal(2, arraylist_count(int_arraylist5, &int_values[1]));
	assert_equal(1, arraylist_count(int_arraylist5, &int_values[0]));
	assert_equal(0, arraylist_count(int_arraylist5, &five));
	arraylist_free(int_arraylist5);

	// arraylist_reverse
	int_arraylist0 = arraylist_new(sizeof(int), int_compare);
	arraylist_reverse(int_arraylist0);
	assert_equal(NULL, arraylist_get(int_arraylist0, 0));
	assert_equal(0, arraylist_len(int_arraylist0));
	arraylist_free(int_arraylist0);
	int_arraylist1 = arraylist_from_array(int_values, 1, sizeof(int), int_compare);
	arraylist_reverse(int_arraylist1);
	assert_equal(0, *(int*)arraylist_get(int_arraylist1, 0));
	assert_equal(1, arraylist_len(int_arraylist1));
	arraylist_free(int_arraylist1);
	arraylist_t *int_arraylist2 = arraylist_from_array(int_values, 2, sizeof(int), int_compare);
	arraylist_reverse(int_arraylist2);
	assert_equal(1, *(int*)arraylist_get(int_arraylist2, 0));
	assert_equal(0, *(int*)arraylist_get(int_arraylist2, 1));
	arraylist_free(int_arraylist2);
	int_arraylist5 = arraylist_from_array(int_values, _countof(int_values), sizeof(int), int_compare);
	arraylist_reverse(int_arraylist5);
	for (int i = 0; i < 5; i++) {
		assert_equal(4 - i, *(int*)arraylist_get(int_arraylist5, i));
	}
	arraylist_free(int_arraylist5);

	// arraylist_copy
	int_arraylist0 = arraylist_new(sizeof(int), int_compare);
	arraylist_t *copy = arraylist_copy(int_arraylist0);
	assert_equal(arraylist_len(int_arraylist0), arraylist_len(copy));
	assert_equal(arraylist_get(int_arraylist0, 0), arraylist_get(copy, 0));
	arraylist_free(int_arraylist0);
	arraylist_free(copy);
	int_arraylist5 = arraylist_from_array(int_values, _countof(int_values), sizeof(int), int_compare);
	copy = arraylist_copy(int_arraylist5);
	assert_equal(arraylist_len(int_arraylist5), arraylist_len(copy));
	for (int i = 0; i < 5; i++) {
		assert_equal(*(int*)arraylist_get(int_arraylist5, i), *(int*)arraylist_get(copy, i));
	}
	arraylist_free(int_arraylist5);
	arraylist_free(copy);

	// arraylist_compare
	// {} == {}
	int_arraylist0 = arraylist_new(sizeof(int), int_compare);
	arraylist_t *int_arraylist0_copy = arraylist_new(sizeof(int), int_compare);
	assert_equal(0, arraylist_compare(int_arraylist0, int_arraylist0_copy));
	// {} < {0}
	arraylist_append(int_arraylist0_copy, &int_values[0]);
	assert_true(arraylist_compare(int_arraylist0, int_arraylist0_copy) < 0);
	// {0} > {}
	assert_true(arraylist_compare(int_arraylist0_copy, int_arraylist0) > 0);
	// {0} == {0}
	arraylist_append(int_arraylist0, &int_values[0]);
	assert_equal(0, arraylist_compare(int_arraylist0, int_arraylist0_copy));
	// {0,1} > {0}
	arraylist_append(int_arraylist0, &int_values[1]);
	assert_true(arraylist_compare(int_arraylist0, int_arraylist0_copy) > 0);
	// {0} < {0,1}
	assert_true(arraylist_compare(int_arraylist0_copy, int_arraylist0) < 0);
	// {0,1} == {0,1}
	arraylist_append(int_arraylist0_copy, &int_values[1]);
	assert_equal(0, arraylist_compare(int_arraylist0, int_arraylist0_copy));
	// {0,1} < {0,2}
	arraylist_set(int_arraylist0_copy, 1, &int_values[2]);
	assert_true(arraylist_compare(int_arraylist0, int_arraylist0_copy) < 0);
	// {0,2} > {0,1}
	assert_true(arraylist_compare(int_arraylist0_copy, int_arraylist0) > 0);
	// {1,0} > {0,1}
	arraylist_set(int_arraylist0, 0, &int_values[1]);
	arraylist_set(int_arraylist0, 1, &int_values[0]);
	arraylist_set(int_arraylist0_copy, 0, &int_values[0]);
	arraylist_set(int_arraylist0_copy, 1, &int_values[1]);
	assert_true(arraylist_compare(int_arraylist0, int_arraylist0_copy) > 0);
	// {0,1} < {1,0}
	assert_true(arraylist_compare(int_arraylist0_copy, int_arraylist0) < 0);
	arraylist_free(int_arraylist0);
	arraylist_free(int_arraylist0_copy);

	// arraylist_foreach
	int_arraylist5 = arraylist_from_array(int_values, _countof(int_values), sizeof(int), int_compare);
	arraylist_foreach(int_arraylist5, increment);
	for (int i = 0; i < arraylist_len(int_arraylist5); i++) {
		assert_equal(i + 1, *(int*)arraylist_get(int_arraylist5, i));
	}
	arraylist_free(int_arraylist5);

	// arraylist iterator
	int_arraylist5 = arraylist_from_array(int_values, _countof(int_values), sizeof(int), int_compare);
	arraylist_iter_t *iter = arraylist_iter_new(int_arraylist5);
	assert_equal(0, *(int*)arraylist_iter_next(iter));
	assert_equal(1, *(int*)arraylist_iter_next(iter));
	assert_equal(2, *(int*)arraylist_iter_next(iter));
	assert_equal(3, *(int*)arraylist_iter_next(iter));
	assert_equal(4, *(int*)arraylist_iter_next(iter));
	assert_equal(NULL, arraylist_iter_next(iter));
	assert_equal(NULL, arraylist_iter_next(iter));
	arraylist_iter_reset(iter);
	assert_equal(0, *(int*)arraylist_iter_next(iter));
	arraylist_iter_free(iter);
	arraylist_free(int_arraylist5);
}


int main(void) {
	run_test(test_arraylist);
	return EXIT_SUCCESS;
}

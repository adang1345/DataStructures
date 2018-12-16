#include "datastructures.h"
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

/* general purpose assertions for testing */
#define assert_equal(a, b) if ((a) != (b)) {printf("assert_equal fail: line %i\n", __LINE__); exit(EXIT_FAILURE);}
#define assert_true(a) if (!a) {printf("assert_true fail: line %i\n", __LINE__); exit(EXIT_FAILURE);}
#define assert_false(a) if (a) {printf("assert_false fail: line %i\n", __LINE__); exit(EXIT_FAILURE);}
#define assert_not_equal(a, b) if ((a) == (b)) {printf("assert_not_equal fail: line %i\n", __LINE__); exit(EXIT_FAILURE);}
#define assert_equal_sized(a, b, size) \
	if (strncmp((char*)(a), (char*)(b), (size))) {printf("assert_equal_sized fail: line %i\n", __LINE__); exit(EXIT_FAILURE);}
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
}



int main(void) {
	run_test(test_arraylist);
	return EXIT_SUCCESS;
}

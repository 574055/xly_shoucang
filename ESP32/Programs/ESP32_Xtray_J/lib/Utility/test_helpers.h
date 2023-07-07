#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <etl/string_view.h>
#include <string>
#include <unity.h>


template <typename T>
std::string to_string(const T& t)
{
    return std::to_string(t);
}

std::string to_string(etl::string_view sv)
{
    return std::string{sv.data(), sv.length()};
}

std::string to_string(const char* s)
{
    return std::string{s};
}


#define TEST_ASSERT_GEN(op, expected, actual) \
    UNITY_TEST_ASSERT(                        \
        (op), __LINE__,                       \
        (std::string("expected '") + std::to_string(expected) + "', got '" + std::to_string(actual) + "'").c_str())

#define TEST_ASSERT_EQ_INT TEST_ASSERT_EQUAL_INT
#define TEST_ASSERT_LT_INT TEST_ASSERT_LESS_THAN_INT
#define TEST_ASSERT_LE_INT TEST_ASSERT_LESS_OR_EQUAL_INT
#define TEST_ASSERT_GT_INT TEST_ASSERT_GREATER_THAN_INT
#define TEST_ASSERT_GE_INT TEST_ASSERT_GREAT_OR_EQUAL_INT

#define TEST_ASSERT_EQ_FLOAT(expected, actual, delta) \
    TEST_ASSERT_GEN(abs((expected) - (actual)) < (delta), expected, actual)

// Generic asserts.
#define TEST_ASSERT_EQ_GEN(expected, actual) TEST_ASSERT_GEN((expected) == (actual), expected, actual)
#define TEST_ASSERT_LT_GEN(expected, actual) TEST_ASSERT_GEN((expected) < (actual), expected, actual)
#define TEST_ASSERT_LE_GEN(expected, actual) TEST_ASSERT_GEN((expected) <= (actual), expected, actual)
#define TEST_ASSERT_GT_GEN(expected, actual) TEST_ASSERT_GEN((expected) > (actual), expected, actual)
#define TEST_ASSERT_GE_GEN(expected, actual) TEST_ASSERT_GEN((expected) >= (actual), expected, actual)

#define TEST_ASSERT_LT_FLOAT(e, a) TEST_ASSERT_LT_GEN(e, a)
#define TEST_ASSERT_LE_FLOAT(e, a) TEST_ASSERT_LE_GEN(e, a)
#define TEST_ASSERT_GT_FLOAT(e, a) TEST_ASSERT_GT_GEN(e, a)
#define TEST_ASSERT_GE_FLOAT(e, a) TEST_ASSERT_GE_GEN(e, a)

#endif
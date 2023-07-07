#include "stringreader.h"

#include <string>
#include <unity.h>


void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

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
        (std::string("expected '") + to_string(expected) + "', got '" + to_string(actual) + "'").c_str())

#define TEST_ASSERT_EQ_INT                     TEST_ASSERT_EQUAL_INT
#define TEST_ASSERT_LT_INT                     TEST_ASSERT_LESS_THAN_INT
#define TEST_ASSERT_LE_INT                     TEST_ASSERT_LESS_OR_EQUAL_INT
#define TEST_ASSERT_GT_INT                     TEST_ASSERT_GREATER_THAN_INT
#define TEST_ASSERT_GE_INT                     TEST_ASSERT_GREAT_OR_EQUAL_INT
#define TEST_ASSERT_EQ_GEN(expected, actual)   TEST_ASSERT_GEN((expected) == (actual), expected, actual)
#define TEST_ASSERT_EQ_FLOAT(expected, actual) TEST_ASSERT_GEN(abs((expected) - (actual)) < 1e-6, expected, actual)
#define TEST_ASSERT_LT_GEN(expected, actual)   TEST_ASSERT_GEN((expected) < (actual), expected, actual)
#define TEST_ASSERT_LE_GEN(expected, actual)   TEST_ASSERT_GEN((expected) <= (actual), expected, actual)
#define TEST_ASSERT_GT_GEN(expected, actual)   TEST_ASSERT_GEN((expected) > (actual), expected, actual)
#define TEST_ASSERT_GE_GEN(expected, actual)   TEST_ASSERT_GEN((expected) >= (actual), expected, actual)


void test_stringreader_read_val()
{
    StringReader s{"(1,2.0),(3.141592,abc)"};
    TEST_ASSERT_TRUE(s.skip('('));
    TEST_ASSERT_FALSE(s.skip(','));
    TEST_ASSERT_EQ_INT(1, s.read<int>(','));
    TEST_ASSERT_TRUE(s.skip(','));
    TEST_ASSERT_EQUAL_FLOAT(2.0, s.read<float>(')'));
    TEST_ASSERT_TRUE(s.skip(')'));
    TEST_ASSERT_TRUE(s.skip(','));
    TEST_ASSERT_TRUE(s.skip('('));
    TEST_ASSERT_EQ_FLOAT(3.141592, s.read<double>(','));
    TEST_ASSERT_TRUE(s.skip(','));
    TEST_ASSERT_EQ_GEN("abc", s.read<etl::string_view>(')'));
    TEST_ASSERT_TRUE(s.skip(')'));
}

void test_stringreader_read_safe()
{
    StringReader s{"1 3.141592 abc"};
    auto r1 = s.readSafe<int>();
    TEST_ASSERT_TRUE(s.skip(' '));
    TEST_ASSERT_TRUE(r1.has_value());
    TEST_ASSERT_EQ_INT(1, *r1);

    auto r2 = s.readSafe<int>();
    TEST_ASSERT_FALSE(s.skip(' '));
    TEST_ASSERT_TRUE(!r2.has_value());

    auto r3 = s.readSafe<float>();
    TEST_ASSERT_TRUE(s.skip(' '));
    TEST_ASSERT_TRUE(r3.has_value());
    TEST_ASSERT_EQ_FLOAT(3.141592, *r3);

    auto r4 = s.readSafe<float>();
    TEST_ASSERT_TRUE(!r4.has_value());

    auto r5 = s.readSafe<etl::string_view>();
    TEST_ASSERT_FALSE(s.skip(' '));
    TEST_ASSERT_TRUE(r5.has_value());
    TEST_ASSERT_EQ_GEN("abc", *r5);
}

void test_stringreader_read_until()
{
    StringReader s{"abc 123 def <>?"};
    auto str0 = s.readUntil(' ');
    auto str0_empty = s.readUntil(' ');
    TEST_ASSERT_TRUE(s.skip(' '));

    auto str1 = s.readUntil(' ');
    TEST_ASSERT_TRUE(s.skip(' '));

    auto str2 = s.readUntil(' ');
    TEST_ASSERT_TRUE(s.skip(' '));

    auto str3 = s.readUntil(' ');
    TEST_ASSERT_FALSE(s.skip(' '));

    TEST_ASSERT_EQ_GEN("abc", str0);
    TEST_ASSERT_TRUE(str0_empty.empty());
    TEST_ASSERT_EQ_GEN("123", str1);
    TEST_ASSERT_EQ_GEN("def", str2);

    TEST_ASSERT_EQ_GEN("<>?", str3);
    TEST_ASSERT_EQ_INT(0, s.available());
}

void test_stringreader_read_token()
{
    StringReader s{"abc 123 def <>?"};
    auto str0 = s.readToken(' ');
    auto str1 = s.readToken(' ');
    auto str2 = s.readToken(' ');
    auto str3 = s.readToken(' ');

    TEST_ASSERT_EQ_GEN("abc", str0);
    TEST_ASSERT_EQ_GEN("123", str1);
    TEST_ASSERT_EQ_GEN("def", str2);
    TEST_ASSERT_EQ_GEN("<>?", str3);
    TEST_ASSERT_EQ_INT(0, s.available());
}


void test_stringreader_read_pair_1()
{
    StringReader s{"<1,2>"};
    auto p = s.read<int, int>();
    TEST_ASSERT_EQ_INT(1, std::get<0>(p));
    TEST_ASSERT_EQ_INT(2, std::get<1>(p));
    TEST_ASSERT_EQ_INT(0, s.available());
    TEST_ASSERT_TRUE(s.read<etl::string_view>().empty());
}

void test_stringreader_read_pair_2()
{
    StringReader s{"<3,4"};
    auto p = s.read<int, int>();
    TEST_ASSERT_EQ_INT(3, std::get<0>(p));
    TEST_ASSERT_EQ_INT(4, std::get<1>(p));
    TEST_ASSERT_EQ_INT(0, s.available());
    TEST_ASSERT_TRUE(s.read<etl::string_view>().empty());
}

void test_stringreader_read_pair_3()
{
    StringReader s{"<5,6> "};
    auto p = s.readSafe<int, int>();
    TEST_ASSERT_TRUE(p.has_value());
    TEST_ASSERT_EQ_INT(5, std::get<0>(*p));
    TEST_ASSERT_EQ_INT(6, std::get<1>(*p));

    TEST_ASSERT_EQ_INT(1, s.available());
}

void test_stringreader_read_pair_4()
{
    StringReader s{"<7,8"};
    auto p = s.readSafe<int, int>();
    TEST_ASSERT_TRUE(!p.has_value());
}

void test_stringreader_read_pair_5()
{
    StringReader s{"<78>"};
    auto p = s.readSafe<int, int>();
    TEST_ASSERT_TRUE(!p.has_value());
}

void test_stringreader_read_tuple_1()
{
    StringReader s{"<1,abc,3.14> "};
    auto p = s.readSafe<int, etl::string_view, float>();
    TEST_ASSERT_TRUE(p.has_value());
    TEST_ASSERT_EQ_INT(1, std::get<0>(*p));
    TEST_ASSERT_EQ_GEN("abc", std::get<1>(*p));
    TEST_ASSERT_EQ_FLOAT(3.14, std::get<2>(*p));

    TEST_ASSERT_EQ_INT(1, s.available());
}

void test_stringreader_read_tuple_2()
{
    StringReader s{"(1|abc|3.14) "};
    auto p1 = s.readSafe<int, etl::string_view, float>();
    TEST_ASSERT_TRUE(!p1.has_value());

    auto p2 = s.readSafe<int, etl::string_view, float>("()", '|');
    TEST_ASSERT_TRUE(p2.has_value());
    TEST_ASSERT_EQ_INT(1, std::get<0>(*p2));
    TEST_ASSERT_EQ_GEN("abc", std::get<1>(*p2));
    TEST_ASSERT_EQ_FLOAT(3.14, std::get<2>(*p2));

    TEST_ASSERT_EQ_INT(1, s.available());
}

void test_stringreader_read_tuple_4()
{
    StringReader s{"<1,2,3,4> "};
    auto p1 = s.readSafe<int, int, int>();
    TEST_ASSERT_TRUE(!p1.has_value());

    auto p2 = s.readSafe<int, int, int, int>();
    TEST_ASSERT_TRUE(p2.has_value());
    TEST_ASSERT_EQ_INT(1, std::get<0>(*p2));
    TEST_ASSERT_EQ_INT(2, std::get<1>(*p2));
    TEST_ASSERT_EQ_INT(3, std::get<2>(*p2));
    TEST_ASSERT_EQ_INT(4, std::get<3>(*p2));

    TEST_ASSERT_EQ_INT(1, s.available());
}

void test_stringreader_read_tuple_5()
{
    StringReader s{"<1,2,3,4,5> "};
    auto p1 = s.readSafe<int>();
    TEST_ASSERT_TRUE(!p1.has_value());

    auto p2 = s.readSafe<int, int>();
    TEST_ASSERT_TRUE(!p2.has_value());

    auto p3 = s.readSafe<int, int, int>();
    TEST_ASSERT_TRUE(!p3.has_value());

    auto p4 = s.readSafe<int, int, int, int>();
    TEST_ASSERT_TRUE(!p4.has_value());

    auto p5 = s.readSafe<int, int, int, int, int>();
    TEST_ASSERT_TRUE(p5.has_value());
    TEST_ASSERT_EQ_INT(1, std::get<0>(*p5));
    TEST_ASSERT_EQ_INT(2, std::get<1>(*p5));
    TEST_ASSERT_EQ_INT(3, std::get<2>(*p5));
    TEST_ASSERT_EQ_INT(4, std::get<3>(*p5));
    TEST_ASSERT_EQ_INT(5, std::get<4>(*p5));

    TEST_ASSERT_EQ_INT(1, s.available());
}


int main(int argc, char** argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_stringreader_read_val);
    RUN_TEST(test_stringreader_read_safe);
    RUN_TEST(test_stringreader_read_until);
    RUN_TEST(test_stringreader_read_token);
    RUN_TEST(test_stringreader_read_pair_1);
    RUN_TEST(test_stringreader_read_pair_2);
    RUN_TEST(test_stringreader_read_pair_3);
    RUN_TEST(test_stringreader_read_pair_4);
    RUN_TEST(test_stringreader_read_pair_5);
    RUN_TEST(test_stringreader_read_tuple_1);
    RUN_TEST(test_stringreader_read_tuple_2);
    RUN_TEST(test_stringreader_read_tuple_4);
    RUN_TEST(test_stringreader_read_tuple_5);

    UNITY_END();
}
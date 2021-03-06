#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "rapidjson/document.h"
#include "mstch/mstch.hpp"
#include "test_context.hpp"
#include "test_data.hpp"
#include "specs_data.hpp"
#include "specs_lambdas.hpp"

using namespace mstchtest;

mstch::node to_value(const rapidjson::Value& val) {
  if (val.IsString())
    return std::string{val.GetString()};
  if (val.IsBool())
    return val.GetBool();
  if (val.IsDouble())
    return val.GetDouble();
  if (val.IsInt())
    return val.GetInt();
  return mstch::node{};
}

mstch::array to_array(const rapidjson::Value& val);

mstch::map to_object(const rapidjson::Value& val) {
  mstch::map ret;
  for (auto i = val.MemberBegin(); i != val.MemberEnd(); ++i) {
    if (i->value.IsArray())
      ret.insert(std::make_pair(i->name.GetString(), to_array(i->value)));
    else if (i->value.IsObject())
      ret.insert(std::make_pair(i->name.GetString(), to_object(i->value)));
    else
      ret.insert(std::make_pair(i->name.GetString(), to_value(i->value)));
  }
  return ret;
}

mstch::array to_array(const rapidjson::Value& val) {
  mstch::array ret;
  for (auto i = val.Begin(); i != val.End(); ++i) {
    if (i->IsArray())
      ret.push_back(to_array(*i));
    else if (i->IsObject())
      ret.push_back(to_object(*i));
    else
      ret.push_back(to_value(*i));
  }
  return ret;
}

mstch::node parse_with_rapidjson(const std::string& str) {
  rapidjson::Document doc;
  doc.Parse(str.c_str());
  return to_object(doc);
}

#define MSTCH_PARTIAL_TEST(x) TEST_CASE(#x) { \
  REQUIRE(x ## _txt == mstch::render(x ## _mustache, x ## _data, {{"partial", x ## _partial}})); \
}

#define MSTCH_TEST(x) TEST_CASE(#x) { \
  REQUIRE(x ## _txt == mstch::render(x ## _mustache, x ## _data)); \
}

#define SPECS_TEST(x) TEST_CASE("specs_" #x) { \
  auto data = parse_with_rapidjson(x ## _json); \
  for (auto& test_item: data.get_map()["tests"].get_array()) {\
    auto test = test_item.get_map(); \
    std::map<std::string,std::string> partials; \
    if (test.count("partials")) \
      for (auto& partial_item: test["partials"].get_map()) \
        partials.insert(std::make_pair(partial_item.first, partial_item.second.get_string())); \
    mstch::map context; \
    for (auto& data_item: test["data"].get_map()) \
      if (data_item.first == "lambda") \
        context.insert(std::make_pair(data_item.first, specs_lambdas[test["name"].get_string()])); \
      else \
        context.insert(data_item); \
    SECTION(test["name"].get_string()) \
      REQUIRE(mstch::render( \
          test["template"].get_string(), \
          context, partials) == \
          test["expected"].get_string()); \
  } \
}

MSTCH_TEST(ampersand_escape)
MSTCH_TEST(apostrophe)
MSTCH_TEST(array_of_strings)
MSTCH_TEST(backslashes)
MSTCH_TEST(bug_11_eating_whitespace)
MSTCH_TEST(bug_length_property)
MSTCH_TEST(changing_delimiters)
MSTCH_TEST(comments)
MSTCH_TEST(complex)
MSTCH_TEST(context_lookup)
MSTCH_TEST(delimiters)
MSTCH_TEST(disappearing_whitespace)
MSTCH_TEST(dot_notation)
MSTCH_TEST(double_render)
MSTCH_TEST(empty_list)
MSTCH_TEST(empty_sections)
MSTCH_TEST(empty_string)
MSTCH_TEST(empty_template)
MSTCH_TEST(error_eof_in_section)
MSTCH_TEST(error_eof_in_tag)
MSTCH_TEST(error_not_found)
MSTCH_TEST(escaped)
MSTCH_TEST(falsy)
MSTCH_TEST(falsy_array)
MSTCH_TEST(grandparent_context)
MSTCH_TEST(higher_order_sections)
MSTCH_TEST(implicit_iterator)
MSTCH_TEST(included_tag)
MSTCH_TEST(inverted_section)
MSTCH_TEST(keys_with_questionmarks)
MSTCH_TEST(multiline_comment)
MSTCH_TEST(nested_dot)
MSTCH_TEST(nested_higher_order_sections)
MSTCH_TEST(nested_iterating)
MSTCH_TEST(nesting)
MSTCH_TEST(nesting_same_name)
MSTCH_TEST(null_lookup_array)
MSTCH_TEST(null_lookup_object)
MSTCH_TEST(null_string)
MSTCH_TEST(null_view)
MSTCH_PARTIAL_TEST(partial_array)
MSTCH_PARTIAL_TEST(partial_array_of_partials)
MSTCH_PARTIAL_TEST(partial_array_of_partials_implicit)
MSTCH_PARTIAL_TEST(partial_empty)
MSTCH_PARTIAL_TEST(partial_template)
MSTCH_PARTIAL_TEST(partial_view)
MSTCH_PARTIAL_TEST(partial_whitespace)
MSTCH_TEST(recursion_with_same_names)
MSTCH_TEST(reuse_of_enumerables)
MSTCH_TEST(section_as_context)
MSTCH_PARTIAL_TEST(section_functions_in_partials)
MSTCH_TEST(simple)
MSTCH_TEST(string_as_context)
MSTCH_TEST(two_in_a_row)
MSTCH_TEST(two_sections)
MSTCH_TEST(unescaped)
MSTCH_TEST(whitespace)
MSTCH_TEST(zero_view)

SPECS_TEST(comments)
SPECS_TEST(delimiters)
SPECS_TEST(interpolation)
SPECS_TEST(inverted)
SPECS_TEST(partials)
SPECS_TEST(sections)
SPECS_TEST(lambdas)

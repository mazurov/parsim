#define BOOST_TEST_MODULE Parsim
#include <boost/test/included/unit_test.hpp>

#include "parsim.h"
#include <string>
#include <vector>
#include <map>
#include <limits>
#include <sstream>

struct printer
{
    typedef boost::spirit::utf8_string string;

    void element(string const& tag, string const& value, int depth) const
    {
        for (int i = 0; i < (depth*4); ++i) // indent to depth
            std::cout << ' ';

        std::cout << "tag: " << tag;
        if (value != "")
            std::cout << ", value: " << value;
        std::cout << std::endl;
    }
};

void print_info(boost::spirit::info const& what)
{
    using boost::spirit::basic_info_walker;

    printer pr;
    basic_info_walker<printer> walker(pr, what.tag, 0);
    boost::apply_visitor(walker, what.value);
}

using namespace std;
namespace qi = boost::spirit::qi;

BOOST_AUTO_TEST_CASE( primitive )
{
    {
      int result;
      BOOST_REQUIRE(parsim::parse_("123", result));
      BOOST_CHECK_EQUAL(result, 123);
    }

    {
      int result;
      BOOST_REQUIRE(parsim::parse_("123", result));
      BOOST_CHECK_EQUAL(result, 123);
    }

  {
      std::string result;
      BOOST_REQUIRE(parsim::parse_("'\u00F6'", result));
      cout << result << endl;
      BOOST_CHECK(result == "\u00F6");
    }

    /*{
      std::wstring result;
      BOOST_REQUIRE(parsim::parse_(L"'Hello'", result));
      BOOST_CHECK_EQUAL(result, "Hello");
    }*/
    {
      long result;
      string number;
      
      stringstream strstream;
      strstream << LONG_MAX;
      strstream >> number;
      BOOST_CHECK(parsim::parse_(number, result));
      BOOST_CHECK_EQUAL(result, LONG_MAX);
    }

    {
      unsigned long result;
      string number;
      
      stringstream strstream;
      strstream << ULONG_MAX;
      strstream >> number;
      BOOST_CHECK(parsim::parse_(number, result));
      BOOST_CHECK_EQUAL(result, ULONG_MAX);
    }    

    {
      double result;
      BOOST_CHECK(parsim::parse_("123.1", result));
      BOOST_CHECK_EQUAL(result, 123.1);
    }
    // Fails
    try{
      int result;
      BOOST_REQUIRE(!parsim::parse_("123.4", result));
    }catch(parsim::expectation_failure err){
      BOOST_CHECK_EQUAL(err.what_.tag, "eoi");
      BOOST_CHECK_EQUAL(string(err.first, err.last), ".4");
    }

}


BOOST_AUTO_TEST_CASE( lists )
{
    {
      std::vector<int> result;
      BOOST_REQUIRE(parsim::parse_("[1,2,3]", result));
      BOOST_REQUIRE(result.size() == 3);
      BOOST_CHECK_EQUAL(result[0], 1);
      BOOST_CHECK_EQUAL(result[1], 2);
      BOOST_CHECK_EQUAL(result[2], 3);
    }

    try{
      std::vector<int> result;
      BOOST_REQUIRE(!parsim::parse_("[1,2,3", result));
    }catch(parsim::expectation_failure err){
      BOOST_CHECK_EQUAL(err.what_.tag, "] or } or )");
      BOOST_CHECK_EQUAL(string(err.first, err.last), "");
    }  

}


BOOST_AUTO_TEST_CASE( maps )
{
    {
      std::map<int,string> result;
      BOOST_REQUIRE(parsim::parse_("{1:'hello', 2:'world'}", result));
      BOOST_REQUIRE(result.size() == 2);
      BOOST_REQUIRE_EQUAL(result.count(1), 1);
      BOOST_REQUIRE_EQUAL(result.count(2), 1);
      
      BOOST_CHECK_EQUAL(result[1], "hello");
      BOOST_CHECK_EQUAL(result[2], "world");
    }

    try{
      std::map<int,string> result;
      BOOST_REQUIRE(parsim::parse_("{1:'hello', 2:, 3:'world'}", result));
    }catch(parsim::expectation_failure err){
      BOOST_CHECK_EQUAL(err.what_.tag, "string");
      BOOST_CHECK_EQUAL(string(err.first, err.last), ", 3:'world'}");
    }
}

/*
 * Copyright (C) 2012 the parsim contributors
 *
 * This file is part of parsim, distributed under the New BSD License.
 * For full terms see the included COPYING file.
 */
// =============================================================================
#ifndef PARSIM_INCLUDE_PARSIM_GRAMMARS_H
#define PARSIM_INCLUDE_PARSIM_GRAMMARS_H 1
#ifdef __GNUC__
#pragma GCC system_header
#endif
// =============================================================================
// Include files
// =============================================================================
// STD:
//==============================================================================
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <tuple>
//==============================================================================
// Boost:
//==============================================================================
#define BOOST_SPIRIT_UNICODE // We'll use unicode (UTF8) all throughou
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/unused.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>

#include <boost/spirit/repository/include/qi_confix.hpp>
//==============================================================================
namespace parsim {
//==============================================================================
// Namespace aliases:
//==============================================================================
namespace sp    = boost::spirit;
namespace ph    = boost::phoenix;
namespace qi    = sp::qi;
namespace enc   = sp::unicode;
namespace rep   = sp::repository;
//==============================================================================
// Grammars
//==============================================================================
template <typename Iterator, typename T,  typename Skipper,
  class Enable = void>
struct Grammar_ {
  /* READ THIS IF YOUR COMPILE BREAKS ON THE FOLLOWING LINE
   *
   * To users: You have to ask developers to implement parser for your type T
   * To developer: You have to implement and register Grammar for type T
   *
   */
  BOOST_MPL_ASSERT_MSG(false, GRAMMAR_FOR_TYPE_DOES_NOT_EXISTS, (T));
};

#define REGISTER_GRAMMAR(ResultType, GrammarName) \
    template <typename Iterator, typename Skipper>\
    struct Grammar_<Iterator, ResultType, Skipper> \
    { \
        typedef GrammarName<Iterator, Skipper> Grammar;\
    }
//==============================================================================
template< typename Iterator>
struct SkipperGrammar  : qi::grammar<Iterator>
{
  SkipperGrammar() : SkipperGrammar::base_type(comments) {
    comments = enc::space | rep::confix("/*", "*/")[*(qi::char_ - "*/")]
          |
          rep::confix("//", (sp::eol | sp::eoi))
            [*(qi::char_ - (sp::eol|sp::eoi))]
          |
          rep::confix("#", (sp::eol | sp::eoi))
            [*(qi::char_ - (sp::eol|sp::eoi))];
    comments.name("comment");
  }
  qi::rule<Iterator> comments;
};
//==============================================================================
template< typename Iterator, typename Skipper>
struct StringGrammar : qi::grammar<Iterator, std::string(), qi::locals<char>,
                                                             Skipper>
{
//------------------------------------------------------------------------------
    typedef std::string ResultT;
//------------------------------------------------------------------------------
    StringGrammar() : StringGrammar::base_type( str ) {
        begin_quote   = enc::char_("\"'");
        quote     = enc::char_(qi::_r1);

        str = qi::lexeme[begin_quote[qi::_a = qi::_1]
                > *( (enc::char_('\\') > quote(qi::_a))[qi::_val += qi::_a]
                    | (enc::char_[qi::_val += qi::_1] - quote(qi::_a))) >
               quote(qi::_a)]
              ;
        str.name("string");
        begin_quote.name("begin_quote");
        quote.name("end_quote");
    }
//------------------------------------------------------------------------------
    qi::rule<Iterator, std::string(), qi::locals<char>, Skipper> str;
    qi::rule<Iterator, char()> begin_quote;
    qi::rule<Iterator, void(char)> quote;
//------------------------------------------------------------------------------
};
REGISTER_GRAMMAR(std::string, StringGrammar);
//REGISTER_GRAMMAR(std::wstring, StringGrammar);
//==============================================================================
template< typename Iterator, typename Skipper>
struct CharGrammar : qi::grammar<Iterator, char(), Skipper>
{
    typedef char ResultT;
    CharGrammar() : CharGrammar::base_type( ch ) {
        ch = qi::int_parser<char>()
            |
            '\'' > (qi::char_-'\'') > '\'';
        ch.name("char");
    }
    qi::rule<Iterator, char(), Skipper> ch;
};
REGISTER_GRAMMAR(char, CharGrammar);
//REGISTER_GRAMMAR(wchar_t, CharGrammar);
//==============================================================================
template< typename Iterator, typename Skipper>
struct BoolGrammar : qi::grammar<Iterator, bool(), Skipper>
{
    typedef bool ResultT;
    BoolGrammar() : BoolGrammar::base_type( boolean_literal ) {
        boolean_literal =
            (qi::lit("true") | "True" | "TRUE" | "1")[qi::_val=true]
            |
            (qi::lit("false") | "False" | "FALSE" | "0")[qi::_val=false];
        boolean_literal.name("boolean");
    }
    qi::rule<Iterator, bool(), Skipper> boolean_literal;
};
REGISTER_GRAMMAR(bool, BoolGrammar);
//==============================================================================
template< typename Iterator, typename RT , typename Skipper>
struct IntGrammar : qi::grammar<Iterator, RT(), Skipper>
{
    typedef RT ResultT;
    IntGrammar() : IntGrammar::base_type( integer ) {
        integer = qi::int_parser<RT>()[qi::_val = qi::_1] 
            > -qi::no_case[qi::char_('L')];
        integer.name("integer");
    }
    qi::rule<Iterator, RT(), Skipper> integer;
};
// ----------------------------------------------------------------------------
// Register IntGrammar:
// ----------------------------------------------------------------------------
template <typename Iterator, typename T, typename Skipper>
struct Grammar_<Iterator, T,  Skipper,
    typename boost::enable_if<boost::is_integral<T> >::type>
{
    typedef IntGrammar<Iterator, T, Skipper> Grammar;
};
//==============================================================================
template< typename Iterator, typename RT, typename Skipper>
struct RealGrammar : qi::grammar<Iterator, RT(), Skipper>
{
    typedef RT ResultT;
    RealGrammar() : RealGrammar::base_type(real) {
        real = qi::real_parser<RT>();
        real.name("float");
    }
    qi::rule<Iterator, RT(), Skipper> real;
};
// ----------------------------------------------------------------------------
// Register RealGrammar:
// ----------------------------------------------------------------------------
template <typename Iterator, typename T, typename Skipper >
struct Grammar_<Iterator, T, Skipper,
               typename boost::enable_if<boost::is_floating_point<T> >::type >
{
    typedef RealGrammar<Iterator, T, Skipper> Grammar;
};
//==============================================================================
template<typename T>
struct tuple_remove_first_type
{
};

template<typename T>
struct tuple_get_first_type
{
};

template<typename T, typename... Ts>
struct tuple_remove_first_type<std::tuple<T, Ts...>>
{
    typedef std::tuple<Ts...> type;
};

template<typename T, typename... Ts>
struct tuple_get_first_type<std::tuple<T, Ts...>>
{
    typedef T type;
};

//==============================================================================

template< typename Iterator, typename TupleT, std::size_t N, typename Skipper>
struct TupleInnerGrammar : qi::grammar<Iterator,
             TupleT(), qi::locals<typename tuple_get_first_type<TupleT>::type>, Skipper>
{
  //---------------------------------------------------------------------------
  typedef TupleT ResultT;
  typedef typename tuple_remove_first_type<TupleT>::type TailT;
  typedef typename tuple_get_first_type<TupleT>::type HeadT;
  //---------------------------------------------------------------------------
  struct Operations
    {
        template <typename A, typename B = boost::fusion::unused_type,
            typename C = boost::fusion::unused_type,
            typename D = boost::fusion::unused_type>
        struct result { typedef void type; };
        //----------------------------------------------------------------------

        void operator()(ResultT& res, HeadT& head, TailT& tail) const {
            res = std::tuple_cat(std::tuple<HeadT>(head), tail);
        }
        //----------------------------------------------------------------------
    };  
  //---------------------------------------------------------------------------
  TupleInnerGrammar(): TupleInnerGrammar::base_type(tup) {
    tup = grHead[qi::_a = qi::_1] > ',' > grLast[op(qi::_val, qi::_a, qi::_1)];
  }

  TupleInnerGrammar<Iterator, TailT, N-1, Skipper> grLast;
  typename
      Grammar_<Iterator, HeadT, Skipper>::Grammar grHead;

  qi::rule<Iterator, ResultT(), qi::locals<HeadT>, Skipper> tup;
  ph::function<Operations> op;
};

template< typename Iterator, typename TupleT, typename Skipper>
struct TupleInnerGrammar<Iterator, TupleT, 1, Skipper>: qi::grammar<Iterator,
             TupleT(), Skipper>
{
  //---------------------------------------------------------------------------
  typedef TupleT ResultT;
  //typedef typename ResultT::value_type Tuple1T;
  //---------------------------------------------------------------------------
  struct Operations
    {
        template <typename A, typename B = boost::fusion::unused_type,
            typename C = boost::fusion::unused_type,
            typename D = boost::fusion::unused_type>
        struct result { typedef void type; };
        //---------------------------------------------------------------------
        void operator()(ResultT& res,
            const typename std::tuple_element<0, ResultT>::type& val) const {
            res = ResultT();
            std::get<0>(res) = val;
        }
        //----------------------------------------------------------------------
    };  
  //---------------------------------------------------------------------------
  TupleInnerGrammar(): TupleInnerGrammar::base_type(tup) {
    tup = grFirst[op(qi::_val, qi::_1)];
  }

  typename
      Grammar_<Iterator, typename std::tuple_element<0, ResultT>::type,
        Skipper>::Grammar grFirst;

  qi::rule<Iterator, ResultT(), Skipper> tup;
  ph::function<Operations> op;
};

//=============================================================================
template< typename Iterator, typename TupleT, std::size_t N, typename Skipper>
struct TupleGrammar : qi::grammar<Iterator, TupleT(), qi::locals<char>, Skipper>
{
  typedef TupleT ResultT;
  TupleGrammar(): TupleGrammar::base_type(tup) {
    begin = enc::char_('[')[qi::_val=']'] | enc::char_('(')[qi::_val=')'];
    end = enc::char_(qi::_r1);

    tup = begin[qi::_a = qi::_1] > grTuple[qi::_val = qi::_1] > end(qi::_a);
  }

  qi::rule<Iterator, char()> begin;
  qi::rule<Iterator, void(char)> end;
  qi::rule<Iterator, ResultT(), qi::locals<char>, Skipper> tup;
  TupleInnerGrammar<Iterator, TupleT, N, Skipper> grTuple;
};

// -----------------------------------------------------------------------------
// Register TupleGrammar for std::tuple:
// ----------------------------------------------------------------------------
template <typename Iterator, typename Skipper, class... Args>
struct Grammar_<Iterator, std::tuple<Args...>, Skipper>
{
    typedef
     TupleGrammar<Iterator, std::tuple<Args...>, sizeof...(Args  ),
        Skipper> Grammar;
};
//==============================================================================
template< typename Iterator, typename VectorT, typename Skipper>
struct VectorGrammar : qi::grammar<Iterator,
             VectorT(), qi::locals<char>,Skipper>
{
//------------------------------------------------------------------------------
    typedef VectorT ResultT;
//------------------------------------------------------------------------------
    VectorGrammar() : VectorGrammar::base_type(vec) {
      begin = enc::char_('[')[qi::_val=']'] | enc::char_('(')[qi::_val=')'];
      end = enc::char_(qi::_r1);
      list = elementGrammar % ',';
      vec = begin[qi::_a = qi::_1] > -list[qi::_val=qi::_1] > end(qi::_a);
      
      begin.name("[ or (");
      end.name("] or )");
      list.name("comma_list");
      vec.name("list");
    }
// ----------------------------------------------------------------------------
    typename
      Grammar_<Iterator, typename VectorT::value_type, Skipper>::Grammar
                                                                 elementGrammar;
    qi::rule<Iterator, char()> begin;
    qi::rule<Iterator, void(char)> end;

    qi::rule<Iterator, ResultT(), qi::locals<char>,Skipper> vec;
    qi::rule<Iterator, ResultT(), Skipper> list;
// -----------------------------------------------------------------------------
};
// -----------------------------------------------------------------------------
// Register VectorGrammar for std::vector:
// ----------------------------------------------------------------------------
template <typename Iterator, typename InnerT, typename AllocatorT,
  typename Skipper>
struct Grammar_<Iterator, std::vector<InnerT, AllocatorT>, Skipper >
{
    typedef
     VectorGrammar<Iterator, std::vector<InnerT, AllocatorT>,Skipper>
                                      Grammar;
};
// ----------------------------------------------------------------------------
// Register VectorGrammar for std::list:
// ----------------------------------------------------------------------------
template <typename Iterator, typename InnerT, typename AllocatorT,
  typename Skipper>
struct Grammar_<Iterator, std::list<InnerT, AllocatorT>, Skipper >
{
    typedef
     VectorGrammar<Iterator, std::list<InnerT, AllocatorT>,Skipper>
                                      Grammar;
};
// ----------------------------------------------------------------------------
// Register VectorGrammar for std::set:
// ----------------------------------------------------------------------------
template <typename Iterator, typename InnerT, typename CompareT,
  typename AllocatorT, typename Skipper>
struct Grammar_<Iterator, std::set<InnerT, CompareT, AllocatorT>, Skipper >
{
    typedef
     VectorGrammar<Iterator, std::set<InnerT, CompareT, AllocatorT>,Skipper>
                                      Grammar;
};

//==============================================================================
template< typename Iterator, typename PairT, typename Skipper>
struct PairGrammar :
  qi::grammar<Iterator,PairT(), qi::locals<char>, Skipper> {
//------------------------------------------------------------------------------
    typedef PairT ResultT;
    typedef typename PairT::first_type first_type;
    typedef typename PairT::second_type second_type;
//------------------------------------------------------------------------------
    PairGrammar() : PairGrammar::base_type(pair) {
        init(",");
    }

    PairGrammar(const std::string& delimeter) : PairGrammar::base_type(pair) {
        init(delimeter);
    }
//------------------------------------------------------------------------------
    struct first {};
    struct second {};
    void init(const std::string& delimeter) {
      begin = enc::char_('(')[qi::_val=')']
              |
              enc::char_('[')[qi::_val=']'];
      end =  qi::char_(qi::_r1);
      pair = begin[qi::_a = qi::_1] > pair_in[qi::_val = qi::_1] > end(qi::_a);
      pair_in =  key > qi::lit(delimeter) > value;

      begin.name("( or [");
      end.name("] or )");
      pair.name("pair");
      pair_in.name("pair");
      key.name("key");
      value.name("value");
    }
// ----------------------------------------------------------------------------
    typename
       Grammar_<Iterator, typename PairT::first_type, Skipper>::Grammar key;
    typename
       Grammar_<Iterator, typename PairT::second_type, Skipper>::Grammar
         value;
    qi::rule<Iterator, char()> begin;
    qi::rule<Iterator, void(char)> end;
    qi::rule<Iterator, ResultT(), qi::locals<char>, Skipper> pair;
    qi::rule<Iterator, ResultT(), Skipper> pair_in;
    //ph::function<Operations> op;
// ----------------------------------------------------------------------------
}; // END PairGrammar
// ----------------------------------------------------------------------------
// Register PairGrammar:
// ----------------------------------------------------------------------------
template <typename Iterator, typename KeyT, typename ValueT,
  typename Skipper>
struct Grammar_<Iterator, std::pair<KeyT, ValueT>, Skipper >
{
    typedef PairGrammar<Iterator, std::pair<KeyT, ValueT>, Skipper> Grammar;
};
// ============================================================================
template< typename Iterator, typename MapT, typename Skipper>
struct MapGrammar : qi::grammar<Iterator,MapT(), Skipper>
{
//------------------------------------------------------------------------------
    typedef MapT ResultT;
    typedef typename MapT::key_type KeyT;
    typedef typename MapT::mapped_type MappedT;
    typedef std::pair<KeyT, MappedT> PairT;

    typedef std::vector<PairT> VectorPairT;
//------------------------------------------------------------------------------
    struct tag_key{};
    struct tag_mapped{};
    struct Operations
    {
        template <typename A, typename B = boost::fusion::unused_type,
            typename C = boost::fusion::unused_type,
            typename D = boost::fusion::unused_type>
        struct result { typedef void type; };
        //----------------------------------------------------------------------
        void operator()(ResultT& res, const VectorPairT& vec) const{
            for(typename VectorPairT::const_iterator cur = vec.begin();
                cur != vec.end(); cur++){
                res.insert(*cur);
            }
        }
        void operator()(PairT& res, const KeyT& key, tag_key) const{
          res.first = key;
        }
        void operator()(PairT& res, const MappedT& value, tag_mapped) const{
          res.second = value;
        }
        //----------------------------------------------------------------------
    };
//------------------------------------------------------------------------------
    MapGrammar() : MapGrammar::base_type(map) {
        pair = key[op(qi::_val,qi::_1, tag_key())] > (qi::lit(':') | '=')  >
        value[op(qi::_val,qi::_1, tag_mapped())];
        list = -(pair % enc::char_(','));
        map = ('{'  > list > '}')[op(qi::_val,qi::_1)];

        map.name("map");
        key.name("key");
        value.name("value");
        list.name("list");
        pair.name("pair");

    }
// ----------------------------------------------------------------------------
    typename
      Grammar_<Iterator, typename MapT::key_type, Skipper>::Grammar key;
    typename
      Grammar_<Iterator, typename MapT::mapped_type, Skipper>::Grammar value;
    qi::rule<Iterator, PairT(), Skipper> pair;
    qi::rule<Iterator, VectorPairT(), Skipper> list;
    qi::rule<Iterator, ResultT(), Skipper> map;
    ph::function<Operations> op;
// ----------------------------------------------------------------------------
};
// ----------------------------------------------------------------------------
// Register MapGrammar for std::map:
// ----------------------------------------------------------------------------
template <typename Iterator, typename KeyT, typename ValueT,
    typename KeyCompareT, typename AllocatorT, typename Skipper>
struct Grammar_<Iterator, std::map<KeyT, ValueT, KeyCompareT, AllocatorT>,
  Skipper > {
    typedef MapGrammar<Iterator,
        std::map<KeyT, ValueT, KeyCompareT, AllocatorT>, Skipper> Grammar;
};
// ============================================================================
} //   parsim
//============================================================================
#endif

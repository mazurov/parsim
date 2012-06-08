// ============================================================================
#ifndef INCLUDE_PARSIM_FACTORY_H
#define INCLUDE_PARSIM_FACTORY_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <string>
#include <vector>
#include <map>
// ============================================================================
// Boost:
// ============================================================================
#include <boost/mpl/assert.hpp>
/*
 * Copyright (C) 2012 the parsim contributors
 *
 * This file is part of parsim, distributed under the New BSD License.
 * For full terms see the included COPYING file.
 */

#include <boost/type_traits.hpp>
// ============================================================================
// Gaudi
// ============================================================================
#include "parsim/grammars.h"
// ============================================================================
namespace parsim {
    // ========================================================================
    typedef qi::expectation_failure<std::string::const_iterator> 
      expectation_failure;
    // ========================================================================
    template<typename Input, typename Result> inline bool
    parse_(const Input& input, Result& result){
      typedef typename Input::const_iterator Iterator;
      typedef SkipperGrammar<Iterator> Skipper;
      Skipper skipper;
      typename Grammar_<Iterator, Result, Skipper>::Grammar g;
      Iterator iter = input.begin(), end = input.end();
      return qi::phrase_parse(iter, end, g > qi::eoi, skipper , result);
    }
    
    template<typename Result> inline bool
      parse_(const char* input, Result& result) {
        return parse_(std::string(input), result);
      }



   
    //=========================================================================
    /*template<> inline bool
    parse_(const std::string& input, std::string& result){
      Skipper skipper;
      Grammar_<Iterator, std::string, Skipper>::Grammar g;
      Iterator iter = input.begin(), end = input.end();
      if (!qi::phrase_parse( iter, end, g > qi::eoi, skipper, result)){
        result = input;
      }
      //@attention always
      return true;
    }*/
    //=========================================================================
} /*  parsim */
//=============================================================================
#endif

/*
 * Copyright (C) 2012 the parsim contributors
 *
 * This file is part of parsim, distributed under the New BSD License.
 * For full terms see the included COPYING file.
 */

#ifndef PARSIM_INCLUDE_ERROR_FACTORY_H
#define PARSIM_INCLUDE_ERROR_FACTORY_H 1

#include <string>
#include <boost/spirit/include/qi_expect.hpp>

namespace parsim {
  struct parse_error {
    int pos;
    std::string tag;
    std::string found;
  };

  template<typename Iterator> inline void fill_error(
      const qi::expectation_failure<Iterator>& ex,
      parse_error *err) {
    if (err != NULL) {
      err->tag = ex.what_.tag;
      err->found = std::string(ex.first, ex.last);
      err->pos = 0;
    }
  }
}


#endif
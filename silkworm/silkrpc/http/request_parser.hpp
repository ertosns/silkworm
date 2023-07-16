/*
   Copyright 2023 The Silkworm Authors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
//
// Copyright (c) 2003-2020 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <tuple>

#include "request.hpp"

namespace silkworm::rpc::http {

//! Parser for incoming requests.
class RequestParser {
  public:
    //! Construct ready to parse the request method.
    RequestParser(){};

    //! Result of parse.
    enum class ResultType {
        good,
        bad,
        indeterminate,
        processing_continue
    };

    /**
     * Parse some data. The enum return value is good when a complete request has
     * been parsed, bad if the data is invalid, indeterminate when more data is
     * required. The InputIterator return value indicates how much of the input
     * has been consumed.
     */
    ResultType parse(Request& req, const char* begin, const char* end);
};

}  // namespace silkworm::rpc::http

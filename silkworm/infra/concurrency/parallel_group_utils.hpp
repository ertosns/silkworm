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

#pragma once

#include <array>
#include <exception>
#include <vector>

namespace silkworm::concurrency {

/**
 * Given 2 exceptions rethrows the one which happened first that was unexpected.
 *
 * "Unexpected" can be anything except boost::system::errc::operation_canceled.
 * If all exceptions are operation_canceled, throws one to propagate cancellation.
 * Does not throw with no exceptions (if all exception_ptr are null).
 */
void rethrow_first_exception_if_any(
    const std::array<std::exception_ptr, 2>& exceptions,
    const std::array<std::size_t, 2>& order);

/**
 * Given some exceptions rethrows the one which happened first that was unexpected.
 *
 * "Unexpected" can be anything except boost::system::errc::operation_canceled.
 * If all exceptions are operation_canceled, throws one to propagate cancellation.
 * Does not throw with no exceptions (if all exception_ptr are null).
 */
void rethrow_first_exception_if_any(
    const std::vector<std::exception_ptr>& exceptions,
    const std::vector<std::size_t>& order);

}  // namespace silkworm::concurrency

/*
   Copyright 2022 The Silkworm Authors

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

#include <catch2/catch_test_macros.hpp>

#include <silkworm/infra/concurrency/stoppable.hpp>

namespace silkworm {

TEST_CASE("Stoppable") {
    silkworm::Stoppable stoppable{};
    REQUIRE(stoppable.is_stopping() == false);
    REQUIRE(stoppable.stop() == true);
    REQUIRE(stoppable.stop() == false);
    REQUIRE(stoppable.is_stopping() == true);
}

}  // namespace silkworm

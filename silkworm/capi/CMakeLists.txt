#[[
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
]]

include("${SILKWORM_MAIN_DIR}/cmake/common/targets.cmake")

set(TARGET silkworm_capi)

find_package(Microsoft.GSL REQUIRED)

set(PUBLIC_LIBS "")
set(PRIVATE_LIBS
    glaze::glaze
    Microsoft.GSL::GSL
    silkworm_core
    silkworm_db
    silkworm_sentry
    silkworm_rpcdaemon
)

# Temporarily skip unit test runner for silkworm_capi target in ASAN build (failures after PR #1879)
if(SILKWORM_SANITIZE)
  set(NO_TEST "NO_TEST")
endif()

# cmake-format: off
silkworm_library(
  ${TARGET}
  PUBLIC ${PUBLIC_LIBS}
  PRIVATE ${PRIVATE_LIBS}
  TYPE SHARED
  ${NO_TEST}
)
# cmake-format: on

if(NOT NO_TEST)
  target_link_libraries(
    silkworm_capi_test
    PRIVATE silkworm_core
            silkworm_db
            silkworm_infra
            silkworm_rpcdaemon
            silkworm_node
            silkworm_db_test_util
  )
endif()

# Remove custom stack_size linker option for this target
get_target_property(LINK_OPTIONS ${TARGET} LINK_OPTIONS)
list(REMOVE_ITEM LINK_OPTIONS "-Wl,-stack_size")
list(REMOVE_ITEM LINK_OPTIONS "-Wl,${SILKWORM_STACK_SIZE}")
set_target_properties(${TARGET} PROPERTIES LINK_OPTIONS "${LINK_OPTIONS}")

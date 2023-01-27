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

#include "repository.hpp"

#include <catch2/catch.hpp>

#include <silkworm/common/directories.hpp>
#include <silkworm/common/log.hpp>
#include <silkworm/test/log.hpp>
#include <silkworm/test/snapshot_files.hpp>

namespace silkworm {

TEST_CASE("SnapshotRepository::SnapshotRepository", "[silkworm][snapshot][snapshot]") {
    CHECK_NOTHROW(SnapshotRepository{SnapshotSettings{}});
}

TEST_CASE("SnapshotRepository::reopen_folder", "[silkworm][snapshot][snapshot]") {
    test::SetLogVerbosityGuard guard{log::Level::kNone};

    const auto tmp_dir = TemporaryDirectory::get_unique_temporary_path();
    std::filesystem::create_directories(tmp_dir);
    test::TemporarySnapshotFile tmp_snapshot_1{tmp_dir, "v1-014500-015000-headers.seg"};
    test::TemporarySnapshotFile tmp_snapshot_2{tmp_dir, "v1-011500-012000-bodies.seg"};
    test::TemporarySnapshotFile tmp_snapshot_3{tmp_dir, "v1-015000-015500-transactions.seg"};
    SnapshotSettings settings{tmp_snapshot_1.path().parent_path()};
    SnapshotRepository repository{settings};
    CHECK_NOTHROW(repository.reopen_folder());
    CHECK(repository.header_snapshots_count() == 0);
    CHECK(repository.body_snapshots_count() == 0);
    CHECK(repository.tx_snapshots_count() == 0);
    CHECK(repository.max_block_available() == 0);
}

TEST_CASE("SnapshotRepository::view", "[silkworm][snapshot][snapshot]") {
    test::SetLogVerbosityGuard guard{log::Level::kNone};

    const auto tmp_dir = TemporaryDirectory::get_unique_temporary_path();
    std::filesystem::create_directories(tmp_dir);
    test::TemporarySnapshotFile tmp_snapshot_1{tmp_dir, "v1-014500-015000-headers.seg"};
    test::TemporarySnapshotFile tmp_snapshot_2{tmp_dir, "v1-011500-012000-bodies.seg"};
    test::TemporarySnapshotFile tmp_snapshot_3{tmp_dir, "v1-015000-015500-transactions.seg"};
    SnapshotSettings settings{tmp_snapshot_1.path().parent_path()};
    SnapshotRepository repository{settings};
    repository.reopen_folder();

    using ViewResult = SnapshotRepository::ViewResult;
    CHECK(repository.view_header_segment(14'500'000, [](const auto&) { return false; }) == ViewResult::kSnapshotNotFound);
    CHECK(repository.view_body_segment(11'500'000, [](const auto&) { return false; }) == ViewResult::kSnapshotNotFound);
    CHECK(repository.view_tx_segment(15'000'000, [](const auto&) { return false; }) == ViewResult::kSnapshotNotFound);
}

}  // namespace silkworm
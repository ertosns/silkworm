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

#include "sentry_options.hpp"

#include <climits>
#include <exception>
#include <filesystem>

#include <silkworm/core/common/util.hpp>
#include <silkworm/infra/common/log.hpp>

#include "common.hpp"
#include "ip_endpoint_option.hpp"

namespace silkworm::cmd::common {

void add_sentry_options(CLI::App& cli, silkworm::sentry::Settings& settings) {
    add_option_ip_endpoint(cli, "--sentry.api.addr", settings.api_address, "GRPC API endpoint");

    cli.add_option("--port", settings.port)
        ->description("Network listening port for incoming peers TCP connections and discovery UDP requests")
        ->check(CLI::Range(1024, 65535))
        ->capture_default_str();

    auto nat_option = cli.add_option("--nat", [&settings](const CLI::results_t& results) {
        return lexical_cast(results[0], settings.nat);
    });
    nat_option->description(
        "NAT port mapping mechanism (none|extip:<IP>)\n"
        "- none              no NAT, use a local IP as public\n"
        "- extip:1.2.3.4     use the given public IP");
    nat_option->default_str("none");

    auto node_key_path_option = cli.add_option("--nodekey", [&settings](const CLI::results_t& results) {
        try {
            settings.node_key = {{std::filesystem::path(results[0])}};
            return true;
        } catch (const std::exception& e) {
            log::Error() << e.what();
            return false;
        }
    });
    node_key_path_option->description("P2P node key file");

    auto node_key_hex_option = cli.add_option("--nodekeyhex", [&settings](const CLI::results_t& results) {
        auto key_bytes = from_hex(results[0]);
        if (key_bytes) {
            settings.node_key = {{key_bytes.value()}};
        }
        return key_bytes.has_value();
    });
    node_key_hex_option->description("P2P node key as a hex string");

    auto static_peers_option = cli.add_option("--staticpeers", [&settings](const CLI::results_t& results) {
        try {
            for (auto& result : results) {
                if (result.empty()) continue;
                settings.static_peers.emplace_back(result);
            }
        } catch (const std::exception& e) {
            log::Error() << e.what();
            return false;
        }
        return true;
    });
    static_peers_option->description("Peers enode URLs to connect to without discovery");
    static_peers_option->type_size(1, INT_MAX);

    cli.add_option("--maxpeers", settings.max_peers)
        ->description("Maximum number of P2P network peers")
        ->check(CLI::Range(0, 1000))
        ->capture_default_str();
}

}  // namespace silkworm::cmd::common

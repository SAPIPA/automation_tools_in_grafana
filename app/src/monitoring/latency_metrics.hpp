#pragma once

#include <monitoring/handler_registry.hpp>

#include <memory>
#include <string_view>
#include <vector>

#include <userver/components/component_base.hpp>
#include <userver/components/statistics_storage.hpp>
#include <userver/server/middlewares/http_middleware_base.hpp>
#include <userver/utils/statistics/histogram.hpp>
#include <userver/utils/statistics/writer.hpp>

namespace app {

class LatencyMetrics final
    : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "latency-metrics";

    LatencyMetrics(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    void Record(std::string_view handler, double seconds) noexcept;

private:
    struct EndpointMetrics {
        std::string_view handler;
        std::string_view method;
        std::string_view path;

        userver::utils::statistics::Histogram duration{{
            0.001, 0.0025, 0.005, 0.01,
            0.025, 0.05, 0.1, 0.25,
            0.5, 1.0, 2.5, 5.0
        }};
    };

    void Write(
        userver::utils::statistics::Writer& writer
    ) const;

    std::vector<EndpointMetrics> endpoints_;
};

class LatencyMiddlewareFactory final
    : public userver::server::middlewares::HttpMiddlewareFactoryBase {
public:
    static constexpr std::string_view kName = "latency-middleware";

    LatencyMiddlewareFactory(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

private:
    std::unique_ptr<userver::server::middlewares::HttpMiddlewareBase>
    Create(
        const userver::server::handlers::HttpHandlerBase& handler,
        userver::yaml_config::YamlConfig config
    ) const override;

    LatencyMetrics& metrics_;
};

}  // namespace app
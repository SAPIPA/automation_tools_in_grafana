#include <monitoring/latency_metrics.hpp>

#include <chrono>
#include <string>
#include <utility>

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/utils/scope_guard.hpp>
#include <userver/components/component_context.hpp>

namespace app {

LatencyMetrics::LatencyMetrics(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : ComponentBase(config, context) {

    endpoints_.reserve(kHandlers.size());

    for (const auto& handler : kHandlers) {
        endpoints_.push_back({
            handler.name,
            handler.method,
            handler.path
        });
    }

    auto& storage = context
        .FindComponent<userver::components::StatisticsStorage>()
        .GetStorage();

    storage.RegisterWriter(
        context.Scopes(),
        "http_request",
        [this](userver::utils::statistics::Writer& writer) {
            Write(writer);
        }
    );
}

void LatencyMetrics::Record(
    std::string_view handler,
    double seconds
) noexcept {
    for (auto& endpoint : endpoints_) {
        if (endpoint.handler == handler) {
            endpoint.duration.Account(seconds);
            return;
        }
    }
}

void LatencyMetrics::Write(
    userver::utils::statistics::Writer& writer
) const {
    for (const auto& endpoint : endpoints_) {
        writer["duration_seconds"].ValueWithLabels(
            endpoint.duration,
            {
                {"method", endpoint.method},
                {"route", endpoint.path}
            }
        );
    }
}

// Middleware runs around the actual HTTP handler.
class LatencyMiddleware final
    : public userver::server::middlewares::HttpMiddlewareBase {
public:
    LatencyMiddleware(
        LatencyMetrics& metrics,
        std::string handler
    )
        : metrics_(metrics),
          handler_(std::move(handler)) {}

private:
    void HandleRequest(
        userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context
    ) const override {
        const auto start = std::chrono::steady_clock::now();

        const userver::utils::ScopeGuard measure{
            [this, start] {
                const auto elapsed =
                    std::chrono::duration<double>(
                        std::chrono::steady_clock::now() - start
                    ).count();

                metrics_.Record(handler_, elapsed);
            }
        };

        Next(request, context);
    }

    LatencyMetrics& metrics_;
    std::string handler_;
};

LatencyMiddlewareFactory::LatencyMiddlewareFactory(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : HttpMiddlewareFactoryBase(config, context),
      metrics_(context.FindComponent<LatencyMetrics>()) {}

std::unique_ptr<userver::server::middlewares::HttpMiddlewareBase>
LatencyMiddlewareFactory::Create(
    const userver::server::handlers::HttpHandlerBase& handler,
    userver::yaml_config::YamlConfig
) const {
    return std::make_unique<LatencyMiddleware>(
        metrics_,
        handler.HandlerName()
    );
}

}  // namespace app
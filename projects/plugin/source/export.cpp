#include <exception>
#include <memory>
#include <stdexcept>
#include <string>

#include "detail/lua/state.hpp"

#include "market/market.hpp"

#include "../../shared/source/logger/logger.hpp"

namespace solution
{
	namespace plugin
	{
		using Logger = shared::Logger;

		static std::unique_ptr < Market > market;

		int main(detail::lua::State::state_t raw_state)
		{
			RUN_LOGGER(logger);

			try
			{
                detail::lua::State state(raw_state);

				market = std::make_unique < Market > (state);

				market->run();

                return EXIT_SUCCESS;
			}
			catch (const std::exception & exception)
			{
				logger.write(Logger::Severity::fatal, exception.what());

                return EXIT_FAILURE;
			}
			catch (...)
			{
				logger.write(Logger::Severity::fatal, "unknown exception");

                return EXIT_FAILURE;
			}
		}

		int stop(detail::lua::State::state_t raw_state)
		{
			market.reset();

			return EXIT_SUCCESS;
		}

	} // namespace plugin

} // namespace solution

extern "C" 
{
	LUALIB_API int luaopen_plugin(solution::plugin::detail::lua::State::state_t raw_state)
	{
		solution::plugin::detail::lua::State state(raw_state);

        state.register_function("main",   solution::plugin::main);
		state.register_function("OnStop", solution::plugin::stop);

		state.set_global("plugin");

		return EXIT_SUCCESS;
	}
}
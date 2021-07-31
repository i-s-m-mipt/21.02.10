#include "cci.hpp"

namespace solution
{
	namespace system
	{
		namespace market
		{
			namespace oscillators
			{
				void CCI::initialize() const
				{
					RUN_LOGGER(logger);

					try
					{
						if (m_timesteps == 0U)
						{
							throw std::domain_error("invalid timesteps value");
						}
					}
					catch (const std::exception & exception)
					{
						shared::catch_handler < oscillator_exception > (logger, exception);
					}
				}

				void CCI::operator()(candles_container_t & candles) const
				{
					RUN_LOGGER(logger);

					try
					{
						std::vector < double > typical_prices(std::size(candles), 0.0);

						std::transform(std::begin(candles), std::end(candles), std::begin(typical_prices), 
							[](const auto & candle) 
							{ 
								return ((candle.price_high + candle.price_low + candle.price_close) / 3.0); 
							});

						std::vector < double > sma;

						sma.reserve(std::size(typical_prices) - m_timesteps + 1U);

						sma.push_back(std::accumulate(std::begin(typical_prices),
							std::next(std::begin(typical_prices), m_timesteps), 0.0) / m_timesteps);

						for (auto i = m_timesteps; i < std::size(typical_prices); ++i)
						{
							sma.push_back(sma.back() + (typical_prices[i] - 
								typical_prices[i - m_timesteps]) / m_timesteps);
						}

						for (std::size_t i = m_timesteps - 1U, j = 0U; i < std::size(typical_prices); ++i, ++j)
						{
							auto mad = std::transform_reduce(std::next(std::begin(typical_prices), j),
								std::next(std::begin(typical_prices), j + m_timesteps), 0.0, std::plus <> (),
									[&sma, j](auto typical_price) 
									{ 
										return (std::abs(typical_price - sma[j])); 
									}) / m_timesteps;

							candles[i].oscillators.push_back((typical_prices[i] - sma[j]) / (0.015 * mad));
						}
					}
					catch (const std::exception & exception)
					{
						shared::catch_handler < oscillator_exception > (logger, exception);
					}
				}

			} // namespace oscillators

		} // namespace market

	} // namespace system

} // namespace solution
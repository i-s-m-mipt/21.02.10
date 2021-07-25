#include "market.hpp"

namespace solution
{
	namespace system
	{
		using Severity = shared::Logger::Severity;

		std::time_t Market::Date_Time::to_time_t() const
		{
			RUN_LOGGER(logger);

			try
			{
				std::tm time = { 
					static_cast < int > (second), 
					static_cast < int > (minute), 
					static_cast < int > (hour), 
					static_cast < int > (day), 
					static_cast < int > (month) - 1, 
					static_cast < int > (year) - 1900 };

				return std::mktime(&time);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		bool operator== (const Market::Date_Time & lhs, const Market::Date_Time & rhs)
		{
			return (
				(lhs.year   == rhs.year  ) &&
				(lhs.month  == rhs.month ) &&
				(lhs.day    == rhs.day   ) &&
				(lhs.hour   == rhs.hour  ) &&
				(lhs.minute == rhs.minute) &&
				(lhs.second == rhs.second));
		}

		bool operator!= (const Market::Date_Time & lhs, const Market::Date_Time & rhs)
		{
			return !(lhs == rhs);
		}

		bool operator< (const Market::Date_Time & lhs, const Market::Date_Time & rhs)
		{
			return (lhs.to_time_t() < rhs.to_time_t());
		}

		bool operator<= (const Market::Date_Time & lhs, const Market::Date_Time & rhs)
		{
			return !(lhs > rhs);
		}

		bool operator> (const Market::Date_Time & lhs, const Market::Date_Time & rhs)
		{
			return (lhs.to_time_t() > rhs.to_time_t());
		}

		bool operator>= (const Market::Date_Time & lhs, const Market::Date_Time & rhs)
		{
			return !(lhs < rhs);
		}

		std::ostream & operator<< (std::ostream & stream, const Market::Level & level)
		{
			static const char delimeter = ',';

			stream <<
				level.begin.year  << delimeter << std::setfill('0') << std::setw(2) <<
				level.begin.month << delimeter << std::setfill('0') << std::setw(2) <<
				level.begin.day   << delimeter;

			stream << std::setprecision(6) << std::fixed << level.price << delimeter << level.strength;

			return stream;
		}

		void Market::Candle::update_date_time() noexcept
		{
			RUN_LOGGER(logger);

			try
			{
				date_time.year   = (raw_date / 100U) / 100U;
				date_time.month  = (raw_date / 100U) % 100U;
				date_time.day    = (raw_date % 100U);

				date_time.hour   = (raw_time / 100U) / 100U;
				date_time.minute = (raw_time / 100U) % 100U;
				date_time.second = (raw_time % 100U);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::Data::load_assets(assets_container_t & assets)
		{
			RUN_LOGGER(logger);

			try
			{
				auto path = File::assets_data;

				std::fstream fin(path.string(), std::ios::in);

				if (!fin)
				{
					throw market_exception("cannot open file " + path.string());
				}

				std::string asset;

				while (std::getline(fin, asset))
				{
					assets.push_back(asset);
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::Data::load_scales(scales_container_t & scales)
		{
			RUN_LOGGER(logger);

			try
			{
				auto path = File::scales_data;

				std::fstream fin(path.string(), std::ios::in);

				if (!fin)
				{
					throw market_exception("cannot open file " + path.string());
				}

				std::string scale;

				while (std::getline(fin, scale))
				{
					scales.push_back(scale);
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::Data::save_self_similarities(const self_similarities_container_t & self_similarities)
		{
			RUN_LOGGER(logger);

			try
			{
				auto path = File::self_similarities_data;

				std::fstream fout(path.string(), std::ios::out | std::ios::trunc);

				if (!fout)
				{
					throw market_exception("cannot open file " + path.string());
				}

				std::ostringstream sout;

				for (const auto & [asset, matrix] : self_similarities)
				{
					auto size = matrix.size();

					sout << asset << " " << size << "\n\n";

					for (auto i = 0U; i < size; ++i)
					{
						for (auto j = 0U; j < size; ++j)
						{
							sout << std::setw(1 + 1 + 6) << std::right << std::setprecision(6) << std::fixed << matrix[i][j] << " ";
						}

						sout << "\n";
					}

					sout << "\n";
				}

				fout << sout.str();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::Data::save_pair_similarities(const pair_similarities_container_t & pair_similarities)
		{
			RUN_LOGGER(logger);

			try
			{
				auto path = File::pair_similarities_data;

				std::fstream fout(path.string(), std::ios::out | std::ios::trunc);

				if (!fout)
				{
					throw market_exception("cannot open file " + path.string());
				}

				std::ostringstream sout;

				for (const auto & [scale, matrix] : pair_similarities)
				{
					auto size = matrix.size();

					sout << scale << " " << size << "\n\n";

					for (auto i = 0U; i < size; ++i)
					{
						for (auto j = 0U; j < size; ++j)
						{
							sout << std::setw(1 + 1 + 6) << std::right << std::setprecision(6) << std::fixed << matrix[i][j] << " ";
						}

						sout << "\n";
					}

					sout << "\n";
				}

				fout << sout.str();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::Data::save_pair_correlations(const pair_correlations_container_t & pair_correlations)
		{
			RUN_LOGGER(logger);

			try
			{
				auto path = File::pair_correlations_data;

				std::fstream fout(path.string(), std::ios::out | std::ios::trunc);

				if (!fout)
				{
					throw market_exception("cannot open file " + path.string());
				}

				std::ostringstream sout;

				for (const auto & [scale, matrix] : pair_correlations)
				{
					auto size = matrix.size();

					sout << scale << " " << size << "\n\n";

					for (auto i = 0U; i < size; ++i)
					{
						for (auto j = 0U; j < size; ++j)
						{
							sout << std::setw(2 + 1 + 6) << std::right << std::setprecision(6) << 
								std::fixed << std::showpos << matrix[i][j] << " ";
						}

						sout << "\n";
					}

					sout << "\n";
				}

				fout << sout.str();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::Data::save_cumulative_distances(const distances_matrix_t & matrix)
		{
			RUN_LOGGER(logger);

			try
			{
				auto path = File::cumulative_distances_data;

				std::fstream fout(path.string(), std::ios::out | std::ios::trunc);

				if (!fout)
				{
					throw market_exception("cannot open file " + path.string());
				}

				std::ostringstream sout;

				auto size_1 = matrix.size();

				auto size_2 = matrix.begin()->size();

				sout << size_1 << " " << size_2 << "\n\n";

				for (auto i = 0U; i < size_1; ++i)
				{
					for (auto j = 0U; j < size_2; ++j)
					{
						sout << std::setw(3 + 1 + 3) << std::right << std::setprecision(3) << std::fixed << matrix[i][j] << " ";
					}

					sout << "\n";
				}

				fout << sout.str();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::Data::save_price_deviations(const charts_container_t & charts)
		{
			RUN_LOGGER(logger);

			try
			{
				auto path = File::price_deviations_data;

				std::fstream fout(path.string(), std::ios::out | std::ios::trunc);

				if (!fout)
				{
					throw market_exception("cannot open file " + path.string());
				}

				std::ostringstream sout;

				for (const auto & [asset, scales] : charts)
				{
					for (const auto & [scale, candles] : scales)
					{
						sout << asset << " " << scale << " " << std::size(candles) << "\n";

						std::for_each(std::begin(candles), std::end(candles), [&sout](const auto & candle)
							{ 
								static const char delimeter = ',';

								sout <<
									candle.date_time.year   << delimeter << std::setfill('0') << std::setw(2) <<
									candle.date_time.month  << delimeter << std::setfill('0') << std::setw(2) <<
									candle.date_time.day    << delimeter;

								sout << std::setprecision(6) << std::fixed << std::showpos <<
									candle.price_deviation << "\n";
							});

						sout << "\n";
					}
				}

				fout << sout.str();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		unsigned int day_of_week(const Market::Candle & candle)
		{
			RUN_LOGGER(logger);

			try
			{
				std::tm tm = { 0, 0, 0,
					static_cast < int > (candle.date_time.day),
					static_cast < int > (candle.date_time.month) - 1,
					static_cast < int > (candle.date_time.year)  - 1900 };

				auto time = std::mktime(&tm);

				return static_cast < unsigned int > (std::localtime(&time)->tm_wday) - 1U;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::Data::save_tagged_charts(const charts_container_t & charts, const Config & config) // TODO
		{
			RUN_LOGGER(logger);

			try
			{
				auto path = File::tagged_charts_data;

				std::fstream fout(path.string(), std::ios::out | std::ios::trunc);

				if (!fout)
				{
					throw market_exception("cannot open file " + path.string());
				}

				std::ostringstream sout;

				static const char delimeter = ',';

				for (const auto & [asset, scales] : charts)
				{
					for (const auto & [scale, candles] : scales)
					{
						sout << asset << " " << scale << " " << std::size(candles) << "\n";

						const auto price_deviation_multiplier = Market::get_price_deviation_multiplier(scale);

						std::for_each(std::begin(candles), std::end(candles), 
							[&sout, &config, price_deviation_multiplier](const auto & candle)
							{ 
								for (auto j = 1U; j < 13U; ++j)
								{
									if (j == candle.date_time.month)
									{
										sout << "1" << delimeter;
									}
									else
									{
										sout << "0" << delimeter;
									}
								}

								sout << std::setprecision(3) << std::fixed << std::noshowpos <<
									candle.date_time.day / days_in_month << delimeter;

								const auto day = day_of_week(candle);

								for (auto j = 0U; j < 5U; ++j)
								{
									if (j == day)
									{
										sout << "1" << delimeter;
									}
									else
									{
										sout << "0" << delimeter;
									}
								}


								if (candle.price_close < std::numeric_limits < double > ::epsilon())
								{
									throw std::domain_error("division by zero");
								}
						
								Level level;

								auto support_deviation = (candle.price_close - candle.support.price) / candle.price_close;

								if (support_deviation < config.level_max_deviation)
								{
									level = candle.support;
								}

								auto resistance_deviation = (candle.resistance.price - candle.price_close) / candle.price_close;

								if (resistance_deviation < config.level_max_deviation &&
									resistance_deviation < support_deviation &&
									candle.support.begin < candle.resistance.begin)
								{
									level = candle.resistance;
								}

								if (level.strength != 0U)
								{
									auto level_alive = (candle.date_time.to_time_t() - level.begin.to_time_t()) /
										seconds_in_day / config.level_max_lifetime;

									sout << std::setprecision(6) << std::fixed << std::noshowpos <<
										(level_alive > 1.0 ? 0.0 : level_alive) << delimeter;
								}
								else
								{
									sout << std::setprecision(6) << std::fixed << std::noshowpos << 0.0 << delimeter;
								}

								auto price_deviation_1 = candle.price_deviation_open * price_deviation_multiplier;
								auto price_deviation_2 = candle.price_deviation      * price_deviation_multiplier;
								auto price_deviation_3 = candle.price_deviation_max  * price_deviation_multiplier;
								auto price_deviation_4 = candle.price_deviation_min  * price_deviation_multiplier;

								sout << std::setprecision(6) << std::fixed << std::showpos <<
									(price_deviation_1 > 1.0 ? 1.0 : (price_deviation_1 < -1.0 ? -1.0 : price_deviation_1)) << delimeter;
								sout << std::setprecision(6) << std::fixed << std::showpos <<
									(price_deviation_2 > 1.0 ? 1.0 : (price_deviation_2 < -1.0 ? -1.0 : price_deviation_2)) << delimeter;
								sout << std::setprecision(6) << std::fixed << std::noshowpos <<
									(price_deviation_3 > 1.0 ? 1.0 : price_deviation_3) << delimeter;
								sout << std::setprecision(6) << std::fixed << std::noshowpos <<
									(price_deviation_4 > 1.0 ? 1.0 : price_deviation_4) << delimeter;
								
								for (auto regression_tag : candle.regression_tags)
								{
									sout << std::setprecision(6) << std::fixed << std::showpos <<
										regression_tag << delimeter;
								}
								
								sout << candle.classification_tag << "\n";
							});
					}
				}

				fout << sout.str();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::Data::save_environment(const charts_container_t & charts, const Config & config)
		{
			RUN_LOGGER(logger);

			try
			{
				auto path = File::environment_data;

				std::fstream fout(path.string(), std::ios::out | std::ios::trunc);

				if (!fout)
				{
					throw market_exception("cannot open file " + path.string());
				}

				const auto delta = config.prediction_timesteps - 1U;

				std::ostringstream sout;

				const auto volume_history_length = config.volume_timesteps;

				if (volume_history_length > config.prediction_timesteps)
				{
					throw std::runtime_error("volume history length is too long");
				}

				static const char delimeter = ',';

				for (const auto & [asset, scales] : charts)
				{
					for (const auto & [scale, candles] : scales)
					{
						const auto size = std::size(candles) - delta - 
							std::count_if(std::next(std::begin(candles), delta), std::end(candles),
								[](const auto & candle) { return (candle.movement_tag == 0); });

						sout << asset << " " << scale << " " << size << "\n";

						const auto price_deviation_multiplier  = Market::get_price_deviation_multiplier (scale);
						const auto volume_deviation_multiplier = Market::get_volume_deviation_multiplier(scale);

						for (auto i = delta; i < std::size(candles); ++i)
						{
							const auto & candle = candles[i];

							if (candle.movement_tag == 0)
							{
								continue;
							}

							/*
							for (auto j = 1U; j < 13U; ++j)
							{
								if (j == candle.date_time.month)
								{
									sout << "1" << delimeter;
								}
								else
								{
									sout << "0" << delimeter;
								}
							}

							sout << std::setprecision(3) << std::fixed << std::noshowpos << 
								candle.date_time.day / days_in_month << delimeter;
							*/

							const auto day = day_of_week(candle);

							for (auto j = 0U; j < 5U; ++j)
							{
								if (j == day)
								{
									sout << "1" << delimeter;
								}
								else
								{
									sout << "0" << delimeter;
								}
							}

							if (candle.price_close < std::numeric_limits < double > ::epsilon())
							{
								throw std::domain_error("division by zero");
							}

							Level level;

							auto support_deviation = (candle.price_close - candle.support.price) / candle.price_close;

							if (support_deviation < config.level_max_deviation)
							{
								level = candle.support;
							}

							auto resistance_deviation = (candle.resistance.price - candle.price_close) / candle.price_close;

							if (resistance_deviation < config.level_max_deviation && 
								resistance_deviation < support_deviation && 
								candle.support.begin < candle.resistance.begin)
							{
								level = candle.resistance;
							}

							if (level.strength != 0U)
							{
								auto level_alive = (candle.date_time.to_time_t() - level.begin.to_time_t()) /
									seconds_in_day / config.level_max_lifetime;

								sout << std::setprecision(6) << std::fixed << std::noshowpos <<
									(level_alive > 1.0 ? 0.0 : level_alive) << delimeter;
							}
							else
							{
								sout << std::setprecision(6) << std::fixed << std::noshowpos << 0.0 << delimeter;
							}

							for (auto j = 0U; j < volume_history_length; ++j)
							{
								auto volume_deviation = candles[i - volume_history_length + j + 1U].volume_deviation * 
									volume_deviation_multiplier;

								sout << std::setprecision(6) << std::fixed << std::showpos <<
									(volume_deviation > 1.0 ? 1.0 : (volume_deviation < -1.0 ? -1.0 : volume_deviation)) << delimeter;
							}

							for (auto j = 0U; j <= delta; ++j)
							{
								auto price_deviation_1 = candles[i - delta + j].price_deviation_open * price_deviation_multiplier;
								auto price_deviation_2 = candles[i - delta + j].price_deviation      * price_deviation_multiplier;

								auto price_deviation = price_deviation_1 + price_deviation_2;

								sout << std::setprecision(6) << std::fixed << std::showpos <<
									(price_deviation > 1.0 ? 1.0 : (price_deviation < -1.0 ? -1.0 : price_deviation)) << delimeter;
							}

							auto price_deviation_1 = candle.price_deviation_open * price_deviation_multiplier;
							auto price_deviation_2 = candle.price_deviation      * price_deviation_multiplier;
							auto price_deviation_3 = candle.price_deviation_max  * price_deviation_multiplier;
							auto price_deviation_4 = candle.price_deviation_min  * price_deviation_multiplier;

							sout << std::setprecision(6) << std::fixed << std::noshowpos <<
								(price_deviation_3 > 1.0 ? 1.0 : price_deviation_3) << delimeter;
							sout << std::setprecision(6) << std::fixed << std::noshowpos <<
								(price_deviation_4 > 1.0 ? 1.0 : price_deviation_4) << delimeter;
							sout << std::setprecision(6) << std::fixed << std::showpos <<
								(price_deviation_1 > 1.0 ? 1.0 : (price_deviation_1 < -1.0 ? -1.0 : price_deviation_1)) << delimeter;
							sout << std::setprecision(6) << std::fixed << std::showpos <<
								(price_deviation_2 > 1.0 ? 1.0 : (price_deviation_2 < -1.0 ? -1.0 : price_deviation_2)) << delimeter;
							
							for (auto regression_tag : candle.regression_tags)
							{
								sout << std::setprecision(6) << std::fixed << std::showpos <<
									regression_tag << delimeter;
							}

							sout << candle.classification_tag << delimeter;

							sout << std::showpos << candle.movement_tag << "\n";
						}
					}
				}

				fout << sout.str();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::Data::save_supports_resistances(const supports_resistances_container_t & supports_resistances)
		{
			RUN_LOGGER(logger);

			try
			{
				auto path = File::supports_resistances_data;

				std::fstream fout(path.string(), std::ios::out | std::ios::trunc);

				if (!fout)
				{
					throw market_exception("cannot open file " + path.string());
				}

				std::ostringstream sout;

				for (const auto & [asset, levels] : supports_resistances)
				{
					sout << asset << " " << std::size(levels) << "\n\n";

					for (const auto & level : levels)
					{
						sout << level << "\n";
					}

					sout << "\n";
				}

				fout << sout.str();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::Data::load(const path_t & path, json_t & object)
		{
			RUN_LOGGER(logger);

			try
			{
				std::fstream fin(path.string(), std::ios::in);

				if (!fin)
				{
					throw market_exception("cannot open file " + path.string());
				}

				object = json_t::parse(fin);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::Data::save(const path_t & path, const json_t & object)
		{
			RUN_LOGGER(logger);

			try
			{
				std::fstream fout(path.string(), std::ios::out);

				if (!fout)
				{
					throw market_exception("cannot open file " + path.string());
				}

				fout << std::setw(4) << object;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::initialize()
		{
			RUN_LOGGER(logger);

			try
			{
				std::filesystem::create_directory(charts_directory);
				std::filesystem::create_directory(levels_directory);
				std::filesystem::create_directory(output_directory);

				load();

				if (m_config.required_self_similarities)
				{
					handle_self_similarities();
				}

				if (m_config.required_pair_similarities)
				{
					handle_pair_similarities();
				}

				if (m_config.required_pair_correlations)
				{
					handle_pair_correlations();
				}

				if (m_config.required_price_deviations)
				{
					handle_price_deviations();
				}

				if (m_config.required_tagged_charts)
				{
					handle_tagged_charts();
				}

				if (m_config.required_environment)
				{
					handle_environment();
				}

				if (m_config.required_quik)
				{
					handle_quik_initialization();
				}

				if (m_config.run_fridays_test)
				{
					run_fridays_test();
				}

				if (m_config.run_mornings_test)
				{
					run_mornings_test();
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::uninitialize()
		{
			RUN_LOGGER(logger);

			try
			{
				m_thread_pool.join();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::load()
		{
			RUN_LOGGER(logger);

			try
			{
				load_assets();
				load_scales();

				if (!m_config.required_quik)
				{
					load_charts();
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::load_assets()
		{
			RUN_LOGGER(logger);

			try
			{
				Data::load_assets(m_assets);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::load_scales()
		{
			RUN_LOGGER(logger);

			try
			{
				Data::load_scales(m_scales);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::load_charts()
		{
			RUN_LOGGER(logger);

			try
			{
				if (m_config.required_charts)
				{
					get_all_charts();
				}

				std::vector < std::future < void > > futures;

				futures.reserve(std::size(m_assets) * std::size(m_scales));

				std::mutex mutex;

				for (const auto & asset : m_assets)
				{
					for (const auto & scale : m_scales)
					{
						auto path = charts_directory; path /= make_file_name(asset, scale);

						if (!std::filesystem::exists(path))
						{
							logger.write(Severity::error, "file " + path.string() + " doesn't exist");

							continue;
						}
						
						std::packaged_task < void() > task([this, path, &mutex, asset, scale, &logger]()
							{
								auto candles = load_candles(asset, scale, path);

								std::scoped_lock lock(mutex);

								if (std::size(candles) >= m_config.prediction_timesteps * 2U)
								{
									m_charts[asset][scale] = std::move(candles);
								}
								else
								{
									logger.write(Severity::empty, "candles size exception: " + asset + " " + scale);
								}
							});

						futures.push_back(boost::asio::post(m_thread_pool, std::move(task)));
					}
				}

				std::for_each(std::begin(futures), std::end(futures), [](auto & future) { future.wait(); });
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		std::pair < Market::path_t, std::size_t > Market::get_all_charts() const
		{
			RUN_LOGGER(logger);

			try
			{
				auto counter = 0U;

				for (const auto & asset : m_assets)
				{
					for (const auto & scale : m_scales)
					{
						get_chart(asset, scale);

						++counter;
					}
				}

				return std::make_pair(charts_directory, counter);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		Market::path_t Market::get_chart(const std::string & asset, const std::string & scale) const
		{
			RUN_LOGGER(logger);

			try
			{
				auto path = charts_directory; path /= make_file_name(asset, scale);

				shared::Python python;

				try
				{
					boost::python::exec("from market import get", python.global(), python.global());

					python.global()["get"](asset.c_str(), scale.c_str(), path.string().c_str());
				}
				catch (const boost::python::error_already_set &)
				{
					logger.write(Severity::error, shared::Python::exception());
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < market_exception > (logger, exception);
				}

				return path;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		std::string Market::make_file_name(const std::string & asset, const std::string & scale) const
		{
			RUN_LOGGER(logger);

			try
			{
				return (asset + "_" + scale + Extension::csv);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		Market::candles_container_t Market::load_candles(const std::string & asset,
			const std::string & scale, const path_t & path) const
		{
			RUN_LOGGER(logger);

			try
			{
				candles_container_t candles;

				std::fstream fin(path.string(), std::ios::in);

				if (!fin)
				{
					throw market_exception("cannot open file " + path.string());
				}

				std::string line;

				while (std::getline(fin, line))
				{
					candles.push_back(parse(line));
				}

				std::reverse(std::begin(candles), std::end(candles));

				update_deviations(asset, scale, candles);

				return candles;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		Market::Candle Market::parse(const std::string & line) const
		{
			RUN_LOGGER(logger);

			try
			{
				Candle_Parser < std::string::const_iterator > parser;

				auto first = std::begin(line);
				auto last  = std::end(line);

				Candle candle;

				auto result = boost::spirit::qi::phrase_parse(
					first, last, parser, boost::spirit::qi::blank, candle);

				if (result)
				{
					candle.update_date_time();

					return candle;
				}
				else
				{
					throw market_exception("cannot parse line " + line);
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::update_deviations(const std::string & asset,
			const std::string & scale, candles_container_t & candles) const
		{
			RUN_LOGGER(logger);

			try
			{
				bool flag = false;

				for (auto i = 0U; i < std::size(candles); ++i)
				{
					if (std::abs(candles[i].price_open) <= std::numeric_limits < double > ::epsilon())
					{
						throw std::domain_error("division by zero");
					}

					candles[i].price_deviation = (candles[i].price_close - candles[i].price_open) / candles[i].price_open;

					if ((candles[i].price_deviation >  m_config.critical_deviation ||
						 candles[i].price_deviation < -m_config.critical_deviation) && !flag)
					{
						std::ostringstream sout;

						sout << "price deviation exception: " << 
							std::setw(5) << std::left  << std::setfill(' ') << asset << " " << scale  << " " <<
							std::setw(4) << std::right << std::setfill('0') << std::noshowpos << candles[i].date_time.year  << "." <<
							std::setw(2) << std::right << std::setfill('0') << std::noshowpos << candles[i].date_time.month << "." <<
							std::setw(2) << std::right << std::setfill('0') << std::noshowpos << candles[i].date_time.day;

						logger.write(Severity::empty, sout.str());

						flag = true;
					}

					if (i == 0U)
					{
						candles[i].price_deviation_open = 0.0;

						candles[i].volume_deviation = 0.0;
					}
					else
					{
						if (std::abs(candles[i - 1].price_close) <= std::numeric_limits < double > ::epsilon())
						{
							throw std::domain_error("division by zero");
						}

						candles[i].price_deviation_open = (candles[i].price_open - candles[i - 1].price_close) / candles[i - 1].price_close;

						if ((candles[i].price_deviation_open >  m_config.critical_deviation ||
							 candles[i].price_deviation_open < -m_config.critical_deviation) && !flag)
						{
							std::ostringstream sout;

							sout << "price deviation exception: " << 
								std::setw(5) << std::left  << std::setfill(' ') << asset << " " << scale << " " <<
								std::setw(4) << std::right << std::setfill('0') << std::noshowpos << candles[i].date_time.year  << "." <<
								std::setw(2) << std::right << std::setfill('0') << std::noshowpos << candles[i].date_time.month << "." <<
								std::setw(2) << std::right << std::setfill('0') << std::noshowpos << candles[i].date_time.day;

							logger.write(Severity::empty, sout.str());

							flag = true;
						}

						if (candles[i - 1].volume == 0ULL)
						{
							candles[i - 1].volume = 1ULL;
						}

						const auto a = static_cast < double > (candles[i    ].volume);
						const auto b = static_cast < double > (candles[i - 1].volume);

						candles[i].volume_deviation = (a - b) / b;
					}

					candles[i].price_deviation_max = (candles[i].price_high - candles[i].price_open) / candles[i].price_open;
					candles[i].price_deviation_min = (candles[i].price_open - candles[i].price_low ) / candles[i].price_open;
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::handle_self_similarities()
		{
			RUN_LOGGER(logger);

			try
			{
				compute_self_similarities();

				save_self_similarities();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::handle_pair_similarities()
		{
			RUN_LOGGER(logger);

			try
			{
				compute_pair_similarities();

				save_pair_similarities();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::handle_pair_correlations()
		{
			RUN_LOGGER(logger);

			try
			{
				compute_pair_correlations();

				save_pair_correlations();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::handle_price_deviations()
		{
			RUN_LOGGER(logger);

			try
			{
				save_price_deviations();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::handle_tagged_charts()
		{
			RUN_LOGGER(logger);

			try
			{
				make_supports_resistances();

				save_supports_resistances();

				make_tagged_charts();

				save_tagged_charts();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::handle_environment()
		{
			RUN_LOGGER(logger);

			try
			{
				make_supports_resistances();

				save_supports_resistances();

				make_environment();

				save_environment();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::handle_quik_initialization()
		{
			RUN_LOGGER(logger);

			try
			{
				initialize_sources();

				update_supports_resistances();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::run_fridays_test() const
		{
			RUN_LOGGER(logger);

			try
			{
				const std::string scale = "D";

				for (const auto & asset : m_assets)
				{
					const auto & candles = m_charts.at(asset).at(scale);

					std::size_t counter = 0U;

					std::size_t counter_total = 0U;

					for (auto i = 0U; i < std::size(candles) - 1U; ++i)
					{
						if (day_of_week(candles[i]) == 3U && day_of_week(candles[i + 1]) == 4U)
						{
							if (candles[i].price_deviation * candles[i + 1].price_deviation > 0)
							{
								++counter;
							}

							++counter_total;
						}
					}

					std::cout <<
						std::setw(5) << std::setfill(' ') << std::left  << asset << " : " <<
						std::setw(3) << std::setfill(' ') << std::right << counter << " coincidences of " <<
						std::setw(3) << std::setfill(' ') << std::right << counter_total << std::endl;
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::run_mornings_test() const
		{
			RUN_LOGGER(logger);

			try
			{
				const auto asset = m_config.mornings_test_asset;
				const auto scale = m_config.mornings_test_scale;
				const auto delta = m_config.mornings_test_delta;

				if (scale != "M60")
				{
					logger.write(Severity::error, "invalid scale");
				}

				const auto & candles = m_charts.at(asset).at(scale);

				std::size_t counter            = 0U;
				std::size_t counter_delta_L    = 0U;
				std::size_t counter_delta_S    = 0U;
				std::size_t counter_delta_both = 0U;

				for (auto i = 1U; i < std::size(candles) - 1U; ++i)
				{
					if (candles[i].date_time.year != m_config.mornings_test_year || candles[i].date_time.hour != 10U)
					{
						continue;
					}

					++counter;

					auto previous_price = candles[i - 1].price_close;

					auto has_delta_L = (std::max(candles[i].price_high, candles[i + 1].price_high) - previous_price >  delta);
					auto has_delta_S = (std::min(candles[i].price_low,  candles[i + 1].price_low ) - previous_price < -delta);

					if (has_delta_L)
					{
						++counter_delta_L;
					}

					if (has_delta_S)
					{
						++counter_delta_S;
					}

					if (has_delta_L && has_delta_S)
					{
						++counter_delta_both;
					}
				}

				std::cout << "days        : " << counter            << std::endl;
				std::cout << "days (L)    : " << counter_delta_L    << std::endl;
				std::cout << "days (S)    : " << counter_delta_S    << std::endl;
				std::cout << "days (both) : " << counter_delta_both << std::endl;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::compute_self_similarities()
		{
			RUN_LOGGER(logger);

			try
			{
				const auto size = std::size(m_scales);

				std::vector < std::future < double > > futures;

				futures.reserve(std::size(m_assets) * size * (size - 1U) / 2U);

				for (const auto & asset : m_assets)
				{
					for (auto i = 0U; i < size; ++i)
					{
						for (auto j = i + 1; j < size; ++j)
						{
							std::packaged_task < double() > task([this, asset, i, j]()
								{ return compute_self_similarity(asset, m_scales[i], m_scales[j]); });

							futures.push_back(boost::asio::post(m_thread_pool, std::move(task)));
						}
					}
				}

				auto index = 0U;

				for (const auto & asset : m_assets)
				{
					m_self_similarities.insert(std::make_pair(asset, self_similarity_matrix_t(boost::extents[size][size])));

					for (auto i = 0U; i < size; ++i)
					{
						for (auto j = 0U; j < size; ++j)
						{
							if (i == j)
							{
								m_self_similarities[asset][i][j] = 0.0;
							}
							else
							{
								if (i > j)
								{
									m_self_similarities[asset][i][j] = m_self_similarities[asset][j][i];
								}
								else
								{
									m_self_similarities[asset][i][j] = futures[index++].get();
								}
							}
						}
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::compute_pair_similarities()
		{
			RUN_LOGGER(logger);

			try
			{
				const auto size = std::size(m_assets);

				std::vector < std::future < double > > futures;

				futures.reserve(std::size(m_scales) * size * (size - 1U) / 2U);

				for (const auto & scale : m_scales)
				{
					for (auto i = 0U; i < size; ++i)
					{
						for (auto j = i + 1; j < size; ++j)
						{
							std::packaged_task < double() > task([this, scale, i, j]()
								{ return compute_pair_similarity(scale, m_assets[i], m_assets[j]); });

							futures.push_back(boost::asio::post(m_thread_pool, std::move(task)));
						}
					}
				}

				auto index = 0U;

				for (const auto & scale : m_scales)
				{
					m_pair_similarities.insert(std::make_pair(scale, pair_similarity_matrix_t(boost::extents[size][size])));

					for (auto i = 0U; i < size; ++i)
					{
						for (auto j = 0U; j < size; ++j)
						{
							if (i == j)
							{
								m_pair_similarities[scale][i][j] = 0.0;
							}
							else
							{
								if (i > j)
								{
									m_pair_similarities[scale][i][j] = m_pair_similarities[scale][j][i];
								}
								else
								{
									m_pair_similarities[scale][i][j] = futures[index++].get();
								}
							}
						}
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::compute_pair_correlations()
		{
			RUN_LOGGER(logger);

			try
			{
				const auto size = std::size(m_assets);

				std::vector < std::future < double > > futures;

				futures.reserve(std::size(m_scales) * size * (size - 1U) / 2U);

				for (const auto & scale : m_scales)
				{
					for (auto i = 0U; i < size; ++i)
					{
						for (auto j = i + 1; j < size; ++j)
						{
							std::packaged_task < double() > task([this, scale, i, j]()
								{ return compute_pair_correlation(scale, m_assets[i], m_assets[j]); });

							futures.push_back(boost::asio::post(m_thread_pool, std::move(task)));
						}
					}
				}

				auto index = 0U;

				for (const auto & scale : m_scales)
				{
					m_pair_correlations.insert(std::make_pair(scale, pair_correlation_matrix_t(boost::extents[size][size])));

					for (auto i = 0U; i < size; ++i)
					{
						for (auto j = 0U; j < size; ++j)
						{
							if (i == j)
							{
								m_pair_correlations[scale][i][j] = 1.0;
							}
							else
							{
								if (i > j)
								{
									m_pair_correlations[scale][i][j] = m_pair_correlations[scale][j][i];
								}
								else
								{
									m_pair_correlations[scale][i][j] = futures[index++].get();
								}
							}
						}
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		template < typename T >
		auto min(T lhs, T rhs)
		{
			return std::min(lhs, rhs);
		}

		template < typename T, typename ... Types >
		auto min(T lhs, Types ... args)
		{
			return std::min(lhs, min(args...));
		}

		double Market::compute_self_similarity(const std::string & asset,
			const std::string & scale_1, const std::string & scale_2) const
		{
			RUN_LOGGER(logger);

			try
			{
				const auto & candles_1 = m_charts.at(asset).at(scale_1);
				const auto & candles_2 = m_charts.at(asset).at(scale_2);

				auto size_1 = std::size(candles_1);
				auto size_2 = std::size(candles_2);

				distances_matrix_t distances(boost::extents[size_1][size_2]);

				for (auto i = 0U; i < size_1; ++i)
				{
					for (auto j = 0U; j < size_2; ++j)
					{
						distances[i][j] = std::abs(candles_1[i].price_deviation - candles_2[j].price_deviation);
					}
				}

				distances_matrix_t cumulative_distances(boost::extents[size_1][size_2]);

				cumulative_distances[0][0] = distances[0][0];

				for (auto i = 1U; i < size_1; ++i)
				{
					cumulative_distances[i][0] = distances[i][0] + cumulative_distances[i - 1][0];
				}

				for (auto j = 1U; j < size_2; ++j)
				{
					cumulative_distances[0][j] = distances[0][j] + cumulative_distances[0][j - 1];
				}

				for (auto i = 1; i < size_1; ++i)
				{
					for (auto j = 1; j < size_2; ++j)
					{
						if (std::abs(i - j) < m_config.self_similarity_DTW_delta)
						{
							cumulative_distances[i][j] = distances[i][j] + min(cumulative_distances[i - 1][j - 1],
								cumulative_distances[i - 1][j], cumulative_distances[i][j - 1]);
						}
						else
						{
							cumulative_distances[i][j] = std::numeric_limits < double > ::infinity();
						}
					}
				}

				if ((asset == m_config.cumulative_distances_asset) && (
					(scale_1 == m_config.cumulative_distances_scale_1 && scale_2 == m_config.cumulative_distances_scale_2) ||
					(scale_2 == m_config.cumulative_distances_scale_1 && scale_1 == m_config.cumulative_distances_scale_2)))
				{
					save_cumulative_distances(cumulative_distances);
				}

				return cumulative_distances[size_1 - 1][size_2 - 1] / std::max(size_1, size_2);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		double Market::compute_pair_similarity(const std::string & scale,
			const std::string & asset_1, const std::string & asset_2) const
		{
			RUN_LOGGER(logger);

			try
			{
				const auto & candles_1 = m_charts.at(asset_1).at(scale);
				const auto & candles_2 = m_charts.at(asset_2).at(scale);

				auto size = std::min(std::size(candles_1), std::size(candles_2));

				std::vector < double > price_deviations_1(size, 0.0);
				std::vector < double > price_deviations_2(size, 0.0);

				std::transform(std::crbegin(candles_1), std::next(std::crbegin(candles_1), size),
					std::begin(price_deviations_1), [](const auto & candle) { return candle.price_deviation; });

				std::transform(std::crbegin(candles_2), std::next(std::crbegin(candles_2), size),
					std::begin(price_deviations_2), [](const auto & candle) { return candle.price_deviation; });

				std::sort(std::begin(price_deviations_1), std::end(price_deviations_1));
				std::sort(std::begin(price_deviations_2), std::end(price_deviations_2));

				return (std::transform_reduce(std::begin(price_deviations_1), std::end(price_deviations_1), std::begin(price_deviations_2),
					0.0, std::plus(), [](const auto lhs, const auto rhs) { return std::abs(lhs - rhs); }) / size);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		double Market::compute_pair_correlation(const std::string & scale,
			const std::string & asset_1, const std::string & asset_2) const
		{
			RUN_LOGGER(logger);

			try
			{
				const auto & candles_1 = m_charts.at(asset_1).at(scale);
				const auto & candles_2 = m_charts.at(asset_2).at(scale);

				auto size = std::min(std::size(candles_1), std::size(candles_2));

				std::vector < std::pair < double, int > > price_deviations_1(size);
				std::vector < std::pair < double, int > > price_deviations_2(size);

				auto index = 1;

				std::transform(std::crbegin(candles_1), std::next(std::crbegin(candles_1), size),
					std::begin(price_deviations_1), [&index](const auto & candle) { return std::make_pair(candle.price_deviation, index++); });

				index = 1;

				std::transform(std::crbegin(candles_2), std::next(std::crbegin(candles_2), size),
					std::begin(price_deviations_2), [&index](const auto & candle) { return std::make_pair(candle.price_deviation, index++); });

				std::sort(std::begin(price_deviations_1), std::end(price_deviations_1),
					[](const auto & lhs, const auto & rhs) {return lhs.first < rhs.first; });
				std::sort(std::begin(price_deviations_2), std::end(price_deviations_2),
					[](const auto & lhs, const auto & rhs) {return lhs.first < rhs.first; });

				return 1.0 - 6.0 * (std::transform_reduce(std::begin(price_deviations_1), std::end(price_deviations_1), std::begin(price_deviations_2),
					0.0, std::plus(), [](const auto lhs, const auto rhs) { return std::pow(lhs.second - rhs.second, 2); }) / (size * (size * size - 1)));
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::save_self_similarities() const
		{
			RUN_LOGGER(logger);

			try
			{
				Data::save_self_similarities(m_self_similarities);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::save_pair_similarities() const
		{
			RUN_LOGGER(logger);

			try
			{
				Data::save_pair_similarities(m_pair_similarities);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::save_pair_correlations() const
		{
			RUN_LOGGER(logger);

			try
			{
				Data::save_pair_correlations(m_pair_correlations);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::save_cumulative_distances(const distances_matrix_t & matrix) const
		{
			RUN_LOGGER(logger);

			try
			{
				Data::save_cumulative_distances(matrix);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::save_price_deviations() const
		{
			RUN_LOGGER(logger);

			try
			{
				Data::save_price_deviations(m_charts);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::make_supports_resistances()
		{
			RUN_LOGGER(logger);

			try
			{
				std::vector < std::future < void > > futures;

				futures.reserve(std::size(m_assets));

				std::mutex mutex;

				for (const auto & asset : m_assets)
				{
					std::packaged_task < void() > task([this, asset, &mutex]()
						{
							auto levels = make_levels(m_charts.at(asset).at(m_config.level_resolution));

							std::sort(std::begin(levels), std::end(levels), [](const auto & lhs, const auto & rhs)
								{ return (lhs.begin < rhs.begin); });
							
							std::scoped_lock lock(mutex);

							m_supports_resistances[asset] = std::move(levels);
						});

					futures.push_back(boost::asio::post(m_thread_pool, std::move(task)));
				}

				std::for_each(std::begin(futures), std::end(futures), [](auto & future) { future.wait(); });
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		Market::levels_container_t Market::make_levels(const candles_container_t & candles) const
		{
			RUN_LOGGER(logger);

			try
			{
				const auto frame = m_config.level_frame;

				levels_container_t levels;

				for (auto first = std::begin(candles); first != std::end(candles); )
				{
					auto last = std::next(first, std::min(frame,
						static_cast < decltype(frame) > (std::distance(first, std::end(candles)))));

					auto extremum = std::minmax_element(first, last, [](const auto & lhs, const auto & rhs)
						{
							return (lhs.price_close < rhs.price_close);
						});

					if ((extremum.first == first && first != std::begin(candles) &&
						std::prev(first)->price_close > extremum.first->price_close) ||
						(extremum.first == std::prev(last) && last != std::end(candles) &&
							last->price_close > extremum.first->price_close) ||
						(extremum.first != first && extremum.first != std::prev(last)))
					{
						levels.push_back(Level{ extremum.first->date_time, extremum.first->price_close, 1U });
					}

					if ((extremum.second == first && first != std::begin(candles) &&
						std::prev(first)->price_close < extremum.second->price_close) ||
						(extremum.second == std::prev(last) && last != std::end(candles) &&
							last->price_close < extremum.second->price_close) ||
						(extremum.second != first && extremum.second != std::prev(last)))
					{
						levels.push_back(Level{ extremum.second->date_time, extremum.second->price_close, 1U });
					}

					first = last;
				}

				if (m_config.required_level_reduction)
				{
					return reduce_levels(std::move(levels));
				}
				else
				{
					return levels;
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		Market::levels_container_t Market::reduce_levels(levels_container_t && levels) const
		{
			RUN_LOGGER(logger);

			try
			{
				if (std::size(levels) > 1)
				{
					for (auto first = std::begin(levels); first != std::end(levels); ++first)
					{
						for (auto current = std::next(first); current != std::end(levels);)
						{
							if (std::abs(first->price - current->price) / first->price <=
								m_config.level_max_deviation)
							{
								++first->strength;

								current = levels.erase(current);
							}
							else
							{
								++current;
							}
						}
					}
				}

				return levels;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::save_supports_resistances() const
		{
			RUN_LOGGER(logger);

			try
			{
				Data::save_supports_resistances(m_supports_resistances);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::make_tagged_charts()
		{
			RUN_LOGGER(logger);

			try
			{
				std::vector < std::future < void > > futures;

				futures.reserve(std::size(m_assets) * std::size(m_scales));

				for (const auto & asset : m_assets)
				{
					for (const auto & scale : m_scales)
					{
						std::packaged_task < void() > task([this, asset, scale]()
							{
								auto & candles = m_charts.at(asset).at(scale);

								update_regression_tags(candles);

								update_classification_tags(candles);

								update_movement_tags(asset, candles);

								auto & levels = m_supports_resistances.at(asset);

								update_supports_resistances(candles, levels);
							});

						futures.push_back(boost::asio::post(m_thread_pool, std::move(task)));
					}
				}

				std::for_each(std::begin(futures), std::end(futures), [](auto & future) { future.wait(); });
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::update_regression_tags(candles_container_t & candles) const
		{
			RUN_LOGGER(logger);

			try
			{
				for (auto i = 0U; i < std::size(candles); ++i)
				{
					for (auto j = 0U; j < Candle::prediction_range; ++j)
					{
						if (i + j + 1 < std::size(candles))
						{
							if (std::abs(candles[i].price_close) <= std::numeric_limits < double > ::epsilon())
							{
								throw std::domain_error("division by zero");
							}

							candles[i].regression_tags[j] = (candles[i + j + 1].price_close -
								candles[i].price_close) / candles[i].price_close;
						}
						else
						{
							candles[i].regression_tags[j] = 0.0;
						}
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::update_classification_tags(candles_container_t & candles) const
		{
			RUN_LOGGER(logger);

			try
			{
				const auto min_price_change   = m_config.min_price_change;
				const auto max_price_rollback = m_config.max_price_rollback;

				for (auto first = std::begin(candles); first != std::end(candles);)
				{
					auto flag = false;

					for (auto last = std::next(first); last != std::end(candles); ++last)
					{
						auto result = std::minmax_element(first, std::next(last), [](const auto & lhs, const auto & rhs)
							{
								return lhs.price_close < rhs.price_close;
							});

						auto min_price = result.first ->price_close;
						auto max_price = result.second->price_close;

						auto first_extremum = result.first;
						auto last_extremum  = result.second;

						if (std::distance(first, result.first) > std::distance(first, result.second))
						{
							first_extremum = result.second;
							last_extremum  = result.first;
						}

						if ((((max_price - min_price) / min_price < min_price_change) ||
							(result.first  == last) || 
							(result.second == last) || ((last_extremum != last) &&
								((max_price - min_price) / min_price > min_price_change) &&
								(std::abs(last_extremum->price_close - last->price_close) < max_price_rollback *
									std::abs(first_extremum->price_close - last_extremum->price_close)))) &&
							(std::abs(last_extremum->price_close - last->price_close) /
								std::min(last_extremum->price_close, last->price_close) < min_price_change))
						{
							continue;
						}

						if (first_extremum->price_close < last_extremum->price_close)
						{
							//sample_classification_tags(candles, first_extremum, "OL");
							//sample_classification_tags(candles, last_extremum,  "CL");

							first_extremum->classification_tag = "OL";
							last_extremum->classification_tag  = "CL";
						}
						else
						{
							//sample_classification_tags(candles, first_extremum, "OS");
							//sample_classification_tags(candles, last_extremum,  "CS");

							first_extremum->classification_tag = "OS";
							last_extremum->classification_tag  = "CS";
						}

						first = last_extremum;

						flag = true;

						break;
					}

					if (!flag)
					{
						break;
					}
				}

				std::string state_tag = State_Tag::C;

				for (auto & candle : candles)
				{
					if (candle.classification_tag.empty())
					{
						candle.classification_tag = state_tag;
					}
					else
					{
						if (candle.classification_tag == "OL")
						{
							state_tag = State_Tag::L;
						}

						if (candle.classification_tag == "CL")
						{
							state_tag = State_Tag::C;
						}

						if (candle.classification_tag == "OS")
						{
							state_tag = State_Tag::S;
						}

						if (candle.classification_tag == "CS")
						{
							state_tag = State_Tag::C;
						}

						if (candle.classification_tag == "CSOL")
						{
							state_tag = State_Tag::L;
						}

						if (candle.classification_tag == "CLOS")
						{
							state_tag = State_Tag::S;
						}

						candle.classification_tag = state_tag;
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::sample_classification_tags(candles_container_t & candles,
			candles_container_t::iterator position, std::string tag) const
		{
			RUN_LOGGER(logger);

			try
			{
				concat_classification_tags(position->classification_tag, tag);

				for (auto iterator = position; iterator != std::begin(candles); --iterator)
				{
					if (std::abs((position->price_close - std::prev(iterator)->price_close) / 
						position->price_close) <= m_config.classification_max_deviation)
					{
						concat_classification_tags(std::prev(iterator)->classification_tag, tag);
					}
					else
					{
						break;
					}
				}

				for (auto iterator = std::next(position); iterator != std::end(candles); ++iterator)
				{
					if (std::abs((position->price_close - iterator->price_close) /
						position->price_close) <= m_config.classification_max_deviation)
					{
						concat_classification_tags(iterator->classification_tag, tag);
					}
					else
					{
						break;
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::concat_classification_tags(std::string & target, const std::string & tag) const
		{
			RUN_LOGGER(logger);

			try
			{
				if ((target.empty()) ||
					(target == "OL" && tag != "CL") ||
					(target == "CL" && tag != "OL") ||
					(target == "OS" && tag != "CS") ||
					(target == "CS" && tag != "OS"))
				{
					target += tag;
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::update_movement_tags(const std::string & asset, candles_container_t & candles) const
		{
			RUN_LOGGER(logger);

			try
			{
				const auto scale = "M60";

				auto path = charts_directory; path /= make_file_name(asset, scale);

				auto movement_candles = load_candles(asset, scale, path);

				for (auto i = 1U; i < std::size(candles); ++i)
				{
					const auto & candle = candles[i];

					auto iterator = std::find_if(std::begin(movement_candles), std::end(movement_candles),
						[candle](const auto & movement_candle)
						{
							return (
								movement_candle.date_time.year  == candle.date_time.year &&
								movement_candle.date_time.month == candle.date_time.month &&
								movement_candle.date_time.day   == candle.date_time.day &&
								movement_candle.date_time.hour  == 10U);
						});

					if (iterator != std::end(movement_candles) && iterator != std::begin(movement_candles))
					{
						auto previous_price_close = std::prev(iterator)->price_close;

						auto delta_L = std::abs(std::max(iterator->price_high, std::next(iterator)->price_high) - previous_price_close);
						auto delta_S = std::abs(std::min(iterator->price_low,  std::next(iterator)->price_low)  - previous_price_close);

						if (delta_L - delta_S > -1.0 * std::numeric_limits < double > ::epsilon())
						{
							candles[i - 1U].movement_tag = +1;
						}
						else
						{
							candles[i - 1U].movement_tag = -1;
						}
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::update_supports_resistances(candles_container_t & candles, const levels_container_t & levels) const
		{
			RUN_LOGGER(logger);

			try
			{
				for (auto & candle : candles)
				{
					for (const auto & level : levels)
					{
						if (level.begin >= candle.date_time)
						{
							break;
						}
						else
						{
							if ((level.price < candle.price_close) &&
								(candle.support.strength == 0U || candle.support.price < level.price))
							{
								candle.support = level;
							}

							if ((level.price > candle.price_close) &&
								(candle.resistance.strength == 0U || candle.resistance.price > level.price))
							{
								candle.resistance = level;
							}
						}
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::save_tagged_charts() const
		{
			RUN_LOGGER(logger);

			try
			{
				Data::save_tagged_charts(m_charts, m_config);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::make_environment()
		{
			RUN_LOGGER(logger);

			try
			{
				std::vector < std::future < void > > futures;

				futures.reserve(std::size(m_assets) * std::size(m_scales));

				for (const auto & asset : m_assets)
				{
					for (const auto & scale : m_scales)
					{
						std::packaged_task < void() > task([this, asset, scale]()
							{
								auto & candles = m_charts.at(asset).at(scale);

								update_regression_tags(candles);

								update_classification_tags(candles);

								update_movement_tags(asset, candles);

								auto & levels = m_supports_resistances.at(asset);

								update_supports_resistances(candles, levels);
							});

						futures.push_back(boost::asio::post(m_thread_pool, std::move(task)));
					}
				}

				std::for_each(std::begin(futures), std::end(futures), [](auto & future) { future.wait(); });
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::save_environment() const
		{
			RUN_LOGGER(logger);

			try
			{
				Data::save_environment(m_charts, m_config);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::initialize_sources()
		{
			RUN_LOGGER(logger);

			try
			{
				for (const auto & asset : m_assets)
				{
					for (const auto & scale : m_scales)
					{
						m_sources[asset][scale] = std::make_shared < Source > (asset, scale);
					}
				}
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::update_supports_resistances()
		{
			RUN_LOGGER(logger);

			try
			{
				const auto scale = m_config.level_resolution;

				if (m_config.required_supports_resistances)
				{
					for (const auto & asset : m_assets)
					{
						get_chart_for_levels(asset, scale);
					}
				}

				std::vector < std::future < void > > futures;

				futures.reserve(std::size(m_assets));

				std::mutex mutex;

				for (const auto & asset : m_assets)
				{
					auto path = levels_directory; path /= make_file_name(asset, scale);

					if (!std::filesystem::exists(path))
					{
						throw std::runtime_error("file " + path.string() + " doesn't exist");
					}

					std::packaged_task < void() > task([this, path, &mutex, asset, scale, &logger]()
						{
							auto candles = load_candles(asset, scale, path);

							std::scoped_lock lock(mutex);

							if (std::size(candles) >= m_config.prediction_timesteps * 2U)
							{
								m_charts[asset][scale] = std::move(candles);
							}
							else
							{
								logger.write(Severity::empty, "candles size exception: " + asset + " " + scale);
							}
						});

					futures.push_back(boost::asio::post(m_thread_pool, std::move(task)));
				}

				std::for_each(std::begin(futures), std::end(futures), [](auto & future) { future.wait(); });

				make_supports_resistances();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		Market::path_t Market::get_chart_for_levels(const std::string & asset, const std::string & scale) const
		{
			RUN_LOGGER(logger);

			try
			{
				auto path = levels_directory; path /= make_file_name(asset, scale);

				shared::Python python;

				try
				{
					boost::python::exec("from market import get_for_levels", python.global(), python.global());

					python.global()["get_for_levels"](asset.c_str(), scale.c_str(), path.string().c_str());
				}
				catch (const boost::python::error_already_set &)
				{
					logger.write(Severity::error, shared::Python::exception());
				}
				catch (const std::exception & exception)
				{
					shared::catch_handler < market_exception > (logger, exception);
				}

				return path;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		std::string Market::get_current_data(const std::string & asset, const std::string & scale, std::size_t size)
		{
			RUN_LOGGER(logger);

			try
			{
				std::istringstream sin(m_sources.at(asset).at(scale)->get(size));

				candles_container_t candles;

				std::string line;

				while (std::getline(sin, line))
				{
					candles.push_back(parse(line));
				}

				update_deviations(asset, scale, candles);

				update_supports_resistances(candles, m_supports_resistances.at(asset));

				return serialize_candles(candles);
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		std::vector < std::string > Market::get_current_data_variation(
			const std::string & asset, const std::string & scale, std::size_t size)
		{
			RUN_LOGGER(logger);

			try
			{
				std::istringstream sin(m_sources.at(asset).at(scale)->get(size));

				candles_container_t candles;

				std::string line;

				while (std::getline(sin, line))
				{
					candles.push_back(parse(line));
				}

				std::vector < std::string > results;

				auto step = 0.00;

				if (asset == "SBER")
				{
					step = 0.10;
				}

				if (asset == "LKOH")
				{
					step = 5.00;
				}

				for (auto i = -50; i <= +50; ++i)
				{
					candles.back().price_close = candles.back().price_open + (step * i);

					candles.back().price_high = std::max(candles.back().price_open, candles.back().price_close);
					candles.back().price_low  = std::min(candles.back().price_open, candles.back().price_close);

					update_deviations(asset, scale, candles);

					update_supports_resistances(candles, m_supports_resistances.at(asset));

					results.push_back(serialize_candles(candles));
				}

				return results;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		void Market::print_last_candle(const std::string & asset, const candles_container_t & candles) const
		{
			RUN_LOGGER(logger);

			try
			{
				std::cout << std::setw(5) << std::left << std::setfill(' ') << asset << " ";
				
				const auto & candle = candles.back();

				std::cout <<
					std::setw(4) << std::right << std::setfill('0') << candle.date_time.year  << '.' <<
					std::setw(2) << std::right << std::setfill('0') << candle.date_time.month << '.' <<
					std::setw(2) << std::right << std::setfill('0') << candle.date_time.day   << " ";

				std::cout << std::setprecision(2) << std::fixed << std::showpos <<
					100.0 * (candle.price_deviation_open + candle.price_deviation) << " ";

				std::cout << std::setw(12) << std::right << std::setfill(' ') << 
					std::setprecision(6) << std::fixed << std::noshowpos << candle.support.price << " ";

				std::cout <<
					std::setw(4) << std::setfill('0') << candle.support.begin.year  << '.' <<
					std::setw(2) << std::setfill('0') << candle.support.begin.month << '.' <<
					std::setw(2) << std::setfill('0') << candle.support.begin.day   << " ";

				std::cout << std::setw(12) << std::right << std::setfill(' ') <<
					std::setprecision(6) << std::fixed << std::noshowpos << candle.resistance.price << " ";

				std::cout <<
					std::setw(4) << std::setfill('0') << candle.resistance.begin.year  << '.' <<
					std::setw(2) << std::setfill('0') << candle.resistance.begin.month << '.' <<
					std::setw(2) << std::setfill('0') << candle.resistance.begin.day   << " ";
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		std::string Market::serialize_candles(const candles_container_t & candles) const
		{
			RUN_LOGGER(logger);

			try
			{
				std::ostringstream sout;

				static const char delimeter = ',';

				const auto & candle = candles.back();

				/*
				for (auto j = 1U; j < 13U; ++j)
				{
					if (j == candle.date_time.month)
					{
						sout << "1" << delimeter;
					}
					else
					{
						sout << "0" << delimeter;
					}
				}

				sout << std::setprecision(3) << std::fixed << std::noshowpos <<
					candle.date_time.day / days_in_month << delimeter;
				*/

				const auto day = day_of_week(candle);

				for (auto j = 0U; j < 5U; ++j)
				{
					if (j == day)
					{
						sout << "1" << delimeter;
					}
					else
					{
						sout << "0" << delimeter;
					}
				}

				Level level;

				auto support_deviation = (candle.price_close - candle.support.price) / candle.price_close;

				if (support_deviation < m_config.level_max_deviation)
				{
					level = candle.support;
				}

				auto resistance_deviation = (candle.resistance.price - candle.price_close) / candle.price_close;

				if (resistance_deviation < m_config.level_max_deviation &&
					resistance_deviation < support_deviation &&
					candle.support.begin < candle.resistance.begin)
				{
					level = candle.resistance;
				}

				if (level.strength != 0U)
				{
					auto level_alive = (candle.date_time.to_time_t() - level.begin.to_time_t()) /
						seconds_in_day / m_config.level_max_lifetime;

					sout << std::setprecision(6) << std::fixed << std::noshowpos <<
						(level_alive > 1.0 ? 0.0 : level_alive) << delimeter;
				}
				else
				{
					sout << std::setprecision(6) << std::fixed << std::noshowpos << 0.0 << delimeter;
				}

				const auto volume_history_length = m_config.volume_timesteps;

				if (volume_history_length > m_config.prediction_timesteps)
				{
					throw std::runtime_error("volume history length is too long");
				}

				const auto volume_deviation_multiplier = get_volume_deviation_multiplier(m_config.prediction_timeframe);

				for (auto j = 0U; j < volume_history_length; ++j)
				{
					auto volume_deviation = candles[std::size(candles) - volume_history_length + j].volume_deviation *
						volume_deviation_multiplier;

					sout << std::setprecision(6) << std::fixed << std::showpos <<
						(volume_deviation > 1.0 ? 1.0 : (volume_deviation < -1.0 ? -1.0 : volume_deviation)) << delimeter;
				}

				const auto price_deviation_multiplier = get_price_deviation_multiplier(m_config.prediction_timeframe);

				for (auto j = 0U; j < m_config.prediction_timesteps; ++j)
				{
					auto price_deviation_1 = candles[j].price_deviation_open * price_deviation_multiplier;
					auto price_deviation_2 = candles[j].price_deviation      * price_deviation_multiplier;

					auto price_deviation = price_deviation_1 + price_deviation_2;

					sout << std::setprecision(6) << std::fixed << std::showpos <<
						(price_deviation > 1.0 ? 1.0 : (price_deviation < -1.0 ? -1.0 : price_deviation)) << delimeter;
				}

				auto price_deviation_1 = candle.price_deviation_open * price_deviation_multiplier;
				auto price_deviation_2 = candle.price_deviation      * price_deviation_multiplier;
				auto price_deviation_3 = candle.price_deviation_max  * price_deviation_multiplier;
				auto price_deviation_4 = candle.price_deviation_min  * price_deviation_multiplier;

				sout << std::setprecision(6) << std::fixed << std::noshowpos <<
					(price_deviation_3 > 1.0 ? 1.0 : price_deviation_3) << delimeter;
				sout << std::setprecision(6) << std::fixed << std::noshowpos <<
					(price_deviation_4 > 1.0 ? 1.0 : price_deviation_4) << delimeter;
				sout << std::setprecision(6) << std::fixed << std::showpos <<
					(price_deviation_1 > 1.0 ? 1.0 : (price_deviation_1 < -1.0 ? -1.0 : price_deviation_1)) << delimeter;
				sout << std::setprecision(6) << std::fixed << std::showpos <<
					(price_deviation_2 > 1.0 ? 1.0 : (price_deviation_2 < -1.0 ? -1.0 : price_deviation_2));

				return sout.str();
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		double Market::get_price_deviation_multiplier(const std::string & scale)
		{
			RUN_LOGGER(logger);

			try
			{
				if (scale == Scale::H)
				{
					return 40.0;
				}

				if (scale == Scale::D)
				{
					return 10.0;
				}

				return 1.0;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

		double Market::get_volume_deviation_multiplier(const std::string & scale)
		{
			RUN_LOGGER(logger);

			try
			{
				return 1.0;
			}
			catch (const std::exception & exception)
			{
				shared::catch_handler < market_exception > (logger, exception);
			}
		}

	} // namespace system

} // namespace solution
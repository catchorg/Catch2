/*
 *  Created by Joachim on 16/04/2019.
 *  Adapted from donated nonius code.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

// User-facing chronometer

#ifndef TWOBLUECUBES_CATCH_CHRONOMETER_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_CHRONOMETER_HPP_INCLUDED

#include "catch_clock.hpp"
#include "catch_optimizer.hpp"
#include "detail/catch_complete_invoke.hpp"
#include "../catch_meta.hpp"

namespace Catch {
    namespace Benchmark {
        namespace Detail {
            struct ChronometerConcept {
                using DefaultClockDuration = ClockDuration<default_clock>;

                virtual void start() = 0;
                virtual void finish() = 0;

                virtual DefaultClockDuration getElapsed() const = 0;
                virtual void setElapsed(DefaultClockDuration) = 0;

                virtual ~ChronometerConcept() = default;
            };
            template <typename Clock>
            struct ChronometerModel final : public ChronometerConcept {
                void start() override { started = Clock::now(); }
                void finish() override { elapsed = Clock::now() - started; }

                DefaultClockDuration getElapsed() const override { return elapsed; }
                void setElapsed(DefaultClockDuration _elapsed) override {
                    elapsed = _elapsed;
                }

                TimePoint<Clock> started;
                ClockDuration<Clock> elapsed;
            };
        } // namespace Detail

        struct Chronometer {
        public:
            using DefaultClockDuration = Detail::ChronometerConcept::DefaultClockDuration;

            template <typename Fun>
            void measure(Fun&& fun) { measure(std::forward<Fun>(fun), is_callable<Fun(int)>()); }

            int runs() const { return k; }

            DefaultClockDuration getElapsed() const { return impl->getElapsed(); }
            void setElapsed(DefaultClockDuration elapsed) { impl->setElapsed(elapsed); }

            Chronometer(Detail::ChronometerConcept& meter, int k)
                : impl(&meter)
                , k(k) {}

        private:
            template <typename Fun>
            void measure(Fun&& fun, std::false_type) {
                measure([&fun](int) { return fun(); }, std::true_type());
            }

            template <typename Fun>
            void measure(Fun&& fun, std::true_type) {
                Detail::optimizer_barrier();
                impl->start();
                for (int i = 0; i < k; ++i) invoke_deoptimized(fun, i);
                impl->finish();
                Detail::optimizer_barrier();
            }

            Detail::ChronometerConcept* impl;
            int k;
        };
    } // namespace Benchmark
} // namespace Catch

#endif // TWOBLUECUBES_CATCH_CHRONOMETER_HPP_INCLUDED

// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#include "CaseEvaluator.h"

#include "Abortion.h"
#include "CaseReporter.h"
#include "Observer.h"

#include <iostream>

namespace clean_test::execute {

CaseResult CaseEvaluator::operator()(framework::Case & tc) noexcept
{
    using Clock = CaseResult::Clock;

    auto reporter = CaseReporter{std::cout};
    auto observer = Observer{reporter};

    reporter(CaseReporter::Start{tc.m_name.path()});
    auto execution_outcome = CaseStatus{CaseStatus::pass};
    auto const time_start = Clock::now();
    try {
        tc.m_runner(observer);
    } catch (execute::Abortion const &) {
        execution_outcome = CaseStatus::abort;
    } catch (std::exception const & xcp) {
        execution_outcome = CaseStatus::abort;
    } catch (...) {
        execution_outcome = CaseStatus::abort;
    }
    auto const wall_time = Clock::now() - time_start;

    auto result = CaseResult{
        std::string{tc.m_name.path()}, execution_outcome, wall_time, std::move(observer).release()};
    reporter(CaseReporter::Stop{tc.m_name.path(), wall_time, result.m_status});
    return result;
}


}

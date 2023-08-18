/*
 * This class stores a single running Aggregate that can do two things:
 *  - Combine with a new Aggregate (insert function)
 *  - Reset to the null Aggregate (clear function)
 *
 * Use this class to aggregate continuously collected measurements into summary statistics. It may lose accuracy for
 * large sets of measurements.
 *
 * Time complexity:
 *  - insertion of new measurement: O(1)
 *  - clear current Aggregate: O(1)
 *  - computing current Aggregate: O(1)
 *
 * Memory usage:
 *  - 1 aggregate for n aggregate chunks
 *
 * Implemented by Kevin Ahrendt for the ESPHome project, 2023
 */

#pragma once

#include "aggregate.h"
#include "aggregate_queue.h"

namespace esphome {
namespace statistics {

class ContinuousSingular : public AggregateQueue {
 public:
  ContinuousSingular(StatisticsCalculationConfig statistics_calculation_config)
      : AggregateQueue(statistics_calculation_config) {
    this->running_aggregate_ = make_unique<Aggregate>(statistics_calculation_config_);
  }

  //////////////////////////////////////////////////////////
  // Overridden virtual methods from AggregateQueue class //
  //////////////////////////////////////////////////////////

  /// @brief Return the summary statistics for the running Aggregate.
  /// @return the current running Aggregate
  Aggregate compute_current_aggregate() override { return *this->running_aggregate_; }

  /// @brief Clear all Aggregates in the queue.
  void clear() override;

  /// @brief Equivalent to clearing all Aggregates in the queue
  void evict() override { this->clear(); }

  /** Insert new Aggregate into running Aggegrate.
   *
   * <value> is aggregated into <running_aggregate_>
   * @param value the Aggregate to be inserted
   */
  void insert(Aggregate value) override;

  /** Verifies whether running_aggregate_ was successfully allocated
   *
   * @param capacity not applicable to class
   * @param tracked_statistics_config not applicable to class
   * @return true if memory was successfully allocated, false otherwise
   */
  bool set_capacity(size_t capacity, TrackedStatisticsConfiguration tracked_statistics_config) override;

 protected:
  // Stores summary statistics for all inserted Aggregate chunks into this queue
  std::unique_ptr<Aggregate> running_aggregate_;
};

}  // namespace statistics
}  // namespace esphome

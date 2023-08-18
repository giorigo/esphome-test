/*
 * Aggregate class stores summary statistics for a set of measurements. It is mathematically a monoid paired with a
 * binary operation and an identity. It has three main purposes:
 *  - Set a default value for a null measurement (a set with 0 measurements) - Aggregate constructor
 *    - This is the identity monoid
 *  - Combine two aggregates from two disjoint sets of measurements - combine_with method
 *    - This is the binary operation
 *    - Summary statistics are combined using parallel algorithms
 *      - See the article "Numerically Stable Parallel Computation of (Co-)Variance" by Schubert and Gertz for details
 *  - Compute summary statistics from the stored aggregates - get_* and compute_* methods
 *    - Some summary statistics are directly stored
 *      - argmax, argmin, count, duration, min, mean, and max
 *      - Use get_* methods for retrieval
 *    - Some of the stored summary statistics in the monoid are not useful immediately but can be combined with other
 *      stored measurements to compute a useful statistic
 *      - Covariance uses the stored count (or duration if duration weighted) and C2 statistics
 *        - C2 uses the mean, count (or duration if duration weighted), and timestamp mean for the combine operation
 *      - Quadrature is the area under the curve and uses the duration and measurement mean statistics
 *      - Trend is computed using C2 and timestamp M2 statistics
 *        - timestamp M2 statistic additionally requires the timestamp mean and count (or duration if duration weighted)
 *          for the combine operation
 *      - Variance and standard deviation are computed using the stored count (or duration if duration weighted) and M2
 *        statistics
 *        - M2 requires the mean statistic and count (or duration if duration weighted) for the combine operation
 *      - Use compute_* methods for retrieval
 *
 * For any statistic that uses timestamp_mean, the Aggregate also stores timestamp_reference.
 *  - timestamp_reference is the offset (in milliseconds) for the timestamp_mean
 *  - Both timestamp_means need to be normalized before combined so that they reference the same timestamp
 *    - The normalizing process involves finding the time delta between the two references; this avoids issues from
 *      millis() rolling over
 *  - This approach ensures one of the timestamp_reference's is 0 when combining two Aggregates
 *    - This makes timestamp_mean as small as possible to minimize potential floating point precision issues
 *
 * Implemented by Kevin Ahrendt for the ESPHome project, 2023
 */

#pragma once

#include "esphome/core/helpers.h"  // necessary for ExternalRAMAllocator

#include <cmath>    // necessary for NaN
#include <cstdint>  // necessary for uint32_t
#include <ctime>    // necessary for time_t
#include <limits>   // necessary for std::numeric_limits infinity

namespace esphome {
namespace statistics {

// Group type determines whether to apply Bessel's correction (for simple weights) or relaibility weights (for duration
// weights)
enum GroupType : uint8_t {
  GROUP_TYPE_SAMPLE,
  GROUP_TYPE_POPULATION,
};

// Weight type determines how measurements are weighted. For simple weighted, all measuremens have a weight of 1. For
// duration weighted, each measurement uses its duration as its weight.
enum WeightType : uint8_t {
  WEIGHT_TYPE_SIMPLE,
  WEIGHT_TYPE_DURATION,
};

struct StatisticsCalculationConfig {
  enum GroupType group_type;
  enum WeightType weight_type;
};

class Aggregate {
 public:
  /// @brief Construct a null measurement Aggregate.
  Aggregate(StatisticsCalculationConfig statistics_calculation_config) {
    this->statistics_calculation_config_ = statistics_calculation_config;
  }

  /** Construct an Aggregate.
   *
   * @param value sensor measurement
   * @param duration length of time (in milliseconds) for measurement
   * @param timestamp timestamp (in milliseconds) since boot; i.e., millis()
   * @param unix_time UTC Unix time for measurement (in seconds)
   */
  Aggregate(StatisticsCalculationConfig statistics_config, double value, uint64_t duration, uint32_t timestamp,
            time_t unix_time);

  /** Combine Aggregate with another.
   *
   * Binary operation that combines two aggregates that store statistics from non-overlapping sets of measurements.
   * @param b Aggregate to combine with
   * @return combined Aggregate
   */
  Aggregate combine_with(const Aggregate &b);

  /** Compute the covariance of the set of measurements with respect to timestamps.
   *
   * Unused in component but available for lambdas.
   * Uses count if WEIGHT_TYPE_SIMPLE and duration if WEIGHT_TYPE_DURATION.
   * Applies Bessel's correction or implements reliability weights if GROUP_TYPE_SAMPLE.
   * @return covariance of the set of measurements
   */
  double compute_covariance() const;

  /** Compute the area under the curve of the measurements.
   *
   * @return area under the curve in units of the sensor times milliseconds
   */
  double compute_quadrature() const;

  /** Compute the standard deviation of the set of measurements.
   *
   * Uses count if WEIGHT_TYPE_SIMPLE and duration if WEIGHT_TYPE_DURATION.
   * Applies Bessel's correction or implements reliability weights if GROUP_TYPE_SAMPLE.
   * @return standard deviation of the set of measurements
   */
  double compute_std_dev() const;

  /** Compute the slope of the line of best fit
   *
   * @return trend of the set of measurements over time
   */
  double compute_trend() const;

  /** Compute the variance of the set of measurements.
   *
   * Uses count if WEIGHT_TYPE_SIMPLE and duration if WEIGHT_TYPE_DURATION.
   * Applies Bessel's correction or implements reliability weights if GROUP_TYPE_SAMPLE.
   * @return variance of the set of measurements
   */
  double compute_variance() const;

  // The UTC Unix time of the most recent maximum value in the set of measurements
  time_t get_argmax() const { return this->argmax_; }
  void set_argmax(time_t timestamp) { this->argmax_ = timestamp; }

  // The UTC Unix time of the most recent minimum  value in the set of measurements
  time_t get_argmin() const { return this->argmin_; }
  void set_argmin(time_t timestamp) { this->argmin_ = timestamp; }

  // C2 from Welford's algorithm; used to compute the covariance of the measurements and timestamps weighted
  double get_c2() const { return this->c2_; }
  void set_c2(double c2) { this->c2_ = c2; }

  // The count of valid measurements in the Aggregate; i.e., not NaN
  size_t get_count() const { return this->count_; }
  void set_count(size_t count) { this->count_ = count; }

  // The duration of measurements in the Aggregate in milliseconds
  uint64_t get_duration() const { return this->duration_; }
  void set_duration(uint64_t duration) { this->duration_ = duration; }

  // The sum of the duration of measurements in the Aggregate squared in milliseconds squared; used for computing
  // reliability weights
  uint64_t get_duration_squared() const { return this->duration_squared_; }
  void set_duration_squared(uint64_t duration_squared) { this->duration_squared_ = duration_squared; }

  // M2 from Welford's algorithm; used to compute variance
  double get_m2() const { return this->m2_; }
  void set_m2(double m2) { this->m2_ = m2; }

  // Maximum value of the measurements in the Aggregate
  float get_max() const { return this->max_; }
  void set_max(float max) { this->max_ = max; }

  // Average value of the measurements in the Aggregate
  double get_mean() const { return this->mean_; }
  void set_mean(double mean) { this->mean_ = mean; }

  // Minimum value of the measurements in the Aggregate
  float get_min() const { return this->min_; }
  void set_min(float min) { this->min_ = min; }

  // M2 from Welford's algorithm for timestamps; used to compute variance of timestamps
  double get_timestamp_m2() const { return this->timestamp_m2_; }
  void set_timestamp_m2(double timestamp_m2) { this->timestamp_m2_ = timestamp_m2; }

  // Average timestamp for the measurements in the Aggregate; offset by timestamp_reference
  double get_timestamp_mean() const { return this->timestamp_mean_; }
  void set_timestamp_mean(double timestamp_mean) { this->timestamp_mean_ = timestamp_mean; }

  // The reference timestamp (in milliseconds) that offsets timestamp_mean
  uint32_t get_timestamp_reference() const { return this->timestamp_reference_; }
  void set_timestamp_reference(uint32_t timestamp_reference) { this->timestamp_reference_ = timestamp_reference; }

 protected:
  /*
   * Default values for the Aggregate are the values for a null measurement or the empty set of measurements;
   * i.e., no measurement/observation at all
   */

  // The reference timestamp for the timestamp mean values
  // e.g., if we have one raw timestamp at 5 ms and the reference is 5 ms, we store 0 ms in timestamp_mean
  uint32_t timestamp_reference_{0};

  // The UTC Unix times for the most recent extreme values in the set of measurements
  time_t argmin_{std::numeric_limits<time_t>::infinity() * (-1)};
  time_t argmax_{std::numeric_limits<time_t>::infinity() * (-1)};

  // Count of non-NaN measurements in the set of measurements
  size_t count_{0};

  // Sum of the durations between successive measurements in the Aggregate's set of measurements; kept as milliseconds
  uint64_t duration_{0};

  // Sum of the durations between successive measurements squared in the Aggregate's set of measurements; necessary for
  // reliability weights
  uint64_t duration_squared_{0};

  // Quantity used in an extension of Welford's algorithm for finding the covariance of the Aggregate's set of
  // measurements and timestamps
  double c2_{NAN};

  // Extrema of the Aggregate's set of measurements
  float max_{std::numeric_limits<float>::infinity() * (-1)};  // the supremum of the empty set is -infinity
  float min_{std::numeric_limits<float>::infinity()};         // the infimum of the empty set is +infinity

  // Quantity used in Welford's algorithm for finding the variance of the Aggregate's set of measurements
  double m2_{NAN};

  // Average of the Aggregate's set of measurements
  double mean_{NAN};

  // Quantity used in Welford's algorithm for finding the variance of timestamps in the Aggregate's set of measurements
  double timestamp_m2_{NAN};

  double timestamp_mean_{NAN};

  StatisticsCalculationConfig statistics_calculation_config_{};

  //////////////////////
  // Internal Methods //
  //////////////////////

  /** Compute the denominator for the variance and covariance calculations.
   *
   * Uses count if WEIGHT_TYPE_SIMPLE and duration if WEIGHT_TYPE_DURATION.
   * Applies Bessel's correction or implements reliability weights if GROUP_TYPE_SAMPLE.
   * @return denominator used in variance and covariance calculations
   */
  double denominator_() const;

  /** Normalize two timestamp means.
   *
   * Normalize the timestamp means so that they both reference the more recent timestamp.
   * @param a_mean timestamp mean of Aggregate - may be adjusted after execution
   * @param a_timestamp_reference timestamp reference of the first Aggregate
   * @param a_count count of measurements in first Aggregate
   * @param b_mean timestamp mean of second Aggregate - may be adjusted after execution
   * @param b_timestamp_reference timestamp reference of second Aggregate
   * @param b_count count of measurements in second Aggregate
   * @return timestamp that a_mean and b_mean are now both in reference to
   */
  double normalize_timestamp_means_(double &a_mean, uint32_t a_timestamp_reference, size_t a_count, double &b_mean,
                                    uint32_t b_timestamp_reference, size_t b_count);

  /** Returns the more recent timestamp
   *
   * Returns the more recent timestamp parameter; handles uint32_t overflow
   * @param a_timestamp first timestamp to compare
   * @param b_timestamp second timestamp to compare
   */
  inline uint32_t more_recent_timestamp_(uint32_t a_timestamp, uint32_t b_timestamp);
};

}  // namespace statistics
}  // namespace esphome

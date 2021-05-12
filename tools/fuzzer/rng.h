/*
 * Copyright 2021 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef INCLUDE_RNG_H
#define INCLUDE_RNG_H

#include <cassert>
#include <functional>
#include <queue>
#include <random>

namespace fuzzer {

typedef std::mt19937::result_type rand_t;

// Extracting common logic for calling a callback with the generated value.
class RngCallbackNotifier {
 public:
  void set_callback(std::function<void(rand_t)> callback) {
    callback_ = std::move(callback);
  }

 protected:
  rand_t consume(rand_t value) {
    callback_(value);
    return value;
  }

 private:
  std::function<void(rand_t)> callback_;
};

class Mt19937 : public RngCallbackNotifier {
 public:
  explicit Mt19937(rand_t seed) : rng_(seed) {}

  using result_type = rand_t;
  static constexpr result_type min() { return std::mt19937::min(); }
  static constexpr result_type max() { return std::mt19937::max(); }

  result_type operator()() { return consume(rng_()); }

 private:
  std::mt19937 rng_;
};

// Random number generator over fixed number sequence.
class FixedRng : public RngCallbackNotifier {
 public:
  explicit FixedRng(std::queue<rand_t> q) : queue_(std::move(q)) {}

  using result_type = rand_t;
  static constexpr result_type min() { return std::mt19937::min(); }
  static constexpr result_type max() { return std::mt19937::max(); }

  result_type operator()() {
    assert(!queue_.empty());
    auto value = queue_.front();
    queue_.pop();
    return consume(value);
  }

 private:
  std::queue<rand_t> queue_;
};

}  // namespace fuzzer

#endif  // INCLUDE_RNG_H
// Copyright (c) 2017-present, Facebook, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.

#include "merge_operator.h"

#include <memory>
#include <assert.h>

#include "format.h"
#include "rocksdb/slice.h"
#include "rocksdb/merge_operator.h"
#include "utilities/merge_operators.h"

namespace rocksdb {
namespace cassandra {

// Implementation for the merge operation (merges two Cassandra values)
bool CassandraValueMergeOperator::FullMergeV2(
    const MergeOperationInput& merge_in,
    MergeOperationOutput* merge_out) const {
  // Clear the *new_value for writing.
  merge_out->new_value.clear();

  if (merge_in.existing_value == nullptr && merge_in.operand_list.size() == 1) {
    // Only one operand
    merge_out->existing_operand = merge_in.operand_list.back();
    return true;
  }

  std::vector<RowValue> row_values;
  if (merge_in.existing_value) {
    row_values.push_back(
      RowValue::Deserialize(merge_in.existing_value->data(),
                            merge_in.existing_value->size()));
  }

  for (auto& operand : merge_in.operand_list) {
    row_values.push_back(RowValue::Deserialize(operand.data(), operand.size()));
  }

  RowValue merged = RowValue::Merge(std::move(row_values));
  merge_out->new_value.reserve(merged.Size());
  merged.Serialize(&(merge_out->new_value));

  return true;
}

// Implementation for the merge operation (merges two Cassandra values)
bool CassandraValueMergeOperator::PartialMerge(const Slice& key,
                                               const Slice& left_operand,
                                               const Slice& right_operand,
                                               std::string* new_value,
                                               Logger* logger) const {
  // Clear the *new_value for writing.
  assert(new_value);
  new_value->clear();

  std::vector<RowValue> row_values;
  row_values.push_back(RowValue::Deserialize(left_operand.data(),
                                             left_operand.size()));
  row_values.push_back(RowValue::Deserialize(right_operand.data(),
                                             right_operand.size()));
  RowValue merged = RowValue::Merge(std::move(row_values));
  new_value->reserve(merged.Size());
  merged.Serialize(new_value);
  return true;
}

bool CassandraValueMergeOperator::PartialMergeMulti(
    const Slice& key,
    const std::deque<Slice>& operand_list,
    std::string* new_value,
    Logger* logger) const {
  // Clear the *new_value for writing.
  assert(new_value);
  new_value->clear();

  std::vector<RowValue> row_values;
  for (auto& operand : operand_list) {
    row_values.push_back(RowValue::Deserialize(operand.data(), operand.size()));
  }
  RowValue merged = RowValue::Merge(std::move(row_values));
  new_value->reserve(merged.Size());
  merged.Serialize(new_value);
  return true;
}

const char* CassandraValueMergeOperator::Name() const  {
  return "CassandraValueMergeOperator";
}

} // namespace cassandra

std::shared_ptr<MergeOperator>
    MergeOperators::CreateCassandraMergeOperator() {
  return std::make_shared<rocksdb::cassandra::CassandraValueMergeOperator>();
}

} // namespace rocksdb

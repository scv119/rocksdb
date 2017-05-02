// Copyright (c) 2017-present, Facebook, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.

#include <stdio.h>
#include <stdlib.h>
#include <jni.h>
#include <string>
#include <memory>

#include "include/org_rocksdb_CassandraValueMergeOperator.h"
#include "rocksjni/portal.h"
#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include "rocksdb/statistics.h"
#include "rocksdb/memtablerep.h"
#include "rocksdb/table.h"
#include "rocksdb/slice_transform.h"
#include "rocksdb/merge_operator.h"
#include "utilities/merge_operators/cassandra/merge_operator.h"

/*
 * Class:     org_rocksdb_CassandraValueMergeOperator
 * Method:    newSharedCassandraValueMergeOperator
 * Signature: ()J
 */
jlong Java_org_rocksdb_CassandraValueMergeOperator_newSharedCassandraValueMergeOperator
(JNIEnv* env, jclass jclazz) {
  auto* sptr_string_append_op = new std::shared_ptr<rocksdb::MergeOperator>(
    rocksdb::CassandraValueMergeOperator::CreateSharedInstance());
  return reinterpret_cast<jlong>(sptr_string_append_op);
}

/*
 * Class:     org_rocksdb_CassandraValueMergeOperator
 * Method:    disposeInternal
 * Signature: (J)V
 */
void Java_org_rocksdb_CassandraValueMergeOperator_disposeInternal(
    JNIEnv* env, jobject jobj, jlong jhandle) {
  auto* sptr_string_append_op =
      reinterpret_cast<std::shared_ptr<rocksdb::MergeOperator>* >(jhandle);
  delete sptr_string_append_op;  // delete std::shared_ptr
}

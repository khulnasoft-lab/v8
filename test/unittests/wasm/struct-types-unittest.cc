// Copyright 2022 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/wasm/struct-types.h"

#include "test/unittests/test-utils.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace v8::internal::wasm {
namespace struct_types_unittest {

class StructTypesTest : public TestWithZone {};

TEST_F(StructTypesTest, Empty) {
  ModuleStructType::Builder builder(this->zone(), 0);
  StructType* type = builder.Build();
  EXPECT_EQ(0u, type->total_fields_size());
}

TEST_F(StructTypesTest, OneField) {
  ModuleStructType::Builder builder(this->zone(), 1);
  builder.AddField(kWasmI32, true);
  StructType* type = builder.Build();
  uint32_t expected = std::max(kUInt32Size, kTaggedSize);
  EXPECT_EQ(expected, type->total_fields_size());
  EXPECT_EQ(0u, type->field_offset(0));
}

TEST_F(StructTypesTest, Packing) {
  ModuleStructType::Builder builder(this->zone(), 5);
  builder.AddField(kWasmI64, true);
  builder.AddField(kWasmI8, true);
  builder.AddField(kWasmI32, true);
  builder.AddField(kWasmI16, true);
  builder.AddField(kWasmI8, true);
  StructType* type = builder.Build();
  EXPECT_EQ(16u, type->total_fields_size());
  EXPECT_EQ(0u, type->field_offset(0));
  EXPECT_EQ(8u, type->field_offset(1));
  EXPECT_EQ(12u, type->field_offset(2));
  EXPECT_EQ(10u, type->field_offset(3));
  EXPECT_EQ(9u, type->field_offset(4));
}

TEST_F(StructTypesTest, CopyingOffsets) {
  ModuleStructType::Builder builder(this->zone(), 5);
  builder.AddField(kWasmI64, true);
  builder.AddField(kWasmI8, true);
  builder.AddField(kWasmI32, true);
  builder.AddField(kWasmI16, true);
  builder.AddField(kWasmI8, true);
  StructType* type = builder.Build();

  ModuleStructType::Builder copy_builder(this->zone(), type->field_count());
  for (uint32_t i = 0; i < type->field_count(); i++) {
    copy_builder.AddField(type->field(i), type->mutability(i),
                          type->field_offset(i));
  }
  copy_builder.set_total_fields_size(type->total_fields_size());

  StructType* copy = copy_builder.Build();
  for (uint32_t i = 0; i < type->field_count(); i++) {
    EXPECT_EQ(type->field_offset(i), copy->field_offset(i));
  }
  EXPECT_EQ(type->total_fields_size(), copy->total_fields_size());
}

}  // namespace struct_types_unittest
}  // namespace v8::internal::wasm

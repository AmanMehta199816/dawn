// Copyright 2020 The Tint Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "src/tint/sem/test_helper.h"
#include "src/tint/sem/texture_type.h"

namespace tint {
namespace sem {
namespace {

using ArrayTest = TestHelper;

TEST_F(ArrayTest, CreateSizedArray) {
  U32 u32;
  auto* arr = create<Array>(&u32, 2u, 4u, 8u, 32u, 16u);
  EXPECT_EQ(arr->ElemType(), &u32);
  EXPECT_EQ(arr->Count(), 2u);
  EXPECT_EQ(arr->Align(), 4u);
  EXPECT_EQ(arr->Size(), 8u);
  EXPECT_EQ(arr->Stride(), 32u);
  EXPECT_EQ(arr->ImplicitStride(), 16u);
  EXPECT_FALSE(arr->IsStrideImplicit());
  EXPECT_FALSE(arr->IsRuntimeSized());
}

TEST_F(ArrayTest, CreateRuntimeArray) {
  U32 u32;
  auto* arr = create<Array>(&u32, 0u, 4u, 8u, 32u, 32u);
  EXPECT_EQ(arr->ElemType(), &u32);
  EXPECT_EQ(arr->Count(), 0u);
  EXPECT_EQ(arr->Align(), 4u);
  EXPECT_EQ(arr->Size(), 8u);
  EXPECT_EQ(arr->Stride(), 32u);
  EXPECT_EQ(arr->ImplicitStride(), 32u);
  EXPECT_TRUE(arr->IsStrideImplicit());
  EXPECT_TRUE(arr->IsRuntimeSized());
}

TEST_F(ArrayTest, TypeName) {
  I32 i32;
  auto* arr = create<Array>(&i32, 2u, 0u, 4u, 4u, 4u);
  EXPECT_EQ(arr->type_name(), "__array__i32_count_2_align_0_size_4_stride_4");
}

TEST_F(ArrayTest, FriendlyNameRuntimeSized) {
  auto* arr = create<Array>(create<I32>(), 0u, 0u, 4u, 4u, 4u);
  EXPECT_EQ(arr->FriendlyName(Symbols()), "array<i32>");
}

TEST_F(ArrayTest, FriendlyNameStaticSized) {
  auto* arr = create<Array>(create<I32>(), 5u, 4u, 20u, 4u, 4u);
  EXPECT_EQ(arr->FriendlyName(Symbols()), "array<i32, 5>");
}

TEST_F(ArrayTest, FriendlyNameRuntimeSizedNonImplicitStride) {
  auto* arr = create<Array>(create<I32>(), 0u, 0u, 4u, 8u, 4u);
  EXPECT_EQ(arr->FriendlyName(Symbols()), "@stride(8) array<i32>");
}

TEST_F(ArrayTest, FriendlyNameStaticSizedNonImplicitStride) {
  auto* arr = create<Array>(create<I32>(), 5u, 4u, 20u, 8u, 4u);
  EXPECT_EQ(arr->FriendlyName(Symbols()), "@stride(8) array<i32, 5>");
}

TEST_F(ArrayTest, TypeName_RuntimeArray) {
  I32 i32;
  auto* arr = create<Array>(&i32, 2u, 4u, 8u, 16u, 16u);
  EXPECT_EQ(arr->type_name(), "__array__i32_count_2_align_4_size_8_stride_16");
}

}  // namespace
}  // namespace sem
}  // namespace tint

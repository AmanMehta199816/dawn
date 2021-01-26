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

#include "gtest/gtest.h"
#include "src/reader/wgsl/parser_impl.h"
#include "src/reader/wgsl/parser_impl_test_helper.h"
#include "src/type/alias_type.h"
#include "src/type/array_type.h"
#include "src/type/i32_type.h"
#include "src/type/struct_type.h"

namespace tint {
namespace reader {
namespace wgsl {
namespace {

TEST_F(ParserImplTest, TypeDecl_ParsesType) {
  auto p = parser("type a = i32");

  auto* i32 = p->builder().create<type::I32>();

  auto t = p->type_alias();
  EXPECT_FALSE(p->has_error());
  EXPECT_FALSE(t.errored);
  EXPECT_TRUE(t.matched);
  ASSERT_NE(t.value, nullptr);
  ASSERT_TRUE(t->Is<type::Alias>());
  auto* alias = t->As<type::Alias>();
  ASSERT_TRUE(alias->type()->Is<type::I32>());
  ASSERT_EQ(alias->type(), i32);
}

TEST_F(ParserImplTest, TypeDecl_ParsesStruct_Ident) {
  auto p = parser("type a = B");

  type::Struct str(p->builder().Symbols().Get("B"), {});
  p->register_constructed("B", &str);

  auto t = p->type_alias();
  EXPECT_FALSE(p->has_error());
  EXPECT_FALSE(t.errored);
  EXPECT_TRUE(t.matched);
  ASSERT_NE(t.value, nullptr);
  ASSERT_TRUE(t->Is<type::Alias>());
  auto* alias = t->As<type::Alias>();
  EXPECT_EQ(p->builder().Symbols().NameFor(alias->symbol()), "a");
  ASSERT_TRUE(alias->type()->Is<type::Struct>());

  auto* s = alias->type()->As<type::Struct>();
  EXPECT_EQ(s->symbol(), p->builder().Symbols().Get("B"));
  EXPECT_EQ(s->symbol(), p->builder().Symbols().Get("B"));
}

TEST_F(ParserImplTest, TypeDecl_MissingIdent) {
  auto p = parser("type = i32");
  auto t = p->type_alias();
  EXPECT_TRUE(t.errored);
  EXPECT_FALSE(t.matched);
  EXPECT_TRUE(p->has_error());
  EXPECT_EQ(t.value, nullptr);
  EXPECT_EQ(p->error(), "1:6: expected identifier for type alias");
}

TEST_F(ParserImplTest, TypeDecl_InvalidIdent) {
  auto p = parser("type 123 = i32");
  auto t = p->type_alias();
  EXPECT_TRUE(t.errored);
  EXPECT_FALSE(t.matched);
  EXPECT_TRUE(p->has_error());
  EXPECT_EQ(t.value, nullptr);
  EXPECT_EQ(p->error(), "1:6: expected identifier for type alias");
}

TEST_F(ParserImplTest, TypeDecl_MissingEqual) {
  auto p = parser("type a i32");
  auto t = p->type_alias();
  EXPECT_TRUE(t.errored);
  EXPECT_FALSE(t.matched);
  EXPECT_TRUE(p->has_error());
  EXPECT_EQ(t.value, nullptr);
  EXPECT_EQ(p->error(), "1:8: expected '=' for type alias");
}

TEST_F(ParserImplTest, TypeDecl_InvalidType) {
  auto p = parser("type a = B");
  auto t = p->type_alias();
  EXPECT_TRUE(t.errored);
  EXPECT_FALSE(t.matched);
  EXPECT_TRUE(p->has_error());
  EXPECT_EQ(t.value, nullptr);
  EXPECT_EQ(p->error(), "1:10: unknown constructed type 'B'");
}

}  // namespace
}  // namespace wgsl
}  // namespace reader
}  // namespace tint

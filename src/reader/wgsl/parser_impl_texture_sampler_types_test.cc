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

#include "src/reader/wgsl/parser_impl_test_helper.h"
#include "src/sem/depth_texture_type.h"
#include "src/sem/multisampled_texture_type.h"
#include "src/sem/sampled_texture_type.h"

namespace tint {
namespace reader {
namespace wgsl {
namespace {

TEST_F(ParserImplTest, TextureSamplerTypes_Invalid) {
  auto p = parser("1234");
  auto t = p->texture_sampler_types();
  EXPECT_EQ(t.value, nullptr);
  EXPECT_FALSE(t.matched);
  EXPECT_FALSE(t.errored);
  EXPECT_FALSE(p->has_error());
}

TEST_F(ParserImplTest, TextureSamplerTypes_Sampler) {
  auto p = parser("sampler");
  auto t = p->texture_sampler_types();
  ASSERT_FALSE(p->has_error()) << p->error();
  EXPECT_TRUE(t.matched);
  EXPECT_FALSE(t.errored);
  ASSERT_NE(t.value, nullptr);
  ASSERT_TRUE(t->Is<sem::Sampler>());
  ASSERT_FALSE(t->As<sem::Sampler>()->IsComparison());
}

TEST_F(ParserImplTest, TextureSamplerTypes_SamplerComparison) {
  auto p = parser("sampler_comparison");
  auto t = p->texture_sampler_types();
  ASSERT_FALSE(p->has_error()) << p->error();
  EXPECT_TRUE(t.matched);
  EXPECT_FALSE(t.errored);
  ASSERT_NE(t.value, nullptr);
  ASSERT_TRUE(t->Is<sem::Sampler>());
  ASSERT_TRUE(t->As<sem::Sampler>()->IsComparison());
}

TEST_F(ParserImplTest, TextureSamplerTypes_DepthTexture) {
  auto p = parser("texture_depth_2d");
  auto t = p->texture_sampler_types();
  ASSERT_FALSE(p->has_error()) << p->error();
  EXPECT_TRUE(t.matched);
  EXPECT_FALSE(t.errored);
  ASSERT_NE(t.value, nullptr);
  ASSERT_TRUE(t->Is<sem::Texture>());
  ASSERT_TRUE(t->Is<sem::DepthTexture>());
  EXPECT_EQ(t->As<sem::Texture>()->dim(), sem::TextureDimension::k2d);
}

TEST_F(ParserImplTest, TextureSamplerTypes_SampledTexture_F32) {
  auto p = parser("texture_1d<f32>");
  auto t = p->texture_sampler_types();
  ASSERT_FALSE(p->has_error()) << p->error();
  EXPECT_TRUE(t.matched);
  EXPECT_FALSE(t.errored);
  ASSERT_NE(t.value, nullptr);
  ASSERT_TRUE(t->Is<sem::Texture>());
  ASSERT_TRUE(t->Is<sem::SampledTexture>());
  ASSERT_TRUE(t->As<sem::SampledTexture>()->type()->Is<sem::F32>());
  EXPECT_EQ(t->As<sem::Texture>()->dim(), sem::TextureDimension::k1d);
}

TEST_F(ParserImplTest, TextureSamplerTypes_SampledTexture_I32) {
  auto p = parser("texture_2d<i32>");
  auto t = p->texture_sampler_types();
  ASSERT_FALSE(p->has_error()) << p->error();
  EXPECT_TRUE(t.matched);
  EXPECT_FALSE(t.errored);
  ASSERT_NE(t.value, nullptr);
  ASSERT_TRUE(t->Is<sem::Texture>());
  ASSERT_TRUE(t->Is<sem::SampledTexture>());
  ASSERT_TRUE(t->As<sem::SampledTexture>()->type()->Is<sem::I32>());
  EXPECT_EQ(t->As<sem::Texture>()->dim(), sem::TextureDimension::k2d);
}

TEST_F(ParserImplTest, TextureSamplerTypes_SampledTexture_U32) {
  auto p = parser("texture_3d<u32>");
  auto t = p->texture_sampler_types();
  ASSERT_FALSE(p->has_error()) << p->error();
  EXPECT_TRUE(t.matched);
  EXPECT_FALSE(t.errored);
  ASSERT_NE(t.value, nullptr);
  ASSERT_TRUE(t->Is<sem::Texture>());
  ASSERT_TRUE(t->Is<sem::SampledTexture>());
  ASSERT_TRUE(t->As<sem::SampledTexture>()->type()->Is<sem::U32>());
  EXPECT_EQ(t->As<sem::Texture>()->dim(), sem::TextureDimension::k3d);
}

TEST_F(ParserImplTest, TextureSamplerTypes_SampledTexture_Invalid) {
  auto p = parser("texture_1d<abc>");
  auto t = p->texture_sampler_types();
  ASSERT_TRUE(p->has_error());
  EXPECT_EQ(t.value, nullptr);
  EXPECT_FALSE(t.matched);
  EXPECT_TRUE(t.errored);
  EXPECT_EQ(p->error(), "1:12: unknown constructed type 'abc'");
}

TEST_F(ParserImplTest, TextureSamplerTypes_SampledTexture_MissingType) {
  auto p = parser("texture_1d<>");
  auto t = p->texture_sampler_types();
  ASSERT_TRUE(p->has_error());
  EXPECT_EQ(t.value, nullptr);
  EXPECT_FALSE(t.matched);
  EXPECT_TRUE(t.errored);
  EXPECT_EQ(p->error(), "1:12: invalid type for sampled texture type");
}

TEST_F(ParserImplTest, TextureSamplerTypes_SampledTexture_MissingLessThan) {
  auto p = parser("texture_1d");
  auto t = p->texture_sampler_types();
  ASSERT_TRUE(p->has_error());
  EXPECT_EQ(t.value, nullptr);
  EXPECT_FALSE(t.matched);
  EXPECT_TRUE(t.errored);
  EXPECT_EQ(p->error(), "1:11: expected '<' for sampled texture type");
}

TEST_F(ParserImplTest, TextureSamplerTypes_SampledTexture_MissingGreaterThan) {
  auto p = parser("texture_1d<u32");
  auto t = p->texture_sampler_types();
  ASSERT_TRUE(p->has_error());
  EXPECT_EQ(t.value, nullptr);
  EXPECT_FALSE(t.matched);
  EXPECT_TRUE(t.errored);
  EXPECT_EQ(p->error(), "1:15: expected '>' for sampled texture type");
}

TEST_F(ParserImplTest, TextureSamplerTypes_MultisampledTexture_I32) {
  auto p = parser("texture_multisampled_2d<i32>");
  auto t = p->texture_sampler_types();
  ASSERT_FALSE(p->has_error()) << p->error();
  EXPECT_TRUE(t.matched);
  EXPECT_FALSE(t.errored);
  ASSERT_NE(t.value, nullptr);
  ASSERT_TRUE(t->Is<sem::Texture>());
  ASSERT_TRUE(t->Is<sem::MultisampledTexture>());
  ASSERT_TRUE(t->As<sem::MultisampledTexture>()->type()->Is<sem::I32>());
  EXPECT_EQ(t->As<sem::Texture>()->dim(), sem::TextureDimension::k2d);
}

TEST_F(ParserImplTest, TextureSamplerTypes_MultisampledTexture_Invalid) {
  auto p = parser("texture_multisampled_2d<abc>");
  auto t = p->texture_sampler_types();
  ASSERT_TRUE(p->has_error());
  EXPECT_EQ(t.value, nullptr);
  EXPECT_FALSE(t.matched);
  EXPECT_TRUE(t.errored);
  EXPECT_EQ(p->error(), "1:25: unknown constructed type 'abc'");
}

TEST_F(ParserImplTest, TextureSamplerTypes_MultisampledTexture_MissingType) {
  auto p = parser("texture_multisampled_2d<>");
  auto t = p->texture_sampler_types();
  ASSERT_TRUE(p->has_error());
  EXPECT_EQ(t.value, nullptr);
  EXPECT_FALSE(t.matched);
  EXPECT_TRUE(t.errored);
  EXPECT_EQ(p->error(), "1:25: invalid type for multisampled texture type");
}

TEST_F(ParserImplTest,
       TextureSamplerTypes_MultisampledTexture_MissingLessThan) {
  auto p = parser("texture_multisampled_2d");
  auto t = p->texture_sampler_types();
  EXPECT_EQ(t.value, nullptr);
  EXPECT_FALSE(t.matched);
  EXPECT_TRUE(t.errored);
  EXPECT_EQ(p->error(), "1:24: expected '<' for multisampled texture type");
}

TEST_F(ParserImplTest,
       TextureSamplerTypes_MultisampledTexture_MissingGreaterThan) {
  auto p = parser("texture_multisampled_2d<u32");
  auto t = p->texture_sampler_types();
  EXPECT_EQ(t.value, nullptr);
  EXPECT_FALSE(t.matched);
  EXPECT_TRUE(t.errored);
  EXPECT_EQ(p->error(), "1:28: expected '>' for multisampled texture type");
}

TEST_F(ParserImplTest, TextureSamplerTypes_StorageTexture_Readonly1dR8Unorm) {
  auto p = parser("texture_storage_1d<r8unorm>");
  auto t = p->texture_sampler_types();
  ASSERT_FALSE(p->has_error()) << p->error();
  EXPECT_TRUE(t.matched);
  EXPECT_FALSE(t.errored);
  ASSERT_NE(t.value, nullptr);

  ASSERT_TRUE(t->Is<sem::Texture>());
  ASSERT_TRUE(t->Is<sem::StorageTexture>());
  EXPECT_EQ(t->As<sem::StorageTexture>()->image_format(),
            sem::ImageFormat::kR8Unorm);
  EXPECT_EQ(t->As<sem::Texture>()->dim(), sem::TextureDimension::k1d);
}

TEST_F(ParserImplTest, TextureSamplerTypes_StorageTexture_Writeonly2dR16Float) {
  auto p = parser("texture_storage_2d<r16float>");
  auto t = p->texture_sampler_types();
  ASSERT_FALSE(p->has_error()) << p->error();
  EXPECT_TRUE(t.matched);
  EXPECT_FALSE(t.errored);
  ASSERT_NE(t.value, nullptr);

  ASSERT_TRUE(t->Is<sem::Texture>());
  ASSERT_TRUE(t->Is<sem::StorageTexture>());
  EXPECT_EQ(t->As<sem::StorageTexture>()->image_format(),
            sem::ImageFormat::kR16Float);
  EXPECT_EQ(t->As<sem::Texture>()->dim(), sem::TextureDimension::k2d);
}

TEST_F(ParserImplTest, TextureSamplerTypes_StorageTexture_InvalidType) {
  auto p = parser("texture_storage_1d<abc>");
  auto t = p->texture_sampler_types();
  EXPECT_EQ(t.value, nullptr);
  EXPECT_FALSE(t.matched);
  EXPECT_TRUE(t.errored);
  EXPECT_EQ(p->error(), "1:20: invalid format for storage texture type");
}

TEST_F(ParserImplTest, TextureSamplerTypes_StorageTexture_MissingType) {
  auto p = parser("texture_storage_1d<>");
  auto t = p->texture_sampler_types();
  EXPECT_EQ(t.value, nullptr);
  EXPECT_FALSE(t.matched);
  EXPECT_TRUE(t.errored);
  EXPECT_EQ(p->error(), "1:20: invalid format for storage texture type");
}

TEST_F(ParserImplTest, TextureSamplerTypes_StorageTexture_MissingLessThan) {
  auto p = parser("texture_storage_1d");
  auto t = p->texture_sampler_types();
  EXPECT_EQ(t.value, nullptr);
  EXPECT_FALSE(t.matched);
  EXPECT_TRUE(t.errored);
  EXPECT_EQ(p->error(), "1:19: expected '<' for storage texture type");
}

TEST_F(ParserImplTest, TextureSamplerTypes_StorageTexture_MissingGreaterThan) {
  auto p = parser("texture_storage_1d<r8unorm");
  auto t = p->texture_sampler_types();
  EXPECT_EQ(t.value, nullptr);
  EXPECT_FALSE(t.matched);
  EXPECT_TRUE(t.errored);
  EXPECT_EQ(p->error(), "1:27: expected '>' for storage texture type");
}
}  // namespace
}  // namespace wgsl
}  // namespace reader
}  // namespace tint

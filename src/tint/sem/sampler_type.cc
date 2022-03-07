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

#include "src/tint/sem/sampler_type.h"

#include "src/tint/program_builder.h"
#include "src/tint/utils/hash.h"

TINT_INSTANTIATE_TYPEINFO(tint::sem::Sampler);

namespace tint {
namespace sem {

Sampler::Sampler(ast::SamplerKind kind) : kind_(kind) {}

Sampler::Sampler(Sampler&&) = default;

Sampler::~Sampler() = default;

size_t Sampler::Hash() const {
  return utils::Hash(TypeInfo::Of<Sampler>().full_hashcode, kind_);
}

bool Sampler::Equals(const sem::Type& other) const {
  if (auto* o = other.As<Sampler>()) {
    return o->kind_ == kind_;
  }
  return false;
}

std::string Sampler::type_name() const {
  return std::string("__sampler_") +
         (kind_ == ast::SamplerKind::kSampler ? "sampler" : "comparison");
}

std::string Sampler::FriendlyName(const SymbolTable&) const {
  return kind_ == ast::SamplerKind::kSampler ? "sampler" : "sampler_comparison";
}

}  // namespace sem
}  // namespace tint

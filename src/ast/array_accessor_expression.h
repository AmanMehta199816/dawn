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

#ifndef SRC_AST_ARRAY_ACCESSOR_EXPRESSION_H_
#define SRC_AST_ARRAY_ACCESSOR_EXPRESSION_H_

#include "src/ast/expression.h"

namespace tint {
namespace ast {

/// An array accessor expression
class ArrayAccessorExpression
    : public Castable<ArrayAccessorExpression, Expression> {
 public:
  /// Constructor
  /// @param source the array accessor source
  /// @param array the array
  /// @param idx_expr the index expression
  ArrayAccessorExpression(const Source& source,
                          Expression* array,
                          Expression* idx_expr);
  /// Move constructor
  ArrayAccessorExpression(ArrayAccessorExpression&&);
  ~ArrayAccessorExpression() override;

  /// @returns the array
  Expression* array() const { return array_; }

  /// @returns the index expression
  Expression* idx_expr() const { return idx_expr_; }

  /// Clones this node and all transitive child nodes using the `CloneContext`
  /// `ctx`.
  /// @param ctx the clone context
  /// @return the newly cloned node
  ArrayAccessorExpression* Clone(CloneContext* ctx) const override;

  /// Writes a representation of the node to the output stream
  /// @param sem the semantic info for the program
  /// @param out the stream to write to
  /// @param indent number of spaces to indent the node when writing
  void to_str(const semantic::Info& sem,
              std::ostream& out,
              size_t indent) const override;

 private:
  ArrayAccessorExpression(const ArrayAccessorExpression&) = delete;

  Expression* const array_;
  Expression* const idx_expr_;
};

}  // namespace ast
}  // namespace tint

#endif  // SRC_AST_ARRAY_ACCESSOR_EXPRESSION_H_

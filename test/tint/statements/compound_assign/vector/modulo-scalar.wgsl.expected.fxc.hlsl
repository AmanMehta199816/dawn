[numthreads(1, 1, 1)]
void unused_entry_point() {
  return;
}

RWByteAddressBuffer v : register(u0, space0);

int4 tint_mod(int4 lhs, int rhs) {
  const int4 r = int4((rhs).xxxx);
  const int4 rhs_or_one = (((r == (0).xxxx) | ((lhs == (-2147483648).xxxx) & (r == (-1).xxxx))) ? (1).xxxx : r);
  if (any(((uint4((lhs | rhs_or_one)) & (2147483648u).xxxx) != (0u).xxxx))) {
    return (lhs - ((lhs / rhs_or_one) * rhs_or_one));
  } else {
    return (lhs % rhs_or_one);
  }
}

void foo() {
  const int4 tint_symbol = tint_mod(asint(v.Load4(0u)), 2);
  v.Store4(0u, asuint(tint_symbol));
}

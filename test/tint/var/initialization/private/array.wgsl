var<private> v : array<i32, 3>;

@compute @workgroup_size(1)
fn main() {
    _ = v;
}

fn ceil_96f597() {
  var res : vec2<f32> = ceil(vec2<f32>());
}

@vertex
fn vertex_main() -> @builtin(position) vec4<f32> {
  ceil_96f597();
  return vec4<f32>();
}

@fragment
fn fragment_main() {
  ceil_96f597();
}

@compute @workgroup_size(1)
fn compute_main() {
  ceil_96f597();
}

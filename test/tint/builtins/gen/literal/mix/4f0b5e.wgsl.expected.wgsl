fn mix_4f0b5e() {
  var res : f32 = mix(1.0, 1.0, 1.0);
}

@vertex
fn vertex_main() -> @builtin(position) vec4<f32> {
  mix_4f0b5e();
  return vec4<f32>();
}

@fragment
fn fragment_main() {
  mix_4f0b5e();
}

@compute @workgroup_size(1)
fn compute_main() {
  mix_4f0b5e();
}

fn abs_1ce782() {
  var arg_0 = vec4<u32>();
  var res : vec4<u32> = abs(arg_0);
}

@vertex
fn vertex_main() -> @builtin(position) vec4<f32> {
  abs_1ce782();
  return vec4<f32>();
}

@fragment
fn fragment_main() {
  abs_1ce782();
}

@compute @workgroup_size(1)
fn compute_main() {
  abs_1ce782();
}

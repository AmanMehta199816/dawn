@group(1) @binding(0) var arg_0 : texture_3d<u32>;

fn textureNumLevels_b4f5ea() {
  var res : i32 = textureNumLevels(arg_0);
}

@vertex
fn vertex_main() -> @builtin(position) vec4<f32> {
  textureNumLevels_b4f5ea();
  return vec4<f32>();
}

@fragment
fn fragment_main() {
  textureNumLevels_b4f5ea();
}

@compute @workgroup_size(1)
fn compute_main() {
  textureNumLevels_b4f5ea();
}

@group(1) @binding(0) var arg_0 : texture_2d<f32>;

@group(1) @binding(1) var arg_1 : sampler;

fn textureSampleBias_6a9113() {
  var res : vec4<f32> = textureSampleBias(arg_0, arg_1, vec2<f32>(), 1.0);
}

@fragment
fn fragment_main() {
  textureSampleBias_6a9113();
}

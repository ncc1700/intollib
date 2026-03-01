struct FragmentData {
    color: vec4<f32>
}

struct VertexData {
    scale: f32,
    position: vec4<f32>
}

struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) color: vec4<f32>
}

@group(0) @binding(0)
var<uniform> fragData: FragmentData;


@group(0) @binding(1)
var<uniform> verData: VertexData;



@vertex
fn VertexShaderMain(@builtin(vertex_index) i: u32) -> VertexOutput {
    let scale = verData.scale;
    let rectArr = array<vec2<f32>, 6> (
        vec2<f32>(-verData.position.x * scale, -verData.position.y * scale),
        vec2<f32>(-verData.position.x * scale, verData.position.y  * scale),
        vec2<f32>(verData.position.x * scale, -verData.position.y  * scale),

        vec2<f32>(verData.position.x * scale, verData.position.y  * scale),
        vec2<f32>(verData.position.x * scale, -verData.position.y  * scale),
        vec2<f32>(-verData.position.x * scale, verData.position.y  * scale),
    );
    var out: VertexOutput;
    out.position = vec4<f32>(rectArr[i], 1.0, 1.0);
    out.color = vec4<f32>(rectArr[i], 0.0, 1.0) ;
    return out;
}

@fragment
fn FragmentShaderMain(@location(0) color: vec4<f32>) -> @location(0) vec4<f32> {
    return color;
}
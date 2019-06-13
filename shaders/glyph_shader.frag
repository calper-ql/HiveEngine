#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec2 frag_uv;
layout(location = 1) in vec4 frag_color;

layout(location = 0) out vec4 outColor;

uniform sampler2D texture1;

void main() {
    if(texture(texture1, frag_uv).r == 0) discard;
    outColor = frag_color;
}
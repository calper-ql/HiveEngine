#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) in vec2 frag_uv;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler2D texSampler;

void main() {
        if(texture(texSampler, frag_uv).r == 0) discard;
        outColor = vec4(1.0, 1.0, 1.0, 1.0);

}
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : require

layout(set = 0, binding = 0) buffer StateBuffer {
    int state[];
} state_storage;

layout(location = 0) in vec4 fragColor;
layout(location = 0) out vec4 outColor;

void main() {
    // gl_PrimitiveID
    if(state_storage.state[gl_PrimitiveID] == 0) discard;
    outColor = fragColor;
}
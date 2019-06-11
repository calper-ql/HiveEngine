#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : require

layout(set = 0, binding = 1) buffer ImageDescriptionBuffer {
    int data[];
} description_storage;

layout(set = 0, binding = 3) buffer ImageColorBuffer {
    vec4 data[];
} color_storage;


layout(location = 0) in vec2 frag_uv;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 2) uniform sampler2D texSampler;

void main() {
    int texture_index = description_storage.data[gl_PrimitiveID];

    if(texture_index < 0) {
        discard;
    } else {
        if(texture(texSampler, frag_uv).r == 0) discard;
        outColor = color_storage.data[gl_PrimitiveID];;
    }

}
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : require

struct ImageDescription {
    int texture_index;
};

layout(set = 0, binding = 1) buffer ImageDescriptionBuffer {
    ImageDescription data[];
} storage;

layout(location = 0) in vec2 frag_uv;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 2) uniform sampler2D texSampler;

void main() {
    int texture_index = storage.data[gl_PrimitiveID].texture_index;

    if(texture_index < 0) {
        discard;
    } else {
        outColor = texture(texSampler, frag_uv);
    }

}
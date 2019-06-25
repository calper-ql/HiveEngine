#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

layout(location = 0) out vec4 fragColor;

uniform mat4 view;
uniform int view_apply;

void main() {
    if(view_apply == 1) gl_Position = view * (vec4(position, 1.0));
    else gl_Position = (vec4(position, 1.0));
    fragColor = color;
}
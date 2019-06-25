#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 position;
layout(location = 1) in dvec3 min;
layout(location = 2) in dvec3 max;
layout(location = 3) in int significance;

layout(location = 0) out vec4 fragColor;

uniform mat4 view;
uniform int view_apply;
uniform dvec3 offset;

void main() {
    vec3 p = position;
    if(position.x < 0.5){
        p.x = float(min.x);
    } else {
        p.x = float(max.x);
    }

    if(position.y < 0.5){
        p.y = float(min.y);
    } else {
        p.y = float(max.y);
    }

    if(position.z < 0.5){
        p.z = float(min.z);
    } else {
        p.z = float(max.z);
    }

    p += vec3(offset);

    if(view_apply == 1) gl_Position = view * (vec4(p, 1.0));
    else gl_Position = (vec4(p, 1.0));

    if(significance == 0){
        fragColor = vec4(1.0, 1.0, 1.0, 1.0);
    } else if (significance == 1) {
        fragColor = vec4(1.0, 0.0, 0.0, 1.0);
    } else if (significance == 2) {
        fragColor = vec4(0.0, 1.0, 0.0, 1.0);
    } else {
        fragColor = vec4(0.0, 0.0, 1.0, 1.0);
    }

}
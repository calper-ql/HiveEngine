#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vertex;
layout(location = 1) in dvec3 pos;
layout(location = 2) in double radius;
layout(location = 3) in int significance;

layout(location = 0) out vec4 fragColor;

uniform mat4 view;
uniform int view_apply;
uniform dvec3 offset;

void main() {
    vec3 p = vertex * float(radius);
    p += vec3(pos);
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
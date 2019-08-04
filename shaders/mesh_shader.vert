#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vertex;
layout(location = 1) in mat3 rotation;
layout(location = 4) in vec3 position;
layout(location = 5) in vec3 scale;

layout(set=0, binding = 0) uniform CameraPackage {
    mat4 view;
    int apply;
    float fov;
    mat3 view_rot;
    vec3 pos;
} camera;

layout(location = 0) out vec4 fragColor;

void main() {
    vec3 p = position + rotation * (vertex * scale);

    if(camera.apply == 1) gl_Position = camera.view * (vec4(p, 1.0));
    else gl_Position = (vec4(p, 1.0));

    fragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
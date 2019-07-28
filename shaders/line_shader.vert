#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set=0, binding = 0) uniform CameraPackage {
    mat4 view;
    int apply;
} camera;

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

layout(location = 0) out vec4 fragColor;

void main() {
    if(camera.apply == 1){
        gl_Position = (camera.view * vec4(position, 1.0));
    } else {
        gl_Position = (vec4(position, 1.0));
    }
    
    fragColor = color;
}
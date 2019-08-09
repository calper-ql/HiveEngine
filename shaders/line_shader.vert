#version 450

layout(set=0, binding = 0) uniform CameraPackage {
    dmat4 view;
    int apply;
    float fov;
    dmat3 view_rot;
    dvec3 pos;
    double near;
    double far;
} camera;

layout(location = 0) in dvec3 position;
layout(location = 2) in vec4 color;

layout(location = 0) out vec4 fragColor;

void main() {
    if(camera.apply == 1){
        gl_Position = vec4(camera.view * dvec4(position, 1.0));
    } else {
        gl_Position = vec4(dvec4(position, 1.0));
    }
    
    fragColor = color;
}
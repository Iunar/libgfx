#version 460 core
layout(location = 0) in vec3 attrib_position;

void main() {
    gl_Position = vec4(attrib_position, 1.0);
}
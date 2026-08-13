#version 440

layout(location = 0) in vec2 position;
layout(location = 0) out vec2 uv;

layout(std140, binding = 0) uniform buf {
    float threshold;
    float yFlip;
};

void main()
{
    uv = position * 0.5 + vec2(0.5);
    gl_Position = vec4(position.x, position.y * yFlip, 0.0, 1.0);
}

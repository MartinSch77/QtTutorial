#version 440

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf {
    float threshold;
    float yFlip;
};

void main()
{
    vec3 colorA = vec3(0.15, 0.55, 0.95);
    vec3 colorB = vec3(0.95, 0.35, 0.15);
    float mixAmount = step(threshold, uv.x);
    fragColor = vec4(mix(colorA, colorB, mixAmount), 0.85);
}

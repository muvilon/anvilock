#version 300 es
precision highp float;

// Input attributes
in vec2 position;

// Uniforms
uniform vec2 offset;
uniform vec2 scale;
uniform float time;

// Output to fragment shader
out vec2 fragCoord;
out vec2 localPos;

void main() {
    // Apply scale and offset transformations
    vec2 scaledPos = position * scale;
    vec2 worldPos = scaledPos + offset;
    
    // Pass local coordinates to fragment shader for rounded corners
    localPos = position;
    fragCoord = worldPos;
    
    // Convert to clip space
    gl_Position = vec4(worldPos, 0.0, 1.0);
}

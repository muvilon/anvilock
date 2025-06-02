#version 300 es
precision highp float;

// Input from vertex shader
in vec2 fragCoord;
in vec2 localPos;

// Uniforms
uniform vec4 color;
uniform vec2 offset;
uniform vec2 scale;
uniform float cornerRadius;
uniform float time;

// Output
out vec4 fragColor;

// Smooth distance function for rounded rectangle
float roundedRectSDF(vec2 pos, vec2 size, float radius) {
    vec2 d = abs(pos) - size + radius;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - radius;
}

// Smooth step function for anti-aliasing
float smoothAlpha(float dist, float width) {
    return 1.0 - smoothstep(-width, width, dist);
}

// Generate subtle noise for texture
float noise(vec2 pos) {
    return fract(sin(dot(pos, vec2(12.9898, 78.233))) * 43758.5453123);
}

// Create subtle gradient effect
vec3 createGradient(vec2 pos, vec3 baseColor) {
    float gradient = 0.5 + 0.5 * (pos.y + pos.x * 0.3);
    return mix(baseColor * 0.95, baseColor * 1.05, gradient);
}

void main() {
    // Calculate field dimensions (assuming normalized coordinates)
    vec2 fieldSize = vec2(0.3, 0.06); // Half-width, half-height
    
    // Calculate distance to rounded rectangle
    float dist = roundedRectSDF(localPos, fieldSize, cornerRadius);
    
    // Anti-aliased edge
    float alpha = smoothAlpha(dist, 0.002);
    
    // Early discard for performance
    if (alpha < 0.001) {
        discard;
    }
    
    // Base color with subtle enhancements
    vec3 finalColor = color.rgb;
    
    // Add subtle gradient based on position
    finalColor = createGradient(localPos, finalColor);
    
    // Add very subtle noise texture for depth
    float noiseValue = noise(fragCoord * 800.0 + time * 0.1) * 0.02;
    finalColor += noiseValue;
    
    // Subtle rim lighting effect for depth
    float rimIntensity = 1.0 - smoothstep(0.0, cornerRadius * 0.5, abs(dist));
    finalColor += rimIntensity * 0.03;
    
    // Time-based subtle shimmer effect (very subtle)
    float shimmer = sin(time * 2.0 + localPos.x * 10.0) * 0.01;
    finalColor += shimmer;
    
    // Apply final alpha
    fragColor = vec4(finalColor, color.a * alpha);
}

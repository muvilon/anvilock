#version 300 es
precision highp float;

in vec2 localPos;

uniform vec4 color;
uniform float cornerRadius;
uniform float time;
uniform vec2 halfSize;

out vec4 fragColor;

float roundedRectSDF(vec2 pos, vec2 size, float radius) {
    vec2 d = abs(pos) - size + radius;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - radius;
}

float smoothAlpha(float dist, float width) {
    return 1.0 - smoothstep(-width, width, dist);
}

void main() {
    // The shader assumes the scale-adjusted quad has been centered at the origin,
    // and `localPos` is in that space.
    float dist = roundedRectSDF(localPos, halfSize, cornerRadius);

    float alpha = smoothAlpha(dist, 0.002); // feathered edge
    if (alpha < 0.001) discard;

    fragColor = vec4(color.rgb, color.a * alpha);
}

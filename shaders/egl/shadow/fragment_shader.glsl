// shadow.frag
#version 300 es
precision highp float;

in vec2 fragCoord;
uniform vec2 resolution;
uniform vec2 shadowOffset;
uniform float blurRadius;
uniform vec4 shadowColor;

out vec4 fragColor;

float gaussian(float x, float sigma) {
    return exp(-(x * x) / (2.0 * sigma * sigma));
}

void main() {
    vec2 uv = fragCoord / resolution;
    float alpha = 0.0;

    for (int x = -8; x <= 8; ++x) {
        for (int y = -8; y <= 8; ++y) {
            vec2 offset = vec2(x, y);
            float w = gaussian(length(offset), blurRadius);
            alpha += w;
        }
    }

    alpha = clamp(alpha / 100.0, 0.0, 1.0);
    fragColor = vec4(shadowColor.rgb, alpha * shadowColor.a);
}

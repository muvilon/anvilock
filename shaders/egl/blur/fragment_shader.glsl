#version 300 es
precision mediump float;

in vec2 vTexCoord;
uniform sampler2D uTex;
uniform vec2 uDirection; // (1.0, 0.0) for horizontal, (0.0, 1.0) for vertical
uniform float uSigma;
uniform float uRadius;
uniform vec2 resolution;

out vec4 fragColor;

float gaussian(float x, float sigma) {
    return exp(- (x * x) / (2.0 * sigma * sigma)) / (sqrt(6.283185 * sigma * sigma));
}

void main() {
    vec2 texelSize = 1.0 / resolution;
    vec4 result = vec4(0.0);
    float sum = 0.0;

    for (float i = -uRadius; i <= uRadius; i++) {
        float weight = gaussian(i, uSigma);
        vec2 offset = uDirection * i * texelSize;
        result += texture(uTex, vTexCoord + offset) * weight;
        sum += weight;
    }

    fragColor = result / sum;
}

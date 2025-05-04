#version 320 es
precision mediump float;

in vec2 texcoord;
out vec4 frag_color;

uniform sampler2D text_texture;
uniform float time;  // Pass system time for animated effects
uniform vec4 color;  // Base color for the text box

void main()
{
    vec4 text_sample = texture(text_texture, texcoord);

    // Apply a soft glow effect based on distance from the center
    float dist = length(texcoord - vec2(0.5, 0.5));
    float glow = exp(-10.0 * dist) * 0.5;  // Ensure no precision issues

    // Slight pulsation effect based on time
    float pulsate = 0.5 + 0.5 * sin(time * 2.0);

    // Blend texture with color and glow
    frag_color = text_sample * color;
    frag_color.rgb += glow * pulsate;
    frag_color.a *= text_sample.a;  // Preserve text transparency
}

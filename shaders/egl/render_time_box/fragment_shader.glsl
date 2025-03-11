#version 300 es
precision mediump float;

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D textTexture;
uniform vec4 color; // Text color

void main()
{
    float alpha = texture(textTexture, TexCoord).r;
    FragColor = vec4(color.rgb, alpha * color.a);
}

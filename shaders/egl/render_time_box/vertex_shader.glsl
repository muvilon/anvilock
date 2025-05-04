#version 320 es
precision mediump float;

layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec2 in_texcoord;

out vec2 texcoord;

uniform mat4 projection;  // Projection matrix for proper positioning

void main()
{
    texcoord = in_texcoord;
    gl_Position = projection * vec4(in_pos, 0.0, 1.0);
}

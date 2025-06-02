#version 300 es

precision mediump float;
varying vec2 vTexCoord;
uniform sampler2D uTexture;
void main() {
 gl_FragColor = texture2D(uTexture, vTexCoord);
}

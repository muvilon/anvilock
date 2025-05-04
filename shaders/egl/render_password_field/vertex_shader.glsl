attribute vec2 position;
uniform vec2 offset;
void main() {
    vec2 pos = position + offset;
    gl_Position = vec4(pos, 0.0, 1.0);
}

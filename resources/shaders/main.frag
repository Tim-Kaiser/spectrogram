#version 460

uniform sampler2D u_spectrogramTex;

in vec2 vUv;
in vec3 vertexOut;

out vec4 fragCol;

void main(){
	fragCol = texture(u_spectrogramTex, vUv);
}
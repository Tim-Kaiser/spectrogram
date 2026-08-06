#version 460

uniform sampler2D u_spectrogramTex;

in vec2 vUv;
in vec3 vertexOut;

out vec4 fragCol;

vec3 kernel(vec2 p){
	vec3 col = texture(u_spectrogramTex, vUv).xyz * 4.;

	col += texture(u_spectrogramTex, vec2(p.x - 1, p.y)).xyz * 2.;
	col += texture(u_spectrogramTex, vec2(p.x + 1, p.y)).xyz * 2.;
	col += texture(u_spectrogramTex, vec2(p.x, p.y - 1)).xyz * 2.;
	col += texture(u_spectrogramTex, vec2(p.x, p.y + 1)).xyz * 2.;

	col += texture(u_spectrogramTex, vec2(p.x + 1, p.y + 1)).xyz * 1.;
	col += texture(u_spectrogramTex, vec2(p.x - 1, p.y + 1)).xyz * 1.;
	col += texture(u_spectrogramTex, vec2(p.x - 1, p.y - 1)).xyz * 1.;
	col += texture(u_spectrogramTex, vec2(p.x + 1, p.y - 1)).xyz * 1.;

	return col / 16.0;
}

void main(){
	fragCol = vec4(texture(u_spectrogramTex, vUv).xyz, 1.0);
}
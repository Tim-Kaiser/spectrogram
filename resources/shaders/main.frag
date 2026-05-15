#version 460

layout(std430, binding=0) buffer audioData
{
	float samples[];
};
uniform int u_time;

in vec2 vUv;
in vec3 vertexOut;


out vec4 fragCol;

void main(){
	fragCol = vec4(vUv, 0., 1.);
}
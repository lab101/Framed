#version 330


uniform mat4 adjMatrix;
uniform mat4 ciModelViewProjection;
uniform mat4 ciModelView;
uniform mat3 ciNormalMatrix;

in vec4 ciPosition;
in vec2	ciTexCoord0;
in vec3	ciNormal;

out highp vec3 vPosition;
out highp vec3 vOrigPosition;
out highp vec3 vViewPosition;
out highp vec3 vNormal;
out highp vec2 vTexCoord0;

uniform float uTime;
uniform float uPulseTime;
uniform float uViewTime;

uniform vec3 uColors[5];


void main() {


	vTexCoord0	= ciTexCoord0;
	vec3 adjNormal =normalize(mat3(adjMatrix)* ciNormal);
	vNormal		= ciNormalMatrix *adjNormal;

	vec3 pos =(adjMatrix*ciPosition).xyz;
	vOrigPosition = ciPosition.xyz;

	pos.z += (sin(uTime* .4 + (pos.y * 2.8)) *.02);
	pos.y += (sin(uTime * .4 + (pos.z * 2.4)) *.02);
	//pos.y+= sin(uTime)*0.07;
	vPosition=pos;
	vViewPosition = (ciModelView * vec4(pos, 1.0)).xyz;


}

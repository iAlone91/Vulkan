#version 450
// In

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

layout( set = 0, binding = 0) uniform GlobalUbo
{
	mat4 proj;
} ubo;

layout(push_constant) uniform Push 
{
	mat4 transform;
} push;

// Out
layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoord;


void main()
{
	gl_Position = ubo.proj * push.transform * vec4( inPosition, 1.0);
	outColor = inColor;
	outTexCoord = inTexCoord;
}
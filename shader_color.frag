#version 430 core

uniform vec3 objectColor;
uniform vec3 lightDir;
uniform vec3 lightDir1;

in vec3 interpNormal;

void main()
{
	vec3 normal = normalize(interpNormal);
	float diffuse = max(dot(normal, -lightDir), 0.0);
	float diffuse1 = max(dot(normal, -lightDir1), 0.0);
	//float sam = diffuse1 * diffuse;
	//gl_FragColor = vec4(objectColor * diffuse1, 1.0);
	gl_FragColor = vec4(objectColor * diffuse1*0.50f, 1.0) + vec4(objectColor * diffuse*0.5f, 1.0);
}
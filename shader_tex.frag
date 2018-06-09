#version 430 core

uniform sampler2D textureSampler;
uniform vec3 lightDir;
uniform vec3 lightDir1;

in vec3 interpNormal;
in vec2 interpTexCoord;

void main()
{
	vec2 modifiedTexCoord = vec2(interpTexCoord.x, 1.0 - interpTexCoord.y); // Poprawka dla tekstur Ziemi, ktore bez tego wyswietlaja sie 'do gory nogami'
	vec3 color = texture2D(textureSampler, modifiedTexCoord).rgb;
	vec3 normal = normalize(interpNormal);
	float diffuse = max(dot(normal, -lightDir), 0.0);
	float diffuse1 = max(dot(normal, -lightDir1), 0.0);
	//float sam = diffuse * diffuse1;
	//gl_FragColor = vec4(color * diffuse1, 1.0);
	gl_FragColor = vec4(color * diffuse1*0.5f , 1.0) + vec4(color * diffuse*0.55f, 1.0);
}

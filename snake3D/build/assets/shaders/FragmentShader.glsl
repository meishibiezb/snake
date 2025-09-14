#version 330 core
out vec4 FragColor;

// in vec3 ourColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;

uniform int type;

void main()
{
	switch (type){
		case 1:
			FragColor = texture(texture1, TexCoord);
			break;
		case 2:
			FragColor = texture(texture2, TexCoord);
			break;
		case 3:
			FragColor = texture(texture3, TexCoord);
			break;
		case 4:
			FragColor = texture(texture4, TexCoord);
			break;
		default:
			FragColor = texture(texture1, TexCoord);
	}
}
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

const vec3 green = vec3(0.2, 1.0, 0.2);
const vec3 black = vec3(0.0, 0.0, 0.0);

void main()
{
    vec3 diffuse = texture(texture_diffuse1, TexCoords).rgb;
    vec3 specular = texture(texture_specular1, TexCoords).rgb;
    vec3 color = mix(diffuse, green, 0.32);
    if (specular.g > 0.2)
    {
        color = mix(color, black, 0.2);
    }
    FragColor = vec4(color, 0.72);
}

#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

void main()
{
    vec3 diffuse = texture(texture_diffuse1, TexCoords).rgb;
    vec3 specular = texture(texture_specular1, TexCoords).rgb;
    vec3 normal = texture(texture_normal1, TexCoords).rgb;
    // FragColor = vec4(mix(diffuse, specular, 0.2), 1.0);
    FragColor = vec4(diffuse, 1.0);
}

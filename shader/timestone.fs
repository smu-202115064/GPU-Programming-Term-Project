#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{
    vec3 diffuse = texture(texture_diffuse1, TexCoords).rgb;
    vec3 ambient = vec3(0.2, 1.0, 0.2);
    vec3 color = mix(diffuse, ambient, 0.2);
    float alpha = 0.72;
    FragColor = vec4(color, alpha);
}

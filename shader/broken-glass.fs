#version 330 core

in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 cameraPos;
uniform samplerCube environmentMap;
uniform sampler2D brokenGlassTexture;

void main()
{
    float eta = 1.52; // glass refractive index
    float ratio = 1.00 / eta;
    vec3 normal = normalize(texture(brokenGlassTexture, TexCoords).rgb * 2.0 - 1.0);

    vec3 I = normalize(FragPos - cameraPos);
    vec3 R = refract(I, normal, ratio);

    vec3 refractedColor = texture(environmentMap, R).rgb;
    FragColor = vec4(refractedColor, 1.0);
}

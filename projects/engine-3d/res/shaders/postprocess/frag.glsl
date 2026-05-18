#version 330 core
out vec4 FragColor;
in vec2 v_TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform float exposure;

void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(scene, v_TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, v_TexCoords).rgb;

    // On ajoute le Bloom à la scène !
    hdrColor += bloomColor;

    // Tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

    // Gamma correct
    result = pow(result, vec3(1.0 / gamma));

    FragColor = vec4(result, 1.0);
}

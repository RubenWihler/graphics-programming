#version 330 core
out vec4 FragColor;

in vec3 v_localPos;

uniform sampler2D u_equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
    // Transforme le vecteur 3D en coordonnées UV 2D sur la sphère
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(v_localPos));
    // On lit le fichier .hdr (qui contient des valeurs au-delà de 1.0)
    vec3 color = texture(u_equirectangularMap, uv).rgb;

    FragColor = vec4(color, 1.0);
}

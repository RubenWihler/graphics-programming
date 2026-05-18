#version 330 core
layout(location=0) out vec4 FragColor;
layout(location=1) out vec4 BrightColor;

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoords;

uniform sampler2D u_cloudMap;
uniform vec3 u_sunDir;

void main() {
    // 1. On lit l'opacité de base du nuage depuis l'image
    float cloudAlpha = texture(u_cloudMap, v_TexCoords).r;
    
    vec3 N = normalize(v_Normal);
    float NdotL = dot(N, u_sunDir);
    
    // 2. LE MASQUE JOUR/NUIT
    // smoothstep(min, max, valeur) :
    // Si NdotL < -0.2 (nuit noire), ça donne 0.0
    // Si NdotL > 0.1 (jour), ça donne 1.0
    // Entre les deux, ça fait un joli dégradé pour la tombée de la nuit !
    float dayNightMask = smoothstep(-0.2, 0.1, NdotL);
    
    // 3. On multiplie l'opacité par le masque. 
    // La nuit, cloudAlpha * 0.0 = 0.0 (invisible !)
    cloudAlpha *= dayNightMask;

    // Optimisation : si le pixel est totalement transparent (ou dans la nuit), on le jette
    if(cloudAlpha < 0.02) discard;

    // Éclairage de base du soleil
    float diff = max(NdotL, 0.0);
    vec3 color = vec3(1.0) * diff; 

    FragColor = vec4(color, cloudAlpha);
    BrightColor = vec4(0.0); 
}

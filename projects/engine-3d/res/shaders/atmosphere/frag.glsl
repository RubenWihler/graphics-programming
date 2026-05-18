#version 330 core
layout(location=0) out vec4 FragColor;
layout(location=1) out vec4 BrightColor; // Pour le Bloom !

in vec3 v_worldPos;

// --- Paramètres ---
uniform vec3 u_planetCenter;
uniform float u_planetRadius;
uniform float u_atmosphereRadius;
uniform vec3 u_sunDir; 
uniform float u_sunIntensity;
uniform vec3 u_cameraPos;

// --- Coefficients Mathématiques de la Terre ---
const vec3 betaR = vec3(3.8e-6, 13.5e-6, 33.1e-6); 
const vec3 betaM = vec3(21e-6); 
const float hR = 7994.0;
const float hM = 1200.0;
const float PI = 3.14159265359;

// --- Fonction d'intersection Rayon/Sphère CORRIGÉE ---
vec2 rsi(vec3 r0, vec3 rd, float sr) {
    float a = dot(rd, rd);
    float b = 2.0 * dot(rd, r0);
    float c = dot(r0, r0) - (sr * sr);
    float d = (b * b) - 4.0 * a * c;
    
    // CORRECTION ICI : On retourne des distances négatives quand ça ne touche rien !
    if (d < 0.0) return vec2(-1.0, -1.0); 
    
    return vec2(
        (-b - sqrt(d)) / (2.0 * a),
        (-b + sqrt(d)) / (2.0 * a)
    );
}

float phaseRayleigh(float cosTheta) {
    return (3.0 / (16.0 * PI)) * (1.0 + cosTheta * cosTheta);
}

float phaseMie(float cosTheta) {
    float g = 0.76;
    float g2 = g * g;
    float c2 = cosTheta * cosTheta;
    float num = 3.0 * (1.0 - g2) * (1.0 + c2);
    float den = 8.0 * PI * pow(1.0 + g2 - 2.0 * g * cosTheta, 1.5) * (2.0 + g2);
    return num / den;
}

void main() {
    vec3 rayDir = normalize(v_worldPos - u_cameraPos);
    vec3 camPosLocal = u_cameraPos - u_planetCenter;
    
    vec2 tAtm = rsi(camPosLocal, rayDir, u_atmosphereRadius);
    vec2 tPla = rsi(camPosLocal, rayDir, u_planetRadius);
    
    float maxDist = (tPla.x >= 0.0 && tPla.x < tPla.y) ? tPla.x : tAtm.y;
    float startDist = max(0.0, tAtm.x);
    
    if (startDist >= maxDist) {
        FragColor = vec4(0.0);
        BrightColor = vec4(0.0);
        return;
    }
    
    // Transformation en VRAIS mètres
    float scaleToMeters = 6371000.0 / u_planetRadius;
    
    int samples = 16; 
    float stepSize = (maxDist - startDist) / float(samples);
    float stepSizeMeters = stepSize * scaleToMeters;
    
    float t = startDist + stepSize * 0.5; 
    
    vec3 totalRayleigh = vec3(0.0);
    vec3 totalMie = vec3(0.0);
    float opticalDepthR = 0.0;
    float opticalDepthM = 0.0;
    
    float cosTheta = dot(rayDir, u_sunDir);
    float pR = phaseRayleigh(cosTheta);
    float pM = phaseMie(cosTheta);

    for (int i = 0; i < samples; i++) {
        vec3 pos = camPosLocal + rayDir * t;
        
        float heightMeters = (length(pos) - u_planetRadius) * scaleToMeters;
        
        float hr = exp(-heightMeters / hR) * stepSizeMeters;
        float hm = exp(-heightMeters / hM) * stepSizeMeters;
        
        opticalDepthR += hr;
        opticalDepthM += hm;
        
        // --- NOUVELLE GESTION DE L'OMBRE ---
        vec2 tSunPla = rsi(pos, u_sunDir, u_planetRadius);
        // "Si le rayon touche la terre, et qu'il en ressort (y > 0.0) devant nous"
        // Ça élimine définitivement les bugs de bruit !
        if (tSunPla.y > 0.0) { 
            t += stepSize;
            continue; 
        }
        
        vec2 tSun = rsi(pos, u_sunDir, u_atmosphereRadius);
        int sunSamples = 8;
        float sunStepSize = max(0.0, tSun.y) / float(sunSamples); // max(0.0) empêche le crash !
        float sunStepSizeMeters = sunStepSize * scaleToMeters;
        float sunT = sunStepSize * 0.5;
        
        float sunOpticalDepthR = 0.0;
        float sunOpticalDepthM = 0.0;
        
        for (int j = 0; j < sunSamples; j++) {
            vec3 sunPos = pos + u_sunDir * sunT;
            float sunHeightMeters = (length(sunPos) - u_planetRadius) * scaleToMeters;
            
            sunOpticalDepthR += exp(-sunHeightMeters / hR) * sunStepSizeMeters;
            sunOpticalDepthM += exp(-sunHeightMeters / hM) * sunStepSizeMeters;
            sunT += sunStepSize;
        }
        
        vec3 tau = betaR * (opticalDepthR + sunOpticalDepthR) + betaM * 1.1 * (opticalDepthM + sunOpticalDepthM);
        vec3 attenuation = exp(-tau);
        
        totalRayleigh += attenuation * hr;
        totalMie += attenuation * hm;
        
        t += stepSize;
    }
    
    vec3 color = u_sunIntensity * (totalRayleigh * betaR * pR + totalMie * betaM * pM);
    
    // Le multiplicateur (10.0 est un bon équilibre)
    color *= 10.0;

    FragColor = vec4(color, 1.0);
    BrightColor = vec4(0.0);
}

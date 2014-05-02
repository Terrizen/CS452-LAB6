#version 130

in vec4 pass_color;
out vec4 out_color;

  float CalcShadowFactor(vec4 LightSpacePos) // calculate the shadow factor of pixels
  {
    vec3 ProjCoords = LightSpacePos.xyz / LightSpacePos.w;
    vec2 UVCoords;
    UVCoords.x = 0.5 * ProjCoords.x + 0.5;
    UVCoords.y = 0.5 * ProjCoords.y + 0.5;
    float z = 0.5 * ProjCoords.z + 0.5;
    float Depth = texture(gShadowMap, UVCoords).x;
    if (Depth < (z + 0.00001))
        return 0.5;
    else
        return 1.0;
  }
  vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, vec3 Normal, float ShadowFactor)
  {
    ...
    return (AmbientColor + ShadowFactor * (DiffuseColor + SpecularColor));
  }
  vec4 CalcDirectionalLight(vec3 Normal)
  {
    return CalcLightInternal(gDirectionalLight.Base, gDirectionalLight.Direction, Normal, 1.0);
  }
  vec4 CalcPointLight(struct PointLight l, vec3 Normal, vec4 LightSpacePos)
  {
    vec3 LightDirection = WorldPos0 - l.Position;
    float Distance = length(LightDirection);
    LightDirection = normalize(LightDirection);
    float ShadowFactor = CalcShadowFactor(LightSpacePos);

    vec4 Color = CalcLightInternal(l.Base, LightDirection, Normal, ShadowFactor);
    float Attenuation = l.Atten.Constant +
        l.Atten.Linear * Distance +
        l.Atten.Exp * Distance * Distance;

    return Color / Attenuation;
  }
  vec4 CalcSpotLight(struct SpotLight l, vec3 Normal, vec4 LightSpacePos)
  {
    vec3 LightToPixel = normalize(WorldPos0 - l.Base.Position);
    float SpotFactor = dot(LightToPixel, l.Direction);

    if (SpotFactor > l.Cutoff) {
        vec4 Color = CalcPointLight(l.Base, Normal, LightSpacePos);
        return Color * (1.0 - (1.0 - SpotFactor) * 1.0/(1.0 - l.Cutoff));
    }
    else {
        return vec4(0,0,0,0);
    }
  }
  void main()
{
    vec3 Normal = normalize(Normal0);
    vec4 TotalLight = CalcDirectionalLight(Normal);

    for (int i = 0 ; i < gNumPointLights ; i++) {
        TotalLight += CalcPointLight(gPointLights[i], Normal, LightSpacePos);
    }

    for (int i = 0 ; i < gNumSpotLights ; i++) {
        TotalLight += CalcSpotLight(gSpotLights[i], Normal, LightSpacePos);
    }

    vec4 SampledColor = texture2D(gSampler, TexCoord0.xy);
    FragColor = SampledColor * TotalLight;
}







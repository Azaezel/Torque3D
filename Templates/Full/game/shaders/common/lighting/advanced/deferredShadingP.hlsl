//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include "../../shaderModelAutoGen.hlsl"
#include "../../postfx/postFx.hlsl"
#include "shaders/common/torque.hlsl"

TORQUE_UNIFORM_SAMPLER2D(colorBufferTex,0);
TORQUE_UNIFORM_SAMPLER2D(diffuseLightingBuffer,1);
TORQUE_UNIFORM_SAMPLER2D(matInfoTex,2);
TORQUE_UNIFORM_SAMPLER2D(specularLightingBuffer,3);
TORQUE_UNIFORM_SAMPLER2D(deferredTex,4);

float4 main( PFXVertToPix IN) : TORQUE_TARGET0
{        
   float depth = TORQUE_DEFERRED_UNCONDITION( deferredTex, IN.uv0 ).w;

   if (depth>0.9999)
      return float4(0,0,0,0);

   float3 albedo = TORQUE_TEX2D( colorBufferTex, IN.uv0 ).rgb; //albedo
   float4 matInfo = TORQUE_TEX2D(matInfoTex, IN.uv0); //flags|smoothness|ao|metallic

   bool emissive = getFlag(matInfo.r, 0);
   if (emissive)
   {
      return float4(albedo, 1.0);
   }
	  
   float4 diffuse = TORQUE_TEX2D( diffuseLightingBuffer, IN.uv0 ); //shadowmap*specular
   float4 specular = TORQUE_TEX2D( specularLightingBuffer, IN.uv0 ); //environment mapping*lightmaps
      
   float metalness = matInfo.a;
   
   float3 diffuseColor = albedo - (albedo * metalness);
   float3 specularColor = lerp(float3(0.04,0.04,0.04), albedo, metalness);

   float3 light = (diffuseColor * diffuse.rgb) + (specularColor * specular.rgb);

   //albedo = diffuseColor+lerp(reflectColor,indiffuseLighting,frez);
   //albedo *= max(diffuseLighting.rgb,float3(0,0,0));
   
   return float4(light.rgb, 1.0);
   //return float4(light.rgb, 1.0);
}

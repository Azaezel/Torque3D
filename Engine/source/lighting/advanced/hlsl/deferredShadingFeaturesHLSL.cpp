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

#include "platform/platform.h"
#include "lighting/advanced/hlsl/deferredShadingFeaturesHLSL.h"

#include "lighting/advanced/advancedLightBinManager.h"
#include "shaderGen/langElement.h"
#include "shaderGen/shaderOp.h"
#include "shaderGen/conditionerFeature.h"
#include "renderInstance/renderDeferredMgr.h"
#include "materials/processedMaterial.h"
#include "materials/materialFeatureTypes.h"


//****************************************************************************
// Deferred Shading Features
//****************************************************************************
U32 PBRConfigMapHLSL::getOutputTargets(const MaterialFeatureData& fd) const
{
   return fd.features[MFT_isDeferred] ? ShaderFeature::RenderTarget2 : ShaderFeature::DefaultTarget;
}

void PBRConfigMapHLSL::processPix( Vector<ShaderComponent*> &componentList, const MaterialFeatureData &fd )
{
   // Get the texture coord.
   Var *texCoord = getInTexCoord( "texCoord", "float2", componentList );

   MultiLine* meta = new MultiLine;
   Var* pbrConfig;
   if (fd.features[MFT_isDeferred])
   {
      pbrConfig = (Var*)LangElement::find(getOutputTargetVarName(ShaderFeature::RenderTarget2));
      if (!pbrConfig)
      {
         // create material var
         pbrConfig = new Var;
         pbrConfig->setType("fragout");
         pbrConfig->setName(getOutputTargetVarName(ShaderFeature::RenderTarget2));
         pbrConfig->setStructName("OUT");
      }
   }
   else
   {
      pbrConfig = (Var*)LangElement::find("PBRConfig");
      if (!pbrConfig)
      {
         pbrConfig = new Var("PBRConfig", "float4");
         meta->addStatement(new GenOp("   @;\r\n", new DecOp(pbrConfig)));
      }
   }

   // create texture var
   Var * pbrConfigMap = new Var;
   pbrConfigMap->setType( "SamplerState" );
   pbrConfigMap->setName( "PBRConfigMap" );
   pbrConfigMap->uniform = true;
   pbrConfigMap->sampler = true;
   pbrConfigMap->constNum = Var::getTexUnitNum();

   Var* pbrConfigMapTex = new Var;
   pbrConfigMapTex->setName("PBRConfigMapTex");
   pbrConfigMapTex->setType("Texture2D");
   pbrConfigMapTex->uniform = true;
   pbrConfigMapTex->texture = true;
   pbrConfigMapTex->constNum = pbrConfigMap->constNum;
   LangElement *texOp = new GenOp("   @.Sample(@, @)", pbrConfigMapTex, pbrConfigMap, texCoord);
   
   Var *metalness = (Var*)LangElement::find("metalness");
   if (!metalness) metalness = new Var("metalness", "float");
   Var *smoothness = (Var*)LangElement::find("smoothness");
   if (!smoothness) smoothness = new Var("smoothness", "float");

   meta->addStatement(new GenOp("   @ = @.r;\r\n", new DecOp(smoothness), texOp));
   meta->addStatement(new GenOp("   @ = @.b;\r\n", new DecOp(metalness), texOp));

   if (fd.features[MFT_InvertSmoothness])
      meta->addStatement(new GenOp("   @ = 1.0-@;\r\n", smoothness, smoothness));

   if (!fd.features[MFT_isDeferred])
   meta->addStatement(new GenOp("   @ = @.ggga;\r\n", pbrConfig, texOp));
   meta->addStatement(new GenOp("   @.bga = float3(@,@.g,@);\r\n", pbrConfig, smoothness, pbrConfig, metalness));
   output = meta;
}

ShaderFeature::Resources PBRConfigMapHLSL::getResources( const MaterialFeatureData &fd )
{
   Resources res; 
   res.numTex = 1;
   res.numTexReg = 1;

   return res;
}

void PBRConfigMapHLSL::setTexData(   Material::StageData &stageDat,
                                       const MaterialFeatureData &fd,
                                       RenderPassData &passData,
                                       U32 &texIndex )
{
   GFXTextureObject *tex = stageDat.getTex(MFT_PBRConfigMap);
   if ( tex )
   {
      passData.mTexType[ texIndex ] = Material::Standard;
      passData.mSamplerNames[ texIndex ] = "PBRConfigMap";
      passData.mTexSlot[ texIndex++ ].texObject = tex;
   }
}

void PBRConfigMapHLSL::processVert( Vector<ShaderComponent*> &componentList,
                                       const MaterialFeatureData &fd )
{
   MultiLine *meta = new MultiLine;
   getOutTexCoord(   "texCoord", 
                     "float2", 
                     fd.features[MFT_TexAnim], 
                     meta, 
                     componentList );
   output = meta;
}

// Material Info Flags -> Red ( Flags ) of Material Info Buffer.
void DeferredMatInfoFlagsHLSL::processPix( Vector<ShaderComponent*> &componentList, const MaterialFeatureData &fd )
{
   // search for material var
   Var* pbrConfig;
   if (fd.features[MFT_isDeferred])
   {
      pbrConfig = (Var*)LangElement::find(getOutputTargetVarName(ShaderFeature::RenderTarget2));
      if (!pbrConfig)
      {
         // create material var
         pbrConfig = new Var;
         pbrConfig->setType("fragout");
         pbrConfig->setName(getOutputTargetVarName(ShaderFeature::RenderTarget2));
         pbrConfig->setStructName("OUT");
      }
   }
   else
   {
      pbrConfig = (Var*)LangElement::find("PBRConfig");
      if (!pbrConfig) pbrConfig = new Var("PBRConfig", "float4");
   }

   Var *matInfoFlags = new Var;
   matInfoFlags->setType( "float" );
   matInfoFlags->setName( "matInfoFlags" );
   matInfoFlags->uniform = true;
   matInfoFlags->constSortPos = cspPotentialPrimitive;

   output = new GenOp( "   @.r = @;\r\n", pbrConfig, matInfoFlags );
}

U32 PBRConfigVarsHLSL::getOutputTargets(const MaterialFeatureData& fd) const
{
   return fd.features[MFT_isDeferred] ? ShaderFeature::RenderTarget2 : ShaderFeature::DefaultTarget;
}

void PBRConfigVarsHLSL::processPix( Vector<ShaderComponent*> &componentList, const MaterialFeatureData &fd )
{
   MultiLine* meta = new MultiLine;
   Var* pbrConfig;
   if (fd.features[MFT_isDeferred])
   {
      pbrConfig = (Var*)LangElement::find(getOutputTargetVarName(ShaderFeature::RenderTarget2));
      if (!pbrConfig)
      {
         // create material var
         pbrConfig = new Var;
         pbrConfig->setType("fragout");
         pbrConfig->setName(getOutputTargetVarName(ShaderFeature::RenderTarget2));
         pbrConfig->setStructName("OUT");
      }
   }
   else
   {
      pbrConfig = (Var*)LangElement::find("PBRConfig");
      if (!pbrConfig) pbrConfig = new Var("PBRConfig", "float4");
      meta->addStatement(new GenOp("   @;\r\n", new DecOp(pbrConfig)));
   }
   Var *metalness = new Var("metalness", "float");
   metalness->uniform = true;
   metalness->constSortPos = cspPotentialPrimitive;

   Var *smoothness = new Var("smoothness", "float");
   smoothness->uniform = true;
   smoothness->constSortPos = cspPotentialPrimitive;

   //matinfo.g slot reserved for AO later
   meta->addStatement(new GenOp("   @.g = 1.0;\r\n", pbrConfig));
   meta->addStatement(new GenOp("   @.b = @;\r\n", pbrConfig, smoothness));
   if (fd.features[MFT_InvertSmoothness])
      meta->addStatement(new GenOp("   @ = 1.0-@;\r\n", smoothness, smoothness));
   meta->addStatement(new GenOp("   @.a = @;\r\n", pbrConfig, metalness));
   output = meta;
}

//deferred emissive
void DeferredEmissiveHLSL::processPix(Vector<ShaderComponent*> &componentList, const MaterialFeatureData &fd)
{
   //for now emission just uses the diffuse color, we could plug in a separate texture for emission at some stage
   Var *diffuseTargetVar = (Var*)LangElement::find(getOutputTargetVarName(ShaderFeature::RenderTarget1));
   if (!diffuseTargetVar)
      return; //oh dear something is not right, maybe we should just write 0's instead

   // search for scene color target var
   Var *sceneColorVar = (Var*)LangElement::find(getOutputTargetVarName(ShaderFeature::RenderTarget3));
   if (!sceneColorVar)
   {
      // create scene color target var
      sceneColorVar = new Var;
      sceneColorVar->setType("fragout");
      sceneColorVar->setName(getOutputTargetVarName(ShaderFeature::RenderTarget3));
      sceneColorVar->setStructName("OUT");
   }

   output = new GenOp("@ = float4(@.rgb,0);", sceneColorVar, diffuseTargetVar);
}

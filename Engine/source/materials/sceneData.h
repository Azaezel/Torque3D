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
#ifndef _SCENEDATA_H_
#define _SCENEDATA_H_

#ifndef _SCENERENDERSTATE_H_
#include "scene/sceneRenderState.h"
#endif
#ifndef _LIGHTMANAGER_H_
#include "lighting/lightManager.h"
#endif
#ifndef _GFXDEVICE_H_
#include "gfx/gfxDevice.h"
#endif

class GFXTexHandle;
class GFXCubemap;
class CustomShaderBindingData;

struct SceneData
{
   /// The special bin types.
   enum BinType
   {
      /// A normal render bin that isn't one of 
      /// the special bins we care about.
      RegularBin = 0,

      /// The glow render bin.
      /// @see RenderGlowMgr
      GlowBin,

      /// The deferred render bin.
      /// @RenderDeferredMgr
      DeferredBin,
      /// The selection-highlight render bin.  
      /// @afxRenderHighlightMgr  
      HighlightBin,
   };

   /// This defines when we're rendering a special bin 
   /// type that the material or lighting system needs
   /// to know about.
   BinType binType = BinType::RegularBin;

   // textures
   GFXTextureObject* lightmap = NULL;
   GFXTextureObject *backBuffTex = NULL;
   GFXTextureObject *reflectTex = NULL;
   GFXTextureObject *miscTex = NULL;
   GFXTextureObject *accuTex = NULL;
   
   /// The current lights to use in rendering
   /// in order of the light importance.
   LightInfo* lights[8] = { NULL };

   ///
   LinearColorF ambientLightColor;

   // fog      
   F32 fogDensity = 0.0f;
   F32 fogDensityOffset = 0.0f;
   F32 fogHeightFalloff = 0.0f;
   LinearColorF fogColor = LinearColorF::ZERO;
  
   // misc
   const MatrixF *objTrans = NULL;
   GFXCubemap *cubemap = NULL;
   F32 visibility = 1.0f;

   /// Enables wireframe rendering for the object.
   bool wireframe = false;

   /// A generic hint value passed from the game
   /// code down to the material for use by shader 
   /// features.
   void *materialHint;

   Vector<CustomShaderBindingData*> customShaderData;

   /// Constructor.
   SceneData() 
   { 
      dMemset( this, 0, sizeof( SceneData ) );
      objTrans = &MatrixF::Identity;
      visibility = 1.0f;
   }

   /// Initializes the data with the scene state setting
   /// common scene wide parameters.
   inline void init( const SceneRenderState *state, BinType type = RegularBin )
   {
      dMemset( this, 0, sizeof( SceneData ) );
      setFogParams( state->getSceneManager()->getFogData() );
      wireframe = GFXDevice::getWireframe();
      binType = type;
      objTrans = &MatrixF::Identity;
      visibility = 1.0f;
      ambientLightColor = state->getAmbientLightColor();
   }

   inline void setFogParams( const FogData &data )
   {
      fogDensity = data.density;
      fogDensityOffset = data.densityOffset;
      if ( !mIsZero( data.atmosphereHeight ) )
         fogHeightFalloff = 1.0f / data.atmosphereHeight;
      else
         fogHeightFalloff = 0.0f;

      fogColor = data.color;
   }
};

#endif // _SCENEDATA_H_

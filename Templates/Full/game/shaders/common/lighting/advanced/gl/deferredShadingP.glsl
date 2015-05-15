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

#include "../../../gl/hlslCompat.glsl"
#include "shadergen:/autogenConditioners.h"
#include "../../../postfx/gl/postFx.glsl"
#include "../../../gl/torque.glsl"

uniform sampler2D colorBufferTex;
uniform sampler2D lightPrePassTex;
uniform sampler2D matInfoTex;
uniform sampler2D lightMapTex;

out vec4 OUT_col;

void main()
{        
   vec4 lightBuffer = texture( lightPrePassTex, uv0 );
   vec4 colorBuffer = texture( colorBufferTex, uv0 );
   vec4 matInfo = texture( matInfoTex, uv0 );
   vec4 lightMapBuffer = texture( lightMapTex, uv0 );
   
   colorBuffer *= vec4(lightBuffer.rgb, 1.0);
   vec3 diffuseColor = colorBuffer.rgb - (colorBuffer.rgb * 0.92 * matInfo.a);
   lightBuffer.rgb = mix( 0.08 * lightMapBuffer.rgb, colorBuffer.rgb, min(matInfo.a, 0.92));
   colorBuffer.rgb =  diffuseColor + lightMapBuffer.rgb*lightBuffer.rgb;

   OUT_col = hdrEncode( colorBuffer );
}

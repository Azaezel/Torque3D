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

#ifndef _TORQUE_UUID_H_
#define _TORQUE_UUID_H_

#ifndef _PLATFORM_H_
   #include "platform/platform.h"
#endif


namespace Torque
{
   /// A universally unique identifier.
   class UUID
   {
      public:
      
         typedef void Parent;
         
      protected:

         U32   a;
         U16   b;
         U16   c;
         U8    d;
         U8    e;
         U8    f[ 6 ];
         
         static UUID smNull;
         
      public:
      
         UUID()
         {
            a = 0;
            b = 0;
            c = 0;
            d = 0;
            e = 0;
            dMemset( f, 0, sizeof( f ) );
         }
         
         ///
         bool isNull() const { return ( *this == smNull ); }
         
         /// Generate a new universally unique identifier (UUID).
         void generate();
         
         /// Convert the given universally unique identifier to a printed string
         /// representation.
         String toString() const;
         
         /// Parse a text string generated by UUIDToString back into a
         /// universally unique identifier (UUID).
         bool fromString( const char* str );

         /// Return a hash value for this UUID.
         U32 getHash() const;

         bool operator ==( const UUID& uuid ) const
         {
            return ( dMemcmp( this, &uuid, sizeof( UUID ) ) == 0 );
         }
         bool operator !=( const UUID& uuid ) const
         {
            return !( *this == uuid );
         }
   };
}

namespace DictHash
{
   inline U32 hash( const Torque::UUID& uuid )
   {
      return uuid.getHash();
   }
}

#endif // !_TORQUE_UUID_H_

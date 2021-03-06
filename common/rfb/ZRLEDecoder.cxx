/* Copyright (C) 2002-2005 RealVNC Ltd.  All Rights Reserved.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 */
#include <rfb/CMsgReader.h>
#include <rfb/CConnection.h>
#include <rfb/PixelBuffer.h>
#include <rfb/ZRLEDecoder.h>

using namespace rfb;

static inline rdr::U32 readOpaque24A(rdr::InStream* is)
{
  is->check(3);
  rdr::U32 r=0;
  ((rdr::U8*)&r)[0] = is->readU8();
  ((rdr::U8*)&r)[1] = is->readU8();
  ((rdr::U8*)&r)[2] = is->readU8();
  return r;

}
static inline rdr::U32 readOpaque24B(rdr::InStream* is)
{
  is->check(3);
  rdr::U32 r=0;
  ((rdr::U8*)&r)[1] = is->readU8();
  ((rdr::U8*)&r)[2] = is->readU8();
  ((rdr::U8*)&r)[3] = is->readU8();
  return r;
}

#define BPP 8
#include <rfb/zrleDecode.h>
#undef BPP
#define BPP 16
#include <rfb/zrleDecode.h>
#undef BPP
#define BPP 32
#include <rfb/zrleDecode.h>
#define CPIXEL 24A
#include <rfb/zrleDecode.h>
#undef CPIXEL
#define CPIXEL 24B
#include <rfb/zrleDecode.h>
#undef CPIXEL
#undef BPP

ZRLEDecoder::ZRLEDecoder(CConnection* conn) : Decoder(conn)
{
}

ZRLEDecoder::~ZRLEDecoder()
{
}

void ZRLEDecoder::readRect(const Rect& r, ModifiablePixelBuffer* pb)
{
  rdr::InStream* is = conn->getInStream();
  rdr::U8* buf = conn->reader()->getImageBuf(64 * 64 * 4);
  const rfb::PixelFormat& pf = conn->cp.pf();
  switch (pf.bpp) {
  case 8:  zrleDecode8 (r, is, &zis, (rdr::U8*) buf, pf, pb); break;
  case 16: zrleDecode16(r, is, &zis, (rdr::U16*)buf, pf, pb); break;
  case 32:
    {
      Pixel maxPixel = pf.pixelFromRGB((rdr::U16)-1, (rdr::U16)-1, (rdr::U16)-1);
      bool fitsInLS3Bytes = maxPixel < (1<<24);
      bool fitsInMS3Bytes = (maxPixel & 0xff) == 0;

      if ((fitsInLS3Bytes && pf.isLittleEndian()) ||
          (fitsInMS3Bytes && pf.isBigEndian()))
      {
        zrleDecode24A(r, is, &zis, (rdr::U32*)buf, pf, pb);
      }
      else if ((fitsInLS3Bytes && pf.isBigEndian()) ||
               (fitsInMS3Bytes && pf.isLittleEndian()))
      {
        zrleDecode24B(r, is, &zis, (rdr::U32*)buf, pf, pb);
      }
      else
      {
        zrleDecode32(r, is, &zis, (rdr::U32*)buf, pf, pb);
      }
      break;
    }
  }
}

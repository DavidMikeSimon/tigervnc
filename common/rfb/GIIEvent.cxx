/* Copyright (C) 2002-2005 RealVNC Ltd.  All Rights Reserved.
 * Copyright 2009-2011 Pierre Ossman for Cendio AB
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

#include <rdr/OutStream.h>
#include <rfb/GIIEvent.h>
#include <rfb/giiMsgTypes.h>

using namespace rfb;

unsigned int GIIKeyEvent::getWriteLength() const
{
  return 22;
}

void GIIKeyEvent::write(rdr::OutStream* os) const
{
  os->writeU8(down ? giiEventKeyPress : giiEventKeyRelease);
  os->pad(2);
  os->writeU8(devId);
  os->writeU32(modMask);
  os->writeU32(sym);
  os->writeU32(label);
  os->writeU32(button);
}

unsigned int GIIPointerEvent::getWriteLength() const
{
  return 22;
}

void GIIPointerEvent::write(rdr::OutStream* os) const
{
  os->writeU8(absolute ? giiEventPointerAbsolute : giiEventPointerRelative);
  os->pad(2);
  os->writeU8(devId);
  os->writeS32(x);
  os->writeS32(y);
  os->writeS32(z);
  os->writeS32(wheel);
}

unsigned int GIIButtonEvent::getWriteLength() const
{
  return 10;
}

void GIIButtonEvent::write(rdr::OutStream* os) const
{
  os->writeU8(down ? giiEventButtonPress : giiEventButtonRelease);
  os->pad(2);
  os->writeU8(devId);
  os->writeU32(buttonNum);
}

unsigned int GIIValuatorEvent::getWriteLength() const
{
  return 14 + values.size()*4;
}

void GIIValuatorEvent::write(rdr::OutStream* os) const
{
  os->writeU8(absolute ? giiEventValuatorAbsolute : giiEventValuatorRelative);
  os->pad(2);
  os->writeU8(devId);
  os->writeU32(firstValuatorNum);
  os->writeU32(values.size());
  for (std::vector<int>::const_iterator v = values.begin(); v < values.end(); ++v)
  {
    os->writeS32(*v);
  }
}

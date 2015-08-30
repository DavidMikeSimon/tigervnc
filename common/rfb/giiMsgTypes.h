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
#ifndef __RFB_GIIMSGTYPES_H__
#define __RFB_GIIMSGTYPES_H__

namespace rfb {
  // message from client describing an input event
  const unsigned int giiMsgSubtypeInjectEvent = 0;

  // bi-directional message indicating version of GII protocol support
  const unsigned int giiMsgSubtypeVersion = 1;

  // bi-directional message for creating input devices
  const unsigned int giiMsgSubtypeDeviceCreation = 2;

  // message from client to destroy an input device
  const unsigned int giiMsgSubtypeDeviceDestruction = 3;

  // add this to a subtype above to indicate big endian data in the message
  const unsigned int giiMsgBigEndianOffset = 128;

  // types of events within InjectEvent message
  const unsigned int giiEventKeyPress = 5;
  const unsigned int giiEventKeyRelease = 6;
  const unsigned int giiEventKeyRepeat = 7;
  const unsigned int giiEventPointerRelative = 8;
  const unsigned int giiEventPointerAbsolute = 9;
  const unsigned int giiEventButtonPress = 10;
  const unsigned int giiEventButtonRelease = 11;
  const unsigned int giiEventValuatorRelative = 12;
  const unsigned int giiEventValuatorAbsolute = 13;
}

#endif // __RFB_GIIMSGTYPES_H

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

// GII input event types and constants

#ifndef __RFB_GIIEVENT_H__
#define __RFB_GIIEVENT_H__

#include <rfb/GIIDevice.h>

namespace rfb {
  const unsigned int giiModMaskShift  = 1 << 0;
  const unsigned int giiModMaskCtrl   = 1 << 1;
  const unsigned int giiModMaskAlt    = 1 << 2;
  const unsigned int giiModMaskMeta   = 1 << 3;
  const unsigned int giiModMaskSuper  = 1 << 4;
  const unsigned int giiModMaskHyper  = 1 << 5;
  const unsigned int giiModMaskAltGr  = 1 << 6;
  const unsigned int giiModMaskCaps   = 1 << 7;
  const unsigned int giiModMaskNum    = 1 << 8;
  const unsigned int giiModMaskScroll = 1 << 9;

  struct GIIEvent {
    //virtual ~GIIEvent =0 {};
  };

  struct GIIKeyEvent : public GIIEvent {
    GIIKeyEvent(bool down, unsigned modMask, unsigned sym,
                unsigned label, unsigned button) :

    bool down;
    unsigned int modMask;
    unsigned int sym;
    unsigned int label;
    unsigned int button;
  };

  struct GIIPointerEvent : public GIIEvent {
    bool absolute;
    int x; // Axis 0
    int y; // Axis 1
    int z; // Axis 2
    int wheel; // Axis 3 (generally not actually used for mouse wheel...)
  };

  struct GIIButtonEvent : public GIIEvent {
    bool down;
    unsigned int buttonNum;
  };

  struct GIIValuatorEvent : public GIIEvent {
    bool absolute;
    unsigned int valuatorNum;
    int value;
  };

  struct GIIGenericEvent {
    enum {GII_EV_KEY, GII_EV_POINTER, GII_EV_BUTTON, GII_EV_VALUATOR} type;
    union {
      GIIKeyEvent keyEvent;
      GIIPointerEvent pointerEvent;
      GIIButtonEvent buttonEvent;
      GIIValuatorEvent valuatorEvent;
    };

    GIIEvent(const GIIKeyEvent&)
  };
}

#endif // __RFB_GIIEVENT_H__

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
//
// SMsgHandler - class to handle incoming messages on the server side.
//

#ifndef __RFB_SMSGHANDLER_H__
#define __RFB_SMSGHANDLER_H__

#include <rdr/types.h>
#include <rfb/PixelFormat.h>
#include <rfb/ConnParams.h>
#include <rfb/InputHandler.h>
#include <rfb/ScreenSet.h>

namespace rdr { class InStream; }

namespace rfb {
  struct GIIDevice;
  struct GIIKeyEvent;
  struct GIIPointerEvent;
  struct GIIButtonEvent;
  struct GIIValuatorEvent;

  class SMsgHandler : public InputHandler {
  public:
    SMsgHandler();
    virtual ~SMsgHandler();

    // The following methods are called as corresponding messages are read.  A
    // derived class should override these methods as desired.  Note that for
    // the setPixelFormat(), setEncodings(), setDesktopSize(), and
    // giiVersionAgreed() methods, a derived class must call on to SMsgHandler's
    // methods.

    virtual void clientInit(bool shared);

    virtual void setPixelFormat(const PixelFormat& pf);
    virtual void setEncodings(int nEncodings, const rdr::S32* encodings);
    virtual void framebufferUpdateRequest(const Rect& r, bool incremental) = 0;
    virtual void setDesktopSize(int fb_width, int fb_height,
                                const ScreenSet& layout) = 0;
    virtual void fence(rdr::U32 flags, unsigned len, const char data[]) = 0;
    virtual void enableContinuousUpdates(bool enable,
                                         int x, int y, int w, int h) = 0;

    virtual void giiVersionAgreed(unsigned version);
    virtual void giiDeviceCreate(const GIIDevice& dev);
    virtual void giiDeviceDestroy(unsigned devId);
    virtual void giiKeyEvent(const GIIKeyEvent& ev);
    virtual void giiPointerEvent(const GIIPointerEvent& ev);
    virtual void giiButtonEvent(const GIIButtonEvent& ev);
    virtual void giiValuatorEvent(const GIIValuatorEvent& ev);

    // InputHandler interface
    // The InputHandler methods will be called for the corresponding messages.

    // supportsLocalCursor() is called whenever the status of
    // cp.supportsLocalCursor has changed.  At the moment this happens on a
    // setEncodings message, but in the future this may be due to a message
    // specially for this purpose.
    virtual void supportsLocalCursor();

    // supportsFence() is called the first time we detect support for fences
    // in the client. A fence message should be sent at this point to notify
    // the client of server support.
    virtual void supportsFence();

    // supportsContinuousUpdates() is called the first time we detect that
    // the client wants the continuous updates extension. A
    // EndOfContinuousUpdates message should be sent back to the client at
    // this point if it is supported.
    virtual void supportsContinuousUpdates();

    // supportsGII is called the first time we detect that the client
    // wants to use the General Input Interface. A giiVersionRange(1,1) should
    // be sent to the client if the server supports it.
    virtual void supportsGII();

    ConnParams cp;
  };
}
#endif

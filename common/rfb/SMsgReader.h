/* Copyright (C) 2002-2005 RealVNC Ltd.  All Rights Reserved.
 * Copyright 2009-2014 Pierre Ossman for Cendio AB
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
// SMsgReader - class for reading RFB messages on the server side
// (i.e. messages from client to server).
//

#ifndef __RFB_SMSGREADER_H__
#define __RFB_SMSGREADER_H__

namespace rdr { class InStream; }

namespace rfb {
  class SMsgHandler;

  class SMsgReader {
  public:
    SMsgReader(SMsgHandler* handler, rdr::InStream* is);
    virtual ~SMsgReader();

    void readClientInit();

    // readMsg() reads a message, calling the handler as appropriate.
    void readMsg();

    rdr::InStream* getInStream() { return is; }

  protected:
    void readSetPixelFormat();
    void readSetEncodings();
    void readSetDesktopSize();

    void readFramebufferUpdateRequest();
    void readEnableContinuousUpdates();

    void readGIIMessage();
    void readGIISubInjectEvents(rdr::U16 len, bool bigEndian);
    void readGIISubKeyEvent(rdr::U16 len, rdr::U8 eType, bool bigEndian);
    void readGIISubButtonEvent(rdr::U16 len, rdr::U8 eType, bool bigEndian);
    void readGIISubPointerEvent(rdr::U16 len, rdr::U8 eType, bool bigEndian);
    void readGIISubValuatorEvent(rdr::U16 len, rdr::U8 eType, bool bigEndian);
    void readGIISubVersionAgreed(rdr::U16 len, bool bigEndian);
    void readGIISubDeviceCreate(rdr::U16 len, bool bigEndian);
    void readGIISubDeviceDestroy(rdr::U16 len, bool bigEndian);

    void readFence();

    void readKeyEvent();
    void readPointerEvent();
    void readClientCutText();

    SMsgHandler* handler;
    rdr::InStream* is;
  };
}
#endif

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
#include <stdio.h>
#include <vector>

#include <rdr/InStream.h>
#include <rfb/msgTypes.h>
#include <rfb/Exception.h>
#include <rfb/util.h>
#include <rfb/SMsgHandler.h>
#include <rfb/SMsgReader.h>
#include <rfb/Configuration.h>
#include <rfb/LogWriter.h>
#include <rfb/giiMsgTypes.h>
#include <rfb/GIIDevice.h>
#include <rfb/GIIEvent.h>

using namespace rfb;

static LogWriter vlog("SMsgReader");

static IntParameter maxCutText("MaxCutText", "Maximum permitted length of an incoming clipboard update", 256*1024);

SMsgReader::SMsgReader(SMsgHandler* handler_, rdr::InStream* is_)
  : handler(handler_), is(is_)
{
}

SMsgReader::~SMsgReader()
{
}

void SMsgReader::readClientInit()
{
  bool shared = is->readU8();
  handler->clientInit(shared);
}

void SMsgReader::readMsg()
{
  int msgType = is->readU8();
  switch (msgType) {
  case msgTypeSetPixelFormat:
    readSetPixelFormat();
    break;
  case msgTypeSetEncodings:
    readSetEncodings();
    break;
  case msgTypeSetDesktopSize:
    readSetDesktopSize();
    break;
  case msgTypeFramebufferUpdateRequest:
    readFramebufferUpdateRequest();
    break;
  case msgTypeEnableContinuousUpdates:
    readEnableContinuousUpdates();
    break;
  case msgTypeClientFence:
    readFence();
    break;
  case msgTypeKeyEvent:
    readKeyEvent();
    break;
  case msgTypePointerEvent:
    readPointerEvent();
    break;
  case msgTypeClientCutText:
    readClientCutText();
    break;
  default:
    fprintf(stderr, "unknown message type %d\n", msgType);
    throw Exception("unknown message type");
  }
}

void SMsgReader::readSetPixelFormat()
{
  is->skip(3);
  PixelFormat pf;
  pf.read(is);
  handler->setPixelFormat(pf);
}

void SMsgReader::readSetEncodings()
{
  is->skip(1);
  int nEncodings = is->readU16();
  rdr::S32Array encodings(nEncodings);
  for (int i = 0; i < nEncodings; i++)
    encodings.buf[i] = is->readU32();
  handler->setEncodings(nEncodings, encodings.buf);
}

void SMsgReader::readSetDesktopSize()
{
  int width, height;
  int screens, i;
  rdr::U32 id, flags;
  int sx, sy, sw, sh;
  ScreenSet layout;

  is->skip(1);

  width = is->readU16();
  height = is->readU16();

  screens = is->readU8();
  is->skip(1);

  for (i = 0;i < screens;i++) {
    id = is->readU32();
    sx = is->readU16();
    sy = is->readU16();
    sw = is->readU16();
    sh = is->readU16();
    flags = is->readU32();

    layout.add_screen(Screen(id, sx, sy, sw, sh, flags));
  }

  handler->setDesktopSize(width, height, layout);
}

void SMsgReader::readFramebufferUpdateRequest()
{
  bool inc = is->readU8();
  int x = is->readU16();
  int y = is->readU16();
  int w = is->readU16();
  int h = is->readU16();
  handler->framebufferUpdateRequest(Rect(x, y, x+w, y+h), inc);
}

void SMsgReader::readEnableContinuousUpdates()
{
  bool enable;
  int x, y, w, h;

  enable = is->readU8();

  x = is->readU16();
  y = is->readU16();
  w = is->readU16();
  h = is->readU16();

  handler->enableContinuousUpdates(enable, x, y, w, h);
}

void SMsgReader::readGIIMessage()
{
  rdr::U8 msgSubtype;
  rdr::U16 len;
  bool bigEndian = false;

  msgSubtype = is->readU8();

  if (msgSubtype >= giiMsgBigEndianOffset) {
    bigEndian = true;
    msgSubtype -= giiMsgBigEndianOffset;
  }

  len = is->readEU16(bigEndian);

  switch (msgSubtype) {
  case giiMsgSubtypeInjectEvent:
    readGIISubInjectEvents(len, bigEndian);
    break;
  case giiMsgSubtypeVersion:
    readGIISubVersionAgreed(len, bigEndian);
    break;
  case giiMsgSubtypeDeviceCreation:
    readGIISubDeviceCreate(len, bigEndian);
    break;
  case giiMsgSubtypeDeviceDestruction:
    readGIISubDeviceDestroy(len, bigEndian);
    break;
  default:
    fprintf(stderr, "unknown GII message Subtype %d\n", msgSubtype);
    throw Exception("unknown GII message Subtype");
  }
}

void SMsgReader::readGIISubInjectEvents(rdr::U16 len, bool bigEndian)
{
  unsigned bytesRead = 0;
  while (bytesRead < len) {
    rdr::U8 eventSize, eventType;
    eventSize = is->readU8();
    eventType = is->readU8();

    switch (eventType) {
      case giiEventKeyPress:
      case giiEventKeyRelease:
      case giiEventKeyRepeat:
        readGIISubKeyEvent(eventSize, eventType, bigEndian);
        break;
      case giiEventPointerRelative:
      case giiEventPointerAbsolute:
        readGIISubPointerEvent(eventSize, eventType, bigEndian);
        break;
      case giiEventButtonPress:
      case giiEventButtonRelease:
        readGIISubButtonEvent(eventSize, eventType, bigEndian);
        break;
      case giiEventValuatorRelative:
      case giiEventValuatorAbsolute:
        readGIISubValuatorEvent(eventSize, eventType, bigEndian);
        break;
      default:
        fprintf(stderr, "unknown GII input event type %d\n", eventType);
        throw Exception("unknown GII input event type");
    }

    bytesRead += eventSize + 2; // two bytes for the ev size and type
    if (bytesRead > len) {
      fprintf(stderr, "overflow on GII event list\n");
      throw Exception("overflow on GII event list\n");
    }
  }
}

void SMsgReader::readGIISubKeyEvent(rdr::U16 len, rdr::U8 eType,
                                    bool bigEndian)
{
  if (len != 22) {
    fprintf(stderr, "invalid GII key event message length %d\n", len);
    throw Exception("invalid GII key event message length");
  }
  is->skip(2); // padding

  GIIKeyEvent ev;
  // Treat key-repeat as key-down, spec isn't very clear on key-repeat...
  ev.down = (eType != giiEventKeyRelease);
  ev.devId = is->readEU32(bigEndian);
  ev.modMask = is->readEU32(bigEndian);
  ev.sym = is->readEU32(bigEndian);
  ev.label = is->readEU32(bigEndian);
  ev.button = is->readEU32(bigEndian);

  handler->giiKeyEvent(ev);
}

void SMsgReader::readGIISubPointerEvent(rdr::U16 len, rdr::U8 eType,
                                        bool bigEndian)
{
  if (len != 22) {
    fprintf(stderr, "invalid GII pointer event message length %d\n", len);
    throw Exception("invalid GII pointer event message length");
  }
  is->skip(2); // padding

  GIIPointerEvent ev;
  ev.absolute = (eType == giiEventPointerAbsolute);
  ev.devId = is->readEU32(bigEndian);
  ev.x = is->readES32(bigEndian);
  ev.y = is->readES32(bigEndian);
  ev.z = is->readES32(bigEndian);
  ev.wheel = is->readES32(bigEndian);

  handler->giiPointerEvent(ev);
}

void SMsgReader::readGIISubButtonEvent(rdr::U16 len, rdr::U8 eType,
                                       bool bigEndian)
{
  if (len != 10) {
    fprintf(stderr, "invalid GII button event message length %d\n", len);
    throw Exception("invalid GII button event message length");
  }
  is->skip(2); // padding

  GIIButtonEvent ev;
  ev.down = (eType == giiEventButtonPress);
  ev.devId = is->readEU32(bigEndian);
  ev.buttonNum = is->readEU32(bigEndian);

  handler->giiButtonEvent(ev);
}

void SMsgReader::readGIISubValuatorEvent(rdr::U16 len, rdr::U8 eType,
                                         bool bigEndian)
{
  if (len < 14) {
    fprintf(stderr, "invalid GII valuator event message length %d\n", len);
    throw Exception("invalid GII valuator event message length");
  }
  is->skip(2); // padding

  GIIValuatorEvent ev;
  ev.absolute = (eType == giiEventValuatorAbsolute);
  ev.devId = is->readEU32(bigEndian);
  ev.firstValuatorNum = is->readEU32(bigEndian);
  rdr::U32 valuatorCount = is->readEU32(bigEndian);
  if (len != 14 + valuatorCount*4) {
    fprintf(stderr, "invalid GII valuator event message length %d\n", len);
    throw Exception("invalid GII valuator event message length");
  }
  ev.values.resize(valuatorCount);
  for (unsigned int i = 0; i < valuatorCount; ++i) {
    ev.values[i] = is->readES32(bigEndian);
  }
  handler->giiValuatorEvent(ev);
}

void SMsgReader::readGIISubVersionAgreed(rdr::U16 len, bool bigEndian)
{
  if (len != 2) {
    fprintf(stderr, "invalid GII version message length %d\n", len);
    throw Exception("invalid GII version message length");
  }
  rdr::U16 version;
  version = is->readEU16(bigEndian);
  if (version != 1) {
    // This should not have happened; the client should only respond with
    // version numbers inside the server-provided range, or else not at all.
    fprintf(stderr, "invalid GII version %d\n", version);
    throw Exception("invalid GII version");
  }
  handler->giiVersionAgreed(version);
}

void SMsgReader::readGIISubDeviceCreate(rdr::U16 len, bool bigEndian)
{
  if (len < 56) {
    fprintf(stderr, "invalid GII device creation message length %d\n", len);
    throw Exception("invalid GII device creation message length");
  }
  GIIDevice dev;
  is->readBytes(&dev.name, 31);
  is->skip(1);
  dev.vendorId = is->readEU32(bigEndian);
  dev.productId = is->readEU32(bigEndian);
  dev.eventMask = is->readEU32(bigEndian);
  dev.numPointerAxes = is->readEU32(bigEndian);
  rdr::U32 numValuators = is->readEU32(bigEndian);
  if (len != 56 + numValuators*116) {
    fprintf(stderr, "invalid GII device creation message length %d\n", len);
    throw Exception("invalid GII device creation message length");
  }
  dev.valuators.resize(numValuators);
  dev.numButtons = is->readEU32(bigEndian);
  for (unsigned int i = 0; i < numValuators; ++i) {
    GIIValuator* v = &dev.valuators[i];
    is->readBytes(&(v->longName), 74);
    is->skip(1);
    is->readBytes(&(v->shortName), 4);
    is->skip(1);
    v->min = is->readES32(bigEndian);
    v->center = is->readES32(bigEndian);
    v->max = is->readES32(bigEndian);
    v->unit = is->readEU32(bigEndian);
    is->skip(16); // ignoring SI unit conversion stuff, don't need it
  }
  handler->giiDeviceCreate(dev);
}

void SMsgReader::readGIISubDeviceDestroy(rdr::U16 len, bool bigEndian)
{
  if (len != 4) {
    fprintf(stderr, "invalid GII device destruction message length %d\n", len);
    throw Exception("invalid GII device destruction message length");
  }
  rdr::U32 devId;
  devId = is->readEU32(bigEndian);
  handler->giiDeviceDestroy(devId);
}

void SMsgReader::readFence()
{
  rdr::U32 flags;
  rdr::U8 len;
  char data[64];

  is->skip(3);

  flags = is->readU32();

  len = is->readU8();
  if (len > sizeof(data)) {
    fprintf(stderr, "Ignoring fence with too large payload\n");
    is->skip(len);
    return;
  }

  is->readBytes(data, len);

  handler->fence(flags, len, data);
}

void SMsgReader::readKeyEvent()
{
  bool down = is->readU8();
  is->skip(2);
  rdr::U32 key = is->readU32();
  handler->keyEvent(key, down);
}

void SMsgReader::readPointerEvent()
{
  int mask = is->readU8();
  int x = is->readU16();
  int y = is->readU16();
  handler->pointerEvent(Point(x, y), mask);
}

void SMsgReader::readClientCutText()
{
  is->skip(3);
  int len = is->readU32();
  if (len > maxCutText) {
    is->skip(len);
    vlog.error("Cut text too long (%d bytes) - ignoring", len);
    return;
  }
  CharArray ca(len+1);
  ca.buf[len] = 0;
  is->readBytes(ca.buf, len);
  handler->clientCutText(ca.buf, len);
}

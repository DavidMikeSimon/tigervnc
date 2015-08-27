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

// GII input device types and constants

#ifndef __RFB_GIIDEVICE_H__
#define __RFB_GIIDEVICE_H__

#include <vector>

namespace rfb {
  const unsigned int siUnitUnknown = 0;
  const unsigned int siUnitTime = 1;
  const unsigned int siUnitFrequency = 2;
  const unsigned int siUnitLength = 3;
  const unsigned int siUnitVelocity = 4;
  const unsigned int siUnitAcceleration = 5;
  const unsigned int siUnitAngle = 6;
  const unsigned int siUnitAngularVelocity = 7;
  const unsigned int siUnitAngularAcceleration = 8;
  const unsigned int siUnitArea = 9;
  const unsigned int siUnitVolume = 10;
  const unsigned int siUnitMass = 11;
  const unsigned int siUnitForce = 12;
  const unsigned int siUnitPressure = 13;
  const unsigned int siUnitTorque = 14;
  const unsigned int siUnitEnergy = 15;
  const unsigned int siUnitPower = 16;
  const unsigned int siUnitTemperature = 17;
  const unsigned int siUnitCurrent = 18;
  const unsigned int siUnitVoltage = 19;
  const unsigned int siUnitResistance = 20;
  const unsigned int siUnitCapacity = 21;
  const unsigned int siUnitInductivity = 22;

  struct GIIValuator {
    GIIValuator();

    char longName[75];
    char shortName[5];
    int min;
    int center;
    int max;
    unsigned int unit;
  };

  const unsigned int giiEventMaskKeyPress = 0x00000020;
  const unsigned int giiEventMaskKeyRelease = 0x00000040;
  const unsigned int giiEventMaskKeyRepeat = 0x00000080;
  const unsigned int giiEventMaskPointerRelative = 0x00000100;
  const unsigned int giiEventMaskPointerAbsolute = 0x00000200;
  const unsigned int giiEventMaskButtonPress = 0x00000400;
  const unsigned int giiEventMaskButtonRelease = 0x00000800;
  const unsigned int giiEventMaskValuatorRelative = 0x00001000;
  const unsigned int giiEventMaskValuatorAbsolute = 0x00002000;

  struct GIIDevice {
    GIIDevice();

    char name[32];
    unsigned int vendorId;
    unsigned int productId;
    unsigned int eventMask;
    unsigned int numPointerAxes;
    unsigned int numButtons;

    std::vector<GIIValuator> valuators;
  };
}

#endif // __RFB_GIIDEVICE_H__

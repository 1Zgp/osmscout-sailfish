/*
  This source is part of the libosmscout library
  Copyright (C) 2012  Tim Teulings

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

#include <osmscout/system/Math.h>

#include <osmscout/util/Geometry.h>
#include <osmscout/util/Magnification.h>

namespace osmscout {

  void Magnification::SetMagnification(double magnification)
  {
    this->magnification=magnification;
    this->level=(uint32_t)log2(this->magnification);
  }

  void Magnification::SetMagnification(Mag magnification)
  {
    this->magnification=magnification;
    this->level=(uint32_t)log2(this->magnification);
  }

  void Magnification::SetLevel(uint32_t level)
  {
    this->magnification=Pow(2,level);
    this->level=level;
  }

    OSMSCOUT_HASHMAP<std::string,Magnification::Mag> stringToMagMap;

  MagnificationConverter::MagnificationConverter()
  {
    stringToMagMap[std::string("world")]=Magnification::magWorld;
    stringToMagMap[std::string("continent")]=Magnification::magContinent;
    stringToMagMap[std::string("state")]=Magnification::magState;
    stringToMagMap[std::string("stateOver")]=Magnification::magStateOver;
    stringToMagMap[std::string("county")]=Magnification::magCounty;
    stringToMagMap[std::string("region")]=Magnification::magRegion;
    stringToMagMap[std::string("proximity")]=Magnification::magProximity;
    stringToMagMap[std::string("cityOver")]=Magnification::magCityOver;
    stringToMagMap[std::string("city")]=Magnification::magCity;
    stringToMagMap[std::string("suburb")]=Magnification::magSuburb;
    stringToMagMap[std::string("detail")]=Magnification::magDetail;
    stringToMagMap[std::string("close")]=Magnification::magClose;
    stringToMagMap[std::string("closer")]=Magnification::magCloser;
    stringToMagMap[std::string("veryClose")]=Magnification::magVeryClose;
    stringToMagMap[std::string("block")]=Magnification::magBlock;
    stringToMagMap[std::string("street")]=Magnification::magStreet;
    stringToMagMap[std::string("house")]=Magnification::magHouse;

    levelToStringMap[(uint32_t)log2(Magnification::magWorld)]="world";
    levelToStringMap[(uint32_t)log2(Magnification::magContinent)]="continent";
    levelToStringMap[(uint32_t)log2(Magnification::magState)]="state";
    levelToStringMap[(uint32_t)log2(Magnification::magStateOver)]="stateOver";
    levelToStringMap[(uint32_t)log2(Magnification::magCounty)]="county";
    levelToStringMap[(uint32_t)log2(Magnification::magRegion)]="region";
    levelToStringMap[(uint32_t)log2(Magnification::magProximity)]="proximity";
    levelToStringMap[(uint32_t)log2(Magnification::magCityOver)]="cityOver";
    levelToStringMap[(uint32_t)log2(Magnification::magCity)]="city";
    levelToStringMap[(uint32_t)log2(Magnification::magSuburb)]="suburb";
    levelToStringMap[(uint32_t)log2(Magnification::magDetail)]="detail";
    levelToStringMap[(uint32_t)log2(Magnification::magClose)]="close";
    levelToStringMap[(uint32_t)log2(Magnification::magCloser)]="closer";
    levelToStringMap[(uint32_t)log2(Magnification::magVeryClose)]="veryClose";
    levelToStringMap[(uint32_t)log2(Magnification::magBlock)]="block";
    levelToStringMap[(uint32_t)log2(Magnification::magStreet)]="street";
    levelToStringMap[(uint32_t)log2(Magnification::magHouse)]="house";
  }

  bool MagnificationConverter::Convert(const std::string& name,
                                       Magnification& magnification)
  {
    OSMSCOUT_HASHMAP<std::string,Magnification::Mag>::const_iterator entry=stringToMagMap.find(name);

    if (entry==stringToMagMap.end()) {
      return false;
    }

    magnification.SetMagnification(entry->second);

    return true;
  }

  bool MagnificationConverter::Convert(size_t level,
                                       std::string& name)
  {
    OSMSCOUT_HASHMAP<size_t,std::string>::const_iterator entry=levelToStringMap.find(level);

    if (entry==levelToStringMap.end()) {
      return false;
    }

    name=entry->second;

    return true;
  }
}

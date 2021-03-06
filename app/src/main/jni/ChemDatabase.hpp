/*  NDKmol - Molecular Viewer on Android NDK

     (C) Copyright 2011 - 2012, biochem_fan

     This file is part of NDKmol.

     NDKmol is free software: you can redistribute it and/or modify
     it under the terms of the GNU Lesser General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU Lesser General Public License for more details.

     You should have received a copy of the GNU Lesser General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef CHEMDATABASE_INCLUDED
#define CHEMDATABASE_INCLUDED

#include "Color.hpp"
#include <map>
#include <string>

class ChemDatabase {
private:
	static std::map<std::string, unsigned int> ElementColors;
	static std::map<std::string, float> vdwRadii;
	static bool ready;
	static void prepare();

public:
	static const unsigned int defaultColor = 0xCCCCCC;
	static const float defaultRadius; // initialization moved to .cpp to avoid use of GNU extension
	static Color getColor(const std::string& elem);
	static float getVdwRadius(const std::string& elem);
};

#endif

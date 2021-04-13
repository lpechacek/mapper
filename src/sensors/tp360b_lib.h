/*
*    Copyright 2019 Libor Pecháček
*
*    This file is part of OpenOrienteering.
*
*    OpenOrienteering is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    OpenOrienteering is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with OpenOrienteering.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LIBTP360B_H
#define LIBTP360B_H

#include <QString>

namespace LibTP360B
{
	enum MeasurementType { Invalid, HorizontalDistance, 
		                   MissingLine, Inclination, Azimuth};
	
	struct Measurement
	{
		MeasurementType type { Invalid };
		double horizontalDistance; /// meters
		double azimuth; /// degrees
		double inclination; /// degrees
		double straightDistance; /// meters
		double distanceAccuracy; /// estimated distance precision in meters
	};
	
	bool operator==(const Measurement a, const Measurement b);
	
	bool checksumNmeaSentence(const QString& data);
	Measurement parseNmeaSentence(const QString& data);
}

#endif // LIBTP360B_H

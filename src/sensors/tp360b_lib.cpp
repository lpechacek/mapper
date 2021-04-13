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

#include "tp360b_lib.h"

#include <algorithm>
#include <iterator>
#include <map>

#include <QByteArray>
#include <QChar>
#include <QList>
#include <QStringList>
#include <QStringRef>
#include <QVector>
#include <QtGlobal>

/* Based on information from Laser Technology, Inc TruPulse 360 / 360B
 * User’s Manual, Section 8 - Serial Data Interface */

namespace {
/**
 * @brief parseNumber converts string received from the laser ranger to a pair
 * of double and bool. The bool is an inverse of the second digit in decimal
 * part.
 * @param data Raw number string received from device.
 * @return Distance to target in meters and target quality estimate.
 * 
 * See Laser Technology, Inc TruPulse 360 / 360B User’s Manual
 * for details about message format
 */
std::pair<double, bool> parseDistanceValue(const QString& data)
{
	auto const parts = data.splitRef('.');
	bool conversion_ok_1;
	bool conversion_ok_2;
	auto const value1 = parts[0].toInt(&conversion_ok_1);
	auto const value2 = parts[1].toInt(&conversion_ok_2);

	if (!conversion_ok_1 || !conversion_ok_2 || value2 < 0)
		return { 0, false };

	// last digit signals low quality target in distance measurements
	bool quality_target = !(value2 % 10);
	
	return { double(value1) + double(value2/10)/10, quality_target };
}


quint32 normalizeDistanceUnits(const double value, const QString unit)
{
	switch (unit[0].toLatin1())
	{
	case 'M':
		return value;
	case 'Y':
		return value * 0.9144;
	case 'F':
		return value * 0.3048;
	default:
		// FIXME throw
		return 0;
	}
}

// Sanity check - degrees are the only possible unit 
// for azimuth and inclination
// this function is supposed to throw
bool checkAngularUnits(const QString& field)
{
	if (field.size() != 1 || field[0] != 'D')
	{
		qDebug("Encountered unexpected angular unit");
		// FIXME throw
		return false;
	}
	return true;
}

} // anonymous namespace


/// C&P from QLocationUtils::hasValidNmeaChecksum
bool LibTP360B::checksumNmeaSentence(const QString& data)
{
	const auto asteriskIndex = data.indexOf('*');
	const int CSUM_LEN = 2;

	if (asteriskIndex < 0 || asteriskIndex + CSUM_LEN >= data.size())
		return false;
	
	// XOR byte value of all characters between '$' and '*'
	quint8 result = 0; 
	std::for_each(std::begin(data) + 1,
	              std::begin(data) + (asteriskIndex),
	              [&result](const QChar ch) { result ^= ch.toLatin1(); });
	
	bool ok = false;
	const auto checksum = data.mid(asteriskIndex+1, 2).toInt(&ok, 16);
	return ok && checksum == result;
}


LibTP360B::Measurement LibTP360B::parseNmeaSentence(const QString& data)
{
	if (!checksumNmeaSentence(data))
	{
		qDebug("Invalid checksum detected on \"%s\"",
		       data.toLatin1().constData());
		// throw
		return {};
	}
	
	// $PLTIT,HV,18.00,F,185.20,D,6.90,D,18.00,F*66
	// HDvalues, INCvalues, and SDvalues always include two decimal places:
	// XX.YY
    // last digit in YY signifies target quality
	//     0 = high quality target
	//     1 = low quality target
	
	const auto parts = data.split(',');
	
	// handle HV (horizontal vector), ML (missing line), ignore HT (height)
	// FIXME throw
	if (parts[0] != "$PLTIT")
		return {};
	
	Measurement ret {};
	
	if (parts[1] == "HV")
	{
		// handle horizontal vector
		// $PLTIT,HV,HDvalue,units,AZvalue,units,INCvalue,units,SDvalue,units,*csum<CR><LF>
		// HD, SD and AZ values may be missing determining the measurement type
		if (parts[4].size())
		{
			ret.type = MeasurementType::Azimuth;
			ret.azimuth = parseDistanceValue(parts[4]).first;
			checkAngularUnits(parts[5]);
		}

		if (parts[6].size())
		{
			ret.type = MeasurementType::Inclination;
			ret.inclination = parseDistanceValue(parts[6]).first;
			checkAngularUnits(parts[7]);
		}

		if (parts[2].size())
		{
			ret.type = MeasurementType::HorizontalDistance;
			const auto hd_pair = parseDistanceValue(parts[2]);
			ret.horizontalDistance = normalizeDistanceUnits(hd_pair.first, parts[3]);
			ret.straightDistance = normalizeDistanceUnits(parseDistanceValue(parts[8]).first, parts[9]);
			ret.distanceAccuracy = hd_pair.second ? .3 : 1; 
		}
	} 
	else if (parts[1] == "ML") 
	{
		// handle missing line
		// $PLTIT,ML,HD,HDunits,AZ,AZunits,INC,INCunits,SD,SDunits*csum<CR><LF>
		ret.type = MeasurementType::MissingLine;
		const auto hd_pair = parseDistanceValue(parts[2]);
		ret.horizontalDistance = normalizeDistanceUnits(hd_pair.first, parts[3]);
		ret.azimuth = parseDistanceValue(parts[4]).first;
		checkAngularUnits(parts[5]);		
		ret.inclination = parseDistanceValue(parts[6]).first;
		checkAngularUnits(parts[7]);		
		ret.straightDistance = normalizeDistanceUnits(parseDistanceValue(parts[8]).first, parts[9]);
		ret.distanceAccuracy = hd_pair.second ? .3 : 1; 
	}
	
	return ret;
}


bool LibTP360B::operator==(const LibTP360B::Measurement a, const LibTP360B::Measurement b)
{
	return a.type == b.type
	        && a.horizontalDistance == b.horizontalDistance
	        && a.azimuth == b.azimuth
	        && a.inclination == b.inclination
	        && a.straightDistance == b.straightDistance
	        && a.distanceAccuracy == b.distanceAccuracy;
}

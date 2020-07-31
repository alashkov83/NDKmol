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

#include "Vector3.hpp"
#include "Line.hpp"
#include "SmoothTube.hpp"
#include <cmath>
#include <cstdlib>

SmoothTube::SmoothTube(std::vector<Vector3> &_points, std::vector<Color> &colors, std::vector<float> &radii)
 : Renderable() {
	if (_points.size() < 2) return;

	int circleDiv = 6, axisDiv = 3;

	float *points = subdivide(_points, axisDiv);
	int nDividedPoints = ((_points.size() - 1) * axisDiv + 1);
	_points.clear();

	auto *vertices = new float[nDividedPoints * 3 * circleDiv];
	auto *normals = new float[nDividedPoints * 3 * circleDiv];
	auto *faces = new unsigned short[(nDividedPoints - 1) * circleDiv * 2 * 3];
	int voffset = 0;
	int foffset = 0;

	Vector3 prevAxis1(1, 0, 0), prevAxis2(0, 1, 0);

	for (int i = 0, lim = nDividedPoints; i < lim; i++) {
		int poffset = i * 3;
		float r;
		if (i == 0) {
			r = radii[i];
		} else {
			int idx = (i - 1) / axisDiv;
			if (idx * axisDiv == i - 1) {
				r = radii[idx];
			} else {
				float tmp = i - 1 - idx * axisDiv;
				r = radii[idx] * tmp + radii[idx + 1] * (1 - tmp);
			}
		}
		Vector3 delta, axis1, axis2;

		if (i < lim - 1) {
			delta.x = points[poffset] - points[poffset + 3];
			delta.y = points[poffset + 1] - points[poffset + 4];
			delta.z = points[poffset + 2] - points[poffset + 5];
			axis1.x = 0; axis1.y = - delta.z; axis1.z = delta.y;
			axis1.normalize().multiplyScalar(r);
			axis2 = Vector3::cross(delta, axis1).normalize().multiplyScalar(r);
			if (Vector3::dot(prevAxis1, axis1) < 0) {
				axis1.multiplyScalar(-1); axis2.multiplyScalar(-1);
			}
			prevAxis1 = axis1; prevAxis2 = axis2;
		} else {
			axis1 = prevAxis1; axis2 = prevAxis2;
		}

		for (int j = 0; j < circleDiv; j++) {
			auto angle = (float)(2 * M_PI / circleDiv * j);
			auto c = (float)std::cos(angle), s = (float)std::sin(angle);
			normals[voffset] = c * axis1.x + s * axis2.x;
			vertices[voffset] = points[poffset] + normals[voffset];
			voffset++;
			normals[voffset] = c * axis1.y + s * axis2.y;
			vertices[voffset] = points[poffset + 1] + normals[voffset];
			voffset++;
			normals[voffset] = c * axis1.z + s * axis2.z;
			vertices[voffset] = points[poffset + 2] + normals[voffset];
			voffset++;
		}
	}

	voffset = 0;
	for (int i = 0, lim = nDividedPoints - 1; i < lim; i++) {
        int reg = 0;
        int vo1 = voffset * 3, vo2 = vo1 + circleDiv * 3;
        auto r1 = (float) Vector3::norm(vertices[vo1] - vertices[vo2],
                                        vertices[vo1 + 1] - vertices[vo2 + 1],
                                        vertices[vo1 + 2] - vertices[vo2 + 2]);
        auto r2 = (float) Vector3::norm(vertices[vo1] - vertices[vo2 + 3],
                                        vertices[vo1 + 1] - vertices[vo2 + 4],
                                        vertices[vo1 + 2] - vertices[vo2 + 5]);
        if (r1 > r2) { reg = 1; }
        for (int j = 0; j < circleDiv; j++) {
            faces[foffset++] = (short) (voffset + j);
            faces[foffset++] = (short) (voffset + (j + reg) % circleDiv + circleDiv);
            faces[foffset++] = (short) (voffset + (j + 1) % circleDiv);
            faces[foffset++] = (short) (voffset + (j + 1) % circleDiv);
            faces[foffset++] = (short) (voffset + (j + reg) % circleDiv + circleDiv);
            faces[foffset++] = (short) (voffset + (j + reg + 1) % circleDiv + circleDiv);
        }
        voffset += circleDiv;
    }

	delete [] points;

	vertexBuffer = vertices;
	colorBuffer = colorVectorToFloatArray(colors, axisDiv * circleDiv);
	nFaces = (nDividedPoints - 1) * circleDiv * 2 * 3;
    nVertices = nDividedPoints * circleDiv * 3;
	vertexColors = true;
	faceBuffer = faces;
	vertexNormalBuffer = normals;
}

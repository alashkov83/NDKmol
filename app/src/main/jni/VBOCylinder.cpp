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

#include "VBOCylinder.hpp"
#include "CylinderGeometry.hpp"
#include "GLES.hpp"
#include <cmath>

int VBOCylinder::faceVBO = -1, VBOCylinder::vertexVBO = -1, VBOCylinder::vertexNormalVBO = -1, VBOCylinder::faceCount = 0;

VBOCylinder::VBOCylinder() = default;

VBOCylinder::VBOCylinder(float x1, float y1, float z1, float x2, float y2, float z2, float radius, Color color) {
	objectColor = color;

	double dist = std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + (z1 - z2) * (z1 - z2));
	if (dist < 0.001) return;

	posx = x1; posy = y1; posz = z1;
	if (std::abs(x1 - x2) > 0.0001 || std::abs(y1 - y2) > 0.001){
		rot = (float) (180 / M_PI * std::acos((z2 - z1) / dist));
		rotx = y1 - y2;
		roty = x2 - x1;
		rotz = 0;
	} else {
		rot = (float) (180 / M_PI * std::acos((z2 - z1) / dist));
		rotx = 1;
		roty = 0;
		rotz = 0;
	}

	scalex = scaley = radius; scalez = (float) dist;
}

void VBOCylinder::prepareVBO() {
	GLuint vbo[3];
	glGenBuffers(3, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    
	glBufferData(GL_ARRAY_BUFFER, CylinderGeometry::getnVertices() * 3 * 4, CylinderGeometry::getVertexBuffer(), GL_STATIC_DRAW);
	vertexVBO = vbo[0];
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, CylinderGeometry::getnVertices() * 3 * 4, CylinderGeometry::getVertexNormalBuffer(), GL_STATIC_DRAW);
	vertexNormalVBO = vbo[1];
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	faceCount = CylinderGeometry::getnFaces();
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER, faceCount * 3 * 2, CylinderGeometry::getFaceBuffer(), GL_STATIC_DRAW);
	faceVBO = vbo[2];
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
}

void VBOCylinder::render() {
    if (vertexVBO == -1) {
        prepareVBO();
    }
	glPushMatrix();
	setMatrix();
    
#ifdef OPENGL_ES1
	glColor4f(objectColor.r, objectColor.g, objectColor.b, objectColor.a);
	glDisableClientState(GL_COLOR_ARRAY);
#else
    glDisableVertexAttribArray(shaderVertexColor);
    glVertexAttrib4f(shaderVertexColor, objectColor.r, objectColor.g, objectColor.b, objectColor.a);
#endif

	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
#ifdef OPENGL_ES1
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, nullptr);
#else
	glEnableVertexAttribArray(shaderVertexPosition);
    glVertexAttribPointer(shaderVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
#endif

	glBindBuffer(GL_ARRAY_BUFFER, vertexNormalVBO);
#ifdef OPENGL_ES1
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, 0, nullptr);
#else
	glEnableVertexAttribArray(shaderVertexNormal);
    glVertexAttribPointer(shaderVertexNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
#endif
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceVBO);
	glDrawElements(GL_TRIANGLES, faceCount * 3, GL_UNSIGNED_SHORT, nullptr);

#ifdef OPENGL_ES1
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
#endif
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glPopMatrix();
}

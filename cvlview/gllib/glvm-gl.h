/*
 * glvm-gl.h
 * 
 * C++ vector and matrix classes that resemble GLSL style.
 *
 * Copyright (C) 2008  Martin Lambers <marlam@marlam.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * This file provides functions to use GLVM with OpenGL.
 */

#ifndef GLVM_GL_H
#define GLVM_GL_H

#include <GL/gl.h>

#include "glvm.h"

namespace glvm
{
    /* Get matrices */

    mat4 glGetMatrix(const GLenum matrixMode)
    {
	mat4 M;
	glGetFloatv(matrixMode, M.vl);
   	return M.transpose();
    }

    mat4 glGetProjectionMatrix()
    {
	return glGetMatrix(GL_PROJECTION_MATRIX);
    }

    mat4 glGetModelViewMatrix()
    {
	return glGetMatrix(GL_MODELVIEW_MATRIX);
    }

    mat4 glGetTextureMatrix()
    {
	return glGetMatrix(GL_TEXTURE_MATRIX);
    }

    mat4 glGetModelViewProjectionMatrix()
    {
	return glGetProjectionMatrix() * glGetModelViewMatrix();
    }

    dmat4 glGetMatrixd(const GLenum matrixMode)
    {
	dmat4 M;
	glGetDoublev(matrixMode, M.vl);
   	return M.transpose();
    }

    dmat4 glGetProjectionMatrixd()
    {
	return glGetMatrixd(GL_PROJECTION_MATRIX);
    }

    dmat4 glGetModelViewMatrixd()
    {
	return glGetMatrixd(GL_MODELVIEW_MATRIX);
    }

    dmat4 glGetTextureMatrixd()
    {
	return glGetMatrixd(GL_TEXTURE_MATRIX);
    }

    dmat4 glGetModelViewProjectionMatrixd()
    {
	return glGetProjectionMatrixd() * glGetModelViewMatrixd();
    }

    /* Load matrices */

    void glLoadMatrix(const mat4 &M)
    {
	glLoadMatrixf(transpose(M).vl);
    }

    void glLoadMatrix(const mat4 &M, const GLint matrixMode)
    {
	GLint matrixModeBak;
	glGetIntegerv(GL_MATRIX_MODE, &matrixModeBak);
	glMatrixMode(matrixMode);
	glLoadTransposeMatrixf(M.vl);
	glMatrixMode(matrixModeBak);
    }

    void glLoadProjectionMatrix(const mat4 &M)
    {
	glLoadMatrix(M, GL_PROJECTION);
    }

    void glLoadModelViewMatrix(const mat4 &M)
    {
	glLoadMatrix(M, GL_MODELVIEW);
    }

    void glLoadTextureMatrix(const mat4 &M)
    {
	glLoadMatrix(M, GL_TEXTURE);
    }

    void glLoadMatrix(const dmat4 &M)
    {
	glLoadMatrixd(transpose(M).vl);
    }

    void glLoadMatrix(const dmat4 &M, const GLint matrixMode)
    {
	GLint matrixModeBak;
	glGetIntegerv(GL_MATRIX_MODE, &matrixModeBak);
	glMatrixMode(matrixMode);
	glLoadTransposeMatrixd(M.vl);
	glMatrixMode(matrixModeBak);
    }

    void glLoadProjectionMatrix(const dmat4 &M)
    {
	glLoadMatrix(M, GL_PROJECTION);
    }

    void glLoadModelViewMatrix(const dmat4 &M)
    {
	glLoadMatrix(M, GL_MODELVIEW);
    }

    void glLoadTextureMatrix(const dmat4 &M)
    {
	glLoadMatrix(M, GL_TEXTURE);
    }

    /* Manipulate matrices */

    void glTranslate(const vec3 &v)
    {
	glTranslatef(v.x, v.y, v.z);
    }

    void glTranslate(const dvec3 &v)
    {
	glTranslated(v.x, v.y, v.z);
    }

    void glScale(const vec3 &s)
    {
	glScalef(s.x, s.y, s.z);
    }

    void glScale(const dvec3 &s)
    {
	glScaled(s.x, s.y, s.z);
    }

    void glRotate(const float angle, const vec3 &xyz)
    {
	glRotatef(angle, xyz.x, xyz.y, xyz.z);
    }

    void glRotate(const double angle, const dvec3 &xyz)
    {
	glRotated(angle, xyz.x, xyz.y, xyz.z);
    }

    void glMultMatrix(const mat4 &M)
    {
	glMultTransposeMatrixf(M.vl);
    }

    void glMultMatrix(const dmat4 &M)
    {
	glMultTransposeMatrixd(M.vl);
    }

    /* Miscellaneous */

    void glColor(const vec3 &c)
    {
	glColor3f(c.r, c.g, c.b);
    }

    void glColor(const dvec3 &c)
    {
	glColor3d(c.r, c.g, c.b);
    }

    void glColor(const vec4 &c)
    {
	glColor4f(c.r, c.g, c.b, c.a);
    }

    void glColor(const dvec4 &c)
    {
	glColor4d(c.r, c.g, c.b, c.a);
    }
}

#endif

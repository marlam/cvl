/*
 * glvm-gl.h
 * 
 * C++ vector and matrix classes that resemble GLSL style.
 *
 * Copyright (C) 2008, 2009  Martin Lambers <marlam@marlam.de>
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
 * This file provides functions to use the GLVM data types with OpenGL.
 * It requires the following extensions:
 * GL_ARB_transpose_matrix
 * GL_EXT_direct_state_access
 * GL_EXT_gpu_shader4
 */

#ifndef GLVM_GL_H
#define GLVM_GL_H

#include <GL/glew.h>

#include "glvm.h"

namespace glvm
{
    /* Viewport */

    inline void glvmViewport(const ivec4 &vp)
    {
	glViewport(vp[0], vp[1], vp[2], vp[3]);
    }

    inline void glvmGetViewport(ivec4 &vp)
    {
	glGetIntegerv(GL_VIEWPORT, vp.vl);
    }

    /* Get matrices */

    inline void glvmGetProjectionMatrix(mat4 &M)
    {
	glGetFloatv(GL_PROJECTION_MATRIX, M.vl);
    }

    inline void glvmGetProjectionMatrix(dmat4 &M)
    {
	glGetDoublev(GL_PROJECTION_MATRIX, M.vl);
    }

    inline void glvmGetModelViewMatrix(mat4 &M)
    {
	glGetFloatv(GL_MODELVIEW_MATRIX, M.vl);
    }

    inline void glvmGetModelViewMatrix(dmat4 &M)
    {
	glGetDoublev(GL_MODELVIEW_MATRIX, M.vl);
    }

    inline void glvmGetTextureMatrix(mat4 &M)
    {
	glGetFloatv(GL_TEXTURE_MATRIX, M.vl);
    }

    inline void glvmGetTextureMatrix(dmat4 &M)
    {
	glGetDoublev(GL_TEXTURE_MATRIX, M.vl);
    }

    inline void glvmGetModelViewProjectionMatrix(mat4 &M)
    {
	mat4 p, m;
	glvmGetProjectionMatrix(p);
	glvmGetModelViewMatrix(m);
	M = p * m;
    }

    inline void glvmGetModelViewProjectionMatrix(dmat4 &M)
    {
	dmat4 p, m;
	glvmGetProjectionMatrix(p);
	glvmGetModelViewMatrix(m);
	M = p * m;
    }

    /* Load matrices */

    inline void glvmLoadMatrix(const mat4 &M)
    {
	glLoadMatrixf(M.vl);
    }

    inline void glvmLoadMatrix(const dmat4 &M)
    {
	glLoadMatrixd(M.vl);
    }

    inline void glvmMatrixLoad(GLenum matrix_mode, const mat4 &M)
    {
	glMatrixLoadfEXT(matrix_mode, M.vl);
    }

    inline void glvmMatrixLoad(GLenum matrix_mode, const dmat4 &M)
    {
	glMatrixLoaddEXT(matrix_mode, M.vl);
    }

    inline void glvmMatrixLoadTranspose(GLenum matrix_mode, const mat4 &M)
    {
	glMatrixLoadTransposefEXT(matrix_mode, M.vl);
    }

    inline void glvmMatrixLoadTranspose(GLenum matrix_mode, const dmat4 &M)
    {
	glMatrixLoadTransposedEXT(matrix_mode, M.vl);
    }

    /* Manipulate matrices */

    inline void glvmFrustum(const frust &f)
    {
	glFrustum(f.l(), f.r(), f.b(), f.t(), f.n(), f.f());
    }

    inline void glvmMatrixFrustum(GLenum matrix_mode, const frust &f)
    {
	glMatrixFrustumEXT(matrix_mode, f.l(), f.r(), f.b(), f.t(), f.n(), f.f());
    }

    inline void glvmFrustum(const dfrust &f)
    {
	glFrustum(f.l(), f.r(), f.b(), f.t(), f.n(), f.f());
    }

    inline void glvmMatrixFrustum(GLenum matrix_mode, const dfrust &f)
    {
	glMatrixFrustumEXT(matrix_mode, f.l(), f.r(), f.b(), f.t(), f.n(), f.f());
    }

    inline void glvmTranslate(const vec3 &v)
    {
	glTranslatef(v.x(), v.y(), v.z());
    }

    inline void glvmMatrixTranslate(GLenum matrix_mode, const vec3 &v)
    {
	glMatrixTranslatefEXT(matrix_mode, v.x(), v.y(), v.z());
    }

    inline void glvmTranslate(const dvec3 &v)
    {
	glTranslated(v.x(), v.y(), v.z());
    }

    inline void glvmMatrixTranslate(GLenum matrix_mode, const dvec3 &v)
    {
	glMatrixTranslatedEXT(matrix_mode, v.x(), v.y(), v.z());
    }

    inline void glvmScale(const vec3 &s)
    {
	glScalef(s.x(), s.y(), s.z());
    }

    inline void glvmMatrixScale(GLenum matrix_mode, const vec3 &s)
    {
	glMatrixScalefEXT(matrix_mode, s.x(), s.y(), s.z());
    }

    inline void glvmScale(const dvec3 &s)
    {
	glScaled(s.x(), s.y(), s.z());
    }

    inline void glvmMatrixScale(GLenum matrix_mode, const dvec3 &s)
    {
	glMatrixScaledEXT(matrix_mode, s.x(), s.y(), s.z());
    }

    inline void glvmRotate(const float angle, const vec3 &xyz)
    {
	glRotatef(angle, xyz.x(), xyz.y(), xyz.z());
    }

    inline void glvmMatrixRotate(GLenum matrix_mode, const float angle, const vec3 &xyz)
    {
	glMatrixRotatefEXT(matrix_mode, angle, xyz.x(), xyz.y(), xyz.z());
    }

    inline void glvmRotate(const double angle, const dvec3 &xyz)
    {
	glRotated(angle, xyz.x(), xyz.y(), xyz.z());
    }

    inline void glvmMatrixRotate(GLenum matrix_mode, const double angle, const dvec3 &xyz)
    {
	glMatrixRotatedEXT(matrix_mode, angle, xyz.x(), xyz.y(), xyz.z());
    }

    inline void glvmRotate(const quat &q)
    {
	glMultMatrixf(toMat4(q).vl);
    }

    inline void glvmMatrixRotate(GLenum matrix_mode, const quat &q)
    {
	glMatrixMultfEXT(matrix_mode, toMat4(q).vl);
    }

    inline void glvmRotate(const dquat &q)
    {
	glMultMatrixd(toMat4(q).vl);
    }

    inline void glvmMatrixRotate(GLenum matrix_mode, const dquat &q)
    {
	glMatrixMultdEXT(matrix_mode, toMat4(q).vl);
    }

    inline void glvmMultMatrix(const mat4 &M)
    {
	glMultMatrixf(M.vl);
    }

    inline void glvmMatrixMult(GLenum matrix_mode, const mat4 &M)
    {
	glMatrixMultfEXT(matrix_mode, M.vl);
    }

    inline void glvmMultMatrix(const dmat4 &M)
    {
	glMultMatrixd(M.vl);
    }

    inline void glvmMatrixMult(GLenum matrix_mode, const dmat4 &M)
    {
	glMatrixMultdEXT(matrix_mode, M.vl);
    }

    inline void glvmMultTransposeMatrix(const mat4 &M)
    {
	glMultTransposeMatrixf(M.vl);
    }

    inline void glvmMatrixMultTranspose(GLenum matrix_mode, const mat4 &M)
    {
	glMatrixMultTransposefEXT(matrix_mode, M.vl);
    }

    inline void glvmMultTransposeMatrix(const dmat4 &M)
    {
	glMultTransposeMatrixd(M.vl);
    }

    inline void glvmMatrixMultTranspose(GLenum matrix_mode, const dmat4 &M)
    {
	glMatrixMultTransposedEXT(matrix_mode, M.vl);
    }

    /* Colors */

    inline void glvmColor(const ivec3 &c)
    {
	glColor3iv(c.vl);
    }

    inline void glvmColor(const uvec3 &c)
    {
	glColor3uiv(c.vl);
    }

    inline void glvmColor(const vec3 &c)
    {
	glColor3fv(c.vl);
    }

    inline void glvmColor(const dvec3 &c)
    {
	glColor3dv(c.vl);
    }

    inline void glvmColor(const ivec4 &c)
    {
	glColor4iv(c.vl);
    }

    inline void glvmColor(const vec4 &c)
    {
	glColor4fv(c.vl);
    }

    inline void glvmColor(const dvec4 &c)
    {
	glColor4dv(c.vl);
    }

    inline void glvmSecondaryColor(const ivec3 &c)
    {
	glSecondaryColor3iv(c.vl);
    }

    inline void glvmSecondaryColor(const uvec3 &c)
    {
	glSecondaryColor3uiv(c.vl);
    }

    inline void glvmSecondaryColor(const vec3 &c)
    {
	glSecondaryColor3fv(c.vl);
    }

    inline void glvmSecondaryColor(const dvec3 &c)
    {
	glSecondaryColor3dv(c.vl);
    }

    /* Vertices */

    inline void glvmVertex(const ivec2 &v)
    {
	glVertex2iv(v.vl);
    }

    inline void glvmVertex(const vec2 &v)
    {
	glVertex2fv(v.vl);
    }

    inline void glvmVertex(const dvec2 &v)
    {
	glVertex2dv(v.vl);
    }

    inline void glvmVertex(const ivec3 &v)
    {
	glVertex3iv(v.vl);
    }

    inline void glvmVertex(const vec3 &v)
    {
	glVertex3fv(v.vl);
    }

    inline void glvmVertex(const dvec3 &v)
    {
	glVertex3dv(v.vl);
    }

    inline void glvmVertex(const ivec4 &v)
    {
	glVertex4iv(v.vl); 
    }

    inline void glvmVertex(const vec4 &v)
    {
	glVertex4fv(v.vl); 
    }

    inline void glvmVertex(const dvec4 &v)
    {
	glVertex4dv(v.vl); 
    }

    /* Normals */

    inline void glvmNormal(const ivec3 &v)
    {
	glNormal3iv(v.vl);
    }

    inline void glvmNormal(const vec3 &v)
    {
	glNormal3fv(v.vl);
    }

    inline void glvmNormal(const dvec3 &v)
    {
	glNormal3dv(v.vl);
    }

    /* TexCoords */

    inline void glvmTexCoord(const int32_t v)
    {
	glTexCoord1i(v);
    }

    inline void glvmTexCoord(const float v)
    {
	glTexCoord1f(v);
    }

    inline void glvmTexCoord(const double v)
    {
	glTexCoord1d(v);
    }

    inline void glvmTexCoord(const ivec2 &v)
    {
	glTexCoord2iv(v.vl);
    }

    inline void glvmTexCoord(const vec2 &v)
    {
	glTexCoord2fv(v.vl);
    }

    inline void glvmTexCoord(const dvec2 &v)
    {
	glTexCoord2dv(v.vl);
    }

    inline void glvmTexCoord(const ivec3 &v)
    {
	glTexCoord3iv(v.vl);
    }

    inline void glvmTexCoord(const vec3 &v)
    {
	glTexCoord3fv(v.vl);
    }

    inline void glvmTexCoord(const dvec3 &v)
    {
	glTexCoord3dv(v.vl);
    }

    inline void glvmTexCoord(const ivec4 &v)
    {
	glTexCoord4iv(v.vl); 
    }

    inline void glvmTexCoord(const vec4 &v)
    {
	glTexCoord4fv(v.vl); 
    }

    inline void glvmTexCoord(const dvec4 &v)
    {
	glTexCoord4dv(v.vl); 
    }

    /* MultiTexCoords */

    inline void glvmMultiTexCoord(const GLenum target, const int32_t v)
    {
	glMultiTexCoord1i(target, v);
    }

    inline void glvmMultiTexCoord(const GLenum target, const float v)
    {
	glMultiTexCoord1f(target, v);
    }

    inline void glvmMultiTexCoord(const GLenum target, const double v)
    {
	glMultiTexCoord1d(target, v);
    }

    inline void glvmMultiTexCoord(const GLenum target, const ivec2 &v)
    {
	glMultiTexCoord2iv(target, v.vl);
    }

    inline void glvmMultiTexCoord(const GLenum target, const vec2 &v)
    {
	glMultiTexCoord2fv(target, v.vl);
    }

    inline void glvmMultiTexCoord(const GLenum target, const dvec2 &v)
    {
	glMultiTexCoord2dv(target, v.vl);
    }

    inline void glvmMultiTexCoord(const GLenum target, const ivec3 &v)
    {
	glMultiTexCoord3iv(target, v.vl);
    }

    inline void glvmMultiTexCoord(const GLenum target, const vec3 &v)
    {
	glMultiTexCoord3fv(target, v.vl);
    }

    inline void glvmMultiTexCoord(const GLenum target, const dvec3 &v)
    {
	glMultiTexCoord3dv(target, v.vl);
    }

    inline void glvmMultiTexCoord(const GLenum target, const ivec4 &v)
    {
	glMultiTexCoord4iv(target, v.vl); 
    }

    inline void glvmMultiTexCoord(const GLenum target, const vec4 &v)
    {
	glMultiTexCoord4fv(target, v.vl); 
    }

    inline void glvmMultiTexCoord(const GLenum target, const dvec4 &v)
    {
	glMultiTexCoord4dv(target, v.vl); 
    }

    /* VertexAttribs */

    inline void glvmVertexAttrib(const GLuint index, const int32_t v)
    {
	glVertexAttribI1iEXT(index, v);
    }

    inline void glvmVertexAttrib(const GLuint index, const uint32_t v)
    {
	glVertexAttribI1uiEXT(index, v);
    }

    inline void glvmVertexAttrib(const GLuint index, const float v)
    {
	glVertexAttrib1f(index, v);
    }

    inline void glvmVertexAttrib(const GLuint index, const double v)
    {
	glVertexAttrib1d(index, v);
    }

    inline void glvmVertexAttrib(const GLuint index, const ivec2 &v)
    {
	glVertexAttribI2ivEXT(index, v.vl);
    }

    inline void glvmVertexAttrib(const GLuint index, const uvec2 &v)
    {
	glVertexAttribI2uivEXT(index, v.vl);
    }

    inline void glvmVertexAttrib(const GLuint index, const vec2 &v)
    {
	glVertexAttrib2fv(index, v.vl);
    }

    inline void glvmVertexAttrib(const GLuint index, const dvec2 &v)
    {
	glVertexAttrib2dv(index, v.vl);
    }

    inline void glvmVertexAttrib(const GLuint index, const ivec3 &v)
    {
	glVertexAttribI3ivEXT(index, v.vl);
    }

    inline void glvmVertexAttrib(const GLuint index, const uvec3 &v)
    {
	glVertexAttribI3uivEXT(index, v.vl);
    }

    inline void glvmVertexAttrib(const GLuint index, const vec3 &v)
    {
	glVertexAttrib3fv(index, v.vl);
    }

    inline void glvmVertexAttrib(const GLuint index, const dvec3 &v)
    {
	glVertexAttrib3dv(index, v.vl);
    }

    inline void glvmVertexAttrib(const GLuint index, const ivec4 &v)
    {
	glVertexAttribI4ivEXT(index, v.vl); 
    }

    inline void glvmVertexAttrib(const GLuint index, const uvec4 &v)
    {
	glVertexAttribI4uivEXT(index, v.vl); 
    }

    inline void glvmVertexAttrib(const GLuint index, const vec4 &v)
    {
	glVertexAttrib4fv(index, v.vl); 
    }

    inline void glvmVertexAttrib(const GLuint index, const dvec4 &v)
    {
	glVertexAttrib4dv(index, v.vl); 
    }


    /* glUniform */

    inline void glvmUniform(const GLint location, const int32_t v)
    {
	glUniform1i(location, v);
    }

    inline void glvmUniform(const GLint location, const uint32_t v)
    {
	glUniform1ui(location, v);
    }

    inline void glvmUniform(const GLint location, const float v)
    {
	glUniform1f(location, v);
    }

    inline void glvmUniform(const GLint location, const ivec2 &v)
    {
	glUniform2iv(location, 1, v.vl);
    }

    inline void glvmUniform(const GLint location, const uvec2 &v)
    {
	glUniform2uiv(location, 1, v.vl);
    }

    inline void glvmUniform(const GLint location, const vec2 &v)
    {
	glUniform2fv(location, 1, v.vl);
    }

    inline void glvmUniform(const GLint location, const ivec3 &v)
    {
	glUniform3iv(location, 1, v.vl);
    }

    inline void glvmUniform(const GLint location, const uvec3 &v)
    {
	glUniform3uiv(location, 1, v.vl);
    }

    inline void glvmUniform(const GLint location, const vec3 &v)
    {
	glUniform3fv(location, 1, v.vl);
    }

    inline void glvmUniform(const GLint location, const ivec4 &v)
    {
	glUniform4iv(location, 1, v.vl);
    }

    inline void glvmUniform(const GLint location, const uvec4 &v)
    {
	glUniform4uiv(location, 1, v.vl);
    }

    inline void glvmUniform(const GLint location, const vec4 &v)
    {
	glUniform4fv(location, 1, v.vl);
    }

    inline void glvmUniform(const GLint location, const GLsizei count, const int32_t *v)
    {
	glUniform1iv(location, count, v);
    }

    inline void glvmUniform(const GLint location, const GLsizei count, const float *v)
    {
	glUniform1fv(location, count, v);
    }

    inline void glvmUniform(const GLint location, const GLsizei count, const ivec2 *v)
    {
	glUniform2iv(location, count, v[0].vl);
    }

    inline void glvmUniform(const GLint location, const GLsizei count, const uvec2 *v)
    {
	glUniform2uiv(location, count, v[0].vl);
    }

    inline void glvmUniform(const GLint location, const GLsizei count, const vec2 *v)
    {
	glUniform2fv(location, count, v[0].vl);
    }

    inline void glvmUniform(const GLint location, const GLsizei count, const ivec3 *v)
    {
	glUniform3iv(location, count, v[0].vl);
    }

    inline void glvmUniform(const GLint location, const GLsizei count, const uvec3 *v)
    {
	glUniform3uiv(location, count, v[0].vl);
    }

    inline void glvmUniform(const GLint location, const GLsizei count, const vec3 *v)
    {
	glUniform3fv(location, count, v[0].vl);
    }

    inline void glvmUniform(const GLint location, const GLsizei count, const ivec4 *v)
    {
	glUniform4iv(location, count, v[0].vl);
    }

    inline void glvmUniform(const GLint location, const GLsizei count, const uvec4 *v)
    {
	glUniform4uiv(location, count, v[0].vl);
    }

    inline void glvmUniform(const GLint location, const GLsizei count, const vec4 *v)
    {
	glUniform4fv(location, count, v[0].vl);
    }

    inline void glvmUniform(const GLint location, const mat2 &m)
    {
	glUniformMatrix2fv(location, 1, GL_FALSE, m.vl);
    }

    inline void glvmUniform(const GLint location, const mat3 &m)
    {
	glUniformMatrix3fv(location, 1, GL_FALSE, m.vl);
    }

    inline void glvmUniform(const GLint location, const mat4 &m)
    {
	glUniformMatrix4fv(location, 1, GL_FALSE, m.vl);
    }

    inline void glvmUniform(const GLint location, const mat2x3 &m)
    {
	glUniformMatrix2x3fv(location, 1, GL_FALSE, m.vl);
    }

    inline void glvmUniform(const GLint location, const mat3x2 &m)
    {
	glUniformMatrix3x2fv(location, 1, GL_FALSE, m.vl);
    }

    inline void glvmUniform(const GLint location, const mat2x4 &m)
    {
	glUniformMatrix2x4fv(location, 1, GL_FALSE, m.vl);
    }

    inline void glvmUniform(const GLint location, const mat4x2 &m)
    {
	glUniformMatrix4x2fv(location, 1, GL_FALSE, m.vl);
    }

    inline void glvmUniform(const GLint location, const mat3x4 &m)
    {
	glUniformMatrix3x4fv(location, 1, GL_FALSE, m.vl);
    }

    inline void glvmUniform(const GLint location, const mat4x3 &m)
    {
	glUniformMatrix4x3fv(location, 1, GL_FALSE, m.vl);
    }

    inline void glvmUniform(const GLint location, const GLsizei count, const mat2 *m)
    {
	glUniformMatrix2fv(location, count, GL_FALSE, m[0].vl);
    }

    inline void glvmUniform(const GLint location, const GLsizei count, const mat3 *m)
    {
	glUniformMatrix3fv(location, count, GL_FALSE, m[0].vl);
    }

    inline void glvmUniform(const GLint location, const GLsizei count, const mat4 *m)
    {
	glUniformMatrix4fv(location, count, GL_FALSE, m[0].vl);
    }

    inline void glvmUniform(const GLint location, const GLsizei count, const mat2x3 *m)
    {
	glUniformMatrix2x3fv(location, count, GL_FALSE, m[0].vl);
    }

    inline void glvmUniform(const GLint location, const GLsizei count, const mat3x2 *m)
    {
	glUniformMatrix3x2fv(location, count, GL_FALSE, m[0].vl);
    }

    inline void glvmUniform(const GLint location, const GLsizei count, const mat2x4 *m)
    {
	glUniformMatrix2x4fv(location, count, GL_FALSE, m[0].vl);
    }

    inline void glvmUniform(const GLint location, const GLsizei count, const mat4x2 *m)
    {
	glUniformMatrix4x2fv(location, count, GL_FALSE, m[0].vl);
    }

    inline void glvmUniform(const GLint location, const GLsizei count, const mat3x4 *m)
    {
	glUniformMatrix3x4fv(location, count, GL_FALSE, m[0].vl);
    }

    inline void glvmUniform(const GLint location, const GLsizei count, const mat4x3 *m)
    {
	glUniformMatrix4x3fv(location, count, GL_FALSE, m[0].vl);
    }

    /* GLU */

    inline GLint glvmuProject(const dvec3 &obj, const dmat4 &model, const dmat4 &proj,
	    const ivec4 &view, dvec3 &win)
    {
	return gluProject(obj.x(), obj.y(), obj.z(),
		model.vl, proj.vl, view.vl,
		&(win.x()), &(win.y()), &(win.z()));
    }

    inline GLint glvmuUnProject(const dvec3 &win, const dmat4 &model, const dmat4 &proj,
	    const ivec4 &view, dvec3 &obj)
    {
	return gluUnProject(win.x(), win.y(), win.z(),  
		model.vl, proj.vl, view.vl,
		&(obj.x()), &(obj.y()), &(obj.z()));
    }
}

#endif

#define HL_NAME(n) sdl_##n
#include <hl.h>

#if defined(HL_IOS) || defined (HL_TVOS)
#	include <SDL2/SDL.h>
#	include <SDL2/SDL_syswm.h>
#	include <OpenGLES/ES3/gl.h>
#	define HL_GLES
#elif defined(HL_MAC)
#	include <SDL2/SDL.h>
#	include <OpenGL/gl3.h>
#	define glBindImageTexture(...) hl_error("Not supported on OSX")
#	define glDispatchCompute(...) hl_error("Not supported on OSX")
#	define glMemoryBarrier(...) hl_error("Not supported on OSX")
#elif defined(_WIN32)
#	include <SDL.h>
#	include <GL/GLU.h>
#	include <glext.h>
#elif defined(HL_CONSOLE)
#	include <graphic/glapi.h>
#elif defined(HL_MESA)
# 	include <GLES3/gl3.h>
#	include <GL/osmesa.h>
#	define HL_GLES
#elif defined(HL_ANDROID)
#	include <SDL.h>
#	include <GLES3/gl3.h>
#	include <GLES3/gl3ext.h>
#	define HL_GLES
#else
#	include <SDL2/SDL.h>
#	include <GL/glu.h>
#	include <GL/glext.h>
#endif

#ifdef HL_GLES
#	define GL_IMPORT(fun, t)
#	define ES_NOT_SUPPORTED hl_error("Not supported by GLES3")
#	define glBindFragDataLocation(...) ES_NOT_SUPPORTED
#	define glBindImageTexture(...) ES_NOT_SUPPORTED
#	define glTexImage2DMultisample(...) ES_NOT_SUPPORTED
#	define glFramebufferTexture(...) ES_NOT_SUPPORTED
#	define glDispatchCompute(...) ES_NOT_SUPPORTED
#	define glMemoryBarrier(...) ES_NOT_SUPPORTED
#	define glPolygonMode(face,mode) if( mode != 0x1B02 ) ES_NOT_SUPPORTED
#	define glGetQueryObjectiv glGetQueryObjectuiv
#	define glClearDepth glClearDepthf
#endif

#if !defined(HL_CONSOLE) && !defined(GL_IMPORT)
#define GL_IMPORT(fun, t) PFNGL##t##PROC fun
#include "GLImports.h"
#undef GL_IMPORT
#define GL_IMPORT(fun,t)	fun = (PFNGL##t##PROC)SDL_GL_GetProcAddress(#fun); if( fun == NULL ) return 1
#endif

static int GLLoadAPI() {
#	include "GLImports.h"
	return 0;
}

#define ZIDX(val) ((val)?(val)->v.i:0)

#ifdef HL_DEBUG
#	define DBG_GL //up to you
#endif

#ifdef DBG_GL
HL_PRIM int HL_NAME(gl_get_error)(); //pre-decl
static inline void chkErr() { sdl_gl_get_error(); }
#else
static inline void chkErr() {}
#endif

// globals
HL_PRIM bool HL_NAME(gl_init)() {
#ifdef DBG_GL
	bool f = GLLoadAPI() == 0;
	chkErr();
	return f;
#else
	return GLLoadAPI() == 0;
#endif
}

HL_PRIM bool HL_NAME(gl_is_context_lost)() {
	// seems like a GL context is rarely lost on desktop
	// let's look at it again on mobile
	return false;
}

HL_PRIM void HL_NAME(gl_clear)( int bits ) {
	glClear(bits);
	chkErr();
}

HL_PRIM int HL_NAME(gl_get_error)() {
#ifdef DBG_GL
	glFlush();
	uint error = glGetError();
	if(error != GL_NO_ERROR)
		printf("gl get error : %d \n", error);
	return error;
#else
	return glGetError();
#endif
}

HL_PRIM void HL_NAME(gl_scissor)( int x, int y, int width, int height ) {
	glScissor(x, y, width, height);
	chkErr();
}

HL_PRIM void HL_NAME(gl_clear_color)( double r, double g, double b, double a ) {
	glClearColor((float)r, (float)g, (float)b, (float)a);
	chkErr();
}

HL_PRIM void HL_NAME(gl_clear_depth)( double value ) {
	glClearDepth(value);
}

HL_PRIM void HL_NAME(gl_clear_stencil)( int value ) {
	glClearStencil(value);
}

HL_PRIM void HL_NAME(gl_viewport)( int x, int y, int width, int height ) {
	glViewport(x, y, width, height);
	chkErr();
}

HL_PRIM void HL_NAME(gl_flush)() {
	glFlush();
	chkErr();
}

HL_PRIM void HL_NAME(gl_finish)() {
	glFinish();
	chkErr();
}

HL_PRIM void HL_NAME(gl_pixel_storei)( int key, int value ) {
	glPixelStorei(key, value);
}

HL_PRIM vbyte *HL_NAME(gl_get_string)(int name) {
#ifdef DBG_GL
	vbyte* v = (vbyte*)glGetString(name);
	chkErr();
#	ifdef HL_MOBILE
	return hl_copy_bytes(v,(int)strlen(v) + 1);
#	else
	return v;
#	endif
#else
    vbyte* v = (vbyte*)glGetString(name);
#	ifdef HL_MOBILE
    return hl_copy_bytes(v,(int)strlen(v) + 1);
	// workaround for string being "lost" between there and call scope
	// original code was
	// return (vbyte*)glGetString(name);
#	else
	return v;
#	endif
#endif
}

// state changes

HL_PRIM void HL_NAME(gl_polygon_mode)(int face, int mode) {
	glPolygonMode(face, mode);
	chkErr();
}

HL_PRIM void HL_NAME(gl_enable)( int feature ) {
	glEnable(feature);
	chkErr();
}

HL_PRIM void HL_NAME(gl_disable)( int feature ) {
	glDisable(feature);
	chkErr();
}

HL_PRIM void HL_NAME(gl_cull_face)( int face ) {
	glCullFace(face);
}

HL_PRIM void HL_NAME(gl_blend_func)( int src, int dst ) {
	glBlendFunc(src, dst);
	chkErr();
}

HL_PRIM void HL_NAME(gl_blend_func_separate)( int src, int dst, int alphaSrc, int alphaDst ) {
	glBlendFuncSeparate(src, dst, alphaSrc, alphaDst);
	chkErr();
}

HL_PRIM void HL_NAME(gl_blend_equation)( int op ) {
	glBlendEquation(op);
	chkErr();
}

HL_PRIM void HL_NAME(gl_blend_equation_separate)( int op, int alphaOp ) {
	glBlendEquationSeparate(op, alphaOp);
	chkErr();
}

HL_PRIM void HL_NAME(gl_depth_mask)( bool mask ) {
	glDepthMask(mask);
	chkErr();
}

HL_PRIM void HL_NAME(gl_depth_func)( int f ) {
	glDepthFunc(f);
	chkErr();
}

HL_PRIM void HL_NAME(gl_color_mask)( bool r, bool g, bool b, bool a ) {
	glColorMask(r, g, b, a);
	chkErr();
}

HL_PRIM void HL_NAME(gl_stencil_mask_separate)(int face, int mask) {
	glStencilMaskSeparate(face, mask);
	chkErr();
}

HL_PRIM void HL_NAME(gl_stencil_func_separate)(int face, int func, int ref, int mask ) {
	glStencilFuncSeparate(face, func, ref, mask);
}

HL_PRIM void HL_NAME(gl_stencil_op_separate)(int face, int sfail, int dpfail, int dppass) {
	glStencilOpSeparate(face, sfail, dpfail, dppass);
	chkErr();
}

// program

static vdynamic *alloc_i32(int v) {
	vdynamic *ret;
	ret = hl_alloc_dynamic(&hlt_i32);
	ret->v.i = v;
	return ret;
}

HL_PRIM vdynamic *HL_NAME(gl_create_program)() {
	int v = glCreateProgram();
	chkErr();
	if( v == 0 ) return NULL;
	return alloc_i32(v);
}

HL_PRIM void HL_NAME(gl_delete_program)( vdynamic *s ) {
	glDeleteProgram(s->v.i);
	chkErr();
}

HL_PRIM void HL_NAME(gl_bind_frag_data_location)( vdynamic *p, int colNum, vstring *name ) {
	char *cname = hl_to_utf8(name->bytes);
	glBindFragDataLocation(p->v.i, colNum, cname);
	chkErr();
}

HL_PRIM void HL_NAME(gl_attach_shader)( vdynamic *p, vdynamic *s ) {
	glAttachShader(p->v.i, s->v.i);
	chkErr();
}

HL_PRIM void HL_NAME(gl_link_program)( vdynamic *p ) {
	glLinkProgram(p->v.i);
	chkErr();
}

HL_PRIM vdynamic *HL_NAME(gl_get_program_parameter)( vdynamic *p, int param ) {
	switch( param ) {
	case 0x8B82 /*LINK_STATUS*/ : {
		int ret = 0;
		glGetProgramiv(p->v.i, param, &ret);
		chkErr();
		return alloc_i32(ret);
	}
	default:
		hl_error("Unsupported param %d",param);
	}
	return NULL;
}

HL_PRIM vbyte *HL_NAME(gl_get_program_info_bytes)( vdynamic *p ) {
	char log[4096];
	*log = 0;
	glGetProgramInfoLog(p->v.i, 4096, NULL, log);
	chkErr();
	return hl_copy_bytes((vbyte*)log,(int)strlen(log) + 1);
}

HL_PRIM vdynamic *HL_NAME(gl_get_uniform_location)( vdynamic *p, vstring *name ) {
	char *cname = hl_to_utf8(name->bytes);
	int u = glGetUniformLocation(p->v.i, cname);
	chkErr();
	if( u < 0 ) return NULL;
	return alloc_i32(u);
}

HL_PRIM int HL_NAME(gl_get_attrib_location)( vdynamic *p, vstring *name ) {
	char *cname = hl_to_utf8(name->bytes);
#ifdef DBG_GL
	int loc = glGetAttribLocation(p->v.i, cname);
	chkErr();
	return loc;
#else
	return glGetAttribLocation(p->v.i, cname);
#endif
}

HL_PRIM void HL_NAME(gl_use_program)( vdynamic *p ) {
	glUseProgram(ZIDX(p));
	chkErr();
}

// shader

HL_PRIM vdynamic *HL_NAME(gl_create_shader)( int type ) {
	int s = glCreateShader(type);
	chkErr();
	if (s == 0) return NULL;
	return alloc_i32(s);
}

HL_PRIM void HL_NAME(gl_shader_source)( vdynamic *s, vstring *src ) {
	const GLchar *c = (GLchar*)hl_to_utf8(src->bytes);
	glShaderSource(s->v.i, 1, &c, NULL);
	chkErr();
}

HL_PRIM void HL_NAME(gl_compile_shader)( vdynamic *s ) {
	glCompileShader(s->v.i);
	chkErr();
}

HL_PRIM vbyte *HL_NAME(gl_get_shader_info_bytes)( vdynamic *s ) {
	char log[4096];
	*log = 0;
	glGetShaderInfoLog(s->v.i, 4096, NULL, log);
	chkErr();
	return hl_copy_bytes((vbyte*)log, (int)strlen(log)+1);
}

HL_PRIM vdynamic *HL_NAME(gl_get_shader_parameter)( vdynamic *s, int param ) {
	switch( param ) {
	case 0x8B81/*COMPILE_STATUS*/:
	case 0x8B4F/*SHADER_TYPE*/:
	case 0x8B80/*DELETE_STATUS*/:
	{
		int ret = 0;
		glGetShaderiv(s->v.i, param, &ret);
		chkErr();
		return alloc_i32(ret);
	}
	default:
		hl_error("Unsupported param %d", param);
	}
	return NULL;
}

HL_PRIM void HL_NAME(gl_delete_shader)( vdynamic *s ) {
	glDeleteShader(s->v.i);
	chkErr();
}

// texture

HL_PRIM vdynamic *HL_NAME(gl_create_texture)() {
	unsigned int t = 0;
	glGenTextures(1, &t);
	chkErr();
	return alloc_i32(t);
}

HL_PRIM void HL_NAME(gl_active_texture)( int t ) {
	glActiveTexture(t);
	chkErr();
}

HL_PRIM void HL_NAME(gl_bind_texture)( int t, vdynamic *texture ) {
	glBindTexture(t, ZIDX(texture));
	chkErr();
}

HL_PRIM void HL_NAME(gl_bind_image_texture)( int unit, int texture, int level, bool layered, int layer, int access, int format ) {
	glBindImageTexture(unit, texture, level, layered, layer, access, format);
	chkErr();
}

HL_PRIM void HL_NAME(gl_tex_parameterf)( int t, int key, float value ) {
	glTexParameterf(t, key, value);
	chkErr();
}

HL_PRIM void HL_NAME(gl_tex_parameteri)( int t, int key, int value ) {
	glTexParameteri(t, key, value);
	chkErr();
}

HL_PRIM void HL_NAME(gl_tex_image2d)( int target, int level, int internalFormat, int width, int height, int border, int format, int type, vbyte *image ) {
	glTexImage2D(target, level, internalFormat, width, height, border, format, type, image);
	chkErr();
}

HL_PRIM void HL_NAME(gl_tex_image3d)( int target, int level, int internalFormat, int width, int height, int depth, int border, int format, int type, vbyte *image ) {
	glTexImage3D(target, level, internalFormat, width, height, depth, border, format, type, image);
	chkErr();
}

HL_PRIM void HL_NAME(gl_tex_image2d_multisample)( int target, int samples, int internalFormat, int width, int height, bool fixedsamplelocations) {
	glTexImage2DMultisample(target, samples, internalFormat, width, height, fixedsamplelocations);
	chkErr();
}

HL_PRIM void HL_NAME(gl_compressed_tex_image2d)( int target, int level, int internalFormat, int width, int height, int border, int imageSize, vbyte *image ) {
	glCompressedTexImage2D(target,level,internalFormat,width,height,border,imageSize,image);
	chkErr();
}

HL_PRIM void HL_NAME(gl_compressed_tex_image3d)( int target, int level, int internalFormat, int width, int height, int depth, int border, int imageSize, vbyte *image ) {
	glCompressedTexImage3D(target,level,internalFormat,width,height,depth,border,imageSize,image);
	chkErr();
}

HL_PRIM void HL_NAME(gl_generate_mipmap)( int t ) {
	glGenerateMipmap(t);
	chkErr();
}

HL_PRIM void HL_NAME(gl_delete_texture)( vdynamic *t ) {
	unsigned int tt = t->v.i;
	glDeleteTextures(1, &tt);
	chkErr();
}

// framebuffer

HL_PRIM void HL_NAME(gl_blit_framebuffer)(int src_x0, int src_y0, int src_x1, int src_y1, int dst_x0, int dst_y0, int dst_x1, int dst_y1, int mask, int filter) {
	glBlitFramebuffer(src_x0, src_y0, src_x1, src_y1, dst_x0, dst_y0, dst_x1, dst_y1, mask, filter);
	chkErr();
}

HL_PRIM vdynamic *HL_NAME(gl_create_framebuffer)() {
	unsigned int f = 0;
	glGenFramebuffers(1, &f);
	chkErr();
	return alloc_i32(f);
}

HL_PRIM void HL_NAME(gl_bind_framebuffer)( int target, vdynamic *f ) {
	unsigned int id = ZIDX(f);
#if	defined(HL_IOS) || defined(HL_TVOS)
	if ( id==0 ) {
		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);
		SDL_GetWindowWMInfo(SDL_GL_GetCurrentWindow(), &info);
		id = info.info.uikit.framebuffer;
	}
#endif
	glBindFramebuffer(target, id);
	chkErr();
}

HL_PRIM void HL_NAME(gl_framebuffer_texture)( int target, int attach, vdynamic *t, int level ) {
	glFramebufferTexture(target, attach, ZIDX(t), level);
	chkErr();
}

HL_PRIM void HL_NAME(gl_framebuffer_texture2d)( int target, int attach, int texTarget, vdynamic *t, int level ) {
	glFramebufferTexture2D(target, attach, texTarget, ZIDX(t), level);
	chkErr();
}

HL_PRIM void HL_NAME(gl_framebuffer_texture_layer)( int target, int attach, vdynamic *t, int level, int layer ) {
	glFramebufferTextureLayer(target, attach, ZIDX(t), level, layer);
	chkErr();
}

HL_PRIM void HL_NAME(gl_delete_framebuffer)( vdynamic *f ) {
	unsigned int ff = (unsigned)f->v.i;
	glDeleteFramebuffers(1, &ff);
	chkErr();
}

HL_PRIM void HL_NAME(gl_read_pixels)( int x, int y, int width, int height, int format, int type, vbyte *data ) {
	glReadPixels(x, y, width, height, format, type, data);
	chkErr();
}

HL_PRIM void HL_NAME(gl_read_buffer)( int mode ) {
	glReadBuffer(mode);
	chkErr();
}

HL_PRIM void HL_NAME(gl_draw_buffers)( int count, unsigned int *buffers) {
	glDrawBuffers(count, buffers);
	chkErr();
}

HL_PRIM int HL_NAME(gl_check_framebuffer_status)( int buffer ) {
#if	defined(HL_MOBILE) || defined(HL_MAC)
	return glCheckFramebufferStatus(buffer);
#endif
}

// renderbuffer

HL_PRIM vdynamic *HL_NAME(gl_create_renderbuffer)() {
	unsigned int buf = 0;
	glGenRenderbuffers(1, &buf);
	chkErr();
	return alloc_i32(buf);
}

HL_PRIM void HL_NAME(gl_bind_renderbuffer)( int target, vdynamic *r ) {
	unsigned int id = ZIDX(r);
#if	defined(HL_IOS) || defined(HL_TVOS)
	if ( id==0 ) {
		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);
		SDL_GetWindowWMInfo(SDL_GL_GetCurrentWindow(), &info);
		id = info.info.uikit.colorbuffer;
	}
#endif
	glBindRenderbuffer(GL_RENDERBUFFER, id);
	chkErr();
}

HL_PRIM void HL_NAME(gl_renderbuffer_storage)( int target, int format, int width, int height ) {
	glRenderbufferStorage(target, format, width, height);
	chkErr();
}


HL_PRIM void HL_NAME(gl_renderbuffer_storage_multisample)( int target, int samples, int format, int width, int height ) {
	glRenderbufferStorageMultisample(target, samples, format, width, height);
	chkErr();
}

HL_PRIM void HL_NAME(gl_framebuffer_renderbuffer)( int frameTarget, int attach, int renderTarget, vdynamic *b ) {
	glFramebufferRenderbuffer(frameTarget, attach, renderTarget, ZIDX(b));
	chkErr();
}

HL_PRIM void HL_NAME(gl_delete_renderbuffer)( vdynamic *b ) {
	unsigned int bb = (unsigned)b->v.i;
	glDeleteRenderbuffers(1, &bb);
	chkErr();
}

// buffer

HL_PRIM vdynamic *HL_NAME(gl_create_buffer)() {
	unsigned int b = 0;
	glGenBuffers(1, &b);
	chkErr();
	return alloc_i32(b);
}

HL_PRIM void HL_NAME(gl_bind_buffer)( int target, vdynamic *b ) {
	glBindBuffer(target, ZIDX(b));
	chkErr();
}

HL_PRIM void HL_NAME(gl_bind_buffer_base)( int target, int index, vdynamic *b ) {
	glBindBufferBase(target, index, ZIDX(b));
	chkErr();
}

HL_PRIM void HL_NAME(gl_buffer_data_size)( int target, int size, int param ) {
	glBufferData(target, size, NULL, param);
	chkErr();
}

HL_PRIM void HL_NAME(gl_buffer_data)( int target, int size, vbyte *data, int param ) {
#ifdef HL_MOBILE
	void* ptr = glMapBufferRange(target, 0, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
	chkErr();
	memcpy(ptr, data, size);
	glUnmapBuffer(target);
	chkErr();
#else
	glBufferData(target, size, data, param);
	chkErr();
#endif
}

HL_PRIM void HL_NAME(gl_buffer_sub_data)( int target, int offset, vbyte *data, int srcOffset, int srcLength ) {
#ifdef HL_MOBILE
	void* ptr = glMapBufferRange(target, offset, srcLength, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
	chkErr();
	memcpy(ptr, data + srcOffset, srcLength);
	glUnmapBuffer(target);
	chkErr();
#else
	glBufferSubData(target, offset, srcLength, data + srcOffset);
#endif
	chkErr();
}

HL_PRIM void HL_NAME(gl_enable_vertex_attrib_array)( int attrib ) {
	glEnableVertexAttribArray(attrib);
	chkErr();
}

HL_PRIM void HL_NAME(gl_disable_vertex_attrib_array)( int attrib ) {
	glDisableVertexAttribArray(attrib);
	chkErr();
}

HL_PRIM void HL_NAME(gl_vertex_attrib_pointer)( int index, int size, int type, bool normalized, int stride, int position ) {
	glVertexAttribPointer(index, size, type, normalized, stride, (void*)(int_val)position);
	chkErr();
}

HL_PRIM void HL_NAME(gl_vertex_attrib_ipointer)( int index, int size, int type, int stride, int position ) {
	glVertexAttribIPointer(index, size, type, stride, (void*)(int_val)position);
	chkErr();
}

HL_PRIM void HL_NAME(gl_vertex_attrib_divisor)( int index, int divisor ) {
	glVertexAttribDivisor(index, divisor);
	chkErr();
}

HL_PRIM void HL_NAME(gl_delete_buffer)( vdynamic *b ) {
	unsigned int bb = (unsigned)b->v.i;
	glDeleteBuffers(1, &bb);
	chkErr();
}

// uniforms

HL_PRIM void HL_NAME(gl_uniform1i)( vdynamic *u, int i ) {
	glUniform1i(u->v.i, i);
	chkErr();
}

HL_PRIM void HL_NAME(gl_uniform4fv)( vdynamic *u, vbyte *buffer, int bufPos, int count ) {
	glUniform4fv(u->v.i, count, (float*)buffer + bufPos);
	chkErr();
}

HL_PRIM void HL_NAME(gl_uniform_matrix4fv)( vdynamic *u, bool transpose, vbyte *buffer, int bufPos, int count ) {
	glUniformMatrix4fv(u->v.i, count, transpose ? GL_TRUE : GL_FALSE, (float*)buffer + bufPos);
	chkErr();
}

// compute
HL_PRIM void HL_NAME(gl_dispatch_compute)( int num_groups_x, int num_groups_y, int num_groups_z ) {
	glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
	chkErr();
}

HL_PRIM void HL_NAME(gl_memory_barrier)( int barriers ) {
#	if !defined(HL_IOS) && !defined(HL_TVOS) && !defined(HL_MAC) && !defined(HL_ANDROID)
	glMemoryBarrier(barriers);
	chkErr();
#	endif
}

// draw

HL_PRIM void HL_NAME(gl_draw_elements)( int mode, int count, int type, int start ) {
	glDrawElements(mode, count, type, (void*)(int_val)start);
	chkErr();
}

HL_PRIM void HL_NAME(gl_draw_arrays)( int mode, int first, int count, int start ) {
	glDrawArrays(mode,first,count);
	chkErr();
}

HL_PRIM void HL_NAME(gl_draw_elements_instanced)( int mode, int count, int type, int start, int primcount ) {
	glDrawElementsInstanced(mode,count,type,(void*)(int_val)start,primcount);
	chkErr();
}

HL_PRIM void HL_NAME(gl_draw_arrays_instanced)( int mode, int first, int count, int primcount ) {
	glDrawArraysInstanced(mode,first,count,primcount);
	chkErr();
}

HL_PRIM void HL_NAME(gl_multi_draw_elements_indirect)( int mode, int type, vbyte *data, int count, int stride ) {
#	ifdef GL_VERSION_4_3
	glMultiDrawElementsIndirect(mode, type, data, count, stride);
	chkErr();
#	endif
}

HL_PRIM int HL_NAME(gl_get_config_parameter)( int feature ) {
	switch( feature ) {
	case 0:
#		ifdef GL_VERSION_4_3
		return 1;
#		else
		return 0;
#		endif
	default:
		break;
	}
	return -1;
}

// queries

HL_PRIM vdynamic *HL_NAME(gl_create_query)() {
	unsigned int t = 0;
	glGenQueries(1, &t);
	chkErr();
	return alloc_i32(t);
}

HL_PRIM void HL_NAME(gl_delete_query)( vdynamic *q ) {
	glDeleteQueries(1, (const GLuint *) &q->v.i);
	chkErr();
}

HL_PRIM void HL_NAME(gl_begin_query)( int target, vdynamic *q ) {
	glBeginQuery(target,q->v.i);
	chkErr();
}

HL_PRIM void HL_NAME(gl_end_query)( int target ) {
	glEndQuery(target);
	chkErr();
}

HL_PRIM bool HL_NAME(gl_query_result_available)( vdynamic *q ) {
	int v = 0;
	glGetQueryObjectiv(q->v.i, GL_QUERY_RESULT_AVAILABLE, &v);
	chkErr();
	return v == GL_TRUE;
}

HL_PRIM double HL_NAME(gl_query_result)( vdynamic *q ) {
	GLuint64 v = -1;
#	if !defined(HL_MESA) && !defined(HL_MOBILE)
	glGetQueryObjectui64v(q->v.i, GL_QUERY_RESULT, &v);
	chkErr();
#	endif
	return (double)v;
}

HL_PRIM void HL_NAME(gl_query_counter)( vdynamic *q, int target ) {
#	if !defined(HL_MESA) && !defined(HL_MOBILE)
	glQueryCounter(q->v.i, target);
	chkErr();
#	endif
}

// vertex array

HL_PRIM vdynamic *HL_NAME(gl_create_vertex_array)() {
	unsigned int f = 0;
	glGenVertexArrays(1, &f);
	chkErr();
	return alloc_i32(f);
}

HL_PRIM void HL_NAME(gl_bind_vertex_array)( vdynamic *b ) {
	unsigned int bb = (unsigned)b->v.i;
	glBindVertexArray(bb);
	chkErr();
}

HL_PRIM void HL_NAME(gl_delete_vertex_array)( vdynamic *b ) {
	unsigned int bb = (unsigned)b->v.i;
	glDeleteVertexArrays(1, &bb);
	chkErr();
}

// uniform buffer

HL_PRIM int HL_NAME(gl_get_uniform_block_index)( vdynamic *p, vstring *name ) {
	char *cname = hl_to_utf8(name->bytes);
#ifdef DBG_GL
	int t = (int)glGetUniformBlockIndex(p->v.i, cname);
	chkErr();
	return t;
#else
	return (int)glGetUniformBlockIndex(p->v.i, cname);
#endif
}

HL_PRIM void HL_NAME(gl_uniform_block_binding)( vdynamic *p, int index, int binding ) {
	glUniformBlockBinding(p->v.i, index, binding);
	chkErr();
}

DEFINE_PRIM(_BOOL,gl_init,_NO_ARG);
DEFINE_PRIM(_BOOL,gl_is_context_lost,_NO_ARG);
DEFINE_PRIM(_VOID,gl_clear,_I32);
DEFINE_PRIM(_I32,gl_get_error,_NO_ARG);
DEFINE_PRIM(_VOID,gl_scissor,_I32 _I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_clear_color,_F64 _F64 _F64 _F64);
DEFINE_PRIM(_VOID,gl_clear_depth,_F64);
DEFINE_PRIM(_VOID,gl_clear_stencil,_I32);
DEFINE_PRIM(_VOID,gl_viewport,_I32 _I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_finish,_NO_ARG);
DEFINE_PRIM(_VOID,gl_flush,_NO_ARG);
DEFINE_PRIM(_VOID,gl_pixel_storei,_I32 _I32);
DEFINE_PRIM(_BYTES,gl_get_string,_I32);
DEFINE_PRIM(_VOID,gl_polygon_mode,_I32 _I32);
DEFINE_PRIM(_VOID,gl_enable,_I32);
DEFINE_PRIM(_VOID,gl_disable,_I32);
DEFINE_PRIM(_VOID,gl_cull_face,_I32);
DEFINE_PRIM(_VOID,gl_blend_func,_I32 _I32);
DEFINE_PRIM(_VOID,gl_blend_func_separate,_I32 _I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_blend_equation,_I32);
DEFINE_PRIM(_VOID,gl_blend_equation_separate,_I32 _I32);
DEFINE_PRIM(_VOID,gl_depth_mask,_BOOL);
DEFINE_PRIM(_VOID,gl_depth_func,_I32);
DEFINE_PRIM(_VOID,gl_color_mask,_BOOL _BOOL _BOOL _BOOL);
DEFINE_PRIM(_VOID,gl_stencil_mask_separate,_I32 _I32);
DEFINE_PRIM(_VOID,gl_stencil_func_separate,_I32 _I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_stencil_op_separate,_I32  _I32 _I32 _I32);
DEFINE_PRIM(_NULL(_I32),gl_create_program,_NO_ARG);
DEFINE_PRIM(_VOID,gl_delete_program,_NULL(_I32));
DEFINE_PRIM(_VOID,gl_bind_frag_data_location,_NULL(_I32) _I32 _STRING);
DEFINE_PRIM(_VOID,gl_attach_shader,_NULL(_I32) _NULL(_I32));
DEFINE_PRIM(_VOID,gl_link_program,_NULL(_I32));
DEFINE_PRIM(_DYN,gl_get_program_parameter,_NULL(_I32) _I32);
DEFINE_PRIM(_BYTES,gl_get_program_info_bytes,_NULL(_I32));
DEFINE_PRIM(_NULL(_I32),gl_get_uniform_location,_NULL(_I32) _STRING);
DEFINE_PRIM(_I32,gl_get_attrib_location,_NULL(_I32) _STRING);
DEFINE_PRIM(_VOID,gl_use_program,_NULL(_I32));
DEFINE_PRIM(_NULL(_I32),gl_create_shader,_I32);
DEFINE_PRIM(_VOID,gl_shader_source,_NULL(_I32) _STRING);
DEFINE_PRIM(_VOID,gl_compile_shader,_NULL(_I32));
DEFINE_PRIM(_BYTES,gl_get_shader_info_bytes,_NULL(_I32));
DEFINE_PRIM(_DYN,gl_get_shader_parameter,_NULL(_I32) _I32);
DEFINE_PRIM(_VOID,gl_delete_shader,_NULL(_I32));
DEFINE_PRIM(_NULL(_I32),gl_create_texture,_NO_ARG);
DEFINE_PRIM(_VOID,gl_active_texture,_I32);
DEFINE_PRIM(_VOID,gl_bind_texture,_I32 _NULL(_I32));
DEFINE_PRIM(_VOID,gl_tex_parameteri,_I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_tex_parameterf,_I32 _I32 _F32);
DEFINE_PRIM(_VOID,gl_tex_image2d,_I32 _I32 _I32 _I32 _I32 _I32 _I32 _I32 _BYTES);
DEFINE_PRIM(_VOID,gl_tex_image3d,_I32 _I32 _I32 _I32 _I32 _I32 _I32 _I32 _I32 _BYTES);
DEFINE_PRIM(_VOID,gl_tex_image2d_multisample,_I32 _I32 _I32 _I32 _I32 _BOOL);
DEFINE_PRIM(_VOID,gl_compressed_tex_image2d,_I32 _I32 _I32 _I32 _I32 _I32 _I32 _BYTES);
DEFINE_PRIM(_VOID,gl_compressed_tex_image3d,_I32 _I32 _I32 _I32 _I32 _I32 _I32 _I32 _BYTES);
DEFINE_PRIM(_VOID,gl_generate_mipmap,_I32);
DEFINE_PRIM(_VOID,gl_delete_texture,_NULL(_I32));
DEFINE_PRIM(_VOID,gl_blit_framebuffer,_I32 _I32 _I32 _I32 _I32 _I32 _I32 _I32 _I32 _I32);
DEFINE_PRIM(_NULL(_I32),gl_create_framebuffer,_NO_ARG);
DEFINE_PRIM(_VOID,gl_bind_framebuffer,_I32 _NULL(_I32));
DEFINE_PRIM(_VOID,gl_framebuffer_texture,_I32 _I32 _NULL(_I32) _I32);
DEFINE_PRIM(_VOID,gl_framebuffer_texture2d,_I32 _I32 _I32 _NULL(_I32) _I32);
DEFINE_PRIM(_VOID,gl_framebuffer_texture_layer,_I32 _I32 _NULL(_I32) _I32 _I32);
DEFINE_PRIM(_VOID,gl_delete_framebuffer,_NULL(_I32));
DEFINE_PRIM(_VOID,gl_read_pixels,_I32 _I32 _I32 _I32 _I32 _I32 _BYTES);
DEFINE_PRIM(_VOID,gl_read_buffer,_I32);
DEFINE_PRIM(_VOID,gl_draw_buffers,_I32 _BYTES);
DEFINE_PRIM(_I32,gl_check_framebuffer_status,_I32);
DEFINE_PRIM(_NULL(_I32),gl_create_renderbuffer,_NO_ARG);
DEFINE_PRIM(_VOID,gl_bind_renderbuffer,_I32 _NULL(_I32));
DEFINE_PRIM(_VOID,gl_renderbuffer_storage,_I32 _I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_renderbuffer_storage_multisample,_I32 _I32 _I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_framebuffer_renderbuffer,_I32 _I32 _I32 _NULL(_I32));
DEFINE_PRIM(_VOID,gl_delete_renderbuffer,_NULL(_I32));
DEFINE_PRIM(_NULL(_I32),gl_create_buffer,_NO_ARG);
DEFINE_PRIM(_VOID,gl_bind_buffer,_I32 _NULL(_I32));
DEFINE_PRIM(_VOID,gl_bind_buffer_base,_I32 _I32 _NULL(_I32));
DEFINE_PRIM(_VOID,gl_buffer_data_size,_I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_buffer_data,_I32 _I32 _BYTES _I32);
DEFINE_PRIM(_VOID,gl_buffer_sub_data,_I32 _I32 _BYTES _I32 _I32);
DEFINE_PRIM(_VOID,gl_enable_vertex_attrib_array,_I32);
DEFINE_PRIM(_VOID,gl_disable_vertex_attrib_array,_I32);
DEFINE_PRIM(_VOID,gl_vertex_attrib_pointer,_I32 _I32 _I32 _BOOL _I32 _I32);
DEFINE_PRIM(_VOID,gl_vertex_attrib_ipointer,_I32 _I32 _I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_delete_buffer,_NULL(_I32));
DEFINE_PRIM(_VOID,gl_uniform1i,_NULL(_I32) _I32);
DEFINE_PRIM(_VOID,gl_uniform4fv,_NULL(_I32) _BYTES _I32 _I32);
DEFINE_PRIM(_VOID,gl_uniform_matrix4fv,_NULL(_I32) _BOOL _BYTES _I32 _I32);
DEFINE_PRIM(_VOID,gl_bind_image_texture,_I32 _I32 _I32 _BOOL _I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_dispatch_compute,_I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_memory_barrier,_I32);
DEFINE_PRIM(_VOID,gl_draw_elements,_I32 _I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_draw_elements_instanced,_I32 _I32 _I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_draw_arrays,_I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_draw_arrays_instanced,_I32 _I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_multi_draw_elements_indirect, _I32 _I32 _BYTES _I32 _I32);
DEFINE_PRIM(_NULL(_I32),gl_create_vertex_array,_NO_ARG);
DEFINE_PRIM(_VOID,gl_bind_vertex_array,_NULL(_I32));
DEFINE_PRIM(_VOID,gl_delete_vertex_array,_NULL(_I32));
DEFINE_PRIM(_VOID,gl_vertex_attrib_divisor,_I32 _I32);

DEFINE_PRIM(_NULL(_I32), gl_create_query, _NO_ARG);
DEFINE_PRIM(_VOID, gl_delete_query, _NULL(_I32));
DEFINE_PRIM(_VOID, gl_begin_query, _I32 _NULL(_I32));
DEFINE_PRIM(_VOID, gl_end_query, _I32);
DEFINE_PRIM(_BOOL, gl_query_result_available, _NULL(_I32));
DEFINE_PRIM(_VOID, gl_query_counter, _NULL(_I32) _I32);
DEFINE_PRIM(_F64, gl_query_result, _NULL(_I32));

DEFINE_PRIM(_I32, gl_get_uniform_block_index, _NULL(_I32) _STRING);
DEFINE_PRIM(_VOID, gl_uniform_block_binding, _NULL(_I32) _I32 _I32);

DEFINE_PRIM(_I32, gl_get_config_parameter, _I32);

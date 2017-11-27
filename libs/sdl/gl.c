#define HL_NAME(n) sdl_##n
#include <hl.h>
#include <stdarg.h>

#if defined(__APPLE__)
#	include <TargetConditionals.h>
#	if TARGET_OS_IOS || TARGET_OS_TV
#		include <SDL2/SDL.h>
#		include <OpenGLES/ES2/gl.h>
#	elif TARGET_OS_OSX
#		include <SDL2/SDL.h>
#		include <OpenGL/gl3.h>
#	endif
#elif defined(_WIN32)
#	include <SDL.h>
#	include <GL/GLU.h>
#	include <glext.h>
#elif defined(HL_CONSOLE)
#	include <graphic/glapi.h>
#else
#	include <SDL2/SDL.h>
#	include <GL/glu.h>
#	include <GL/glext.h>
#endif

#ifndef HL_CONSOLE
#define GL_IMPORT(fun, t) PFNGL##t##PROC fun
#include "GLImports.h"
#undef GL_IMPORT
#define GL_IMPORT(fun,t)	fun = (PFNGL##t##PROC)SDL_GL_GetProcAddress(#fun); if( fun == NULL ) return 1
#endif

static int GLLoadAPI() {
#	include "GLImports.h"
	return 0;
}

//#define GL_LOG 1

#ifdef GL_LOG
//#	define GLOG(fmt,...)	if( gl_log_active ) fprintf(gl_log_out, __FUNCTION__ "(" fmt ")\n", __VA_ARGS__)
//#	define GLOGR(ret,v,fmt,...)	if( gl_log_active ) fprintf(gl_log_out, __FUNCTION__ "(" fmt ") = " ret "\n", __VA_ARGS__, v)

#	define GLOG(...) printf(__VA_ARGS__)
#	define GLOGR(...) printf(__VA_ARGS__)


static FILE *gl_log_out;
static bool gl_log_active = true;

static char *hexlog( vbyte *b, int size ) {
	static char tmp[1024];
	static const char *HEX = "0123456789ABCDEF";
	int pos = 0;
	if( b == NULL )
		return "NULL";
	if( size >= (sizeof(tmp)>>1) ) size = (sizeof(tmp)>>1) - 1;
	while( size-- ) {
		unsigned char c = *b++;
		tmp[pos++] = HEX[c>>4];
		tmp[pos++] = HEX[c&15];
	}
	tmp[pos] = 0;
	return tmp;
}

#else
#	define GLOG(...)
#	define GLOGR(...)
#endif

#define ZIDX(val) ((val)?(val)->v.i:0)

// globals
HL_PRIM bool HL_NAME(gl_init)() {
#	ifdef GL_LOG
	gl_log_out = fopen("gllog.txt","wb");
#	endif
	return GLLoadAPI() == 0;
}

HL_PRIM bool HL_NAME(gl_is_context_lost)() {
	// seems like a GL context is rarely lost on desktop
	// let's look at it again on mobile
	return false;
}

HL_PRIM int HL_NAME(gl_get_error)();

HL_PRIM void HL_NAME(gl_clear)( int bits ) {
	//printf("gl_clear%d \n",bits);
	glClear(bits);
	sdl_gl_get_error();
}

HL_PRIM int HL_NAME(gl_get_error)() {
	/*glFlush();
	uint error = glGetError();
	if(error != GL_NO_ERROR)
		printf("gl get error : %d \n", error);
	return error;*/
    return 0;
}

HL_PRIM void HL_NAME(gl_scissor)( int x, int y, int width, int height ) {
	//printf("gl_scissor%d,%d,%d,%d \n",x,y,width,height);
	glScissor(x, y, width, height);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_clear_color)( double r, double g, double b, double a ) {
	//printf("gl_clear_color%g,%g,%g,%g \n",r,g,b,a);
	glClearColor((float)r, (float)g, (float)b, (float)a);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_clear_depth)( double value ) {
	//printf("gl_clear_depth%g \n",value);
	glClearDepthf(value);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_clear_stencil)( int value ) {
	//printf("gl_clear_stencil%d \n",value);
	glClearStencil(value);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_viewport)( int x, int y, int width, int height ) {
	//printf("gl_viewport%d,%d,%d,%d \n",x,y,width,height);
	glViewport(x, y, width, height);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_finish)() {
	//printf("gl_finish \n");
#	ifdef GL_LOG
	fflush(gl_log_out);
#	endif
	glFinish();
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_pixel_storei)( int key, int value ) {
	//printf("gl_pixel_storei%d,%d \n",key,value);
	glPixelStorei(key, value);
	sdl_gl_get_error();
}

HL_PRIM vbyte *HL_NAME(gl_get_string)(int name) {
	//printf("gl_get_string%d \n", name);
	return (vbyte*)glGetString(name);
}

// state changes

HL_PRIM void HL_NAME(gl_enable)( int feature ) {
	//printf("glEnable %d \n",feature);
	glEnable(feature);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_disable)( int feature ) {
	//printf("gl_disable %d \n",feature);
	glDisable(feature);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_cull_face)( int face ) {
	//printf("gl_cull_face %d \n",face);
	glCullFace(face);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_blend_func)( int src, int dst ) {
	//printf("gl_blend_func%d,%d \n",src,dst);
	glBlendFunc(src, dst);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_blend_func_separate)( int src, int dst, int alphaSrc, int alphaDst ) {
	//printf("gl_blend_func_separate%d,%d,%d,%d \n",src,dst,alphaSrc,alphaDst);
	glBlendFuncSeparate(src, dst, alphaSrc, alphaDst);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_blend_equation)( int op ) {
	//printf("gl_blend_equation%d \n",op);
	glBlendEquation(op);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_blend_equation_separate)( int op, int alphaOp ) {
	//printf("gl_blend_equation_separate%d,%d \n",op,alphaOp);
	glBlendEquationSeparate(op, alphaOp);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_depth_mask)( bool mask ) {
	//printf("gl_depth_mask%d \n",mask);
	glDepthMask(mask);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_depth_func)( int f ) {
	//printf("gl_depth_func%d \n",f);
	glDepthFunc(f);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_color_mask)( bool r, bool g, bool b, bool a ) {
	//printf("gl_color_mask%d,%d,%d,%d \n",r,g,b,a);
	glColorMask(r, g, b, a);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_stencil_mask_separate)(int face, int mask) {
	//printf("gl_stencil_mask_separate%d,%d \n",face,mask);
	glStencilMaskSeparate(face, mask);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_stencil_func_separate)(int face, int func, int ref, int mask ) {
	//printf("gl_stencil_func_separate%d,%d,%d,%d \n",face,func,ref,mask);
	glStencilFuncSeparate(face, func, ref, mask);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_stencil_op_separate)(int face, int sfail, int dpfail, int dppass) {
	//printf("gl_stencil_op_separate%d,%d,%d,%d \n",face,sfail,dpfail,dppass);
	glStencilOpSeparate(face, sfail, dpfail, dppass);
	sdl_gl_get_error();
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
	sdl_gl_get_error();
	//printf("gl_create_program%d \n",v);
	if( v == 0 ) return NULL;
	return alloc_i32(v);
}

HL_PRIM void HL_NAME(gl_delete_program)( vdynamic *s ) {
	//printf("gl_delete_program%d \n",s->v.i);
	glDeleteProgram(s->v.i);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_bind_frag_data_location)( vdynamic *p, int colNum, vstring *name ) {
	char *cname = hl_to_utf8(name->bytes);
	//printf("gl_bind_frag_data_location%d,%d,%n \n",p->v.i,colNum,cname);
	//glBindFragDataLocation(p->v.i, colNum, cname);
}

HL_PRIM void HL_NAME(gl_attach_shader)( vdynamic *p, vdynamic *s ) {
	//printf("gl_attach_shader%d,%d \n",p->v.i,s->v.i);
	glAttachShader(p->v.i, s->v.i);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_link_program)( vdynamic *p ) {
	//printf("gl_link_program%d \n",p->v.i);
	glLinkProgram(p->v.i);
	sdl_gl_get_error();
}

HL_PRIM vdynamic *HL_NAME(gl_get_program_parameter)( vdynamic *p, int param ) {
	switch( param ) {
	case 0x8B82 /*LINK_STATUS*/ : {
		int ret = 0;
		glGetProgramiv(p->v.i, param, &ret);
		sdl_gl_get_error();
		//printf("gl_get_program_parameter%d,%d,%d \n",ret,p->v.i,param);
		return alloc_i32(ret);
	}
	default:
		hl_error_msg(USTR("Unsupported param %d"),param);
	}
	return NULL;
}

HL_PRIM vbyte *HL_NAME(gl_get_program_info_bytes)( vdynamic *p ) {
	char log[4096];
	*log = 0;
	glGetProgramInfoLog(p->v.i, 4096, NULL, log);
	sdl_gl_get_error();
	//printf("gl_get_program_info_bytes%s,%d \n",log,p->v.i);
	return hl_copy_bytes((vbyte*)log,(int)strlen(log) + 1);
}

HL_PRIM vdynamic *HL_NAME(gl_get_uniform_location)( vdynamic *p, vstring *name ) {
	char *cname = hl_to_utf8(name->bytes);
	int u = glGetUniformLocation(p->v.i, cname);
	sdl_gl_get_error();
	//printf("gl_get_uniform_location%d,%d,%s \n",u,p->v.i,cname);
	if( u < 0 ) return NULL;
	return alloc_i32(u);
}

HL_PRIM int HL_NAME(gl_get_attrib_location)( vdynamic *p, vstring *name ) {
	char *cname = hl_to_utf8(name->bytes);
	int loc = glGetAttribLocation(p->v.i, cname);
	sdl_gl_get_error();
	//printf("gl_get_attrib_location%d,%d,%s \n",loc,p->v.i,cname);
	return loc;
}

#include <SDL2/SDL_syswm.h>

HL_PRIM void HL_NAME(gl_use_program)( vdynamic *p ) {
	//printf("gl_use_program%d \n",ZIDX(p));
    glUseProgram(ZIDX(p));
	sdl_gl_get_error();
}

// shader

HL_PRIM vdynamic *HL_NAME(gl_create_shader)( int type ) {
	int s = glCreateShader(type);
	sdl_gl_get_error();
	//printf("gl_create_shader%d,%d \n",s,type);
	if (s == 0) return NULL;
	return alloc_i32(s);
}

HL_PRIM void HL_NAME(gl_shader_source)( vdynamic *s, vstring *src ) {
	const GLchar *c = (GLchar*)hl_to_utf8(src->bytes);
	//printf("gl_shader_source%d,%s \n",s->v.i,c);
	glShaderSource(s->v.i, 1, &c, NULL);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_compile_shader)( vdynamic *s ) {
	//printf("gl_compile_shader%d \n",s->v.i);
	glCompileShader(s->v.i);
	sdl_gl_get_error();
}

HL_PRIM vbyte *HL_NAME(gl_get_shader_info_bytes)( vdynamic *s ) {
	char log[4096];
	*log = 0;
	glGetShaderInfoLog(s->v.i, 4096, NULL, log);
	sdl_gl_get_error();
	//printf("gl_get_shader_info_bytes%s,%d \n",log,s->v.i);
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
		sdl_gl_get_error();
		//printf("gl_get_shader_parameter%d,%d,%d \n",ret,s->v.i,param);
		return alloc_i32(ret);
	}
	default:
		hl_error_msg(USTR("Unsupported param %d"), param);
	}
	return NULL;
}

HL_PRIM void HL_NAME(gl_delete_shader)( vdynamic *s ) {
	//printf("gl_delete_shader%d \n",s->v.i);
	glDeleteShader(s->v.i);
	sdl_gl_get_error();
}

// texture

HL_PRIM vdynamic *HL_NAME(gl_create_texture)() {
	unsigned int t = 0;
	glGenTextures(1, &t);
	sdl_gl_get_error();
	//printf("gl_create_texture %d \n",t);
	return alloc_i32(t);
}

HL_PRIM void HL_NAME(gl_active_texture)( int t ) {
	//printf("gl_active_texture %d \n",t);
    glActiveTexture(t);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_bind_texture)( int t, vdynamic *texture ) {
	//printf("gl_bind_texture %d,%d \n",t,ZIDX(texture));
	glBindTexture(t, ZIDX(texture));
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_tex_parameteri)( int t, int key, int value ) {
	//printf("gl_tex_parameteri %d,%d,%d \n",t,key,value);
	glTexParameteri(t, key, value);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_tex_image2d)( int target, int level, int internalFormat, int width, int height, int border, int format, int type, vbyte *image ) {
	//printf("gl_tex_image2d 0x%x,%d,0x%x,%d,%d,%d,0x%x,0x%x ptr=0x%x\n",target,level,internalFormat,width,height,border,format,type, image/*,hexlog(image,16)*/);
	glTexImage2D(target, level, internalFormat, width, height, border, format, type, image);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_compressed_tex_image2d)( int target, int level, int internalFormat, int width, int height, int border, int size, vbyte *data ) {
	//printf("gl_compressed_tex_image2d 0x%x,%d,0x%x,%d,%d,%d,0x%x ptr=0x%x\n",target,level,internalFormat,width,height,border,size, data);
	glCompressedTexImage2D(target, level, internalFormat, width, height, border, size, data);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_generate_mipmap)( int t ) {
	//printf("gl_generate_mipmap %d \n",t);
	glGenerateMipmap(t);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_delete_texture)( vdynamic *t ) {
	unsigned int tt = t->v.i;
	//printf("gl_delete_texture %d \n",tt);
	glDeleteTextures(1, &tt);
	sdl_gl_get_error();
}

// framebuffer

HL_PRIM vdynamic *HL_NAME(gl_create_framebuffer)() {
	unsigned int f = 0;
	glGenFramebuffers(1, &f);
    sdl_gl_get_error();
	//printf("gl_create_framebuffer%d \n",f);
	return alloc_i32(f);
}

HL_PRIM void HL_NAME(gl_bind_framebuffer)( int target, vdynamic *f ) {
	//printf("gl_bind_framebuffer%d,%d \n",target,ZIDX(f));
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    SDL_GetWindowWMInfo(SDL_GL_GetCurrentWindow(), &info);
    glBindFramebuffer(target, ZIDX(f) ==0 ? info.info.uikit.framebuffer : ZIDX(f));
    sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_framebuffer_texture2d)( int target, int attach, int texTarget, vdynamic *t, int level ) {
	//printf("gl_framebuffer_texture2d%d,%d,%d,%d,%d \n",target,attach,texTarget,ZIDX(t),level);
	glFramebufferTexture2D(target, attach, texTarget, ZIDX(t), level);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_delete_framebuffer)( vdynamic *f ) {
	unsigned int ff = (unsigned)f->v.i;
	//printf("gl_delete_framebuffer%d \n",ff);
	glDeleteFramebuffers(1, &ff);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_read_pixels)( int x, int y, int width, int height, int format, int type, vbyte *data ) {
	//printf("gl_read_pixels%d,%d,%d,%d,%d,%d,%X \n",x,y,width,height,format,type,(int)(int_val)data);
	glReadPixels(x, y, width, height, format, type, data);
	sdl_gl_get_error();
}

// renderbuffer

HL_PRIM vdynamic *HL_NAME(gl_create_renderbuffer)() {
	unsigned int buf = 0;
	glGenRenderbuffers(1, &buf);
	sdl_gl_get_error();
	//printf("gl_create_renderbuffer%d \n",buf);
	return alloc_i32(buf);
}

HL_PRIM void HL_NAME(gl_bind_renderbuffer)( int target, vdynamic *r ) {
	//printf("gl_bind_renderbuffer%d,%d \n",target,ZIDX(r));
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    SDL_GetWindowWMInfo(SDL_GL_GetCurrentWindow(), &info);
    glBindRenderbuffer(GL_RENDERBUFFER, ZIDX(r) == 0 ? info.info.uikit.colorbuffer : ZIDX(r));
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_renderbuffer_storage)( int target, int format, int width, int height ) {
	//printf("gl_renderbuffer_storage%d,%d,%d,%d \n",target,format,width,height);
	glRenderbufferStorage(target, format, width, height);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_framebuffer_renderbuffer)( int frameTarget, int attach, int renderTarget, vdynamic *b ) {
	//printf("gl_framebuffer_renderbuffer%d,%d,%d,%d \n",frameTarget,attach,renderTarget,ZIDX(b));
	glFramebufferRenderbuffer(frameTarget, attach, renderTarget, ZIDX(b));
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_delete_renderbuffer)( vdynamic *b ) {
	unsigned int bb = (unsigned)b->v.i;
	//printf("gl_delete_renderbuffer%d \n",bb);
	glDeleteRenderbuffers(1, &bb);
	sdl_gl_get_error();
}

// buffer

HL_PRIM vdynamic *HL_NAME(gl_create_buffer)() {
	unsigned int b = 0;
	glGenBuffers(1, &b);
	sdl_gl_get_error();
	//printf("gl_create_buffer%d \n",b);
	return alloc_i32(b);
}

HL_PRIM void HL_NAME(gl_bind_buffer)( int target, vdynamic *b ) {
	//printf("gl_bind_buffer%d,%d \n",target,ZIDX(b));
	glBindBuffer(target, ZIDX(b));
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_buffer_data_size)( int target, int size, int param ) {
	//printf("gl_buffer_data_size%d,%d,%d \n",target,size,param);
	glBufferData(target, size, NULL, param);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_buffer_data)( int target, int size, vbyte *data, int param ) {
	//printf("gl_buffer_data%d,%d,%d \n",target,size,param);
	glBufferData(target, size, data, param);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_buffer_sub_data)( int target, int offset, vbyte *data, int srcOffset, int srcLength ) {
	//printf("gl_buffer_sub_data%d,%d,%d \n",target,offset,srcLength);
	glBufferSubData(target, offset, srcLength, data + srcOffset);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_enable_vertex_attrib_array)( int attrib ) {
	//printf("gl_enable_vertex_attrib_array%d \n",attrib);
	glEnableVertexAttribArray(attrib);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_disable_vertex_attrib_array)( int attrib ) {
	//printf("gl_disable_vertex_attrib_array%d \n",attrib);
	glDisableVertexAttribArray(attrib);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_vertex_attrib_pointer)( int index, int size, int type, bool normalized, int stride, int position ) {
	//printf("gl_vertex_attrib_pointer%d,%d,%d,%d,%d,%d \n",index,size,type,normalized,stride,position);
	glVertexAttribPointer(index, size, type, normalized, stride, (void*)(int_val)position);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_delete_buffer)( vdynamic *b ) {
	unsigned int bb = (unsigned)b->v.i;
	//printf("gl_delete_buffer%d \n",bb);
	glDeleteBuffers(1, &bb);
	sdl_gl_get_error();
}

// uniforms

HL_PRIM void HL_NAME(gl_uniform1i)( vdynamic *u, int i ) {
	//printf("gl_uniform1i%d,%d \n",u->v.i,i);
	glUniform1i(u->v.i, i);
	sdl_gl_get_error();
}

HL_PRIM void HL_NAME(gl_uniform4fv)( vdynamic *u, vbyte *buffer, int bufPos, int count ) {
	//printf("gl_uniform4fv%d,%d \n",u->v.i,count);
#	ifdef GL_LOG
	if( gl_log_active ) {
		int i;
		fprintf(gl_log_out,"\t");
		for(i=0;i<count;i++) {
			fprintf(gl_log_out,"%g",((float*)buffer)[bufPos + i]);
			if( i < count-1 ) {
				fprintf(gl_log_out,",");
				if( ((i+1) & 3) == 0 ) fprintf(gl_log_out,"\n\t");
			}
		}
		fprintf(gl_log_out,"\n");
	}
#	endif
	glUniform4fv(u->v.i, count, (float*)buffer + bufPos);
	sdl_gl_get_error();
}

// draw

HL_PRIM void HL_NAME(gl_draw_elements)( int mode, int count, int type, int start ) {
	//printf("gl_draw_elements%d,%d,%d,%d \n",mode,count,type,start);
	glDrawElements(mode, count, type, (void*)(int_val)start);
	sdl_gl_get_error();
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
DEFINE_PRIM(_VOID,gl_pixel_storei,_I32 _I32);
DEFINE_PRIM(_BYTES,gl_get_string,_I32);
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
DEFINE_PRIM(_VOID,gl_tex_image2d,_I32 _I32 _I32 _I32 _I32 _I32 _I32 _I32 _BYTES);
DEFINE_PRIM(_VOID,gl_compressed_tex_image2d,_I32 _I32 _I32 _I32 _I32 _I32 _I32 _BYTES);
DEFINE_PRIM(_VOID,gl_generate_mipmap,_I32);
DEFINE_PRIM(_VOID,gl_delete_texture,_NULL(_I32));
DEFINE_PRIM(_NULL(_I32),gl_create_framebuffer,_NO_ARG);
DEFINE_PRIM(_VOID,gl_bind_framebuffer,_I32 _NULL(_I32));
DEFINE_PRIM(_VOID,gl_framebuffer_texture2d,_I32 _I32 _I32 _NULL(_I32) _I32);
DEFINE_PRIM(_VOID,gl_delete_framebuffer,_NULL(_I32));
DEFINE_PRIM(_VOID,gl_read_pixels,_I32 _I32 _I32 _I32 _I32 _I32 _BYTES);
DEFINE_PRIM(_NULL(_I32),gl_create_renderbuffer,_NO_ARG);
DEFINE_PRIM(_VOID,gl_bind_renderbuffer,_I32 _NULL(_I32));
DEFINE_PRIM(_VOID,gl_renderbuffer_storage,_I32 _I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_framebuffer_renderbuffer,_I32 _I32 _I32 _NULL(_I32));
DEFINE_PRIM(_VOID,gl_delete_renderbuffer,_NULL(_I32));
DEFINE_PRIM(_NULL(_I32),gl_create_buffer,_NO_ARG);
DEFINE_PRIM(_VOID,gl_bind_buffer,_I32 _NULL(_I32));
DEFINE_PRIM(_VOID,gl_buffer_data_size,_I32 _I32 _I32);
DEFINE_PRIM(_VOID,gl_buffer_data,_I32 _I32 _BYTES _I32);
DEFINE_PRIM(_VOID,gl_buffer_sub_data,_I32 _I32 _BYTES _I32 _I32);
DEFINE_PRIM(_VOID,gl_enable_vertex_attrib_array,_I32);
DEFINE_PRIM(_VOID,gl_disable_vertex_attrib_array,_I32);
DEFINE_PRIM(_VOID,gl_vertex_attrib_pointer,_I32 _I32 _I32 _BOOL _I32 _I32);
DEFINE_PRIM(_VOID,gl_delete_buffer,_NULL(_I32));
DEFINE_PRIM(_VOID,gl_uniform1i,_NULL(_I32) _I32);
DEFINE_PRIM(_VOID,gl_uniform4fv,_NULL(_I32) _BYTES _I32 _I32);
DEFINE_PRIM(_VOID,gl_draw_elements,_I32 _I32 _I32 _I32);

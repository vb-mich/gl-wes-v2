/*
Copyright (C) 2009  Lachlan Tychsen - Smith aka Adventus

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "wes.h"
#include "wes_shader.h"
#include "wes_matrix.h"

//shader global variables:
GLuint          sh_program;
GLuint          sh_fragment;
GLuint          sh_vertex;

//function declarations:
GLvoid
wes_shader_error(GLuint ind)
{
    int len;
    char* log;
    wes_gl->glGetShaderiv(ind, GL_INFO_LOG_LENGTH, &len);
    log = (char*) malloc(len * sizeof(char));
    wes_gl->glGetShaderInfoLog(ind, len, NULL, log);
    PRINT_ERROR("\nShader Error: %s", log);
    free(log);
}

GLvoid
wes_program_error(GLuint ind)
{
    int len;
    char* log;
    wes_gl->glGetProgramiv(ind, GL_INFO_LOG_LENGTH, &len);
    log = (char*) malloc(len * sizeof(char));
    wes_gl->glGetProgramInfoLog(ind, len, NULL, log);
    PRINT_ERROR("\nProgram Error: %s", log);
    free(log);
}

GLuint
wes_shader_create(char* fname, GLenum type)
{
    FILE*   file;
    int     size;
    char*   data;
    GLuint  index;
    GLint   success;

    //Load file into mem:
    file = fopen(fname, "rb");
	if (!file){
	    PRINT_ERROR("Could not find file: %s", fname);
		return (-1);
	}
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);
	data = (char*) malloc(size + 1);
	if (!data){
	    PRINT_ERROR("Could not allocate: %i bytes", size + 1);
    	return (-1);
    }
	if (fread(data, sizeof(char), size, file) != size){
        free(data);
        PRINT_ERROR("Could not read file: %s", fname);
		return (-1);
	}
	data[size] = '\0';
	fclose(file);

    //Compile:
    index = wes_gl->glCreateShader(type);
    wes_gl->glShaderSource(index, 1, &data, NULL);
    wes_gl->glCompileShader(index);
    free(data);

    //test status:
    wes_gl->glGetShaderiv(index, GL_COMPILE_STATUS, &success);
    if (success){
        return index;
    } else {
        wes_shader_error(index);
        wes_gl->glDeleteShader(index);
        return (-1);
    }

}


GLvoid
wes_setattribloc(GLuint prog)
{
    wes_gl->glBindAttribLocation(prog, WES_APOS,       "aPosition");
    wes_gl->glBindAttribLocation(prog, WES_ATEXCOORD0, "aTexCoord0");
    wes_gl->glBindAttribLocation(prog, WES_ATEXCOORD1, "aTexCoord1");
    wes_gl->glBindAttribLocation(prog, WES_ATEXCOORD2, "aTexCoord2");
    wes_gl->glBindAttribLocation(prog, WES_ATEXCOORD3, "aTexCoord3");
    wes_gl->glBindAttribLocation(prog, WES_ANORMAL,    "aNormal");
    wes_gl->glBindAttribLocation(prog, WES_AFOGCOORD,  "aFogCoord");
    wes_gl->glBindAttribLocation(prog, WES_ACOLOR0,     "aColor");
    wes_gl->glBindAttribLocation(prog, WES_ACOLOR1,    "aColor2nd");
}

GLuint
wes_program_create(GLuint frag, GLuint vert)
{
    GLuint  prog;
    GLint   success;

    //Create & attach
    prog = wes_gl->glCreateProgram();
    wes_gl->glAttachShader(prog, frag);
    wes_gl->glAttachShader(prog, vert);
    wes_gl->glLinkProgram(prog);
    wes_setattribloc(prog);
    wes_gl->glLinkProgram(prog);

    //check status:
    wes_gl->glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (success || wes_gl->glGetError()){
        return prog;
    } else {
        wes_program_error(prog);
        wes_gl->glDeleteProgram(prog);
        return (-1);
    }
};


GLvoid
wes_shader_init()
{
    sh_fragment = wes_shader_create("WES.fsh", GL_FRAGMENT_SHADER);
    sh_vertex   = wes_shader_create("WES.vsh", GL_VERTEX_SHADER);
    sh_program  = wes_program_create(sh_fragment, sh_vertex);
}

GLvoid
wes_shader_destroy()
{

}




#ifndef __EXTENSIONS_H__
#define __EXTENSIONS_H__

#include <GL/glew.h>

/*
GL_ARB_framebuffer_object:                                     OK 
--------------------------
  glBindFramebuffer:                                           OK
  glBindRenderbuffer:                                          OK
  glBlitFramebuffer:                                           OK
  glCheckFramebufferStatus:                                    OK
  glDeleteFramebuffers:                                        OK
  glDeleteRenderbuffers:                                       OK
  glFramebufferRenderbuffer:                                   OK
  glFramebufferTexture1D:                                      OK
  glFramebufferTexture2D:                                      OK
  glFramebufferTexture3D:                                      OK
  glFramebufferTextureLayer:                                   OK
  glGenFramebuffers:                                           OK
  glGenRenderbuffers:                                          OK
  glGenerateMipmap:                                            OK
  glGetFramebufferAttachmentParameteriv:                       OK
  glGetRenderbufferParameteriv:                                OK
  glIsFramebuffer:                                             OK
  glIsRenderbuffer:                                            OK
  glRenderbufferStorage:                                       OK
  glRenderbufferStorageMultisample:                            OK
*/


struct FramebufferExt
{
	enum Type
	{
		ChooseEXT,
		ChooseARB,
	};
	typedef decltype(glBindFramebuffer)			BindFramebuffer_f;
	typedef decltype(glGenFramebuffers)			GenFramebuffers_f;
	typedef decltype(glFramebufferTexture2D)	FramebufferTexture2D_f;
	typedef decltype(glCheckFramebufferStatus)	CheckFramebufferStatus_f;
	typedef decltype(glDeleteFramebuffers)		DeleteFramebuffers_f;
	typedef decltype(glGenerateMipmap)			GenerateMipmap_f;

	static BindFramebuffer_f		BindFramebuffer;
	static GenFramebuffers_f		GenFramebuffers;
	static FramebufferTexture2D_f	FramebufferTexture2D;
	static CheckFramebufferStatus_f	CheckFramebufferStatus;
	static DeleteFramebuffers_f		DeleteFramebuffers;
	static GenerateMipmap_f			GenerateMipmap;

	static void InitializeExtension(Type chooseType);
};


#endif //__EXTENSIONS_H__

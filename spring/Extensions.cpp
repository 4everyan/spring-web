
#include "Extensions.h"


FramebufferExt::BindFramebuffer_f		 FramebufferExt::BindFramebuffer;
FramebufferExt::GenFramebuffers_f		 FramebufferExt::GenFramebuffers;
FramebufferExt::FramebufferTexture2D_f	 FramebufferExt::FramebufferTexture2D;
FramebufferExt::CheckFramebufferStatus_f FramebufferExt::CheckFramebufferStatus;
FramebufferExt::DeleteFramebuffers_f	 FramebufferExt::DeleteFramebuffers;
FramebufferExt::GenerateMipmap_f		 FramebufferExt::GenerateMipmap;



void FramebufferExt::InitializeExtension(Type chooseType)
{
	switch( chooseType )
	{
	case ChooseEXT:
		BindFramebuffer		   = glBindFramebufferEXT;
		GenFramebuffers		   = glGenFramebuffersEXT;
		FramebufferTexture2D   = glFramebufferTexture2DEXT;
		CheckFramebufferStatus = glCheckFramebufferStatusEXT;
		DeleteFramebuffers	   = glDeleteFramebuffersEXT;
		GenerateMipmap		   = glGenerateMipmapEXT;
		break;

	case ChooseARB:
		BindFramebuffer		   = glBindFramebuffer;
		GenFramebuffers		   = glGenFramebuffers;
		FramebufferTexture2D   = glFramebufferTexture2D;
		CheckFramebufferStatus = glCheckFramebufferStatus;
		DeleteFramebuffers	   = glDeleteFramebuffers;
		GenerateMipmap		   = glGenerateMipmap;
		break;
	};
}

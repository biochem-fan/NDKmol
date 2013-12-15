//
//  EAGLView.h
//  MolecularViewer
//
//  Created by Takanori Nakane on 12/01/13.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

// This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
// The view content is basically an EAGL surface you render your OpenGL scene into.
// Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
@interface MolecularView : UIView
{
@private
    EAGLContext *context;
    GLint framebufferWidth, framebufferHeight;

    // The OpenGL ES names for the framebuffer and renderbuffer used to render to this view.
    GLuint defaultFramebuffer, colorRenderbuffer, depthRenderbuffer;
	GLuint msaaFramebuffer, msaaRenderBuffer, msaaDepthBuffer;
	bool msaaEnabled;
}

@property (nonatomic, retain) EAGLContext *context;
@property (readonly) GLint framebufferWidth, framebufferHeight;

- (void)setFramebuffer;
- (BOOL)presentFramebuffer;

@end

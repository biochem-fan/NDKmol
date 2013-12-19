/*  NDKmol - Molecular Viewer on Android NDK and iOS

     (C) Copyright 2011 - 2013, biochem_fan

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

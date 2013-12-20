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

#import <QuartzCore/QuartzCore.h>

#import "MolecularView.h"
#import "NdkView.h"

@interface MolecularView (PrivateMethods)
- (void)createFramebuffer;
- (void)deleteFramebuffer;
@end

@implementation MolecularView

@dynamic context;

@synthesize framebufferWidth, framebufferHeight;

// You must implement this method
+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

//The EAGL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:.
- (id)initWithCoder:(NSCoder*)coder
{
    self = [super initWithCoder:coder];
	if (self)
    {
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking,
                                        kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
                                        nil];
    }
	msaaEnabled = ([[[UIDevice currentDevice]systemVersion] floatValue] >= 4.0f);
    if (msaaEnabled) NSLog(@"msaa enabled");
	else  NSLog(@"msaa disabled");
	
    return self;
}

- (void)dealloc
{
    [self deleteFramebuffer];    
    [context release];
    
    [super dealloc];
}

- (EAGLContext *)context
{
    return context;
}

- (void)setContext:(EAGLContext *)newContext
{
    if (context != newContext)
    {
        [self deleteFramebuffer];
        
        [context release];
        context = [newContext retain];
        
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)createFramebuffer
{
    if (context && !defaultFramebuffer)
    {
        [EAGLContext setCurrentContext:context];
        
        // Create default framebuffer object.
        glGenFramebuffers(1, &defaultFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
        glGenRenderbuffers(1, &colorRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
        [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &framebufferWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &framebufferHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);
		
		// Create depth buffer
		glGenRenderbuffers(1, &depthRenderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, framebufferWidth, framebufferHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
        
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
		
		// for MSAA 
		// Reference: http://www.gandogames.com/2010/07/tutorial-using-anti-aliasing-msaa-in-the-iphone/
		if (msaaEnabled) {
			glGenFramebuffersOES(1, &msaaFramebuffer);
			glGenRenderbuffersOES(1, &msaaRenderBuffer);
			glBindFramebufferOES(GL_FRAMEBUFFER_OES, msaaFramebuffer);
			glBindRenderbufferOES(GL_RENDERBUFFER_OES, msaaRenderBuffer);
			glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER_OES, 4, GL_RGB5_A1_OES, framebufferWidth, framebufferHeight);
			glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, msaaRenderBuffer);
			glGenRenderbuffersOES(1, &msaaDepthBuffer); 
			glBindRenderbufferOES(GL_RENDERBUFFER_OES, msaaDepthBuffer);
			glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER_OES, 4, GL_DEPTH_COMPONENT16_OES, framebufferWidth , framebufferHeight);
			glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, msaaDepthBuffer);
		}
	}
}

- (void)deleteFramebuffer
{
//	NSLog(@"deleteFramebuffer begin");
    if (context)
    {
        [EAGLContext setCurrentContext:context];
        
        if (defaultFramebuffer)
        {
            glDeleteFramebuffers(1, &defaultFramebuffer);
            defaultFramebuffer = 0;
        }
        
        if (colorRenderbuffer)
        {
            glDeleteRenderbuffers(1, &colorRenderbuffer);
            colorRenderbuffer = 0;
        }
		
		if (depthRenderbuffer)
        {
            glDeleteRenderbuffers(1, &depthRenderbuffer);
            depthRenderbuffer = 0;
        }
    }
}

- (void)setFramebuffer
{
//	NSLog(@"setFramebuffer begin");
    if (context)
    {
        [EAGLContext setCurrentContext:context];
        
        if (!defaultFramebuffer) {
            [self createFramebuffer];
//			NSLog(@"nativeGLInit called");
			nativeGLInit();
			glFlush();
		}
        
        glBindFramebuffer(GL_FRAMEBUFFER, msaaEnabled ? msaaFramebuffer : defaultFramebuffer);
        glViewport(0, 0, framebufferWidth, framebufferHeight);
    }
}

- (BOOL)presentFramebuffer
{
//	NSLog(@"presentFramebuffer begin");
    BOOL success = FALSE;
    
    if (context)
    {
		if (msaaEnabled) {
			GLenum attachments[] = {GL_DEPTH_ATTACHMENT_OES};
			glDiscardFramebufferEXT(GL_READ_FRAMEBUFFER_APPLE, 1, attachments);
			
			glBindFramebufferOES(GL_READ_FRAMEBUFFER_APPLE, msaaFramebuffer);
			glBindFramebufferOES(GL_DRAW_FRAMEBUFFER_APPLE, colorRenderbuffer);
			glResolveMultisampleFramebufferAPPLE();
		}
	
		[EAGLContext setCurrentContext:context];
		glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
		success = [context presentRenderbuffer:GL_RENDERBUFFER_OES];	
	}
    
    return success;
}

- (void)layoutSubviews
{
    // The framebuffer will be re-created at the beginning of the next setFramebuffer method call.
    [self deleteFramebuffer];
}

@end

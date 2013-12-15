//
//  MolecularViewerViewController.m
//  MolecularViewer
//
//  Created by Takanori Nakane on 12/01/13.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>

#import "MolecularViewerViewController.hh"
#import "MolecularView.h"
#import "SettingViewController.h"

#include "GLES.hpp"

@interface MolecularViewerViewController ()
@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) CADisplayLink *displayLink;
@end

@implementation MolecularViewerViewController

@synthesize animating, context, displayLink, settingOpened;
@synthesize proteinMode, hetatmMode , symmetryMode, colorMode, nucleicAcidMode;
@synthesize showSidechain, showUnitcell, showSolvents, doNotSmoothen, symopHetatms;


- (void)loadMolecule: (NSString *) fileName {
    NSString* filePath = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:fileName];
	nativeLoadProtein([filePath UTF8String]);
	[self resetView];
}

- (void)resetView {
	proteinMode = MAINCHAIN_THICKRIBBON; hetatmMode = HETATM_STICK; symmetryMode = SYMOP_BIOMT;
	colorMode = COLOR_CHAINBOW; nucleicAcidMode = BASE_LINE;
	showSidechain = showUnitcell = showSolvents = symopHetatms = doNotSmoothen = FALSE;
	settingOpened = FALSE;
	nativeAdjustZoom(&objX, &objY, &objZ, &cameraZ, &slabNear, &slabFar, (symmetryMode == SYMOP_BIOMT));
	rotationQ.x = 0; rotationQ.y = 0; rotationQ.z = 0; rotationQ.w = -1;
}

- (void)awakeFromNib
{	
	// GL context creation
    EAGLContext *aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    if (!aContext)
        NSLog(@"Failed to create ES context");
    else if (![EAGLContext setCurrentContext:aContext])
        NSLog(@"Failed to set ES context current");
	self.context = aContext;
	[aContext release];
	
	// For Retina display.
	if ([self.view respondsToSelector:@selector(setContentScaleFactor:)]) {
		// iPad1 (iOS < 4.0) doesn't have this property so this call must be guarded.
		self.view.contentScaleFactor = [UIScreen mainScreen].scale;
		// TODO: for better quality, lineWidth must be scaled accordingly.
	}
	
    [(MolecularView *)self.view setContext:context];
    [(MolecularView *)self.view setFramebuffer];
	

    // Initialize Shader
	shaderProgram = CreateShader(vertexShader, fragmentShader);
    
	if (shaderProgram != 0) {
	    shaderVertexPosition = glGetAttribLocation(shaderProgram, "vertexPosition");
        shaderProjectionMatrix = glGetUniformLocation(shaderProgram, "projectionMatrix");
        shaderModelViewMatrix = glGetUniformLocation(shaderProgram, "modelviewMatrix");
        
        shaderVertexColor = glGetAttribLocation(shaderProgram, "vertexColor");
        shaderUseVertexColor = glGetUniformLocation(shaderProgram, "useVertexColor");
        shaderObjColor = glGetUniformLocation(shaderProgram, "objColor");
	} else {
        printf("Failed to create shader\n");
    }
    
    // Animation. I don't know about them.
	animating = FALSE;
    animationFrameInterval = 0;
    self.displayLink = nil;
	
	// UI handlers
	self.view.multipleTouchEnabled = YES;
	UIPinchGestureRecognizer *pinchGesture = [[UIPinchGestureRecognizer alloc]
											  initWithTarget:self action:@selector(handlePinchGesture:)];
	[self.view addGestureRecognizer:pinchGesture];
	[pinchGesture release];
	UIPanGestureRecognizer *panGesture = [[UIPanGestureRecognizer alloc]
										  initWithTarget:self action:@selector(handlePanGesture:)];
	panGesture.minimumNumberOfTouches = 2;
	panGesture.maximumNumberOfTouches = 2;
	[self.view addGestureRecognizer:panGesture];
	UIPanGestureRecognizer *singlePanGesture = [[UIPanGestureRecognizer alloc]
										  initWithTarget:self action:@selector(handleSinglePanGesture:)];
	singlePanGesture.minimumNumberOfTouches = 1;
	singlePanGesture.maximumNumberOfTouches = 1;
	[self.view addGestureRecognizer:singlePanGesture];
	[panGesture release];
	UILongPressGestureRecognizer *longPressGesture = [[UILongPressGestureRecognizer alloc]
													  initWithTarget:self action:@selector(handleLongPressGesture:)];
	[self.view addGestureRecognizer:longPressGesture];
	[longPressGesture release];
	UIRotationGestureRecognizer *rotationGesture = [[UIRotationGestureRecognizer alloc]
													  initWithTarget:self action:@selector(handleRotationGesture:)];
	[self.view addGestureRecognizer:rotationGesture];
	[rotationGesture release];
	
	// Load initial model
	[self loadMolecule: @"3V8T.pdb"];
}

// TODO: Add slab UI

- (void)handlePinchGesture:(id)sender { // Zoom
	UIPinchGestureRecognizer *pinch = (UIPinchGestureRecognizer*)sender;
	CGFloat scale = [pinch scale];
	
	cameraZ = currentCameraZ / (scale * 0.8f);
	[self drawFrame]; // CHECK: Should I use setNeedsDisplay??
}

- (void)handlePanGesture:(id)sender { // Translation
	UIPanGestureRecognizer *pan = (UIPanGestureRecognizer*)sender;
	CGPoint point = [pan translationInView:self.view];
	
	float scaleFactor = 0.13f;
	if (cameraZ > -150) scaleFactor = 0.035f; // FIXME: improve
	if (cameraZ > -50) scaleFactor = 0.02f;
	float px, py, pz = 0;
	px = point.x * scaleFactor; py = -point.y * scaleFactor;
	float tx, ty, tz;
	Quaternion rotinv = rotationQ.clone().normalize();
	rotinv.rotateVector(px, py, pz, &tx, &ty, &tz);
	objX = currentX + tx;
	objY = currentY + ty;
	objZ = currentZ + tz;
	[self drawFrame];
}

- (void)handleSinglePanGesture:(id)sender { // Rotation
	UIPanGestureRecognizer *pan = (UIPanGestureRecognizer*)sender;
	CGPoint point = [pan translationInView:self.view];
	
	// TODO: cache the result. but what if screen size changes dynamically? 
	CGRect rect = [[UIScreen mainScreen] applicationFrame];
	float dx = point.x / rect.size.width, dy = point.y / rect.size.height;
	float r = (float)sqrt(dx * dx + dy * dy);
	if (r == 0) return;
	
	float rs = (float)sin(r * M_PI) / r;
	Quaternion dq(rs * dy, rs * dx, 0, (float)cos(r * M_PI)); 
	rotationQ = Quaternion::multiply(dq, currentQ);
	
	[self drawFrame];
}
- (void)handleRotationGesture:(id)sender { // Rotation Z
	UIRotationGestureRecognizer *rot = (UIRotationGestureRecognizer*)sender;
	CGFloat rotation = -[rot rotation] / 2;
		
	Quaternion dq(0, 0, (float)sin(rotation), (float)cos(rotation)); 
	rotationQ = Quaternion::multiply(dq, currentQ);
	
	[self drawFrame];
}

- (void) touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	UITouch *touch = [touches anyObject];
	CGPoint location = [touch locationInView:self.view];
	
	[super touchesBegan:touches withEvent:event];
	
	startX = location.x;
	startY = location.y;
	currentX = objX;
	currentY = objY;
	currentZ = objZ;
	currentSlabNear = slabNear;
	currentSlabFar = slabFar;
	currentCameraZ = cameraZ;
	currentQ = rotationQ;
}

- (void) touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event { // Rotation
}

// Open Setting panel
- (void)handleLongPressGesture:(id)sender {	
	if (settingOpened) return; // LongPressGesture might fire twice.
	settingOpened = TRUE;
	SettingViewController *vc = [[SettingViewController alloc] 
								 initWithNibName:@"SettingPanel" 
								 bundle:[NSBundle mainBundle]];
	[self.navigationController pushViewController:vc animated:YES];
	[vc release];
}

- (void)dealloc
{
    if (program)
    {
        glDeleteProgram(program);
        program = 0;
    }
    
    // Tear down context.
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
    
    [context release];
    
    [super dealloc];
}

// Rebuild scene
- (void)viewWillAppear:(BOOL)animated
{
	//    [self startAnimation]; // FIXME: "correct" way of disabling this
	
	settingOpened = FALSE;
	[self.navigationController setNavigationBarHidden: TRUE];
	
	buildScene(proteinMode, hetatmMode, symmetryMode, colorMode, showSidechain, 
			   showUnitcell, nucleicAcidMode, showSolvents, FALSE, doNotSmoothen, symopHetatms);
    [super viewWillAppear:animated];
	[self drawFrame];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [self stopAnimation];
    
    [super viewWillDisappear:animated];
}

- (void)viewDidUnload
{
	[super viewDidUnload];
	
    if (program)
    {
        glDeleteProgram(program);
        program = 0;
    }
	
    // Tear down context.
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
	self.context = nil;	
}

- (NSInteger)animationFrameInterval
{
    return animationFrameInterval;
}

- (void)setAnimationFrameInterval:(NSInteger)frameInterval
{
    /*
	 Frame interval defines how many display frames must pass between each time the display link fires.
	 The display link will only fire 30 times a second when the frame internal is two on a display that refreshes 60 times a second. The default frame interval setting of one will fire 60 times a second when the display refreshes at 60 times a second. A frame interval setting of less than one results in undefined behavior.
	 */
    if (frameInterval >= 1)
    {
        animationFrameInterval = frameInterval;
        
        if (animating)
        {
            [self stopAnimation];
            [self startAnimation];
        }
    }
}

- (void)startAnimation
{
	NSLog(@"startAnimation");
    if (!animating)
    {
        CADisplayLink *aDisplayLink = [[UIScreen mainScreen] displayLinkWithTarget:self selector:@selector(drawFrame)];
        [aDisplayLink setFrameInterval:animationFrameInterval];
        [aDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        self.displayLink = aDisplayLink;
        
        animating = TRUE;
    }
}

- (void)stopAnimation
{
    if (animating)
    {
        [self.displayLink invalidate];
        self.displayLink = nil;
        animating = FALSE;
    }
}

- (void)drawFrame
{
    [(MolecularView *)self.view setFramebuffer];
	
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float cameraNear = -cameraZ + slabNear;
	if (cameraNear < 1) cameraNear = 1;
	float cameraFar = -cameraZ + slabFar;
	if (cameraNear + 1 > cameraFar) cameraFar = cameraNear + 1;
	double xmin, xmax, ymin, ymax, zNear = cameraNear, zFar = cameraFar,
	aspect = [(MolecularView *)self.view framebufferWidth] / (float)[(MolecularView *)self.view framebufferHeight];
	ymax = zNear * tan(20 * M_PI / 360.0);
	ymin = -ymax;
	xmin = ymin * aspect;
	xmax = ymax * aspect;
	Mat16 projectionMatrix = matrixFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar);
    
    glFogf(GL_FOG_START, zNear * 0.3 + zFar * 0.7);
	glFogf(GL_FOG_END, zFar);
    
    currentModelViewMatrix = translationMatrix(0, 0, cameraZ);
    float ax, ay, az;
	rotationQ.getAxis(&ax, &ay, &az);
	Mat16 tmp = rotationMatrix(rotationQ.getAngle(), ax, ay, az);
    currentModelViewMatrix = multiplyMatrix(currentModelViewMatrix, tmp);
    tmp = translationMatrix(objX, objY, objZ);
    currentModelViewMatrix = multiplyMatrix(currentModelViewMatrix, tmp);
    
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(shaderProjectionMatrix, 1, GL_FALSE, projectionMatrix.m);
    glUniform1f(shaderUseVertexColor, 0);
    glUniform4f(shaderObjColor, 0, 1, 0, 1);
    
//  glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//	glTranslatef(0, 0, cameraZ);
//	glRotatef(180 * rotationQ.getAngle() / M_PI, ax, ay, az);
//	glTranslatef(objX, objY, objZ);

    nativeGLRender();
    
    [(MolecularView *)self.view presentFramebuffer];
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc. that aren't in use.
}
@end

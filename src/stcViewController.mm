

#import <QuartzCore/QuartzCore.h>

#import "stcViewController.h"
#import "EAGLView.h"


@interface stcViewController ()
@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) CADisplayLink *displayLink;
@end

@implementation stcViewController

@synthesize animating, context, displayLink;

- (void)awakeFromNib {
    
    EAGLContext *aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
    
    if (!aContext)
        NSLog(@"Failed to create ES context");
    else if (![EAGLContext setCurrentContext:aContext])
        NSLog(@"Failed to set ES context current");
    
	self.context = aContext;
	[aContext release];
	
    [(EAGLView *)self.view setContext:context];
    [(EAGLView *)self.view setFramebuffer];
        
    animating = FALSE;
    animationFrameInterval = 1;
    self.displayLink = nil;
    
    mGame = new Stc::Game();
    mPlatformObjC = new Stc::PlatformObjC(self);
    mGame->init(mPlatformObjC);
}

- (void)dealloc {
    if (program) {
        glDeleteProgram(program);
        program = 0;
    }
    
    // Tear down context.
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
    
    [context release];
    
    [super dealloc];
}

- (void)viewWillAppear:(BOOL)animated {
    [self startAnimation];
    [super viewWillAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated {
    [self stopAnimation];
    [super viewWillDisappear:animated];
}

- (void)viewDidUnload {
	[super viewDidUnload];
	
    if (program) {
        glDeleteProgram(program);
        program = 0;
    }

    // Tear down context.
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
	self.context = nil;	
}

- (NSInteger)animationFrameInterval {
    return animationFrameInterval;
}

- (void)setAnimationFrameInterval:(NSInteger)frameInterval {
    /*
	 Frame interval defines how many display frames must pass between each time 
	 the display link fires. The display link will only fire 30 times a second 
	 when the frame internal is two on a display that refreshes 60 times a second. 
	 The default frame interval setting of one will fire 60 times a second when 
	 the display refreshes at 60 times a second. A frame interval setting of less 
	 than one results in undefined behavior.
	 */
    if (frameInterval >= 1) {
        animationFrameInterval = frameInterval;
        
        if (animating) {
            [self stopAnimation];
            [self startAnimation];
        }
    }
}

- (void)startAnimation {
    if (!animating) {
        CADisplayLink *aDisplayLink = [CADisplayLink displayLinkWithTarget:self 
													 selector:@selector(drawFrame)];
		
        [aDisplayLink setFrameInterval:animationFrameInterval];
        [aDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] 
								forMode:NSDefaultRunLoopMode];
        self.displayLink = aDisplayLink;
        
        animating = TRUE;
    }
}

- (void)stopAnimation {
    if (animating) {
        [self.displayLink invalidate];
        self.displayLink = nil;
        animating = FALSE;
    }
}

- (void)drawFrame {
    [(EAGLView *)self.view setFramebuffer];

    mGame->update();
    //mPlatformObjC->renderGame();
    
    [(EAGLView *)self.view presentFramebuffer];
}

- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc. that aren't in use.
    NSLog(@"-- Memory warning");
}

@end

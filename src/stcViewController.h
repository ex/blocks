
#import <UIKit/UIKit.h>

#import <OpenGLES/EAGL.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#include "PlatformObjC.hpp"

@interface stcViewController : UIViewController <UIAlertViewDelegate> {
@private    
    EAGLContext *context;
    BOOL animating;
    NSInteger animationFrameInterval;
    CADisplayLink *displayLink;
    
    stc::Game *mGame; 
    stc::PlatformObjC *mPlatformObjC;
    
    UIEvent *mEventTouchStart;
    UIEvent *mEventTouchEnd;

	UIAlertView *mAlertPaused;
	UIAlertView *mAlertRestart;	
}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;

- (void) startAnimation;
- (void) stopAnimation;

- (UIEvent *) getEventsTouchStart;
- (void) clearEventsTouchStart;

- (UIEvent *) getEventsTouchEnd;
- (void) clearEventsTouchEnd;

- (UIAlertView *) alertPaused;
- (void) setAlertPaused:(UIAlertView *)alertPaused;

- (UIAlertView *) alertRestart;
- (void) setAlertRestart:(UIAlertView *)alertRestart;

@end

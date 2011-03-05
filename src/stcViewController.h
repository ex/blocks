
#import <UIKit/UIKit.h>

#import <OpenGLES/EAGL.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#include "PlatformObjC.hpp"

@interface stcViewController : UIViewController {
@private    
    EAGLContext *context;
    BOOL animating;
    NSInteger animationFrameInterval;
    CADisplayLink *displayLink;
    
    Stc::Game *mGame; 
    Stc::PlatformObjC *mPlatformObjC;
    
    UIEvent *mEventTouchStart;
    UIEvent *mEventTouchEnd;
}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;

- (void)startAnimation;
- (void)stopAnimation;

- (UIEvent *)getEventsTouchStart;
- (void)clearEventsTouchStart;

- (UIEvent *)getEventsTouchEnd;
- (void)clearEventsTouchEnd;

@end

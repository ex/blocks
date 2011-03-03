

#import <UIKit/UIKit.h>

@class stcViewController;

@interface stcAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    stcViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet stcViewController *viewController;

@end


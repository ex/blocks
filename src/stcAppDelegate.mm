
#import "stcAppDelegate.h"
#import "stcViewController.h"

@implementation stcAppDelegate

@synthesize window;
@synthesize viewController;

- (BOOL)application:(UIApplication *)application 
					 didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [self.window addSubview:self.viewController.view];
	
	[[UIApplication sharedApplication] setStatusBarOrientation:UIInterfaceOrientationLandscapeRight];

    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    [self.viewController stopAnimation];
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    [self.viewController startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application {
    [self.viewController stopAnimation];
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Handle any background procedures not related to animation here.
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Handle any foreground procedures not related to animation here.
}

- (void)dealloc {
    [viewController release];
    [window release];
    
    [super dealloc];
}

@end

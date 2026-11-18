#import <UIKit/UIKit.h>
#import <test/test.h>

@interface AppDelegate : UIResponder <UIApplicationDelegate>
@property (strong, nonatomic) UIWindow *window;
@end

@implementation AppDelegate
@end

int main(int argc, char *argv[]) {
    return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class])) + test_value();
}

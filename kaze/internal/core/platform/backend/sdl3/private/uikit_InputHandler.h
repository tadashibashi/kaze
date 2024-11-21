#import <UIKit/UIKit.h>

@interface KZInputHandler : NSObject <UITextFieldDelegate>
@property (nonatomic, strong) UITextField *hiddenTextField;

- (void)startListening;
- (void)stopListening;

@end

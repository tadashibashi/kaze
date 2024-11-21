#import "uikit_InputHandler.h"
#import <kaze/core/debug.h>

#import <SDL3/SDL.h>

@implementation KZInputHandler

- (instancetype)init {
    self = [super init];
    if (self) {
        _hiddenTextField = [[UITextField alloc] initWithFrame: CGRectZero];
        _hiddenTextField.hidden = YES;
        _hiddenTextField.delegate = self;
        _hiddenTextField.autocorrectionType = UITextAutocorrectionTypeNo;
        _hiddenTextField.spellCheckingType = UITextSpellCheckingTypeNo;

        [[UIApplication sharedApplication].keyWindow addSubview: _hiddenTextField];
    }

    return self;
}

- (void)startListening {
    KAZE_CORE_LOG("Started listening to hidden text field");
    [self.hiddenTextField becomeFirstResponder];
}

- (void)stopListening {
    KAZE_CORE_LOG("Stopped listening to invisible text field");
    [self.hiddenTextField resignFirstResponder];
}

- (BOOL)textFieldShouldReturn: (UITextField *)textField {
    SDL_Event e;
    e.type = SDL_EVENT_KEY_DOWN;
    e.key.key = SDLK_RETURN;
    e.key.scancode = SDL_SCANCODE_RETURN;
    e.key.repeat = false;
    e.key.down = true;

    SDL_PushEvent(&e);

    e.type = SDL_EVENT_KEY_UP;
    e.key.down = false;

    SDL_PushEvent(&e);

    KAZE_CORE_LOG("Pressed Return!");

    return YES;
}

@end

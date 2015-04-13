//
//  DDNAPopup.m
//  ImageMessageDemo
//
//  Created by David White on 01/12/2014.
//  Copyright (c) 2014 deltadna. All rights reserved.
//

#import "DDNAPopup.h"

@interface DDNABasicPopup () {
    
    UIImage* _spriteMap;
    UIView* _shimView;
    UIView* _backgroundView;
    
    NSDictionary* _shimAction;
    NSDictionary* _backgroundAction;
    NSArray* _buttonActions;
    
    CGFloat _dimmedMaskAlpha;
    CGFloat _backgroundScale;
    
    NSDictionary* _image;
    
    BOOL _isShowing;
}

- (void)buttonPressed:(id)sender;

@end

@implementation DDNABasicPopup

- (id)init {
    return [self initWithFrame:[[UIScreen mainScreen] bounds]];
}

-(id)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        // More initialisation...
        
        _dimmedMaskAlpha = 0.5f;
        _backgroundScale = 1.0f;
        
        _shimView = [[UIView alloc] init];
        
        _backgroundView = [[UIView alloc] init];
        
        
    }
    
    return self;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    CGPoint locationPoint = [[touches anyObject] locationInView:self];
    UIView* viewYouWishToObtain = [self hitTest:locationPoint withEvent:event];
    
    if (viewYouWishToObtain == _shimView) {
        [self actionHandlerFor:@"shim" withAction:_shimAction];
    }
    
    if (viewYouWishToObtain == _backgroundView) {
        [self actionHandlerFor:@"background" withAction:_backgroundAction];
    }
}

+ (DDNABasicPopup*)popup {
    DDNABasicPopup* popup = [[[self class] alloc] init];
    return popup;
}

- (void)prepareWithImage:(NSDictionary *)image {
    
    if (self.beforePrepare != nil) {
        self.beforePrepare();
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
       
        if (image[@"url"]) {
            NSURL *url = [NSURL URLWithString:image[@"url"]];
            NSData *data = [NSData dataWithContentsOfURL:url];
            
            if (data != nil) {
                _spriteMap = [UIImage imageWithData:data];
                _image = [NSDictionary dictionaryWithDictionary:image];
                
                if (self.afterPrepare != nil) {
                    self.afterPrepare();
                }
            }
            
            
        }
    });
    
}

- (void)show {
    
    self.hidden = NO;
    self.alpha = 1.0;
    
    UIImage* backgroundImage;
    NSMutableArray* buttonImages = [[NSMutableArray alloc] init];
    
    // sprite map
    
    if (_image[@"spritemap"]) {
        NSDictionary* spritemap = _image[@"spritemap"];
        
        if (spritemap[@"background"]) {
            NSDictionary* bg = spritemap[@"background"];
            CGRect cropRect = CGRectMake(
                [bg[@"x"] integerValue],
                [bg[@"y"] integerValue],
                [bg[@"width"] integerValue],
                [bg[@"height"] integerValue]);
            
            CGImageRef imageRef = CGImageCreateWithImageInRect([_spriteMap CGImage], cropRect);
            
            backgroundImage = [UIImage imageWithCGImage:imageRef];
        }
        
        if (spritemap[@"buttons"]) {
            NSArray* btns = spritemap[@"buttons"];
            for (NSDictionary *btn in btns) {
                CGRect cropRect = CGRectMake(
                    [btn[@"x"] integerValue],
                    [btn[@"y"] integerValue],
                    [btn[@"width"] integerValue],
                    [btn[@"height"] integerValue]);
                
                CGImageRef imageRef = CGImageCreateWithImageInRect([_spriteMap CGImage], cropRect);
                UIImage* buttonImage = [UIImage imageWithCGImage:imageRef];
                
                [buttonImages addObject:buttonImage];
            }
        }
        
    }
    
    // shim
    
    if (_image[@"shim"]) {
        NSDictionary* shim = _image[@"shim"];
        if (shim[@"mask"]) {
            NSString* mask = shim[@"mask"];
            if ([mask isEqualToString:@"dimmed"]) {
                _shimView.backgroundColor = [UIColor colorWithRed:(0.0/255.0f) green:(0.0/255.0f) blue:(0.0/255.0f) alpha:_dimmedMaskAlpha];
            } else if ([mask isEqualToString:@"clear"]) {
                _shimView.backgroundColor = [UIColor clearColor];
            }
            if (![mask isEqualToString:@"none"]) {
                _shimView.userInteractionEnabled = YES;
                _shimView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
                _shimView.frame = self.bounds;

                [self addSubview:_shimView];
            }
        }
        if (shim[@"action"]) {
            _shimAction = [NSDictionary dictionaryWithDictionary:shim[@"action"]];
        }
    }
    
    // background
    
    if (_image[@"layout"]) {
        NSDictionary* layout = _image[@"layout"];
        
        UIInterfaceOrientation orientation = [[UIApplication sharedApplication] statusBarOrientation];
        BOOL landscape = (orientation == UIInterfaceOrientationLandscapeLeft || orientation == UIInterfaceOrientationLandscapeRight);
        
        NSDictionary * orientationDict = nil;
        if (layout[@"landscape"] && landscape) {
            orientationDict = layout[@"landscape"];
        } else if (layout[@"portrait"] && !landscape) {
            orientationDict = layout[@"portrait"];
        } else if (layout[@"landscape"]) {
            orientationDict = layout[@"landscape"];
        } else if (layout[@"portrait"]) {
            orientationDict = layout[@"portrait"];
        }
        
        if (orientationDict != nil) {
            
            if (orientationDict[@"background"]) {
                NSDictionary* background = orientationDict[@"background"];
                CGRect dim;
                if (background[@"cover"]) {
                    dim = [self renderAsCoverWithConstraints:background[@"cover"] andImage:backgroundImage];
                }
                else if (background[@"contain"]) {
                    dim = [self renderAsContainWithConstraints:background[@"contain"] andImage:backgroundImage];
                }
                
                _backgroundView = [[UIImageView alloc] initWithImage:backgroundImage];  // build the background before defining constraints on it
                _backgroundView.translatesAutoresizingMaskIntoConstraints = NO;
                _backgroundView.autoresizesSubviews = NO;
                _backgroundView.userInteractionEnabled = YES;
                
                NSLayoutConstraint* leftConstraint = [NSLayoutConstraint constraintWithItem:_backgroundView attribute:NSLayoutAttributeLeft relatedBy:NSLayoutRelationEqual toItem:self attribute:NSLayoutAttributeLeft multiplier:1.0f constant:dim.origin.x];
                NSLayoutConstraint* topConstraint = [NSLayoutConstraint constraintWithItem:_backgroundView attribute:NSLayoutAttributeTop relatedBy:NSLayoutRelationEqual toItem:self attribute:NSLayoutAttributeTop multiplier:1.0f constant:dim.origin.y];
                NSLayoutConstraint* widthConstraint = [NSLayoutConstraint constraintWithItem:_backgroundView attribute:NSLayoutAttributeWidth relatedBy:NSLayoutRelationEqual toItem:nil attribute:NSLayoutAttributeNotAnAttribute multiplier:1.0f constant:dim.size.width];
                NSLayoutConstraint* heightConstraint = [NSLayoutConstraint constraintWithItem:_backgroundView attribute:NSLayoutAttributeHeight relatedBy:NSLayoutRelationEqual toItem:nil attribute:NSLayoutAttributeNotAnAttribute multiplier:1.0f constant:dim.size.height];
                
                
                [self addSubview:_backgroundView];  // must do this before adding constraints

                [self addConstraint:leftConstraint];
                [self addConstraint:topConstraint];
                [_backgroundView addConstraint:widthConstraint];
                [_backgroundView addConstraint:heightConstraint];
                
                if (background[@"action"]) {
                    _backgroundAction = [NSDictionary dictionaryWithDictionary:background[@"action"]];
                }
            }
            
            if (orientationDict[@"buttons"]) {
                NSArray* buttons = orientationDict[@"buttons"];
                
                NSMutableArray* buttonActions = [[NSMutableArray alloc] init];
                
                for (int i = 0; i < [buttons count]; ++i) {
                    
                    UIImage* btnImage = buttonImages[i];
                    NSDictionary* btnDict = buttons[i];
                    
                    CGRect dim = CGRectMake([btnDict[@"x"] integerValue] * _backgroundScale,
                                            [btnDict[@"y"] integerValue] * _backgroundScale,
                                            btnImage.size.width * _backgroundScale,
                                            btnImage.size.height * _backgroundScale);
                    
                    UIButton* btn = [UIButton buttonWithType:UIButtonTypeCustom];
                    btn.translatesAutoresizingMaskIntoConstraints = NO;
                    btn.contentEdgeInsets = UIEdgeInsetsZero;
                    [btn setBackgroundImage:buttonImages[i] forState:UIControlStateNormal];
                    [btn setTag:i+1];   // help identify the button when clicked
                    [btn addTarget:self action:@selector(buttonPressed:) forControlEvents:UIControlEventTouchUpInside];
                    
                    NSLayoutConstraint* leftConstraint = [NSLayoutConstraint constraintWithItem:btn attribute:NSLayoutAttributeLeft relatedBy:NSLayoutRelationEqual toItem:_backgroundView attribute:NSLayoutAttributeLeft multiplier:1.0f constant:dim.origin.x];
                    NSLayoutConstraint* topConstraint = [NSLayoutConstraint constraintWithItem:btn attribute:NSLayoutAttributeTop relatedBy:NSLayoutRelationEqual toItem:_backgroundView attribute:NSLayoutAttributeTop multiplier:1.0f constant:dim.origin.y];
                    NSLayoutConstraint* widthConstraint = [NSLayoutConstraint constraintWithItem:btn attribute:NSLayoutAttributeWidth relatedBy:NSLayoutRelationEqual toItem:nil attribute:NSLayoutAttributeNotAnAttribute multiplier:1.0f constant:dim.size.width];
                    NSLayoutConstraint* heightConstraint = [NSLayoutConstraint constraintWithItem:btn attribute:NSLayoutAttributeHeight relatedBy:NSLayoutRelationEqual toItem:nil attribute:NSLayoutAttributeNotAnAttribute multiplier:1.0f constant:dim.size.height];
                    
                    [_backgroundView addSubview:btn];
                    [_backgroundView addConstraint:leftConstraint];
                    [_backgroundView addConstraint:topConstraint];
                    [btn addConstraint:widthConstraint];
                    [btn addConstraint:heightConstraint];
                    
                    // action
                    if (btnDict[@"action"]) {
                        [buttonActions addObject:btnDict[@"action"]];
                    }
                }
                
                _buttonActions = [NSArray arrayWithArray:buttonActions];
            }

        }
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
        
        // Add to the top window...
        if (!self.superview) {
            NSEnumerator *frontToBackWindows = [[[UIApplication sharedApplication] windows] reverseObjectEnumerator];
            
            for (UIWindow *window in frontToBackWindows) {
                if (window.windowLevel == UIWindowLevelNormal) {
                    [window addSubview:self];
                    _isShowing = YES;
                    break;
                }
            }
        }
    });
}

- (void)close
{
    if (_isShowing) {
        if (self.beforeClose != nil) {
            self.beforeClose();
        }
        
        [self removeFromSuperview];
        
        if (self.afterClose != nil) {
            self.afterClose();
        }
        _isShowing = NO;
    }
    
}

- (CGRect)renderAsCoverWithConstraints: (NSDictionary*)constraints andImage: (UIImage *)image
{
    CGRect screenRect = [[UIScreen mainScreen] bounds];
    CGFloat screenWidth = screenRect.size.width;
    CGFloat screenHeight = screenRect.size.height;
    
    float scale = MAX(screenWidth / image.size.width, screenHeight / image.size.height);
    float width = image.size.width * scale;
    float height = image.size.height * scale;
    
    float top = screenHeight / 2.0f - height / 2.0f;    // default center
    float left = screenWidth / 2.0f - width / 2.0f;
    
    if (constraints[@"valign"]) {
        NSString * valign = constraints[@"valign"];
        if ([valign isEqualToString:@"top"]) {
            top = 0;
        }
        else if ([valign isEqualToString:@"bottom"]) {
            top = screenHeight - height;
        }
    }
    
    if (constraints[@"halign"]) {
        NSString * halign = constraints[@"halign"];
        if ([halign isEqualToString:@"left"]) {
            left = 0;
        }
        else if ([halign isEqualToString:@"right"]) {
            left = screenWidth - width;
        }
    }
    
    _backgroundScale = scale;
    
    return CGRectMake(left, top, width, height);
}

- (CGRect)renderAsContainWithConstraints: (NSDictionary *)constraints andImage: (UIImage *)image
{
    CGRect screenRect = [[UIScreen mainScreen] bounds];
    CGFloat screenWidth = screenRect.size.width;
    CGFloat screenHeight = screenRect.size.height;
    
    float lc = 0, rc = 0, tc = 0, bc = 0;
    if (constraints[@"left"]) {
        lc = [self readConstraintPixels:constraints[@"left"] screenEdge:screenWidth];
    }
    
    if (constraints[@"right"]) {
        rc = [self readConstraintPixels:constraints[@"right"] screenEdge:screenWidth];
    }
    
    if (constraints[@"top"]) {
        tc = [self readConstraintPixels:constraints[@"top"] screenEdge:screenHeight];
    }
    
    if (constraints[@"bottom"]) {
        bc = [self readConstraintPixels:constraints[@"bottom"] screenEdge:screenHeight];
    }
    
    float ws = (screenWidth - lc - rc) / image.size.width;
    float hs = (screenHeight - tc - bc) / image.size.height;
    float scale = MIN(ws, hs);
    float height = image.size.height * scale;
    float width = image.size.width * scale;
    
    float top = ((screenHeight - tc - bc) / 2.0f - height / 2.0f) + tc; // default "center"
    float left = ((screenWidth - lc - rc) / 2.0f - width / 2.0f) + lc;
    
    if (constraints[@"valign"]) {
        NSString * valign = constraints[@"valign"];
        if ([valign isEqualToString:@"top"]) {
            top = tc;
        }
        else if ([valign isEqualToString:@"bottom"]) {
            top = screenHeight - height - bc;
        }
    }
    
    if (constraints[@"halign"]) {
        NSString * halign = constraints[@"halign"];
        if ([halign isEqualToString:@"left"]) {
            left = lc;
        }
        else if ([halign isEqualToString:@"right"]) {
            left = screenWidth - width - rc;
        }
    }
    
    _backgroundScale = scale;
    
    return CGRectMake(left, top, width, height);
}

- (CGFloat)readConstraintPixels: (NSString *)constraint screenEdge: (CGFloat)screenEdge
{
    CGFloat result = 0;
    
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:@"(\\d+)(px|%)" options:NSRegularExpressionCaseInsensitive error:nil];
    
    NSArray *matches = [regex matchesInString:constraint options:0 range:NSMakeRange(0, [constraint length])];

    if ([matches count] > 0) {
        NSTextCheckingResult *match = matches[0];
        NSRange matchValueRange = [match rangeAtIndex:1];
        NSRange matchTypeRange = [match rangeAtIndex:2];
        
        NSString *matchValueString = [constraint substringWithRange:matchValueRange];
        result = matchValueString.intValue;
        
        NSString *matchTypeString = [constraint substringWithRange:matchTypeRange];
        if ([matchTypeString isEqualToString:@"%"]) {
            result = result / 100.0f * screenEdge;
        }
    }
    
    return result;
}

- (void)buttonPressed:(UIButton*)sender
{
    NSString* name = [NSString stringWithFormat:@"button%ld", (long)sender.tag];
    [self actionHandlerFor:name withAction:_buttonActions[sender.tag-1]];
}

- (void)actionHandlerFor:(NSString*)name withAction: (NSDictionary*)action
{
    NSString* type = action[@"type"];
    NSString* value = action[@"value"];
    
    if ([type isEqualToString:@"none"]) {
        return; // do nothing
    }
    else if ([type isEqualToString:@"action"]) {
        if (value != nil && self.onAction != nil) {
            self.onAction(name, type, value);
        }
    }
    else if ([type isEqualToString:@"link"]) {
        if (value != nil) {
            [[UIApplication sharedApplication] openURL:[NSURL URLWithString:value]];
        }
        if (self.onAction != nil) {
            self.onAction(name, type, value);
        }
    }
    else if ([type isEqualToString:@"dismiss"]) {
        if (self.dismiss != nil) {
            self.dismiss(name);
        }
    }
    
    [self close];
}

@end

/*
This file is part of OGS Mahjong components:
  https://github.com/OGStudio/ogs-mahjong-components

Copyright (C) 2018 Opensource Game Studio

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef OGS_MAHJONG_COMPONENTS_IOS_H
#define OGS_MAHJONG_COMPONENTS_IOS_H

#import <UIKit/UIKit.h>

// AppDelegate Start
@interface AppDelegate : UIResponder <UIApplicationDelegate>
    @property (nonatomic, strong) UIWindow *window;
@end

// AppDelegate End

// RenderVC Start
@interface RenderVC : UIViewController

// RenderVC End
    // RenderVC+FrameReporting Start
    @property (nonatomic, copy) void (^frame)();
    
    // RenderVC+FrameReporting End
// RenderVC Start
@end

// RenderVC End

#endif // OGS_MAHJONG_COMPONENTS_IOS_H

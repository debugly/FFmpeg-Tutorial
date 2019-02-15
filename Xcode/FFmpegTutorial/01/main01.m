//
//  main01.m
//  Tutorial01
//
//  Created by Matt Reach on 2019/2/14.
//  Copyright © 2019 Awesome FFmpeg Study Demo. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "tutorial01.h"

int main(int argc, const char * argv[]) {

    NSString *path = [NSString stringWithUTF8String:__FILE__];
    NSRange range = [path rangeOfString:@"/Xcode/"];
    if (range.location != NSNotFound) {
        NSString *dir = [path substringToIndex:range.location];
        NSString *moviePath = [dir stringByAppendingPathComponent:@"assets/xp5.mp4"];
        
        const char *source = argv[0];
        const char *movie = [moviePath UTF8String];
        const char *argvs[] = {source,movie};
        main01(2, argvs);
        
    }
    return 0;
}

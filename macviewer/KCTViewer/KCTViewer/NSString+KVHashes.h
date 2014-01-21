//
//  NSString+KVHashes.h
//  KCTViewer
//
//  Created by Johannes Ekberg on 2014-01-21.
//  Copyright (c) 2014 MacaroniCode. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSString (KVHashes)

- (NSString *)sha1;
- (int32_t)s_crc32;

@end
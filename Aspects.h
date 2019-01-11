//
//  Aspects.h
//  Aspects - A delightful, simple library for aspect oriented programming.
//
//  Copyright (c) 2014 Peter Steinberger. Licensed under the MIT license.
//

#import <Foundation/Foundation.h>

//提供了 方法执行后插入  执行替换 执行后插入  昙花一现插入（自己起的名字）
typedef NS_OPTIONS(NSUInteger, AspectOptions) {
    AspectPositionAfter   = 0,            /// Called after the original implementation (default)
    AspectPositionInstead = 1,            /// Will replace the original implementation.
    AspectPositionBefore  = 2,            /// Called before the original implementation.
    
    AspectOptionAutomaticRemoval = 1 << 3 /// Will remove the hook after the first execution.
};

/// Opaque Aspect Token that allows to deregister the hook.
//透明协议AspectToken 提供了撤销Hook 的方法
@protocol AspectToken <NSObject>

/// Deregisters an aspect.
/// @return YES if deregistration is successful, otherwise NO.
- (BOOL)remove;

@end

/// The AspectInfo protocol is the first parameter of our block syntax.
//AspectInfo 协议是 block 规则的第一个 参数
@protocol AspectInfo <NSObject>

/// The instance that is currently hooked.
//hook的实例
- (id)instance;

/// The original invocation of the hooked method.
//原始的invocation 要执行的方法包装成了 NSInvovation
- (NSInvocation *)originalInvocation;

/// All method arguments, boxed. This is lazily evaluated.
//执行函数的参数 懒加载的形式
- (NSArray *)arguments;

@end

/**
 Aspects uses Objective-C message forwarding to hook into messages. This will create some overhead. Don't add aspects to methods that are called a lot. Aspects is meant for view/controller code that is not called a 1000 times per second.
 
 Adding aspects returns an opaque token which can be used to deregister again. All calls are thread safe.
 */
@interface NSObject (Aspects)

/// Adds a block of code before/instead/after the current `selector` for a specific class.
///+方法形式的hook
/// @param block Aspects replicates the type signature of the method being hooked.
///block 复制被hook的函数签名
/// The first parameter will be `id<AspectInfo>`, followed by all parameters of the method.
/// 复制的lblock的第一个参数是id<AspectInfo> 类型
/// These parameters are optional and will be filled to match the block signature.
///  这些参数会填充到 block的函数签名中
/// You can even use an empty block, or one that simple gets `id<AspectInfo>`.
///可以用空的 block  或者一个 id<AspectInfo>
/// @note Hooking static methods is not supported.  不支持hook 静态方法
/// @return A token which allows to later deregister the aspect. 返回一个AspectToken协议的类
+ (id<AspectToken>)aspect_hookSelector:(SEL)selector
                           withOptions:(AspectOptions)options
                            usingBlock:(id)block
                                 error:(NSError **)error;
///-方法形式的hook
/// Adds a block of code before/instead/after the current `selector` for a specific instance.
- (id<AspectToken>)aspect_hookSelector:(SEL)selector
                           withOptions:(AspectOptions)options
                            usingBlock:(id)block
                                 error:(NSError **)error;

@end


typedef NS_ENUM(NSUInteger, AspectErrorCode) {
    AspectErrorSelectorBlacklisted,                   /// Selectors like release, retain, autorelease are blacklisted. 对于release, retain, autorelease 方法的hook是被加入黑名单的
    AspectErrorDoesNotRespondToSelector,              /// Selector could not be found.找不到相应的响应方法
    AspectErrorSelectorDeallocPosition,               /// When hooking dealloc, only AspectPositionBefore is allowed. 对于delloc 只hook 执行前AspectPositionBefore
    AspectErrorSelectorAlreadyHookedInClassHierarchy, /// Statically hooking the same method in subclasses is not allowed. 子类方法是不能hook的
    AspectErrorFailedToAllocateClassPair,             /// The runtime failed creating a class pair. 没有说生成派生类
    AspectErrorMissingBlockSignature,                 /// The block misses compile time signature info and can't be called. block的签名丢失了
    AspectErrorIncompatibleBlockSignature,            /// The block signature does not match the method or is too large. 方法不匹配或者方法太大了 神奇（方法太大了....）
    
    AspectErrorRemoveObjectAlreadyDeallocated = 100   /// (for removing) The object hooked is already deallocated. hook的对象准备释放了
};

extern NSString *const AspectErrorDomain;

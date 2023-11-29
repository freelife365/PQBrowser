/**
*  @file        Version.h
*  @author      YangQi
*  @date        2017-04-07
*
*  @brief
*  @note
*
*  @version
*/

#pragma once

// 版本号说明
// VERSION_MAJOR: 主版本号
// VERSION_MINOR: 次版本号
// VERSION_REVISION: BUG修正版本号
// VERSION_BUILD: 编译版本号
// VERSION_SOURCE: svn代码提交版本号

// 版本号修改原则:
// 主版本号停留在 0 的版本号，即 0.x.x.x 应当视作还在内部开发阶段的代码，当发布第一
// 个版本时，版本号为 1.0.0.0

// 1. 当项目在进行了局部修改或 bug 修正时，主版本号和次版本号都不变，修正版本号加 1，
//    编译版本号复位为 -1，==>需要 ##人为修改##， 脚本会自动给编译版本号+1即为0；
// 2. 当项目在原有的基础上增加了部分功能时，主版本号不变，次版本号加 1，修正版本号复
//    位为0，编译版本号复位为 -1，==>需要 ##人为修改##，脚本会自动给编译版本号+1即为0；
// 3. 当项目在进行了重大修改或局部修正累积较多，而导致项目整体发生全局变化时，主版本
//    号加 1，次版本号、修正版本号复位为0，编译版本号复位为 -1，==>需要 ##人为修改##，
//    脚本会自动给编译版本号+1即为0；
// 4. 编译版本号编译器在编译过程中会  ##自动生成## (每次编译 +1)，不需要人为控制

#define _VERSIONTOSTRING(arg)          #arg
#define VERSIONTOSTRING(arg)           _VERSIONTOSTRING(arg)
#define VERSION_MAJOR                  1
#define VERSION_MINOR                  0
#define VERSION_REVISION               1
#define VERSION_BUILD                  0
#define VERSION_SOURCE                 9783aed
#define VERSION_FULL                   VERSION_MAJOR.VERSION_MINOR.VERSION_REVISION.VERSION_BUILD
#define VERSION_FULL_RC                VERSION_MAJOR,VERSION_MINOR,VERSION_REVISION,VERSION_BUILD
#define STR_VERSIOIN_FULL              VERSIONTOSTRING(VERSION_FULL)
#define STR_VERSION_FULL_BUILD         VERSIONTOSTRING(VERSION_FULL(build VERSION_SOURCE))



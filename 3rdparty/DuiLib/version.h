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

// �汾��˵��
// VERSION_MAJOR: ���汾��
// VERSION_MINOR: �ΰ汾��
// VERSION_REVISION: BUG�����汾��
// VERSION_BUILD: ����汾��
// VERSION_SOURCE: svn�����ύ�汾��

// �汾���޸�ԭ��:
// ���汾��ͣ���� 0 �İ汾�ţ��� 0.x.x.x Ӧ�����������ڲ������׶εĴ��룬��������һ
// ���汾ʱ���汾��Ϊ 1.0.0.0

// 1. ����Ŀ�ڽ����˾ֲ��޸Ļ� bug ����ʱ�����汾�źʹΰ汾�Ŷ����䣬�����汾�ż� 1��
//    ����汾�Ÿ�λΪ -1��==>��Ҫ ##��Ϊ�޸�##�� �ű����Զ�������汾��+1��Ϊ0��
// 2. ����Ŀ��ԭ�еĻ����������˲��ֹ���ʱ�����汾�Ų��䣬�ΰ汾�ż� 1�������汾�Ÿ�
//    λΪ0������汾�Ÿ�λΪ -1��==>��Ҫ ##��Ϊ�޸�##���ű����Զ�������汾��+1��Ϊ0��
// 3. ����Ŀ�ڽ������ش��޸Ļ�ֲ������ۻ��϶࣬��������Ŀ���巢��ȫ�ֱ仯ʱ�����汾
//    �ż� 1���ΰ汾�š������汾�Ÿ�λΪ0������汾�Ÿ�λΪ -1��==>��Ҫ ##��Ϊ�޸�##��
//    �ű����Զ�������汾��+1��Ϊ0��
// 4. ����汾�ű������ڱ�������л�  ##�Զ�����## (ÿ�α��� +1)������Ҫ��Ϊ����

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



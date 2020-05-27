#ifndef _LIB_QVAR_3D_HPP_
#define _LIB_QVAR_3D_HPP_

#include "amcomdef.h"
#include "libqvar.hpp"
#include "qvar3DSceneManager.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct _tag_qvar_model_desc {
        MPChar     pNodePath;
        MUInt32     nMusicCount;
        MPChar*    ppMusicPaths;
    }QVARModelDesc;
    
    typedef struct _tag_qvar_scene_desc {
        MUInt32      nModelCount;
        QVARModelDesc* pModelDesc;
    } QVARSceneDesc;
    
    // 创建QVARScene的实例
    MRESULT qvarTemplateCreateScene(MHandle pCtx, MPVoid* ppScene);
    // 销毁QVARScene的实例
    MRESULT qvarTemplateDestoryScene(MHandle pCtx, MPVoid pScene);
    // 加载模版文件内的模型，音频等信息并将音频等绑定到对应的模型上
    MRESULT qvarTemplateLoadScene(MHandle pCtx, MPVoid pScene, QVARSceneDesc* pSceneDesc);
    // 渲染模型
    // pInfo 传入当前相机的投影矩阵
    MRESULT qvarTemplateRender(MHandle pCtx, QVARdrawTargetDesc* pDesc, QVARcameraProjectInfo *pInfo);
    // 获取当前已加载的模型句柄
    MRESULT qvarTemplateGetLoadedModel(MHandle pCtx, MPVoid pScene, MInt32 index, MPVoid* ppModel);
    // 设置当前要添加的模型
    // random设true时，随机添加模版内任意一个模型，设false时，添加模版内所有模型，位置随机
    MRESULT qvarTemplateSetAttachModel(MHandle pCtx, MPVoid pScene, MBool random);
    // 设置当前要操作的模型
    MRESULT qvarTemplateSetSelectedModel(MHandle pCtx, MPVoid pScene, MPVoid pModel);
    // 添加并播放目标模型的音频
    // pModel 为目标模型
    MRESULT qvarTemplateAttachPlayer(MHandle pCtx, MPVoid pModel);
    // 删除音频
    MRESULT qvarTemplateDetachPlayer(MHandle pCtx);
    
    typedef struct _tag_qvar_audio_prop {
        MInt32  mIndex;
        MFloat  mVolume;
        MBool   isLoops;
        MBool   isPositional;
        MBool   isBGM;
    } QVARAudioProp;
    // 初始化模型内包含的音频属性
    MRESULT qvarTemplateInitAudioProperty(MHandle pCtx, MPVoid pModel, QVARAudioProp* pAudioProp);
    
    // 改变当前音频属性
    // volume 音量
    // loop 循环播放
    // positional 3D位置效果
    MRESULT qvarTemplateSetAudioVolume(MHandle pCtx, MPVoid pModel, MInt32 index, MFloat volume);
    MRESULT qvarTemplateSetLoopStatus(MHandle pCtx, MPVoid pModel, MInt32 index, MBool loop);
    MRESULT qvarTemplateSetPositionalStatus(MHandle pCtx, MPVoid pModel, MInt32 index, MBool positional);
    
    // 获取当前音频属性
    // volume 音量
    // loop 循环播放
    // positional 3D位置效果
    MRESULT qvarTemplateGetAudioVolume(MHandle pCtx, MPVoid pModel, MInt32 index, MFloat* volume);
    MRESULT qvarTemplateGetLoopStatus(MHandle pCtx, MPVoid pModel, MInt32 index, MInt32* loop);
    MRESULT qvarTemplateGetPositionalStatus(MHandle pCtx, MPVoid pModel, MInt32 index, MInt32* positional);
    
    MRESULT qvarTemplateGetModelAudioCount(MHandle pCtx, MPVoid pModel, MInt32* pCount);
    MRESULT qvarTemplateSetOperatingParameter(MHandle pCtx, MBool asChild);
    MRESULT qvarGetFirstPlanePosition(MHandle pCtx, MFloat* pMat);
    MRESULT qvarRemoveTemplateScenes(MHandle pCtx, MPVoid pScene);
//    MRESULT qvarModelAttachedStatus(MHandle pCtx, MPVoid pScene, QVARAddModelCompletion completion);
#ifdef __cplusplus
}
#endif

#endif
